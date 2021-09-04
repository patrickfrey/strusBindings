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
	,m_context(),m_luahandler(),m_path(path_)
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
		const PapugaContextRef& context_,
		const PapugaLuaRequestHandlerRef& luahandler_)
	:m_handler(handler_)
	,m_logger(logger_)
	,m_logMask(logger_->logMask())
	,m_transactionPool(transactionPool_),m_transactionRef()
	,m_contextType(contextType_),m_contextName(contextName_)
	,m_context(context_),m_luahandler(luahandler_),m_path()
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
			m_attributes, m_context, m_luahandler);
}

static const char* createTransaction_( void* self, const char* type, papuga_RequestContext* context, papuga_Allocator* allocator)
{
	return ((WebRequestContext*)(self))->createTransaction( type, context, allocator);
}

static bool doneTransaction_( void* self)
{
	return ((WebRequestContext*)(self))->doneTransaction();
}

bool WebRequestContext::initLuaScript( const WebRequestContent& content)
{
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_TransactionHandler transactionHandler{ this, &createTransaction_, &doneTransaction_ };

	papuga_LuaRequestHandlerScript const* script = m_handler->script( m_contextType);
	if (!script)
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
	papuga_LuaRequestHandler* reqhnd
		= papuga_create_LuaRequestHandler(
			script, m_handler->schemaMap(), m_handler->contextPool(), m_context.get(),
			&transactionHandler, &m_attributes,
			"PUT", ROOT_CONTEXT_NAME, "", content.str(), content.len(),
			&errcode);
	if (!reqhnd)
	{
		setAnswer( papugaErrorToErrorCode( errcode));
		return false;
	}
	m_luahandler.create( reqhnd);
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
	for (; ni != ne; ++ni)
	{
		papuga_DelegateRequest const* delegate = papuga_LuaRequestHandler_get_delegateRequest( m_luahandler.get(), ni);
		if (!m_handler->delegateRequest( delegate->requesturl, delegate->requestmethod,
						       delegate->contentstr, delegate->contentlen, &m_luahandler))
		{
			setAnswer( ErrorCodeDelegateRequestFailed, delegate->requesturl, true);
			return false;
		}
	}
	return true;
}

bool WebRequestContext::execute(
		const WebRequestContent& content)
{
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
					auto script = m_handler->script( m_contextName);
					if (script)
					{
						std::string options("OPTIONS,");
						options.append( papuga_LuaRequestHandlerScript_options( script));
						m_answer.setMessage( 200/*OK*/, "Allow", options.c_str(), true);
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
		return runLuaScript();
	}
	WEBREQUEST_CONTEXT_CATCH_ERROR_RETURN( false);
}

WebRequestAnswer WebRequestContext::getAnswer() const
{
	return m_answer;
}

