/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, initialization, managing and deinitialization of handler and context objects in the request
/// \file "webRequestContext_result.cpp"
#include "webRequestContext.hpp"
#include "webRequestUtils.hpp"
#include "webRequestHandler.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "schemas_base.hpp"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/typedefs.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

static inline bool isEqual( const char* name, const char* oth)
{
	return name[0] == oth[0] && 0==std::strcmp(name,oth);
}

bool WebRequestContext::loadMainConfiguration( const WebRequestContent& content)
{
	if (!initEmptyObject()) return false;
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "configuration", "load", 0, 0);
	}
	return executeContentSchemaRequest( SchemaId( "", ROOT_CONTEXT_NAME), content);
}

bool WebRequestContext::loadEmbeddedConfiguration( const WebRequestContent& content)
{
	if (!initRootObject()) return false;
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "configuration", "embedded", m_contextType, m_contextName);
	}
	return executeContentSchemaRequest( getSchemaId( m_contextType, "PUT"), content);
}

bool WebRequestContext::loadConfigurationRequest( const WebRequestContent& content)
{
	if (isEqual( m_contextType, ROOT_CONTEXT_NAME))
	{
		setAnswer( ErrorCodeInvalidRequest);
		return false;
	}
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "configuration", m_method, m_contextType, m_contextName);
	}
	std::string configstr = webRequestContent_tostring( content);
	ConfigurationDescription cfgdescr( m_contextType, m_contextName, m_method, content.doctype(), configstr);
	m_configHandler->storeConfiguration( m_configTransaction, cfgdescr);
	SchemaId schemaid = getSchemaId();
	if (!initRootObject()) return false;
	if (executeContentSchemaRequest( schemaid, content))
	{
		if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogConfiguration))
		{
			m_logger->logPutConfiguration( m_contextType, m_contextName, configstr);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool WebRequestContext::deleteConfigurationRequest()
{
	if (isEqual( m_contextType, ROOT_CONTEXT_NAME))
	{
		setAnswer( ErrorCodeInvalidRequest);
		return false;
	}
	if (!m_handler->removeContext( m_contextType, m_contextName, m_answer))
	{
		return false;
	}
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "configuration", m_method, m_contextType, m_contextName);
	}
	ConfigurationDescription config = m_configHandler->getStoredConfiguration( m_contextType, m_contextName);
	if (config.valid())
	{
		std::string deleteMethodName = std::string( "DELETE_") + config.method;
		SchemaId schemaid = getSchemaId( ROOT_CONTEXT_NAME, deleteMethodName.c_str());
		if (!initRootObject()) return false;
		if (papuga_RequestHandler_get_automaton( m_handler->impl(), schemaid.contextType, schemaid.schemaName))
		{
			WebRequestContent content( "UTF-8", config.doctype.c_str(), config.contentbuf.c_str(), config.contentbuf.size());
			if (!executeContentSchemaRequest( schemaid, content))
			{
				return false;
			}
			if (hasContentRequestDelegateRequests())
			{
				if (!!(m_logMask & WebRequestLoggerInterface::LogWarning))
				{
					m_logger->logWarning( _TXT("delete configuration schema with delegate requests that are ignored"));
				}
			}
		}
		m_configHandler->deleteStoredConfiguration( config.type.c_str(), config.name.c_str());
		if (!!(m_logMask & WebRequestLoggerInterface::LogAction))
		{
			m_logger->logAction( config.type.c_str(), config.name.c_str(), _TXT("deleted configurations stored"));
		}
	}
	m_answer.setStatus( 204/*no content*/);
	return true;
}

