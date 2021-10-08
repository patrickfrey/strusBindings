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
#include "papuga/errors.hpp"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.h"
#include "papuga/valueVariant.hpp"
#include "papuga/requestParser.h"
#include "papuga/allocator.h"
#include "papuga/serialization.h"
#include "papuga/serialization.hpp"
#include "strus/base/numstring.hpp"
#include "strus/lib/bindings_description.hpp"
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <iostream>

using namespace strus;

static inline bool isEqual( const char* name, const char* oth)
{
	return name[0] == oth[0] && 0==std::strcmp(name,oth);
}

ErrorCode strus::papugaErrorToErrorCode( papuga_ErrorCode errcode)
{
	switch (errcode)
	{
		case papuga_Ok:				return ErrorCodeUnknown;
		case papuga_LogicError:			return ErrorCodeLogicError;
		case papuga_NoMemError:			return ErrorCodeOutOfMem;
		case papuga_TypeError:			return ErrorCodeBindingLanguageError;
		case papuga_EncodingError:		return ErrorCodeEncoding;
		case papuga_BufferOverflowError:	return ErrorCodeBufferOverflow;
		case papuga_OutOfRangeError:		return ErrorCodeValueOutOfRange;
		case papuga_NofArgsError:		return ErrorCodeMaxNofItemsExceeded;
		case papuga_MissingSelf:		return ErrorCodeBindingLanguageError;
		case papuga_InvalidAccess:		return ErrorCodeBindingLanguageError;
		case papuga_UnexpectedEof:		return ErrorCodeUnexpectedEof;
		case papuga_NotImplemented:		return ErrorCodeNotImplemented;
		case papuga_ValueUndefined:		return ErrorCodeIncompleteDefinition;
		case papuga_MixedConstruction:		return ErrorCodeBindingLanguageError;
		case papuga_DuplicateDefinition:	return ErrorCodeDuplicateDefinition;
		case papuga_SyntaxError:		return ErrorCodeInputFormat;
		case papuga_UncaughtException:		return ErrorCodeUncaughtException;
		case papuga_ExecutionOrder:		return ErrorCodeLogicError;
		case papuga_AtomicValueExpected:	return ErrorCodeBindingLanguageError;
		case papuga_StructureExpected:		return ErrorCodeBindingLanguageError;
		case papuga_NotAllowed:			return ErrorCodeNotAllowed;
		case papuga_IteratorFailed:		return ErrorCodeHiddenError;
		case papuga_AddressedItemNotFound:	return ErrorCodeRequestResolveError;
		case papuga_HostObjectError:		return ErrorCodeHiddenError;
		case papuga_AmbiguousReference:		return ErrorCodeBindingLanguageError;
		case papuga_MaxRecursionDepthReached:	return ErrorCodeMaxRecursionDepht;
		case papuga_ComplexityOfProblem:	return ErrorCodeRefusedDueToComplexity;
		case papuga_InvalidRequest:		return ErrorCodeInvalidRequest;
		case papuga_AttributeNotAtomic:		return ErrorCodeInvalidRequest;
		case papuga_UnknownContentType:		return ErrorCodeInvalidRequest;
		case papuga_UnknownSchema:		return ErrorCodeInvalidRequest;
		case papuga_MissingStructureDescription:return ErrorCodeLogicError;
		case papuga_DelegateRequestFailed:	return ErrorCodeInvalidRequest;
		case papuga_ServiceImplementationError: return ErrorCodeLogicError;
		case papuga_BindingLanguageError:	return ErrorCodeBindingLanguageError;
		case papuga_InvalidOutput:		return ErrorCodeRuntimeError;
	}
	return ErrorCodeUnknown;
}

