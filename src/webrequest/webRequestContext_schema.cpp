/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, functions for executing schema content requests
/// \file "webRequestContext_schema.cpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/lib/error.hpp"
#include "schemas_base.hpp"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

static const char* concatSchemaName( papuga_Allocator* allocator, const char* prefix, char sep, const char* tail)
{
	std::size_t len = std::strlen( prefix) + std::strlen( tail) + 1/*sep*/;
	char* rt = (char*)papuga_Allocator_alloc( allocator, len, 1);
	if (!rt) throw std::bad_alloc();
	std::snprintf( rt, len+1, "%s%c%s", prefix, sep, tail);
	return rt;
}

static inline bool isEqual( const char* name, const char* oth)
{
	return name[0] == oth[0] && 0==std::strcmp(name,oth);
}

static inline bool startsWith( const char* name, const char* oth, std::size_t othlen)
{
	return name[0] == oth[0] && 0==std::memcmp(name,oth,othlen);
}

WebRequestContext::SchemaId WebRequestContext::getSchemaId( const char* contextType_, WebRequestContext::MethodId methodId_)
{
	if (methodId_ == Method_POST || methodId_ == Method_PUT || methodId_ == Method_DELETE)
	{
		return SchemaId( ROOT_CONTEXT_NAME, concatSchemaName( &m_allocator, methodIdName(methodId_), '/', contextType_));
	}
	else
	{
		return SchemaId( contextType_, methodIdName(methodId_));
	}
}

WebRequestContext::SchemaId WebRequestContext::getSchemaId()
{
	if (m_path.hasMore())
	{
		const char* pt = m_path.getRest();
		return SchemaId( m_contextType, concatSchemaName( &m_allocator, methodIdName(m_methodId), '~', pt));
	}
	else
	{
		return getSchemaId( m_contextType, m_methodId);
	}
}

WebRequestContext::SchemaId WebRequestContext::getSchemaId_updateConfiguration( const char* contextType_)
{
	return SchemaId( contextType_, "PUT");
}

WebRequestContext::SchemaId WebRequestContext::getSchemaId_deleteConfiguration( const char* contextType_, const char* createConfigurationMethod)
{
	char mbuf[ 32];
	std::snprintf( mbuf, sizeof(mbuf), "DELETE_%s", createConfigurationMethod);
	mbuf[ sizeof( mbuf)-1] = 0;
	return SchemaId( ROOT_CONTEXT_NAME, concatSchemaName( &m_allocator, mbuf, '/', contextType_));
}

WebRequestContext::SchemaId WebRequestContext::getSchemaId_description()
{
	const char* mt = m_path.getNext();
	if (!mt)
	{
		setAnswer( ErrorCodeIncompleteRequest, _TXT("method not defined in schema description request"));
		return WebRequestContext::SchemaId();
	}
	const char* ct = m_path.getNext();
	if (!ct)
	{
		setAnswer( ErrorCodeIncompleteRequest, _TXT("context type not defined in schema description request"));
		return WebRequestContext::SchemaId();
	}
	if (m_path.hasMore())
	{
		const char* pt = m_path.getRest();
		return SchemaId( ct, concatSchemaName( &m_allocator, mt, '~', pt));
	}
	if (0==std::memcmp(mt,"DELETE_",7) || 0==std::strcmp(mt,"POST") || 0==std::strcmp(mt,"PUT") || 0==std::strcmp(mt,"DELETE"))
	{
		return SchemaId( ROOT_CONTEXT_NAME, concatSchemaName( &m_allocator, mt, '/', ct));
	}
	else
	{
		return SchemaId( ct, mt);
	}
}

bool WebRequestContext::hasContentSchemaAutomaton( const SchemaId& schemaid)
{
	return !!papuga_RequestHandler_get_automaton( m_handler->impl(), schemaid.contextType, schemaid.schemaName);
}

bool WebRequestContext::initContentSchemaAutomaton( const SchemaId& schemaid)
{
	m_atm = papuga_RequestHandler_get_automaton( m_handler->impl(), schemaid.contextType, schemaid.schemaName);
	if (!m_atm)
	{
		strus::ErrorCode ec = ErrorCodeRequestResolveError;
		if (!schemaid.contextType[0])
		{
			m_answer.setError_fmt( errorCodeToHttpStatus(ec), ec, _TXT("unknown schema '%s' in root context"), schemaid.schemaName);
		}
		else
		{
			m_answer.setError_fmt( errorCodeToHttpStatus(ec), ec, _TXT("unknown schema '%s' for '%s'"), schemaid.schemaName, schemaid.contextType);
		}
		return false;
	}
	if (0!=(m_logMask & WebRequestLoggerInterface::LogRequests))
	{
		m_logger->logRequestType( "content schema request", schemaid.schemaName, m_contextType, m_contextName);
	}
	return true;
}

