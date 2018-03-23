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

const WebRequestHandler::MethodDescription* WebRequestHandler::getListMethod( int classid)
{
	static const MethodDescription::ParamType patharg[] = {MethodDescription::ParamPathArray,MethodDescription::ParamEnd};
	static const MethodDescription context_method( strus::bindings::method::Context::introspectionDir(), patharg);
	static const MethodDescription storage_method( strus::bindings::method::StorageClient::introspectionDir(), patharg);
	
	switch (classid)
	{
		case STRUS_BINDINGS_CLASSID_Context:		return &context_method;
		case STRUS_BINDINGS_CLASSID_StorageClient:	return &storage_method;
		default:					return NULL;
	}
}

const WebRequestHandler::MethodDescription* WebRequestHandler::getViewMethod( int classid)
{
	static const MethodDescription::ParamType patharg[] = {MethodDescription::ParamPathArray,MethodDescription::ParamEnd};
	static const MethodDescription context_method( strus::bindings::method::Context::introspection(), patharg);
	static const MethodDescription storage_method( strus::bindings::method::StorageClient::introspection(), patharg);
	
	switch (classid)
	{
		case STRUS_BINDINGS_CLASSID_Context:		return &context_method;
		case STRUS_BINDINGS_CLASSID_StorageClient:	return &storage_method;
		default:					return NULL;
	}
}

const WebRequestHandler::MethodDescription* WebRequestHandler::getPostDocumentMethod( int classid)
{
	return NULL;
}

const WebRequestHandler::MethodDescription* WebRequestHandler::getPutDocumentMethod( int classid)
{
	return NULL;
}

const WebRequestHandler::MethodDescription* WebRequestHandler::getDeleteMethod( int classid)
{
	return NULL;
}

const WebRequestHandler::MethodDescription* WebRequestHandler::getPatchMethod( int classid)
{
	return NULL;
}

static void addStringList( char const**& stringlist, int& stringlistsize, const char* elem)
{
	char const** new_stringlist = (char const**)std::realloc( stringlist, (stringlistsize+2) * sizeof(stringlist[0]));
	if (!new_stringlist) throw std::bad_alloc();
	stringlist = new_stringlist;
	stringlist[ stringlistsize++] = elem;
	stringlist[ stringlistsize] = NULL;
}

void WebRequestHandler::addScheme( std::size_t nofschemes, const char* type, const char* name, const papuga_RequestAutomaton* automaton)
{
	if (!papuga_RequestHandler_add_scheme( m_impl, type, name, automaton)) throw std::bad_alloc();
	if (0==std::strcmp(type,"context"))
	{
		addStringList( m_context_schemes, m_nofcontext_schemes, name);
	}
	addStringList( m_schemes, m_nofschemes, name);
}


