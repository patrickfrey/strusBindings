/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestContext.hpp"
#ifndef _STRUS_WEB_REQUEST_CONTEXT_IMPL_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_CONTEXT_IMPL_HPP_INCLUDED
#include "strus/webRequestContextInterface.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "pathBuf.hpp"
#include "transaction.hpp"
#include "papuga/luaRequestHandler.h"
#include "papuga/requestContext.h"
#include "papuga/requestParser.h"
#include "papuga/typedefs.h"
#include "papuga/allocator.h"
#include "papugaContextRef.hpp"
#include "papugaLuaRequestHandlerRef.hpp"
#include <stdexcept>

#define STRUS_LIST_ROOT_ELEMENT "list"

namespace strus
{

/// \brief Forward declaration
class WebRequestHandler;

/// \brief Implementation of the interface for executing an XML/JSON request on the strus bindings
class WebRequestContext
	:public WebRequestContextInterface
{
public:
	/// \brief Constructor for web request
	WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		TransactionPool* transactionPool_,
		const char* http_accept_,
		const char* html_base_href_,
		const char* method_,
		const char* path_);

	virtual ~WebRequestContext();

	virtual bool execute( const WebRequestContent& content);

	virtual WebRequestAnswer getAnswer() const;

	enum {
		MaxLogContentSize=2048 /*2K*/,
		MaxLogItemSize=48
	};

public:/*WebRequestContext*/
	static papuga_StringEncoding defaultEncoding() {return papuga_UTF8;}
	static WebRequestContent::Type defaultDocType() {return WebRequestContent::HTML;}

private:
	/// \brief Clone constructor
	WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		TransactionPool* transactionPool_,
		const char* method_,
		const char* contextType_,
		const char* contextName_,
		const papuga_RequestAttributes& attributes_,
		const PapugaContextRef& context_,
		const PapugaLuaRequestHandlerRef& luahandler_);

	/// \brief Constructor of a clone with a reset state for the execution of schemas to process the delegate request results
	WebRequestContext* createClone() const;

private:
	// Implemented in webRequestContext_obj:
	/// \brief Initialize the context of the request from the specified context type/name pair
	bool initContext();
	/// \brief Reset object for requests
	void resetContext();
	/// \brief Initialize the context of the request
	bool initRequestContext();

	// Implemented in webRequestContext_result:
	/// \brief Transfer the current context to the handler as active object
	bool transferContext();
	/// \brief Define the current request to have failed
	void setAnswer( int errcode, const char* errstr=0, bool doCopy=false);

	/// \brief Run the request as a Lua script call
	bool initLuaScript( const WebRequestContent& content);
	/// \brief Run the request as a Lua script call
	bool runLuaScript();

public:
	const char* createTransaction( const char* type, papuga_RequestContext* context, papuga_Allocator* allocator);
	bool doneTransaction();

private:
	WebRequestHandler* m_handler;		//< request handler creating this request
	WebRequestLoggerInterface* m_logger;	//< logger for logging this request
	int m_logMask;				//< what to log
	TransactionPool* m_transactionPool;	//< transaction pool
	TransactionRef m_transactionRef;	//< transaction reference
	papuga_Allocator m_allocator;		//< papuga allocator used for this request context
	papuga_RequestAttributes m_attributes;	//< request attributes
	char m_requestMethod[8];		//< request method
	const char* m_contextType;		//< context type
	const char* m_contextName;		//< context name
	PapugaContextRef m_context;		//< context reference
	PapugaLuaRequestHandlerRef m_luahandler;//< lua request handler reference
	PathBuf m_path;				//< iterator on path of the request
	papuga_ErrorBuffer m_errbuf;		//< error buffer for papuga
	WebRequestAnswer m_answer;		//< answer of the request
	char m_errbuf_mem[ 4096];		//< memory used by error buffer for papuga
	char m_allocator_mem[ 1<<14];		//< memory used by papuga allocator used for this request context
};

}//namespace
#endif


