/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Eventloop with ticker events and a connector interface for delegated request (sub request to another server)
/// \file "strus/webRequestEventLoopInterface.hpp"
#ifndef _STRUS_BINDINGS_WEBREQUEST_EVENTLOOP_INTERFACE_HPP_INCLUDED
#define _STRUS_BINDINGS_WEBREQUEST_EVENTLOOP_INTERFACE_HPP_INCLUDED
#include <string>

namespace strus {

/// \brief Forward declaration
class WebRequestDelegateContextInterface;

class WebRequestEventLoopInterface
{
public:
	virtual ~WebRequestEventLoopInterface(){}

	/// \brief Start background thread
	/// \return true on success, false on error
	virtual bool start()=0;
	/// \brief Stop background thread without finishing idle jobs in the queue
	virtual void stop()=0;

	/// \brief Send a HTTP request to another strus webservice
	/// \note Pushes it as request on the queue processed by the event loop
	/// \note Used to delegate part of a request to a sub service
	/// \note Response is handled via a callback with a closure encapsulated in a receiver object
	/// \param[in] address where to send the request to
	/// \param[in] method request method
	/// \param[in] content content of the request
	/// \param[in] receiver delegate (callback with closure) (passed with ownership)
	/// \return true on success, false on error
	virtual bool send( const std::string& address,
			const std::string& method,
			const std::string& content,
			WebRequestDelegateContextInterface* receiver)=0;

	typedef void (*TickerFunction)( void* THIS);
	/// \brief Add a method to call on a ticker event
	/// \param[in] obj object bound to the method
	/// \param[in] func method function pointer
	/// \return true on success, false on memory allocation error
	virtual bool addTickerEvent( void* obj, TickerFunction func)=0;

	/// \brief Handle an exception
	/// \param[in] message of the exception
	virtual void handleException( const char* msg)=0;

	/// \brief Get the current time
	/// \return Get the current time in seconds (unixtime)
	/// \note Wrapper to empower tests to create their own emulation of time
	virtual long time() const=0;
};

}//namespace
#endif


