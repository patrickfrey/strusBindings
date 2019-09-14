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
#include <string>
#include <cstdarg>

using namespace strus;

void CurlLogger::print( LogType logtype, const char* fmt, ...)
{
	char msgbuf[ 1024];
	va_list ap;
	va_start( ap, fmt);
	if ((int)sizeof(msgbuf) <= std::vsnprintf( msgbuf, sizeof(msgbuf), fmt, ap)) msgbuf[ sizeof(msgbuf)-1] = 0;
	va_end( ap);

	switch (logtype)
	{
		case LogFatal:
			m_logger->logError( msgbuf);
			break;
		case LogError:
			m_logger->logError( msgbuf);
			break;
		case LogWarning:
			m_logger->logWarning( msgbuf);
			break;
		case LogInfo:
			m_logger->logConnectionEvent( msgbuf);
			break;
	}
}

CurlLogger::LogType CurlLogger::evalLogLevel( WebRequestLoggerInterface* logger_)
{
	if ((logger_->logMask() & WebRequestLoggerInterface::LogConnectionEvents) != 0)
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


