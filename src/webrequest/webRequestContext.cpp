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
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/requestParser.h"
#include "papuga/requestHandler.h"
#include "papuga/requestResult.h"
#include "papuga/valueVariant.h"
#include "papuga/encoding.h"
#include "papuga/allocator.h"
#include "private/internationalization.hpp"
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <iostream>

using namespace strus;

#undef STRUS_LOWLEVEL_DEBUG

WebRequestContext::WebRequestContext(
		const WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		const char* accepted_charset_,
		const char* accepted_doctype_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_request(0)
	,m_encoding(papuga_Binary),m_doctype(papuga_ContentType_Unknown),m_doctypestr(0)
	,m_atm(0)
	,m_result_encoding(papuga_Binary),m_result_doctype(WebRequestContent::Unknown)
	,m_accepted_charset(accepted_charset_),m_accepted_doctype(accepted_doctype_)
{
	std::memset( &m_context, 0, sizeof(m_context));
	papuga_init_Allocator( &m_allocator, m_allocator_mem, sizeof(m_allocator_mem));
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));
}

WebRequestContext::~WebRequestContext()
{
	if (m_request) papuga_destroy_Request( m_request);
	papuga_destroy_Allocator( &m_allocator);
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
			m_logger->logLoggerError( papuga_ErrorCode_tostring( papuga_NoMemError));
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
		setAnswer( answer, ErrorCodeNotImplemented, _TXT("none of the accept charsets implemented"));
		return false;
	}
	std::size_t result_length = 0;
	const char* result = papuga_Request_tostring( m_request, &m_allocator, m_result_encoding, m_handler->debug_maxdepth(), &result_length, &errcode);
	if (result)
	{
		WebRequestContent content( papuga_stringEncodingName( m_result_encoding), "text/plain", result, result_length * papuga_StringEncoding_unit_size( m_result_encoding));
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
	if (!papuga_RequestContext_execute_request( &m_context, m_request, &m_errbuf, &errpos))
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
		setAnswer( answer, ErrorCodeNotImplemented, _TXT("none of the accept charsets implemented"));
		return false;
	}
	if (m_result_doctype == WebRequestContent::Unknown)
	{
		setAnswer( answer, ErrorCodeNotImplemented, _TXT("none of the accept content types implemented"));
		return false;
	}
	return true;
}

