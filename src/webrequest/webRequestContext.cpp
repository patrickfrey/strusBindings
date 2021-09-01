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

static void assignRequestMethod( char* destbuf, size_t destbufsize, char const* ri) noexcept
{
	size_t di = 0;
	destbufsize -= 1;
	for (; di != destbufsize && *ri; ++di) {destbuf[ di] = *ri++ & ~32;}
	destbuf[ di] = 0;
}

static bool isEqual( const char* name, const char* oth)
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
		TransactionPool* transactionPool_,
		const char* http_accept_,
		const char* html_base_href_,
		const char* method_,
		const char* path_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_logMask(logger_->logMask())
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_contextType(nullptr),m_contextName(nullptr)
	,m_context(),m_path(path_)
	,m_errbuf(),m_answer()
{
	papuga_init_Allocator( &m_allocator, m_allocator_mem, sizeof(m_allocator_mem));
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));

	papuga_init_RequestAttributes(
		&m_attributes, http_accept_, html_base_href_,
		m_handler->beautifiedOutput(), false/*deterministicOutput*/);
	assignRequestMethod( m_requestMethod, sizeof(m_requestMethod), method_);
}

/// \brief Clone constructor
WebRequestContext::WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		TransactionPool* transactionPool_,
		const char* method_,
		const char* contextType_,
		const char* contextName_,
		const papuga_RequestAttributes& attributes_,
		const PapugaContextRef& context_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_logMask(logger_->logMask())
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_contextType(contextType_),m_contextName(contextName_)
	,m_context(context_),m_path()
	,m_errbuf(),m_answer()
{
	assignRequestMethod( m_requestMethod, sizeof(m_requestMethod), method_);
	papuga_init_Allocator( &m_allocator, m_allocator_mem, sizeof(m_allocator_mem));
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));
	m_contextType = papuga_Allocator_copy_charp( &m_allocator, contextType_);
	m_contextName = papuga_Allocator_copy_charp( &m_allocator, contextName_);
	papuga_copy_RequestAttributes( &m_allocator, &m_attributes, &attributes_);
}

WebRequestContext::~WebRequestContext()
{
	m_transactionRef.reset();
	m_context.reset();
	papuga_destroy_Allocator( &m_allocator);
}

WebRequestContext* WebRequestContext::createClone() const
{
	return new WebRequestContext(
			m_handler, m_logger, m_transactionPool,
			m_requestMethod, m_contextType, m_contextName,
			m_attributes, m_context);
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
				setAnswer( ErrorCodeNotImplemented);
				return false;
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
					return (loadConfigurationRequest( content) && setAnswerLink( m_contextType, m_contextName, 2/*link level*/));
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
						resetRequestObject();
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
	bool rt = true;
	papuga_ErrorCode errcode = papuga_Ok;
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];

	try
	{
		initRequestContext();
		if (isEqual( m_requestMethod,"OPTIONS"))
		{
			if (!m_contextType)
			{
				m_answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,GET", true);
				return true;
			}
			else if (0==std::strcmp( m_contextType, "schema"))
			{
				m_answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,GET", true);
				return true;
			}
			else
			{
				if (m_contextName)
				{
					for (auto script : m_handler->scripts())
					{
						if (0==std::strcmp( papuga_LuaRequestHandlerScript_name( script), m_contextName))
						{
							std::string options("OPTIONS,");
							options.append( papuga_LuaRequestHandlerScript_options( script));
							m_answer.setMessage( 200/*OK*/, "Allow", options.c_str(), true);
							return true;
						}
					}
					setAnswer( ErrorCodeRequestResolveError);
					return false;
				}
				else
				{
					m_answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,GET", true);
					return true;
				}
			}
		}
		else if (isEqual( m_requestMethod,"GET"))
		{
			if (0==std::strcmp( m_contextType, "schema"))
			{
				if (m_contextName)
				{
					papuga_Schema const* schema = papuga_SchemaMap_get( m_handler->schemaMap(), m_contextName);
					if (schema)
					{
						return mapStringToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_href_base,
									"schema"/*rootname*/, schema->name,
									papuga_UTF8, papuga_http_default_doctype( &m_attributes),
									m_attributes.beautifiedOutput, schema->source);
					}
					else
					{
						setAnswer( ErrorCodeRequestResolveError);
						return false;
					}
				}
				else
				{
					char const** lst = papuga_SchemaList_get_names( m_handler->schemaList(), lstbuf, lstbufsize);
					if (!lst)
					{
						setAnswer( ErrorCodeBufferOverflow);
						return false;
					}
					return mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_href_base,
									"schema"/*rootname*/, "name"/*itemname*/, 
									papuga_UTF8, papuga_http_default_doctype( &m_attributes),
									m_attributes.beautifiedOutput, lstbuf));
				}
			}
			else
			{
				!!!! execute script
			}
		}
		return rt;
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
		const WebRequestAnswer& answer)
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
		if (!answer.ok())
		{
			m_answer = answer;
			return false;
		}
		if (!answer.content().empty())
		{
			strus::Reference<WebRequestContext> delegateContext( createClone( ObjectRequest));
			delegateContext->initContentType( answer.content());
			bool rt = delegateContext->initContentSchemaAutomaton( SchemaId( m_contextType, schema))
				&& delegateContext->executeContentSchemaAutomaton( answer.content());
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
		return true;
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
		std::string infomsgs = fetchContextInfoMessages();
		if (!infomsgs.empty() && (m_logMask & WebRequestLoggerInterface::LogContextInfoMessages) != 0)
		{
			m_logger->logContextInfoMessages( infomsgs.c_str());
		}
		return rt;
	}
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( false);
}

WebRequestAnswer WebRequestContext::getAnswer() const
{
	return m_answer;
}

static int g_call_contextInfoMessages_paramtypes[1] = {0};
static papuga_RequestMethodDescription g_call_contextInfoMessages =
{
	strus::bindings::method::Context::infoMessagesDump(),
	g_call_contextInfoMessages_paramtypes,
	false/*has_content*/,
	200/*http status success*/,
	0/*result type name*/,
	0/*result_rootelem*/,0/*result_listelem*/
};

std::string WebRequestContext::fetchContextInfoMessages()
{
	std::string rt;
	void* self = 0;
	if (m_obj
		&& m_obj->valuetype == papuga_TypeHostObject
		&& m_obj->value.hostObject->classid == g_call_contextInfoMessages.id.classid)
	{
		self = m_obj->value.hostObject->data;
	}
	else if (m_context.get())
	{
		const papuga_ValueVariant* ctxobj = papuga_RequestContext_get_variable( m_context.get(), ROOT_CONTEXT_NAME);
		if (ctxobj
			&& ctxobj->valuetype == papuga_TypeHostObject
			&& ctxobj->value.hostObject->classid == g_call_contextInfoMessages.id.classid)
		{
			self = ctxobj->value.hostObject->data;
		}
	}
	if (self)
	{
		rt = callHostObjMethodToString( self, &g_call_contextInfoMessages, ""/*path*/, WebRequestContent());
	}
	return rt;
}

