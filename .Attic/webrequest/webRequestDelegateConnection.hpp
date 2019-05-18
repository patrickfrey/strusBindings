/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the context of a delegated request (sub request to another server)
/// \file "webRequestDelegateConnection.hpp"
#ifndef _STRUS_WEB_REQUEST_DELEGATE_CONNECTION_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_DELEGATE_CONNECTION_HPP_INCLUDED
#include "strus/base/shared_ptr.hpp"
#include "strus/base/filehandle.hpp"
#include "strus/base/jobQueueWorker.hpp"
#include "strus/base/thread.hpp"
#include "strus/webRequestAnswer.hpp"
#include "strus/errorCodes.hpp"
#include <curl/curl.h>
#include <map>
#include <queue>
#include <string>
#include <cstring>

namespace strus {

/// \brief Forward declaration
class WebRequestDelegateContextInterface;

class WebRequestDelegateConnection
{
public:
	explicit WebRequestDelegateConnection( const std::string& address);
	~WebRequestDelegateConnection();

	void send( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_);
	SocketHandle socket();

private:
	enum State {Init,Connected,ReceiveAnswer,Done};

	struct RequestData
	{
		std::string method;
		std::string content;
		std::size_t dataSent;
		strus::shared_ptr<WebRequestDelegateContextInterface> receiver;

		RequestData( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_)
			:method(method_),content(content_),dataSent(0),receiver(receiver_){}
		RequestData( const RequestData& o)
			:method(o.method),content(o.content),dataSent(o.dataSent),receiver(o.receiver){}
	};

private:
	CURL* m_curl;
	std::string m_url;
	int m_port;
	std::string m_response_content;
	char m_response_errbuf[ CURL_ERROR_SIZE];
	CURLcode m_response_result;
	State m_state;
	strus::mutex m_mutex;
	std::queue<RequestData> m_requestQueue;

private:
	friend class WebRequestDelegateConnectionPool;
	static void connectionResume( void* context, ErrorCode status);
	static void connectionReadProc( void* context_);
	static void connectionWriteProc( void* context_);
	static void connectionExceptProc( void* context_);

	void connect();
	void completeResponse();
	void returnAnswer( const WebRequestAnswer& answer);
	void sendData();
	void receiveData();
	void handleConnectionException( ErrorCode status);
};

typedef strus::shared_ptr<WebRequestDelegateConnection> WebRequestDelegateConnectionRef;


class WebRequestDelegateConnectionPool
{
public:
	explicit WebRequestDelegateConnectionPool( JobQueueWorker* jobqueue_)
		:connectionMap(),jobqueue(jobqueue_){}
	~WebRequestDelegateConnectionPool()
	{}

	WebRequestDelegateConnectionRef getOrCreateConnection( const std::string& address);

private:
	typedef std::map<std::string,WebRequestDelegateConnectionRef> ConnectionMap;

private:
	ConnectionMap connectionMap;
	JobQueueWorker* jobqueue;
};


}//namespace
#endif