WebRequestHandler::WebRequestHandler(
		WebRequestLoggerInterface* logger_,
		const std::string& html_head_,
		const std::string& config_store_dir_,
		const std::string& configstr_)
	:m_mutex()
	,m_config_counter(0)
	,m_debug_maxdepth(logger_?logger_->structDepth():0)
	,m_logger(logger_)
	,m_impl(0)
	,m_html_head(html_head_)
	,m_config_store_dir(config_store_dir_)
	,m_schemes(NULL)
	,m_nofschemes(0)
	,m_context_schemes(NULL)
	,m_nofcontext_schemes(0)
{
	std::memset( &m_call_logger, 0, sizeof(m_call_logger));
	m_call_logger.self = logger_;
	int mask = logger_->logMask();
	if (!!(mask & (int)WebRequestLoggerInterface::LogMethodCalls)) m_call_logger.logMethodCall = &logMethodCall;
	m_impl = papuga_create_RequestHandler( &m_call_logger);
	if (!m_impl) throw std::bad_alloc();

	std::size_t nofschemes = 0;
	using namespace strus::webrequest;
#define DEFINE_SCHEME( CONTEXT_TYPE, SCHEME_NAME, SCHEME_IMPL)\
	static const Scheme ## SCHEME_IMPL scheme ## SCHEME_IMPL;\
	addScheme( nofschemes, CONTEXT_TYPE, SCHEME_NAME, scheme ## SCHEME_IMPL .impl());\

	DEFINE_SCHEME( "", "init", CreateContext);
	DEFINE_SCHEME( "context", "newstorage", CreateStorage);
	DEFINE_SCHEME( "context", "delstorage", DestroyStorage);
	DEFINE_SCHEME( "context", "storage", OpenStorage);
	DEFINE_SCHEME( "storage", "queryorig", QueryStorageOriginal); 
	DEFINE_SCHEME( "storage", "queryana", QueryStorageAnalyzed); 
	DEFINE_SCHEME( "storage", "analyzequery", AnalyzeQuery);

	loadConfiguration( configstr_);
	loadStoredConfigurations();
}

WebRequestHandler::~WebRequestHandler()
{
	papuga_destroy_RequestHandler( m_impl);
	std::free( m_schemes);
	std::free( m_context_schemes);
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

WebRequestContext* WebRequestHandler::createContext_( const char* accepted_charset, const char* accepted_doctype, WebRequestAnswer& status) const
{
	try
	{
		return new WebRequestContext( this, m_logger, accepted_charset, accepted_doctype);
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

WebRequestContextInterface* WebRequestHandler::createContext(
		const char* accepted_charset,
		const char* accepted_doctype,
		WebRequestAnswer& status) const
{
	return createContext_( accepted_charset, accepted_doctype, status);
}

struct SubConfig
{
	std::string name;
	std::string id;
	std::string content;

	SubConfig( const SubConfig& o)
		:name(o.name),id(o.id),content(o.content){}
	SubConfig( const std::string& name_, const std::string& id_,  const std::string& content_)
		:name(name_),id(id_),content(content_){}
};

static SubConfig createSubConfig( const std::string& name, papuga_Allocator& allocator, papuga_SerializationIter& itr)
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
			if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
			switch (papuga_SerializationIter_tag( &itr))
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
					}
			}
		}
	}
	else if (papuga_SerializationIter_tag( &itr) == papuga_TagValue)
	{
		if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
	}
#ifdef STRUS_LOWLEVEL_DEBUG
	std::cerr << "sub configuration:" << std::endl << papuga::Serialization_tostring( ser, 1/*linemode*/, 100, errcode) << std::endl;
#endif
	papuga_ValueVariant subconfigval;
	papuga_init_ValueVariant_serialization( &subconfigval, &ser);
	papuga_RequestResult cfgst;

	if (!papuga_init_RequestResult_single( &cfgst, &allocator, NULL, "elem", getBindingsInterfaceDescription()->structs, &subconfigval)) throw std::bad_alloc();
	std::size_t subcfglen;
	const char* subcfgstr = (const char*)papuga_RequestResult_tojson( &cfgst, papuga_UTF8, &subcfglen, &errcode);
	if (!subcfgstr) throw std::bad_alloc();
	return SubConfig( name, subconfidid.empty() ? name : subconfidid,  subcfgstr);
}

