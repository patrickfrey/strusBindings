/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestHandler.cpp"
#include "webRequestHandler.hpp"
#include "webRequestContext.hpp"
#include "webRequestUtils.hpp"
#include "strus/lib/bindings_description.hpp"
#include "strus/lib/error.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/errorCodes.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/shared_ptr.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/sleep.hpp"
#include "papuga/requestContext.h"
#include "papuga/luaRequestHandler.h"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.hpp"
#include "papuga/valueVariant.h"
#include "papuga/constants.h"
#include "private/internationalization.hpp"
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdarg>
#include <ctime>

using namespace strus;

static void tickerFunction( void* THIS)
{
	((WebRequestHandler*)THIS)->tick();
}

static std::pair<papuga_SchemaList*,papuga_SchemaMap*> loadSchemas( papuga_Allocator* allocator, const std::string& schema_dir);
static PagugaLuaRequestHandlerScriptMap loadScripts( papuga_Allocator* allocator, const std::string& script_dir);

WebRequestHandler::WebRequestHandler(
		WebRequestEventLoopInterface* eventLoop_,
		WebRequestLoggerInterface* logger_,
		const std::string& html_head_,
		const std::string& config_dir_,
		const std::string& script_dir_,
		const std::string& schema_dir_,
		const std::string& service_name_,
		int port_,
		bool beautifiedOutput_,
		int maxIdleTime_,
		int nofTransactionsPerSeconds)
	:m_debug_maxdepth(logger_?logger_->structDepth():0)
	,m_logger(logger_)
	,m_contextPool(nullptr)
	,m_schemaList(nullptr)
	,m_schemaMap(nullptr)
	,m_scriptMap()
	,m_html_head(html_head_)
	,m_config_dir(config_dir_)
	,m_script_dir(script_dir_)
	,m_schema_dir(schema_dir_)
	,m_serviceName(service_name_)
	,m_transactionPool( eventLoop_->time(), maxIdleTime_*2, nofTransactionsPerSeconds, logger_)
	,m_port((port_==80||port_==0) ? std::string() : strus::string_format("%d",port_))
	,m_maxIdleTime(maxIdleTime_)
	,m_beautifiedOutput(beautifiedOutput_)
	,m_eventLoop( eventLoop_)
{
	m_contextPool = papuga_create_RequestContextPool();
	if (!m_contextPool || !m_eventLoop->addTickerEvent( this, &tickerFunction)) throw std::bad_alloc();

	try
	{
		papuga_init_Allocator( &m_allocator, nullptr, 0);
		auto scma = loadSchemas( &m_allocator, m_schema_dir);
		m_schemaList = scma.first;
		m_schemaMap = scma.second;
		m_scriptMap = loadScripts( &m_allocator, m_script_dir);
	}
	catch (const std::bad_alloc&)
	{
		papuga_destroy_RequestContextPool( m_contextPool);
		throw std::bad_alloc();
	}
	catch (const std::runtime_error& err)
	{
		papuga_destroy_RequestContextPool( m_contextPool);
		throw err;
	}
}

WebRequestHandler::~WebRequestHandler()
{
	papuga_destroy_RequestContextPool( m_contextPool);
}

static size_t countLines( const std::string& source)
{
	size_t rt = 0;
	auto si = source.begin(), se = source.end();
	for (; si != se; ++si)
	{
		if (*si == '\n')
		{
			++rt;
		}
	}
	return rt;
}

static std::pair<papuga_SchemaList*,papuga_SchemaMap*> loadSchemas( papuga_Allocator* allocator, const std::string& schema_dir)
{
	std::pair<papuga_SchemaList*,papuga_SchemaMap*> rt;
	std::vector<std::string> files;
	int ec = strus::readDirFiles( schema_dir, ".psm", files);
	if (ec) throw strus::runtime_error( "Error (%d) reading schema files: %s", ec, ::strerror( ec));

	std::vector<std::string>::const_iterator fi = files.begin(), fe = files.end();
	std::vector<size_t> startPositions;
	size_t startPosition = 0;
	std::string source;
	char errstrbuf[ 1024];
	papuga_SchemaError errbuf;

	for (; fi != fe; ++fi)
	{
		std::string fcontent;
		const std::string schema_fname = strus::joinFilePath( schema_dir, *fi);
		if (schema_fname.empty()) throw std::bad_alloc();
		ec = strus::readFile( schema_fname, fcontent);
		if (ec) throw strus::runtime_error( "Error (%d) reading schema file: %s", ec, ::strerror( ec));

		source.append( fcontent);
		source.push_back( '\n');
		size_t flines = countLines( fcontent);
		startPosition += flines;
		startPositions.push_back( startPosition + 1);
	}
	papuga_init_SchemaError( &errbuf);

	rt.first = papuga_create_SchemaList( source.c_str(), &errbuf);
	rt.second = papuga_create_SchemaMap( source.c_str(), &errbuf);
	if (!rt.first || !rt.second)
	{
		if (errbuf.line)
		{
			size_t fidx = 0;
			for (; fidx < files.size() && startPositions[ fidx] < (size_t)errbuf.line; ++fidx){}
			if (fidx < files.size())
			{
				size_t fileline = fidx == 0
						? errbuf.line
						: (errbuf.line - startPositions[ fidx-1]);
				std::string schemaName( files[fidx].c_str(), files[fidx].size()-4);
				std::snprintf( errstrbuf, sizeof(errstrbuf),
						"Error in schema '%s' at line %d: %s", schemaName.c_str(),
						(int)fileline, papuga_ErrorCode_tostring( errbuf.code));
			}
			else
			{
				std::snprintf( errstrbuf, sizeof(errstrbuf),
						"Failed to load schemas from '%s': %s", schema_dir.c_str(),
						papuga_ErrorCode_tostring( errbuf.code));
			}
		}
		else
		{
			std::snprintf( errstrbuf, sizeof(errstrbuf),
					"Failed to load schemas from '%s': %s", schema_dir.c_str(),
					papuga_ErrorCode_tostring( errbuf.code));
		}
		throw std::runtime_error( errstrbuf);
	}
	return rt;
}

