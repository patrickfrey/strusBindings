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
#include "papuga/stringBuffer.h"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/textscanner.hpp"
#include <string.h>
#include <limits>
#include <inttypes.h>

#define FORMAT_UINT "%" PRIu64
#define FORMAT_INT "%" PRId64
#define FORMAT_DOUBLE "%.15g"

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
static papuga_ErrorCode convert_uft8string_to_langstring( char* destbuf, int destbufsize, size_t* destlen, const char* str, size_t strsize)
{
	papuga_ErrorCode err = papuga_Ok;
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
		err = papuga_BufferOverflowError;
	}
	return err;
}


template <class LANGCHARSET>
static papuga_ErrorCode convert_langstring_to_uft8string( papuga_StringBuffer* buf, char const* str, size_t strsize)
{
	papuga_ErrorCode err = papuga_Ok;
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( reinterpret_cast<const char*>(str), strsize));

	enum {LocalBufSize=2032};
	char localbuf[ LocalBufSize+16];
	textwolf::StaticBuffer out( localbuf, sizeof(localbuf));

	textwolf::charset::UTF8 u8out;

	textwolf::UChar chr;
	for (; 0!=(chr=*itr); ++itr)
	{
		u8out.print( chr, out);
		if (out.size() >= LocalBufSize)
		{
			if (!papuga_StringBuffer_append_string( buf, out.ptr(), out.size()))
			{
				err = papuga_NoMemError;
				break;
			}
			out.clear();
		}
	}
	return err;
}

template <class LANGCHARSET>
static bool compare_langstring_ascii( char const* str, size_t strsize, char const* cmpstr)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( reinterpret_cast<const char*>(str), strsize));

	unsigned char chr;
	for (; 0!=(chr=itr.ascii()); ++itr)
	{
		if (chr != (unsigned char)*cmpstr++) break;
	}
	return (chr == 0 && *cmpstr == 0);
}

template <class LANGCHARSET>
static char* langstring_toascii( char* destbuf, size_t destbufsize, char const* str, size_t strsize)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,LANGCHARSET> ScannerLangString;
	ScannerLangString itr( textwolf::CStringIterator( reinterpret_cast<const char*>(str), strsize));
	size_t destpos = 0;
	unsigned char chr;
	for (; 0!=(chr=itr.ascii()) && destpos < destbufsize; ++itr)
	{
		destbuf[ destpos] = chr;
	}
	if (destpos >= destbufsize) return false;
	destbuf[ destpos] = 0;
	return (chr == 0) ? destbuf : 0;
}

static char* ValueVariant_toascii( char* destbuf, size_t destbufsize, const papuga_ValueVariant* val)
{
	if (!papuga_ValueVariant_isstring( val)) return false;
	if (val->valuetype == papuga_String)
	{
		if (destbufsize <= (size_t)val->length) return 0;
		memcpy( destbuf, val->value.string, val->length);
		destbuf[ val->length] = 0;
		return destbuf;
	}
	else//if (val->valuetype == papuga_LangString)
	{
		switch ((papuga_StringEncoding)val->encoding)
		{
			case papuga_UTF8:
				if (destbufsize <= (size_t)val->length) return 0;
				memcpy( destbuf, val->value.string, val->length);
				destbuf[ val->length] = 0;
				return destbuf;
			case papuga_UTF16BE:
				return langstring_toascii<textwolf::charset::UTF16BE>( destbuf, destbufsize, val->value.string, val->length*2);
			case papuga_UTF16LE:
				return langstring_toascii<textwolf::charset::UTF16LE>( destbuf, destbufsize, val->value.string, val->length*2);
			case papuga_UTF16:
				return langstring_toascii<W16CHARSET>( destbuf, destbufsize, val->value.string, val->length*2);
			case papuga_UTF32BE:
				return langstring_toascii<textwolf::charset::UCS4BE>( destbuf, destbufsize, val->value.string, val->length*4);
			case papuga_UTF32LE:
				return langstring_toascii<textwolf::charset::UCS4LE>( destbuf, destbufsize, val->value.string, val->length*4);
			case papuga_UTF32:
				return langstring_toascii<W32CHARSET>( destbuf, destbufsize, val->value.string, val->length*4);
			default:
				return 0;
		}
	}
	return 0;
}

