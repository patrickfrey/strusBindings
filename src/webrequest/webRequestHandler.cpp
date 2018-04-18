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
#include "strus/bindingClasses.h"
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
#include "papuga/constants.h"
#include "private/internationalization.hpp"
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdarg>
#include <ctime>

using namespace strus;

#define ROOT_CONTEXT_NAME "context"

struct MethodDescription
{
	enum {MaxNofArgs=8};

	const char* request_method;
	papuga_RequestMethodDescription data;
	int args[ MaxNofArgs-1];

	MethodDescription(
		const char* request_method_,
		const papuga_RequestMethodId& id,
		int httpstatus_success, 
		const char* resulttype,
		const char* result_rootelem,
		const char* result_listelem,
		bool has_content,
		int nofParams,
		...)
	{
		va_list params;
		va_start( params, nofParams);

		if (nofParams >= MaxNofArgs) throw std::bad_alloc();
		int ai = 0, ae = nofParams;
		for (; ai != ae; ++ai)
		{
			args[ai] = (int)va_arg( params, int);
		}
		args[ai] = 0;
		request_method = request_method_;
		std::memcpy( &data.id, &id, sizeof( data.id));
		data.paramtypes = args;
		data.has_content = has_content;
		data.httpstatus_success = httpstatus_success;
		data.resulttype = resulttype;
		data.result_rootelem = result_rootelem;
		data.result_listelem = result_listelem;
		va_end( params);
	}

	void addToHandler( papuga_RequestHandler* handler) const
	{
		if (!papuga_RequestHandler_add_method( handler, request_method, &data)) throw std::bad_alloc();
	}
};

struct IntrospectionMethodDescription
	:public MethodDescription
{
	IntrospectionMethodDescription( const papuga_RequestMethodId& id, const char* rootelem)
		:MethodDescription( "GET", id, 200, NULL, rootelem, "value", false/*has content*/, 1, WebRequestHandler::ParamPathArray){}
};
struct TransformationMethodDescription
	:public MethodDescription
{
	TransformationMethodDescription( const papuga_RequestMethodId& id, const char* rootelem)
		:MethodDescription( "GET", id, 200, NULL, rootelem, "value", true/*has content*/, 1, WebRequestHandler::ParamContent){}
};
struct PostTransactionMethodDescription
	:public MethodDescription
{
	PostTransactionMethodDescription( const papuga_RequestMethodId& id, const char* rootelem)
		:MethodDescription( "POST/transaction", id, 201, NULL, rootelem, "id", false/*has content*/, 0){}
};

template <class SCHEME>
class DefineScheme
	:public SCHEME
{
public:
	explicit DefineScheme( const char* contextType)
		:m_contextType(contextType){}

	void addToHandler( papuga_RequestHandler* handler, std::set<std::string>& context_typenames, const char* schemeName) const
	{
		if (!papuga_RequestHandler_add_scheme( handler, m_contextType, schemeName, papuga::RequestAutomaton::impl())) throw std::bad_alloc();\
		context_typenames.insert( m_contextType);
	}

private:
	const char* m_contextType;
};