static PagugaLuaRequestHandlerScriptMap loadScripts( papuga_Allocator* allocator, const std::string& script_dir)
{
	PagugaLuaRequestHandlerScriptMap rt;
	std::vector<std::string> files;
	int ec = strus::readDirFiles( script_dir, ".lua", files);
	if (ec) throw strus::runtime_error( "Error (%d) reading script files: %s", ec, ::strerror( ec));

	std::vector<std::string>::const_iterator fi = files.begin(), fe = files.end();
	char errstrbuf[ 2048];
	papuga_ErrorBuffer errbuf;
	papuga_init_ErrorBuffer( &errbuf, errstrbuf, sizeof(errstrbuf));

	for (; fi != fe; ++fi)
	{
		std::string scriptName;
		ec = strus::getFileName( *fi, scriptName, false/*not with extension*/);
		if (ec) throw strus::runtime_error( "Error (%d) reading script file name: %s", ec, ::strerror( ec));
		std::string scriptSrc;
		const std::string scriptPath = strus::joinFilePath( script_dir, *fi);
		if (scriptPath.empty()) throw std::bad_alloc();
		ec = strus::readFile( scriptPath, scriptSrc);
		if (ec) throw strus::runtime_error( "Error (%d) reading script file: %s", ec, ::strerror( ec));

		papuga_LuaRequestHandlerScript* script
			= papuga_create_LuaRequestHandlerScript( scriptName.c_str(), scriptSrc.c_str(), &errbuf);
		if (!script)
		{
			throw std::runtime_error( papuga_ErrorBuffer_lastError( &errbuf));
		}
		rt.add( script);
	}
	return rt;
}

static void setAnswer( WebRequestAnswer& answer, ErrorCode errcode, char const* errstr=0, bool doCopy=false)
{
	int httpstatus = errorCodeToHttpStatus( errcode);
	if (errstr)
	{
		answer.setError( httpstatus, errcode, errstr, doCopy);
	}
	else
	{
		answer.setError( httpstatus, errcode, strus::errorCodeToString(errcode));
	}
}

#define WEBREQUEST_HANDLER_CATCH_ERROR_RETURN( answer, errorReturnValue) \
	catch (const std::bad_alloc&) \
	{\
		setAnswer( answer, ErrorCodeOutOfMem);\
		return errorReturnValue;\
	}\
	catch (const std::runtime_error& err)\
	{\
		setAnswer( answer, ErrorCodeRuntimeError, err.what(), true/*do copy*/);\
		return errorReturnValue;\
	}\
	catch (...)\
	{\
		setAnswer( answer, ErrorCodeUncaughtException);\
		return errorReturnValue;\
	}

static papuga_RequestAttributes g_configRequestAttributes = {
	0xffFF/*accepted_encoding_set*/,
	0xffFF/*accepted_doctype_set*/,
	nullptr/*html_head*/,
	nullptr/*html_base_href*/,
	true/*beautifiedOutput*/,
	true/*deterministicOutput*/};