bool WebRequestContext::getContentRequestResult( WebRequestAnswer& answer)
{
	papuga_RequestResult result;
	papuga_ErrorCode errcode = papuga_Ok;
	if (!papuga_set_RequestResult( &result, &m_context, m_request))
	{
		setAnswer( answer, ErrorCodeOutOfMem);
		return false;
	}
	// Map the result:
	char* resultstr = 0;
	std::size_t resultulen = 0;
	switch (m_result_doctype)
	{
		case WebRequestContent::XML:  resultstr = (char*)papuga_RequestResult_toxml( &result, m_result_encoding, &resultulen, &errcode); break;
		case WebRequestContent::JSON: resultstr = (char*)papuga_RequestResult_tojson( &result, m_result_encoding, &resultulen, &errcode); break;
		case WebRequestContent::HTML: resultstr = (char*)papuga_RequestResult_tohtml5( &result, m_result_encoding, m_handler->html_head(), &resultulen, &errcode); break;
		case WebRequestContent::TEXT: resultstr = (char*)papuga_RequestResult_totext( &result, m_result_encoding, &resultulen, &errcode); break;
		case WebRequestContent::Unknown:
		{
			setAnswer( answer, ErrorCodeNotImplemented, _TXT("output content type unknown"));
			return false;
		}
		default: break;
	}
	if (resultstr)
	{
		int usize = papuga_StringEncoding_unit_size( m_result_encoding);
		const char* encname = papuga_stringEncodingName( m_result_encoding);
		WebRequestContent content( encname, WebRequestContent::typeMime(m_result_doctype), resultstr, resultulen * usize);
		answer.setContent( content);
		return true;
	}
	else
	{
		setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	return true;
}

bool WebRequestContext::inheritRequestContext( WebRequestAnswer& answer, const char* contextType, const char* contextName)
{
	papuga_ErrorCode errcode = papuga_Ok;
	char buf[ 1024];
	const papuga_RequestContext* inherit_context = papuga_RequestHandler_find_context( m_handler->impl(), contextType, contextName);
	if (!inherit_context)
	{
		errcode = papuga_NotImplemented;
		goto ERROR;
	}
	if (!papuga_RequestContext_inherit( &m_context, inherit_context , &errcode))
	{
		goto ERROR;
	}
	return true;
ERROR:
	if (errcode == papuga_NotImplemented)
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


bool WebRequestContext::initContentRequestContext( WebRequestAnswer& answer, const char* contextType, const char* contextName)
{
	papuga_ErrorCode errcode = papuga_Ok;
	//PF:HACK: Logging is not const, but thread safe, so a const_cast is, though never good, Ok here:
	papuga_init_RequestContext( &m_context, &m_allocator, const_cast<WebRequestHandler*>( m_handler)->call_logger());
	if (contextName)
	{
		if (!inheritRequestContext( answer, contextType, contextName)) return false;
	}
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

bool WebRequestContext::initContentRequest( WebRequestAnswer& answer, const char* contextType, const char* scheme)
{
	m_atm = papuga_RequestHandler_get_scheme( m_handler->impl(), contextType, scheme);
	if (!m_atm)
	{
		setAnswer( answer, ErrorCodeRequestResolveError, _TXT("unknown scheme"));
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
		return rt;
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
			papuga_init_ValueVariant_string_enc( &arg, enc, content.str(), content.len() / papuga_StringEncoding_unit_size(enc));
			return true;
	}
	errcode = ErrorCodeLogicError;
	return false;
}

bool WebRequestContext::callHostObjMethod( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content, WebRequestAnswer& answer)
{
	papuga_CallResult retval;
	char membuf_err[ 4096];
	papuga_init_CallResult( &retval, &m_allocator, true, membuf_err, sizeof(membuf_err));

	// Get method function pointer to call:
	const papuga_ClassDef* cdeflist = strus_getBindingsClassDefs();
	const papuga_ClassDef* cdef = &cdeflist[ methoddescr->id.classid-1];
	if (methoddescr->id.functionid == 0)
	{
		setAnswer( answer, ErrorCodeNotAllowed);
		return false;
	}
	papuga_ClassMethod method = cdef->methodtable[ methoddescr->id.functionid-1];

	// Initialize the arguments:
	enum {MaxNofArgs=32};
	papuga_ValueVariant argv[MaxNofArgs];
	int argc = 0;
	strus::ErrorCode errcode = ErrorCodeUnknown;
	for (; argc < MaxNofArgs && methoddescr->paramtypes[argc]; ++argc)
	{
		WebRequestHandler::MethodParamType paramtype = (WebRequestHandler::MethodParamType)methoddescr->paramtypes[argc];
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
	// Call the method:
	if (!(*method)( self, &retval, 1/*argc*/, argv))
	{
		char* errstr = papuga_CallResult_lastError( &retval);
		char const* msgitr = errstr;
		int apperr = strus::errorCodeFromMessage( msgitr);
		if (apperr) strus::removeErrorCodesFromMessage( errstr);

		setAnswer( answer, apperr, errstr, true);
		return false;
	}
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
			if (!mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), methoddescr->result_rootelem, methoddescr->result_listelem, m_result_encoding, m_result_doctype, retval.valuear[0]))
			{
				return false;
			}
			return true;
		}
	}
}

bool WebRequestContext::callObjMethod( const papuga_ValueVariant* obj, const char* methodname, const char* path, const WebRequestContent& content, WebRequestAnswer& answer)
{
	// Call object method with content if object defined and method defined, else fallback to next:
	if (obj->valuetype == papuga_TypeHostObject)
	{
		int classid = obj->value.hostObject->classid;
		void* self = obj->value.hostObject->data;
		const papuga_RequestMethodDescription* methoddescr = papuga_RequestHandler_get_method( m_handler->impl(), classid, methodname);
		if (methoddescr)
		{
			return callHostObjMethod( self, methoddescr, path, content, answer);
		}
	}
	setAnswer( answer, ErrorCodeRequestResolveError);
	return false;
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
static bool checkPapugaListEmpty( const char** ci, WebRequestAnswer& answer)
{
	if (!*ci)
	{
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
	return true;
}

bool WebRequestContext::dumpViewAll( papuga_Serialization* ser, WebRequestAnswer& answer)
{
	bool rt = true;
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];

	char const** typelist = papuga_RequestHandler_list_context_types( m_handler->impl(), lstbuf, lstbufsize);
	if (!checkPapugaListBufferOverflow( typelist, answer)) return false;
	char const** ti = typelist;
	for (; *ti; ++ti)
	{
		rt &= papuga_Serialization_pushName_charp( ser, *ti);
		rt &= papuga_Serialization_pushOpen( ser);
		if (!dumpViewType( *ti, ser, answer)) return false;
		rt &= papuga_Serialization_pushClose( ser);
	}
	if (!rt) setAnswer( answer, ErrorCodeOutOfMem);
	return rt;
}

bool WebRequestContext::dumpViewType( const char* typenam, papuga_Serialization* ser, WebRequestAnswer& answer)
{
	bool rt = true;
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];
	char const** contextlist = papuga_RequestHandler_list_contexts( m_handler->impl(), typenam, lstbuf, lstbufsize);
	if (!checkPapugaListBufferOverflow( contextlist, answer)) return false;
	if (!checkPapugaListEmpty( contextlist, answer)) return false;
	char const** ci = contextlist;
	for (; *ci; ++ci)
	{
		rt &= papuga_Serialization_pushName_charp( ser, *ci);
		rt &= papuga_Serialization_pushOpen( ser);
		if (!dumpViewName( typenam, *ci, ser, answer)) return false;
		rt &= papuga_Serialization_pushClose( ser);
	}
	if (!rt) setAnswer( answer, ErrorCodeOutOfMem);
	return rt;
}