extern "C" const char* papuga_ValueVariant_tostring( const papuga_ValueVariant* value, papuga_StringBuffer* buf, size_t* len, papuga_ErrorCode* err)
{
	papuga_ErrorCode ec = papuga_Ok;
	if (papuga_ValueVariant_isatomic( value))
	{
		if (papuga_ValueVariant_isstring( value))
		{
			if (value->valuetype == papuga_String)
			{
				*len = value->length;
				return value->value.string;
			}
			else//if (value->valuetype == papuga_LangString)
			{
				switch ((papuga_StringEncoding)value->encoding)
				{
					case papuga_UTF8:
						ec = convert_langstring_to_uft8string<textwolf::charset::UTF8>( buf, value->value.string, value->length);
						break;
					case papuga_UTF16BE:
						ec = convert_langstring_to_uft8string<textwolf::charset::UTF16BE>( buf, value->value.string, value->length * 2);
						break;
					case papuga_UTF16LE:
						ec = convert_langstring_to_uft8string<textwolf::charset::UTF16LE>( buf, value->value.string, value->length * 2);
						break;
					case papuga_UTF16:
						ec = convert_langstring_to_uft8string<W16CHARSET>( buf, value->value.string, value->length * 2);
						break;
					case papuga_UTF32BE:
						ec = convert_langstring_to_uft8string<textwolf::charset::UCS4BE>( buf, value->value.string, value->length * 4);
						break;
					case papuga_UTF32LE:
						ec = convert_langstring_to_uft8string<textwolf::charset::UCS4LE>( buf, value->value.string, value->length * 4);
						break;
					case papuga_UTF32:
						ec = convert_langstring_to_uft8string<W32CHARSET>( buf, value->value.string, value->length * 4);
						break;
					default:
						ec = papuga_TypeError;
				}
				if (ec != papuga_Ok)
				{
					*err = ec;
					return 0;
				}
				*len = buf->arsize;
				return buf->ar;
			}
		}
		else//if (papuga_ValueVariant_isnumeric( value))
		{
			char localbuf[256];
			if (value->valuetype == papuga_Double)
			{
				*len = snprintf( localbuf, sizeof(localbuf), FORMAT_DOUBLE, value->value.Double);
			}
			else if (value->valuetype == papuga_Int)
			{
				*len = snprintf( localbuf, sizeof(localbuf), FORMAT_INT, value->value.Int);
			}
			else if (value->valuetype == papuga_UInt)
			{
				*len = snprintf( localbuf, sizeof(localbuf), FORMAT_UINT, value->value.UInt);
			}
			else
			{
				*err = papuga_LogicError;
				return 0;
			}
			if (!papuga_StringBuffer_append_string( buf, localbuf, *len))
			{
				*err = papuga_NoMemError;
				return 0;
			}
			return buf->ar;
		}
	}
	else
	{
		*err = papuga_TypeError;
		return 0;
	}
}

