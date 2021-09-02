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
		&m_attributes, http_accept_, m_handler->html_head(), html_base_href_,
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

bool WebRequestContext::execute(
		const WebRequestContent& content)
{
	bool rt = true;
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
					papuga_SchemaSource const* schema = papuga_SchemaList_get( m_handler->schemaList(), m_contextName);
					if (schema)
					{
						return mapStringToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
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
					return mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
									"schema"/*rootname*/, "name"/*itemname*/, 
									papuga_UTF8, papuga_http_default_doctype( &m_attributes),
									m_attributes.beautifiedOutput, lst);
				}
			}
			else if (!m_contextType)
			{
				char const* const* tplist = papuga_RequestContextPool_list_types( m_handler->contextPool(), &m_allocator);
				return mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
								"class"/*rootname*/, "name"/*itemname*/,
								papuga_UTF8, papuga_http_default_doctype( &m_attributes),
								m_attributes.beautifiedOutput, tplist);
			}
			else if (!m_contextName)
			{
				char const* const* tplist = papuga_RequestContextPool_list_names( m_handler->contextPool(), m_contextType, &m_allocator);
				return mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
								"object"/*rootname*/, "name"/*itemname*/, 
								papuga_UTF8, papuga_http_default_doctype( &m_attributes),
								m_attributes.beautifiedOutput, tplist);
			}
		}
		if (!m_contextName)
		{
			setAnswer( ErrorCodeRequestResolveError);
			return false;
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
		return rt;
	}
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( false);
}

WebRequestAnswer WebRequestContext::getAnswer() const
{
	return m_answer;
}

