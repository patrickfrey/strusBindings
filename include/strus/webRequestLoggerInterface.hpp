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
		LogConfiguration=0x10,
		LogAction=0x20,
		LogContentEvents=0x40,
		LogAll=0xFF
	};

	/// \brief Destructor
	virtual ~WebRequestLoggerInterface(){}

	/// \brief Get the mask that tells what to log
	virtual int logMask() const=0;

	/// \brief Get the depth of structures to log
	virtual int structDepth() const=0;

	/// \brief Log a request
	/// \remark expected to be thread safe
	virtual void logRequest( const char* reqstr)=0;

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
	/// \param[in] value value of the event
	/// \remark expected to be thread safe
	virtual void logContentEvent( const char* title, const char* item, const char* value)=0;

	/// \brief Log a method call
	/// \param[in] classname name identifier of the class called
	/// \param[in] methodname name identifier of the method called
	/// \param[in] arguments serialized arguments of the method called
	/// \param[in] result serialized result of the method called
	/// \param[in] resultvar name of the variable the result is assigned to
	/// \remark expected to be thread safe
	virtual void logMethodCall(
			const char* classname,
			const char* methodname,
			const char* arguments,
			const char* result,
			const char* resultvar)=0;

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

