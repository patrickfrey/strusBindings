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
#include "papuga/requestResult.h"
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

#define STRUS_LOWLEVEL_DEBUG
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
		case papuga_ValueUndefined:		return ErrorCauseIncompleteDefinition;
		case papuga_MixedConstruction:		return ErrorCauseBindingLanguageError;
		case papuga_DuplicateDefinition:	return ErrorCauseBindingLanguageError;
		case papuga_SyntaxError:		return ErrorCauseInputFormat;
		case papuga_UncaughtException:		return ErrorCauseUncaughtException;
		case papuga_ExecutionOrder:		return ErrorCauseLogicError;
		case papuga_AtomicValueExpected:	return ErrorCauseBindingLanguageError;
		case papuga_NotAllowed:			return ErrorCauseNotAllowed;
		case papuga_IteratorFailed:		return ErrorCauseHiddenError;
		case papuga_AddressedItemNotFound:	return ErrorCauseRequestResolveError;
		case papuga_HostObjectError:		return ErrorCauseHiddenError;
		case papuga_AmbiguousReference:		return ErrorCauseBindingLanguageError;
		case papuga_MaxRecursionDepthReached:	return ErrorCauseMaxRecursionDepht;
	}
	return ErrorCauseUnknown;
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
		case ErrorCauseMaxLimitReached: return 500 /*Internal Server Error*/;
		case ErrorCauseBufferOverflow: return 500 /*Internal Server Error*/;
		case ErrorCauseMaxNofItemsExceeded: return 500 /*Internal Server Error*/;
		case ErrorCauseMaxRecursionDepht: return 500 /*Internal Server Error*/;
		case ErrorCauseRuntimeError: return 500 /*Internal Server Error*/;
		case ErrorCauseIncompleteRequest: return 400 /*Bad Request*/;
		case ErrorCauseIncompleteResult: return 500 /*Internal Server Error*/;
		case ErrorCauseUnexpectedEof: return 400 /*Bad Request*/;
		case ErrorCauseHiddenError: return 500 /*Internal Server Error*/;
		case ErrorCauseInputFormat: return 400 /*Bad Request*/;
		case ErrorCauseEncoding: return 400 /*Bad Request*/;
		case ErrorCauseRequestResolveError: return 404 /*Not found*/;
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

struct AcceptElem
{
	float weight;
	const char* type;

	bool operator < (const AcceptElem& o) const
	{
		if (weight > o.weight) return true;
		if (weight < o.weight) return false;
		return (type < o.type);			//... make sort stable
	}
};

static AcceptElem* nextElem( AcceptElem* elembuf, std::size_t elembufsize, std::size_t& elembufpos, const char* start)
{
	if (elembufpos+1 >= elembufsize) throw std::bad_alloc();
	AcceptElem* elem = elembuf + elembufpos++;

	elem->type = start;
	elem->weight = 1.0;
	return elem;
}

static void push_char( char* strbuf, std::size_t strbufsize, std::size_t& strbufpos, char ch)
{
	if (strbufpos+1 >= strbufsize) throw std::bad_alloc();
	strbuf[ strbufpos++] = ch;
}

