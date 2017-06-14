/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Conversion methods for the variant type
/// \file valueVariant.cpp
#include "papuga/valueVariant.h"
#include "papuga/valueVariant.hpp"
#include "papuga/allocator.h"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/textscanner.hpp"
#include <string.h>
#include <limits>
#include <inttypes.h>

#define PRINT_FORMAT_UINT "%" PRIu64
#define PRINT_FORMAT_INT "%" PRId64
#define PRINT_FORMAT_DOUBLE "%.15g"
#define SCAN_FORMAT_UINT "%" PRIu64
#define SCAN_FORMAT_INT "%" PRId64
#define SCAN_FORMAT_DOUBLE "%lf"

#ifdef _MSC_VER
#define IS_BIG_ENDIAN  0
#elif __GNUC__
#define IS_BIG_ENDIAN  (__ORDER_BIG_ENDIAN__)
#else
#error Cannot determine endianness of platform
#endif

#if IS_BIG_ENDIAN
#define W16CHARSET textwolf::charset::UTF16BE
#define W32CHARSET textwolf::charset::UCS4BE
#else
#define W16CHARSET textwolf::charset::UTF16LE
#define W32CHARSET textwolf::charset::UCS4LE
#endif

template <class DESTCHARSET>
static void* uft8string_to_langstring( char* destbuf, int destbufsize, size_t* destlen, const char* str, size_t strsize, papuga_ErrorCode* err)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,textwolf::charset::UTF8> ScannerUTF8;
	ScannerUTF8 itr( textwolf::CStringIterator( str, strsize));

	DESTCHARSET wcout;
	textwolf::StaticBuffer out( destbuf, destbufsize);

	textwolf::UChar chr;
	for (; 0!=(chr=*itr); ++itr)
	{
		if (chr == 0xFFFF) continue;

		wcout.print( chr, out);
	}
	*destlen = out.size();
	if (out.overflow())
	{
		*err = papuga_BufferOverflowError;
		return 0;
	}
	return destbuf;
}

static void* uft8string_to_any_langstring( papuga_StringEncoding enc, const char* str, std::size_t strsize, void* buf, size_t bufsize, size_t* len, papuga_ErrorCode* err)
{
	void* rt;
	switch (enc)
	{
		case papuga_UTF8:
			rt = uft8string_to_langstring<textwolf::charset::UTF8>( (char*)buf, bufsize, len, str, strsize, err);
			break;
		case papuga_UTF16BE:
			rt = uft8string_to_langstring<textwolf::charset::UTF16BE>( (char*)buf, bufsize*2, len, str, strsize, err);
			*len /= 2;
			break;
		case papuga_UTF16LE:
			rt = uft8string_to_langstring<textwolf::charset::UTF16LE>( (char*)buf, bufsize*2, len, str, strsize, err);
			*len /= 2;
			break;
		case papuga_UTF16:
			rt = uft8string_to_langstring<W16CHARSET>( (char*)buf, bufsize*2, len, str, strsize, err);
			*len /= 2;
			break;
		case papuga_UTF32BE:
			rt = uft8string_to_langstring<textwolf::charset::UCS4BE>( (char*)buf, bufsize*4, len, str, strsize, err);
			*len /= 4;
			break;
		case papuga_UTF32LE:
			rt = uft8string_to_langstring<textwolf::charset::UCS4LE>( (char*)buf, bufsize*4, len, str, strsize, err);
			*len /= 4;
			break;
		case papuga_UTF32:
			rt = uft8string_to_langstring<W32CHARSET>( (char*)buf, bufsize*4, len, str, strsize, err);
			*len /= 4;
			break;
		default:
			*err = papuga_TypeError;
			rt = 0;
	}
	return rt;
}

template <class LANGCHARSET>
static char* langstring_to_uft8string( papuga_Allocator* allocator, const char* str, size_t strsize, papuga_ErrorCode* err)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( str, strsize));

	std::string out;

	textwolf::charset::UTF8 u8out;

	textwolf::UChar chr;
	for (; 0!=(chr=*itr); ++itr)
	{
		u8out.print( chr, out);
	}
	return papuga_Allocator_copy_string( allocator, out.c_str(), out.size());
}

