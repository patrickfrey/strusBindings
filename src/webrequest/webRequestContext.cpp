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
#include "webRequestDelegateContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "configuration.hpp"
#include "strus/errorCodes.hpp"
#include "strus/lib/error.hpp"
#include "strus/lib/lua.h"
#include "strus/base/fileio.hpp"
#include "private/internationalization.hpp"
#include "papuga/valueVariant.h"
#include "papuga/constants.h"

using namespace strus;

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

void WebRequestContext::setAnswerFromException()
{
	try
	{
		throw;
	}
	catch (const std::bad_alloc&)
	{
		setAnswer( ErrorCodeOutOfMem);
	}
	catch (const std::runtime_error& err)
	{
		char const* errmsgitr = err.what();
		int apperr = strus::errorCodeFromMessage( errmsgitr);
		if (apperr)
		{
			setAnswer( apperr, errmsgitr, true/*do copy*/);
		}
		else
		{
			setAnswer( ErrorCodeRuntimeError, err.what(), true/*do copy*/);
		}
	}
	catch (...)
	{
		setAnswer( ErrorCodeUncaughtException);
	}
}

WebRequestContext::WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		TransactionPool* transactionPool_,
		const char* http_accept_,
		const char* html_base_href_,
		const char* method_,
		const char* path_,
		const char* contentstr_,
		size_t contentlen_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_logLevel(logger_->level())
	,m_transactionPool(transactionPool_),m_transactionRef(),m_configTransactionTmpFile()
	,m_contextType(nullptr),m_contextName(nullptr)
	,m_context(),m_luahandler(),m_path(path_)
	,m_errbuf(),m_answer()
{
	papuga_init_Allocator( &m_allocator, m_allocator_mem, sizeof(m_allocator_mem));
	papuga_init_ErrorBuffer( &m_errbuf, m_errbuf_mem, sizeof(m_errbuf_mem));

	papuga_init_RequestAttributes(
		&m_attributes, http_accept_, m_handler->html_head(), html_base_href_,
		m_handler->beautifiedOutput(), false/*deterministicOutput*/);
	assignRequestMethod( m_requestMethod, sizeof(m_requestMethod), method_);
	initRequestContext( contentstr_, contentlen_);
	if (!executeBuiltInCommand())
	{
		initLuaScript( contentstr_, contentlen_);
	}
}

WebRequestContext::~WebRequestContext()
{
	m_transactionRef.reset();
	m_context.reset();
	papuga_destroy_Allocator( &m_allocator);
}

static const char* createTransaction_( void* self, const char* type, papuga_RequestContext* context, papuga_Allocator* allocator)
{
	try
	{
		return ((WebRequestContext*)(self))->createTransaction( type, context, allocator);
	}
	catch (...)
	{
		return nullptr;
	}
}

const char* WebRequestContext::createTransaction( const char* type, papuga_RequestContext* context, papuga_Allocator* allocator)
{
	PapugaContextRef contextref( context);
	std::string tr = m_transactionPool->createTransaction( type, contextref, m_handler->maxIdleTime());
	const char* trstr = papuga_Allocator_copy_string( &m_allocator, tr.c_str(), tr.size());
	return trstr;
}

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

bool WebRequestContext::transferContext()
{
	if (m_context.use_count() != 1)
	{
		setAnswer( ErrorCodeLogicError, _TXT("transferred object not singular"));
		return false;
	}
	if (!m_handler->transferContext( m_contextType, m_contextName, m_context.release(), m_answer))
	{
		return false;
	}
	try
	{
		Configuration::commit( m_configTransactionTmpFile);
	}
	catch (...)
	{
		setAnswerFromException();
		return false;
	}
	return true;
}

bool WebRequestContext::destroyContext()
{
	if (m_context.use_count() != 1)
	{
		setAnswer( ErrorCodeLogicError, _TXT("deleted object not singular"));
		return false;
	}
	try
	{
		m_handler->deleteConfiguration( m_contextType, m_contextName);
	}
	catch (...)
	{
		setAnswerFromException();
		return false;
	}
	m_context.reset();
	if (!m_handler->removeContext( m_contextType, m_contextName, m_answer))
	{
		return false;
	}
	return true;
}

bool WebRequestContext::isCreateRequest() const noexcept
{
	return isEqual( m_requestMethod, "PUT") && !m_path.hasMore() && !m_transactionRef.get();
}

bool WebRequestContext::isDeleteRequest() const noexcept
{
	return isEqual( m_requestMethod, "DELETE") && !m_path.hasMore() && !m_transactionRef.get();
}

