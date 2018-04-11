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
#include "papuga/requestHandler.h"
#include "papuga/requestLogger.h"
#include "transaction.hpp"
#include <cstddef>
#include <utility>
#include <set>

namespace strus
{
/// \brief Forward declaration
class WebRequestLoggerInterface;
/// \brief Forward declaration
class WebRequestContext;

/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
class WebRequestHandler
	:public WebRequestHandlerInterface
{
public:
	WebRequestHandler( 
			WebRequestLoggerInterface* logger_,
			const std::string& html_head_,
			const std::string& config_store_dir_,
			const std::string& configstr_,
			int maxIdleTime,
			int nofTransactionsPerSeconds);
	virtual ~WebRequestHandler();

	virtual WebRequestContextInterface* createContext(
			const char* accepted_charset,
			const char* accepted_doctype,
			const char* html_base_href,
			WebRequestAnswer& status) const;

	virtual void tick();

public:/*WebRequestContext*/
	enum MethodParamType {ParamEnd=0,ParamPathString,ParamPathArray,ParamDocumentClass,ParamContent};

	const papuga_RequestHandler* impl() const			{return m_impl;}
	const char* html_head() const					{return m_html_head.c_str();}
	int debug_maxdepth() const					{return m_debug_maxdepth;}
	std::vector<std::string> contextTypes() const;
	std::vector<std::string> contextNames( const std::string& name) const;

	bool loadConfiguration(
			const char* contextType,
			const char* contextName,
			bool storedForReload,
			const WebRequestContent& content,
			WebRequestAnswer& status);
	bool deleteConfiguration(
			const char* contextType,
			const char* contextName,
			WebRequestAnswer& status);

private:/*Load store configuration source*/
	struct ConfigurationTransaction
	{
		std::string failed_filename;
		std::string filename;
	};

	bool storeConfiguration(
			ConfigurationTransaction& transaction,
			const char* contextType,
			const char* contextName,
			const WebRequestContent& content,
			WebRequestAnswer& status) const;
	bool commitStoreConfiguration(
			const ConfigurationTransaction& transaction,
			WebRequestAnswer& status) const;
	bool deleteStoredConfigurations(
			const char* contextType,
			const char* contextName,
			WebRequestAnswer& status) const;

private:/*Constructor/Destructor*/
	void loadConfiguration( const std::string& configstr);
	void loadInitConfiguration( const std::string& configstr);
	bool loadStoredConfigurations();
	bool deleteStoredConfiguration( const char* contextType, const char* contextName, WebRequestAnswer& status);

	void clear();

public:
	/// \brief Configuration of an object
	struct SubConfig
	{
		std::string name;
		std::string id;
		std::string content;
	
		SubConfig( const SubConfig& o)
			:name(o.name),id(o.id),content(o.content){}
		SubConfig( const std::string& name_, const std::string& id_,  const std::string& content_)
			:name(name_),id(id_),content(content_){}
	};

private:/*Load configuration*/
	std::vector<SubConfig> getSubConfigList( const std::string& content) const;
	bool isSubConfigSection( const std::string& name) const;
	WebRequestContext* createContext_(
			const char* accepted_charset,
			const char* accepted_doctype,
			const char* html_base_href,
			WebRequestAnswer& status) const;

private:
	typedef std::pair<std::string,std::string> ContextNameDef;

private:
	mutable strus::mutex m_mutex;			//< mutex for locking mutual exclusion of configuration requests
	mutable int m_config_counter;			//< counter to order configurations stored that have the same date
	int m_debug_maxdepth;				//< maximum depth for debug structures
	WebRequestLoggerInterface* m_logger;		//< request logger 
	papuga_RequestHandler* m_impl;			//< request handler
	std::string m_html_head;			//< header include for HTML output (for stylesheets, meta data etc.)
	std::string m_config_store_dir;			//< directory where to store configurations loaded as request
	std::set<ContextNameDef> m_context_names;	//< context definitions type name pairs
	std::set<std::string> m_context_typenames;	//< defined context types
	TransactionPool m_transactionPool;		//< transaction pool
};

}//namespace
#endif


