/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Worker thread for asynchronous background message processing based on CURL
/// \file "curlEventLoop.hpp"
#ifndef _STRUS_BINDINGS_CURL_EVENT_LOOP_HPP_INCLUDED
#define _STRUS_BINDINGS_CURL_EVENT_LOOP_HPP_INCLUDED
#include "strus/webRequestEventLoopInterface.hpp"
#include <string>

namespace strus {

/// \brief Forward declaration
class WebRequestLoggerInterface;

/// \brief Job queue worker and periodic timer event ticker thread
class CurlEventLoop
	:public WebRequestEventLoopInterface
{
public:
	enum {NumberOfRequestsBeforeTick=5};

	/// \brief Constructor
	/// \param[in] logger_ logger to log events and errors
	/// \param[in] secondsPeriod_ Period of timer ticker events in seconds
	/// \note Period equals timeout without worker jobs triggered
	/// \param[in] max_total_conn max simultaneously open connections (CURLMOPT_MAX_TOTAL_CONNECTIONS)
	/// \param[in] max_host_conn set max number of simultaneous connections to a single host (CURLMOPT_MAX_HOST_CONNECTIONS)
	/// \remark Regularity of timer events is not guaranteed, bookkeeping about real time is up to client
	CurlEventLoop( WebRequestLoggerInterface* logger, int secondsPeriod_, int max_total_conn, int max_host_conn);

	/// \brief Destructor
	~CurlEventLoop();

	virtual bool start();
	virtual void stop();
	virtual long time() const;

	bool send( const std::string& address,
			const std::string& method,
			const std::string& content,
			WebRequestDelegateContextInterface* receiver);

	virtual bool addTickerEvent( void* obj, TickerFunction func);

public:
	struct Data;					//... private as PIMPL

private:
	void run();

private:
	Data* m_data;
};

} // namespace
#endif