WebRequestHandler::WebRequestHandler(
		WebRequestLoggerInterface* logger_,
		const std::string& html_head_,
		const std::string& config_store_dir_,
		const std::string& configstr_,
		int maxIdleTime,
		int nofTransactionsPerSeconds)
	:m_mutex()
	,m_config_counter(0)
	,m_debug_maxdepth(logger_?logger_->structDepth():0)
	,m_logger(logger_)
	,m_impl(0)
	,m_html_head(html_head_)
	,m_config_store_dir(config_store_dir_)
	,m_transactionPool( ::time(NULL), maxIdleTime*2, nofTransactionsPerSeconds, logger_)
	,m_maxIdleTime(maxIdleTime)
	,m_ticker(this,std::max( maxIdleTime/20, 10))
{
	m_impl = papuga_create_RequestHandler( strus_getBindingsClassDefs());
	if (!m_impl) throw std::bad_alloc();

	using namespace strus::webrequest;

	try
	{
		namespace mt = strus::bindings::method;

		// [1] Add schemes
		static const DefineScheme<Scheme_INIT_Context> scheme_INIT_Context( ""/*type*/);
		scheme_INIT_Context.addToHandler( m_impl, m_context_typenames, ROOT_CONTEXT_NAME/*scheme name*/);

		static const DefineScheme<Scheme_Context_INIT_Storage> scheme_Context_INIT_Storage( ROOT_CONTEXT_NAME/*type*/);
		scheme_Context_INIT_Storage.addToHandler( m_impl, m_context_typenames, "storage");
		static const DefineScheme<Scheme_Context_PUT_Storage> scheme_Context_PUT_Storage( ROOT_CONTEXT_NAME/*type*/);
		scheme_Context_PUT_Storage.addToHandler( m_impl, m_context_typenames, "PUT/storage");
		static const DefineScheme<Scheme_Context_DELETE_Storage> scheme_Context_DELETE_Storage( ROOT_CONTEXT_NAME/*type*/);
		scheme_Context_DELETE_Storage.addToHandler( m_impl, m_context_typenames, "DELETE/storage");

		static const DefineScheme<Scheme_Context_PUT_DocumentAnalyzer> scheme_Context_PUT_DocumentAnalyzer( ROOT_CONTEXT_NAME/*type*/);
		scheme_Context_PUT_DocumentAnalyzer.addToHandler( m_impl, m_context_typenames, "docana");
		scheme_Context_PUT_DocumentAnalyzer.addToHandler( m_impl, m_context_typenames, "PUT/docana");

		// [2] Add methods
		static const IntrospectionMethodDescription mt_Context_GET( mt::Context::introspection(), "config");
		mt_Context_GET.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_StorageClient_GET( mt::Context::introspection(), "storage");
		mt_StorageClient_GET.addToHandler( m_impl);
		
		static const PostTransactionMethodDescription mt_StorageClient_POST_transaction( mt::StorageClient::createTransaction(), "transaction");
		mt_StorageClient_POST_transaction.addToHandler( m_impl);

		static const PostTransactionMethodDescription mt_Inserter_POST_transaction( mt::Inserter::createTransaction(), "transaction");
		mt_StorageClient_POST_transaction.addToHandler( m_impl);

		static const TransformationMethodDescription mt_DocumentAnalyzer_GET( mt::DocumentAnalyzer::analyzeMultiPart(), "doc");
		mt_DocumentAnalyzer_GET.addToHandler( m_impl);

		loadConfiguration( configstr_);
		loadStoredConfigurations();
		if (!m_ticker.start()) throw std::bad_alloc();
	}
	catch (const std::bad_alloc&)
	{
		clear();
		throw std::bad_alloc();
	}
	catch (const std::runtime_error& err)
	{
		clear();
		throw err;
	}
}

void WebRequestHandler::clear()
{
	if (m_impl) {papuga_destroy_RequestHandler( m_impl); m_impl=0;}
}
	

WebRequestHandler::~WebRequestHandler()
{
	m_ticker.stop();
	clear();
}

static void setStatus( WebRequestAnswer& status, ErrorCode errcode, const char* errmsg=0)
{
	const char* errstr = errorCodeToString( errcode);
	int httpstatus = errorCodeToHttpStatus( errcode);
	if (errmsg)
	{
		char errbuf[ 1024];
		if ((int)sizeof(errbuf)-1 >= std::snprintf( errbuf, sizeof(errbuf), "%s, %s", errmsg, errstr))
		{
			errbuf[ sizeof(errbuf)-1] = 0;
		}
		status.setError( httpstatus, errcode, errbuf, true);
	}
	else
	{
		status.setError( httpstatus, errcode, errstr);
	}
}

WebRequestContextInterface* WebRequestHandler::createContext(
		const char* accepted_charset,
		const char* accepted_doctype,
		const char* html_base_href,
		WebRequestAnswer& status)
{
	return createContext_( accepted_charset, accepted_doctype, html_base_href, status);
}