static char* any_langstring_to_uft8string( papuga_Allocator* allocator, papuga_StringEncoding enc, const void* str, std::size_t strsize, papuga_ErrorCode* err)
{
	switch (enc)
	{
		case papuga_UTF8:
			return langstring_to_uft8string<textwolf::charset::UTF8>( allocator, (const char*)str, strsize, err);
		case papuga_UTF16BE:
			return langstring_to_uft8string<textwolf::charset::UTF16BE>( allocator, (const char*)str, strsize * 2, err);
		case papuga_UTF16LE:
			return langstring_to_uft8string<textwolf::charset::UTF16LE>( allocator, (const char*)str, strsize * 2, err);
		case papuga_UTF16:
			return langstring_to_uft8string<W16CHARSET>( allocator, (const char*)str, strsize * 2, err);
		case papuga_UTF32BE:
			return langstring_to_uft8string<textwolf::charset::UCS4BE>( allocator, (const char*)str, strsize * 4, err);
		case papuga_UTF32LE:
			return langstring_to_uft8string<textwolf::charset::UCS4LE>( allocator, (const char*)str, strsize * 4, err);
		case papuga_UTF32:
			return langstring_to_uft8string<W32CHARSET>( allocator, (const char*)str, strsize * 4, err);
		default:
			*err = papuga_TypeError;
			return 0;
	}
}

template <class LANGCHARSET>
static std::string langstring_to_uft8string_stl( const char* str, size_t strsize)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( str, strsize));

	std::string rt;
	textwolf::charset::UTF8 u8out;

	textwolf::UChar chr;
	for (; 0!=(chr=*itr); ++itr)
	{
		u8out.print( chr, rt);
	}
	return rt;
}

static std::string any_langstring_to_uft8string_stl( papuga_StringEncoding enc, const void* str, std::size_t strsize)
{
	switch (enc)
	{
		case papuga_UTF8:
			return langstring_to_uft8string_stl<textwolf::charset::UTF8>( (const char*)str, strsize);
		case papuga_UTF16BE:
			return langstring_to_uft8string_stl<textwolf::charset::UTF16BE>( (const char*)str, strsize * 2);
		case papuga_UTF16LE:
			return langstring_to_uft8string_stl<textwolf::charset::UTF16LE>( (const char*)str, strsize * 2);
		case papuga_UTF16:
			return langstring_to_uft8string_stl<W16CHARSET>( (const char*)str, strsize * 2);
		case papuga_UTF32BE:
			return langstring_to_uft8string_stl<textwolf::charset::UCS4BE>( (const char*)str, strsize * 4);
		case papuga_UTF32LE:
			return langstring_to_uft8string_stl<textwolf::charset::UCS4LE>( (const char*)str, strsize * 4);
		case papuga_UTF32:
			return langstring_to_uft8string_stl<W32CHARSET>( (const char*)str, strsize * 4);
	}
	return std::string();
	
}

template <class LANGCHARSET>
static bool compare_langstring_ascii( const char* str, size_t strsize, const char* cmpstr)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( str, strsize));

	unsigned char chr;
	for (; 0!=(chr=itr.ascii()); ++itr)
	{
		if (chr != (unsigned char)*cmpstr++) break;
	}
	return (chr == 0 && *cmpstr == 0);
}

static bool compare_any_langstring_ascii( papuga_StringEncoding enc, const void* str, size_t strsize, const char* cmpstr)
{
	std::size_t cmplen;
	switch (enc)
	{
		case papuga_UTF8:
			cmplen = std::strlen( cmpstr);
			return cmplen == strsize && 0 == strncmp( cmpstr, (const char*)str, strsize);
		case papuga_UTF16BE:
			return compare_langstring_ascii<textwolf::charset::UTF16BE>( (const char*)str, strsize*2, cmpstr);
		case papuga_UTF16LE:
			return compare_langstring_ascii<textwolf::charset::UTF16LE>( (const char*)str, strsize*2, cmpstr);
		case papuga_UTF16:
			return compare_langstring_ascii<W16CHARSET>( (const char*)str, strsize*2, cmpstr);
		case papuga_UTF32BE:
			return compare_langstring_ascii<textwolf::charset::UCS4BE>( (const char*)str, strsize*4, cmpstr);
		case papuga_UTF32LE:
			return compare_langstring_ascii<textwolf::charset::UCS4LE>( (const char*)str, strsize*4, cmpstr);
		case papuga_UTF32:
			return compare_langstring_ascii<W32CHARSET>( (const char*)str, strsize*4, cmpstr);
		default:
			return false;
	}
}

