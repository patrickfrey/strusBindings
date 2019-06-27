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

/// \brief Job queue worker and periodic timer event ticker thread
class ConfigurationUpdateRequestContext
	:public WebRequestDelegateContextInterface
{
public:
	virtual void putAnswer( const WebRequestAnswer& status);

private:
};

}//namespace
#endif