extern "C" const void* papuga_ValueVariant_tolangstring( const papuga_ValueVariant* value, papuga_StringEncoding enc, void* buf, size_t bufsize, size_t* len, papuga_ErrorCode* err)
{
	papuga_ErrorCode ec = papuga_Ok;
	if (papuga_ValueVariant_isatomic( value))
	{
		if (papuga_ValueVariant_isstring( value))
		{
			if (value->valuetype == papuga_String || (value->valuetype == papuga_LangString && (papuga_StringEncoding)value->encoding == papuga_UTF8))
			{
				switch (enc)
				{
					case papuga_UTF8:
						ec = convert_uft8string_to_langstring<textwolf::charset::UTF8>( (char*)buf, bufsize, len, value->value.string, value->length);
						break;
					case papuga_UTF16BE:
						ec = convert_uft8string_to_langstring<textwolf::charset::UTF16BE>( (char*)buf, bufsize*2, len, value->value.string, value->length);
						*len /= 2;
						break;
					case papuga_UTF16LE:
						ec = convert_uft8string_to_langstring<textwolf::charset::UTF16LE>( (char*)buf, bufsize*2, len, value->value.string, value->length);
						*len /= 2;
						break;
					case papuga_UTF16:
						ec = convert_uft8string_to_langstring<W16CHARSET>( (char*)buf, bufsize*2, len, value->value.string, value->length);
						*len /= 2;
						break;
					case papuga_UTF32BE:
						ec = convert_uft8string_to_langstring<textwolf::charset::UCS4BE>( (char*)buf, bufsize*4, len, value->value.string, value->length);
						*len /= 4;
						break;
					case papuga_UTF32LE:
						ec = convert_uft8string_to_langstring<textwolf::charset::UCS4LE>( (char*)buf, bufsize*4, len, value->value.string, value->length);
						*len /= 4;
						break;
					case papuga_UTF32:
						ec = convert_uft8string_to_langstring<W32CHARSET>( (char*)buf, bufsize*4, len, value->value.string, value->length);
						*len /= 4;
						break;
					default:
						ec = papuga_TypeError;
				}
				if (ec != papuga_Ok)
				{
					*err = ec;
					return 0;
				}
				return buf;
			}
			else
			{
				*err = papuga_TypeError;
				return 0;
			}
		}
		else//if (papuga_ValueVariant_isnumeric( value))
		{
			char localbuf[256];
			int numlen;
			if (value->valuetype == papuga_Double)
			{
				numlen = snprintf( localbuf, sizeof(localbuf), FORMAT_DOUBLE, value->value.Double);
			}
			else if (value->valuetype == papuga_Int)
			{
				numlen = snprintf( localbuf, sizeof(localbuf), FORMAT_INT, value->value.Int);
			}
			else if (value->valuetype == papuga_UInt)
			{
				numlen = snprintf( localbuf, sizeof(localbuf), FORMAT_UINT, value->value.UInt);
			}
			else
			{
				*err = papuga_LogicError;
				return 0;
			}
			switch (enc)
			{
				case papuga_UTF8:
					ec = convert_uft8string_to_langstring<textwolf::charset::UTF8>( (char*)buf, bufsize, len, localbuf, numlen);
					break;
				case papuga_UTF16BE:
					ec = convert_uft8string_to_langstring<textwolf::charset::UTF16BE>( (char*)buf, bufsize*2, len, localbuf, numlen);
					*len /= 2;
					break;
				case papuga_UTF16LE:
					ec = convert_uft8string_to_langstring<textwolf::charset::UTF16LE>( (char*)buf, bufsize*2, len, localbuf, numlen);
					*len /= 2;
					break;
				case papuga_UTF16:
					ec = convert_uft8string_to_langstring<W16CHARSET>( (char*)buf, bufsize*2, len, localbuf, numlen);
					*len /= 2;
					break;
				case papuga_UTF32BE:
					ec = convert_uft8string_to_langstring<textwolf::charset::UCS4BE>( (char*)buf, bufsize*4, len, localbuf, numlen);
					*len /= 4;
					break;
				case papuga_UTF32LE:
					ec = convert_uft8string_to_langstring<textwolf::charset::UCS4LE>( (char*)buf, bufsize*4, len, localbuf, numlen);
					*len /= 4;
					break;
				case papuga_UTF32:
					ec = convert_uft8string_to_langstring<W32CHARSET>( (char*)buf, bufsize*4, len, localbuf, numlen);
					*len /= 4;
					break;
				default:
					ec = papuga_TypeError;
			}
			if (ec != papuga_Ok)
			{
				*err = ec;
				return 0;
			}
			return buf;
		}
	}
	else
	{
		*err = papuga_TypeError;
		return 0;
	}
}