template <class LANGCHARSET>
static bool starts_langstring_ascii( const char* str, size_t strsize, const char* cmpstr)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( str, strsize));

	unsigned char chr;
	for (; 0!=(chr=itr.ascii() && *cmpstr); ++itr)
	{
		if (chr != (unsigned char)*cmpstr++) break;
	}
	return (*cmpstr == 0);
}

static bool starts_any_langstring_ascii( papuga_StringEncoding enc, const void* str, size_t strsize, const char* cmpstr)
{
	std::size_t cmplen;
	switch (enc)
	{
		case papuga_UTF8:
			cmplen = std::strlen( cmpstr);
			return 0 == strncmp( (const char*)str, cmpstr, cmplen);
		case papuga_UTF16BE:
			return starts_langstring_ascii<textwolf::charset::UTF16BE>( (const char*)str, strsize*2, cmpstr);
		case papuga_UTF16LE:
			return starts_langstring_ascii<textwolf::charset::UTF16LE>( (const char*)str, strsize*2, cmpstr);
		case papuga_UTF16:
			return starts_langstring_ascii<W16CHARSET>( (const char*)str, strsize*2, cmpstr);
		case papuga_UTF32BE:
			return starts_langstring_ascii<textwolf::charset::UCS4BE>( (const char*)str, strsize*4, cmpstr);
		case papuga_UTF32LE:
			return starts_langstring_ascii<textwolf::charset::UCS4LE>( (const char*)str, strsize*4, cmpstr);
		case papuga_UTF32:
			return starts_langstring_ascii<W32CHARSET>( (const char*)str, strsize*4, cmpstr);
		default:
			return false;
	}
}

template <class LANGCHARSET>
static char* langstring_toascii( char* destbuf, size_t destbufsize, const char* str, size_t strsize)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( str, strsize));
	size_t destpos = 0;
	unsigned char chr;
	for (; 0!=(chr=itr.ascii()) && destpos < destbufsize; ++itr)
	{
		destbuf[ destpos++] = chr;
	}
	if (destpos >= destbufsize) return 0;
	destbuf[ destpos] = 0;
	return (chr == 0) ? destbuf : 0;
}

static char* any_langstring_toascii( papuga_StringEncoding enc, char* destbuf, size_t destbufsize, void const* str, size_t strsize)
{
	switch (enc)
	{
		case papuga_UTF8:
			return langstring_toascii<textwolf::charset::UTF8>( destbuf, destbufsize, (const char*)str, strsize);
		case papuga_UTF16BE:
			return langstring_toascii<textwolf::charset::UTF16BE>( destbuf, destbufsize, (const char*)str, strsize*2);
		case papuga_UTF16LE:
			return langstring_toascii<textwolf::charset::UTF16LE>( destbuf, destbufsize, (const char*)str, strsize*2);
		case papuga_UTF16:
			return langstring_toascii<W16CHARSET>( destbuf, destbufsize, (const char*)str, strsize*2);
		case papuga_UTF32BE:
			return langstring_toascii<textwolf::charset::UCS4BE>( destbuf, destbufsize, (const char*)str, strsize*4);
		case papuga_UTF32LE:
			return langstring_toascii<textwolf::charset::UCS4LE>( destbuf, destbufsize, (const char*)str, strsize*4);
		case papuga_UTF32:
			return langstring_toascii<W32CHARSET>( destbuf, destbufsize, (const char*)str, strsize*4);
		default:
			return 0;
	}
}

enum NumericType {NumericDouble,NumericInt,NumericUInt,NumericNone};
template <class LANGCHARSET>
static NumericType langstring_tonumstr( char* destbuf, size_t destbufsize, const char* str, size_t strsize)
{
	NumericType rt = NumericNone;
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( str, strsize));
	size_t destpos = 0;
	unsigned char chr = itr.ascii();
	++itr;
	if (!chr || destbufsize < 2) return NumericNone;
	if (chr == '-')
	{
		destbuf[ destpos++] = chr;
		chr = itr.ascii();
		++itr;
		if (chr < '0' || chr > '9') return NumericNone;

		rt = NumericInt;
	}
	else if (chr >= '0' && chr <= '9')
	{
		rt = NumericUInt;
	}
	else
	{
		return NumericNone;
	}
	destbuf[ destpos++] = chr;

	for (++itr; 0!=(chr=itr.ascii()) && destpos < destbufsize; ++itr)
	{
		if (chr == '.')
		{
			if (rt == NumericDouble) return NumericNone;
			rt = NumericDouble;
		}
		else if (chr < '0' || chr > '9')
		{
			break;
		}
		destbuf[ destpos++] = chr;
	}
	if (destpos < destbufsize && (chr|32) == 'e')
	{
		rt = NumericDouble;
		destbuf[ destpos++] = chr;
		++itr;
		chr = itr.ascii();
		if (destpos < destbufsize && chr == '-')
		{
			destbuf[ destpos++] = chr;
			++itr;
		}
		for (; 0!=(chr=itr.ascii()) && destpos < destbufsize; ++itr)
		{
			if (chr < '0' || chr > '9')
			{
				break;
			}
			destbuf[ destpos++] = chr;
		}
	}
	if (chr != 0 || destpos >= destbufsize) return NumericNone;
	destbuf[ destpos] = 0;
	return rt;
}

