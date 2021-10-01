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
#include "strus/errorCodes.hpp"
#include "strus/lib/error.hpp"
#include "strus/base/fileio.hpp"
#include "private/internationalization.hpp"

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
	,m_transactionPool(transactionPool_),m_transactionRef()
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
	initRequestContext();
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
	return ((WebRequestContext*)(self))->createTransaction( type, context, allocator);
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
		setAnswer( ErrorCodeLogicError, _TXT("transferred configuration object not singular (referenced twice)"));
		return false;
	}
	if (!m_handler->transferContext( m_contextType, m_contextName, m_context.release(), m_answer))
	{
		return false;
	}
	return true;
}

void WebRequestContext::resetContext()
{
	m_transactionRef.reset();
	m_context.reset();
}

bool WebRequestContext::initContext()
{
	m_transactionRef.reset();
	m_context.create();
	if (!m_context.get())
	{
		setAnswer( ErrorCodeOutOfMem);
		return false;
	}
	if (!papuga_RequestContext_inherit( m_context.get(), m_handler->contextPool(), m_contextType, m_contextName))
	{
		papuga_ErrorCode errcode = papuga_RequestContext_last_error( m_context.get(), true);
		setAnswer( papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}

bool WebRequestContext::initRequestContext()
{
	resetContext();
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
		initContext();
	}
	return true;
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
			script, m_handler->schemaMap(), m_handler->contextPool(), m_context.get(),
			&transactionHandler, m_handler->papugaLogger(), &m_attributes,
			"PUT", ROOT_CONTEXT_NAME, "", contentstr, contentlen,
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
		setAnswer( ErrorCodeOutOfMem);
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
	if (ne)
	{
		m_openDelegates.reset( new int( ne));
	}
	for (; ni != ne; ++ni)
	{
		papuga_DelegateRequest const* delegate = papuga_LuaRequestHandler_get_delegateRequest( m_luahandler.get(), ni);
		WebRequestDelegateContext* dhnd = new WebRequestDelegateContext( m_luahandler, ni, m_openDelegates);
		if (!m_handler->delegateRequest( delegate->requesturl, delegate->requestmethod, delegate->contentstr, delegate->contentlen, dhnd))
		{
			setAnswer( ErrorCodeDelegateRequestFailed, delegate->requesturl, true);
			return false;
		}
	}
	return true;
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
			else if (0==std::strcmp( m_contextType, "schema"))
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
			if (0==std::strcmp( m_contextType, "schema"))
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
									"schema"/*rootname*/, "name"/*itemname*/, 
									papuga_UTF8, papuga_http_default_doctype( &m_attributes),
									m_attributes.beautifiedOutput, lst);
				}
				return true;
			}
			else if (!m_contextType)
			{
				char const* const* tplist = papuga_RequestContextPool_list_types( m_handler->contextPool(), &m_allocator);
				mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
								"class"/*rootname*/, "name"/*itemname*/,
								papuga_UTF8, papuga_http_default_doctype( &m_attributes),
								m_attributes.beautifiedOutput, tplist);
				return true;
			}
			else if (!m_contextName)
			{
				char const* const* tplist = papuga_RequestContextPool_list_names( m_handler->contextPool(), m_contextType, &m_allocator);
				mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_attributes.html_base_href,
								"object"/*rootname*/, "name"/*itemname*/, 
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
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( false);
}

bool WebRequestContext::execute()
{
	try
	{
		if (!m_answer.ok()) return true;
		return (!m_openDelegates.get() || *m_openDelegates == 0) ? runLuaScript() : false;
	}
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( false);
}

WebRequestAnswer WebRequestContext::getAnswer() const
{
	return m_answer;
}