papuga_ErrorCode strus::errorCodeToPapugaError( ErrorCode errcode)
{
	switch (errcode)
	{
		case ErrorCodeUnknown: return papuga_InvalidRequest;
		case ErrorCodeLogicError: return papuga_LogicError;
		case ErrorCodeOutOfMem: return papuga_NoMemError;
		case ErrorCodeEncoding: return papuga_EncodingError;
		case ErrorCodeBufferOverflow: return papuga_BufferOverflowError;
		case ErrorCodeValueOutOfRange: return papuga_OutOfRangeError;
		case ErrorCodeMaxNofItemsExceeded: return papuga_NofArgsError;
		case ErrorCodeBindingLanguageError: return papuga_InvalidAccess;
		case ErrorCodeUnexpectedEof: return papuga_UnexpectedEof;
		case ErrorCodeNotImplemented: return papuga_NotImplemented;
		case ErrorCodeIncompleteDefinition: return papuga_ValueUndefined;
		case ErrorCodeDuplicateDefinition: return papuga_DuplicateDefinition;
		case ErrorCodeInputFormat: return papuga_SyntaxError;
		case ErrorCodeUncaughtException: return papuga_UncaughtException;
		case ErrorCodeNotAllowed: return papuga_NotAllowed;
		case ErrorCodeRequestResolveError: return papuga_AddressedItemNotFound;
		case ErrorCodeMaxRecursionDepht: return papuga_MaxRecursionDepthReached;
		case ErrorCodeRefusedDueToComplexity: return papuga_ComplexityOfProblem;
		case ErrorCodeInvalidRequest: return papuga_InvalidRequest;
		case ErrorCodeRuntimeError: return papuga_ServiceImplementationError;
		default: return papuga_InvalidRequest;
	}
}

int strus::errorCodeToHttpStatus( ErrorCode errcode)
{
	switch (errcode)
	{
		case ErrorCodeUnknown: return 200 /*Ok*/;
		case ErrorCodeErrno: return 500 /*Internal server error*/;
		case ErrorCodeOutOfMem: return 500 /*Internal server error*/;
		case ErrorCodeDataCorruption: return 500 /*Internal server error*/;
		case ErrorCodeMutexLockFailed: return 500 /*Internal server error*/;
		case ErrorCodeLogicError: return 500 /*Internal server error*/;
		case ErrorCodeUncaughtException: return 500 /*Internal server error*/;

		case ErrorCodeAvailability: return 503; /*Service not available*/;
		case ErrorCodeNotAllowed: return 400; /*Bad request*/
		case ErrorCodeProtocolError: return 400 /*Bad request*/;
		case ErrorCodePlatformIncompatibility: return 500 /*Internal server error*/;
		case ErrorCodePlatformRequirements: return 500 /*Internal server error*/;

		case ErrorCodeSyntax: return 400 /*Bad request*/;
		case ErrorCodeNotFound: return 500 /*Internal server error*/;
		case ErrorCodeIOError: return 500 /*Internal server error*/;
		case ErrorCodeVersionMismatch: return 500 /*Internal server error*/;
		case ErrorCodeInvalidArgument: return 500 /*Internal server error*/;
		case ErrorCodeInvalidRegex: return 500 /*Internal server error*/;
		case ErrorCodeInvalidOperation: return 500 /*Internal server error*/;
		case ErrorCodeInvalidFilePath: return 500 /*Internal server error*/;
		case ErrorCodeInvalidRequest: return 400 /*Bad request*/;
		case ErrorCodeLoadModuleFailed: return 500 /*Internal server error*/;
		case ErrorCodeNotImplemented: return 500 /*Internal server error*/;
		case ErrorCodeIncompleteInitialization: return 500 /*Internal server error*/;
		case ErrorCodeIncompleteDefinition: return 500 /*Internal server error*/;
		case ErrorCodeIncompleteConfiguration: return 500 /*Internal server error*/;
		case ErrorCodeDuplicateDefinition: return 500 /*Internal server error*/;
		case ErrorCodeBindingLanguageError: return 500 /*Internal server error*/;
		case ErrorCodeUnknownIdentifier: return 400 /*Bad request*/;
		case ErrorCodeOperationOrder: return 500 /*Internal server error*/;
		case ErrorCodeValueOutOfRange: return 500 /*Internal server error*/;
		case ErrorCodeMaxLimitReached: return 500 /*Internal server error*/;
		case ErrorCodeBufferOverflow: return 500 /*Internal server error*/;
		case ErrorCodeMaxNofItemsExceeded: return 500 /*Internal server error*/;
		case ErrorCodeMaxRecursionDepht: return 500 /*Internal server error*/;
		case ErrorCodeRefusedDueToComplexity: return 400 /*Bad request*/;
		case ErrorCodeRuntimeError: return 500 /*Internal server error*/;
		case ErrorCodeIncompleteRequest: return 400 /*Bad request*/;
		case ErrorCodeIncompleteResult: return 500 /*Internal server error*/;
		case ErrorCodeUnexpectedEof: return 400 /*Bad request*/;
		case ErrorCodeHiddenError: return 500 /*Internal server error*/;
		case ErrorCodeInputFormat: return 400 /*Bad request*/;
		case ErrorCodeEncoding: return 400 /*Bad request*/;
		case ErrorCodeRequestResolveError: return 404 /*Not found*/;
		case ErrorCodeNotAcceptable: return 406 /*Not acceptable*/;
		case ErrorCodeDelegateRequestFailed: return 500 /*Internal server error*/;
		case ErrorCodeServiceShutdown: return 503 /*Service temporarily unavailable*/;
		case ErrorCodeServiceTemporarilyUnavailable: return 503 /*Service temporarily unavailable*/;
		case ErrorCodeServiceNeedExclusiveAccess: return 503 /*Service temporarily unavailable*/;
	}
	return 500 /*Internal server error*/;
}

