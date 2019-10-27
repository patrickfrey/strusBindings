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

#define WEBREQUEST_CONTEXT_CATCH_ERROR_SET_BOOL( errflag) \
	catch (const std::bad_alloc&) \
	{\
		setAnswer( ErrorCodeOutOfMem);\
		errflag = false;\
	}\
	catch (const std::runtime_error& err)\
	{\
		setAnswer( ErrorCodeRuntimeError, err.what(), true/*do copy*/);\
		errflag = false;\
	}\
	catch (...)\
	{\
		setAnswer( ErrorCodeUncaughtException);\
		errflag = false;\
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
	,m_logMask(logger_->logMask())
	,m_configHandler(configHandler_),m_configTransaction()
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_requestType(configRequestType(contextType_))
	,m_contextType(0),m_contextName(0)
	,m_context(),m_obj(0),m_request(0),m_methodId(Method_Undefined),m_path()
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
	,m_logMask(logger_->logMask())
	,m_configHandler(configHandler_),m_configTransaction()
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_requestType(UndefinedRequest)
	,m_contextType(0),m_contextName(0)
	,m_context(),m_obj(0),m_request(0),m_methodId(method_?methodIdFromName(method_):Method_GET),m_path(path_)
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

	if (m_methodId == Method_OPTIONS)
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
	,m_logMask(logger_->logMask())
	,m_configHandler(configHandler_),m_configTransaction()
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_requestType(requestType_)
	,m_contextType(contextType_),m_contextName(contextName_)
	,m_context(context_),m_obj(0),m_request(0),m_methodId(Method_Undefined),m_path()
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

enum MethodId {
	Method_Undefined,
	Method_GET,
	Method_PUT,
	Method_POST,
	Method_PATCH,
	Method_DELETE,
	Method_OPTIONS,
	Method_HEAD
};
static const char* g_methodNameAr[] = {NULL, "GET", "PUT", "POST", "PATCH", "DELETE", "OPTIONS", "HEAD", 0};

const char* WebRequestContext::methodIdName( const MethodId& m)
{
	return g_methodNameAr[ m];
}
WebRequestContext::MethodId WebRequestContext::methodIdFromName( const char* methodname)
{
	int ai = 1;
	for (; g_methodNameAr[ai] && 0!=std::strcmp( methodname, g_methodNameAr[ai]); ++ai){}
	return g_methodNameAr[ai] ? (MethodId)(ai) : Method_Undefined;
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
			if (m_methodId == Method_PUT)
			{
				if (m_transactionRef.get())
				{
					if (content.empty())
					{
						// [1.A] PUT transaction:
						return executeCommitTransaction();
					}
					//... else fallback to [3] or [4]
				}
				else
				{
					if (content.empty())
					{
						setAnswer( ErrorCodeIncompleteRequest);
						return false;
					}
					else if (m_obj)
					{
						// [1.B.1] UPDATE of configuration:
						return updateConfigurationRequest( content);
					}
					else
					{
						// [1.B.2] PUT of configuration:
						return loadConfigurationRequest( content);
					}
				}
			}
			else if (m_methodId == Method_PATCH)
			{
			}
			else if (m_methodId == Method_POST)
			{
				if (content.empty())
				{
					setAnswer( ErrorCodeIncompleteRequest);
					return false;
				}
				else
				{
					// [1.D] POST of configuration, name defined by client
					// [NOTE] This is not conforming to REST, the REST interface is defined by [1.C]
					return (loadConfigurationRequest( content) && setAnswerLink( m_contextType, m_contextName, 1/*link level*/));
				}
			}
			else if (m_methodId == Method_DELETE)
			{
				if (content.empty())
				{
					if (m_transactionRef.get())
					{
						// [1.E] DELETE of a transaction object:
						m_transactionRef.reset();
						m_answer.setHttpStatus( 204/*no content*/);
						return true;//... transaction automatically released when not returned
					}
					else
					{
						// [1.F] DELETE of a configuration object:
						return deleteConfigurationRequest();
					}
				}
				else
				{
					setAnswer( ErrorCodeInvalidRequest);
					//... DELETE with content invalid
					return false;
				}
			}
			// else fallback to [3] or [4]
		}
		else if (content.empty())
		{
			if (m_methodId == Method_GET)
			{
				bool beautified = m_handler->beautifiedOutput();
				// [2] Top level introspection without context defined:
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
						return strus::mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, beautified, contextlist);
					}
				}
				else
				{
					std::vector<std::string> contextTypes = m_configHandler->contextTypes();
					contextTypes.push_back( ROOT_CONTEXT_NAME);
					return strus::mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, beautified, contextTypes);
				}
			}
			else
			{
				setAnswer( ErrorCodeRequestResolveError);
				return false;
			}
		}
		else if (m_methodId == Method_POST)
		{
			// [1.C] POST of configuration without name specified (name allocated by service):
			bool rt = true;
			std::string newContextName = m_handler->allocTemporaryContextName( m_contextType, ""/*prefix*/);
			try
			{
				m_contextName = papuga_Allocator_copy_string( &m_allocator, newContextName.c_str(), newContextName.size());
				rt = loadConfigurationRequest( content);
				if (rt)
				{
					m_answer.setHttpStatus( 201/*created*/);
					rt = setAnswerLink( m_contextType, newContextName, 1/*link level*/);
				}
			}
			WEBREQUEST_CONTEXT_CATCH_ERROR_SET_BOOL( rt);
			if (!rt) m_handler->releaseTemporaryContextName( m_contextType, newContextName);
			return rt;
		}
	}
	if (m_obj)
	{
		if (m_obj->valuetype == papuga_TypeHostObject)
		{
			if (m_methodId == Method_POST && isEqual( m_path.rest(),"transaction"))
			{
				// [3.A] Call POST transaction (a method), e.g. create a new transaction
				return executePostTransaction();
			}
			else
			{
				// [3.B] Call object method if defined or fallback to next:
				int classid = m_obj->value.hostObject->classid;
				void* self = m_obj->value.hostObject->data;

				const papuga_RequestMethodDescription* methoddescr = papuga_RequestHandler_get_method( m_handler->impl(), classid, methodIdName(m_methodId), !content.empty());
				if (methoddescr)
				{
					return callHostObjMethodToAnswer( self, methoddescr, m_path.getRest(), content);
				}
				//... fallback to [4]
			}
		}
		else if (m_methodId == Method_GET)
		{
			// [3.C] Map the addressed structure that is not a host object:
			bool beautified = m_handler->beautifiedOutput();
			return mapValueVariantToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, "value", m_result_encoding, m_result_doctype, beautified, *m_obj);
		}
	}
	if (m_context.get())
	{
		if (!content.empty())
		{
			// [4.A] Execute schema in context with content:
			return initContentSchemaAutomaton( getSchemaId()) && executeContentSchemaAutomaton( content);
		}
		else if (m_methodId == Method_GET)
		{
			// [4.B] List variables when no main object with introspection method defined (section [3.B]):
			return executeListVariables();
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
			case InterruptedLoadConfigurationRequest:
				return updateConfigurationRequest_retry( content);
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
		if (m_logger && (m_logMask & WebRequestLoggerInterface::LogAction) != 0)
		{
			m_logger->logAction( m_contextType, m_contextName, "put delegate answer");
		}
		if (!m_answer.ok())
		{
			if (0!=(m_logMask & WebRequestLoggerInterface::LogWarning))
			{
				m_logger->logWarning( _TXT( "ignoring delegate request answer because of previous error"));
			}
			return true;
		}
		strus::Reference<WebRequestContext> delegateContext( createClone( ObjectRequest));
		delegateContext->initContentType( content);
		bool rt = delegateContext->initContentSchemaAutomaton( SchemaId( m_contextType, schema))
			&& delegateContext->executeContentSchemaAutomaton( content);
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
				case InterruptedLoadConfigurationRequest:
					rt &= getContentRequestResult();
					if (rt && m_configTransaction.defined())
					{
						rt &= transferContext();
					}
					break;
			}
			if (rt && m_logger)
			{
				if ((m_logMask & WebRequestLoggerInterface::LogAction) != 0)
				{
					m_logger->logAction( m_contextType, m_contextName, "request complete");
				}
				if ((m_logMask & WebRequestLoggerInterface::LogRequests) != 0)
				{
					m_logger->logRequestAnswer( m_answer.content().str(), m_answer.content().len());
				}
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

