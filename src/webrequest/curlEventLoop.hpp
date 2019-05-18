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
#include <stdexcept>
#include <new>
#include <curl/curl.h>

namespace strus {

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
	class Job {
	public:
		Job( CURL* handle_) :m_handle(handle_){}

		virtual ~Job(){}
		virtual void resume( CURLcode ec)=0;

	public:
		CURL* handle() const {return m_handle;}

	private:
		CURL* m_handle;
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
	bool pushJob( Job* job);

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
