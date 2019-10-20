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
		if (status.httpstatus() < 200 || status.httpstatus() >= 300)
		{
			if (0!=(m_logMask & WebRequestLoggerInterface::LogError))
			{
				if (status.errorstr())
				{
					logErrorFmt( _TXT("error in delegate request (http status %d): %s"), status.httpstatus(), status.errorstr());
				}
				else
				{
					logErrorFmt( _TXT("error in delegate request (http status %d)"), status.httpstatus());
				}
			}
		}
		else if (!status.content().empty())
		{
			if (!m_context->putDelegateRequestAnswer( m_schema, status.content()))
			{
				if (0!=(m_logMask & WebRequestLoggerInterface::LogError))
				{
					WebRequestAnswer ctxstatus = m_context->getAnswer();
					const char* msg = ctxstatus.errorstr() ? ctxstatus.errorstr() : strus::errorCodeToString( ctxstatus.apperror());
					logErrorFmt( _TXT("error processing delegate request answer with schema '%s': %s"), m_schema, msg);
				}
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


