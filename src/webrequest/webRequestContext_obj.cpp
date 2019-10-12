/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, initialization, managing and deinitialization of handler and context objects in the request
/// \file "webRequestContext_result.cpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/base/string_format.hpp"
#include "papugaLoggerFunctions.hpp"
#include "schemas_base.hpp"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/typedefs.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

static inline bool isEqual( const char* name, const char* oth)
{
	return name[0] == oth[0] && 0==std::strcmp(name,oth);
}

void WebRequestContext::initCallLogger()
{
	std::memset( &m_callLogger, 0, sizeof(m_callLogger));
	m_callLogger.self = m_logger;
	if (0!=(m_logMask & (int)WebRequestLoggerInterface::LogMethodCalls)) m_callLogger.logMethodCall = &papugaLogMethodCall;
	if (0!=(m_logMask & (int)WebRequestLoggerInterface::LogContentEvents)) m_callLogger.logContentEvent = &papugaLogContentEvent;
}

void WebRequestContext::resetRequestObject()
{
	m_transactionRef.reset();
	m_context.reset();
	m_obj = 0;
}

bool WebRequestContext::initEmptyObject()
{
	m_transactionRef.reset();
	m_context.create();
	m_obj = 0;
	if (!m_context.get())
	{
		setAnswer( ErrorCodeOutOfMem);
		return false;
	}
	return true;
}

