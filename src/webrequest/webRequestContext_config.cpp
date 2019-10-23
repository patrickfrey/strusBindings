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
	if (0!=(m_logMask & WebRequestLoggerInterface::LogAction))
	{
		m_logger->logAction( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, "load main configuration");
	}
	if (!initContentSchemaAutomaton(SchemaId( "", ROOT_CONTEXT_NAME))) return false;
	if (!executeContentSchemaAutomaton( content)) return false;
	return true;
}

bool WebRequestContext::loadEmbeddedConfiguration( const WebRequestContent& content)
{
	if (!initRootObject()) return false;
	if (0!=(m_logMask & WebRequestLoggerInterface::LogAction))
	{
		m_logger->logAction( m_contextType, m_contextName, "load embedded configuration");
	}
	if (!initContentSchemaAutomaton( getSchemaId( m_contextType, "PUT"))) return false;
	if (!executeContentSchemaAutomaton( content)) return false;
	return true;
}

bool WebRequestContext::loadConfigurationRequest( const WebRequestContent& content)
{
	if (isEqual( m_contextType, ROOT_CONTEXT_NAME))
	{
		setAnswer( ErrorCodeInvalidRequest);
		return false;
	}
	std::string configstr = webRequestContent_tostring( content);
	ConfigurationDescription cfgdescr( m_contextType, m_contextName, m_method, content.doctype(), configstr);
	m_configHandler->storeConfiguration( m_configTransaction, cfgdescr);
	SchemaId schemaid = getSchemaId();
	if (!initRootObject()) return false;
	if (!initContentSchemaAutomaton( schemaid)) return false;
	if (!executeContentSchemaAutomaton( content)) return false;

	if (0!=(m_logMask & WebRequestLoggerInterface::LogConfiguration))
	{
		m_logger->logPutConfiguration( m_contextType, m_contextName, configstr);
	}
	return true;
}

bool WebRequestContext::updateConfigurationRequest( const WebRequestContent& content)
{
	if (isEqual( m_contextType, ROOT_CONTEXT_NAME))
	{
		setAnswer( ErrorCodeInvalidRequest);
		return false;
	}
	std::string configstr = webRequestContent_tostring( content);
	ConfigurationDescription cfgdescr( m_contextType, m_contextName, m_method, content.doctype(), configstr);
	m_configHandler->storeConfiguration( m_configTransaction, cfgdescr);
	if (0!=(m_logMask & WebRequestLoggerInterface::LogConfiguration))
	{
		m_logger->logPutConfiguration( m_contextType, m_contextName, configstr);
	}
	SchemaId schemaid = getSchemaId();
	if (!initContentSchemaAutomaton( schemaid)) return false;
	if (papuga_RequestAutomaton_has_exclusive_access( m_atm))
	{
		if (m_requestType == ObjectRequest)
		{
			m_requestType = InterruptedLoadConfigurationRequest;
			setAnswer( ErrorCodeServiceNeedExclusiveAccess);
			return false;
		}
		else
		{
			setAnswer( ErrorCodeLogicError, _TXT("illegal state in update configuration request"));
			return false;
		}
	}
	else
	{
		if (!executeContentSchemaAutomaton( content)) return false;
	}
	return true;
}

bool WebRequestContext::updateConfigurationRequest_retry( const WebRequestContent& content)
{
	if (!executeContentSchemaAutomaton( content)) return false;
	return true;
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
	if (0!=(m_logMask & WebRequestLoggerInterface::LogAction))
	{
		m_logger->logAction( m_contextType, m_contextName, "removed");
	}
	ConfigurationDescription config = m_configHandler->getStoredConfiguration( m_contextType, m_contextName);
	if (config.valid())
	{
		std::string deleteMethodName = std::string( "DELETE_") + config.method;
		SchemaId schemaid = getSchemaId( m_contextType, deleteMethodName.c_str());
		if (!initRootObject()) return false;
		if (papuga_RequestHandler_get_automaton( m_handler->impl(), schemaid.contextType, schemaid.schemaName))
		{
			WebRequestContent content( "UTF-8", config.doctype.c_str(), config.contentbuf.c_str(), config.contentbuf.size());
			if (!initContentType( content)) return false;
			if (!initContentSchemaAutomaton( schemaid)) return false;
			if (!executeContentSchemaAutomaton( content)) return false;
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
		m_answer.setHttpStatus( 204/*no content*/);
		return true;
	}
	else
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
}

