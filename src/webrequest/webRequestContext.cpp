/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of context for executing XML/JSON requests on the strus bindings
/// \file "webRequestContext.cpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "schemas_base.hpp"
#include "strus/errorCodes.hpp"
#include "strus/lib/error.hpp"
#include "strus/base/fileio.hpp"
#include "private/internationalization.hpp"

using namespace strus;

static std::string parentPath( const std::string& url)
{
	std::string rt;
	int ec = strus::getParentPath( url, rt);
	if (ec) throw strus::runtime_error_ec( ErrorCodeRequestResolveError, _TXT("illegal URL"));
	return rt;
}

static inline bool isEqual( const char* name, const char* oth)
{
	return name[0] == oth[0] && 0==std::strcmp(name,oth);
}

#define WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( errorReturnValue) \
	catch (const std::bad_alloc&) \
	{\
		setAnswer( ErrorCodeOutOfMem);\
		return errorReturnValue;\
	}\
	catch (const std::runtime_error& err)\
	{\
		setAnswer( ErrorCodeRuntimeError, err.what(), true/*do copy*/);\
		return errorReturnValue;\
	}\
	catch (...)\
	{\
		setAnswer( ErrorCodeUncaughtException);\
		return errorReturnValue;\
	}


WebRequestContext* createClone();

WebRequestContext::WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		ConfigurationHandler* configHandler_,
		TransactionPool* transactionPool_,
		const char* contextType_,
		const char* contextName_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_configHandler(configHandler_),m_configTransaction()
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_requestType(configRequestType(contextType_))
	,m_contextType(0),m_contextName(0)
	,m_context(),m_obj(0),m_request(0),m_method(0),m_path()
	,m_encoding(papuga_UTF8),m_doctype(papuga_ContentType_JSON),m_doctypestr(0)
	,m_atm(0)
	,m_result_encoding(papuga_UTF8),m_result_doctype(WebRequestContent::JSON)
	,m_results(0),m_nofResults(0),m_resultIdx(0)
	,m_errbuf(),m_answer()
	,m_accepted_charset(0),m_accepted_doctype(0),m_html_base_href()
{
	initCallLogger();
	papuga_init_Allocator( &m_allocator, m_allocator_mem, sizeof(m_allocator_mem));
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));

	m_contextType = contextType_ ? papuga_Allocator_copy_charp( &m_allocator, contextType_) : ROOT_CONTEXT_NAME;
	m_contextName = contextName_ ? papuga_Allocator_copy_charp( &m_allocator, contextName_) : ROOT_CONTEXT_NAME;
	if (!m_contextType || !m_contextName) throw std::bad_alloc();
}

WebRequestContext::WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		ConfigurationHandler* configHandler_,
		TransactionPool* transactionPool_,
		const char* accepted_charset_,
		const char* accepted_doctype_,
		const char* html_base_href_,
		const char* method_,
		const char* path_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_configHandler(configHandler_),m_configTransaction()
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_requestType(UndefinedRequest)
	,m_contextType(0),m_contextName(0)
	,m_context(),m_obj(0),m_request(0),m_method(method_),m_path(path_)
	,m_encoding(papuga_Binary),m_doctype(papuga_ContentType_Unknown),m_doctypestr(0)
	,m_atm(0)
	,m_result_encoding(papuga_Binary),m_result_doctype(WebRequestContent::Unknown)
	,m_results(0),m_nofResults(0),m_resultIdx(0)
	,m_errbuf(),m_answer()
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
	initCallLogger();
	papuga_init_Allocator( &m_allocator, m_allocator_mem, sizeof(m_allocator_mem));
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));

	if (!m_method)
	{
		m_method = "GET";
		m_requestType = ObjectRequest;
	}
	else if (isEqual( m_method, "OPTIONS"))
	{
		m_requestType = MethodOptionsRequest;
	}
	else if (m_path.startsWith( "schema", 6/*"schema"*/))
	{
		(void)m_path.getNext();//... skip "schema"
		m_requestType = SchemaDescriptionRequest;
	}
	else
	{
		m_requestType = ObjectRequest;
	}
}