void WebRequestHandler::tick()
{
	m_transactionPool.collectGarbage( ::time(NULL));
}

WebRequestContext* WebRequestHandler::createContext_(
			const char* accepted_charset,
			const char* accepted_doctype,
			const char* html_base_href,
			WebRequestAnswer& status)
{
	try
	{
		return new WebRequestContext( this, m_logger, accepted_charset, accepted_doctype, html_base_href);
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorCodeOutOfMem);
	}
	catch (...)
	{
		setStatus( status, ErrorCodeUncaughtException);
	}
	return NULL;
}

static WebRequestHandler::SubConfig createSubConfig( const std::string& name, papuga_Allocator& allocator, papuga_SerializationIter& itr)
{
	papuga_ErrorCode errcode = papuga_Ok;
	std::string subconfidid;
	papuga_Serialization ser;
	papuga_init_Serialization( &ser, &allocator);
	int taglevel = 0;

	if (!papuga_Serialization_pushName_string( &ser, name.c_str(), name.size())) throw std::bad_alloc();
	if (papuga_SerializationIter_tag( &itr) == papuga_TagOpen)
	{
		++taglevel;
		if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
		papuga_SerializationIter_skip( &itr); 

		for (; taglevel > 0 && !papuga_SerializationIter_eof( &itr); papuga_SerializationIter_skip( &itr))
		{
			switch (papuga_SerializationIter_tag( &itr))
			{
				case papuga_TagValue:
					if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
					break;
				case papuga_TagOpen:
					if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
					++taglevel;
					break;
				case papuga_TagClose:
					if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
					--taglevel;
					break;
				case papuga_TagName:
					if (taglevel == 1)
					{
						char nambuf[ 128];
						const char* nam = papuga_ValueVariant_toascii( nambuf, sizeof(nambuf), papuga_SerializationIter_value( &itr));
						if (nam && 0==std::strcmp( nam, "id"))
						{
							papuga_SerializationIter_skip( &itr);
							if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
	
							if (papuga_SerializationIter_tag( &itr) == papuga_TagValue)
							{
								std::size_t idlen;
								const char* idval = papuga_ValueVariant_tostring( papuga_SerializationIter_value( &itr), &allocator, &idlen, &errcode);
								if (!idval) throw std::bad_alloc();
								subconfidid = std::string( idval, idlen);
							}
						}
						else
						{
							if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
						}
					}
					else
					{
						if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
					}
			}
		}
	}
	else if (papuga_SerializationIter_tag( &itr) == papuga_TagValue)
	{
		if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
	}
	papuga_ValueVariant subconfigval;
	papuga_init_ValueVariant_serialization( &subconfigval, &ser);
	std::size_t subcfglen;
	const char* subcfgstr = (const char*)papuga_ValueVariant_tojson(
					&subconfigval, &allocator, getBindingsInterfaceDescription()->structs,
					papuga_UTF8, NULL, &subcfglen, &errcode);
	if (!subcfgstr) throw std::bad_alloc();
	return WebRequestHandler::SubConfig( name, subconfidid.empty() ? name : subconfidid,  std::string(subcfgstr, subcfglen));
}

bool WebRequestHandler::isSubConfigSection( const std::string& name) const
{
	return (name != ROOT_CONTEXT_NAME && m_context_typenames.find( name) != m_context_typenames.end());
}