static NumericType any_langstring_tonumstr( papuga_StringEncoding enc, char* destbuf, size_t destbufsize, const void* str, size_t strsize)
{
	switch (enc)
	{
		case papuga_UTF8:
			return langstring_tonumstr<textwolf::charset::UTF8>( destbuf, destbufsize, (const char*)str, strsize);
		case papuga_UTF16BE:
			return langstring_tonumstr<textwolf::charset::UTF16BE>( destbuf, destbufsize, (const char*)str, strsize*2);
		case papuga_UTF16LE:
			return langstring_tonumstr<textwolf::charset::UTF16LE>( destbuf, destbufsize, (const char*)str, strsize*2);
		case papuga_UTF16:
			return langstring_tonumstr<W16CHARSET>( destbuf, destbufsize, (const char*)str, strsize*2);
		case papuga_UTF32BE:
			return langstring_tonumstr<textwolf::charset::UCS4BE>( destbuf, destbufsize, (const char*)str, strsize*4);
		case papuga_UTF32LE:
			return langstring_tonumstr<textwolf::charset::UCS4LE>( destbuf, destbufsize, (const char*)str, strsize*4);
		case papuga_UTF32:
			return langstring_tonumstr<W32CHARSET>( destbuf, destbufsize, (const char*)str, strsize*4);
		default:
			return NumericNone;
	}
}

static NumericType string_tonumstr( char* destbuf, size_t destbufsize, const char* str, size_t strsize)
{
	return langstring_tonumstr<textwolf::charset::UTF8>( destbuf, destbufsize, str, strsize);
}

static bool numstr_to_variant( papuga_ValueVariant* value, NumericType numtype, const char* numstr)
{
	switch (numtype)
	{
		case NumericDouble:
		{
			papuga_Float val;
			if (scanf( SCAN_FORMAT_DOUBLE, &val) < 1)
			{
				return false;
			}
			papuga_init_ValueVariant_double( value, val);
			return true;
		}
		case NumericInt:
		{
			papuga_Int val;
			if (scanf( SCAN_FORMAT_INT, &val) < 1)
			{
				return false;
			}
			papuga_init_ValueVariant_int( value, val);
			return true;
		}
		case NumericUInt:
		{
			papuga_UInt val;
			if (scanf( SCAN_FORMAT_UINT, &val) < 1)
			{
				return false;
			}
			papuga_init_ValueVariant_uint( value, val);
			return true;
		}
		case NumericNone:
			break;
	}
	return false;
}

static bool bufprint_number_variant( char* buf, std::size_t bufsize, std::size_t& len, const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	if (value->valuetype == papuga_TypeDouble)
	{
		len = snprintf( buf, bufsize, PRINT_FORMAT_DOUBLE, value->value.Double);
	}
	else if (value->valuetype == papuga_TypeInt)
	{
		len = snprintf( buf, bufsize, PRINT_FORMAT_INT, value->value.Int);
	}
	else if (value->valuetype == papuga_TypeUInt)
	{
		len = snprintf( buf, bufsize, PRINT_FORMAT_UINT, value->value.UInt);
	}
	else
	{
		*err = papuga_TypeError;
		return false;
	}
	if (len >= bufsize)
	{
		*err = papuga_BufferOverflowError;
		return false;
	}
	return true;
}


