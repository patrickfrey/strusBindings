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
#include <cstddef>
#include <utility>

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
			const std::string& config_store_dir_);
	virtual ~WebRequestHandler();

	virtual bool hasSchema(
			const char* contextType,
			const char* schema) const;

	virtual WebRequestContextInterface* createContext(
			const char* accepted_charset,
			const char* accepted_doctype,
			WebRequestAnswer& status) const;

	virtual bool loadConfiguration(
			const char* contextType,
			const char* contextName,
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& status);

	virtual bool storeConfiguration(
			const char* contextType,
			const char* contextName,
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& status);

	virtual bool loadStoredConfigurations(
			WebRequestAnswer& answer);

public:/*WebRequestContext*/
	const papuga_RequestHandler* impl() const	{return m_impl;}
	const char* html_head() const			{return m_html_head.c_str();}

private:
	WebRequestContext* createContext_( const char* accepted_charset, const char* accepted_doctype, WebRequestAnswer& status) const;

	/// \brief Get the [type,name] tuple of the context that is base of a context loaded by 'loadConfiguration':
	std::pair<const char*,const char*> getConfigSourceContext( const char* contextType, const char* contextName);

private:
	strus::mutex m_mutex;			//< mutex for locking mutual exclusion of configuration requests
	papuga_RequestLogger m_logger;		//< request logger
	papuga_RequestHandler* m_impl;		//< request handler
	std::string m_html_head;		//< header include for HTML output (for stylesheets, meta data etc.)
	std::string m_config_store_dir;		//< directory where to store configurations loaded as request
	int m_config_counter;			//< counter to order configurations stored that have the same date
};

}//namespace
#endif