static std::vector<SubConfig> getSubConfigList( const char* contentstr, std::size_t contentsize, const char** schemes)
{
	std::vector<SubConfig> rt;
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_Allocator allocator;
	char allocator_mem[ 4096];
	papuga_init_Allocator( &allocator, allocator_mem, sizeof(allocator_mem));

	try
	{
		papuga_ValueVariant configstruct;
		if (!papuga_init_ValueVariant_json( &configstruct, &allocator, papuga_UTF8, contentstr, contentsize, &errcode)) goto EXIT;
		if (configstruct.valuetype != papuga_TypeSerialization) goto EXIT;
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << "configuration:" << std::endl << papuga::Serialization_tostring( *configstruct.value.serialization, 1/*linemode*/, 100, errcode) << std::endl;
#endif
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
						if (nam)
						{
							char const** si = schemes;
							for (; *si && 0!=std::strcmp(*si,nam); ++si){}
							if (*si)
							{
								papuga_SerializationIter_skip( &seriter);
								rt.push_back( createSubConfig( nam, allocator, seriter));
								continue;
							}
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
	static const char* root_context = "context";
	static const char* init_scheme = "init";
	static const char* config_doctype = "json";
	static const char* config_charset = "utf-8";
	WebRequestAnswer status;

	strus::WebRequestContent content( config_charset, config_doctype, configstr.c_str(), configstr.size());

	if (!loadConfiguration( root_context/*destContextType*/, root_context/*destContextName*/, init_scheme, false/*do not store for reload*/, content, status))
	{
		if (status.apperror())
		{
			throw strus::runtime_error( status.apperror(), "%s", status.errorstr() ? status.errorstr() : _TXT("unknown error"));
		}
		else
		{
			throw std::runtime_error( status.errorstr() ? status.errorstr() : _TXT("unknown error"));
		}
	}
	std::vector<SubConfig> cfglist = getSubConfigList( configstr.c_str(), configstr.size(), m_context_schemes);
	std::vector<SubConfig>::const_iterator ci = cfglist.begin(), ce = cfglist.end();
	for (; ci != ce; ++ci)
	{
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << strus::string_format( _TXT("sub configuration %s '%s':\n<config>\n%s\n</config>"), ci->name.c_str(), ci->id.c_str(), ci->content.c_str()) << std::endl;
#endif
		strus::WebRequestContent subcontent( config_charset, config_doctype, ci->content.c_str(), ci->content.size());

		if (!loadConfiguration( ci->name.c_str()/*context type*/, ci->id.c_str()/*context name*/, ci->name.c_str()/*scheme*/, false/*do not store for reload*/, subcontent, status))
		{
			throw strus::runtime_error( _TXT("error loading sub configuration %s '%s': %s"), ci->name.c_str()/*context type*/, ci->id.c_str()/*context name*/, status.errorstr());
		}
	}
}

bool WebRequestHandler::loadConfiguration(
			const char* contextType,
			const char* contextName,
			const char* scheme,
			bool storedForReload,
			const WebRequestContent& content,
			WebRequestAnswer& status)
{
	try
	{
#ifdef STRUS_LOWLEVEL_DEBUG
		std::string co( webRequestContent_tostring( content));
		if (co.size() > 1000) co.resize( 1000);
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

		if (ctxi->executeContextScheme( parentContextType, parentContextName, scheme, content, status))
		{
			ConfigurationTransaction transaction;
			if (storedForReload && !storeConfiguration( transaction, contextType, contextName, scheme, content, status)) return false;

			strus::unique_lock lock( m_mutex);
			papuga_RequestContext* ctximpl = ctx->impl();
			papuga_ErrorCode errcode = papuga_Ok;

			if (!papuga_RequestHandler_add_context( m_impl, contextType, contextName, ctximpl, &errcode))
			{
				char buf[ 1024];
				std::snprintf( buf, sizeof(buf), _TXT("error adding web request context %s '%s' to handler"), contextType, contextName);
				setStatus( status, papugaErrorToErrorCode( errcode), buf);
				return false;
			}
			if (storedForReload && !commitStoreConfiguration( transaction, status))
			{
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
		setStatus( status, ErrorCodeUncaughtException);
		return false;
	}
}

bool WebRequestHandler::storeConfiguration(
		ConfigurationTransaction& transaction,
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

bool WebRequestHandler::loadStoredConfigurations()
{
	std::vector<std::string> configFileNames;
	int ec = strus::readDirFiles( m_config_store_dir, ".conf", configFileNames);
	if (ec) throw strus::runtime_error( ec, _TXT("error loading stored configuration: %s"), std::strerror(ec));

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
		if (ec) throw strus::runtime_error( ec, _TXT("error reading stored configuration file: %s"), std::strerror(ec));

		WebRequestContent content( "UTF-8", doctype.c_str(), contentstr.c_str(), contentstr.size());
		WebRequestAnswer status;
		if (!loadConfiguration( contextType.c_str(), contextName.c_str(), scheme.c_str(), false/*do not store for reload*/, content, status))
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


