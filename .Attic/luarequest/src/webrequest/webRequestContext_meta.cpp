/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, methods to execute meta introspection requests, e.g. OPTIONS, list of variables, schema description
/// \file "webRequestContext_result.cpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
#include "papuga/encoding.h"
#include "papuga/constants.h"
#include "papuga/schema.h"
#include "papuga/luaRequestHandler.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

bool WebRequestContext::executeListVariables()
{
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "introspection", "variables", m_contextType, m_contextName);
	}
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];
	char const** varlist = papuga_RequestContext_list_variables( m_context.get(), 1/*max inheritcnt*/, lstbuf, lstbufsize);
	if (!varlist)
	{
		setAnswer( ErrorCodeBufferOverflow);
		return false;
	}
	bool beautified = m_handler->beautifiedOutput();
	if (!strus::mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, beautified, varlist))
	{
		return false;
	}
	return true;
}

bool WebRequestContext::executeListSchemas()
{
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "schema", "GET", "schema", 0);
	}
	enum {MaxNofSchemas=128};
	char const* buf[ MaxNofSchemas];
	const char** schemaNames = papuga_SchemaList_get_names( m_handler->schemaList(), buf, MaxNofSchemas);
	if (!schemaNames)
	{
		setAnswer( ErrorCodeBufferOverflow);
		return false;
	}
	bool beautified = m_handler->beautifiedOutput();
	if (!strus::mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, beautified, schemaNames))
	{
		return false;
	}
	return true;
}

bool WebRequestContext::executeSchemaDescription( const char* schemaName)
{
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "schema", "GET", "schema", schemaName);
	}
	papuga_SchemaSource const* schema = papuga_SchemaList_get( m_handler->schemaList(), schemaName);
	if (!schema)
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
	else
	{
		WebRequestContent answerContent( papuga_stringEncodingName( m_result_encoding), WebRequestContent::typeMime(m_result_doctype), schema->source, std::strlen(schema->source));
		m_answer.setContent( answerContent);
		return true;
	}
}

bool WebRequestContext::executeOPTIONS()
{
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "options", "OPTIONS", m_contextType, m_contextName);
	}
	if (m_contextType)
	{
		for (auto script : m_handler->scripts())
		{
			if (0==std::strcmp( papuga_LuaRequestHandlerScript_name( script), m_contextType))
			{
				std::string options("OPTIONS,");
				options.append( papuga_LuaRequestHandlerScript_options( script));
				m_answer.setMessage( 200/*OK*/, "Allow", options.c_str(), true);
				return true;
			}
		}
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
	else
	{
		m_answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,GET");
		return true;
	}
}
