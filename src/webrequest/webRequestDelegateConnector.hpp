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
#include "strus/webRequestAnswer.hpp"
#include "strus/webRequestDelegateConnectorInterface.hpp"
#include "strus/errorCodes.hpp"
#include "curlEventLoop.hpp"
#include <curl/curl.h>
#include <map>
#include <queue>
#include <string>
#include <cstring>

namespace strus {

/// \brief Forward declaration
class WebRequestDelegateContextInterface;

struct WebRequestDelegateData
{
	std::string method;
	std::string content;
	strus::shared_ptr<WebRequestDelegateContextInterface> receiver;

	WebRequestDelegateData( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_)
		:method(method_),content(content_),receiver(receiver_){}
	WebRequestDelegateData( const WebRequestDelegateData& o)
		:method(o.method),content(o.content),receiver(o.receiver){}
};

typedef strus::shared_ptr<WebRequestDelegateData> WebRequestDelegateDataRef;


class WebRequestDelegateConnection
{
public:
	WebRequestDelegateConnection( const std::string& address, CurlEventLoop* eventloop_);
	~WebRequestDelegateConnection();

	void push( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_);
	WebRequestDelegateDataRef fetch();

	void reconnect();
	void done();
	void dropAllPendingRequests( const char* errmsg);

	CURL* handle() const {return m_curl;}

private:
	enum State {Init,Connected,Process};

private:
	CURL* m_curl;
	std::string m_url;
	int m_port;
	State m_state;
	strus::mutex m_mutex;
	std::queue<WebRequestDelegateDataRef> m_requestQueue;
	CurlEventLoop* m_eventloop;

private:
	void connect();
};

typedef strus::shared_ptr<WebRequestDelegateConnection> WebRequestDelegateConnectionRef;
typedef strus::shared_ptr<WebRequestDelegateContextInterface> WebRequestDelegateContextRef;


class WebRequestDelegateJob
	:public CurlEventLoop::Job {
public:
	WebRequestDelegateJob( const WebRequestDelegateConnectionRef& conn_, const WebRequestDelegateDataRef& data_, CurlEventLoop* eventloop_);

	virtual ~WebRequestDelegateJob(){}
	virtual void resume( CURLcode ec);

private:
	WebRequestDelegateConnectionRef m_conn;
	WebRequestDelegateDataRef m_data;
	CurlEventLoop* m_eventloop;

	std::string m_response_content;
	char m_response_errbuf[ CURL_ERROR_SIZE];
};

class WebRequestDelegateConnector
	:public WebRequestDelegateConnectorInterface
{
public:
	explicit WebRequestDelegateConnector( CurlEventLoop* eventloop_)
		:m_connectionMap(),m_eventloop(eventloop_){}
	virtual ~WebRequestDelegateConnector(){}

	virtual void send( const std::string& address_, const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_);

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

