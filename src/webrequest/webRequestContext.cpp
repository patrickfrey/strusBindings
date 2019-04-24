/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestContextInterface.hpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/errorCodes.hpp"
#include "strus/lib/error.hpp"
#include "strus/versionStorage.hpp"
#include "strus/versionModule.hpp"
#include "strus/versionRpc.hpp"
#include "strus/versionTrace.hpp"
#include "strus/versionAnalyzer.hpp"
#include "strus/versionBase.hpp"
#include "strus/bindingClasses.h"
#include "strus/bindingObjects.h"
#include "strus/bindingMethodIds.hpp"
#include "strus/base/string_format.hpp"
#include "strus/analyzer/documentClass.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/string_format.hpp"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/requestParser.h"
#include "papuga/requestHandler.h"
#include "papuga/valueVariant.h"
#include "papuga/valueVariant.hpp"
#include "papuga/encoding.h"
#include "papuga/allocator.h"
#include "papuga/constants.h"
#include "papuga/serialization.hpp"
#include "private/internationalization.hpp"
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include <sstream>

#undef STRUS_LOWLEVEL_DEBUG
using namespace strus;

static void logMethodCall( void* self_, int nofItems, ...);

static std::string parentPath( const std::string& url)
{
	std::string rt;
	int ec = strus::getParentPath( url, rt);
	if (ec) throw strus::runtime_error_ec( ErrorCodeRequestResolveError, _TXT("illegal URL"));
	return rt;
}

WebRequestContext::WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		ConfigurationHandler* confighandler_,
		TransactionPool* transactionPool_,
		const char* accepted_charset_,
		const char* accepted_doctype_,
		const char* html_base_href_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_confighandler(confighandler_)
	,m_transactionPool(transactionPool_)
	,m_context(0)
	,m_context_ownership(false)
	,m_request(0)
	,m_encoding(papuga_Binary),m_doctype(papuga_ContentType_Unknown),m_doctypestr(0)
	,m_atm(0)
	,m_result_encoding(papuga_Binary),m_result_doctype(WebRequestContent::Unknown)
	,m_accepted_charset(accepted_charset_),m_accepted_doctype(accepted_doctype_)
	,m_html_base_href(html_base_href_)
{
	if (!m_html_base_href.empty())
	{
		if (m_html_base_href[m_html_base_href.size()-1] == '/' && m_html_base_href[ m_html_base_href.size()-2] == '*')
		{
			m_html_base_href.resize( m_html_base_href.size()-1);
			m_html_base_href = parentPath( m_html_base_href) + "/";
		}
		else if (m_html_base_href[ m_html_base_href.size()-1] == '*')
		{
			m_html_base_href = parentPath( m_html_base_href);
		}
	}
	std::memset( &m_callLogger, 0, sizeof(m_callLogger));
	m_callLogger.self = logger_;
	int mask = logger_->logMask();
	if (!!(mask & (int)WebRequestLoggerInterface::LogMethodCalls)) m_callLogger.logMethodCall = &logMethodCall;

	papuga_init_Allocator( &m_allocator, m_allocator_mem, sizeof(m_allocator_mem));
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));
}

WebRequestContext::~WebRequestContext()
{
	if (m_context_ownership && m_context) papuga_destroy_RequestContext( m_context);
	if (m_request) papuga_destroy_Request( m_request);
	papuga_destroy_Allocator( &m_allocator);
}

static inline bool isEqual( const char* name, const char* oth)
{
	return name[0] == oth[0] && 0==std::strcmp(name,oth);
}

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
			self->logError( papuga_ErrorCode_tostring( errcode));
		}
	}
	catch (const std::bad_alloc&)
	{
		self->logError( papuga_ErrorCode_tostring( papuga_NoMemError));
	}
	catch (const std::runtime_error& err)
	{
		self->logError( err.what());
	}
	catch (...)
	{
		self->logError( papuga_ErrorCode_tostring( papuga_UncaughtException));
	}
	va_end( arguments);
}

static void setAnswer( WebRequestAnswer& answer, int errcode, const char* errstr=0, bool doCopy=false)
{
	int httpstatus = errorCodeToHttpStatus( (ErrorCode)errcode);
	if (errstr)
	{
		answer.setError( httpstatus, errcode, errstr, doCopy);
	}
	else
	{
		answer.setError( httpstatus, errcode, strus::errorCodeToString(errcode));
	}
}

bool WebRequestContext::feedContentRequest( WebRequestAnswer& answer, const WebRequestContent& content)
{
	if (content.len() == 0)
	{
		setAnswer( answer, ErrorCodeIncompleteRequest, _TXT("request content is empty"));
		return false;
	}
	// Evaluate the character set encoding:
	if (content.charset()[0] == '\0')
	{
		setAnswer( answer, ErrorCodeNotImplemented, _TXT("charset field in content type is empty. HTTP 1.1 standard character set ISO-8859-1 not implemented"));
		/// ... according to https://www.w3.org/International/articles/http-charset/index we should use "ISO-8859-1" if not defined, currently not available
		return false;
	}
	m_encoding = strus::getStringEncoding( content.charset(), content.str(), content.len());
	if (m_encoding == papuga_Binary)
	{
		setAnswer( answer, ErrorCodeNotImplemented);
		return false;
	}
	// Evaluate the request content type:
	m_doctype = content.doctype() ? papuga_contentTypeFromName( content.doctype()) : papuga_guess_ContentType( content.str(), content.len());
	if (m_doctype == papuga_ContentType_Unknown)
	{
		setAnswer( answer, ErrorCodeInputFormat);
		return false;
	}
	m_doctypestr = content.doctype();
	if (!setResultContentType( answer, m_encoding, papugaTranslatedContentType( m_doctype)))
	{
		return false;
	}
	// Parse the request:
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_RequestParser* parser = papuga_create_RequestParser( &m_allocator, m_doctype, m_encoding, content.str(), content.len(), &errcode);
	if (!parser)
	{
		setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	if (!papuga_RequestParser_feed_request( parser, m_request, &errcode))
	{
		char buf[ 2048];
		int pos = papuga_RequestParser_get_position( parser, buf, sizeof(buf));
		papuga_ErrorBuffer_reportError( &m_errbuf, _TXT( "error at position %d: %s, feeding request, location: %s"), pos, papuga_ErrorCode_tostring( errcode), buf);
		papuga_destroy_RequestParser( parser);

		setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorBuffer_lastError( &m_errbuf), true);
		return false;
	}
	papuga_destroy_RequestParser( parser);
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogRequests))
	{
		const char* reqstr = papuga_request_content_tostring( &m_allocator, m_doctype, m_encoding, content.str(), content.len(), 0/*scope startpos*/, m_logger->structDepth(), &errcode);
		if (!reqstr)
		{
			m_logger->logError( papuga_ErrorCode_tostring( papuga_NoMemError));
		}
		else
		{
			m_logger->logRequest( reqstr);
		}
	}
	return true;
}

