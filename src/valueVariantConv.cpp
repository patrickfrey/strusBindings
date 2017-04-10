/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Conversion methods for the variant type
/// \file valueVariantConv.cpp
#include "valueVariantConv.hpp"
#include "wcharString.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/numParser.hpp"
#include "internationalization.hpp"
#include "strus/bindings/serialization.hpp"
#include <string>
#include <cstring>
#include <inttypes.h>

using namespace strus;
using namespace bindings;

#define FORMAT_UINT "%" PRIu64
#define FORMAT_INT "%" PRId64

std::string ValueVariantConv::tostring( const ValueVariant& val)
{
	if (val.type == ValueVariant::String)
	{
		return std::string( val.value.string, val.length);
	}
	else
	{
		std::string rt;
		Slice slice( toslice( rt, val));
		if (slice.ptr == rt.c_str())
		{
			return rt;
		}
		else
		{
			return std::string( slice.ptr, slice.size);
		}
	}
}

ValueVariantConv::Slice ValueVariantConv::toslice( std::string& buf, const ValueVariant& val)
{
	if (val.type == ValueVariant::String)
	{
		return Slice( val.value.string, val.length);
	}
	else switch (val.type)
	{
		case ValueVariant::Void:
			return Slice();
		case ValueVariant::Double:
			buf = string_format( "%.12f", val.value.Double);
			return Slice( buf.c_str(), buf.size());
		case ValueVariant::UInt:
			buf = string_format( FORMAT_UINT, val.value.UInt);
			return Slice( buf.c_str(), buf.size());
		case ValueVariant::Int:
			buf = string_format( FORMAT_INT, val.value.Int);
			return Slice( buf.c_str(), buf.size());
		case ValueVariant::String:
			return Slice( val.value.string, val.length);
		case ValueVariant::WString:
			buf = convert_w16string_to_uft8string( val.value.wstring, val.length);
			return Slice( buf.c_str(), buf.size());
		case ValueVariant::StrusObject:
			throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "host object reference", "slice");
		case ValueVariant::StrusSerialization:
			if (val.value.serialization->size() == 1 && (*val.value.serialization)[0].tag == Serialization::Value)
			{
				return toslice( buf, (*val.value.serialization)[0]);
			}
			else
			{
				throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "serialization", "slice");
			}
	}
	throw strus::runtime_error(_TXT("cannot convert value variant to %s: %s"), "slice of basic string", _TXT("unknown type"));
}

std::basic_string<uint16_t> ValueVariantConv::towstring( const ValueVariant& val)
{
	if (val.type == ValueVariant::WString)
	{
		return std::basic_string<uint16_t>( val.value.wstring, val.length);
	}
	else
	{
		std::basic_string<uint16_t> rt;
		SliceW16 slice( towslice( rt, val));
		if (slice.ptr == rt.c_str())
		{
			return rt;
		}
		else
		{
			return std::basic_string<uint16_t>( slice.ptr, slice.size);
		}
	}
}

template <typename TYPE>
static std::basic_string<uint16_t> print2uint16string( const char* fmt, TYPE value)
{
	std::basic_string<uint16_t> rt;
	char buf[ 64];
	snprintf( buf, sizeof(buf), fmt, value);
	std::size_t ii = 0;
	for (; buf[ii]; ++ii) rt.push_back( (uint16_t)(unsigned char)buf[ii]);
	return rt;
}

static bool try_map2ascii( char* destbuf, std::size_t destbufsize, const uint16_t* src, std::size_t srcsize)
{
	std::size_t ii = 0;
	if (srcsize+1 >= destbufsize) return false;
	for (; ii<srcsize; ++ii)
	{
		if (src[ii] > 128) return false;
		destbuf[ ii] = src[ii];
	}
	destbuf[ ii] = 0;
	return ii;
}

