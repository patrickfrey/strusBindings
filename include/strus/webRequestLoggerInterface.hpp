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
	enum Level
	{
		Fatal,
		Error,
		Warning,
		Info,
		Trace
	};

	static const char* levelName( const Level level)
	{
		static const char* ar[] = {"fatal","error","warning","info","trace"};
		return ar[ level];
	}

	/// \brief Destructor
	virtual ~WebRequestLoggerInterface(){}

	/// \brief Get the mask that tells what to log
	virtual int level() const noexcept=0;

	/// \brief Log a warning message
	/// \param[in] level log level of the message
	/// \param[in] tag tag identifier of the log message
	/// \param[in] msg message to log
	/// \param[in] msglen length of the message to log in bytes (strlen)
	/// \remark expected to be thread safe
	virtual void print( const Level level, const char* tag, const char* msg, size_t msglen) noexcept=0;
};

}//namespace
#endif

