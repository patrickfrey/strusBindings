/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for the context of a delegated request (sub request to another server)
/// \file "webRequestDelegateContextInterface.hpp"
#ifndef _STRUS_WEB_REQUEST_DELEGATE_CONTEXT_INTERFACE_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_DELEGATE_CONTEXT_INTERFACE_HPP_INCLUDED
#include "strus/webRequestAnswer.hpp"
#include "strus/webRequestContent.hpp"
#include <cstddef>
#include <vector>
#include <string>

namespace strus
{

/// \brief Interface for the context of a delegated request (sub request to another server)
class WebRequestDelegateContextInterface
{
public:
	virtual ~WebRequestDelegateContextInterface(){}
	virtual void putAnswer( const WebRequestAnswer& status)=0;
};

}//namespace
#endif

