/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Worker thread for asynchronous background message processing based on CURL
/// \file "curlEventLoop.hpp"
#ifndef _STRUS_CURL_EVENT_LOOP_HPP_INCLUDED
#define _STRUS_CURL_EVENT_LOOP_HPP_INCLUDED
#include "strus/webRequestAnswer.hpp"
#include "strus/webRequestDelegateConnectorInterface.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/base/shared_ptr.hpp"
#include "strus/base/thread.hpp"
#include <stdexcept>
#include <new>
#include <queue>
#include <curl/curl.h>

namespace strus {

/// \brief Forward declaration
class CurlEventLoop;

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
{
public:
	WebRequestDelegateJob( const WebRequestDelegateConnectionRef& conn_, const WebRequestDelegateDataRef& data_, CurlEventLoop* eventloop_);

	~WebRequestDelegateJob(){}
	void resume( CURLcode ec);

public:
	CURL* handle() const {return m_handle;}

private:
	CURL* m_handle;
	WebRequestDelegateConnectionRef m_conn;
	WebRequestDelegateDataRef m_data;
	CurlEventLoop* m_eventloop;

	std::string m_response_content;
	char m_response_errbuf[ CURL_ERROR_SIZE];
};

/// \brief Job queue worker and periodic timer event ticker thread
class CurlEventLoop
{
public:
	enum {NumberOfRequestsBeforeTick=5};
	class Ticker {
	public:
		virtual ~Ticker(){}
		virtual void tick()=0;
	};
	class Logger {
	public:
		enum LogType {LogFatal,LogError,LogWarning};
		Logger( LogType loglevel_)	:m_loglevel(loglevel_){}

		virtual ~Logger(){}
		virtual void print( LogType logtype, const char* fmt, ...)=0;

		LogType loglevel() const	{return m_loglevel;}
	private:
		LogType m_loglevel;
	};

	/// \brief Constructor
	/// \param[in] secondsPeriod_ Period of timer ticker events in seconds
	/// \note Period equals timeout without worker jobs triggered
	/// \param[in] logger_ logger to log events and errors
	/// \param[in] ticker_ ticker triggered on timer event
	/// \param[in] max_total_conn max simultaneously open connections (CURLMOPT_MAX_TOTAL_CONNECTIONS)
	/// \param[in] max_host_conn set max number of simultaneous connections to a single host (CURLMOPT_MAX_HOST_CONNECTIONS)
	/// \remark Regularity of timer events is not guaranteed, bookkeeping about real time is up to client
	CurlEventLoop( int secondsPeriod_, Logger* logger_, Ticker* ticker_, int max_total_conn, int max_host_conn);

	/// \brief Destructor
	~CurlEventLoop();

	/// \brief Start thread
	/// \return true on success, false on error
	bool start();
	/// \brief Stop thread without finishing idle jobs in the queue
	void stop();

	/// \brief Push a new job
	/// \param[in] job pointer to job (passed with ownership)
	/// \return true on success, false on error
	bool pushJob( WebRequestDelegateJob* job);

	/// \brief Handle an exception
	/// \param[in] message of the exception
	void handleException( const char* msg);

private:
	void run();

private:
	struct Data;
	Data* m_data;
};

} // namespace
#endif