bool WebRequestContext::debugContentRequest( WebRequestAnswer& answer)
{
	papuga_ErrorCode errcode = papuga_Ok;
	m_result_encoding = getResultStringEncoding( m_accepted_charset, m_encoding);
	if (m_result_encoding == papuga_Binary)
	{
		char buf[ 2048];
		std::snprintf( buf, sizeof(buf), _TXT("none of the accept charsets implemented: %s"), m_accepted_charset);
		setAnswer( answer, ErrorCodeNotImplemented, buf);
		return false;
	}
	std::size_t result_length = 0;
	const char* result = papuga_Request_tostring( m_request, &m_allocator, m_result_encoding, m_handler->debug_maxdepth(), &result_length, &errcode);
	if (result)
	{
		WebRequestContent content( papuga_stringEncodingName( m_result_encoding), "text/plain", result, result_length);
		answer.setContent( content);
		return true;
	}
	else
	{
		setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
}

bool WebRequestContext::executeContentRequest( WebRequestAnswer& answer, const WebRequestContent& content)
{
	papuga_ErrorCode errcode = papuga_Ok;
	int errpos = -1;
	if (!papuga_RequestContext_execute_request( m_context, m_request, &m_callLogger, &m_errbuf, &errpos))
	{
		errcode = papuga_HostObjectError;
		char* errmsg = papuga_ErrorBuffer_lastError(&m_errbuf);

		int apperr = 0;
		if (errmsg)
		{
			// Exract last error code and remove error codes from app error message:
			char const* errmsgitr = errmsg;
			apperr = strus::errorCodeFromMessage( errmsgitr);

			// Add position info (scope of error in input) to error message, if available:
			if (errpos >= 0)
			{
				// Evaluate more info about the location of the error, we append the scope of the document to the error message:
				const char* locinfo = papuga_request_content_tostring( &m_allocator, m_doctype, m_encoding, content.str(), content.len(), errpos, 4/*maxdepth*/, &errcode);
				if (locinfo)
				{
					papuga_ErrorBuffer_appendMessage( &m_errbuf, " (error scope: %s)", locinfo);
					errmsg = papuga_ErrorBuffer_lastError(&m_errbuf);
				}
			}
			if (apperr) removeErrorCodesFromMessage( errmsg);
			setAnswer( answer, apperr, errmsg, true);
		}
		else
		{
			setAnswer( answer, papugaErrorToErrorCode( errcode));
		}
		return false;
	}
	return true;
}

bool WebRequestContext::setResultContentType( WebRequestAnswer& answer, papuga_StringEncoding default_encoding, WebRequestContent::Type default_doctype)
{
	m_result_encoding = strus::getResultStringEncoding( m_accepted_charset, default_encoding);
	m_result_doctype = strus::getResultContentType( m_accepted_doctype, default_doctype);
	if (m_result_encoding == papuga_Binary)
	{
		char buf[ 2048];
		std::snprintf( buf, sizeof(buf), _TXT("none of the accept charsets implemented: %s"), m_accepted_charset);
		setAnswer( answer, ErrorCodeNotImplemented, buf);
		return false;
	}
	if (m_result_doctype == WebRequestContent::Unknown)
	{
		char buf[ 2048];
		std::snprintf( buf, sizeof(buf), _TXT("none of the accept content types implemented: %s"), m_accepted_doctype);
		setAnswer( answer, ErrorCodeNotImplemented, buf);
		return false;
	}
	return true;
}

bool WebRequestContext::getContentRequestResult( WebRequestAnswer& answer, const WebRequestContent& content)
{
	papuga_ErrorCode errcode = papuga_Ok;
	const char* resultname = papuga_Request_resultname( m_request);
	const papuga_StructInterfaceDescription* structdefs = papuga_Request_struct_descriptions( m_request);
	char* resultstr = 0;
	std::size_t resultulen = 0;

	if (resultname)
	{
		// Serialize the result:
		papuga_Serialization* resultser = papuga_Allocator_alloc_Serialization( &m_allocator);
		if (!resultser)
		{
			setAnswer( answer, ErrorCodeOutOfMem);
			return false;
		}
		if (papuga_Request_resultmerge( m_request))
		{
			if (!papuga_Serialization_merge_request_result( resultser, m_context, m_request, m_encoding, m_doctype, content.str(), content.len(), &errcode))
			{
				setAnswer( answer, papugaErrorToErrorCode( errcode));
				return false;
			}
		}
		else
		{
			if (!papuga_Serialization_serialize_request_result( resultser, m_context, m_request, &errcode))
			{
				setAnswer( answer, papugaErrorToErrorCode( errcode));
				return false;
			}
		}
		papuga_ValueVariant resultval;
		papuga_init_ValueVariant_serialization( &resultval, resultser);
#ifdef STRUS_LOWLEVEL_DEBUG
		if (!papuga_ValueVariant_isvalid( &resultval))
		{
			std::cerr << "Illegal result" << std::endl;
		}
#endif
		// Map the result:
		switch (m_result_doctype)
		{
			case WebRequestContent::XML:  resultstr = (char*)papuga_ValueVariant_toxml( &resultval, &m_allocator, structdefs, m_result_encoding, resultname, NULL/*no array possible*/, &resultulen, &errcode); break;
			case WebRequestContent::JSON: resultstr = (char*)papuga_ValueVariant_tojson( &resultval, &m_allocator, structdefs, m_result_encoding, resultname, NULL/*no array possible*/, &resultulen, &errcode); break;
			case WebRequestContent::HTML: resultstr = (char*)papuga_ValueVariant_tohtml5( &resultval, &m_allocator, structdefs, m_result_encoding, resultname, NULL/*no array possible*/, m_handler->html_head(), m_html_base_href.c_str(), &resultulen, &errcode); break;
			case WebRequestContent::TEXT: resultstr = (char*)papuga_ValueVariant_totext( &resultval, &m_allocator, structdefs, m_result_encoding, resultname, NULL/*no array possible*/, &resultulen, &errcode); break;
			case WebRequestContent::Unknown:
			{
				setAnswer( answer, ErrorCodeNotImplemented, _TXT("output content type unknown"));
				return false;
			}
			default: break;
		}
		if (resultstr)
		{
			const char* encname = papuga_stringEncodingName( m_result_encoding);
			WebRequestContent resultContent( encname, WebRequestContent::typeMime(m_result_doctype), resultstr, resultulen);
			answer.setContent( resultContent);
			return true;
		}
		else
		{
			setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
			return false;
		}
	}
	return true;
}

bool WebRequestContext::inheritRequestContext( WebRequestAnswer& answer, const char* contextType, const char* contextName)
{
	papuga_ErrorCode errcode = papuga_Ok;
	char buf[ 1024];
	if (!m_context)
	{
		errcode = papuga_ExecutionOrder;
		goto ERROR;
	}
	if (!papuga_RequestContext_inherit( m_context, m_handler->impl(), contextType, contextName))
	{
		errcode = papuga_RequestContext_last_error( m_context, true);
		goto ERROR;
	}
	return true;
ERROR:
	if (errcode == papuga_AddressedItemNotFound)
	{
		std::snprintf( buf, sizeof(buf), _TXT("undefined %s '%s'"), contextType, contextName);
	}
	else
	{
		std::snprintf( buf, sizeof(buf), _TXT("failed to inherit from %s '%s'"), contextType, contextName);
	}
	buf[ sizeof(buf)-1] = 0;
	setAnswer( answer, errcode, buf, true);
	return false;
}


bool WebRequestContext::createRequestContext( WebRequestAnswer& answer, const char* contextType, const char* contextName)
{
	if (!createEmptyRequestContext( answer, 0/*classname*/)) return false;
	if (contextName)
	{
		if (!inheritRequestContext( answer, contextType, contextName)) return false;
	}
	return true;
}

bool WebRequestContext::createEmptyRequestContext( WebRequestAnswer& answer, const char* className)
{
	if (m_context_ownership && m_context) papuga_destroy_RequestContext( m_context);
	m_context = papuga_create_RequestContext( className);
	m_context_ownership = true;
	if (!m_context)
	{
		setAnswer( answer, ErrorCodeOutOfMem);
		return false;
	}
	return true;
}

bool WebRequestContext::initRequestContext( WebRequestAnswer& answer)
{
	papuga_ErrorCode errcode = papuga_Ok;
	if (!m_request)
	{
		setAnswer( answer, ErrorCodeOperationOrder);
		return false;
	}
	papuga_RequestInheritedContextDef const* di = papuga_Request_get_inherited_contextdefs( m_request, &errcode);
	if (!di)
	{
		setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	for (; di->type && di->name; ++di)
	{
		if (!inheritRequestContext( answer, di->type, di->name)) return false;
	}
	return true;
}

bool WebRequestContext::initContentRequest( WebRequestAnswer& answer, const char* contextType, const char* schema)
{
	m_atm = papuga_RequestHandler_get_automaton( m_handler->impl(), contextType, schema);
	if (!m_atm)
	{
		setAnswer( answer, ErrorCodeRequestResolveError, _TXT("unknown schema"));
		return false;
	}
	if (m_request) papuga_destroy_Request( m_request);
	m_request = papuga_create_Request( m_atm);
	if (!m_request)
	{
		setAnswer( answer, ErrorCodeOutOfMem);
		return false;
	}
	return true;
}

class PathBuf
{
public:
	PathBuf( const char* pt)
	{
		if ((int)sizeof(buf)-1 <= std::snprintf( buf, sizeof(buf), "%s", pt)) throw std::bad_alloc();
		itr = buf;
		while (*itr == '/') ++itr;
	}

	bool startsWith( const char* prefix, int prefixlen)
	{
		char* itrnext = std::strchr( itr, '/');
		return (prefixlen == itrnext-itr && prefix[0] == itr[0] && 0==std::memcmp( prefix, itr, prefixlen));
	}
	bool hasMore()
	{
		return itr[0];
	}
	const char* getNext()
	{
		if (!itr[0]) return 0;
		char const* rt = itr;
		char* itrnext = std::strchr( itr, '/');
		if (itrnext)
		{
			*itrnext = 0;
			itr = itrnext+1;
			while (*itr == '/') ++itr;
		}
		else
		{
			itr = std::strchr( itr, '\0');
		}
		return rt;
	}
	const char* getRest()
	{
		char const* rt = itr;
		itr = std::strchr( itr, '\0');
		char* enditr = itr;
		while (enditr != buf && *(enditr-1) == '/') --enditr;
		*enditr = '\0';
		return rt;
	}
	const char* rest()
	{
		char* enditr = std::strchr( itr, '\0');
		while (enditr != buf && *(enditr-1) == '/') --enditr;
		*enditr = '\0';
		return itr;
	}

private:
	char* itr;
	char buf[ 2048];
};

static bool initHostObjMethodParam( papuga_ValueVariant& arg, WebRequestHandler::MethodParamType paramtype, const char* path, const WebRequestContent& content, papuga_Allocator* allocator, strus::ErrorCode& errcode)
{
	papuga_Serialization* ser;
	papuga_StringEncoding enc;
	switch (paramtype)
	{
		case WebRequestHandler::ParamEnd:
			return false;
		case WebRequestHandler::ParamPathString:
			papuga_init_ValueVariant_charp( &arg, path);
			return true;
		case WebRequestHandler::ParamPathArray:
			ser = papuga_Allocator_alloc_Serialization( allocator);
			if (!ser)
			{
				errcode = ErrorCodeOutOfMem;
				return false;
			}
			else
			{
				PathBuf pathsplit( path);
				const char* pathelem;
				while (!!(pathelem = pathsplit.getNext()))
				{
					if (!papuga_Serialization_pushValue_charp( ser, pathelem))
					{
						errcode = ErrorCodeOutOfMem;
						return false;
					}
				}
			}
			papuga_init_ValueVariant_serialization( &arg, ser);
			return true;
		case WebRequestHandler::ParamDocumentClass:
			ser = papuga_Allocator_alloc_Serialization( allocator);
			if (!ser)
			{
				errcode = ErrorCodeOutOfMem;
				return false;
			}
			papuga_Serialization_set_structid( ser, STRUS_BINDINGS_STRUCTID_DocumentClass);
			if (!papuga_Serialization_pushValue_charp( ser, content.doctype())
			||	!papuga_Serialization_pushValue_charp( ser, content.charset()))
			{
				errcode = ErrorCodeOutOfMem;
				return false;
			}
			papuga_init_ValueVariant_serialization( &arg, ser);
			return true;
		case WebRequestHandler::ParamContent:
			if (!papuga_getStringEncodingFromName( &enc, content.charset()))
			{
				enc = papuga_Binary;
			}
			papuga_init_ValueVariant_string_enc( &arg, enc, content.str(), content.len());
			return true;
	}
	errcode = ErrorCodeLogicError;
	return false;
}

bool WebRequestContext::callHostObjMethod( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content, papuga_CallResult& retval, WebRequestAnswer& answer)
{
	// Get method function pointer to call:
	const papuga_ClassDef* cdeflist = strus_getBindingsClassDefs();
	const papuga_ClassDef* cdef = &cdeflist[ methoddescr->id.classid-1];
	if (methoddescr->id.functionid == 0)
	{
		setAnswer( answer, ErrorCodeInvalidArgument);
		return false;
	}
	papuga_ClassMethod method = cdef->methodtable[ methoddescr->id.functionid-1];

	// Initialize the arguments:
	enum {MaxNofArgs=32};
	papuga_ValueVariant argv[MaxNofArgs];
	int argc = 0;
	strus::ErrorCode errcode = ErrorCodeUnknown;
	bool path_argument_used = false;
	for (; argc < MaxNofArgs && methoddescr->paramtypes[argc]; ++argc)
	{
		WebRequestHandler::MethodParamType paramtype = (WebRequestHandler::MethodParamType)methoddescr->paramtypes[argc];
		path_argument_used |= (paramtype == WebRequestHandler::ParamPathArray || WebRequestHandler::ParamPathString);
		if (!initHostObjMethodParam( argv[ argc], paramtype, path, content, &m_allocator, errcode))
		{
			setAnswer( answer, errcode);
			return false;
		}
	}
	if (argc == MaxNofArgs)
	{
		setAnswer( answer, ErrorCodeMaxNofItemsExceeded);
		return false;
	}
	if (!path_argument_used && path[0])
	{
		// ... path defined but not required in method
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
	// Call the method:
	if (!(*method)( self, &retval, argc, argv))
	{
		char* errstr = papuga_CallResult_lastError( &retval);
		char const* msgitr = errstr;
		int apperr = strus::errorCodeFromMessage( msgitr);
		if (apperr) strus::removeErrorCodesFromMessage( errstr);

		setAnswer( answer, apperr, errstr, true);
		return false;
	}
	answer.setStatus( methoddescr->httpstatus_success);
	return true;
}

bool WebRequestContext::callExtensionMethod( void* self, const papuga_RequestMethodDescription* methoddescr, papuga_RequestContext* context, const char* resultname, WebRequestAnswer& answer)
{
	papuga_CallResult retval;
	char membuf_err[ 4096];
	papuga_init_CallResult( &retval, &m_allocator, false/*allocator ownerwhip*/, membuf_err, sizeof(membuf_err));
	WebRequestContent content;

	if (!callHostObjMethod( self, methoddescr, "", content, retval, answer))
	{
		return false;
	}
	// Assign the result to context:
	if (retval.nofvalues == 0)
	{
		setAnswer( answer, ErrorCodeRuntimeError, _TXT( "result expected"));
		return false;
	}
	else if (retval.nofvalues > 1)
	{
		setAnswer( answer, ErrorCodeRuntimeError, _TXT( "only one result expected"));
		return false;
	}
	else if (!papuga_RequestContext_add_variable( context, resultname, &retval.valuear[0]))
	{
		return false;
	}
	return true;
}

bool WebRequestContext::callHostObjMethod( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content, WebRequestAnswer& answer)
{
	papuga_CallResult retval;
	char membuf_err[ 4096];
	papuga_init_CallResult( &retval, &m_allocator, false/*allocator ownerwhip*/, membuf_err, sizeof(membuf_err));

	if (!callHostObjMethod( self, methoddescr, path, content, retval, answer)) return false;

	// Assign the result:
	if (methoddescr->resulttype)
	{
		if (retval.nofvalues == 0)
		{
			answer.setMessage( methoddescr->httpstatus_success, methoddescr->resulttype, "");
			return true;
		}
		else if (retval.nofvalues > 1)
		{
			setAnswer( answer, ErrorCodeRuntimeError, _TXT( "only one result expected"));
			return false;
		}
		else
		{
			size_t msglen;
			papuga_ErrorCode ec = papuga_Ok;
			const char* msgstr = papuga_ValueVariant_tostring( &retval.valuear[0], &m_allocator, &msglen, &ec);
			if (!msgstr)
			{
				setAnswer( answer, papugaErrorToErrorCode( ec));
				return false;
			}
			else
			{
				answer.setMessage( methoddescr->httpstatus_success, methoddescr->resulttype, msgstr);
				return true;
			}
		}
	}
	else
	{
		if (retval.nofvalues == 0)
		{
			answer.setStatus( methoddescr->httpstatus_success);
			return true;
		}
		else if (retval.nofvalues > 1)
		{
			setAnswer( answer, ErrorCodeRuntimeError, _TXT( "only one result expected"));
			return false;
		}
		else
		{
			if (!mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), methoddescr->result_rootelem, methoddescr->result_listelem, m_result_encoding, m_result_doctype, retval.valuear[0]))
			{
				return false;
			}
			return true;
		}
	}
}

static bool checkPapugaListBufferOverflow( const char** ci, WebRequestAnswer& answer)
{
	if (!ci)
	{
		setAnswer( answer, ErrorCodeBufferOverflow);
		return false;
	}
	return true;
}

struct ObjectDescr
{
	papuga_RequestContext* context;
	const papuga_ValueVariant* obj;
	const char* typenam;
	const char* classnam;
	const char* contextnam;
	const char* varnam;
	bool context_ownership;

	ObjectDescr()
		:context(0),obj(0),typenam(0),classnam(0),contextnam(0),varnam(0),context_ownership(false){}
	~ObjectDescr()
	{
		if (context_ownership) papuga_destroy_RequestContext( context);
	}
	void reset()
	{
		if (context_ownership) papuga_destroy_RequestContext( context);
		context = 0;
		obj = 0;
	}

	void init( papuga_RequestContext* context_, const char* typenam_, const char* classnam_, const char* contextnam_)
	{
		context = context_;
		typenam = typenam_;
		classnam = classnam_;
		contextnam = contextnam_;
		obj = papuga_RequestContext_get_variable( context, typenam, NULL/*param[out] isArray*/);
		varnam = 0;
		context_ownership = false;
	}

	bool setPath( const char* path)
	{
		papuga_ValueVariant pathvalue;
		papuga_init_ValueVariant_charp( &pathvalue, path);
		return papuga_RequestContext_add_variable( context, "_path", &pathvalue);
	}

	bool init( const papuga_RequestHandler* handler, PathBuf& path, WebRequestAnswer& answer)
	{
		papuga_ErrorCode errcode = papuga_Ok;
		enum {lstbufsize=256};
		char const* lstbuf[ lstbufsize];

		if (!(typenam = path.getNext())) return true;
		if (isEqual( typenam, ROOT_CONTEXT_NAME))
		{
			contextnam = ROOT_CONTEXT_NAME;
		}
		else
		{
			if (!(contextnam = path.getNext())) return true;
		}
		context = papuga_create_RequestContext( 0/*className*/);
		if (!context)
		{
			setAnswer( answer, ErrorCodeOutOfMem);
			return false;
		}
		context_ownership = true;
		if (!papuga_RequestContext_inherit( context, handler, typenam, contextnam))
		{
			errcode = papuga_RequestContext_last_error( context, true);
			reset();
			if (errcode == papuga_AddressedItemNotFound)
			{
				return true;
			}
			else
			{
				reset();
				setAnswer( answer, papugaErrorToErrorCode( errcode));
				return false;
			}
		}
		char const** varlist = papuga_RequestContext_list_variables( context, 1/*max inheritcnt*/, lstbuf, lstbufsize);
		if (!checkPapugaListBufferOverflow( varlist, answer)) return false;
		if (varlist[0] && !varlist[1] && isEqual( *varlist, typenam))
		{
			obj = papuga_RequestContext_get_variable( context, typenam, NULL/*param[out] isArray*/);
			return true;
		}
		if (!(varnam = path.getNext())) return true;
		obj = papuga_RequestContext_get_variable( context, varnam, NULL/*param[out] isArray*/);
		if (!obj)
		{
			reset();
			setAnswer( answer, ErrorCodeRequestResolveError);
			return false;
		}
		return true;
	}
};

void WebRequestContext::releaseContext()
{
	m_context_ownership = false;
}

bool WebRequestContext::executeMainSchema( const char* schema, const WebRequestContent& content, WebRequestAnswer& answer)
{
	return executeContextSchema( (const char*)0, (const char*)0, schema, content, answer);
}

bool WebRequestContext::executeContextSchema( const char* contextType, const char* contextName, const char* schema, const WebRequestContent& content, WebRequestAnswer& answer)
{
	return	createRequestContext( answer, contextType, contextName)
	&&	initContentRequest( answer, contextType, schema)
	&&	feedContentRequest( answer, content)
	&&	initRequestContext( answer)
	&&	executeContentRequest( answer, content)
	&&	getContentRequestResult( answer, content);
}

bool WebRequestContext::executeContextSchema( papuga_RequestContext* context, const char* contextType, const char* schema, const WebRequestContent& content, WebRequestAnswer& answer)
{
	if (m_context_ownership && m_context) papuga_destroy_RequestContext( m_context);
	m_context = context;
	m_context_ownership = false;
	return	initContentRequest( answer, contextType, schema)
	&&	feedContentRequest( answer, content)
	&&	initRequestContext( answer)
	&&	executeContentRequest( answer, content)
	&&	getContentRequestResult( answer, content);
}

bool WebRequestContext::executeOPTIONS(
		const char* path_,
		const WebRequestContent& content,
		WebRequestAnswer& answer)
{
	PathBuf path( path_);
	ObjectDescr selector;
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];

	if (!selector.init( m_handler->impl(), path, answer)) return false;

	if (!content.empty())
	{
		setAnswer( answer, ErrorCodeInvalidArgument);
		return false;
	}
	else if (selector.obj && selector.obj->valuetype == papuga_TypeHostObject)
	{
		std::string http_allow("OPTIONS,");
		char const** sl = papuga_RequestHandler_list_schema_names( m_handler->impl(), selector.typenam, lstbuf, lstbufsize);
		if (!checkPapugaListBufferOverflow( sl, answer)) return false;
		for (; *sl; ++sl) http_allow.append( *sl);
		int classid = selector.obj->value.hostObject->classid;
		char const** ml = papuga_RequestHandler_list_methods( m_handler->impl(), classid, lstbuf, lstbufsize);
		if (!checkPapugaListBufferOverflow( ml, answer)) return false;
		for (; *ml; ++ml) http_allow.append( *ml);
		const char* msgstr = papuga_Allocator_copy_string( &m_allocator, http_allow.c_str(), http_allow.size());
		if (!msgstr)
		{
			setAnswer( answer, ErrorCodeOutOfMem);
			return false;
		}
		answer.setMessage( 200/*OK*/, "Allow", msgstr);
		return true;
	}
	else if (selector.contextnam)
	{
		if (selector.context)
		{
			std::string http_allow("OPTIONS,");
			char const** sl = papuga_RequestHandler_list_schema_names( m_handler->impl(), selector.typenam, lstbuf, lstbufsize);
			if (!checkPapugaListBufferOverflow( sl, answer)) return false;
			for (; *sl; ++sl) http_allow.append( *sl);
			const char* msgstr = papuga_Allocator_copy_string( &m_allocator, http_allow.c_str(), http_allow.size());
			if (!msgstr)
			{
				setAnswer( answer, ErrorCodeOutOfMem);
				return false;
			}
			answer.setMessage( 200/*OK*/, "Allow", msgstr);
			return true;
		}
		else
		{
			answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,PUT");
			return true;
		}
	}
	else
	{
		answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,GET");
		return true;
	}
}