std::vector<WebRequestHandler::SubConfig> WebRequestHandler::getSubConfigList( const std::string& content) const
{
	std::vector<SubConfig> rt;
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_Allocator allocator;
	char allocator_mem[ 4096];
	papuga_init_Allocator( &allocator, allocator_mem, sizeof(allocator_mem));

	try
	{
		papuga_ValueVariant configstruct;
		if (!papuga_init_ValueVariant_json( &configstruct, &allocator, papuga_UTF8, content.c_str(), content.size(), &errcode)) goto EXIT;
		if (configstruct.valuetype != papuga_TypeSerialization) goto EXIT;
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, configstruct.value.serialization);
		int taglevel = 0;

		while (!papuga_SerializationIter_eof( &seriter))
		{
			switch (papuga_SerializationIter_tag( &seriter))
			{
				case papuga_TagValue:
					break;
				case papuga_TagOpen:
					++taglevel;
					break;
				case papuga_TagClose:
					--taglevel;
					break;
				case papuga_TagName:
					if (taglevel == 0)
					{
						char nambuf[ 128];
						const char* nam = papuga_ValueVariant_toascii( nambuf, sizeof(nambuf), papuga_SerializationIter_value( &seriter));
						if (nam && isSubConfigSection( nam))
						{
							papuga_SerializationIter_skip( &seriter);
							rt.push_back( createSubConfig( nam, allocator, seriter));
							continue;
						}
					}
					break;
			}
			papuga_SerializationIter_skip( &seriter);
		}
	}
	catch (const std::bad_alloc&)
	{
		errcode = papuga_NoMemError;
	}

EXIT:
	papuga_destroy_Allocator( &allocator);
	if (errcode != papuga_Ok) throw papuga::error_exception( errcode, _TXT("parse sub configurations"));
	return rt;
}

void WebRequestHandler::loadConfiguration( const std::string& configstr)
{
	const char* config_doctype = "json";
	const char* config_charset = "utf-8";

	loadInitConfiguration( configstr);

	std::vector<SubConfig> cfglist = getSubConfigList( configstr);
	std::vector<SubConfig>::const_iterator ci = cfglist.begin(), ce = cfglist.end();
	for (; ci != ce; ++ci)
	{
		WebRequestAnswer status;
		strus::WebRequestContent subcontent( config_charset, config_doctype, ci->content.c_str(), ci->content.size());
		if (!loadConfiguration(
				ci->name.c_str()/*context type*/, ci->id.c_str()/*context name*/,
				false/*do not store for reload*/, subcontent, status))
		{
			throw strus::runtime_error( _TXT("error loading sub configuration %s '%s': %s"), ci->name.c_str()/*context type*/, ci->id.c_str()/*context name*/, status.errorstr());
		}
	}
}

bool WebRequestHandler::deleteConfiguration(
		const char* contextType,
		const char* contextName,
		WebRequestAnswer& status)
{
	try
	{
		bool rt = true;
		char scheme[ 64];
		std::snprintf( scheme, sizeof(scheme)-1, "DELETE/%s", contextType);
		scheme[ sizeof(scheme)-1] = 0;

		bool has_destroy_scheme = !!papuga_RequestHandler_get_scheme( m_impl, contextType, contextName);

		papuga_ErrorCode errcode = papuga_Ok;
		if (papuga_RequestHandler_destroy_context( m_impl, contextType, contextName, &errcode))
		{
			if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
			{
				m_logger->logAction( contextType, contextName, "unlink configured object");
			}
		}
		std::string configBuf;
		WebRequestContent configContent;
		if (has_destroy_scheme)
		{
			if (getStoredConfiguration( contextType, contextName, configBuf, configContent, status))
			{
				if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
				{
					m_logger->logAction( contextType, contextName, "get configuration content for delete");
				}
			}
			else
			{
				rt = false;
			}
		}
		if (deleteStoredConfiguration( contextType, contextName, status))
		{
			if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
			{
				m_logger->logAction( contextType, contextName, "delete configuration for load on restart");
			}
		}
		else
		{
			rt = false;
		}
		if (has_destroy_scheme)
		{
			const char* accepted_charset = "UTF-8";
			const char* accepted_doctype = "application/json"; 
			strus::local_ptr<WebRequestContext> ctx( createContext_( accepted_charset, accepted_doctype, ""/*html_base_href*/, status));
			WebRequestContext* ctxi = ctx.get();
			if (!ctxi) return false;
	
			if (ctxi->executeContextScheme( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, scheme, configContent, status))
			{
				if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
				{
					m_logger->logAction( contextType, contextName, "call scheme DELETE");
				}
			}
			else
			{
				rt = false;
			}
		}
		return rt;
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorCodeOutOfMem);
		return false;
	}
	catch (...)
	{
		setStatus( status, ErrorCodeUncaughtException);
		return false;
	}
}