std::string strus::webRequestContent_tostring( const WebRequestContent& content, int maxsize)
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
	if (maxsize >= 0 && maxsize < (int)rt.size())
	{
		enum {B11000000 = 192, B10000000 = 128};
		while (maxsize > 0 && (rt[ maxsize-1] & B11000000) == B10000000) --maxsize;
		rt.resize( maxsize);
	}
	return rt;
}

static void push_char( char* strbuf, std::size_t strbufsize, std::size_t& strbufpos, char ch)
{
	if (strbufpos+1 >= strbufsize) throw std::bad_alloc();
	strbuf[ strbufpos++] = ch;
}

char const* skipSpaces( char const* ai)
{
	for (; *ai && (unsigned char)*ai <= 32; ++ai){}
	return ai;
}

static bool parseIdent( char const*& src, char* strbuf, std::size_t strbufsize)
{
	std::size_t strbufpos = 0;
	char const* ai = skipSpaces( src);
	for (;(*ai|32) >= 'a' && (*ai|32) <= 'z'; ++ai)
	{
		push_char( strbuf, strbufsize, strbufpos, *ai|32);
	}
	if (strbufpos)
	{
		src = skipSpaces( ai);
		push_char( strbuf, strbufsize, strbufpos, 0);
		return true;
	}
	else
	{
		return false;
	}
}

papuga_StringEncoding strus::getStringEncoding( const char* encoding, const char* content, std::size_t contentlen)
{
	papuga_StringEncoding rt;
	if (encoding)
	{
		if (!papuga_getStringEncodingFromName( &rt, encoding))
		{
			return papuga_Binary;
		}
		return rt;
	}
	else
	{
		return papuga_guess_StringEncoding( content, contentlen);
	}
}

