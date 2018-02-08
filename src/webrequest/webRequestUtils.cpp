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
#include "private/internationalization.hpp"
#include "papuga/encoding.h"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.h"
#include "papuga/valueVariant.hpp"
#include <stdexcept>

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
		case ErrorCauseMutexLockFailed: return 500 /*Internal Server Error*/;
		case ErrorCauseLogicError: return 500 /*Internal Server Error*/;
		case ErrorCauseUncaughtException: return 500 /*Internal Server Error*/;

		case ErrorCauseAvailability: return 503; /*Service not available*/;
		case ErrorCauseNotAllowed: return 405; /* Method not allowed */
		case ErrorCauseProtocolError: return 500 /*Internal Server Error*/;
		case ErrorCausePlatformIncompatibility: return 400 /*Bad Request*/;
		case ErrorCausePlatformRequirements: return 500 /*Internal Server Error*/;

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
		case ErrorCauseRuntimeError: return 500 /*Internal Server Error*/;
		case ErrorCauseIncompleteRequest: return 400 /*Bad Request*/;
		case ErrorCauseUnexpectedEof: return 400 /*Bad Request*/;
		case ErrorCauseHiddenError: return 500 /*Internal Server Error*/;
		case ErrorCauseInputFormat: return 400 /*Bad Request*/;
		case ErrorCauseEncoding: return 400 /*Bad Request*/;
	}
	return 500 /*Internal Server Error*/;
}

std::string strus::webRequestContent_tostring( const WebRequestContent& content)
{
	papuga_StringEncoding encoding;
	if (!papuga_getStringEncodingFromName( &encoding, content.charset()))
	{
		throw std::runtime_error( papuga_ErrorCode_tostring( papuga_EncodingError));
	}
	papuga_ValueVariant contentval;
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_init_ValueVariant_string_enc( &contentval, encoding, content.str(), content.len());
	std::string rt = papuga::ValueVariant_tostring( contentval, errcode);
	if (errcode != papuga_Ok)
	{
		throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
	}
	return rt;
}

static bool findAccept( const char* http_accept_list, const char* suggestion)
{
	char const* ai = http_accept_list;
	do
	{
		for (; (unsigned char)*ai <= 32; ++ai){}
		char const* ei = suggestion;
		for (; *ei && *ai && *ai != ',' && *ai != ';'; ++ei,++ai)
		{
			for (; *ai == ' ' || *ai == '-'; ++ai){}
			for (; *ei == ' ' || *ei == '-'; ++ei){}
			if ((*ei|32) != (*ai|32)) break;
		}
		if (*ai == ';') for (; *ai && *ai != ','; ++ai){} //... skip and ignore weighting with the quality value syntax (e.g. ";q=0.9")
		for (; *ai == ' '; ++ai){}
		if (!*ei && (*ai == ',' || *ai == '\0')) return true;
		++ai;
	} while (*ai);
	return false;
}

papuga_StringEncoding strus::getResultStringEncoding( const char* accepted_charset, papuga_StringEncoding inputenc)
{
	if (!accepted_charset || !accepted_charset[0]) return inputenc;
	if (inputenc != papuga_Binary && findAccept( accepted_charset, papuga_stringEncodingName( inputenc))) return inputenc;
	if (findAccept( accepted_charset, papuga_stringEncodingName( papuga_UTF8))) return papuga_UTF8;
	if (findAccept( accepted_charset, papuga_stringEncodingName( papuga_UTF16BE))) return papuga_UTF16BE;
	if (findAccept( accepted_charset, papuga_stringEncodingName( papuga_UTF16LE))) return papuga_UTF16LE;
	if (findAccept( accepted_charset, papuga_stringEncodingName( papuga_UTF32BE))) return papuga_UTF32BE;
	if (findAccept( accepted_charset, papuga_stringEncodingName( papuga_UTF32LE))) return papuga_UTF32LE;
	return papuga_Binary;
}

papuga_ContentType strus::getResultContentType( const char* http_accept, papuga_ContentType inputdoctype)
{
	if (!http_accept || !http_accept[0]) return inputdoctype;
	if (inputdoctype != papuga_ContentType_Unknown && findAccept( http_accept, papuga_ContentType_mime( inputdoctype))) return inputdoctype;
	if (findAccept( http_accept, papuga_ContentType_mime( papuga_ContentType_JSON))) return papuga_ContentType_JSON;
	if (findAccept( http_accept, papuga_ContentType_mime( papuga_ContentType_XML))) return papuga_ContentType_XML;
	return papuga_ContentType_Unknown;
}