/// \brief Clone constructor
WebRequestContext::WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		ConfigurationHandler* configHandler_,
		TransactionPool* transactionPool_,
		const char* contextType_,
		const char* contextName_,
		const PapugaContextRef& context_,
		const RequestType& requestType_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_configHandler(configHandler_),m_configTransaction()
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_requestType(requestType_)
	,m_contextType(contextType_),m_contextName(contextName_)
	,m_context(),m_obj(0),m_request(0),m_method(0),m_path()
	,m_encoding(papuga_Binary),m_doctype(papuga_ContentType_Unknown),m_doctypestr(0)
	,m_atm(0)
	,m_result_encoding(papuga_Binary),m_result_doctype(WebRequestContent::Unknown)
	,m_results(0),m_nofResults(0),m_resultIdx(0)
	,m_errbuf(),m_answer()
	,m_accepted_charset(""),m_accepted_doctype("")
	,m_html_base_href("")
{
	initCallLogger();
	papuga_init_Allocator( &m_allocator, m_allocator_mem, sizeof(m_allocator_mem));
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));
}

WebRequestContext::~WebRequestContext()
{
	m_transactionRef.reset();
	m_context.reset();
	if (m_request) papuga_destroy_Request( m_request);
	papuga_destroy_Allocator( &m_allocator);
}

WebRequestContext* WebRequestContext::createClone( const RequestType& requestType_) const
{
	return new WebRequestContext(
			m_handler, m_logger, m_configHandler, m_transactionPool, m_contextType, m_contextName,
			m_context, requestType_);
}

WebRequestContext::RequestType WebRequestContext::configRequestType( const char* contextType)
{
	return contextType == 0 || isEqual( contextType, ROOT_CONTEXT_NAME)
		? WebRequestContext::LoadMainConfiguration
		: WebRequestContext::LoadEmbeddedConfiguration;
}

bool WebRequestContext::executeObjectRequest( const WebRequestContent& content)
{
	if (!m_path.hasMore())
	{
		if (m_contextName)
		{
			if (isEqual( m_method,"PUT") || isEqual( m_method,"POST"))
			{
				// PUT or POST of configuration/transaction or fallback to next:
				if (m_transactionRef.get())
				{
					if (content.empty())
					{
						return executeCommitTransaction();
					}
					//... else fallback
				}
				else
				{
					if (content.empty())
					{
						setAnswer( ErrorCodeIncompleteRequest);
						return false;
					}
					else
					{
						return loadConfigurationRequest( content);
					}
				}
			}
			else if (isEqual( m_method,"DELETE") && content.empty())
			{
				// DELETE of configuration/transaction or fallback to next:
				if (m_transactionRef.get())
				{
					m_answer.setStatus( 204/*no content*/);
					return true;//... transaction automatically released when not returned
				}
				else
				{
					return deleteConfigurationRequest();
				}
			}
			else if (isEqual( m_method, "GET") && !m_obj && m_context.get())
			{
				// List variables if no main object defined but context exists
				if (!content.empty())
				{
					setAnswer( ErrorCodeInvalidArgument);
					return false;
				}
				if (!executeListVariables())
				{
					return false;
				}
				return false;
			}
			// else fallback
		}
		else if (content.empty())
		{
			// Top level introspection without context defined:
			if (isEqual( m_method, "GET"))
			{
				if (m_contextType)
				{
					std::vector<std::string> contextlist = m_configHandler->contextNames( m_contextType);
					if (contextlist.empty())
					{
						setAnswer( ErrorCodeRequestResolveError);
						return false;
					}
					else
					{
						return strus::mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, contextlist);
					}
				}
				else
				{
					std::vector<std::string> contextTypes = m_configHandler->contextTypes();
					contextTypes.push_back( ROOT_CONTEXT_NAME);
					return strus::mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, contextTypes);
				}
			}
			else
			{
				setAnswer( ErrorCodeRequestResolveError);
				return false;
			}
		}
	}
	if (m_obj)
	{
		// Call object method or POST transaction (also a method), fallback to next if not method defined:
		if (m_obj->valuetype == papuga_TypeHostObject)
		{
			if (isEqual( m_method,"POST") && isEqual( m_path.rest(),"transaction"))
			{
				return executePostTransaction();
			}
			else
			{
				int classid = m_obj->value.hostObject->classid;
				void* self = m_obj->value.hostObject->data;

				const papuga_RequestMethodDescription* methoddescr = papuga_RequestHandler_get_method( m_handler->impl(), classid, m_method, !content.empty());
				if (methoddescr)
				{
					return callHostObjMethod( self, methoddescr, m_path.rest(), content);
				}
				//... fallback
			}
		}
		else if (isEqual( m_method,"GET"))
		{
			return mapValueVariantToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, "value", m_result_encoding, m_result_doctype, *m_obj);
		}
	}
	if (m_context.get())
	{
		// Execute schema in context with content:
		if (!content.empty())
		{
			return executeContentSchemaRequest( getSchemaId(), content);
		}
		else
		{
			setAnswer( ErrorCodeIncompleteRequest);
			return false;
		}
	}
	else
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
}

