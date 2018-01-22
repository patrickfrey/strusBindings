/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Helper functions and classes for executing XML/JSON requests on the strus bindings
/// \file "webRequestUtils.hpp"
#include "webRequestUtils.hpp"

using namespace strus;

int strus::papugaErrorToHttpStatusCode( papuga_ErrorCode errcode)
{
	switch (errcode)
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
	return 500 /*Internal Server Error*/;
}