void WebRequestHandler::loadInitConfiguration( const std::string& configstr)
{
	const char* config_doctype = "json";
	const char* config_charset = "utf-8";
	const char* accepted_charset = "UTF-8";
	const char* accepted_doctype = "application/json";
	const char* scheme = ROOT_CONTEXT_NAME;
	const char* contextType = ROOT_CONTEXT_NAME;
	const char* contextName = ROOT_CONTEXT_NAME;

	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogConfiguration))
	{
		m_logger->logPutConfiguration( contextType, contextName, configstr);
	}
	WebRequestAnswer status;
	strus::WebRequestContent content( config_charset, config_doctype, configstr.c_str(), configstr.size());

	ContextNameDef cndef( contextType, contextName);
	strus::local_ptr<WebRequestContext> ctx( createContext_( accepted_charset, accepted_doctype, ""/*html_base_href*/, status));
	WebRequestContext* ctxi = ctx.get();
	if (!ctxi) throw std::runtime_error( status.errorstr() ? status.errorstr() : _TXT("unknown error"));

	if (ctxi->executeInitScheme( scheme, content, status))
	{
		if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
		{
			m_logger->logAction( contextType, contextName, "call init");
		}
	}
	else
	{
		throw std::runtime_error( status.errorstr() ? status.errorstr() : _TXT("unknown error"));
	}
	strus::unique_lock lock( m_mutex);
	m_context_names.erase( cndef);
	papuga_ErrorCode errcode = papuga_Ok;

	if (!papuga_RequestHandler_transfer_context( m_impl, contextType, contextName, ctx->fetchContext(), &errcode))
	{
		char buf[ 1024];
		std::snprintf( buf, sizeof(buf), _TXT("error adding web request context %s '%s' to handler"), contextType, contextName);
		throw std::runtime_error( buf);
	}
	m_context_names.insert( cndef);
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
	{
		m_logger->logAction( contextType, contextName, "initialized");
	}
}

bool WebRequestHandler::loadConfiguration(
			const char* contextType,
			const char* contextName,
			bool storedForReload,
			const WebRequestContent& content,
			WebRequestAnswer& status)
{
	try
	{
		if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogConfiguration))
		{
			std::string co( webRequestContent_tostring( content, 1000));
			m_logger->logPutConfiguration( contextType, contextName, co);
		}
		ContextNameDef cndef( contextType, contextName);
		const char* accepted_charset = "UTF-8";
		const char* accepted_doctype = "application/json"; 
		strus::local_ptr<WebRequestContext> ctx( createContext_( accepted_charset, accepted_doctype, ""/*html_base_href*/, status));
		WebRequestContext* ctxi = ctx.get();
		if (!ctxi) return false;

		char schemebuf[ 64];
		const char* scheme;
		const char* action;
		if (storedForReload)
		{
			std::snprintf( schemebuf, sizeof(schemebuf), "PUT/%s", contextType);
			scheme = schemebuf;
			action = "call PUT";
		}
		else
		{
			scheme = contextType;
			action = "call init";
		}
		if (ctxi->executeContextScheme( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, scheme, content, status))
		{
			if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
			{
				m_logger->logAction( contextType, contextName, action);
			}
		}
		else
		{
			return false;
		}
		ConfigurationTransaction transaction;
		if (storedForReload)
		{
			if (!storeConfiguration( transaction, contextType, contextName, content, status)) return false;
			if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
			{
				m_logger->logAction( contextType, contextName, "store configuration for load on restart");
			}
		}
		strus::unique_lock lock( m_mutex);
		m_context_names.erase( cndef);
		papuga_ErrorCode errcode = papuga_Ok;

		if (!papuga_RequestHandler_transfer_context( m_impl, contextType, contextName, ctx->fetchContext(), &errcode))
		{
			char buf[ 1024];
			std::snprintf( buf, sizeof(buf), _TXT("error adding web request context %s '%s' to handler"), contextType, contextName);
			setStatus( status, papugaErrorToErrorCode( errcode), buf);
			return false;
		}
		m_context_names.insert( cndef);
		if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
		{
			m_logger->logAction( contextType, contextName, "create object");
		}
		if (storedForReload && !commitStoreConfiguration( transaction, status))
		{
			return false;
		}
		if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
		{
			m_logger->logAction( contextType, contextName, "commit configuration");
		}
		return true;
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorCodeOutOfMem);
		return false;
	}
	catch (...)
	{
		setStatus( status, ErrorCodeUncaughtException);
		return false;
	}
}


