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

static void setAnswer( WebRequestAnswer& answer, ErrorOperation operation, ErrorCause cause, const char* errstr=0, bool doCopy=false)
{
	int httpstatus = errorCauseToHttpStatus( cause);
	answer.setError( httpstatus, *ErrorCode( StrusComponentWebService, operation, cause), errstr?errstr : strus::errorCauseMessage(cause));
}

static void setAnswer( WebRequestAnswer& answer, int apperrorcode, const char* errstr=0)
{
	ErrorCode err( apperrorcode);
	int httpstatus = errorCauseToHttpStatus( err.cause());
	answer.setError( httpstatus, apperrorcode, errstr?errstr : strus::errorCauseMessage(err.cause()));
}

bool WebRequestContext::feedContentRequest( WebRequestAnswer& answer, const WebRequestContent& content)
{
	if (content.len() == 0)
	{
		setAnswer( answer, ErrorOperationScanInput, ErrorCauseIncompleteRequest, _TXT("request content is empty"));
		return false;
	}
	// Evaluate the character set encoding:
	if (content.charset()[0] == '\0')
	{
		setAnswer( answer, ErrorOperationScanInput, ErrorCauseNotImplemented, _TXT("charset field in content type is empty. HTTP 1.1 standard character set ISO-8859-1 not implemented"));
		/// ... according to https://www.w3.org/International/articles/http-charset/index we should use "ISO-8859-1" if not defined, currently not available
		return false;
	}
	m_encoding = strus::getStringEncoding( content.charset(), content.str(), content.len());
	if (m_encoding == papuga_Binary)
	{
		setAnswer( answer, ErrorOperationScanInput, ErrorCauseNotImplemented);
		return false;
	}
	// Evaluate the request content type:
	m_doctype = content.doctype() ? papuga_contentTypeFromName( content.doctype()) : papuga_guess_ContentType( content.str(), content.len());
	if (m_doctype == papuga_ContentType_Unknown)
	{
		setAnswer( answer, ErrorOperationScanInput, ErrorCauseInputFormat);
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
		setAnswer( answer, ErrorOperationScanInput, papugaErrorToErrorCause( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	if (!papuga_RequestParser_feed_request( parser, m_request, &errcode))
	{
		char buf[ 2048];
		int pos = papuga_RequestParser_get_position( parser, buf, sizeof(buf));
		papuga_ErrorBuffer_reportError( &m_errbuf, _TXT( "error at position %d: %s, feeding request, location: %s"), pos, papuga_ErrorCode_tostring( errcode), buf);
		papuga_destroy_RequestParser( parser);

		setAnswer( answer, ErrorOperationScanInput, papugaErrorToErrorCause( errcode), papuga_ErrorBuffer_lastError( &m_errbuf));
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
		setAnswer( answer, ErrorOperationBuildResult, ErrorCauseNotImplemented, _TXT("none of the accept charsets implemented"));
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
		setAnswer( answer, ErrorOperationBuildResult, papugaErrorToErrorCause( errcode), papuga_ErrorCode_tostring( errcode));
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
			setAnswer( answer, apperr, errmsg);
		}
		else
		{
			setAnswer( answer, ErrorOperationCallIndirection, papugaErrorToErrorCause( errcode));
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
		setAnswer( answer, ErrorOperationBuildResult, ErrorCauseNotImplemented, _TXT("none of the accept charsets implemented"));
		return false;
	}
	if (m_result_doctype == WebRequestContent::Unknown)
	{
		setAnswer( answer, ErrorOperationBuildResult, ErrorCauseNotImplemented, _TXT("none of the accept content types implemented"));
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
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
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
			setAnswer( answer, ErrorOperationBuildResult, ErrorCauseNotImplemented, _TXT("output content type unknown"));
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
		setAnswer( answer, ErrorOperationBuildResult, papugaErrorToErrorCause( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	return true;
}

bool WebRequestContext::initContentRequest( WebRequestAnswer& answer, const char* contextType, const char* contextName, const char* scheme)
{
	papuga_ErrorCode errcode = papuga_Ok;
	if (!papuga_init_RequestContext_child( &m_context, &m_allocator, m_handler->impl(), contextType, contextName, &errcode))
	{
		setAnswer( answer, ErrorOperationBuildData, papugaErrorToErrorCause( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	m_atm = papuga_RequestHandler_get_scheme( m_handler->impl(), m_context.type, scheme, &errcode);
	if (!m_atm)
	{
		setAnswer( answer, ErrorOperationBuildData, papugaErrorToErrorCause( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	if (m_request) papuga_destroy_Request( m_request);
	m_request = papuga_create_Request( m_atm);
	if (!m_request)
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
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

static bool getListMethod( papuga_RequestMethodId& mid, int classid)
{
	switch (classid)
	{
		case STRUS_BINDINGS_CLASSID_Context:		mid = strus::bindings::method::Context::introspectionDir(); return true;
		case STRUS_BINDINGS_CLASSID_StorageClient:	mid = strus::bindings::method::StorageClient::introspectionDir(); return true;
		default:					return false;
	}
}

static bool getViewMethod( papuga_RequestMethodId& mid, int classid)
{
	switch (classid)
	{
		case STRUS_BINDINGS_CLASSID_Context:		mid = strus::bindings::method::Context::introspection(); return true;
		case STRUS_BINDINGS_CLASSID_StorageClient:	mid = strus::bindings::method::StorageClient::introspection(); return true;
		default:					return false;
	}
}

bool WebRequestContext::callMethod( void* self, const papuga_RequestMethodId& mid, const char* path_, papuga_ValueVariant& result, WebRequestAnswer& answer)
{
	const papuga_ClassDef* cdeflist = getBindingsClassDefs();
	const papuga_ClassDef* cdef = &cdeflist[mid.classid-1];
	if (mid.functionid == 0)
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseNotAllowed);
		return false;
	}
	papuga_ClassMethod method = cdef->methodtable[ mid.functionid-1];
	papuga_CallResult retval;
	char membuf_err[ 4096];
	papuga_ValueVariant argv;
	papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( &m_allocator);
	if (!ser)
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
		return false;
	}
	papuga_init_ValueVariant_serialization( &argv, ser);
	PathBuf path( path_);
	const char* pathelem;
	while (!!(pathelem = path.getNext()))
	{
		if (!papuga_Serialization_pushValue_charp( ser, pathelem))
		{
			setAnswer( answer, ErrorOperationCallIndirection, ErrorCauseOutOfMem);
			return false;
		}
	}
	papuga_init_CallResult( &retval, &m_allocator, true, membuf_err, sizeof(membuf_err));
	if (!(*method)( self, &retval, 1/*argc*/, &argv))
	{
		char* errstr = papuga_CallResult_lastError( &retval);
		char const* msgitr = errstr;
		int apperr = strus::errorCodeFromMessage( msgitr);
		if (apperr) strus::removeErrorCodesFromMessage( errstr);

		setAnswer( answer, apperr, errstr);
		return false;
	}
	if (retval.nofvalues == 0)
	{
		setAnswer( answer, ErrorOperationCallIndirection, ErrorCauseIncompleteResult);
		return false;
	}
	if (retval.nofvalues > 1)
	{
		setAnswer( answer, ErrorOperationCallIndirection, ErrorCauseRuntimeError, _TXT("only one result expected"));
		return false;
	}
	papuga_init_ValueVariant_value( &result, &retval.valuear[0]);
	return true;
}

bool WebRequestContext::callListMethod( const papuga_ValueVariant* obj, const char* path, WebRequestAnswer& answer)
{
	if (obj->valuetype == papuga_TypeHostObject)
	{
		papuga_RequestMethodId mid;
		if (!getListMethod( mid, obj->value.hostObject->classid))
		{
			setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
			return false;
		}
		void* self = obj->value.hostObject->data;
		papuga_ValueVariant result;
		if (!callMethod( self, mid, path, result, answer)) return false;
		return mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), "list", "elem", m_result_encoding, m_result_doctype, result);
	}
	else
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
		return false;
	}
}

bool WebRequestContext::callViewMethod( const papuga_ValueVariant* obj, const char* path, papuga_ValueVariant& result, WebRequestAnswer& answer)
{
	if (obj->valuetype == papuga_TypeHostObject)
	{
		papuga_RequestMethodId mid;
		if (!getViewMethod( mid, obj->value.hostObject->classid))
		{
			setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
			return false;
		}
		void* self = obj->value.hostObject->data;
		if (!callMethod( self, mid, path, result, answer)) return false;
	}
	else if (*path == '\0')
	{
		papuga_init_ValueVariant_value( &result, obj);
	}
	else
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
		return false;
	}
	return true;
}

bool WebRequestContext::callViewMethod( const papuga_ValueVariant* obj, const char* path, WebRequestAnswer& answer)
{
	papuga_ValueVariant result; 
	if (!callViewMethod( obj, path, result, answer)) return false;
	return mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), "view", "elem", m_result_encoding, m_result_doctype, result);
}

static bool checkPapugaListBufferOverflow( const char** ci, WebRequestAnswer& answer)
{
	if (!ci)
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseBufferOverflow);
		return false;
	}
	return true;
}
static bool checkPapugaListEmpty( const char** ci, WebRequestAnswer& answer)
{
	if (!*ci)
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
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
	if (!rt) setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
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
	if (!rt) setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
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
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
		return false;
	}
	else
	{
		char const** varlist = papuga_RequestContext_list_variables( context, 0/*max inheritcnt*/, lstbuf, lstbufsize);
		if (!checkPapugaListBufferOverflow( varlist, answer)) return false;
		if (varlist[0] && !varlist[1] && 0==std::strcmp( *varlist, typenam))
		{
			if (!dumpViewVar( context, *varlist, ser, answer)) return false;
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
	if (!rt) setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
	return rt;
}

bool WebRequestContext::dumpViewVar( const papuga_RequestContext* context, const char* varnam, papuga_Serialization* ser, WebRequestAnswer& answer)
{
	papuga_ValueVariant result;
	const papuga_ValueVariant* obj = papuga_RequestContext_get_variable( context, varnam);
	if (!obj)
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
		return false;
	}
	if (!callViewMethod( obj, "", result, answer)) return false;
	if (!papuga_Serialization_pushValue( ser, &result))
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
		return false;
	}
	return true;
}

bool WebRequestContext::executeList(
		const char* path_,
		WebRequestAnswer& answer)
{
	try
	{
		if (!setResultContentType( answer, papuga_UTF8, WebRequestContent::HTML)) return false;

		PathBuf path( path_);
		const papuga_RequestContext* context;
		const papuga_ValueVariant* obj;

		enum {lstbufsize=256};
		char const* lstbuf[ lstbufsize];
		const char* typenam;
		const char* contextnam;
		const char* varnam;

		if (!(typenam = path.getNext()))
		{
			char const** typelist = papuga_RequestHandler_list_context_types( m_handler->impl(), lstbuf, lstbufsize);
			if (!checkPapugaListBufferOverflow( typelist, answer)) return false;
			if (!checkPapugaListEmpty( typelist, answer)) return false;
			return strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), "list", "elem", m_result_encoding, m_result_doctype, typelist);
		}
		if (!(contextnam = path.getNext()))
		{
			char const** contextlist = papuga_RequestHandler_list_contexts( m_handler->impl(), typenam, lstbuf, lstbufsize);
			if (!checkPapugaListBufferOverflow( contextlist, answer)) return false;
			if (!checkPapugaListEmpty( contextlist, answer)) return false;
			return strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), "list", typenam, m_result_encoding, m_result_doctype, contextlist);
		}
		if (std::strcmp( contextnam, "scheme") == 0)
		{
			const char** schemelist = papuga_RequestHandler_list_schemes( m_handler->impl(), typenam, lstbuf, lstbufsize);
			if (!checkPapugaListBufferOverflow( schemelist, answer)) return false;
			return strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), "list", "scheme", m_result_encoding, m_result_doctype, schemelist);
		}
		context = papuga_RequestHandler_find_context( m_handler->impl(), typenam, contextnam);
		if (context == NULL)
		{
			setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
			return false;
		}
		char const** varlist = papuga_RequestContext_list_variables( context, 0/*max inheritcnt*/, lstbuf, lstbufsize);
		if (!checkPapugaListBufferOverflow( varlist, answer)) return false;
		if (varlist[0] && !varlist[1] && 0==std::strcmp( *varlist, typenam))
		{
			obj = papuga_RequestContext_get_variable( context, typenam);
		}
		else if (!(varnam = path.getNext()))
		{
			if (!checkPapugaListEmpty( varlist, answer)) return false;
			return strus::mapStringArrayToAnswer( answer, &m_allocator, m_handler->html_head(), "list", contextnam, m_result_encoding, m_result_doctype, varlist);
		}
		else
		{
			obj = papuga_RequestContext_get_variable( context, varnam);
		}
		if (!obj)
		{
			setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
			return false;
		}
		return callListMethod( obj, path.getRest(), answer);
	}
	catch (...)
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
		return false;
	}
}