bool WebRequestContext::dumpViewName( const char* typenam, const char* contextnam, papuga_Serialization* ser, WebRequestAnswer& answer)
{
	bool rt = true;
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];

	const papuga_RequestContext* context = papuga_RequestHandler_find_context( m_handler->impl(), typenam, contextnam);
	if (context == NULL)
	{
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
	else
	{
		char const** varlist = papuga_RequestContext_list_variables( context, 0/*max inheritcnt*/, lstbuf, lstbufsize);
		if (!checkPapugaListBufferOverflow( varlist, answer)) return false;
		if (varlist[0] && !varlist[1] && 0==std::strcmp( *varlist, typenam))
		{
			if (!dumpViewVar( context, typenam, ser, answer)) return false;
		}
		else
		{
			char const** vi = varlist;
			for (; *vi; ++vi)
			{
				rt &= papuga_Serialization_pushName_charp( ser, *vi);
				if (!dumpViewVar( context, *vi, ser, answer)) return false;
			}
		}
	}
	if (!rt) setAnswer( answer, ErrorCodeOutOfMem);
	return rt;
}

bool WebRequestContext::dumpViewVar( const papuga_RequestContext* context, const char* varnam, papuga_Serialization* ser, WebRequestAnswer& answer)
{
	papuga_ValueVariant result;
	const papuga_ValueVariant* obj = papuga_RequestContext_get_variable( context, varnam);
	if (!obj)
	{
		setAnswer( answer, ErrorCodeRequestResolveError);
		return false;
	}
	static const WebRequestContent empty_content;
	if (!callObjMethod( obj, "GET", "", empty_content, answer)) return false;
	if (!papuga_Serialization_pushValue( ser, &result))
	{
		setAnswer( answer, ErrorCodeOutOfMem);
		return false;
	}
	return true;
}

struct ObjectDescr
{
	const papuga_RequestContext* context;
	const papuga_ValueVariant* obj;

	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];
	const char* typenam;
	const char* contextnam;
	const char* varnam;

	ObjectDescr()
		:context(0),obj(0),typenam(0),contextnam(0),varnam(0){}

	bool init( const papuga_RequestHandler* handler, PathBuf& path, WebRequestAnswer& answer)
	{
		if (!(typenam = path.getNext())) return true;
		if (!(contextnam = path.getNext())) return true;
		context = papuga_RequestHandler_find_context( handler, typenam, contextnam);
		if (context == NULL)
		{
			return true;
		}
		char const** varlist = papuga_RequestContext_list_variables( context, 0/*max inheritcnt*/, lstbuf, lstbufsize);
		if (!checkPapugaListBufferOverflow( varlist, answer)) return false;
		if (varlist[0] && !varlist[1] && 0==std::strcmp( *varlist, typenam))
		{
			obj = papuga_RequestContext_get_variable( context, typenam);
			return true;
		}
		if (!(varnam = path.getNext())) return true;
		obj = papuga_RequestContext_get_variable( context, varnam);
		if (!obj)
		{
			setAnswer( answer, ErrorCodeRequestResolveError);
			return false;
		}
		return true;
	}
};

bool WebRequestContext::executeContextScheme( const char* contextType, const char* contextName, const char* scheme, const WebRequestContent& content, WebRequestAnswer& answer)
{
	return initContentRequest( answer, contextType, scheme)
	&&	feedContentRequest( answer, content)
	&&	initContentRequestContext( answer, contextType, contextName)
	&&	executeContentRequest( answer, content)
	&&	getContentRequestResult( answer);
}

