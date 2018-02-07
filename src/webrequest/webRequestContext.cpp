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
#include "strus/base/string_format.hpp"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/requestParser.h"
#include "papuga/requestHandler.h"
#include "papuga/requestResult.h"
#include "papuga/valueVariant.h"
#include "papuga/encoding.h"
#include "private/internationalization.hpp"
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <iostream>

using namespace strus;

#define STRUS_LOWLEVEL_DEBUG

WebRequestContext::WebRequestContext(
	papuga_RequestHandler* handlerimpl,
	const char* context,
	const char* schema,
	const char* role)
		:m_handler(handlerimpl),m_request(0),m_encoding(papuga_Binary),m_doctype(papuga_ContentType_Unknown),m_errcode(papuga_Ok),m_atm(0),m_resultstr(0),m_resultlen(0)
{
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));
	if (!papuga_init_RequestContext_child( &m_impl, handlerimpl, context, role, &m_errcode))
	{
		throw Exception( m_errcode, "failed to instantiate context '%s' for '%s'", context?context:"", role?role:"*");
	}
	if (m_impl.schemaprefix)
	{
		char namespace_schema[ 256];
		if (sizeof(namespace_schema) <= std::snprintf( namespace_schema, sizeof(namespace_schema), "%s_%s", m_impl.schemaprefix, schema))
		{
			m_errcode = papuga_BufferOverflowError;
			throw Exception( m_errcode, "failed to get schema %s '%s'", m_impl.schemaprefix, schema);
		}
		m_atm = papuga_RequestHandler_get_schema( handlerimpl, namespace_schema, role, &m_errcode);
	}
	else
	{
		m_atm = papuga_RequestHandler_get_schema( handlerimpl, schema, role, &m_errcode);
	}
	if (!m_atm)
	{
		papuga_destroy_RequestContext( &m_impl);
		throw Exception( m_errcode, "failed to get schema %s '%s' for '%s'", m_impl.schemaprefix?m_impl.schemaprefix:"", schema, role?role:"*");
	}
	m_request = papuga_create_Request( m_atm);
	if (!m_request)
	{
		papuga_destroy_RequestContext( &m_impl);
		throw Exception( papuga_NoMemError, "failed to create request structure '%s' on '%s' for '%s'", schema, context, role?role:"*");
	}
}

WebRequestContext::~WebRequestContext()
{
	papuga_destroy_RequestContext( &m_impl);
	papuga_destroy_Request( m_request);
	if (m_resultstr) std::free( m_resultstr);
}

WebRequestContext::Exception::Exception( papuga_ErrorCode errcode_, const char* fmt_, ...)
	:std::runtime_error("WebRequestContextException"),m_errcode(errcode_)
{
	va_list vl;
	va_start(vl,fmt_);
	std::vsnprintf( m_errmsg, sizeof(m_errmsg), fmt_, vl);
	va_end(vl);
}

void WebRequestContext::clearContent()
{
	if (m_resultstr) std::free( m_resultstr);
	m_resultstr = NULL;
	m_resultlen = 0;
}

bool WebRequestContext::addVariable( const std::string& name, const std::string& value)
{
	papuga_ValueVariant vv;
	papuga_init_ValueVariant_string( &vv, value.c_str(), value.size());
	if (!papuga_RequestContext_add_variable( &m_impl, name.c_str(), &vv))
	{
		m_errcode = papuga_RequestContext_last_error( &m_impl);
		return false;
	}
	return true;
}

static papuga_StringEncoding getStringEncoding( const char* encoding, const char* content, std::size_t contentlen)
{
	papuga_StringEncoding rt;
	if (encoding)
	{
		if (!papuga_getStringEncodingFromName( &rt, encoding))
		{
			return papuga_Binary;
		}
		return rt;
	}
	else
	{
		return papuga_guess_StringEncoding( content, contentlen);
	}
}

static void setStatus( WebRequestAnswer& status, ErrorOperation operation, ErrorCause cause, const char* errstr, bool doCopy=false)
{
	int httpstatus = errorCauseToHttpStatus( cause);
	status.setError( httpstatus, *ErrorCode( StrusComponentWebService, operation, cause), errstr);
}

static void setStatus( WebRequestAnswer& status, int apperrorcode, const char* errstr)
{
	ErrorCode err( apperrorcode);
	int httpstatus = errorCauseToHttpStatus( err.cause());
	status.setError( httpstatus, apperrorcode, errstr);
}