static void push_prefix( char* strbuf, std::size_t strbufsize, std::size_t& strbufpos, const char* prefix)
{
	char const* di = prefix;
	for (; *di && *di != '/'; ++di)
	{
		push_char( strbuf, strbufsize, strbufpos, *di);
	}
	if (*di != '/') throw std::runtime_error( "syntax");
	push_char( strbuf, strbufsize, strbufpos, *di);
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

static bool parseToken( char const*& src, char* strbuf, std::size_t strbufsize)
{
	std::size_t strbufpos = 0;
	char const* ai = skipSpaces( src);
	for (;((*ai|32) >= 'a' && (*ai|32) <= 'z') || (*ai >= '0' && *ai <= '9') || (*ai == '.' || *ai == '_' || *ai == '-'); ++ai)
	{
		push_char( strbuf, strbufsize, strbufpos, *ai);
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

static bool parseAssigment( char const*& src, char* keybuf, std::size_t keybufsize, char* valbuf, std::size_t valbufsize)
{
	char const* ai = src;
	if (parseIdent( ai, keybuf, keybufsize) && *ai == '=')
	{
		++ai;
		if (parseToken( ai, valbuf, valbufsize))
		{
			src = ai;
			return true;
		}
	}
	return false;
}

static const AcceptElem* parseAccept( const char* acceptsrc, char* strbuf, std::size_t strbufsize, AcceptElem* elembuf, std::size_t elembufsize, bool upcase)
{
	std::size_t strbufpos = 0;
	std::size_t elembufpos = 0;
	char const* ai = acceptsrc;
	const char* prefix = 0;
	try
	{
		while (*ai)
		{
			std::size_t eidx = elembufpos;
			for (;;)
			{
				ai = skipSpaces( ai);
				AcceptElem* elem = nextElem( elembuf, elembufsize, elembufpos, strbuf + strbufpos);
				if (prefix)
				{
					push_prefix( strbuf, strbufsize, strbufpos, prefix);
				}
				for (; *ai && *ai != ',' && *ai != ';' && *ai != '+'; ++ai)
				{
					push_char( strbuf, strbufsize, strbufpos, upcase ? toupper(*ai) : tolower(*ai));
				}
				push_char( strbuf, strbufsize, strbufpos, 0);

				if (*ai == '+')
				{
					++ai;
					prefix = elem->type;
					continue;
				}
				else
				{
					prefix = 0;
				}
				if (*ai == ',')
				{
					++ai;
				}
				else
				{
					break;
				}
			}
			if (*ai == ';')
			{
				++ai;
				char keybuf[ 32];
				char valbuf[ 64];
				while (parseAssigment( ai, keybuf, sizeof(keybuf), valbuf, sizeof(valbuf)))
				{
					if (0==std::strcmp( keybuf, "q"))
					{
						NumParseError numerr = strus::NumParseOk;
						double weight = strus::doubleFromString( valbuf, std::strlen(valbuf), numerr);
						if (numerr != strus::NumParseOk) throw std::runtime_error("conversion");
						for (; eidx < elembufpos; ++eidx)
						{
							elembuf[ eidx].weight = weight;
						}
					}
					if (*ai == ';') ++ai;
				}
			}
			if (*ai == ',') ++ai;
		}
	}
	catch (...)
	{
		return NULL;
	}
	if (elembufpos >= elembufsize) return NULL;
	std::sort( elembuf, elembuf+elembufpos);
	elembuf[ elembufpos].weight = 0.0;
	elembuf[ elembufpos].type = NULL;
	return elembuf;
}

static bool findAccept( const AcceptElem* http_accept_list, const char* suggestion)
{
	AcceptElem const* ai = http_accept_list;
	for (; ai->type; ++ai)
	{
		if (0==std::strcmp( suggestion, ai->type)) return true;
	}
	return false;
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

papuga_StringEncoding strus::getResultStringEncoding( const char* accepted_charset, papuga_StringEncoding inputenc)
{
	char strbuf[ 8092];
	AcceptElem elembuf[ 512];

	// Parse http accept-charset list:
	if (!accepted_charset || !accepted_charset[0]) return inputenc;
	const AcceptElem* accepted_charset_list = parseAccept( accepted_charset, strbuf, sizeof(strbuf), elembuf, sizeof(elembuf)/sizeof(*elembuf), true);
	if (!accepted_charset_list) return papuga_Binary;

	// Prioritise the charset specified as argument:
	if (inputenc == papuga_UTF16BE && findAccept( accepted_charset_list, papuga_stringEncodingName( papuga_UTF16))) return papuga_UTF16BE;
	if (inputenc == papuga_UTF32BE && findAccept( accepted_charset_list, papuga_stringEncodingName( papuga_UTF32))) return papuga_UTF32BE;
	if (inputenc != papuga_Binary && findAccept( accepted_charset_list, papuga_stringEncodingName( inputenc)))
	{
		if (inputenc == papuga_UTF16) return papuga_UTF16BE;
		if (inputenc == papuga_UTF32) return papuga_UTF32BE;
		return inputenc;
	}
	// Find the charset supported with the highest priority in the accepted list:
	AcceptElem const* ai = accepted_charset_list;
	for (; ai->type; ++ai)
	{
		papuga_StringEncoding encoding;
		if (papuga_getStringEncodingFromName( &encoding, ai->type))
		{
			if (encoding == papuga_UTF16) return papuga_UTF16BE;
			if (encoding == papuga_UTF32) return papuga_UTF32BE;
			return encoding;
		}
	}
	return papuga_Binary;
}

WebRequestContent::Type strus::getResultContentType( const char* http_accept, WebRequestContent::Type inputdoctype)
{
	char strbuf[ 8092];
	AcceptElem elembuf[ 512];

	// Parse http accept list:
	if (!http_accept || !http_accept[0]) return inputdoctype;
	const AcceptElem* accepted_doctype_list = parseAccept( http_accept, strbuf, sizeof(strbuf), elembuf, sizeof(elembuf)/sizeof(*elembuf), false);
	if (!accepted_doctype_list) return WebRequestContent::Unknown;

	// Prioritise the content type specified as argument:
	if (inputdoctype != WebRequestContent::Unknown && findAccept( accepted_doctype_list, WebRequestContent::typeMime( inputdoctype))) return inputdoctype;
	// Find the content type supported with the highest priority in the accepted list:
	AcceptElem const* ai = accepted_doctype_list;
	for (; ai->type; ++ai)
	{
		WebRequestContent::Type doctype = strus::webRequestContentFromTypeName( ai->type);
		if (doctype != WebRequestContent::Unknown) return doctype;
	}
	return WebRequestContent::Unknown;
}

papuga_ContentType strus::getPapugaResultContentType( const char* http_accept, papuga_ContentType inputdoctype)
{
	char strbuf[ 8092];
	AcceptElem elembuf[ 512];

	// Parse http accept list:
	if (!http_accept || !http_accept[0]) return inputdoctype;
	const AcceptElem* accepted_doctype_list = parseAccept( http_accept, strbuf, sizeof(strbuf), elembuf, sizeof(elembuf)/sizeof(*elembuf), false);
	if (!accepted_doctype_list) return papuga_ContentType_Unknown;

	// Prioritise the content type specified as argument:
	if (inputdoctype != papuga_ContentType_Unknown && findAccept( accepted_doctype_list, papuga_ContentType_mime( inputdoctype))) return inputdoctype;
	// Find the content type supported with the highest priority in the accepted list:
	AcceptElem const* ai = accepted_doctype_list;
	for (; ai->type; ++ai)
	{
		papuga_ContentType doctype = papuga_contentTypeFromName( ai->type);
		if (doctype != papuga_ContentType_Unknown) return doctype;
	}
	return papuga_ContentType_Unknown;
}

WebRequestContent::Type strus::webRequestContentFromTypeName( const char* name)
{
	char namebuf[ 128];
	char const* si = name;
	if (!parseIdent( si, namebuf, sizeof(namebuf)))
	{
		return WebRequestContent::Unknown;
	}
	else if (*si == '/')
	{
		++si;
		if (0==std::strcmp( namebuf, "application"))
		{
			if (!parseIdent( si, namebuf, sizeof(namebuf)))
			{
				return WebRequestContent::Unknown;
			}
			else if (0==std::strcmp( namebuf, "xml"))
			{
				return WebRequestContent::XML;
			}
			else if (0==std::strcmp( namebuf, "json"))
			{
				return WebRequestContent::JSON;
			}
		}
		else if (0==std::strcmp( namebuf, "text"))
		{
			if (0==std::strcmp( namebuf, "html"))
			{
				return WebRequestContent::HTML;
			}
			else if (0==std::strcmp( namebuf, "plain"))
			{
				return WebRequestContent::TEXT;
			}
		}
	}
	else if (0==std::strcmp( namebuf, "xml"))
	{
		return WebRequestContent::XML;
	}
	else if (0==std::strcmp( namebuf, "json"))
	{
		return WebRequestContent::JSON;
	}
	else if (0==std::strcmp( namebuf, "html"))
	{
		return WebRequestContent::HTML;
	}
	else if (0==std::strcmp( namebuf, "text"))
	{
		return WebRequestContent::TEXT;
	}
	return WebRequestContent::Unknown;
}

papuga_ContentType strus::papugaContentType( WebRequestContent::Type doctype)
{
	static papuga_ContentType ar[] = {
		papuga_ContentType_Unknown/*Unknown*/,
		papuga_ContentType_XML/*XML*/,
		papuga_ContentType_JSON/*JSON*/,
		papuga_ContentType_Unknown/*HTML*/,
		papuga_ContentType_Unknown/*TEXT*/};
	return ar[doctype];
}

WebRequestContent::Type strus::papugaTranslatedContentType( papuga_ContentType doctype)
{
	static WebRequestContent::Type ar[] = {
		WebRequestContent::Unknown,
		WebRequestContent::XML,
		WebRequestContent::JSON
	};
	return ar[doctype];
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
typedef const char** CStringArray;
template <>
bool serialize<CStringArray>( papuga_Serialization* ser, bool start, const CStringArray& result)
{
	if (!start && !papuga_Serialization_pushOpen( ser)) return false;
	char const** ri = result;
	for (; *ri; ++ri)
	{
		const char* str = papuga_Allocator_copy_charp( ser->allocator, *ri);
		if (!str || !papuga_Serialization_pushValue_charp( ser, str)) return false;
	}
	if (!start && !papuga_Serialization_pushClose( ser)) return false;
	return true;
}

static void setAnswer( WebRequestAnswer& answer, ErrorOperation operation, ErrorCause cause, const char* errstr)
{
	int httpstatus = errorCauseToHttpStatus( cause);
	answer.setError( httpstatus, *ErrorCode( StrusComponentWebService, operation, cause), errstr);
}

template <class ResultType>
static bool mapResult(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		WebRequestContent::Type doctype,
		const ResultType& input)
{
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_ValueVariant value;
	papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( allocator);
	if (!ser)
	{
		errcode = papuga_NoMemError;
		setAnswer( answer, ErrorOperationBuildData, papugaErrorToErrorCause( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	serialize( ser, true, input);
	papuga_init_ValueVariant_serialization( &value, ser);
	return strus::mapValueVariantToAnswer( answer, allocator, html_head, rootname, elemname, encoding, doctype, value);
}
}//namespace

bool strus::mapStringToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* name,
		papuga_StringEncoding encoding,
		WebRequestContent::Type doctype,
		const std::string& input)
{
	return mapResult( answer, allocator, html_head, 0, name, encoding, doctype, input);
}

bool strus::mapStringArrayToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		WebRequestContent::Type doctype,
		const std::vector<std::string>& input)
{
	return mapResult( answer, allocator, html_head, rootname, elemname, encoding, doctype, input);
}

bool strus::mapStringMapToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* name,
		papuga_StringEncoding encoding,
		WebRequestContent::Type doctype,
		const std::map<std::string,std::string>& input)
{
	return mapResult( answer, allocator, html_head, 0, name, encoding, doctype, input);
}

bool strus::mapStringArrayToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		WebRequestContent::Type doctype,
		const char** input)
{
	return mapResult( answer, allocator, html_head, rootname, elemname, encoding, doctype, input);
}

bool strus::mapValueVariantToAnswer(
		WebRequestAnswer& answer,
		papuga_Allocator* allocator,
		const char* html_head,
		const char* rootname,
		const char* elemname,
		papuga_StringEncoding encoding,
		WebRequestContent::Type doctype,
		const papuga_ValueVariant& value)
{
	papuga_ErrorCode errcode = papuga_Ok;
#ifdef STRUS_LOWLEVEL_DEBUG
	std::cerr << "CALL strus::mapValueVariantToAnswer" << std::endl;
	if (papuga_ValueVariant_isatomic( &value))
	{
		std::cerr << "  " << papuga::ValueVariant_tostring( value, errcode) << std::endl;
	}
	else if (value.valuetype == papuga_TypeSerialization)
	{
		std::cerr << papuga::Serialization_tostring( *value.value.serialization, "- ", errcode) << std::endl << "--" << std::endl;
	}
	else
	{
		std::cerr << "<" << papuga_Type_name( value.valuetype) << ">" << std::endl;
	}
#endif
	char* resultstr = 0;
	std::size_t resultlen = 0;
	papuga_RequestResult result;
	if (!papuga_init_RequestResult_single( &result, allocator, rootname, elemname, strus::getBindingsInterfaceDescription()->structs, &value))
	{
		errcode = papuga_NoMemError;
		setAnswer( answer, ErrorOperationBuildResult, papugaErrorToErrorCause( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	// Map the result:
	switch (doctype)
	{
		case WebRequestContent::XML:  resultstr = (char*)papuga_RequestResult_toxml( &result, encoding, &resultlen, &errcode); break;
		case WebRequestContent::JSON: resultstr = (char*)papuga_RequestResult_tojson( &result, encoding, &resultlen, &errcode); break;
		case WebRequestContent::HTML: resultstr = (char*)papuga_RequestResult_tohtml5( &result, encoding, html_head, &resultlen, &errcode); break;
		case WebRequestContent::TEXT: resultstr = (char*)papuga_RequestResult_totext( &result, encoding, &resultlen, &errcode); break;
		case WebRequestContent::Unknown:
		{
			setAnswer( answer, ErrorOperationBuildResult, ErrorCauseNotImplemented, _TXT("output content type unknown"));
			return false;
		}
		default: break;
	}
	if (resultstr)
	{
		WebRequestContent content( papuga_stringEncodingName( encoding), WebRequestContent::typeMime(doctype), resultstr, resultlen);
		answer.setContent( content);
		return true;
	}
	else
	{
		setAnswer( answer, ErrorOperationBuildData, papugaErrorToErrorCause( errcode), papuga_ErrorCode_tostring( errcode));
		return false;
	}
	return true;
}

