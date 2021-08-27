/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Delegate request sink for configuration update
/// \file "configurationUpdateRequest.cpp"
#include "configurationUpdateRequest.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/webRequestHandlerInterface.hpp"
#include "strus/webRequestContextInterface.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/lib/error.hpp"
#include "private/internationalization.hpp"
#include "private/errorUtils.hpp"
#include <iostream>
#include <cstdio>

using namespace strus;

void ConfigurationUpdateRequestContext::logErrorFmt( const char* fmt, ...)
{
	char buf[ 4096];
	va_list ap;
	va_start( ap, fmt);
	if ((int)sizeof(buf) <= std::vsnprintf( buf, sizeof(buf), fmt, ap)) {buf[ sizeof(buf)-1] = 0;}
	m_logger->logError( buf);
	va_end(ap);
}

void ConfigurationUpdateRequestContext::putAnswer( const WebRequestAnswer& status)
{
	try
	{
		if (status.httpStatus() < 200 || status.httpStatus() >= 300)
		{
			if (0!=(m_logMask & WebRequestLoggerInterface::LogError))
			{
				if (status.errorStr())
				{
					logErrorFmt( _TXT("error in delegate request (http status %d): %s"), status.httpStatus(), status.errorStr());
				}
				else
				{
					logErrorFmt( _TXT("error in delegate request (http status %d)"), status.httpStatus());
				}
			}
		}
		if (!m_context->putDelegateRequestAnswer( m_schema, status))
		{
			if (0!=(m_logMask & WebRequestLoggerInterface::LogError))
			{
				WebRequestAnswer ctxstatus = m_context->getAnswer();
				const char* msg = ctxstatus.errorStr() ? ctxstatus.errorStr() : strus::errorCodeToString( ctxstatus.appErrorCode());
				logErrorFmt( _TXT("error processing delegate request answer with schema '%s': %s"), m_schema, msg);
			}
		}
	}
	catch (const std::bad_alloc&)
	{
		if (0!=(m_logMask & WebRequestLoggerInterface::LogError))
		{
			logErrorFmt( _TXT("error returning delegate request answer: %s"), _TXT("memory allocation error"));
		}
	}
	catch (const std::runtime_error& err)
	{
		if (0!=(m_logMask & WebRequestLoggerInterface::LogError))
		{
			logErrorFmt( _TXT("error returning delegate request answer: %s"), err.what());
		}
	}
	m_counter->decrement( 1);
	if (m_counter->value() <= 0)
	{
		if (0!=(m_logMask & WebRequestLoggerInterface::LogConnectionEvents))
		{
			m_logger->logConnectionState( "completed delegate requests", m_requestCount);
		}
	}
}