extern "C" int64_t papuga_ValueVariant_toint( const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	int64_t rt;
	char destbuf[256];
	char* numstr;

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_Int)
		{
			return value->value.Int;
		}
		else if (value->valuetype == papuga_UInt)
		{
			if (value->value.UInt > (papuga_UIntType)std::numeric_limits<papuga_IntType>::max())
			{
				*err = papuga_OutOfRangeError;
				return 0;
			}
			return value->value.UInt;
		}
		else if (value->valuetype == papuga_Double)
		{
			if (value->value.Double > std::numeric_limits<papuga_IntType>::max())
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
			numstr = ValueVariant_toascii( destbuf, sizeof(destbuf), value);
			if (numstr == NULL) return 0;
			if (scanf( FORMAT_INT, &rt) < 1)
			{
				*err = papuga_TypeError;
				return 0;
			}
			return rt;
		}
		else
		{
			*err = papuga_TypeError;
			return 0;
		}
	}
	else
	{
		*err = papuga_TypeError;
		return 0;
	}
}

extern "C" uint64_t papuga_ValueVariant_touint( const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	uint64_t rt;
	char destbuf[256];
	char* numstr;

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_UInt)
		{
			return value->value.UInt;
		}
		else if (value->valuetype == papuga_Int)
		{
			if (value->value.Int < 0)
			{
				*err = papuga_OutOfRangeError;
				return 0;
			}
			return value->value.Int;
		}
		else if (value->valuetype == papuga_Double)
		{
			if (value->value.Double > std::numeric_limits<papuga_UIntType>::max()
			||  value->value.Double < 0.0)
			{
				*err = papuga_OutOfRangeError;
				return 0;
			}
			return (uint64_t)(value->value.Double + std::numeric_limits<float>::epsilon());
		}
		else if (papuga_ValueVariant_isstring( value))
		{
			numstr = ValueVariant_toascii( destbuf, sizeof(destbuf), value);
			if (numstr == NULL) return 0;
			if (scanf( FORMAT_UINT, &rt) < 1)
			{
				*err = papuga_TypeError;
				return 0;
			}
			return rt;
		}
		else
		{
			*err = papuga_TypeError;
			return 0;
		}
	}
	else
	{
		*err = papuga_TypeError;
		return 0;
	}
}

extern "C" double papuga_ValueVariant_todouble( const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	double rt;
	char destbuf[256];
	char* numstr;

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_Double)
		{
			return value->value.Double;
		}
		else if (value->valuetype == papuga_UInt)
		{
			return value->value.UInt;
		}
		else if (value->valuetype == papuga_Int)
		{
			return value->value.Int;
		}
		else if (papuga_ValueVariant_isstring( value))
		{
			numstr = ValueVariant_toascii( destbuf, sizeof(destbuf), value);
			if (numstr == NULL) return 0;
			if (scanf( "%lf", &rt) < 1)
			{
				*err = papuga_TypeError;
				return 0;
			}
		}
	}
	*err = papuga_TypeError;
	return 0;
}

extern "C" bool papuga_ValueVariant_tobool( const papuga_ValueVariant* value, papuga_ErrorCode* err)
{
	int rt;
	char destbuf[256];
	char* numstr;

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_UInt)
		{
			return !!value->value.UInt;
		}
		else if (value->valuetype == papuga_Int)
		{
			return !!value->value.Int;
		}
		else if (value->valuetype == papuga_Double)
		{
			return (value->value.Double < -std::numeric_limits<float>::epsilon()
				||  value->value.Double > +std::numeric_limits<float>::epsilon());
		}
		else if (papuga_ValueVariant_isstring( value))
		{
			numstr = ValueVariant_toascii( destbuf, sizeof(destbuf), value);
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
	}
	*err = papuga_TypeError;
	return 0;
}

enum NumericType {NumericDouble,NumericInt,NumericUInt,NumericNone};