static std::size_t map2ascii( char* destbuf, std::size_t destbufsize,const uint16_t* src, std::size_t srcsize, const char* where)
{
	if (!try_map2ascii( destbuf, destbufsize, src, srcsize))
	{ 
		throw strus::runtime_error( _TXT("error in %s: cannot convert string to ASCII"), where);
	}
	return srcsize;
}

ValueVariantConv::SliceW16 ValueVariantConv::towslice( std::basic_string<uint16_t>& buf, const ValueVariant& val)
{
	if (val.type == ValueVariant::WString)
	{
		return SliceW16( val.value.wstring, val.length);
	}
	else switch (val.type)
	{
		case ValueVariant::Void:
			return SliceW16();
		case ValueVariant::Double:
			buf = print2uint16string( "%.12f", val.value.Double);
			return SliceW16( buf.c_str(), buf.size());
		case ValueVariant::UInt:
			buf = print2uint16string( FORMAT_UINT, val.value.UInt);
			return SliceW16( buf.c_str(), buf.size());
		case ValueVariant::Int:
			buf = print2uint16string( FORMAT_INT, val.value.Int);
			return SliceW16( buf.c_str(), buf.size());
		case ValueVariant::String:
			buf = convert_uft8string_to_w16string( val.value.string, val.length);
			return SliceW16( buf.c_str(), buf.size());
		case ValueVariant::WString:
			return SliceW16( val.value.wstring, val.length);
		case ValueVariant::StrusObject:
			throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "host object reference", "wslice");
		case ValueVariant::StrusSerialization:
			if (val.value.serialization->size() == 1 && (*val.value.serialization)[0].tag == Serialization::Value)
			{
				return towslice( buf, (*val.value.serialization)[0]);
			}
			else
			{
				throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "serialization", "wslice");
			}
	}
	throw strus::runtime_error(_TXT("cannot convert value variant to %s: %s"), "slice of w16char string", _TXT("unknown type"));
}

static void checkError( NumParseError err, const char* type)
{
	switch (err)
	{
		case NumParserOk:
			return;
		case NumParserErrNoMem:
			throw strus::runtime_error( _TXT( "failed to convert string to %s: %s"), type, "out of memory");
		case NumParserErrConversion:
			throw strus::runtime_error( _TXT( "failed to convert string to %s: %s"), type, "conversion error");
		case NumParserErrOutOfRange:
			throw strus::runtime_error( _TXT( "failed to convert string to %s: %s"), type, "value out of range");
	}
	throw strus::runtime_error( _TXT( "failed to convert string to %s: %s"), type, "undefined error code");
}

double ValueVariantConv::todouble( const ValueVariant& val)
{
	switch (val.type)
	{
		case ValueVariant::Void:
			return 0.0;
		case ValueVariant::Double:
			return val.value.Double;
		case ValueVariant::UInt:
			return val.value.UInt; 
		case ValueVariant::Int:
			return val.value.Int; 
		case ValueVariant::String:
		{
			NumParseError err;
			double rt = doubleFromString( val.value.string, val.length, err);
			checkError( err, "double");
			return rt;
		}
		case ValueVariant::WString:
		{
			char buf[ 64];
			std::size_t bufsize = map2ascii( buf, sizeof(buf), val.value.wstring, val.length, _TXT("conversion too double"));
			NumParseError err;
			double rt = doubleFromString( buf, bufsize, err);
			checkError( err, "double");
			return rt;
		}
		case ValueVariant::StrusObject:
			throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "host object reference", "double");
		case ValueVariant::StrusSerialization:
			if (val.value.serialization->size() == 1 && (*val.value.serialization)[0].tag == Serialization::Value)
			{
				return todouble( (*val.value.serialization)[0]);
			}
			else
			{
				throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "serialization", "double");
			}
	}
	throw strus::runtime_error(_TXT("cannot convert value variant to %s: %s"), "double", _TXT("unknown type"));
}

