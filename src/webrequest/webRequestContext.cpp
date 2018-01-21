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
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/requestParser.h"
#include "papuga/requestHandler.h"
#include "papuga/requestResult.h"
#include "papuga/valueVariant.h"
#include "papuga/encoding.h"
#include "private/internationalization.hpp"
#include <cstddef>

using namespace strus;

WebRequestContext::WebRequestContext(
	papuga_RequestHandler* handlerimpl,
	const char* schema,
	const char* role)
{
	m_errcode = papuga_Ok;
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));
	m_atm = papuga_RequestHandler_get_schema( handlerimpl, schema, role, &m_errcode);
	if (!m_atm) throw Exception( m_errcode);
	if (!papuga_init_RequestContext_child( &m_impl, handlerimpl, schema, role, &m_errcode)) throw Exception( m_errcode);
	m_request = papuga_create_Request( m_atm);
	if (!m_request)
	{
		papuga_destroy_RequestContext( &m_impl);
		throw Exception( papuga_NoMemError);
	}
	m_encoding = papuga_Binary;
	m_doctype = papuga_ContentType_Unknown;
	m_resultstr = NULL;
	m_resultlen = 0;
}

WebRequestContext::~WebRequestContext()
{
	papuga_destroy_RequestContext( &m_impl);
	papuga_destroy_Request( m_request);
	if (m_resultstr) std::free( m_resultstr);
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

WebRequestAnswer WebRequestContext::execute( const char* doctype, const char* encoding, const char* content, std::size_t contentlen)
{
	int errpos = -1;
	if (m_resultstr) std::free( m_resultstr);
	m_resultstr = NULL;
	m_resultlen = 0;

	// Evaluate the character set encoding:
	if (encoding)
	{
		if (!papuga_getStringEncodingFromName( &m_encoding, encoding))
		{
			m_errcode = papuga_TypeError;
			return WebRequestAnswer( papuga_ErrorCode_tostring( m_errcode), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
		}
	}
	else
	{
		m_encoding = papuga_guess_StringEncoding( content, contentlen);
		if (m_encoding == papuga_Binary)
		{
			m_errcode = papuga_TypeError;
			return WebRequestAnswer( papuga_ErrorCode_tostring( m_errcode), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
		}
	}
	// Evaluate the request content type:
	m_doctype = doctype ? papuga_contentTypeFromName( doctype) : papuga_guess_ContentType( content, contentlen);
	if (m_doctype == papuga_ContentType_Unknown)
	{
		m_errcode = papuga_TypeError;
		return WebRequestAnswer( papuga_ErrorCode_tostring( m_errcode), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
	}
	// Parse the request:
	papuga_RequestParser* parser = papuga_create_RequestParser( m_doctype, m_encoding, content, contentlen, &m_errcode);
	if (!parser)
	{
		return WebRequestAnswer( papuga_ErrorCode_tostring( m_errcode), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
	}

	if (!papuga_RequestParser_feed_request( parser, m_request, &m_errcode))
	{
		char buf[ 2048];
		int pos = papuga_RequestParser_get_position( parser, buf, sizeof(buf));
		papuga_ErrorBuffer_reportError( &m_errbuf, _TXT( "error at position %d: %s, feeding request, location: %s"), pos, papuga_ErrorCode_tostring( m_errcode), buf);
		papuga_destroy_RequestParser( parser);
		return WebRequestAnswer( papuga_ErrorBuffer_lastError( &m_errbuf), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
	}
	papuga_destroy_RequestParser( parser);

	// Execute the request:
	if (!papuga_RequestContext_execute_request( &m_impl, m_request, &m_errbuf, &errpos))
	{
		m_errcode = papuga_HostObjectError;
		if (papuga_ErrorBuffer_lastError(&m_errbuf))
		{
			if (errpos >= 0)
			{
				// Evaluate more info about the location of the error, we append the scope of the document to the error message:
				char locinfobuf[ 4096];
				const char* locinfo = papuga_request_error_location( m_doctype, m_encoding, content, contentlen, errpos, locinfobuf, sizeof(locinfobuf));
				if (locinfo)
				{
					papuga_ErrorBuffer_appendMessage( &m_errbuf, " (error scope: %s)", locinfo);
				}
			}
			return WebRequestAnswer( papuga_ErrorBuffer_lastError( &m_errbuf), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
		}
		else
		{
			return WebRequestAnswer( papuga_ErrorCode_tostring( m_errcode), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
		}
	}
	// Get the result:
	papuga_RequestResult result;
	if (!papuga_set_RequestResult( &result, &m_impl, m_request))
	{
		m_errcode = papuga_NoMemError;
		return WebRequestAnswer( papuga_ErrorCode_tostring( m_errcode), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
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
		return WebRequestAnswer( 0, 200, m_resultstr, m_resultlen);
	}
	else
	{
		return WebRequestAnswer( papuga_ErrorCode_tostring( m_errcode), WebRequestHandler::papugaErrorToHttpStatus( m_errcode), 0, 0);
	}
}



