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
			int* errcode) const
{
	try
	{
		return new WebRequestContext( m_impl, schema, role);
	}
	catch (const std::bad_alloc&)
	{
		*errcode = papuga_NoMemError;
	}
	catch (const WebRequestContext::Exception& err)
	{
		*errcode = err.errcode();
	}
	catch (...)
	{
		*errcode = papuga_UncaughtException;
	}
	return NULL;
}

const char* WebRequestHandler::errorstring( int errcode) const
{
	return papuga_ErrorCode_tostring((papuga_ErrorCode)errcode);
}

int WebRequestHandler::httpstatus( int errcode) const
{
	return papugaErrorToHttpStatus( errcode);
}

int WebRequestHandler::papugaErrorToHttpStatus( int errcode)
{
	switch ((papuga_ErrorCode)errcode)
	{
		case papuga_Ok:				return 200 /*Ok*/;
		case papuga_LogicError:			return 400 /*Bad Request*/;
		case papuga_NoMemError:			return 500 /*Internal Server Error*/;
		case papuga_TypeError:			return 400 /*Bad Request*/;
		case papuga_EncodingError:		return 400 /*Bad Request*/;
		case papuga_BufferOverflowError:	return 400 /*Bad Request*/;
		case papuga_OutOfRangeError:		return 400 /*Bad Request*/;
		case papuga_NofArgsError:		return 500 /*Internal Server Error*/;
		case papuga_MissingSelf:		return 500 /*Internal Server Error*/;
		case papuga_InvalidAccess:		return 500 /*Internal Server Error*/;
		case papuga_UnexpectedEof:		return 500 /*Internal Server Error*/;
		case papuga_NotImplemented:		return 501 /*Not Implemented*/;
		case papuga_ValueUndefined:		return 400 /*Bad Request*/;
		case papuga_MixedConstruction:		return 500 /*Internal Server Error*/;
		case papuga_DuplicateDefinition:	return 500 /*Internal Server Error*/;
		case papuga_SyntaxError:		return 500 /*Bad Request*/;
		case papuga_UncaughtException:		return 500 /*Internal Server Error*/;
		case papuga_ExecutionOrder:		return 500 /*Internal Server Error*/;
		case papuga_AtomicValueExpected:	return 500 /*Internal Server Error*/;
		case papuga_NotAllowed:			return 401 /*Unauthorized*/;
		case papuga_IteratorFailed:		return 500 /*Internal Server Error*/;
		case papuga_AddressedItemNotFound:	return 500 /*Internal Server Error*/;
		case papuga_HostObjectError:		return 400 /*Bad Request*/;
		case papuga_AmbiguousReference:		return 500 /*Internal Server Error*/;
	}
}

