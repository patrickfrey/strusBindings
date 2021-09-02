/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, functions for accesing the result or answer of the request
/// \file "webRequestContext_result.cpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/errorCodes.hpp"
#include "strus/base/fileio.hpp"
#include "strus/lib/error.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/requestContext.h"
#include "papuga/typedefs.h"
#include "papuga/constants.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

void WebRequestContext::setAnswer( int errcode, const char* errstr, bool doCopy)
{
	int httpstatus = errorCodeToHttpStatus( (ErrorCode)errcode);
	if (errstr)
	{
		m_answer.setError( httpstatus, errcode, errstr, doCopy);
	}
	else
	{
		m_answer.setError( httpstatus, errcode, strus::errorCodeToString(errcode));
	}
}

bool WebRequestContext::transferContext()
{
	if (m_context.use_count() != 1)
	{
		setAnswer( ErrorCodeLogicError, _TXT("transferred configuration object not singular (referenced twice)"));
		return false;
	}
	if (!m_handler->transferContext( m_contextType, m_contextName, m_context.release(), m_answer))
	{
		return false;
	}
	return true;
}