bool WebRequestHandler::init( 
		char const* configsrc,
		size_t configlen,
		WebRequestAnswer& answer)
{
	try
	{
		auto fi = m_scriptMap.find( ROOT_CONTEXT_NAME);
		if (fi != m_scriptMap.end())
		{
			papuga_ErrorCode errcode = papuga_Ok;
			papuga_RequestContext* context = papuga_create_RequestContext();
			papuga_LuaRequestHandler* reqhnd
				= papuga_create_LuaRequestHandler(
					fi->second.get(), m_schemaMap, m_contextPool, context,
					nullptr/*transaction handler*/, &g_configRequestAttributes,
					"PUT", ROOT_CONTEXT_NAME, "", configsrc, configlen,
					&errcode);
			if (!reqhnd)
			{
				setAnswer( answer, papugaErrorToErrorCode( errcode));
				papuga_destroy_RequestContext( context);
				return false;
			}
			papuga_ErrorBuffer errbuf;
			char errbufmem[ 2048];
			papuga_init_ErrorBuffer( &errbuf, errbufmem, sizeof(errbufmem));
			if (!papuga_run_LuaRequestHandler( reqhnd, &errbuf))
			{
				if (papuga_ErrorBuffer_hasError( &errbuf))
				{
					char const* msg = papuga_ErrorBuffer_lastError( &errbuf);
					setAnswer( answer, ErrorCodeRuntimeError, msg, true);
				}
				else
				{
					setAnswer( answer, ErrorCodeRuntimeError, _TXT("yield not allowed in configuration"), false);
				}
				papuga_destroy_RequestContext( context);
				papuga_destroy_LuaRequestHandler( reqhnd);
				return false;
			}
			if (0<papuga_LuaRequestHandler_nof_DelegateRequests( reqhnd))
			{
				setAnswer( answer, ErrorCodeRuntimeError, _TXT("no delegate requests allowed in configuration"), false);
				papuga_destroy_RequestContext( context);
				papuga_destroy_LuaRequestHandler( reqhnd);
				return false;
			}
			if (!papuga_RequestContextPool_transfer_context( m_contextPool, ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, context, &errcode))
			{
				setAnswer( answer, papugaErrorToErrorCode( errcode));
				papuga_destroy_RequestContext( context);
				papuga_destroy_LuaRequestHandler( reqhnd);
				return false;
			}
			papuga_destroy_LuaRequestHandler( reqhnd);
			return true;
		}
		else
		{
			setAnswer( answer, ErrorCodeRuntimeError, _TXT("no initialization script (name 'context') found"), false);
			return false;
		}
	}
	WEBREQUEST_HANDLER_CATCH_ERROR_RETURN( answer, false);
}

WebRequestContextInterface* WebRequestHandler::createContext(
		char const* http_accept,
		char const* html_base_href,
		char const* method,
		char const* path,
		WebRequestAnswer& answer)
{
	try
	{
		return new WebRequestContext( this, m_logger, &m_transactionPool, http_accept, html_base_href, method, path);
	}
	WEBREQUEST_HANDLER_CATCH_ERROR_RETURN( answer, NULL);
}

bool WebRequestHandler::delegateRequest(
		char const* address,
		char const* method,
		char const* contentstr,
		size_t contentlen,
		WebRequestDelegateContextInterface* context)
{
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogDelegateRequests))
	{
		m_logger->logDelegateRequest( address, method, contentstr, contentlen);
	}
	return m_eventLoop->send( address, method, contentstr, contentlen, context);
}

WebRequestAnswer WebRequestHandler::getSimpleRequestAnswer(
		char const* http_accept,
		char const* html_base_href,
		char const* name,
		char const* messagestr,
		size_t messagelen)
{
	WebRequestAnswer rt;
	try
	{
		papuga_RequestAttributes attributes;
		papuga_init_RequestAttributes(
			&attributes, http_accept, html_head(), html_base_href,
			m_beautifiedOutput, false/*deterministicOutput*/);

		papuga_StringEncoding result_encoding = papuga_UTF8;
		papuga_ContentType result_doctype = papuga_http_default_doctype( &attributes);

		if (result_doctype == papuga_ContentType_Unknown)
		{
			rt.setError_fmt( strus::errorCodeToHttpStatus( ErrorCodeNotImplemented), ErrorCodeNotImplemented, _TXT("none of the accept content types implemented: %s"), http_accept);
			return rt;
		}
		(void)strus::mapStringToAnswer( rt, 0/*allocator*/, html_head(), ""/*html href base*/, SYSTEM_MESSAGE_HEADER, name, result_encoding, result_doctype, m_beautifiedOutput, std::string( messagestr, messagelen));
	}
	catch (const std::bad_alloc&)
	{
		setAnswer( rt, ErrorCodeOutOfMem);
	}
	catch (const std::runtime_error& err)
	{
		setAnswer( rt, ErrorCodeRuntimeError, err.what(), true);
	}
	return rt;
}

void WebRequestHandler::tick()
{
	m_transactionPool.collectGarbage( m_eventLoop->time());
}

bool WebRequestHandler::transferContext(
		char const* contextType,
		char const* contextName,
		papuga_RequestContext* context,
		WebRequestAnswer& answer)
{
	papuga_ErrorCode errcode = papuga_Ok;
	strus::unique_lock lock( m_mutex_context_transfer);
	if (!papuga_RequestContextPool_transfer_context( m_contextPool, contextType, contextName, context, &errcode))
	{
		papuga_destroy_RequestContext( context);
		setAnswer( answer, papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}

bool WebRequestHandler::removeContext(
		char const* contextType,
		char const* contextName,
		WebRequestAnswer& answer)
{
	papuga_ErrorCode errcode = papuga_Ok;
	strus::unique_lock lock( m_mutex_context_transfer);
	if (!papuga_RequestContextPool_remove_context( m_contextPool, contextType, contextName, &errcode))
	{
		setAnswer( answer, papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}