bool WebRequestContext::feedRequest( WebRequestAnswer& status, const char* doctype, const char* encoding, const char* content, std::size_t contentlen)
{
#ifdef STRUS_LOWLEVEL_DEBUG
	std::cerr << "call WebRequestContext::feedRequest" << std::endl;
#endif
	clearContent();

	// Evaluate the character set encoding:
	m_encoding = getStringEncoding( encoding, content, contentlen);
	if (m_encoding == papuga_Binary)
	{
		setStatus( status, ErrorOperationScanInput, ErrorCauseEncoding, papuga_ErrorCode_tostring( m_errcode = papuga_NotImplemented));
		return false;
	}
	// Evaluate the request content type:
	m_doctype = doctype ? papuga_contentTypeFromName( doctype) : papuga_guess_ContentType( content,contentlen);
	if (m_doctype == papuga_ContentType_Unknown)
	{
		setStatus( status, ErrorOperationScanInput, ErrorCauseInputFormat, papuga_ErrorCode_tostring( m_errcode = papuga_NotImplemented));
		return false;
	}
#ifdef STRUS_LOWLEVEL_DEBUG
	std::cerr << "create the request parser" << std::endl;
#endif
	// Parse the request:
	papuga_RequestParser* parser = papuga_create_RequestParser( m_doctype, m_encoding, content, contentlen, &m_errcode);
	if (!parser)
	{
		setStatus( status, ErrorOperationScanInput, papugaErrorToErrorCause( m_errcode), papuga_ErrorCode_tostring( m_errcode));
		return false;
	}
#ifdef STRUS_LOWLEVEL_DEBUG
	std::cerr << "parse the request" << std::endl;
#endif
	if (!papuga_RequestParser_feed_request( parser, m_request, &m_errcode))
	{
		char buf[ 2048];
		int pos = papuga_RequestParser_get_position( parser, buf, sizeof(buf));
		papuga_ErrorBuffer_reportError( &m_errbuf, _TXT( "error at position %d: %s, feeding request, location: %s"), pos, papuga_ErrorCode_tostring( m_errcode), buf);
		papuga_destroy_RequestParser( parser);

		setStatus( status, ErrorOperationScanInput, papugaErrorToErrorCause( m_errcode), papuga_ErrorBuffer_lastError( &m_errbuf));
		return false;
	}
	papuga_destroy_RequestParser( parser);
	return true;
}

bool WebRequestContext::executeRequest( WebRequestAnswer& status, const char* content, std::size_t contentlen)
{
	int errpos = -1;
	if (!papuga_RequestContext_execute_request( &m_impl, m_request, &m_errbuf, &errpos))
	{
		m_errcode = papuga_HostObjectError;
		const char* errmsg = papuga_ErrorBuffer_lastError(&m_errbuf);

#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << strus::string_format( "execute request failed, pos %d: %s", errpos, errmsg) << std::endl;
#endif
		int lastapperr = 0;
		if (errmsg)
		{
			// Exract last error code and remove error codes from app error message:
			char const* errmsgitr = errmsg;
			int apperr = strus::errorCodeFromMessage( errmsgitr);
			lastapperr = apperr;
			if (0 <= apperr) while (0 <= (apperr = strus::errorCodeFromMessage( errmsgitr)))
			{
				if (apperr > 0) lastapperr = apperr;
			}
			// Add position info (scope of error in input) to error message, if available:
			if (errpos >= 0)
			{
				// Evaluate more info about the location of the error, we append the scope of the document to the error message:
				char locinfobuf[ 4096];
				const char* locinfo = papuga_request_error_location( m_doctype, m_encoding, content, contentlen, errpos, locinfobuf, sizeof(locinfobuf));
				if (locinfo)
				{
					papuga_ErrorBuffer_appendMessage( &m_errbuf, " (error scope: %s)", locinfo);
					errmsg = papuga_ErrorBuffer_lastError(&m_errbuf);
				}
			}
			if (lastapperr)
			{
				char* errmsgptr = papuga_ErrorBuffer_lastError(&m_errbuf);
				removeErrorCodesFromMessage( errmsgptr);
				errmsg = errmsgptr;
			}
		}
		else
		{
			errmsg = papuga_ErrorCode_tostring( m_errcode);
		}
		if (lastapperr >= 0)
		{
			setStatus( status, lastapperr, errmsg);
		}
		else
		{
			setStatus( status, ErrorOperationCallIndirection, papugaErrorToErrorCause( m_errcode), errmsg);
		}
		return false;
	}
	return true;
}

bool WebRequestContext::getRequestResult( WebRequestAnswer& status)
{
	papuga_RequestResult result;
	if (!papuga_set_RequestResult( &result, &m_impl, m_request))
	{
		m_errcode = papuga_NoMemError;
		setStatus( status, ErrorOperationBuildData, papugaErrorToErrorCause( m_errcode), papuga_ErrorCode_tostring( m_errcode));
		return false;
	}
	// Map the result:
	switch (m_doctype)
	{
		case papuga_ContentType_XML:  m_resultstr = (char*)papuga_RequestResult_toxml( &result, m_encoding, &m_resultlen, &m_errcode); break;
		case papuga_ContentType_JSON: m_resultstr = (char*)papuga_RequestResult_tojson( &result, m_encoding, &m_resultlen, &m_errcode); break;
		case papuga_ContentType_Unknown:
		default: break;
	}
	if (m_resultstr)
	{
		status.setContent( m_resultstr, m_resultlen);
		status.setStatus( 200);
		return true;
	}
	else
	{
		setStatus( status, ErrorOperationBuildData, papugaErrorToErrorCause( m_errcode), papuga_ErrorCode_tostring( m_errcode));
		return false;
	}
	return true;
}

