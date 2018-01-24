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

ErrorCause strus::papugaErrorToErrorCause( papuga_ErrorCode errcode)
{
	switch (errcode)
	{
		case papuga_Ok:				return ErrorCauseUnknown;
		case papuga_LogicError:			return ErrorCauseLogicError;
		case papuga_NoMemError:			return ErrorCauseOutOfMem;
		case papuga_TypeError:			return ErrorCauseBindingLanguageError;
		case papuga_EncodingError:		return ErrorCauseEncoding;
		case papuga_BufferOverflowError:	return ErrorCauseBufferOverflow;
		case papuga_OutOfRangeError:		return ErrorCauseValueOutOfRange;
		case papuga_NofArgsError:		return ErrorCauseMaxNofItemsExceeded;
		case papuga_MissingSelf:		return ErrorCauseBindingLanguageError;
		case papuga_InvalidAccess:		return ErrorCauseBindingLanguageError;
		case papuga_UnexpectedEof:		return ErrorCauseUnexpectedEof;
		case papuga_NotImplemented:		return ErrorCauseNotImplemented;
		case papuga_ValueUndefined:		return ErrorCauseBindingLanguageError;
		case papuga_MixedConstruction:		return ErrorCauseBindingLanguageError;
		case papuga_DuplicateDefinition:	return ErrorCauseBindingLanguageError;
		case papuga_SyntaxError:		return ErrorCauseInputFormat;
		case papuga_UncaughtException:		return ErrorCauseUncaughtException;
		case papuga_ExecutionOrder:		return ErrorCauseLogicError;
		case papuga_AtomicValueExpected:	return ErrorCauseBindingLanguageError;
		case papuga_NotAllowed:			return ErrorCauseNotAllowed;
		case papuga_IteratorFailed:		return ErrorCauseHiddenError;
		case papuga_AddressedItemNotFound:	return ErrorCauseBindingLanguageError;
		case papuga_HostObjectError:		return ErrorCauseHiddenError;
		case papuga_AmbiguousReference:		return ErrorCauseBindingLanguageError;
	}
}

int strus::errorCauseToHttpStatus( ErrorCause cause)
{
	switch (cause)
	{
		case ErrorCauseUnknown: return 200 /*Ok*/;
		case ErrorCauseErrno: return 500 /*Internal Server Error*/;
		case ErrorCauseOutOfMem: return 500 /*Internal Server Error*/;
		case ErrorCauseDataCorruption: return 500 /*Internal Server Error*/;
		case ErrorCauseAvailability: return 503; /*Service not available*/;
		case ErrorCauseNotAllowed: return 405; /* Method not allowed */
		case ErrorCauseProtocolError: return 500 /*Internal Server Error*/;
		case ErrorCauseSyntax: return 500 /*Internal Server Error*/;
		case ErrorCauseNotFound: return 500 /*Internal Server Error*/;
		case ErrorCauseIOError: return 500 /*Internal Server Error*/;
		case ErrorCauseVersionMismatch: return 500 /*Internal Server Error*/;
		case ErrorCauseInvalidArgument: return 500 /*Internal Server Error*/;
		case ErrorCauseInvalidRegex: return 500 /*Internal Server Error*/;
		case ErrorCauseInvalidOperation: return 500 /*Internal Server Error*/;
		case ErrorCauseNotImplemented: return 500 /*Internal Server Error*/;
		case ErrorCauseIncompleteDefinition: return 500 /*Internal Server Error*/;
		case ErrorCauseBindingLanguageError: return 500 /*Internal Server Error*/;
		case ErrorCauseUnknownIdentifier: return 400 /*Bad Request*/;
		case ErrorCauseOperationOrder: return 500 /*Internal Server Error*/;
		case ErrorCauseValueOutOfRange: return 500 /*Internal Server Error*/;
		case ErrorCauseMaximumLimitReached: return 500 /*Internal Server Error*/;
		case ErrorCauseBufferOverflow: return 500 /*Internal Server Error*/;
		case ErrorCauseMaxNofItemsExceeded: return 500 /*Internal Server Error*/;
		case ErrorCauseInputFormat: return 400 /*Bad Request*/;
		case ErrorCauseLogicError: return 500 /*Internal Server Error*/;
		case ErrorCauseRuntimeError: return 500 /*Internal Server Error*/;
		case ErrorCauseUncaughtException: return 500 /*Internal Server Error*/;
		case ErrorCauseIncompleteRequest: return 400 /*Bad Request*/;
		case ErrorCauseUnexpectedEof: return 400 /*Bad Request*/;
		case ErrorCausePlatformIncompatibility: return 400 /*Bad Request*/;
		case ErrorCausePlatformRequirements: return 500 /*Internal Server Error*/;
		case ErrorCauseHiddenError: return 500 /*Internal Server Error*/;
		case ErrorCauseEncoding: return 400 /*Bad Request*/;
	}
	return 500 /*Internal Server Error*/;
}