papuga_ContentType strus::webRequestContentFromTypeName( const char* name)
{
	char namebuf[ 128];
	char const* si = name;
	if (!parseIdent( si, namebuf, sizeof(namebuf)))
	{
		return papuga_ContentType_Unknown;
	}
	else if (*si == '/')
	{
		++si;
		if (isEqual( namebuf, "application"))
		{
			if (parseIdent( si, namebuf, sizeof(namebuf)))
			{
				if (isEqual( namebuf, "xml"))
				{
					return papuga_ContentType_XML;
				}
				else if (isEqual( namebuf, "json"))
				{
					return papuga_ContentType_JSON;
				}
			}
		}
		else if (isEqual( namebuf, "text"))
		{
			if (parseIdent( si, namebuf, sizeof(namebuf)))
			{
				if (isEqual( namebuf, "html"))
				{
					return papuga_ContentType_HTML;
				}
				else if (isEqual( namebuf, "plain"))
				{
					return papuga_ContentType_TEXT;
				}
			}
		}
	}
	else if (isEqual( namebuf, "xml"))
	{
		return papuga_ContentType_XML;
	}
	else if (isEqual( namebuf, "json"))
	{
		return papuga_ContentType_JSON;
	}
	else if (isEqual( namebuf, "html"))
	{
		return papuga_ContentType_HTML;
	}
	else if (isEqual( namebuf, "text"))
	{
		return papuga_ContentType_TEXT;
	}
	return papuga_ContentType_Unknown;
}

namespace {
template <class ResultType>
bool serialize( papuga_Serialization* ser, bool start, const ResultType& result)
{
	return false;
}
template <>
bool serialize<std::string>( papuga_Serialization* ser, bool start, const std::string& result)
{
	const char* str = papuga_Allocator_copy_string( ser->allocator, result.c_str(), result.size());
	if (!str || !papuga_Serialization_pushValue_string( ser, str, result.size())) return false;
	return true;
}
template <>
bool serialize<std::vector<std::string> >( papuga_Serialization* ser, bool start, const std::vector<std::string>& result)
{
	if (!start && !papuga_Serialization_pushOpen( ser)) return false;
	std::vector<std::string>::const_iterator ri = result.begin(), re = result.end();
	for (; ri != re; ++ri)
	{
		if (!serialize( ser, false, *ri)) return false;
	}
	if (!start && !papuga_Serialization_pushClose( ser)) return false;
	return true;
}
template <>
bool serialize<std::map<std::string,std::string> >( papuga_Serialization* ser, bool start, const std::map<std::string, std::string>& result)
{
	if (!start && !papuga_Serialization_pushOpen( ser)) return false;
	std::map<std::string,std::string>::const_iterator ri = result.begin(), re = result.end();
	for (; ri != re; ++ri)
	{
		const char* keystr = papuga_Allocator_copy_string( ser->allocator, ri->first.c_str(), ri->first.size());
		if (!keystr || !papuga_Serialization_pushName_string( ser, keystr, ri->first.size())) return false;
		if (!serialize( ser, false, ri->second)) return false;
	}
	if (!start && !papuga_Serialization_pushClose( ser)) return false;
	return true;
}
typedef char const* const* CStringArray;
template <>
bool serialize<CStringArray>( papuga_Serialization* ser, bool start, const CStringArray& result)
{
	if (!start && !papuga_Serialization_pushOpen( ser)) return false;
	size_t ri = 0;
	for (; result[ri]; ++ri)
	{
		const char* str = papuga_Allocator_copy_charp( ser->allocator, result[ri]);
		if (!str || !papuga_Serialization_pushValue_charp( ser, str)) return false;
	}
	if (!start && !papuga_Serialization_pushClose( ser)) return false;
	return true;
}

static void setAnswer( WebRequestAnswer& answer, ErrorCode errcode, const char* errstr)
{
	int httpstatus = errorCodeToHttpStatus( errcode);
	answer.setError( httpstatus, errcode, errstr);
}

template <class ResultType>
static bool mapResult(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* html_href_base,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		papuga_ContentType doctype,
		bool beautified,
		const ResultType& input)
{
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_ValueVariant value;
	papuga_Allocator serallocator;
	int serallocator_mem[ 1024];
	papuga_Serialization* ser;
	if (allocator)
	{
		ser = papuga_Allocator_alloc_Serialization( allocator);
	}
	else
	{
		papuga_init_Allocator( &serallocator, &serallocator_mem, sizeof( serallocator_mem));
		ser = papuga_Allocator_alloc_Serialization( &serallocator);
	}
	if (!ser)
	{
		errcode = papuga_NoMemError;
		setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
		if (!allocator) papuga_destroy_Allocator( &serallocator);
		return false;
	}
	if (!serialize( ser, true, input))
	{
		errcode = papuga_NoMemError;
		setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
		if (!allocator) papuga_destroy_Allocator( &serallocator);
		return false;
	}
	papuga_init_ValueVariant_serialization( &value, ser);
	bool rt = strus::mapValueVariantToAnswer( answer, allocator, html_head, html_href_base, rootname, elemname, encoding, doctype, beautified, value);
	if (!allocator) papuga_destroy_Allocator( &serallocator);
	return rt;
}
}//namespace

