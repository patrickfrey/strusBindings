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
#include "private/internationalization.hpp"
#include "private/errorUtils.hpp"
#include <iostream>
#include <cstdio>

using namespace strus;

void ConfigurationUpdateRequestContext::putAnswer( const WebRequestAnswer& status)
{
	char errbuf[ 1024];
	try
	{
		const char* accepted_charset = "UTF-8";
		const char* accepted_doctype = "application/json";
	
		if (status.httpstatus() < 200 || status.httpstatus() >= 300)
		{
			if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogError))
			{
				if (status.errorstr())
				{
					std::snprintf( errbuf, sizeof(errbuf), _TXT("error in delegate request (http status %d): %s"), status.httpstatus(), status.errorstr());
				}
				else
				{
					std::snprintf( errbuf, sizeof(errbuf), _TXT("error in delegate request (http status %d)"), status.httpstatus());
				}
				m_logger->logError( errbuf);
			}
		}
		else if (!status.content().empty())
		{
			if (0!=std::strcmp( status.content().charset(), accepted_charset)
			||  0!=std::strcmp( status.content().doctype(), accepted_doctype))
			{
				throw strus::runtime_error( _TXT("expected delegate request to be JSON/UTF-8"));
			}
			WebRequestAnswer ctxstatus;
			strus::local_ptr<WebRequestContextInterface> ctx( m_handler->createContext( accepted_charset, accepted_doctype, ""/*html_base_href*/, ctxstatus));
			if (!ctx.get())
			{
				if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogError))
				{
					std::snprintf( errbuf, sizeof(errbuf), _TXT("error returning delegate request answer: %s"), ctxstatus.errorstr());
					errbuf[ sizeof(errbuf)-1] = 0;
					m_logger->logError( errbuf);
				}
				return;
			}
			strus::WebRequestContent subcontent( accepted_charset, accepted_doctype, status.content().str(), status.content().len());

			if (!ctx->pushConfigurationDelegateRequestAnswer( m_type.c_str(), m_name.c_str(), m_schema.c_str(), subcontent, ctxstatus))
			{
				if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogError))
				{
					std::snprintf( errbuf, sizeof(errbuf), _TXT("error returning delegate request answer: %s"), ctxstatus.errorstr());
					errbuf[ sizeof(errbuf)-1] = 0;
					m_logger->logError( errbuf);
				}
				return;
			}
			*m_counter -= 1;
			if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogConnectionEvents) && *m_counter == 0)
			{
				m_logger->logConnectionState( "completed delegate requests", m_requestCount);
			}
		}
	}
	catch (const std::bad_alloc&)
	{
		if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogError))
		{
			std::snprintf( errbuf, sizeof(errbuf), _TXT("error returning delegate request answer: %s"), _TXT("memory allocation error"));
			errbuf[ sizeof(errbuf)-1] = 0;
			m_logger->logError( errbuf);
		}
	}
	catch (const std::runtime_error& err)
	{
		if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogError))
		{
			std::snprintf( errbuf, sizeof(errbuf), _TXT("error returning delegate request answer: %s"), err.what());
			errbuf[ sizeof(errbuf)-1] = 0;
			m_logger->logError( errbuf);
		}
	}
}