extern "C" const char* papuga_ValueVariant_toascii( char* destbuf, size_t destbufsize, const papuga_ValueVariant* val)
{
	if (!papuga_ValueVariant_isstring( val)) return false;
	if (val->valuetype == papuga_TypeString)
	{
		if (destbufsize <= (size_t)val->length) return 0;
		const char* si = val->value.string;
		char* di = destbuf;
		while (*si && *si < 128)
		{
			*di++ = *si++;
		}
		*di = 0;
		return destbuf;
	}
	else//if (val->valuetype == papuga_TypeLangString)
	{
		return any_langstring_toascii( (papuga_StringEncoding)val->encoding, destbuf, destbufsize, val->value.langstring, val->length);
	}
	return 0;
}

extern "C" const char* papuga_ValueVariant_tostring( const papuga_ValueVariant* value, papuga_Allocator* allocator, size_t* len, papuga_ErrorCode* err)
{
	if (papuga_ValueVariant_isstring( value))
	{
		if (value->valuetype == papuga_TypeString)
		{
			*len = value->length;
			return value->value.string;
		}
		else//if (value->valuetype == papuga_TypeLangString)
		{
			return any_langstring_to_uft8string( allocator, (papuga_StringEncoding)value->encoding, value->value.langstring, value->length, err);
		}
	}
	else if (papuga_ValueVariant_isnumeric( value))
	{
		const char* rt;
		char localbuf[256];
		if (!bufprint_number_variant( localbuf, sizeof(localbuf), *len, value, err)) return 0;
		rt = papuga_Allocator_copy_string( allocator, localbuf, *len);
		if (!rt)
		{
			*err = papuga_NoMemError;
			return 0;
		}
		return rt;
	}
	else if (papuga_ValueVariant_defined( value))
	{
		*err = papuga_TypeError;
		return 0;
	}
	else
	{
		*err = papuga_ValueUndefined;
		return 0;
	}
}

std::string papuga::ValueVariant_tostring( const papuga_ValueVariant& value, papuga_ErrorCode& errcode)
{
	try
	{
		if (papuga_ValueVariant_isstring( &value))
		{
			if (value.valuetype == papuga_TypeString)
			{
				return std::string( value.value.string, value.length);
			}
			else//if (value.valuetype == papuga_TypeLangString)
			{
				return any_langstring_to_uft8string_stl( (papuga_StringEncoding)value.encoding, value.value.langstring, value.length);
			}
		}
		else if (papuga_ValueVariant_isnumeric( &value))
		{
			char localbuf[256];
			std::size_t numlen;
			if (!bufprint_number_variant( localbuf, sizeof(localbuf), numlen, &value, &errcode)) return std::string();
			return std::string( localbuf, numlen);
		}
		else if (papuga_ValueVariant_defined( &value))
		{
			errcode = papuga_TypeError;
			return std::string();
		}
		else
		{
			errcode = papuga_ValueUndefined;
			return std::string();
		}
	}
	catch (const std::bad_alloc&)
	{
		errcode = papuga_NoMemError;
		return std::string();
	}
}

extern "C" const void* papuga_ValueVariant_tolangstring( const papuga_ValueVariant* value, papuga_StringEncoding enc, void* buf, size_t bufsize, size_t* len, papuga_ErrorCode* err)
{
	if (papuga_ValueVariant_isstring( value))
	{
		if (value->valuetype == papuga_TypeString || (value->valuetype == papuga_TypeLangString && (papuga_StringEncoding)value->encoding == papuga_UTF8))
		{
			return uft8string_to_any_langstring( enc, value->value.string, value->length, (char*)buf, bufsize, len, err);
		}
		else
		{
			*err = papuga_TypeError;
			return 0;
		}
	}
	else if (papuga_ValueVariant_isnumeric( value))
	{
		char localbuf[256];
		std::size_t numlen;
		if (!bufprint_number_variant( localbuf, sizeof(localbuf), numlen, value, err)) return 0;
		return uft8string_to_any_langstring( enc, localbuf, numlen, (char*)buf, bufsize, len, err);
	}
	else if (papuga_ValueVariant_defined( value))
	{
		*err = papuga_TypeError;
		return 0;
	}
	else
	{
		*err = papuga_ValueUndefined;
		return 0;
	}
}