bool WebRequestContext::execute( const char* doctype, const char* encoding, const char* content, std::size_t contentlen, WebRequestAnswer& status)
{
#ifdef STRUS_LOWLEVEL_DEBUG
	std::cerr << "call WebRequestContext::execute" << std::endl;
#endif
	return feedRequest( status, doctype, encoding, content, contentlen)
	&&	executeRequest( status, content, contentlen)
	&&	getRequestResult( status);
}

bool WebRequestContext::executeConfig( const char* destContextName, const char* destContextSchemaPrefix, const char* doctype, const char* encoding, const char* content, std::size_t contentlen, WebRequestAnswer& status)
{
#ifdef STRUS_LOWLEVEL_DEBUG
	std::cerr << "call WebRequestContext::executeConfig" << std::endl;
#endif
	return feedRequest( status, doctype, encoding, content, contentlen)
	&&	executeRequest( status, content, contentlen)
	&&	addToHandler( status, destContextName, destContextSchemaPrefix);
}

static void escJsonOutput( char* buf, std::size_t buflen)
{
	char* si = buf;
	char* se = buf + buflen;
	for (; si != se; ++si)
	{
		if (*si == '\\')
		{
			*si = ' ';
		}
		if (*si == '\"')
		{
			*si = '\'';
		}
		else if (*si == '\1')
		{
			*si = '\"';
		}
	}
}

bool WebRequestContext::mapError( char* buf, std::size_t bufsize, std::size_t& len, const WebRequestAnswer& answer)
{
	ErrorCode appErrorCode( answer.apperror());
	std::size_t bufpos = 0;
	switch (m_doctype)
	{
		case papuga_ContentType_XML:
			bufpos = std::snprintf(
					buf, bufsize, "<result type='error'><err id=\"%d\"><status>%d</status><comp>%s</comp><op>%d</op><code>%d</code><msg>%s</msg><err></result>",
					*appErrorCode,
					answer.httpstatus(),
					strus::errorComponentName( appErrorCode.component()),
					(int)appErrorCode.operation(),
					(int)appErrorCode.cause(),
					answer.errorstr());
			if (bufpos >= bufsize) return false;

		case papuga_ContentType_JSON:
			bufpos = std::snprintf(
					buf, bufsize, "{\1err\1: {\n\t\1id\1:%d\n\t\1status\1:%d\n\t\1comp\1:%s\n\t\1op\1:%d\n\t\1code\1:%d\n\t\1msg\1:\1%s\1\n}}\n",
					*appErrorCode,
					answer.httpstatus(),
					strus::errorComponentName( appErrorCode.component()),
					(int)appErrorCode.operation(),
					(int)appErrorCode.cause(),
					answer.errorstr());
			if (bufpos >= bufsize) return false;
			escJsonOutput( buf, bufpos);

		case papuga_ContentType_Unknown:
			bufpos = std::snprintf( buf, bufsize, "%d", *appErrorCode);
			if (bufpos >= bufsize) return false;
	}
	if (m_encoding == papuga_UTF8)
	{
		len = bufpos;
	}
	else
	{
		papuga_ValueVariant res;
		papuga_init_ValueVariant_string( &res, buf, bufpos);
		std::size_t usize = papuga_StringEncoding_unit_size( m_encoding);
		if (!usize) return false;
		for (; bufpos % usize != 0; ++bufpos){}
		if (bufpos >= bufsize) return false;
		std::size_t outlen;
		if (!papuga_ValueVariant_tostring_enc( &res, m_encoding, buf+bufpos, bufsize-bufpos, &outlen, &m_errcode)) return false;
		outlen *= usize;
		std::memmove( buf, buf+bufpos, outlen);
		len = outlen;
	}
	return true;
}

bool WebRequestContext::addToHandler( WebRequestAnswer& status, const char* contextName, const char* schemaPrefix)
{
	if (!papuga_RequestContext_set_schemaprefix( &m_impl, schemaPrefix))
	{
		m_errcode = papuga_NoMemError;
		setStatus( status, ErrorOperationBuildData, papugaErrorToErrorCause( m_errcode), papuga_ErrorCode_tostring( m_errcode));
		return false;
	}
	if (!papuga_RequestHandler_add_context( m_handler, contextName, &m_impl, &m_errcode))
	{
		char buf[ 1024];
		std::snprintf( buf, sizeof(buf), _TXT("error adding web request context %s '%s' to handler: %s"), schemaPrefix, contextName, papuga_ErrorCode_tostring( m_errcode));
		setStatus( status, ErrorOperationBuildData, papugaErrorToErrorCause( m_errcode), buf, true);
		return false;
	}
	return true;
}