bool WebRequestContext::executeOPTIONS(
		const char* path_,
		const WebRequestContent& content,
		WebRequestAnswer& answer)
{
	PathBuf path( path_);
	ObjectDescr selector;
	if (!selector.init( m_handler->impl(), path, answer)) return false;

	if (!content.empty())
	{
		setAnswer( answer, ErrorCodeInvalidArgument);
		return false;
	}
	else if (selector.obj && selector.obj->valuetype == papuga_TypeHostObject)
	{
		std::string http_allow("OPTIONS,");
		char const** sl = papuga_RequestHandler_list_schemes( m_handler->impl(), selector.typenam, selector.lstbuf, selector.lstbufsize);
		if (!checkPapugaListBufferOverflow( sl, answer)) return false;
		for (; *sl; ++sl) http_allow.append( *sl);
		int classid = selector.obj->value.hostObject->classid;
		char const** ml = papuga_RequestHandler_list_methods( m_handler->impl(), classid, selector.lstbuf, selector.lstbufsize);
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
	else
	{
		answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,LIST,GET");
		return true;
	}
}

bool WebRequestContext::executeRequest(
		const char* method_,
		const char* path_,
		const WebRequestContent& content,
		WebRequestAnswer& answer)
{
	try
	{
		if (0==std::strcmp(method_,"OPTIONS"))
		{
			return executeOPTIONS( path_, content, answer);
		}

		PathBuf path( path_);
		char const* method = method_;
		std::size_t methodsize = std::strlen( method_);
		bool debug = false;

		if (methodsize > 4 && std::memcmp( method, "DBG_", 4) == 0)
		{
			debug = true;
			method = method + 4;
		}
		if (!setResultContentType( answer, papuga_UTF8, WebRequestContent::HTML)) return false;

		ObjectDescr selector;
		if (!selector.init( m_handler->impl(), path, answer)) return false;

		// Call object method with content if object defined and method defined, else fallback to next:
		if (selector.obj && selector.obj->valuetype == papuga_TypeHostObject)
		{
			int classid = selector.obj->value.hostObject->classid;
			void* self = selector.obj->value.hostObject->data;
			const papuga_RequestMethodDescription* methoddescr = papuga_RequestHandler_get_method( m_handler->impl(), classid, method);
			if (methoddescr)
			{
				if (debug)
				{
					setAnswer( answer, ErrorCodeRequestResolveError);
					return false;
					//... debug requests only available on request schemes
				}
				else
				{
					return callHostObjMethod( self, methoddescr, path.getRest(), content, answer);
				}
			}
		}
		// Toplevel introspection if object not defined:
		if (!selector.contextnam)
		{
			//... No object selected, handle LIST or GET for all
			if (debug)
			{
				setAnswer( answer, ErrorCodeRequestResolveError);
				return false;
			}
			if (!content.empty())
			{
				setAnswer( answer, ErrorCodeInvalidArgument);
				return false;
			}
			if (0==std::strcmp( method,"LIST"))
			{
				if (!selector.typenam)
				{
					char const** typelist = papuga_RequestHandler_list_context_types( m_handler->impl(), selector.lstbuf, selector.lstbufsize);
					if (!checkPapugaListBufferOverflow( typelist, answer)) return false;
					if (!checkPapugaListEmpty( typelist, answer)) return false;
					return strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), "list", "elem", m_result_encoding, m_result_doctype, typelist);
				}
				else
				{
					char const** contextlist = papuga_RequestHandler_list_contexts( m_handler->impl(), selector.typenam, selector.lstbuf, selector.lstbufsize);
					if (!checkPapugaListBufferOverflow( contextlist, answer)) return false;
					if (!checkPapugaListEmpty( contextlist, answer)) return false;
					return strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), "list", selector.typenam, m_result_encoding, m_result_doctype, contextlist);
				}
				
			}
			else if (0==std::strcmp( method, "GET"))
			{
				papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( &m_allocator);
				if (!ser)
				{
					setAnswer( answer, ErrorCodeOutOfMem);
					return false;
				}
				papuga_ValueVariant result;
				papuga_init_ValueVariant_serialization( &result, ser);
		
				if (!selector.typenam)
				{
					if (!dumpViewAll( ser, answer)) return false;
					return mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), "view", "elem", m_result_encoding, m_result_doctype, result);
				}
				else
				{
					if (!dumpViewType( selector.typenam, ser, answer)) return false;
					return mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), "view", selector.typenam, m_result_encoding, m_result_doctype, result);
				}
			}
			else
			{
				setAnswer( answer, ErrorCodeRequestResolveError);
				return false;
			}
		}
		else if (!selector.context)
		{
			// Object selected does not exist, we do a configuration request:
			if (path.getRest()[0] != '\0')
			{
				if (0==std::strcmp(method,"PUT"))
				{
					//PF:HACK: Loading configuration is not const, but thread safe, so a const_cast is, though never good, Ok here:
					if (!const_cast<WebRequestHandler*>( m_handler)->loadConfiguration( selector.typenam, selector.contextnam, true/*store for reload*/, content, answer))
					{
						return false;
					}
					return true;
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
		else
		{
			// Scheme execution else:
			const char* contextType = path.getNext();
			const char* contextName = path.getNext();

			return executeContextScheme( contextType, contextName, method, content, answer);
		}
	}
	catch (const std::bad_alloc&)
	{
		setAnswer( answer, ErrorCodeOutOfMem);
		return false;
	}
	catch (...)
	{
		setAnswer( answer, ErrorCodeUncaughtException);
		return false;
	}
}