bool WebRequestHandler::deleteStoredConfiguration(
		const char* contextType,
		const char* contextName,
		WebRequestAnswer& status)
{
	try
	{
		strus::unique_lock lock( m_mutex);
		ContextNameDef cndef( contextType, contextName);
		m_context_names.erase( cndef);

		std::string fileext = strus::string_format( ".%s.%s.conf", contextType, contextName);
		std::vector<std::string> files;
		int ec = strus::readDirFiles( m_config_store_dir, fileext, files);
		if (ec)
		{
			setStatus( status, (ErrorCode)ec);
			return false;
		}
		std::vector<std::string>::const_iterator fi = files.begin(), fe = files.end();
		if (fi == fe) return false;
		for (; fi != fe; ++fi)
		{
			ec = removeFile( *fi, true);
			if (ec)
			{
				setStatus( status, (ErrorCode)ec);
				return false;
			}
		}
		return true;
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorCodeOutOfMem);
		return false;
	}
}

bool WebRequestHandler::storeConfiguration(
		ConfigurationTransaction& transaction,
		const char* contextType,
		const char* contextName,
		const WebRequestContent& content,
		WebRequestAnswer& status) const
{
	try
	{
		strus::unique_lock lock( m_mutex);
		std::string contentUtf8 = webRequestContent_tostring( content, -1);
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

		std::string filename = strus::string_format( "%s_%s.%s.%s.%s.conf", timebuf, idxbuf, doctypeName, contextType, contextName);
		transaction.filename = strus::joinFilePath( m_config_store_dir, filename);
		transaction.failed_filename = transaction.filename + ".failed";
		int ec = strus::createDir( m_config_store_dir, false);
		if (ec)
		{
			setStatus( status, (ErrorCode)ec);
			return false;
		}
		ec = strus::writeFile( transaction.failed_filename, contentUtf8);
		if (ec)
		{
			setStatus( status, (ErrorCode)ec);
			return false;
		}
		return true;
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorCodeOutOfMem);
		return false;
	}
}