extern "C" int64_t papuga_ValueVariant_toint( const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	char destbuf[256];

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_TypeInt)
		{
			return value->value.Int;
		}
		else if (value->valuetype == papuga_TypeUInt)
		{
			if (value->value.UInt > (papuga_UInt)std::numeric_limits<papuga_Int>::max())
			{
				*err = papuga_OutOfRangeError;
				return 0;
			}
			return value->value.UInt;
		}
		else if (value->valuetype == papuga_TypeDouble)
		{
			if (value->value.Double > std::numeric_limits<papuga_Int>::max())
			{
				*err = papuga_OutOfRangeError;
				return 0;
			}
			if (value->value.Double > 0)
			{
				return (int64_t)(value->value.Double + std::numeric_limits<float>::epsilon());
			}
			else
			{
				return (int64_t)(value->value.Double - std::numeric_limits<float>::epsilon());
			}
		}
		else if (papuga_ValueVariant_isstring( value))
		{
			NumericType numtype;
			if (value->valuetype == papuga_TypeString)
			{
				numtype = string_tonumstr( destbuf, sizeof(destbuf), value->value.string, value->length);
			}
			else
			{
				numtype = any_langstring_tonumstr( (papuga_StringEncoding)value->encoding, destbuf, sizeof(destbuf), value->value.langstring, value->length);
			}
			papuga_ValueVariant numval;
			if (!numstr_to_variant( &numval, numtype, destbuf))
			{
				*err = papuga_TypeError;
				return 0;
			}
			return papuga_ValueVariant_toint( &numval, err);
		}
		else
		{
			*err = papuga_TypeError;
			return 0;
		}
	}
	else if (papuga_ValueVariant_defined( value))
	{
		*err = papuga_TypeError;
		return 0;
	}
	else
	{
		*err = papuga_ValueUndefined;
		return 0;
	}
}

extern "C" uint64_t papuga_ValueVariant_touint( const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	char destbuf[256];

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_TypeUInt)
		{
			return value->value.UInt;
		}
		else if (value->valuetype == papuga_TypeInt)
		{
			if (value->value.Int < 0)
			{
				*err = papuga_OutOfRangeError;
				return 0;
			}
			return value->value.Int;
		}
		else if (value->valuetype == papuga_TypeDouble)
		{
			if (value->value.Double > std::numeric_limits<papuga_UInt>::max()
			||  value->value.Double < 0.0)
			{
				*err = papuga_OutOfRangeError;
				return 0;
			}
			return (uint64_t)(value->value.Double + std::numeric_limits<float>::epsilon());
		}
		else if (papuga_ValueVariant_isstring( value))
		{
			NumericType numtype;
			if (value->valuetype == papuga_TypeString)
			{
				numtype = string_tonumstr( destbuf, sizeof(destbuf), value->value.string, value->length);
			}
			else
			{
				numtype = any_langstring_tonumstr( (papuga_StringEncoding)value->encoding, destbuf, sizeof(destbuf), value->value.langstring, value->length);
			}
			papuga_ValueVariant numval;
			if (!numstr_to_variant( &numval, numtype, destbuf))
			{
				*err = papuga_TypeError;
				return 0;
			}
			return papuga_ValueVariant_touint( &numval, err);
		}
		else
		{
			*err = papuga_TypeError;
			return 0;
		}
	}
	else if (papuga_ValueVariant_defined( value))
	{
		*err = papuga_TypeError;
		return 0;
	}
	else
	{
		*err = papuga_ValueUndefined;
		return 0;
	}
}

extern "C" double papuga_ValueVariant_todouble( const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	char destbuf[256];

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_TypeDouble)
		{
			return value->value.Double;
		}
		else if (value->valuetype == papuga_TypeUInt)
		{
			return value->value.UInt;
		}
		else if (value->valuetype == papuga_TypeInt)
		{
			return value->value.Int;
		}
		else if (papuga_ValueVariant_isstring( value))
		{
			NumericType numtype;
			if (value->valuetype == papuga_TypeString)
			{
				numtype = string_tonumstr( destbuf, sizeof(destbuf), value->value.string, value->length);
			}
			else
			{
				numtype = any_langstring_tonumstr( (papuga_StringEncoding)value->encoding, destbuf, sizeof(destbuf), value->value.langstring, value->length);
			}
			papuga_ValueVariant numval;
			if (!numstr_to_variant( &numval, numtype, destbuf))
			{
				*err = papuga_TypeError;
				return 0;
			}
			return papuga_ValueVariant_todouble( &numval, err);
		}
		else
		{
			*err = papuga_TypeError;
			return 0;
		}
	}
	else if (papuga_ValueVariant_defined( value))
	{
		*err = papuga_TypeError;
		return 0;
	}
	else
	{
		*err = papuga_ValueUndefined;
		return 0;
	}
}

