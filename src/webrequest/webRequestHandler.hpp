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
#include "configurationHandler.hpp"
#include "strus/base/thread.hpp"
#include "papuga/requestHandler.h"
#include "papuga/requestLogger.h"
#include "transaction.hpp"
#include "curlEventLoop.hpp"
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
			const std::string& config_store_dir_,
			const std::string& service_name_,
			int port_,
			bool beautifiedOutput_,
			int maxIdleTime_,
			int nofTransactionsPerSeconds);
	virtual ~WebRequestHandler();

	virtual bool init( 
			const std::string& configsrc,
			WebRequestAnswer& answer);

	virtual WebRequestContextInterface* createContext(
			const char* accepted_charset,
			const char* accepted_doctype,
			const char* html_base_href,
			const char* method,
			const char* path,
			WebRequestAnswer& answer);

	virtual bool delegateRequest(
			const std::string& address,
			const std::string& method,
			const std::string& content,
			WebRequestDelegateContextInterface* context);

	virtual WebRequestAnswer getSimpleRequestAnswer(
			const char* accepted_charset,
			const char* accepted_doctype,
			const char* html_base_href,
			const std::string& name,
			const std::string& message);

private:
	/// \brief Create the structure for handling a configuration request
	/// \note A configuration request is defined as embedded section in the configuration file and it adresses its object not by the path but explicitely addresses the object by type and name.
	/// \note It does not store any artifacts for recreation on restart because it does not have to remember any objects defined as they are defined in the main configuration file.
	/// \note It is not recommended to use this context except for creation of the main context. Do not use embedded configurations of other objects, if you want to build bigger systems.
	/// \param[in] contextType context type
	/// \param[in] contextName context name
	/// \param[out] answer the error status
	/// \return the context structure for handling the configuration request or NULL in case of an error (inspect answer for the error details)
	WebRequestContextInterface* createConfigurationContext(
			const char* contextType,
			const char* contextName,
			WebRequestAnswer& answer);

public:/*WebRequestContext*/
	enum MethodParamType {ParamEnd=0,ParamPathString,ParamPathArray,ParamDocumentClass,ParamContent};

	const papuga_RequestHandler* impl()				{return m_impl;}
	const char* html_head() const					{return m_html_head.c_str();}
	int debug_maxdepth() const					{return m_debug_maxdepth;}
	int maxIdleTime() const						{return m_maxIdleTime;}
	bool beautifiedOutput() const					{return m_beautifiedOutput;}
	const char* serviceName() const					{return m_serviceName.c_str();}

	/// \brief Pass ownership of a context to the request handler
	/// \param[in] contextType type name of context
	/// \param[in] contextName object name of context
	/// \param[in] context context transferred (with ownership, destroyed in case of failure)
	/// \param[out] answer describes the error in case failure
	bool transferContext(
			const char* contextType,
			const char* contextName,
			papuga_RequestContext* context,
			WebRequestAnswer& answer);
	/// \brief Remove a context addressed by type and name
	/// \param[in] contextType type name of context
	/// \param[in] contextName object name of context
	/// \param[out] answer describes the error in case failure
	bool removeContext(
			const char* contextType,
			const char* contextName,
			WebRequestAnswer& answer);

	std::string allocTemporaryContextName( const std::string& contextType, const char* prefix)
	{
		return m_configHandler.allocTemporaryContextName( contextType, prefix);
	}

	void releaseTemporaryContextName( const std::string& contextType, const std::string& contextName)
	{
		return m_configHandler.releaseTemporaryContextName( contextType, contextName);
	}

public:/*libstrus_webrequest*/
	/// \brief Store all request schemas in a tree with a given directory as root
	void storeSchemaDescriptions( const std::string& dir) const;
	void storeSchemaDescriptions( const std::string& dir, const std::string& doctype) const;

public:/*CurlEventLoopTicker*/
	void tick();

private:/*init*/
	bool runConfigurationLoad( WebRequestContextInterface* ctx, const WebRequestContent& content, WebRequestAnswer& answer);
	bool loadSubConfiguration( const ConfigurationDescription& configdescr, bool initload, WebRequestAnswer& answer);
	bool loadMainConfiguration( const std::string& configstr, WebRequestAnswer& answer);
	bool loadConfiguration( const std::string& configstr, WebRequestAnswer& answer);
	/// \brief Try to evaluate if a request is to itself
	/// \return the start of the path if yes, else return NULL 
	const char* pathToSelf( const char* address);
	/// \brief Loopback request to itself
	/// \brief Needed during configuration phase are executed directly because otherwise we get into a hen and egg problem as the service needed to answer the request is not yet up
	bool loopbackConfigurationLoadDelegateRequest( WebRequestContextInterface* receiverContext, const char* receiverSchema, const char* method, const char* path, const std::string& content, WebRequestAnswer& answer);

private:
	strus::mutex m_mutex_context_transfer;		//< mutual exclusion of request context access
	int m_debug_maxdepth;				//< maximum depth for debug structures
	WebRequestLoggerInterface* m_logger;		//< request logger 
	papuga_RequestHandler* m_impl;			//< request handler
	ConfigurationHandler m_configHandler;		//< configuration handler
	std::string m_html_head;			//< header include for HTML output (for stylesheets, meta data etc.)
	std::string m_serviceName;			//< identifier of the webserver
	TransactionPool m_transactionPool;		//< transaction pool
	std::string m_port;				//< port number of this request handler used to identify calls to self via loopback
	int m_maxIdleTime;				//< maximum idle time transactions
	bool m_beautifiedOutput;			//< true, if output should be beautyfied for more readability
	WebRequestEventLoopInterface* m_eventLoop;	//< queue for requests to other servers and periodic timer event to handle timeout of transactions
};

}//namespace
#endif


