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
		LogMethodCalls=0x1,
		LogRequests=0x2,
		LogAll=0xFF
	};

	/// \brief Destructor
	virtual ~WebRequestLoggerInterface(){}

	/// \brief Get the mask that tells what to log
	virtual int logMask() const=0;

	/// \brief Get the depth of structures to log
	virtual int structDepth() const=0;

	/// \brief Log a request
	virtual void logRequest( const char* reqstr)=0;

	/// \brief Log a method call
	/// \param[in] classname name identifier of the class called
	/// \param[in] methodname name identifier of the method called
	/// \param[in] arguments serialized arguments of the method called
	/// \param[in] result serialized result of the method called
	virtual void logMethodCall(
			const std::string& classname,
			const std::string& methodname,
			const std::string& arguments,
			const std::string& result)=0;

	/// \brief Log an error during logging
	/// \param[in] errmsg error message to log
	virtual void logLoggerError( const char* errmsg)=0;
};

}//namespace
#endif