bool WebRequestContext::initRequestContext( const char* contentstr, size_t contentlen)
{
	try
	{
		m_transactionRef.reset();
		m_context.reset();

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
		}
		else if (m_path.startsWith( "schema", 6/*"schema"*/))
		{
			m_contextType = m_path.getNext();
			m_contextName = m_path.getNext();
		}
		else
		{
			if (!(m_contextType = m_path.getNext())) return true;
			if (isEqual( m_contextType, ROOT_CONTEXT_NAME))
			{
				m_contextName = ROOT_CONTEXT_NAME;
			}
			else
			{
				m_contextName = m_path.getNext();
				if (!m_contextName) return true;
			}
			m_transactionRef.reset();
			m_context.create();
			if (!m_context.get())
			{
				setAnswer( ErrorCodeOutOfMem);
				return false;
			}
			if (isCreateRequest())
			{
				if (!papuga_RequestContext_inherit( m_context.get(), m_handler->contextPool(), ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME))
				{
					papuga_ErrorCode errcode = papuga_RequestContext_last_error( m_context.get(), true);
					setAnswer( papugaErrorToErrorCode( errcode));
					return false;
				}
				m_configTransactionTmpFile = m_handler->storeConfigurationTemporary( m_contextType, m_contextName, std::string( contentstr, contentlen));
			}
			else
			{
				if (!papuga_RequestContext_inherit( m_context.get(), m_handler->contextPool(), m_contextType, m_contextName))
				{
					papuga_ErrorCode errcode = papuga_RequestContext_last_error( m_context.get(), true);
					setAnswer( papugaErrorToErrorCode( errcode));
					return false;
				}
			}
		}
		return true;
	}
	catch (...)
	{
		setAnswerFromException();
		return false;
	}
}

bool WebRequestContext::initLuaScript( const char* contentstr, size_t contentlen)
{
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_TransactionHandler transactionHandler{ this, &createTransaction_ };

	papuga_LuaRequestHandlerScript const* script = m_handler->script( m_contextType);
	if (!script)
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
	papuga_LuaRequestHandler* reqhnd
		= papuga_create_LuaRequestHandler(
			script, (papuga_LuaInitProc*)&luaopen_strus, m_handler->schemaMap(), m_handler->contextPool(), m_context.get(),
			&transactionHandler, m_handler->papugaLogger(), &m_attributes,
			m_requestMethod, ROOT_CONTEXT_NAME, m_path.rest(), contentstr, contentlen,
			&errcode);
	if (!reqhnd)
	{
		setAnswer( papugaErrorToErrorCode( errcode));
		return false;
	}
	try
	{
		m_luahandler.reset( reqhnd);
	}
	catch (...)
	{
		setAnswerFromException();
		return false;
	}
	return true;
}

bool WebRequestContext::runLuaScript()
{
	papuga_ErrorBuffer errbuf;
	char errbufmem[ 2048];
	papuga_init_ErrorBuffer( &errbuf, errbufmem, sizeof(errbufmem));

	if (!papuga_run_LuaRequestHandler( m_luahandler.get(), &errbuf))
	{
		if (papuga_ErrorBuffer_hasError( &errbuf))
		{
			char const* msg = papuga_ErrorBuffer_lastError( &errbuf);
			setAnswer( ErrorCodeRuntimeError, msg, true);
		}
		else
		{
			setAnswer( ErrorCodeRuntimeError, _TXT("yield not allowed in configuration"), true);
		}
		return false;
	}
	int ni = 0, ne = papuga_LuaRequestHandler_nof_DelegateRequests( m_luahandler.get());
	if (isCreateRequest())
	{
		// Ensure that a PUT object request can be reissued as load configuation on restart:
		if (ne)
		{
			std::snprintf( errbufmem, sizeof(errbufmem), _TXT("PUT request to %s/%s has delegate requests (forbidden)"), m_contextType, m_contextName);
			setAnswer( ErrorCodeDelegateRequestFailed, errbufmem, true);
			return false;
		}
		else if (papuga_LuaRequestHandler_get_result( m_luahandler.get()))
		{
			std::snprintf( errbufmem, sizeof(errbufmem), _TXT("PUT request to %s/%s has a result (forbidden)"), m_contextType, m_contextName);
			setAnswer( ErrorCodeDelegateRequestFailed, errbufmem, true);
			return false;
		}
	}
	if (ne)
	{
		m_openDelegates.reset( new int( ne));
		for (; ni != ne; ++ni)
		{
			papuga_DelegateRequest const* delegate = papuga_LuaRequestHandler_get_delegateRequest( m_luahandler.get(), ni);
			WebRequestDelegateContext* dhnd = new WebRequestDelegateContext( m_luahandler, ni, m_openDelegates);
			if (!m_handler->delegateRequest( delegate->requesturl, delegate->requestmethod, delegate->contentstr, delegate->contentlen, dhnd))
			{
				std::snprintf( errbufmem, sizeof(errbufmem), _TXT("Request to '%s' failed"), delegate->requesturl);
				setAnswer( ErrorCodeDelegateRequestFailed, errbufmem, true);
				return false;
			}
		}
		return false;
	}
	else
	{
		const papuga_LuaRequestResult* result = papuga_LuaRequestHandler_get_result( m_luahandler.get());
		if (result)
		{
			m_answer.setContent( WebRequestContent(
						papuga_StringEncoding_name(result->encoding),
						papuga_ContentType_name(result->doctype), 
						result->contentstr, result->contentlen));
		}
		if (m_transactionRef.get())
		{
			m_transactionPool->returnTransaction( m_transactionRef);
		}
		return true;
	}
}

