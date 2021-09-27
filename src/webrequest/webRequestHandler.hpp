/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestHandler.hpp"
#ifndef _STRUS_WEB_REQUEST_HANDLER_IMPL_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_HANDLER_IMPL_HPP_INCLUDED
#include "strus/webRequestHandlerInterface.hpp"
#include "strus/base/thread.hpp"
#include "papuga/requestContext.h"
#include "papuga/schema.h"
#include "papuga/luaRequestHandler.h"
#include "transaction.hpp"
#include "curlEventLoop.hpp"
#include "papugaLuaRequestHandlerRef.hpp"
#include <cstddef>
#include <utility>
#include <set>

#define ROOT_CONTEXT_NAME "context"
#define SYSTEM_MESSAGE_HEADER "service"

namespace strus
{
/// \brief Forward declaration
class WebRequestLoggerInterface;
/// \brief Forward declaration
class WebRequestContext;
/// \brief Forward declaration
class WebRequestEventLoopInterface;

/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
class WebRequestHandler
	:public WebRequestHandlerInterface
{
public:
	WebRequestHandler(
			WebRequestEventLoopInterface* eventloop_,
			WebRequestLoggerInterface* logger_,
			const std::string& html_head_,
			const std::string& config_dir_,
			const std::string& script_dir_,
			const std::string& schema_dir_,
			const std::string& service_name_,
			int port_,
			bool beautifiedOutput_,
			int maxIdleTime_,
			int nofTransactionsPerSeconds);
	virtual ~WebRequestHandler();

	virtual bool init(
			char const* configsrc,
			size_t configlen,
			WebRequestAnswer& answer);

	virtual WebRequestContextInterface* createContext(
			char const* http_accept,
			char const* html_base_href,
			char const* method,
			char const* path,
			char const* contentstr,
			size_t contentlen,
			WebRequestAnswer& answer);

	virtual bool delegateRequest(
			char const* address,
			char const* method,
			char const* contentstr,
			size_t contentsize,
			WebRequestDelegateContextInterface* context);

	virtual WebRequestAnswer getSimpleRequestAnswer(
			char const* http_accept,
			char const* html_base_href,
			char const* name,
			char const* messagestr,
			size_t messagelen);

public:/*WebRequestContext*/
	papuga_SchemaList const* schemaList() const noexcept			{return m_schemaList;}
	papuga_SchemaMap* schemaMap() const noexcept				{return m_schemaMap;}
	const papuga_LuaRequestHandlerScript* script( const char* name) const noexcept	{auto fi = m_scriptMap.find(name); return fi==m_scriptMap.end() ? nullptr : fi->second.get();}
	papuga_RequestContextPool* contextPool() const noexcept			{return m_contextPool;}
	char const* html_head() const noexcept					{return m_html_head.c_str();}
	int debug_maxdepth() const noexcept					{return m_debug_maxdepth;}
	int maxIdleTime() const noexcept					{return m_maxIdleTime;}
	bool beautifiedOutput() const noexcept					{return m_beautifiedOutput;}
	char const* serviceName() const noexcept				{return m_serviceName.c_str();}

	/// \brief Pass ownership of a context to the request handler
	/// \param[in] contextType type name of context
	/// \param[in] contextName object name of context
	/// \param[in] context context transferred (with ownership, destroyed in case of failure)
	/// \param[out] answer describes the error in case failure
	bool transferContext(
			char const* contextType,
			char const* contextName,
			papuga_RequestContext* context,
			WebRequestAnswer& answer);
	/// \brief Remove a context addressed by type and name
	/// \param[in] contextType type name of context
	/// \param[in] contextName object name of context
	/// \param[out] answer describes the error in case failure
	bool removeContext(
			char const* contextType,
			char const* contextName,
			WebRequestAnswer& answer);

public:/*CurlEventLoopTicker*/
	void tick();

private:
	strus::mutex m_mutex_context_transfer;		//< mutual exclusion of request context access
	int m_debug_maxdepth;				//< maximum depth for debug structures
	WebRequestLoggerInterface* m_logger;		//< request logger
	papuga_RequestContextPool* m_contextPool;	//< request context pool
	papuga_SchemaList* m_schemaList;		//< list of schemas
	papuga_SchemaMap* m_schemaMap;			//< map of schemas
	PagugaLuaRequestHandlerScriptMap m_scriptMap; 	//< map of scripts
	std::string m_html_head;			//< header include for HTML output (for stylesheets, meta data etc.)
	std::string m_config_dir;			//< directory where configuration is stored
	std::string m_script_dir;			//< directory where scripts are stored
	std::string m_schema_dir;			//< directory where schemas are stored
	std::string m_serviceName;			//< identifier of the webserver
	TransactionPool m_transactionPool;		//< transaction pool
	std::string m_port;				//< port number of this request handler used to identify calls to self via loopback
	int m_maxIdleTime;				//< maximum idle time transactions
	bool m_beautifiedOutput;			//< true, if output should be beautyfied for more readability
	WebRequestEventLoopInterface* m_eventLoop;	//< queue for requests to other servers and periodic timer event to handle timeout of transactions
	papuga_Allocator m_allocator;			//< allocator used for schemas and scripts
};

}//namespace
#endif


