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
#include "schemes.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/errorCodes.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/fileio.hpp"
#include "papuga/request.h"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.hpp"
#include "papuga/valueVariant.h"
#include "private/internationalization.hpp"
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdarg>
#include <ctime>

using namespace strus;

#undef STRUS_LOWLEVEL_DEBUG

static std::vector<std::string> getLogArgument( int structDepth, std::size_t nof_arguments, va_list arguments, std::size_t nof_itypes, const papuga_RequestLogItem* itype, papuga_ErrorCode& errcode)
{
	std::vector<std::string> rt( nof_itypes);
	std::size_t nofargs = 0;
	int ai=0, ae=nof_arguments;
	for(; ai < ae; ++ai)
	{
		typedef int inttype;
		typedef const char* charp;

		papuga_RequestLogItem aitype = (papuga_RequestLogItem)va_arg( arguments, inttype);
		int ei=0, ee=nof_itypes;
		for(; ei < ee && itype[ ei] != aitype; ++ei){}
		switch (aitype)
		{
			case papuga_LogItemClassName:
			case papuga_LogItemMethodName:
			case papuga_LogItemMessage:
				if (ei < ee) rt[ei] = va_arg( arguments,charp);
				break;
			case papuga_LogItemResult:
			{
				if (ei < ee)
				{
					papuga_ValueVariant* val = va_arg( arguments, papuga_ValueVariant*);
					if (val->valuetype == papuga_TypeSerialization)
					{
						if (structDepth > 0)
						{
							rt[ei] = std::string("{") + papuga::Serialization_tostring( *val->value.serialization, false, structDepth, errcode) + "}";
						}
						else
						{
							rt[ei] = "{}";
						}
					}
					else if (papuga_ValueVariant_isatomic( val))
					{
						rt[ei] = papuga::ValueVariant_tostring( *val, errcode);
					}
					else
					{
						rt[ei] = std::string("<") + papuga_Type_name( val->valuetype) + ">";
					}
				}
				break;
			}
			case papuga_LogItemArgc:
			{
				nofargs = va_arg( arguments,size_t);
				if (ei < ee)
				{
					std::ostringstream num;
					num << nofargs;
					rt[ei] = num.str();
				}
				break;
			}
			case papuga_LogItemArgv:
			{
				if (ei < ee)
				{
					std::ostringstream argstr;
					papuga_ValueVariant* ar = va_arg( arguments, papuga_ValueVariant*);
					std::size_t ii=0, ie=nofargs;
					for (; ii!=ie; ++ii)
					{
						if (ii) argstr << ", ";
						if (papuga_ValueVariant_isatomic( ar+ii))
						{
							argstr << '\"' << papuga::ValueVariant_tostring( ar[ii], errcode) << '\"';
						}
						else if (ar[ii].valuetype == papuga_TypeSerialization)
						{
							if (structDepth > 0)
							{
								argstr << "{" << papuga::Serialization_tostring( *ar[ii].value.serialization, false, structDepth, errcode) << "}";
							}
							else
							{
								argstr << "{}";
							}
						}
						else
						{
							argstr << "<" << papuga_Type_name( ar[ii].valuetype) << ">";
						}
					}
					rt[ei] = argstr.str();
				}
				break;
			}
		}
	}
	return rt;
}

static void logMethodCall( void* self_, int nofItems, ...)
{
	WebRequestLoggerInterface* self = (WebRequestLoggerInterface*)self_;
	va_list arguments;
	va_start( arguments, nofItems );

	enum {nof_itypes=5};
	static const papuga_RequestLogItem itypes[nof_itypes] = {
		papuga_LogItemClassName,
		papuga_LogItemMethodName,
		papuga_LogItemArgv,
		papuga_LogItemResult
	};
	try
	{
		papuga_ErrorCode errcode = papuga_Ok;
		std::vector<std::string> args = getLogArgument( self->structDepth(), nofItems, arguments, nof_itypes, itypes, errcode);
		if (errcode == papuga_Ok)
		{
			self->logMethodCall( args[0], args[1], args[2], args[3]);
		}
		else
		{
			self->logLoggerError( papuga_ErrorCode_tostring( errcode));
		}
	}
	catch (const std::bad_alloc&)
	{
		self->logLoggerError( papuga_ErrorCode_tostring( papuga_NoMemError));
	}
	catch (const std::runtime_error& err)
	{
		self->logLoggerError( err.what());
	}
	catch (...)
	{
		self->logLoggerError( papuga_ErrorCode_tostring( papuga_UncaughtException));
	}
	va_end( arguments);
}

std::pair<const char*,const char*> WebRequestHandler::getConfigSourceContext( const char* contextType, const char* contextName)
{
	if (0==std::strcmp( contextType, "context")) return std::pair<const char*,const char*>(NULL,NULL);
	return std::pair<const char*,const char*>("context","context");
}

