/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for executing XML/JSON requests based on the strus bindings API
/// \file "webRequestHandlerInterface.hpp"
#ifndef _STRUS_WEB_REQUEST_LOGGER_INTERFACE_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_LOGGER_INTERFACE_HPP_INCLUDED
#include <cstddef>
#include <string>

namespace strus
{

/// \brief Interface for executing XML/JSON requests based on the strus bindings API
class WebRequestLoggerInterface
{
public:
	/// \brief Values joined as flags in an integer telling what is to log
	enum Mask
	{
		LogNothing=0x0,
		LogError=0x1,
		LogWarning=0x2,
		LogMethodCalls=0x4,
		LogRequests=0x8,
		LogDelegateRequests=0x10,
		LogConfiguration=0x20,
		LogAction=0x40,
		LogContentEvents=0x80,
		LogConnectionEvents=0x100,
		LogAll=0xFFFF
	};

	/// \brief Destructor
	virtual ~WebRequestLoggerInterface(){}

	/// \brief Get the mask that tells what to log
	virtual int logMask() const=0;

	/// \brief Get the depth of structures to log
	virtual int structDepth() const=0;

	/// \brief Log a request
	/// \param[in] content pointer to content string of the request
	/// \param[in] contentsize size of the content sent in bytes
	/// \remark expected to be thread safe
	virtual void logRequest( const char* content, std::size_t contentsize)=0;

	/// \brief Log a delegate http request
	/// \param[in] address address of the request URL with port and path
	/// \param[in] method http request method of the request
	/// \param[in] content pointer to content string of the request
	/// \param[in] contentsize size of the content sent in bytes
	/// \remark expected to be thread safe
	virtual void logDelegateRequest( const char* address, const char* method, const char* content, std::size_t contentsize)=0;

	/// \brief Log a loaded configuration
	/// \param[in] type type of the configuration
	/// \param[in] name name of the configuration
	/// \param[in] configstr content string of the configuration
	/// \remark expected to be thread safe
	virtual void logPutConfiguration( const char* type, const char* name, const std::string& configstr)=0;

	/// \brief Log an action
	/// \param[in] type type of the configuration
	/// \param[in] name name of the configuration
	/// \param[in] action string describing the action on the object
	/// \remark expected to be thread safe
	virtual void logAction( const char* type, const char* name, const char* action)=0;

	/// \brief Log an event parsing content
	/// \param[in] title name of the event
	/// \param[in] item type name of the item
	/// \param[in] content pointer to content string of the event
	/// \param[in] contentsize size of bytes of the content string
	/// \remark expected to be thread safe
	virtual void logContentEvent( const char* title, const char* item, const char* content, std::size_t contentsize)=0;

	/// \brief Log a method call
	/// \param[in] classname name identifier of the class called
	/// \param[in] methodname name identifier of the method called
	/// \param[in] arguments serialized arguments of the method called
	/// \param[in] result serialized result of the method called
	/// \param[in] resultsize size of bytes of the result string
	/// \param[in] resultvar name of the variable the result is assigned to
	/// \remark expected to be thread safe
	virtual void logMethodCall(
			const char* classname,
			const char* methodname,
			const char* arguments,
			const char* result,
			std::size_t resultsize,
			const char* resultvar)=0;

	/// \brief Log a verbose event from the network client library (cURL)
	/// \param[in] content content to log
	/// \remark expected to be thread safe
	virtual void logConnectionEvent( const char* content)=0;

	/// \brief Log a state of a delegate request connection handled by the network client library (cURL)
	/// \param[in] state name of the state to log
	/// \param[in] conn pointer to connection handle
	/// \param[in] ecode status code, depending on the network client library
	/// \remark expected to be thread safe
	virtual void logConnectionState( const char* state, void* conn, int ecode)=0;

	/// \brief Log a warning message
	/// \param[in] warnmsg warning message to log
	/// \remark expected to be thread safe
	virtual void logWarning( const char* warnmsg)=0;

	/// \brief Log an error
	/// \param[in] errmsg error message to log
	/// \remark expected to be thread safe
	virtual void logError( const char* errmsg)=0;
};

}//namespace
#endif

