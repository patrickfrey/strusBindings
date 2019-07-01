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
#include <string>

namespace strus {

/// \brief Forward declaration
class WebRequestHandlerInterface;
/// \brief Forward declaration
class WebRequestLoggerInterface;

/// \brief Job queue worker and periodic timer event ticker thread
class ConfigurationUpdateRequestContext
	:public WebRequestDelegateContextInterface
{
public:
	ConfigurationUpdateRequestContext( WebRequestHandlerInterface* handler_, WebRequestLoggerInterface* logger_, const std::string& type_, const std::string& name_, const std::string& schema_)
		:m_handler(handler_),m_logger(logger_),m_type(type_),m_name(name_),m_schema(schema_){}

	virtual void putAnswer( const WebRequestAnswer& status);

private:
	WebRequestHandlerInterface* m_handler;
	WebRequestLoggerInterface* m_logger;
	std::string m_type;
	std::string m_name;
	std::string m_schema;
};

}//namespace
#endif