bool WebRequestContext::execute(
		const WebRequestContent& content)
{
	try
	{
		switch (m_requestType)
		{
			case UndefinedRequest:
				setAnswer( ErrorCodeIncompleteRequest);
				return false;
			case SchemaDescriptionRequest:
				if (!initContentType( content)) return false;
				if (!initSchemaDescriptionObject()) return false;
				if (!content.empty())
				{
					setAnswer( ErrorCodeInvalidArgument);
					return false;
				}
				return executeSchemaDescription();
			case MethodOptionsRequest:
				if (!initContentType( content)) return false;
				if (!initRequestObject()) return false;
				if (!content.empty())
				{
					setAnswer( ErrorCodeInvalidArgument);
					return false;
				}
				return executeOPTIONS();
			case LoadMainConfiguration:
				return loadMainConfiguration( content);
			case LoadEmbeddedConfiguration:
				return loadEmbeddedConfiguration( content);
			case ObjectRequest:
				if (!initContentType( content)) return false;
				if (!initRequestObject()) return false;
				return executeObjectRequest( content);
		}
		setAnswer( ErrorCodeInvalidRequest);
		return false;
	}
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( false);
}

std::vector<WebRequestDelegateRequest> WebRequestContext::getDelegateRequests()
{
	try
	{
		std::vector<WebRequestDelegateRequest> rt;
		if (!getContentRequestDelegateRequests( rt))
		{
			return std::vector<WebRequestDelegateRequest>();
		}
		return rt;
	}
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( std::vector<WebRequestDelegateRequest>());
}

bool WebRequestContext::putDelegateRequestAnswer(
		const char* schema,
		const WebRequestContent& content)
{
	try
	{
		if (!m_answer.ok())
		{
			if (0!=(m_logger->logMask() & WebRequestLoggerInterface::LogWarning))
			{
				m_logger->logWarning( _TXT( "ignoring delegate request answer because of previous error"));
			}
			return true;
		}
		strus::Reference<WebRequestContext> delegateContext( createClone( ObjectRequest));
		delegateContext->initContentType( content);
		bool rt = executeContentSchemaRequest( SchemaId( m_contextType, schema), content);
		if (rt)
		{
			if (delegateContext->hasContentRequestDelegateRequests())
			{
				setAnswer( ErrorCodeInvalidOperation, _TXT("delegate requests not allowed in delegate request result schema definitions"));
				return false;
			}
			if (delegateContext->hasContentRequestResult())
			{
				setAnswer( ErrorCodeInvalidOperation, _TXT("results not expected in delegate request result schema definitions"));
				return false;
			}
			rt = delegateContext->complete();
		}
		if (!rt)
		{
			m_answer = delegateContext->getAnswer();
		}
		return rt;
	}
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( false);
}

bool WebRequestContext::complete()
{
	try
	{
		bool rt = true;
		if (m_answer.ok())
		{
			switch (m_requestType)
			{
				case UndefinedRequest:
					setAnswer( ErrorCodeIncompleteRequest);
					rt = false;
					break;
				case SchemaDescriptionRequest:
					rt &= getContentRequestResult();
					break;
				case MethodOptionsRequest:
					rt &= getContentRequestResult();
					break;
				case LoadMainConfiguration:
				case LoadEmbeddedConfiguration:
					rt &= transferContext();
					break;
				case ObjectRequest:
					rt &= getContentRequestResult();
					if (rt && m_configTransaction.defined())
					{
						rt &= transferContext();
					}
					break;
			}
		}
		else
		{
			rt = false;
		}
		if (m_transactionRef.get())
		{
			m_transactionPool->returnTransaction( m_transactionRef);
		}
		return rt;
	}
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( false);
}

WebRequestAnswer WebRequestContext::getAnswer() const
{
	return m_answer;
}