bool WebRequestContext::executeView(
		const char* path_,
		WebRequestAnswer& answer)
{
	try
	{
		if (!setResultContentType( answer, papuga_UTF8, WebRequestContent::HTML)) return false;

		PathBuf path( path_);
		const papuga_RequestContext* context;
		const papuga_ValueVariant* obj;

		enum {lstbufsize=256};
		char const* lstbuf[ lstbufsize];
		const char* typenam;
		const char* contextnam;
		const char* varnam;

		papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( &m_allocator);
		if (!ser)
		{
			setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
			return false;
		}
		papuga_ValueVariant result;
		papuga_init_ValueVariant_serialization( &result, ser);

		if (!(typenam = path.getNext()))
		{
			if (!dumpViewAll( ser, answer)) return false;
			return mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), "view", "elem", m_result_encoding, m_result_doctype, result);
		}
		else if (!(contextnam = path.getNext()))
		{
			if (!dumpViewType( typenam, ser, answer)) return false;
			return mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), "view", typenam, m_result_encoding, m_result_doctype, result);
		}
		context = papuga_RequestHandler_find_context( m_handler->impl(), typenam, contextnam);
		if (context == NULL)
		{
			setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
			return false;
		}
		char const** varlist = papuga_RequestContext_list_variables( context, 0/*max inheritcnt*/, lstbuf, lstbufsize);
		if (!checkPapugaListBufferOverflow( varlist, answer)) return false;
		if (varlist[0] && !varlist[1] && 0==std::strcmp( *varlist, typenam))
		{
			obj = papuga_RequestContext_get_variable( context, typenam);
		}
		else if (!(varnam = path.getNext()))
		{
			if (!dumpViewName( typenam, contextnam, ser, answer)) return false;
			return mapValueVariantToAnswer( answer, &m_allocator, m_handler->html_head(), "view", contextnam, m_result_encoding, m_result_doctype, result);
		}
		else
		{
			obj = papuga_RequestContext_get_variable( context, varnam);
		}
		if (!obj)
		{
			setAnswer( answer, ErrorOperationBuildData, ErrorCauseRequestResolveError);
			return false;
		}
		return callViewMethod( obj, path.getRest(), answer);
	}
	catch (...)
	{
		setAnswer( answer, ErrorOperationBuildData, ErrorCauseOutOfMem);
		return false;
	}
}

bool WebRequestContext::executeContent( const char* contextType, const char* contextName, const char* scheme, const WebRequestContent& content, WebRequestAnswer& answer)
{
	return initContentRequest( answer, contextType, contextName, scheme)
	&&	feedContentRequest( answer, content)
	&&	executeContentRequest( answer, content)
	&&	getContentRequestResult( answer);
}

bool WebRequestContext::debugContent( const char* contextType, const char* contextName, const char* scheme, const WebRequestContent& content, WebRequestAnswer& answer)
{
	return initContentRequest( answer, contextType, contextName, scheme)
	&&	feedContentRequest( answer, content)
	&&	debugContentRequest( answer);
}


