/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, functions for accesing the result or answer of the request
/// \file "webRequestContext_result.cpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/errorCodes.hpp"
#include "strus/lib/error.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "schemas_base.hpp"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/typedefs.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

void WebRequestContext::setAnswer( int errcode, const char* errstr, bool doCopy)
{
	int httpstatus = errorCodeToHttpStatus( (ErrorCode)errcode);
	if (errstr)
	{
		m_answer.setError( httpstatus, errcode, errstr, doCopy);
	}
	else
	{
		m_answer.setError( httpstatus, errcode, strus::errorCodeToString(errcode));
	}
}

static const char* getDelegateRequestString( papuga_Allocator* allocator, papuga_RequestResult* result, std::size_t& resultlen, const papuga_Request* request, papuga_ErrorCode& errcode)
{
	papuga_ValueVariant resultval;
	if (papuga_Serialization_empty( &result->serialization))
	{
		resultlen = 0;
		return "";
	}
	else
	{
		papuga_init_ValueVariant_serialization( &resultval, &result->serialization);
		const papuga_StructInterfaceDescription* structdefs = papuga_Request_struct_descriptions( request);

		return (const char*)papuga_ValueVariant_tojson( &resultval, allocator, structdefs, papuga_UTF8, result->name, NULL/*no array possible*/, &resultlen, &errcode);
	}
}

static const char* getDelegateRequestUrl( papuga_Allocator* allocator, const papuga_ValueVariant* adressval, char const* path, papuga_ErrorCode* errcode)
{
	if (papuga_ValueVariant_isstring( adressval))
	{
		while (*path == '/') {++path;}
		std::size_t pathlen = path ? std::strlen( path) : 0;
		std::size_t urlbuflen = adressval->length * 4 + pathlen + 16;
		char* urlbuf = (char*)papuga_Allocator_alloc( allocator, urlbuflen, 1/*align*/);
		if (!urlbuf)
		{
			*errcode = papuga_NoMemError;
			return NULL;
		}
		std::size_t urllen;
		if (!papuga_ValueVariant_tostring_enc( adressval, papuga_UTF8, urlbuf, urlbuflen, &urllen, errcode))
		{
			return NULL;
		}
		while (urllen > 0 && urlbuf[ urllen-1] == '/') {--urllen;}
		if (urllen == 0)
		{
			*errcode = papuga_TypeError;
			return NULL;
		}
		if (urllen + pathlen + 2 > urlbuflen)
		{
			*errcode = papuga_BufferOverflowError;
			return NULL;
		}
		urlbuf[ urllen] = '/';
		std::memcpy( urlbuf + urllen + 1, path, pathlen + 1);
		if (!papuga_Allocator_shrink_last_alloc( allocator, urlbuf, urlbuflen, urllen + pathlen + 2))
		{
			*errcode = papuga_LogicError;
			return NULL;
		}
		return urlbuf;
	}
	else
	{
		*errcode = papuga_TypeError;
		return NULL;
	}
}

static bool resultAppendContentVariableValues( papuga_RequestResult* result, papuga_RequestContext* ctx, papuga_ErrorCode& errcode)
{
	bool rt = true;

	char const* const* vi = result->contentvar;
	for (; *vi; ++vi)
	{
		const papuga_ValueVariant* contentvalue = papuga_RequestContext_get_variable( ctx, *vi);
		if (contentvalue)
		{
			char const* vnam = *vi;
			for (; vnam[0] == '_'; ++vnam){}
			papuga_Serialization_pushName_charp( &result->serialization, vnam);
			papuga_Serialization_pushValue( &result->serialization, contentvalue);
		}
	}
	if (!rt && errcode == papuga_Ok) errcode = papuga_NoMemError;
	return rt;
}