bool WebRequestHandler::commitStoreConfiguration( const ConfigurationTransaction& transaction, WebRequestAnswer& status) const
{
	int ec = strus::renameFile( transaction.failed_filename, transaction.filename);
	if (ec)
	{
		setStatus( status, ErrorCodeOutOfMem, _TXT("commit of storing configuration failed"));
		return false;
	}
	return true;
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

struct greater
{
	template<class T>
	bool operator()(T const &a, T const &b) const { return a > b; }
};

bool WebRequestHandler::getStoredConfiguration(
		const char* contextType,
		const char* contextName,
		std::string& buf,
		WebRequestContent& content,
		WebRequestAnswer& status)
{
	std::vector<std::string> configFileNames;
	int ec = strus::readDirFiles( m_config_store_dir, ".conf", configFileNames);
	if (ec) throw strus::runtime_error( ec, _TXT("error loading stored configuration: %s"), std::strerror(ec));

	std::sort( configFileNames.begin(), configFileNames.end(), greater());
	std::vector<std::string>::const_iterator ci = configFileNames.begin(), ce = configFileNames.end();
	for (; ci != ce; ++ci)
	{
		std::string doctype = getConfigFilenamePart( *ci, 1);
		if (doctype.empty()) continue;
		std::string candidateContextType = getConfigFilenamePart( *ci, 2);
		std::string candidateContextName = getConfigFilenamePart( *ci, 3);
		if (candidateContextType != contextType || candidateContextName != contextName) continue;
		std::string date = getConfigFilenamePart( *ci, 0);
		std::string filepath = strus::joinFilePath( m_config_store_dir, *ci);
		ec = strus::readFile( filepath, buf);
		if (ec) throw strus::runtime_error( ec, _TXT("error reading stored configuration file: %s"), std::strerror(ec));
		std::size_t bufsize = buf.size();
		buf.push_back( '\0');
		std::size_t doctypeidx = buf.size();
		buf.append( doctype);
		content.setContent( buf.c_str(), bufsize);
		content.setCharset( "UTF-8");
		content.setDoctype( buf.c_str() + doctypeidx);
		return true;
	}
	return false;
}

bool WebRequestHandler::loadStoredConfigurations()
{
	std::vector<std::string> configFileNames;
	int ec = strus::readDirFiles( m_config_store_dir, ".conf", configFileNames);
	if (ec) throw strus::runtime_error( ec, _TXT("error loading stored configuration: %s"), std::strerror(ec));

	std::sort( configFileNames.begin(), configFileNames.end());
	std::vector<std::string>::const_iterator ci = configFileNames.begin(), ce = configFileNames.end();
	for (; ci != ce; ++ci)
	{
		std::string doctype = getConfigFilenamePart( *ci, 1);
		if (doctype.empty()) continue;
		std::string contextType = getConfigFilenamePart( *ci, 2);
		std::string contextName = getConfigFilenamePart( *ci, 3);
		std::string date = getConfigFilenamePart( *ci, 0);
		std::string contentstr;
		std::string filepath = strus::joinFilePath( m_config_store_dir, *ci);
		ec = strus::readFile( filepath, contentstr);
		if (ec) throw strus::runtime_error( ec, _TXT("error reading stored configuration file: %s"), std::strerror(ec));

		WebRequestContent content( "UTF-8", doctype.c_str(), contentstr.c_str(), contentstr.size());
		WebRequestAnswer status;
		if (!loadConfiguration( contextType.c_str(), contextName.c_str(), false/*do not store for reload*/, content, status))
		{
			if (status.apperror())
			{
				throw strus::runtime_error( status.apperror(), _TXT("error loading stored configuration file %s: %s"), ci->c_str(), status.errorstr() ? status.errorstr() : _TXT("unknown error"));
			}
			else
			{
				throw strus::runtime_error( _TXT("error loading stored configuration file %s: %s"), ci->c_str(), status.errorstr() ? status.errorstr() : _TXT("unknown error"));
			}
		}
	}
	return true;
}

std::vector<std::string> WebRequestHandler::contextNames( const std::string& name) const
{
	std::vector<std::string> rt;
	ContextNameDef cndef( name, std::string());
	strus::unique_lock lock( m_mutex);
	std::set<ContextNameDef>::const_iterator ci = m_context_names.upper_bound( cndef);
	for (; ci != m_context_names.end() && ci->first == name; ++ci)
	{
		rt.push_back( ci->second);
	}
	return rt;
}

std::vector<std::string> WebRequestHandler::contextTypes() const
{
	std::set<std::string> res;
	strus::unique_lock lock( m_mutex);
	std::set<ContextNameDef>::const_iterator ci = m_context_names.begin();
	for (; ci != m_context_names.end(); ++ci)
	{
		res.insert( ci->first);
	}
	return std::vector<std::string>( res.begin(), res.end());
}

std::string WebRequestHandler::postTransaction(
		papuga_RequestContext* context)
{
	return m_transactionPool.createTransaction( context, m_maxIdleTime);
}

TransactionRef WebRequestHandler::fetchTransaction( const std::string& tid)
{
	return m_transactionPool.fetchTransaction( tid);
}

void WebRequestHandler::returnTransaction( const TransactionRef& tr)
{
	m_transactionPool.returnTransaction( tr);
}

void WebRequestHandler::releaseTransaction( const std::string& tid)
{
	(void)m_transactionPool.fetchTransaction( tid);
}