bool strus::mapStringToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* html_href_base,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		papuga_ContentType doctype,
		bool beautified,
		const std::string& input)
{
	papuga_ValueVariant value;
	papuga_init_ValueVariant_string( &value, input.c_str(), input.size());
	return strus::mapValueVariantToAnswer( answer, allocator, html_head, html_href_base, rootname, elemname, encoding, doctype, beautified, value);
}

bool strus::mapStringArrayToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* html_href_base,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		papuga_ContentType doctype,
		bool beautified,
		const std::vector<std::string>& input)
{
	return mapResult( answer, allocator, html_head, html_href_base, rootname, elemname, encoding, doctype, beautified, input);
}

bool strus::mapStringMapToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* html_href_base,
		const char* rootname,
		papuga_StringEncoding encoding,
		papuga_ContentType doctype,
		bool beautified,
		const std::map<std::string,std::string>& input)
{
	return mapResult( answer, allocator, html_head, html_href_base, rootname, 0/*elemname*/, encoding, doctype, beautified, input);
}

bool strus::mapStringArrayToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* html_href_base,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		papuga_ContentType doctype,
		bool beautified,
		char const* const* input)
{
	return mapResult( answer, allocator, html_head, html_href_base, rootname, elemname, encoding, doctype, beautified, input);
}

bool strus::mapValueVariantToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* html_href_base,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		papuga_ContentType doctype,
		bool beautified,
		const papuga_ValueVariant& value)
{
	papuga_ErrorCode errcode = papuga_Ok;
	char* resultstr = 0;
	std::size_t resultlen = 0;
	const papuga_StructInterfaceDescription* structdefs = strus::getBindingsInterfaceDescription()->structs;
	// Map the result:
	switch (doctype)
	{
		case papuga_ContentType_XML:  resultstr = (char*)papuga_ValueVariant_toxml( &value, allocator, structdefs, encoding, beautified, rootname, elemname, &resultlen, &errcode); break;
		case papuga_ContentType_JSON: resultstr = (char*)papuga_ValueVariant_tojson( &value, allocator, structdefs, encoding, beautified, rootname, elemname, &resultlen, &errcode); break;
		case papuga_ContentType_HTML: resultstr = (char*)papuga_ValueVariant_tohtml5( &value, allocator, structdefs, encoding, beautified, rootname, elemname, html_head, html_href_base, &resultlen, &errcode); break;
		case papuga_ContentType_TEXT: resultstr = (char*)papuga_ValueVariant_totext( &value, allocator, structdefs, encoding, beautified, rootname, elemname, &resultlen, &errcode); break;
		case papuga_ContentType_Unknown:
		{
			setAnswer( answer, ErrorCodeNotImplemented, _TXT("output content type unknown"));
			return false;
		}
		default: break;
	}
	if (resultstr)
	{
		if (!allocator) answer.defineMemBlock( resultstr);
		WebRequestContent content( papuga_stringEncodingName( encoding), papuga_ContentType_mime(doctype), resultstr, resultlen);
		answer.setContent( content);
		return true;
	}
	else
	{
		setAnswer( answer, papugaErrorToErrorCode( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	return true;
}

