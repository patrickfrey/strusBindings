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
		LogError=0x1,
		LogWarning=0x2,
		LogInfo=0x4,
		LogTrace=0x8,
		LogAll=0xFFFF
	};

	/// \brief Destructor
	virtual ~WebRequestLoggerInterface(){}

	/// \brief Get the mask that tells what to log
	virtual int logMask() const=0;

	/// \brief Log a warning message
	/// \param[in] level log level of the message
	/// \param[in] tag tag identifier of the log message
	/// \param[in] msg message to log
	/// \param[in] msglen length of the message to log in bytes (strlen)
	/// \remark expected to be thread safe
	virtual void print( const Level level, const char* tag, const char* msg, size_t msglen)=0;
};

}//namespace
#endif

