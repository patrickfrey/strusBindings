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

bool WebRequestContext::executeListVariables()
{
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];
	char const** varlist = papuga_RequestContext_list_variables( m_context.get(), 1/*max inheritcnt*/, lstbuf, lstbufsize);
	if (!varlist)
	{
		setAnswer( ErrorCodeBufferOverflow);
		return false;
	}
	else if (!strus::mapStringArrayToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), STRUS_LIST_ROOT_ELEMENT, PAPUGA_HTML_LINK_ELEMENT, m_result_encoding, m_result_doctype, varlist))
	{
		return false;
	}
	return true;
}

bool WebRequestContext::executeSchemaDescription()
{
	if (isEqual( m_method,"GET"))
	{
		m_method = m_path.getNext();
		if (!m_method)
		{
			setAnswer( ErrorCodeRequestResolveError);
			return false;
		}
	}
	else
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
	SchemaId schemaId = getSchemaId();
	const papuga_SchemaDescription* descr = papuga_RequestHandler_get_description( m_handler->impl(), schemaId.contextType, schemaId.schemaName);
	if (!descr)
	{
		setAnswer( ErrorCodeRequestResolveError);
		return false;
	}
	else
	{
		papuga_ContentType schema_doctype = papuga_ContentType_Unknown;
		switch (m_result_doctype)
		{
			case WebRequestContent::Unknown:
			case WebRequestContent::HTML:
			case WebRequestContent::TEXT:
				setAnswer( ErrorCodeNotImplemented);
				return false;
			case WebRequestContent::JSON:
				schema_doctype = papuga_ContentType_JSON;
				break;
			case WebRequestContent::XML:
				schema_doctype = papuga_ContentType_XML;
				break;
		}
		std::size_t txtlen;
		const char* txt = (const char*)papuga_SchemaDescription_get_text( descr, &m_allocator, schema_doctype, m_result_encoding, &txtlen);
		if (txt)
		{
			WebRequestContent answerContent( papuga_stringEncodingName( m_result_encoding), WebRequestContent::typeMime(m_result_doctype), txt, txtlen);
			m_answer.setContent( answerContent);
			return true;
		}
		else
		{
			papuga_ErrorCode errcode = papuga_SchemaDescription_last_error( descr);
			setAnswer( papugaErrorToErrorCode( errcode));
			return false;
		}
	}
}

bool WebRequestContext::executeOPTIONS()
{
	enum {lstbufsize=256};
	char const* lstbuf[ lstbufsize];

	if (m_obj && m_obj->valuetype == papuga_TypeHostObject)
	{
		std::string http_allow("OPTIONS,");
		char const** sl = papuga_RequestHandler_list_schema_names( m_handler->impl(), m_contextType, lstbuf, lstbufsize);
		if (!sl)
		{
			setAnswer( ErrorCodeBufferOverflow);
			return false;
		}
		for (; *sl; ++sl) http_allow.append( *sl);
		int classid = m_obj->value.hostObject->classid;
		char const** ml = papuga_RequestHandler_list_methods( m_handler->impl(), classid, lstbuf, lstbufsize);
		if (!ml)
		{
			setAnswer( ErrorCodeBufferOverflow);
			return false;
		}
		for (; *ml; ++ml) http_allow.append( *ml);
		const char* msgstr = papuga_Allocator_copy_string( &m_allocator, http_allow.c_str(), http_allow.size());
		if (!msgstr)
		{
			setAnswer( ErrorCodeOutOfMem);
			return false;
		}
		m_answer.setMessage( 200/*OK*/, "Allow", msgstr);
		return true;
	}
	else if (m_contextName)
	{
		if (m_context.get())
		{
			std::string http_allow("OPTIONS,");
			char const** sl = papuga_RequestHandler_list_schema_names( m_handler->impl(), m_contextType, lstbuf, lstbufsize);
			if (!sl)
			{
				setAnswer( ErrorCodeBufferOverflow);
				return false;
			}
			for (; *sl; ++sl) http_allow.append( *sl);
			const char* msgstr = papuga_Allocator_copy_string( &m_allocator, http_allow.c_str(), http_allow.size());
			if (!msgstr)
			{
				setAnswer( ErrorCodeOutOfMem);
				return false;
			}
			m_answer.setMessage( 200/*OK*/, "Allow", msgstr);
			return true;
		}
		else
		{
			m_answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,PUT");
			return true;
		}
	}
	else
	{
		m_answer.setMessage( 200/*OK*/, "Allow", "OPTIONS,GET");
		return true;
	}
}


