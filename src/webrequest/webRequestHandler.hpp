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
#include <cstddef>

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
	explicit WebRequestHandler( WebRequestLoggerInterface* logger_);
	virtual ~WebRequestHandler();

	virtual bool hasSchema( const char* schema) const;

	virtual WebRequestContextInterface* createContext(
			const char* context,
			const char* schema,
			const char* role,
			const char* accepted_charset,
			WebRequestAnswer& status) const;

	virtual bool loadConfiguration(
			const char* destContextName,
			const char* destContextSchemaPrefix,
			const char* srcContextName,
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& status);

private:
	WebRequestContext* createContext_( const char* context, const char* schema, const char* role, const char* accepted_charset, WebRequestAnswer& status) const;

private:
	strus::mutex m_mutex;			//< mutex for locking mutual exclusion of configuration requests
	papuga_RequestLogger m_logger;		//< request logger
	papuga_RequestHandler* m_impl;		//< request handler
};

}//namespace
#endif