WebRequestHandler::WebRequestHandler(
		WebRequestLoggerInterface* logger_,
		const std::string& html_head_,
		const std::string& config_store_dir_)
	:m_mutex()
	,m_config_counter(0)
	,m_debug_maxdepth(logger_?logger_->structDepth():0)
	,m_logger(logger_)
	,m_impl(0)
	,m_html_head(html_head_)
	,m_config_store_dir(config_store_dir_)
{
	std::memset( &m_call_logger, 0, sizeof(m_call_logger));
	m_call_logger.self = logger_;
	int mask = logger_->logMask();
	if (!!(mask & (int)WebRequestLoggerInterface::LogMethodCalls)) m_call_logger.logMethodCall = &logMethodCall;
	m_impl = papuga_create_RequestHandler( &m_call_logger);
	if (!m_impl) throw std::bad_alloc();

	using namespace strus::webrequest;
#define DEFINE_SCHEME( CONTEXT_TYPE, SCHEME_NAME, SCHEME_IMPL)\
	static const Scheme ## SCHEME_IMPL  scheme ## SCHEME_IMPL;\
	if (!papuga_RequestHandler_add_scheme( m_impl, CONTEXT_TYPE, SCHEME_NAME, scheme ## SCHEME_IMPL .impl())) throw std::bad_alloc();

	DEFINE_SCHEME( "", "init", CreateContext);
	DEFINE_SCHEME( "context", "newstorage", CreateStorage);
	DEFINE_SCHEME( "context", "delstorage", DestroyStorage);
	DEFINE_SCHEME( "context", "storage", OpenStorage);
	DEFINE_SCHEME( "storage", "queryorig", QueryStorageOriginal); 
	DEFINE_SCHEME( "storage", "queryana", QueryStorageAnalyzed); 
	DEFINE_SCHEME( "storage", "analyzequery", AnalyzeQuery); 
}

WebRequestHandler::~WebRequestHandler()
{
	papuga_destroy_RequestHandler( m_impl);
}

bool WebRequestHandler::hasScheme(
		const char* contextType,
		const char* scheme) const
{
	return papuga_RequestHandler_has_scheme( m_impl, contextType, scheme);
}

static void setStatus( WebRequestAnswer& status, ErrorOperation operation, ErrorCause errcause, const char* errmsg=0)
{
	const char* errstr = errorCauseMessage( errcause);
	int httpstatus = errorCauseToHttpStatus( errcause);
	if (errmsg)
	{
		char errbuf[ 1024];
		if ((int)sizeof(errbuf)-1 >= std::snprintf( errbuf, sizeof(errbuf), "%s, %s", errmsg, errstr))
		{
			errbuf[ sizeof(errbuf)-1] = 0;
		}
		status.setError( httpstatus, *ErrorCode( StrusComponentWebService, operation, errcause), errbuf, true);
	}
	else
	{
		status.setError( httpstatus, *ErrorCode( StrusComponentWebService, operation, errcause), errstr);
	}
}

WebRequestContext* WebRequestHandler::createContext_( const char* accepted_charset, const char* accepted_doctype, WebRequestAnswer& status) const
{
	try
	{
		return new WebRequestContext( this, m_logger, accepted_charset, accepted_doctype);
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorOperationBuildData, ErrorCauseOutOfMem);
	}
	catch (...)
	{
		setStatus( status, ErrorOperationBuildData, ErrorCauseUncaughtException);
	}
	return NULL;
}

WebRequestContextInterface* WebRequestHandler::createContext(
		const char* accepted_charset,
		const char* accepted_doctype,
		WebRequestAnswer& status) const
{
	return createContext_( accepted_charset, accepted_doctype, status);
}

bool WebRequestHandler::loadConfiguration(
			const char* contextType,
			const char* contextName,
			const char* scheme,
			const WebRequestContent& content,
			WebRequestAnswer& status)
{
	try
	{
#ifdef STRUS_LOWLEVEL_DEBUG
		std::string co( webRequestContent_tostring( content));
		if (co.size() > 200) co.resize( 200);
		std::cerr << strus::string_format( "load configuration: context %s %s, scheme %s, doctype %s, encoding %s, content '%s'",
							contextType, contextName, scheme,
							content.doctype(), content.charset(), co.c_str()) << std::endl;
#endif
		std::pair<const char*,const char*> parentContext = getConfigSourceContext( contextType, contextName);
		const char* parentContextType = parentContext.first;
		const char* parentContextName = parentContext.second;

		strus::local_ptr<WebRequestContext> ctx( createContext_( "UTF-8"/*accepted_charset*/, "application/json"/*accepted_doctype*/, status));
		if (!ctx.get()) return false;
		WebRequestContext* ctxi = ctx.get();

		strus::unique_lock lock( m_mutex);
		if (ctxi->executeContent( parentContextType, parentContextName, scheme, content, status))
		{
			papuga_RequestContext* ctximpl = ctx->impl();
			papuga_ErrorCode errcode = papuga_Ok;

			if (!papuga_RequestHandler_add_context( m_impl, contextType, contextName, ctximpl, &errcode))
			{
				char buf[ 1024];
				std::snprintf( buf, sizeof(buf), _TXT("error adding web request context %s '%s' to handler"), contextType, contextName);
				setStatus( status, ErrorOperationBuildData, papugaErrorToErrorCause( errcode), buf);
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	catch (...)
	{
		setStatus( status, ErrorOperationConfiguration, ErrorCauseUncaughtException);
		return false;
	}
}

bool WebRequestHandler::storeConfiguration(
		const char* contextType,
		const char* contextName,
		const char* scheme,
		const WebRequestContent& content,
		WebRequestAnswer& status) const
{
	try
	{
		strus::unique_lock lock( m_mutex);
		std::string contentUtf8 = webRequestContent_tostring( content);
		char timebuf[ 256];
		char idxbuf[ 32];
		time_t timer;
		struct tm* tm_info;
		
		time(&timer);
		tm_info = localtime(&timer);

		std::strftime( timebuf, sizeof(timebuf), "%Y%m%d_%H%M%S", tm_info);
		std::snprintf( idxbuf, sizeof(idxbuf), "%03d", ++m_config_counter);
		WebRequestContent::Type doctype = strus::webRequestContentFromTypeName( content.doctype());
		const char* doctypeName = WebRequestContent::typeName( doctype);

		std::string filename = strus::string_format( "%s_%s.%s.%s.%s.%s.conf", timebuf, idxbuf, contextType, contextName, scheme, doctypeName);
		std::string filepath = strus::joinFilePath( m_config_store_dir, filename);
		int ec = strus::createDir( filepath, false);
		if (ec)
		{
			setStatus( status, ErrorOperationWriteFile, ErrorCause(ec));
			return false;
		}
		ec = strus::writeFile( filepath, contentUtf8);
		if (ec)
		{
			setStatus( status, ErrorOperationWriteFile, ErrorCause(ec));
			return false;
		}
		return true;
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorOperationConfiguration, ErrorCauseOutOfMem);
		return false;
	}
}

static std::string getConfigFilenamePart( const std::string& filename, int pi)
{
	char const* si = filename.c_str();
	while (pi--)
	{
		si = std::strchr( si, '.');
		if (!si) return std::string();
		++si;
	}
	const char* se = std::strchr( si, '.');
	if (!se) se = std::strchr( si, '\0');
	return std::string( si, se-si);
}

bool WebRequestHandler::loadStoredConfigurations(
		WebRequestAnswer& status)
{
	try
	{
		std::vector<std::string> configFileNames;
		int ec = strus::readDirFiles( m_config_store_dir, ".conf", configFileNames);
		if (ec)
		{
			setStatus( status, ErrorOperationReadFile, (ErrorCause)ec);
			return false;
		}
		std::sort( configFileNames.begin(), configFileNames.end());
		std::vector<std::string>::const_iterator ci = configFileNames.begin(), ce = configFileNames.end();
		for (; ci != ce; ++ci)
		{
			std::string doctype = getConfigFilenamePart( *ci, 4);
			if (doctype.empty()) continue;
			std::string scheme = getConfigFilenamePart( *ci, 3);
			std::string contextType = getConfigFilenamePart( *ci, 1);
			std::string contextName = getConfigFilenamePart( *ci, 2);
			std::string date = getConfigFilenamePart( *ci, 0);
			std::string contentstr;
			std::string filepath = strus::joinFilePath( m_config_store_dir, *ci);
			ec = strus::readFile( filepath, contentstr);
			if (ec)
			{
				setStatus( status, ErrorOperationReadFile, ErrorCause(ec));
				return false;
			}
			WebRequestContent content( "UTF-8", doctype.c_str(), contentstr.c_str(), contentstr.size());
			if (!loadConfiguration( contextType.c_str(), contextName.c_str(), scheme.c_str(), content, status))
			{
				char msgbuf[ 1024];
				std::snprintf( msgbuf, sizeof(msgbuf), _TXT("error loading configuration file %s"), ci->c_str());
				msgbuf[ sizeof(msgbuf)-1] = 0;
				status.explain( msgbuf);
				return false;
			}
		}
		return true;
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorOperationConfiguration, ErrorCauseOutOfMem);
		return false;
	}
}