extern "C" bool papuga_ValueVariant_tobool( const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	int rt;
	char destbuf[256];
	const char* numstr;

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_TypeUInt)
		{
			return !!value->value.UInt;
		}
		else if (value->valuetype == papuga_TypeInt)
		{
			return !!value->value.Int;
		}
		else if (value->valuetype == papuga_TypeDouble)
		{
			return (value->value.Double < -std::numeric_limits<float>::epsilon()
				||  value->value.Double > +std::numeric_limits<float>::epsilon());
		}
		else if (papuga_ValueVariant_isstring( value))
		{
			numstr = papuga_ValueVariant_toascii( destbuf, sizeof(destbuf), value);
			if (numstr == NULL) return 0;
			if (!numstr[1])
			{
				if (numstr[0] == '1' || (numstr[0]|32) == 'y' || (numstr[0]|32) == 't') return true;
				if (numstr[0] == '0' || (numstr[0]|32) == 'n' || (numstr[0]|32) == 'f') return false;
			}
			if (scanf( "%d", &rt) < 1)
			{
				*err = papuga_TypeError;
				return 0;
			}
			return !!rt;
		}
		else
		{
			*err = papuga_TypeError;
			return 0;
		}
	}
	else if (papuga_ValueVariant_defined( value))
	{
		*err = papuga_TypeError;
		return 0;
	}
	else
	{
		*err = papuga_ValueUndefined;
		return 0;
	}
}

extern "C" papuga_ValueVariant* papuga_ValueVariant_tonumeric( const papuga_ValueVariant* value, papuga_ValueVariant* res, papuga_ErrorCode* err)
{
	char destbuf[64];

	if (papuga_ValueVariant_isatomic( value))
	{
		if (papuga_ValueVariant_isnumeric( value))
		{
			if (value->valuetype == papuga_TypeUInt)
			{
				papuga_init_ValueVariant_uint( res, value->value.UInt);
				return res;
			}
			else if (value->valuetype == papuga_TypeInt)
			{
				papuga_init_ValueVariant_int( res, value->value.Int);
				return res;
			}
			else if (value->valuetype == papuga_TypeDouble)
			{
				papuga_init_ValueVariant_double( res, value->value.Double);
				return res;
			}
			else
			{
				*err = papuga_TypeError;
				return 0;
			}
		}
		else if (papuga_ValueVariant_isstring( value))
		{
			NumericType numtype;
			if (value->valuetype == papuga_TypeString)
			{
				numtype = string_tonumstr( destbuf, sizeof(destbuf), value->value.string, value->length);
			}
			else
			{
				numtype = any_langstring_tonumstr( (papuga_StringEncoding)value->encoding, destbuf, sizeof(destbuf), value->value.langstring, value->length);
			}
			papuga_ValueVariant numval;
			if (!numstr_to_variant( &numval, numtype, destbuf))
			{
				*err = papuga_TypeError;
				return 0;
			}
			return papuga_ValueVariant_tonumeric( &numval, res, err);
		}
		else
		{
			*err = papuga_TypeError;
			return 0;
		}
	}
	else if (papuga_ValueVariant_defined( value))
	{
		*err = papuga_TypeError;
		return 0;
	}
	else
	{
		*err = papuga_ValueUndefined;
		return 0;
	}
}

extern "C" bool papuga_ValueVariant_isequal_ascii( const papuga_ValueVariant* val, const char* cmpstr)
{
	if (!papuga_ValueVariant_isstring( val)) return false;
	if (val->valuetype == papuga_TypeString)
	{
		return 0 == strncmp( val->value.string, cmpstr, val->length);
	}
	else//if (val->valuetype == papuga_TypeLangString)
	{
		return compare_any_langstring_ascii( (papuga_StringEncoding)val->encoding, val->value.langstring, val->length, cmpstr);
	}
	return false;
}

extern "C" bool papuga_ValueVariant_starts_ascii( const papuga_ValueVariant* val, const char* cmpstr)
{
	if (!papuga_ValueVariant_isstring( val)) return false;
	int cmplen = std::strlen(cmpstr);
	if (val->length < cmplen) return false;

	if (val->valuetype == papuga_TypeString)
	{
		return 0 == strncmp( val->value.string, cmpstr, cmplen);
	}
	else//if (val->valuetype == papuga_TypeLangString)
	{
		return starts_any_langstring_ascii( (papuga_StringEncoding)val->encoding, val->value.langstring, val->length, cmpstr);
	}
}


