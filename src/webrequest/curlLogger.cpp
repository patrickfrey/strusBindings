/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Wrapper to strus logger used in messaging based on cURL
/// \file "curlLogger.cpp"
#include "curlLogger.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include <cstring>
#include <string>
#include <cstdarg>

using namespace strus;

void CurlLogger::print( LogType logtype, const char* fmt, ...)
{
	char msgbuf[ 4096];
	va_list ap;
	va_start( ap, fmt);
	if ((int)sizeof(msgbuf) <= std::vsnprintf( msgbuf, sizeof(msgbuf), fmt, ap)) msgbuf[ sizeof(msgbuf)-1] = 0;
	va_end( ap);

	switch (logtype)
	{
		case LogFatal:
			m_logger->print( WebRequestLoggerInterface::LogError, "curl fatal", msgbuf, std::strlen(msgbuf));
			break;
		case LogError:
			m_logger->print( WebRequestLoggerInterface::LogError, "curl error", msgbuf, std::strlen(msgbuf));
			break;
		case LogWarning:
			m_logger->print( WebRequestLoggerInterface::LogWarning, "curl warning", msgbuf, std::strlen(msgbuf));
			break;
		case LogInfo:
			m_logger->print( WebRequestLoggerInterface::LogInfo, "curl", msgbuf, std::strlen(msgbuf));
			break;
	}
}

void CurlLogger::logState( const char* state, int arg)
{
	if ((m_logger->logMask() & WebRequestLoggerInterface::LogTrace) != 0)
	{
		char buf[ 1024];
		std::snprintf( buf, sizeof(buf), "%s %d", state, arg);
		buf[ sizeof(buf)-1] = 0;
		m_logger->print( WebRequestLoggerInterface::LogTrace, "curl state", buf, std::strlen(buf));
	}
}

CurlLogger::LogType CurlLogger::evalLogLevel( WebRequestLoggerInterface* logger_)
{
	if ((logger_->logMask() & WebRequestLoggerInterface::LogInfo) != 0)
	{
		return LogInfo;
	}
	else if ((logger_->logMask() & WebRequestLoggerInterface::LogWarning) != 0)
	{
		return LogWarning;
	}
	else if ((logger_->logMask() & WebRequestLoggerInterface::LogError) != 0)
	{
		return LogError;
	}
	else
	{
		return LogFatal;
	}
}