bool WebRequestContext::getContentRequestDelegateRequests( std::vector<WebRequestDelegateRequest>& delegateRequests)
{
	papuga_ErrorCode errcode = papuga_Ok;

	for (; m_resultIdx < m_nofResults; ++m_resultIdx)
	{
		papuga_RequestResult* result = m_results + m_resultIdx;
		if (!result->schema) continue;

		if (!resultAppendContentVariableValues( result, m_context.get(), errcode))
		{
			setAnswer( papugaErrorToErrorCode( errcode));
			return false;
		}
		std::size_t resultlen = 0;
		const char* resultstr = getDelegateRequestString( &m_allocator, result, resultlen, m_request, errcode);
		if (resultstr)
		{
			const papuga_ValueVariant* addressval = papuga_RequestContext_get_variable( m_context.get(), result->addressvar);
			if (!addressval)
			{
				m_answer.setError_fmt( errorCodeToHttpStatus( ErrorCodeNotFound), ErrorCodeNotFound, _TXT("undefined variable '%s'"), result->addressvar);
				return false;
			}
			if (addressval->valuetype == papuga_TypeSerialization)
			{
				papuga_SerializationIter si;
				papuga_init_SerializationIter( &si, addressval->value.serialization);
				for (; !papuga_SerializationIter_eof( &si); papuga_SerializationIter_skip( &si))
				{
					if (papuga_SerializationIter_tag( &si) == papuga_TagValue)
					{
						const papuga_ValueVariant* addressvalelem = papuga_SerializationIter_value(&si);
						const char* url = getDelegateRequestUrl( &m_allocator, addressvalelem, result->path, &errcode);
						if (!url)
						{
							strus::ErrorCode ec = papugaErrorToErrorCode( errcode);
							m_answer.setError_fmt( errorCodeToHttpStatus(ec), ec, _TXT("error resolving url of delegate request from variable '%s'"), result->addressvar);
							return false;
						}
						delegateRequests.push_back( WebRequestDelegateRequest( result->requestmethod, url, result->schema, resultstr, resultlen));
					}
					else
					{
						m_answer.setError_fmt( errorCodeToHttpStatus( ErrorCodeSyntax), ErrorCodeSyntax, _TXT("list of delegate request service URLs expected for variable '%s'"), result->addressvar);
						return false;
					}
				}
			}
			else
			{
				const char* url = getDelegateRequestUrl( &m_allocator, addressval, result->path, &errcode);
				if (!url)
				{
					strus::ErrorCode ec = papugaErrorToErrorCode( errcode);
					m_answer.setError_fmt( errorCodeToHttpStatus(ec), ec, _TXT("error resolving url of delegate request from variable '%s'"), result->addressvar);
					return false;
				}
				delegateRequests.push_back( WebRequestDelegateRequest( result->requestmethod, url, result->schema, resultstr, resultlen));
			}
			++m_resultIdx;
			return true;
		}
		else
		{
			setAnswer( papugaErrorToErrorCode( errcode));
			return false;
		}
	}
	return true;
}

bool WebRequestContext::getContentRequestResult()
{
	papuga_ErrorCode errcode = papuga_Ok;

	if (!m_context.get()) return true;
	int ri = 0, re = m_nofResults;
	for (; ri != re; ++ri)
	{
		papuga_RequestResult* result = m_results + ri;
		if (result->schema) continue;

		if (!resultAppendContentVariableValues( result, m_context.get(), errcode))
		{
			setAnswer( papugaErrorToErrorCode( errcode));
			return false;
		}
		std::size_t resultlen = 0;
		const char* resultstr = getResultString( result, resultlen, errcode);
		if (resultstr)
		{
			const char* encname = papuga_stringEncodingName( m_result_encoding);
			WebRequestContent resultContent( encname, WebRequestContent::typeMime(m_result_doctype), resultstr, resultlen);
			if (m_answer.content().empty())
			{
				m_answer.setContent( resultContent);
			}
			else
			{
				setAnswer( ErrorCodeSyntax, _TXT("duplicate definition of result"));
				return false;
			}
			return true;
		}
		else
		{
			setAnswer( papugaErrorToErrorCode( errcode));
			return false;
		}
	}
	return true;
}

bool WebRequestContext::hasContentRequestDelegateRequests() const
{
	int ri = 0, re = m_nofResults;
	for (; ri != re && !m_results[ri].schema; ++ri)
	{}
	return ri != re;
}

bool WebRequestContext::hasContentRequestResult() const
{
	int ri = 0, re = m_nofResults;
	for (; ri != re && m_results[ri].schema; ++ri)
	{}
	return ri != re;
}

bool WebRequestContext::transferContext()
{
	if (m_context.refcnt() != 1)
	{
		setAnswer( ErrorCodeLogicError, _TXT("transferred configuration object not singular (referenced twice)"));
		return false;
	}
	if (!m_handler->transferContext( m_contextType, m_contextName, m_context.release(), m_answer))
	{
		return false;
	}
	if (m_configTransaction.defined())
	{
		m_configHandler->commitStoreConfiguration( m_configTransaction);
		m_answer.setStatus( 204/*no content*/);
	}
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogAction))
	{
		m_logger->logAction( m_contextType, m_contextName, m_configTransaction.defined()?_TXT("put configuration"):_TXT("loaded embedded configuration"));
	}
	return true;
}



