/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Connector interface for delegated request (sub request to another server)
/// \file "strus/webRequestDelegateConnectorInterface.hpp"
#ifndef _STRUS_WEB_REQUEST_DELEGATE_CONNECTOR_INTERFACE_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_DELEGATE_CONNECTOR_INTERFACE_HPP_INCLUDED
#include "strus/webRequestDelegateContextInterface.hpp"

namespace strus {

class WebRequestDelegateConnectorInterface
{
public:
	virtual ~WebRequestDelegateConnectorInterface(){}

	/// \brief Send a HTTP request to another strus webservice
	/// \param[in] address where to send the request to
	/// \param[in] method request method
	/// \param[in] content content of the request
	/// \param[in] receiver delegate (callback with closure) (passed with ownership)
	/// \note Used to delegate part of a request to a sub service
	/// \note Response is handled via a callback with a closure encapsulated in a receiver object
	virtual void send( 
		const std::string& address,
		const std::string& method,
		const std::string& content,
		WebRequestDelegateContextInterface* receiver)=0;
};

}//namespace
#endif


