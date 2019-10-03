/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Delegate request sink for configuration update
/// \file "configurationUpdateRequest.hpp"
#ifndef _STRUS_WEBREQUEST_CONFIGURATION_UPDATE_REQUEST_HPP_INCLUDED
#define _STRUS_WEBREQUEST_CONFIGURATION_UPDATE_REQUEST_HPP_INCLUDED
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/base/atomic.hpp"
#include <string>

namespace strus {

/// \brief Forward declaration
class WebRequestHandlerInterface;
/// \brief Forward declaration
class WebRequestContextInterface;
/// \brief Forward declaration
class WebRequestLoggerInterface;

/// \brief Object to hold the context of an open configuration update delegate request
class ConfigurationUpdateRequestContext
	:public WebRequestDelegateContextInterface
{
public:
	/// \brief Constructor
	/// \param[in] handler_ request handler
	/// \param[in] logger_ logger for logging request
	/// \param[in] receiver_ configuration context to process the request result
	/// \param[in] receiverSchema_ schema to use for processing of the request result
	/// \param[in] counter_ shared counter of open requests for book-keeping
	ConfigurationUpdateRequestContext( WebRequestHandlerInterface* handler_, WebRequestLoggerInterface* logger_, WebRequestContextInterface* receiver_, const char* receiverSchema_, AtomicCounter<int>* counter_)
		:m_handler(handler_)
		,m_logger(logger_)
		,m_context(receiver_)
		,m_schema(receiverSchema_)
		,m_counter(counter_)
		,m_requestCount(*counter_){}

	/// \brief Push the request answer to the receivers context
	/// \param[in] status the request answer with status
	virtual void putAnswer( const WebRequestAnswer& status);

private:
	WebRequestHandlerInterface* m_handler;
	WebRequestLoggerInterface* m_logger;
	WebRequestContextInterface* m_context;
	const char* m_schema;
	AtomicCounter<int>* m_counter;
	int m_requestCount;
};

}//namespace
#endif