static NumericType getNumericType( char const* ni)
{
	bool sign = false;
	if (*ni == '-')
	{
		sign = true;
		++ni;
	}
	while (*ni == ' ') ++ni;
	if (!*ni) return NumericNone;
	while (*ni >= '0' && *ni <= '9') ++ni;
	if (*ni == '.' || (*ni|32) == 'E') return NumericDouble;
	if (*ni) return NumericNone;
	return sign ? NumericInt : NumericUInt;
}

extern "C" papuga_ErrorCode papuga_ValueVariant_convert_tonumeric( papuga_ValueVariant* value)
{
	char destbuf[256];
	char* numstr;

	if (papuga_ValueVariant_isatomic( value))
	{
		if (value->valuetype == papuga_UInt)
		{
			papuga_init_ValueVariant_uint( value, value->value.UInt);
			return papuga_Ok;
		}
		else if (value->valuetype == papuga_Int)
		{
			papuga_init_ValueVariant_int( value, value->value.Int);
			return papuga_Ok;
		}
		else if (value->valuetype == papuga_Double)
		{
			papuga_init_ValueVariant_double( value, value->value.Double);
			return papuga_Ok;
		}
		numstr = ValueVariant_toascii( destbuf, sizeof(destbuf), value);

		if (numstr == NULL) return papuga_TypeError;
		switch (getNumericType( numstr))
		{
			case NumericDouble:
			{
				papuga_ErrorCode rt;
				double cv = papuga_ValueVariant_todouble( value, &rt);
				if (rt != papuga_Ok)
				{
					papuga_init_ValueVariant( value);
				}
				else
				{
					papuga_init_ValueVariant_double( value, cv);
				}
				return rt;
			}
			case NumericInt:
			{
				papuga_ErrorCode rt;
				int64_t cv = papuga_ValueVariant_toint( value, &rt);
				if (rt != papuga_Ok)
				{
					papuga_init_ValueVariant( value);
				}
				else
				{
					papuga_init_ValueVariant_int( value, cv);
				}
				return rt;
			}
			case NumericUInt:
			{
				papuga_ErrorCode rt;
				uint64_t cv = papuga_ValueVariant_touint( value, &rt);
				if (rt != papuga_Ok)
				{
					papuga_init_ValueVariant( value);
				}
				else
				{
					papuga_init_ValueVariant_uint( value, cv);
				}
				return rt;
			}
			case NumericNone:
			default:
			{
				break;
			}
		}
	}
	papuga_init_ValueVariant( value);
	return papuga_TypeError;
}

extern "C" bool papuga_ValueVariant_isequal_ascii( const papuga_ValueVariant* val, const char* cmpstr)
{
	if (!papuga_ValueVariant_isstring( val)) return false;
	if (val->valuetype == papuga_String)
	{
		return 0 == strncmp( val->value.string, cmpstr, val->length);
	}
	else//if (val->valuetype == papuga_LangString)
	{
		switch ((papuga_StringEncoding)val->encoding)
		{
			case papuga_UTF8:
				return 0 == strncmp( val->value.string, cmpstr, val->length);
			case papuga_UTF16BE:
				return compare_langstring_ascii<textwolf::charset::UTF16BE>( val->value.string, val->length*2, cmpstr);
			case papuga_UTF16LE:
				return compare_langstring_ascii<textwolf::charset::UTF16LE>( val->value.string, val->length*2, cmpstr);
			case papuga_UTF16:
				return compare_langstring_ascii<W16CHARSET>( val->value.string, val->length*2, cmpstr);
			case papuga_UTF32BE:
				return compare_langstring_ascii<textwolf::charset::UCS4BE>( val->value.string, val->length*4, cmpstr);
			case papuga_UTF32LE:
				return compare_langstring_ascii<textwolf::charset::UCS4LE>( val->value.string, val->length*4, cmpstr);
			case papuga_UTF32:
				return compare_langstring_ascii<W32CHARSET>( val->value.string, val->length*4, cmpstr);
			default:
				return false;
		}
	}
	return false;
}