template <typename TYPE>
static TYPE variant_touint( const ValueVariant& val)
{
	switch (val.type)
	{
		case ValueVariant::Void:
			return 0;
		case ValueVariant::Double:
		{
			return (ValueVariant::UIntType)( val.value.Double + std::numeric_limits<float>::epsilon()); 
		}
		case ValueVariant::UInt:
			if (val.value.UInt > (ValueVariant::UIntType)std::numeric_limits<TYPE>::max())
			{
				throw strus::runtime_error(_TXT("cannot convert value to unsigned integer (out of range)"));
			}
			return val.value.UInt; 
		case ValueVariant::Int:
			if (val.value.Int > (ValueVariant::IntType)std::numeric_limits<TYPE>::max()
			||  val.value.Int < 0)
			{
				throw strus::runtime_error(_TXT("cannot convert value to unsigned integer (out of range)"));
			}
			return (TYPE)(ValueVariant::UIntType)val.value.Int; 
		case ValueVariant::String:
		{
			NumParseError err;
			TYPE rt = uintFromString( val.value.string, val.length, std::numeric_limits<TYPE>::max(), err);
			checkError( err, "uint");
			return rt;
		}
		case ValueVariant::WString:
		{
			char buf[ 64];
			std::size_t bufsize = map2ascii( buf, sizeof(buf), val.value.wstring, val.length, _TXT("conversion too double"));
			NumParseError err;
			TYPE rt = uintFromString( buf, bufsize, std::numeric_limits<TYPE>::max(), err);
			checkError( err, "uint");
			return rt;
		}
		case ValueVariant::StrusObject:
			throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "host object reference", "uint");
		case ValueVariant::StrusSerialization:
			if (val.value.serialization->size() == 1 && (*val.value.serialization)[0].tag == Serialization::Value)
			{
				return variant_touint<TYPE>( (*val.value.serialization)[0]);
			}
			else
			{
				throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "serialization", "uint");
			}
	}
	throw strus::runtime_error(_TXT("cannot convert value variant to %s: %s"), "uint", _TXT("unknown type"));
}

template <typename TYPE>
static TYPE variant_toint( const ValueVariant& val)
{
	switch (val.type)
	{
		case ValueVariant::Void:
			return 0;
		case ValueVariant::Double:
			if (val.value.Double < 0.0)
			{
				if (val.value.Double < (double)std::numeric_limits<TYPE>::min()) throw strus::runtime_error(_TXT("cannot convert value to integer (out of range)"));
				return (ValueVariant::IntType)(val.value.Double - std::numeric_limits<float>::epsilon()); 
			}
			else
			{
				if (val.value.Double > (double)std::numeric_limits<TYPE>::max()) throw strus::runtime_error(_TXT("cannot convert value to integer (out of range)"));
				return (ValueVariant::IntType)(val.value.Double + std::numeric_limits<float>::epsilon());
			}
		case ValueVariant::UInt:
			if (val.value.UInt > (ValueVariant::UIntType)std::numeric_limits<TYPE>::max())
			{
				throw strus::runtime_error(_TXT("cannot convert value to unsigned integer (out of range)"));
			}
			return val.value.UInt; 
		case ValueVariant::Int:
			if (val.value.Int > (ValueVariant::IntType)std::numeric_limits<int>::max()
			||  val.value.Int < (ValueVariant::IntType)std::numeric_limits<int>::min())
			{
				throw strus::runtime_error(_TXT("cannot convert value to int (out of range)"));
			}
			return val.value.Int; 
		case ValueVariant::String:
		{
			NumParseError err;
			TYPE rt = intFromString( val.value.string, val.length, std::numeric_limits<TYPE>::max(), err);
			checkError( err, "int");
			return rt;
		}
		case ValueVariant::WString:
		{
			char buf[ 64];
			std::size_t bufsize = map2ascii( buf, sizeof(buf), val.value.wstring, val.length, _TXT("conversion too double"));
			NumParseError err;
			TYPE rt = intFromString( buf, bufsize, std::numeric_limits<TYPE>::max(), err);
			checkError( err, "int");
			return rt;
		}
		case ValueVariant::StrusObject:
			throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "host object reference", "int");
		case ValueVariant::StrusSerialization:
			if (val.value.serialization->size() == 1 && (*val.value.serialization)[0].tag == Serialization::Value)
			{
				return variant_toint<TYPE>( (*val.value.serialization)[0]);
			}
			else
			{
				throw strus::runtime_error(_TXT("cannot convert value variant %s to %s"), "serialization", "int");
			}
	}
	throw strus::runtime_error(_TXT("cannot convert value variant to %s: %s"), "int", _TXT("unknown type"));
}

