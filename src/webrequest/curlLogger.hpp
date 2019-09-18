/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Wrapper to strus logger used in messaging based on cURL
/// \file "curlLogger.hpp"
#ifndef _STRUS_BINDINGS_CURL_LOGGER_HPP_INCLUDED
#define _STRUS_BINDINGS_CURL_LOGGER_HPP_INCLUDED
#include <string>

namespace strus {

/// \brief Forward declaration
class WebRequestLoggerInterface;

class CurlLogger {
public:
	enum LogType {LogFatal,LogError,LogWarning,LogInfo};

	explicit CurlLogger( WebRequestLoggerInterface* logger_)
		:m_logger(logger_),m_loglevel(evalLogLevel( logger_)){}
	~CurlLogger(){}

	void print( LogType logtype, const char* fmt, ...);
	LogType loglevel() const	{return m_loglevel;}

	void logState( const char* state, int arg);

private:
	static LogType evalLogLevel( WebRequestLoggerInterface* logger_);

private:
	WebRequestLoggerInterface* m_logger;
	LogType m_loglevel;
};

}//namespace
#endif