bool WebRequestContext::executeBuiltInCommand()
{
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];
	try
	{
		if (isEqual( m_requestMethod,"OPTIONS"))
		{
			if (!m_contextType)
			{
				m_answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,GET", true);
				return true;
			}
			else if (isEqual( m_contextType, "schema"))
			{
				m_answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,GET", true);
				return true;
			}
			else
			{
				if (m_contextName)
				{
					auto script = m_handler->script( m_contextName);
					if (script)
					{
						std::string options("OPTIONS,");
						options.append( papuga_LuaRequestHandlerScript_options( script));
						m_answer.setMessage( 200/*OK*/, "Allow", options.c_str(), true);
					}
					else
					{
						setAnswer( ErrorCodeRequestResolveError);
					}
					return true;
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
			if (isEqual( m_contextType, "schema"))
			{
				if (m_contextName)
				{
					papuga_SchemaSource const* schema = papuga_SchemaList_get( m_handler->schemaList(), m_contextName);
					if (schema)
					{
						mapStringToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
									"schema"/*rootname*/, schema->name,
									papuga_UTF8, papuga_http_default_doctype( &m_attributes),
									m_attributes.beautifiedOutput, schema->source);
					}
					else
					{
						setAnswer( ErrorCodeRequestResolveError);
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
					mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
									"schema"/*rootname*/, PAPUGA_HTML_LINK_ELEMENT/*itemname*/, 
									papuga_UTF8, papuga_http_default_doctype( &m_attributes),
									m_attributes.beautifiedOutput, lst);
				}
				return true;
			}
			else if (!m_contextType)
			{
				char const* const* tplist = papuga_RequestContextPool_list_types( m_handler->contextPool(), &m_allocator);
				mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
								"class"/*rootname*/, PAPUGA_HTML_LINK_ELEMENT/*itemname*/,
								papuga_UTF8, papuga_http_default_doctype( &m_attributes),
								m_attributes.beautifiedOutput, tplist);
				return true;
			}
			else if (!m_contextName)
			{
				char const* const* tplist = papuga_RequestContextPool_list_names( m_handler->contextPool(), m_contextType, &m_allocator);
				mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
								"object"/*rootname*/, PAPUGA_HTML_LINK_ELEMENT/*itemname*/, 
								papuga_UTF8, papuga_http_default_doctype( &m_attributes),
								m_attributes.beautifiedOutput, tplist);
				return true;
			}
		}
		if (!m_contextName)
		{
			setAnswer( ErrorCodeRequestResolveError);
			return true;
		}
		return false;
	}
	catch (...)
	{
		setAnswerFromException();
		return false;
	}
}

bool WebRequestContext::execute()
{
	try
	{
		if (!m_answer.ok()) return true;
		if (!m_openDelegates.get() || *m_openDelegates == 0)
		{
			bool terminated = runLuaScript();
			if (terminated)
			{
				if (m_answer.ok())
				{
					if (m_contextName && m_contextType)
					{
						if (isCreateRequest())
						{
							transferContext();
						}
						else if (isDeleteRequest())
						{
							destroyContext();
						}
					}
				}
				else if (!m_configTransactionTmpFile.empty())
				{
					Configuration::drop( m_configTransactionTmpFile);
				}
			}
			return terminated;
		}
		else
		{
			return false;
		}
	}
	catch (...)
	{
		setAnswerFromException();
		return false;
	}
}

WebRequestAnswer WebRequestContext::getAnswer() const
{
	return m_answer;
}

