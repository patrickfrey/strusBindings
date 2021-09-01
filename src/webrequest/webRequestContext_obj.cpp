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
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/base/string_format.hpp"
#include "strus/lib/error.hpp"
#include "papuga/schema.h"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

static inline bool isEqual( const char* name, const char* oth)
{
	return name[0] == oth[0] && 0==std::strcmp(name,oth);
}

void WebRequestContext::resetContext()
{
	m_transactionRef.reset();
	m_context.reset();
}

bool WebRequestContext::initContext()
{
	m_transactionRef.reset();
	m_context.create();
	if (!m_context.get())
	{
		setAnswer( ErrorCodeOutOfMem);
		return false;
	}
	if (!papuga_RequestContext_inherit( m_context.get(), m_handler->contextPool(), m_contextType, m_contextName))
	{
		papuga_ErrorCode errcode = papuga_RequestContext_last_error( m_context.get(), true);
		setAnswer( papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}

bool WebRequestContext::initRequestContext()
{
	resetContext();
	if (m_path.startsWith( "transaction", 11/*"transaction"*/))
	{
		// Fetch transaction object from pool with exclusive ownership:
		m_contextType = m_path.getNext();
		m_contextName = m_path.getNext();
		if (!m_contextName)
		{
			setAnswer( ErrorCodeIncompleteRequest);
			return false;
		}
		m_transactionRef = m_transactionPool->fetchTransaction( m_contextName);
		if (!m_transactionRef.get())
		{
			setAnswer( ErrorCodeRequestResolveError);
			return false;
		}
		m_contextType = m_transactionRef->contextType();
		m_context = m_transactionRef->context();
	}
	else if (m_path.startsWith( "schema", 6/*"schema"*/))
	{
		m_contextType = m_path.getNext();
		m_contextName = m_path.getNext();
	}
	else
	{
		if (!(m_contextType = m_path.getNext())) return true;
		if (isEqual( m_contextType, ROOT_CONTEXT_NAME))
		{
			m_contextName = ROOT_CONTEXT_NAME;
		}
		else
		{
			m_contextName = m_path.getNext();
			if (!m_contextName) return true;
		}
		initContext();
	}
	return true;
}