bool WebRequestContext::executePostTransaction( void* self, int classid, const char* typenam, const char* contextnam, WebRequestAnswer& answer)
{
	const char* method = "POST/transaction";
	const char* resultname = "transaction";
	const papuga_RequestMethodDescription* methoddescr
		= papuga_RequestHandler_get_method( m_handler->impl(), classid, method, false);
	if (!methoddescr)
	{
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
	std::string className = strus::string_format( "transaction/%s", typenam);
	if (!createEmptyRequestContext( answer, className.c_str()))
	{
		return false;
	}
	if (!papuga_RequestContext_inherit( m_context, m_handler->impl(), typenam, contextnam))
	{
		setAnswer( answer, papugaErrorToErrorCode( papuga_RequestContext_last_error( m_context, true)));
		return false;
	}
	if (!callExtensionMethod( self, methoddescr, m_context, resultname, answer))
	{
		return false;
	}
	std::string tid = m_transactionPool->createTransaction( m_context, m_handler->maxIdleTime());
	std::string linkbase;
	int ec = strus::getAncestorPath( m_html_base_href, 3, linkbase);
	std::string tlinkparent = strus::joinFilePath( linkbase, "transaction");
	std::string tlink = strus::joinFilePath( tlinkparent, tid);
	if (ec)
	{
		setAnswer( answer, ErrorCode( ec), _TXT("failed to get link base"));
		return false;
	}
	if (tid.empty() || tlinkparent.empty() || tlink.empty())
	{
		setAnswer( answer, ErrorCodeOutOfMem);
		return false;
	}
	releaseContext();
	return strus::mapStringToAnswer( answer, &m_allocator, m_handler->html_head(), ""/*html href base*/, PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, tlink);
}

bool WebRequestContext::executeCommitTransaction( const papuga_ValueVariant* obj, WebRequestAnswer& answer)
{
	if (obj && obj->valuetype == papuga_TypeHostObject)
	{
		int classid = obj->value.hostObject->classid;
		void* self = obj->value.hostObject->data;

		const papuga_RequestMethodDescription* methoddescr
			= papuga_RequestHandler_get_method(
				m_handler->impl(), classid, "PUT/transaction"/*method*/, false/*has content*/);
		if (methoddescr)
		{
			WebRequestContent content;
			return callHostObjMethod( self, methoddescr, ""/*path*/, content, answer);
		}
		else
		{
			setAnswer( answer, ErrorCodeRequestResolveError);
			return false;
		}
	}
	else
	{
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
}

bool WebRequestContext::executeDeclareConfiguration( const char* typenam, const char* contextnam, const char* request_method, bool init, const WebRequestContent& content, WebRequestAnswer& answer)
{
	ConfigurationTransaction cfgtransaction;
	std::string configstr = webRequestContent_tostring( content, 0);
	ConfigurationDescription cfgdescr( typenam, contextnam, content.doctype(), configstr);
	char schema[ 128];
	if ((int)sizeof(schema) <= std::snprintf( schema, sizeof(schema), "%s/%s", request_method, typenam))
	{
		setAnswer( answer, ErrorCodeBufferOverflow);
		return false;
	}
	if (!init)
	{
		m_confighandler->storeConfiguration( cfgtransaction, cfgdescr);
	}
	if (!executeContextSchema( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, schema, content, answer)
	||  !m_handler->transferContext( typenam, contextnam, m_context, answer))
	{
		return false;
	}
	releaseContext();
	if (!init)
	{
		m_confighandler->commitStoreConfiguration( cfgtransaction);
	}
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
	{
		m_logger->logAction( typenam, contextnam, init?_TXT("load configuration"):_TXT("put configuration"));
	}
	answer.setStatus( 204/*no content*/);
	return true;
}

bool WebRequestContext::executeLoadMainConfiguration( const WebRequestContent& content, WebRequestAnswer& answer)
{
	if (!executeMainSchema( ROOT_CONTEXT_NAME, content, answer)
	||  !m_handler->transferContext( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, m_context, answer))
	{
		return false;
	}
	releaseContext();
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
	{
		m_logger->logAction( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, "load configuration");
	}
	return true;
}

bool WebRequestContext::executeLoadSubConfiguration( const char* typenam, const char* contextnam, const WebRequestContent& content, WebRequestAnswer& answer)
{
	return executeDeclareConfiguration( typenam, contextnam, "PUT", true/*init*/, content, answer);
}

bool WebRequestContext::executeDeleteConfiguration( const char* typenam, const char* contextnam, WebRequestAnswer& answer)
{
	bool configContextFound = true;
	
	char schema[ 128];
	if ((int)sizeof(schema) <= std::snprintf( schema, sizeof(schema), "DELETE/%s", typenam))
	{
		setAnswer( answer, ErrorCodeBufferOverflow);
		return false;
	}
	configContextFound = m_handler->removeContext( typenam, contextnam, answer);
	if (papuga_RequestHandler_get_automaton( m_handler->impl(), ROOT_CONTEXT_NAME, schema))
	{
		ConfigurationDescription config = m_confighandler->getStoredConfiguration( typenam, contextnam);
		if (!config.valid())
		{
			if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
			{
				m_logger->logWarning( _TXT("configuration content not found, delete schema not executed"));
			}
		}
		else
		{
			WebRequestContent content( "UTF-8", config.doctype.c_str(), config.contentbuf.c_str(), config.contentbuf.size());
			if (!executeContextSchema( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, schema, content, answer))
			{
				return false;
			}
		}
	}
	m_confighandler->deleteStoredConfiguration( typenam, contextnam);
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
	{
		m_logger->logAction( typenam, contextnam, _TXT("delete configuration"));
	}
	if (!configContextFound)
	{
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
	answer.setStatus( 204/*no content*/);
	return true;
}

bool WebRequestContext::executeSchemaDescriptionRequest(
		const char* method,
		const char* pathstr,
		WebRequestAnswer& answer)
{
	PathBuf path( pathstr);

	std::string schema;
	const char* typenam = path.getNext();
	const char* contextType = "";
	if (!typenam)
	{
		schema = ROOT_CONTEXT_NAME;
	}
	else if (isEqual( method, "POST") || isEqual( method, "PUT") || isEqual( method, "DELETE"))
	{
		contextType = ROOT_CONTEXT_NAME;
		schema = strus::string_format( "%s/%s", method, typenam);
	}
	else
	{
		contextType = typenam;
		schema = method;
	}
	if (path.hasMore())
	{
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
	const papuga_SchemaDescription* descr = papuga_RequestHandler_get_description( m_handler->impl(), contextType, schema.c_str());
	if (!descr)
	{
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
	else
	{
		papuga_ContentType schema_doctype = papuga_ContentType_Unknown;
		switch (m_result_doctype)
		{
			case WebRequestContent::Unknown:
			case WebRequestContent::HTML:
			case WebRequestContent::TEXT:
				setAnswer( answer, ErrorCodeNotImplemented);
				return false;
			case WebRequestContent::JSON:
				schema_doctype = papuga_ContentType_JSON;
				break;
			case WebRequestContent::XML:
				schema_doctype = papuga_ContentType_XML;
				break;
		}
		std::size_t txtlen;
		const char* txt = (const char*)papuga_SchemaDescription_get_text( descr, &m_allocator, schema_doctype, m_result_encoding, &txtlen);
		if (txt)
		{
			WebRequestContent answerContent( papuga_stringEncodingName( m_result_encoding), WebRequestContent::typeMime(m_result_doctype), txt, txtlen);
			answer.setContent( answerContent);
			return true;
		}
		else
		{
			papuga_ErrorCode errcode = papuga_SchemaDescription_last_error( descr);
			setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
			return false;
		}
	}
}

bool WebRequestContext::getMessageAnswer(
		const std::string& message,
		WebRequestAnswer& answer)
{
	if (!setResultContentType( answer, papuga_UTF8, WebRequestContent::HTML)) return false;
	return strus::mapStringToAnswer( answer, &m_allocator, m_handler->html_head(), ""/*html href base*/, "", m_result_encoding, m_result_doctype, message);
}

bool WebRequestContext::executeRequest(
		const char* method,
		const char* path_,
		const WebRequestContent& content,
		WebRequestAnswer& answer)
{
	TransactionRef transactionRef;
	try
	{
		PathBuf path( path_);
		if (isEqual( method, "OPTIONS"))
		{
			return executeOPTIONS( path_, content, answer);
		}
		if (!setResultContentType( answer, papuga_UTF8, WebRequestContent::HTML)) return false;

		ObjectDescr selector;
		if (path.startsWith( "transaction", 11/*"transaction"*/))
		{
			const char* typenam = path.getNext();
			const char* contextnam = path.getNext();
			if (!typenam || !contextnam)
			{
				setAnswer( answer, ErrorCodeIncompleteRequest);
				return false;
			}
			transactionRef = m_transactionPool->fetchTransaction( contextnam);
			if (!transactionRef.get())
			{
				setAnswer( answer, ErrorCodeRequestResolveError);
				return false;
			}
			const char* className = papuga_RequestContext_classname( transactionRef->context());
			selector.init( transactionRef->context(), typenam, className, contextnam);
		}
		else if (path.startsWith( "schema", 6/*"schema"*/))
		{
			(void)path.getNext();//... "schema" is not part of request
			if (isEqual(method,"GET"))
			{
				const char* schema_method_ptr = path.getNext();
				if (!schema_method_ptr)
				{
					setAnswer( answer, ErrorCodeIncompleteRequest);
					return false;
				}
				std::string schema_method = strus::string_conv::toupper( schema_method_ptr);
				return executeSchemaDescriptionRequest( schema_method.c_str(), path.rest(), answer);
			}
			else
			{
				setAnswer( answer, ErrorCodeNotImplemented);
				return false;
			}
		}
		else if (!selector.init( m_handler->impl(), path, answer))
		{
			return false;
		}
		if (!path.hasMore())
		{
			if (selector.contextnam)
			{
				// PUT or DELETE of configuration/transaction or fallback to next:
				if (isEqual(method,"PUT") || isEqual(method,"POST"))
				{
					if (transactionRef.get())
					{
						if (content.empty())
						{
							return executeCommitTransaction( selector.obj, answer);
						}
						//... else fallback
					}
					else
					{
						selector.reset();
						return executeDeclareConfiguration( selector.typenam, selector.contextnam, method, false/*init*/, content, answer);
					}
				}
				else if (isEqual(method,"DELETE"))
				{
					if (transactionRef.get())
					{
						if (content.empty())
						{
							transactionRef.reset();
							m_transactionPool->releaseTransaction( selector.contextnam);
							answer.setStatus( 204/*no content*/);
							return true;
						}
						//... else fallback
					}
					else
					{
						selector.reset();
						return executeDeleteConfiguration( selector.typenam, selector.contextnam, answer);
					}
				}
				else if (!selector.obj && selector.context && content.empty() && isEqual( method, "GET"))
				{
					enum {lstbufsize=256};
					char const* lstbuf[ lstbufsize];
					char const** varlist = papuga_RequestContext_list_variables( selector.context, 1/*max inheritcnt*/, lstbuf, lstbufsize);
					if (!checkPapugaListBufferOverflow( varlist, answer)) return false;
					if (!strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), "list", PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, varlist)) return false;
					goto DONE;
				}
				// else fallback
			}
			else
			{
				// Top level introspection without object defined:
				if (!content.empty())
				{
					setAnswer( answer, ErrorCodeInvalidArgument);
					return false;
				}
				if (isEqual( method, "GET"))
				{
					if (!selector.typenam)
					{
						std::vector<std::string> contextTypes = m_confighandler->contextTypes();
						contextTypes.push_back( ROOT_CONTEXT_NAME);
						return strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), "list", PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, contextTypes);
					}
					else
					{
						std::vector<std::string> contextlist = m_confighandler->contextNames( selector.typenam);
						if (contextlist.empty())
						{
							setAnswer( answer, ErrorCodeRequestResolveError);
							return false;
						}
						else
						{
							return strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), "list", PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, contextlist);
						}
					}
				}
				else
				{
					setAnswer( answer, ErrorCodeRequestResolveError);
					return false;
				}
			}
		}
		// Call object method with content if object defined and method defined, else fallback to next:
		if (selector.obj && selector.obj->valuetype == papuga_TypeHostObject)
		{
			int classid = selector.obj->value.hostObject->classid;
			void* self = selector.obj->value.hostObject->data;

			if (isEqual(method,"POST") && isEqual( path.rest(),"transaction"))
			{
				return executePostTransaction( self, classid, selector.typenam, selector.contextnam, answer);
			}
			else
			{
				const papuga_RequestMethodDescription* methoddescr = papuga_RequestHandler_get_method( m_handler->impl(), classid, method, !content.empty());
				if (methoddescr)
				{
					if (!callHostObjMethod( self, methoddescr, path.rest(), content, answer)) return false;
					goto DONE;
				}
				//... fallback
			}
		}
		if (selector.context)
		{
			if (!content.empty())
			{
				// schema execution else:
				if (!selector.setPath( path.rest()))
				{
					setAnswer( answer, ErrorCodeOutOfMem);
					return false;
				}
				if (!executeContextSchema( selector.context, selector.classnam ? selector.classnam : selector.typenam, method, content, answer)) return false;
				goto DONE;
			}
			else
			{
				setAnswer( answer, ErrorCodeIncompleteRequest);
				return false;
			}
		}
		else
		{
			setAnswer( answer, ErrorCodeRequestResolveError);
			return false;
		}
	}
	catch (const std::bad_alloc&)
	{
		setAnswer( answer, ErrorCodeOutOfMem);
		return false;
	}
	catch (const std::runtime_error& err)
	{
		setAnswer( answer, ErrorCodeRuntimeError, err.what(), true/*do copy*/);
		return false;
	}
	catch (...)
	{
		setAnswer( answer, ErrorCodeUncaughtException);
		return false;
	}
DONE:
	if (transactionRef.get())
	{
		try
		{
			m_transactionPool->returnTransaction( transactionRef);
		}
		catch (const std::bad_alloc&)
		{
			setAnswer( answer, ErrorCodeOutOfMem);
			return false;
		}
		catch (const std::exception& err)
		{
			setAnswer( answer, ErrorCodeRuntimeError, err.what(), true/*doCopa*/);
			return false;
		}
		catch (...)
		{
			setAnswer( answer, ErrorCodeUncaughtException);
		}
	}
	return true;
}