bool WebRequestContext::initRootObject()
{
	if (!initEmptyObject())
	{
		return false;
	}
	if (!papuga_RequestContext_inherit( m_context.get(), m_handler->impl(), ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME))
	{
		papuga_ErrorCode errcode = papuga_RequestContext_last_error( m_context.get(), true);
		setAnswer( papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}

bool WebRequestContext::initSchemaDescriptionObject()
{
	if (!(m_contextType = m_path.getNext())) return true;
	if (isEqual( m_contextType, ROOT_CONTEXT_NAME))
	{
		m_contextName = ROOT_CONTEXT_NAME;
		return false;
	}
	else
	{
		return !!(m_contextName = m_path.getNext());
	}
}

bool WebRequestContext::initRequestObject()
{
	resetRequestObject();
	papuga_ErrorCode errcode = papuga_Ok;
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];

	if (m_path.startsWith( "transaction", 11/*"transaction"*/))
	{
		// Fetch transaction object from pool with exclusive ownership:
		m_contextType = m_path.getNext();
		m_contextName = m_path.getNext();
		if (!m_contextName)
		{
			setAnswer( ErrorCodeIncompleteRequest);
			return false;
		}
		m_transactionRef = m_transactionPool->fetchTransaction( m_contextName);
		if (!m_transactionRef.get())
		{
			setAnswer( ErrorCodeRequestResolveError);
			return false;
		}
		m_contextType = m_transactionRef->contextType();
		m_context = m_transactionRef->context();
		m_obj = papuga_RequestContext_get_variable( m_context.get(), "transaction");
		if (!m_obj)
		{
			setAnswer( ErrorCodeRequestResolveError);
			return false;
		}
		return true;
	}
	else
	{
		// Find addressed object and create clone of it:
		if (!(m_contextType = m_path.getNext())) return true;
		if (isEqual( m_contextType, ROOT_CONTEXT_NAME))
		{
			m_contextName = ROOT_CONTEXT_NAME;
		}
		else
		{
			if (!(m_contextName = m_path.getNext())) return true;
		}
		m_context.create();
		if (!m_context.get())
		{
			setAnswer( ErrorCodeOutOfMem);
			return false;
		}
		if (!papuga_RequestContext_inherit( m_context.get(), m_handler->impl(), m_contextType, m_contextName))
		{
			errcode = papuga_RequestContext_last_error( m_context.get(), true);
			resetRequestObject();
			if (errcode == papuga_AddressedItemNotFound)
			{
				return true;
			}
			else
			{
				setAnswer( papugaErrorToErrorCode( errcode));
				return false;
			}
		}
		char const** varlist = papuga_RequestContext_list_variables( m_context.get(), 1/*max inheritcnt*/, lstbuf, lstbufsize);
		if (!varlist)
		{
			setAnswer( ErrorCodeBufferOverflow);
			return false;
		}
		if (m_path.hasMore() && !m_path.startsWith( m_contextType, std::strlen(m_contextType)))
		{
			char const** vi = varlist;
			for (; *vi; ++vi)
			{
				if (m_path.startsWith( *vi, std::strlen(*vi)))
				{
					const char* varnam = m_path.getNext();
					if (!varnam)
					{
						resetRequestObject();
						setAnswer( papugaErrorToErrorCode( papuga_LogicError));
						return false;
					}
					m_obj = papuga_RequestContext_get_variable( m_context.get(), varnam);
					return true;
				}
			}
		}
		m_obj = papuga_RequestContext_get_variable( m_context.get(), m_contextType);
		return true;
	}
	return true;
}

bool WebRequestContext::inheritRequestContext( const char* contextType_, const char* contextName_)
{
	papuga_ErrorCode errcode = papuga_Ok;
	if (!m_context.get())
	{
		errcode = papuga_ExecutionOrder;
		goto ERROR;
	}
	if (!papuga_RequestContext_inherit( m_context.get(), m_handler->impl(), contextType_, contextName_))
	{
		errcode = papuga_RequestContext_last_error( m_context.get(), true);
		goto ERROR;
	}
	return true;
ERROR:
	if (errcode == papuga_AddressedItemNotFound)
	{
		m_answer.setError_fmt( errorCodeToHttpStatus( papugaErrorToErrorCode( errcode)), papugaErrorToErrorCode( errcode), _TXT("undefined %s '%s'"), contextType_, contextName_);
	}
	else
	{
		m_answer.setError_fmt( errorCodeToHttpStatus( papugaErrorToErrorCode( errcode)), papugaErrorToErrorCode( errcode), _TXT("failed to inherit from %s '%s'"), contextType_, contextName_);
	}
	return false;
}

bool WebRequestContext::initContentType( const WebRequestContent& content)
{
	// Set the request character set encoding:
	if (content.charset()[0] == '\0')
	{
		setAnswer( ErrorCodeNotImplemented, _TXT("charset field in content type is empty. HTTP 1.1 standard character set ISO-8859-1 not implemented"));
		/// ... according to https://www.w3.org/International/articles/http-charset/index we should use "ISO-8859-1" if not defined, currently not available
		return false;
	}
	m_encoding = strus::getStringEncoding( content.charset(), content.str(), content.len());
	if (m_encoding == papuga_Binary)
	{
		setAnswer( ErrorCodeNotImplemented);
		return false;
	}
	// Set the request content type:
	m_doctype = content.doctype() ? papuga_contentTypeFromName( content.doctype()) : papuga_guess_ContentType( content.str(), content.len());
	if (m_doctype == papuga_ContentType_Unknown)
	{
		setAnswer( ErrorCodeInputFormat);
		return false;
	}
	m_doctypestr = content.doctype();

	// Set the result content encoding:
	if (m_encoding == papuga_Binary)
	{
		m_result_encoding = strus::getResultStringEncoding( m_accepted_charset, defaultEncoding());
	}
	else
	{
		m_result_encoding = strus::getResultStringEncoding( m_accepted_charset, m_encoding);
	}
	if (m_result_encoding == papuga_Binary)
	{
		int httpstatus = errorCodeToHttpStatus( ErrorCodeNotImplemented);
		m_answer.setError_fmt( httpstatus, ErrorCodeNotImplemented, _TXT("none of the accept charsets implemented: %s"), m_accepted_charset);
		return false;
	}
	// Set the result document type:
	if (m_doctype == papuga_ContentType_Unknown)
	{
		m_result_doctype = strus::getResultContentType( m_accepted_doctype, defaultDocType());
	}
	else
	{
		m_result_doctype = strus::getResultContentType( m_accepted_doctype, papugaTranslatedContentType(m_doctype));
	}
	if (m_result_doctype == WebRequestContent::Unknown)
	{
		int httpstatus = errorCodeToHttpStatus( ErrorCodeNotImplemented);
		m_answer.setError_fmt( httpstatus, ErrorCodeNotImplemented, _TXT("none of the accept content types implemented: %s"), m_accepted_doctype);
		return false;
	}
	return true;
}

bool WebRequestContext::executePostTransaction()
{
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "transaction", "post", m_contextType, m_contextName);
	}
	if (m_obj->valuetype != papuga_TypeHostObject)
	{
		setAnswer( ErrorCodeLogicError);
		return false;
	}
	int classid = m_obj->value.hostObject->classid;
	void* self = m_obj->value.hostObject->data;

	const char* method = "POST/transaction";
	const char* resultname = "transaction";
	const papuga_RequestMethodDescription* methoddescr = papuga_RequestHandler_get_method( m_handler->impl(), classid, method, false);
	if (!methoddescr)
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
	if (!callHostObjMethodToVariable( self, methoddescr, m_context, resultname))
	{
		return false;
	}
	std::string transaction_typenam = strus::string_format( "transaction/%s", m_contextType);
	std::string tid = m_transactionPool->createTransaction( transaction_typenam, m_context, m_handler->maxIdleTime());
	std::string linkbase;
	int ec = strus::getAncestorPath( m_html_base_href, 3, linkbase);
	std::string tlinkparent = strus::joinFilePath( linkbase, "transaction");
	std::string tlink = strus::joinFilePath( tlinkparent, tid);
	if (ec)
	{
		setAnswer( ErrorCode( ec), _TXT("failed to get link base"));
		return false;
	}
	if (tid.empty() || tlinkparent.empty() || tlink.empty())
	{
		setAnswer( ErrorCodeOutOfMem);
		return false;
	}
	bool beautified = m_handler->beautifiedOutput();
	return strus::mapStringToAnswer( m_answer, &m_allocator, m_handler->html_head(), ""/*html href base*/, "transaction", PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, beautified, tlink);
}

bool WebRequestContext::executeCommitTransaction()
{
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "transaction", "commit", m_contextType, m_contextName);
	}
	if (m_obj && m_obj->valuetype == papuga_TypeHostObject)
	{
		int classid = m_obj->value.hostObject->classid;
		void* self = m_obj->value.hostObject->data;

		const papuga_RequestMethodDescription* methoddescr
			= papuga_RequestHandler_get_method(
				m_handler->impl(), classid, "PUT/transaction"/*method*/, false/*has content*/);
		if (methoddescr)
		{
			WebRequestContent content;
			if (!callHostObjMethodToAnswer( self, methoddescr, ""/*path*/, content))
			{
				m_transactionRef.reset();
				return false;
			}
			m_transactionRef.reset();
			return true;
		}
		else
		{
			setAnswer( ErrorCodeRequestResolveError);
			return false;
		}
	}
	else
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
}


