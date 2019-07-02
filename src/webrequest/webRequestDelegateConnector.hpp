/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the connector for delegated request (sub request to another server)
/// \file "webRequestDelegateConnector.hpp"
#ifndef _STRUS_WEB_REQUEST_DELEGATE_CONNECTOR_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_DELEGATE_CONNECTOR_HPP_INCLUDED
#include "strus/base/shared_ptr.hpp"
#include "strus/base/filehandle.hpp"
#include "strus/base/thread.hpp"
#include "curlEventLoop.hpp"
#include <curl/curl.h>
#include <map>
#include <string>
#include <cstring>

namespace strus {

class WebRequestDelegateConnector
	:public WebRequestDelegateConnectorInterface
{
public:
	explicit WebRequestDelegateConnector( CurlEventLoop* eventloop_)
		:m_connectionMap(),m_eventloop(eventloop_){}
	virtual ~WebRequestDelegateConnector(){}

	virtual void send( 
		const std::string& address,
		const std::string& method,
		const std::string& content,
		WebRequestDelegateContextInterface* receiver_);

private:
	typedef std::map<std::string,WebRequestDelegateConnectionRef> ConnectionMap;
	WebRequestDelegateConnectionRef getConnection( const std::string& address);
	void setConnection( const std::string& address, const WebRequestDelegateConnectionRef& ref);
	WebRequestDelegateConnectionRef getOrCreateConnection( const std::string& address);

private:
	strus::mutex m_mutex;
	ConnectionMap m_connectionMap;
	CurlEventLoop* m_eventloop;
};


}//namespace
#endif