bool WebRequestContext::initContentSchemaRequest()
{
	if (m_request) papuga_destroy_Request( m_request);
	m_request = papuga_create_Request( m_atm, &m_callLogger);
	if (!m_request)
	{
		setAnswer( ErrorCodeOutOfMem);
		return false;
	}
	return true;
}

bool WebRequestContext::inheritContentSchemaRequestContext()
{
	papuga_ErrorCode errcode = papuga_Ok;
	if (!m_request)
	{
		setAnswer( ErrorCodeOperationOrder);
		return false;
	}
	papuga_RequestInheritedContextDef const* di = papuga_Request_get_inherited_contextdefs( m_request, &errcode);
	if (!di)
	{
		setAnswer( papugaErrorToErrorCode( errcode));
		return false;
	}
	for (; di->type && di->name; ++di)
	{
		if (!inheritRequestContext( di->type, di->name)) return false;
	}
	return true;
}

bool WebRequestContext::feedContentSchemaRequest( const WebRequestContent& content)
{
	if (content.len() == 0)
	{
		setAnswer( ErrorCodeIncompleteRequest, _TXT("request content is empty"));
		return false;
	}
	// Log the request:
	papuga_ErrorCode errcode = papuga_Ok;
	// Parse the request:
	papuga_RequestParser* parser = papuga_create_RequestParser( &m_allocator, m_doctype, m_encoding, content.str(), content.len(), &errcode);
	if (!parser)
	{
		setAnswer( papugaErrorToErrorCode( errcode));
		return false;
	}
	if (!papuga_RequestParser_feed_request( parser, m_request, &errcode))
	{
		int itemid = papuga_Request_last_error_itemid( m_request);
		const char* itemname = 0;
		if (itemid >= 0)
		{
			itemname = webrequest::AutomatonNameSpace::itemName( (webrequest::AutomatonNameSpace::Item)itemid);
		}
		char buf[ 2048];
		int pos = papuga_RequestParser_get_position( parser, buf, sizeof(buf));
		if (pos > 0)
		{
			if (itemname)
			{
				papuga_ErrorBuffer_reportError( &m_errbuf, _TXT( "error feeding request item %s at position %d: %s, location: %s"), itemname, pos, papuga_ErrorCode_tostring( errcode), buf);
			}
			else
			{
				papuga_ErrorBuffer_reportError( &m_errbuf, _TXT( "error feeding request at position %d: %s, location: %s"), pos, papuga_ErrorCode_tostring( errcode), buf);
			}
		}
		else
		{
			if (itemname)
			{
				papuga_ErrorBuffer_reportError( &m_errbuf, _TXT( "error feeding request item %s"), itemname, papuga_ErrorCode_tostring( errcode));
			}
			else
			{
				papuga_ErrorBuffer_reportError( &m_errbuf, _TXT( "error feeding request: %s"), papuga_ErrorCode_tostring( errcode));
			}
		}
		papuga_destroy_RequestParser( parser);

		setAnswer( papugaErrorToErrorCode( errcode), papuga_ErrorBuffer_lastError( &m_errbuf), true);
		return false;
	}
	papuga_destroy_RequestParser( parser);
	return true;
}


bool WebRequestContext::executeContentSchemaCalls( const WebRequestContent& content)
{
	papuga_RequestError reqerr;
	if (!papuga_RequestContext_execute_request( m_context.get(), m_request, &m_allocator, &m_callLogger, &m_results, &m_nofResults, &reqerr))
	{
		int apperr = 0;
		if (reqerr.errormsg[0])
		{
			char const* errmsgitr = reqerr.errormsg;
			apperr = strus::errorCodeFromMessage( errmsgitr);
			if (apperr) removeErrorCodesFromMessage( reqerr.errormsg);
		}
		if (!apperr) apperr = papugaErrorToErrorCode( reqerr.errcode);
		reportRequestError( reqerr, content);
		setAnswer( apperr, m_errbuf.ptr, true);
		return false;
	}
	return true;
}

bool WebRequestContext::executeContentSchemaAutomaton( const WebRequestContent& content)
{
	if (!initSchemaEnvAssignments()) return false;
	if (!initContentSchemaRequest()) return false;
	if (!feedContentSchemaRequest( content)) return false;
	if (!inheritContentSchemaRequestContext()) return false;
	if (!executeContentSchemaCalls( content)) return false;
	return true;
}

