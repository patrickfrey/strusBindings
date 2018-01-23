/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestHandler.cpp"
#include "webRequestHandler.hpp"
#include "webRequestContext.hpp"
#include "webRequestUtils.hpp"
#include "strus/errorCodes.hpp"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
#include "private/internationalization.hpp"

using namespace strus;

WebRequestHandler::WebRequestHandler()
{
	m_impl = papuga_create_RequestHandler();
	if (!m_impl) throw std::bad_alloc();
}

WebRequestHandler::~WebRequestHandler()
{
	papuga_destroy_RequestHandler( m_impl);
}

WebRequestContextInterface* WebRequestHandler::createRequestContext(
			const char* schema,
			const char* role,
			WebRequestAnswer& status) const
{
	papuga_ErrorCode errcode = papuga_Ok;
	try
	{
		return new WebRequestContext( m_impl, schema, role);
	}
	catch (const std::bad_alloc&)
	{
		errcode = papuga_NoMemError;
	}
	catch (const WebRequestContext::Exception& err)
	{
		errcode = err.errcode();
	}
	catch (...)
	{
		errcode = papuga_UncaughtException;
	}
	ErrorCause errcause = papugaErrorToErrorCause( errcode);
	const char* errstr = papuga_ErrorCode_tostring( errcode);
	int httpstatus = errorCauseToHttpStatus( errcause);
	status.setError( httpstatus, *ErrorCode( StrusComponentWebService, ErrorOperationBuildData, errcause), errstr);
	return NULL;
}