int ValueVariantConv::toint( const ValueVariant& val)
{
	return variant_toint<int>( val);
}


unsigned int ValueVariantConv::touint( const ValueVariant& val)
{
	return variant_touint<unsigned int>( val);
}

int64_t ValueVariantConv::toint64( const ValueVariant& val)
{
	return variant_toint<int64_t>( val);
}

uint64_t ValueVariantConv::touint64( const ValueVariant& val)
{
	return variant_touint<uint64_t>( val);
}

bool ValueVariantConv::tobool( const ValueVariant& val)
{
	unsigned int rt = variant_touint<unsigned int>( val);
	if (rt > 1) throw strus::runtime_error(_TXT("cannot convert value to bool: must be 0 or 1"));
	return !!rt;
}

bool ValueVariantConv::isequal_ascii( const ValueVariant& val, const char* value)
{
	if (val.type == ValueVariant::String)
	{
		return (std::strlen( value) == (std::size_t)val.length
			&& std::memcmp( val.value.string, value, val.length) == 0);
	}
	else if (val.type == ValueVariant::WString)
	{
		std::size_t wi = 0, we = val.length;
		for (; wi != we; ++wi)
		{
			if (val.value.wstring[wi] != (ValueVariant::WCharType)(unsigned char)value[wi])
			{
				return false;
			}
		}
		return (0==value[wi]);
	}
	else
	{
		return false;
	}
}

static bool try_convertStringToNumber( ValueVariant& val, const char* str, std::size_t strsize)
{
	bool sign = false;
	std::size_t si = 0, se = strsize;
	if (str[si] == '-')
	{
		++si;
		sign = true;
	}
	if (str[si] >= '0' && str[si] <= '9')
	{
		for (++si; si != se && str[si] >= '0' && str[si] <= '9'; ++si){}
	}
	else
	{
		return false;
	}
	if (si == se)
	{
		if (sign)
		{
			NumParseError err;
			int64_t res = uintFromString( str, strsize, std::numeric_limits<int64_t>::max(), err);
			if (err == NumParserOk)
			{
				val.init( -res);
				return true;
			}
		}
		else
		{
			NumParseError err;
			uint64_t res = uintFromString( str, strsize, std::numeric_limits<uint64_t>::max(), err);
			if (err == NumParserOk)
			{
				val.init( res);
				return true;
			}
		}
	}
	else if (str[si] == '.')
	{
		++si;
		for (; si != se && str[si] >= '0' && str[si] <= '9'; ++si){}
		if (si == se)
		{
			NumParseError err;
			double res = doubleFromString( str, strsize, err);
			if (err == NumParserOk)
			{
				val.init( sign?(-res):res);
				return true;
			}
		}
	}
	return false;
}

bool ValueVariantConv::try_convertToNumber( ValueVariant& val)
{
	if (!val.isAtomicType()) return false;
	if (!val.isNumericType()) return true;
	if (val.type == ValueVariant::String && val.length < 64)
	{
		return try_convertStringToNumber( val, val.value.string, val.length);
	}
	if (val.type == ValueVariant::String && val.length < 64)
	{
		char buf[ 128];
		if (try_map2ascii( buf, sizeof(buf), val.value.wstring, val.length))
		{
			return try_convertStringToNumber( val, buf, val.length);
		}
	}
	return false;
}

