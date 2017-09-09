/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_VALUE_VARIANT_CONVERSIONS_HPP_INCLUDED
#define _STRUS_BINDINGS_VALUE_VARIANT_CONVERSIONS_HPP_INCLUDED
/// \brief Conversion methods for the variant type
/// \file valueVariantConv.hpp
#include "papuga/valueVariant.hpp"
#include "strus/bindingClassId.hpp"
#include "strus/base/stdint.h"
#include "callResultUtils.hpp"
#include <string>
#include <cstring>

namespace strus {
namespace bindings {

class ValueVariantConv
{
public:
	/// \brief slice of chars (UTF8)
	struct Slice
	{
		const char* ptr;
		std::size_t size;

		Slice() :ptr(0),size(0){}
		Slice( const char* ptr_, std::size_t size_) :ptr(ptr_),size(size_){}
		Slice( const Slice& o) :ptr(o.ptr),size(o.size){}

		bool operator==( const std::string& o)
		{
			return o.size() == size && 0==std::memcmp( ptr, o.c_str(), size);
		}
	};
	/// \brief slice of chars (UTF16)
	struct SliceW16
	{
		const uint16_t* ptr;
		std::size_t size;

		SliceW16() :ptr(0),size(0){}
		SliceW16( const uint16_t* ptr_, std::size_t size_) :ptr(ptr_),size(size_){}
		SliceW16( const SliceW16& o) :ptr(o.ptr),size(o.size){}
	};

	static std::string tostring( const papuga::ValueVariant& val);
	static Slice toslice( std::string& buf, const papuga::ValueVariant& val);
	static const char* tocharp( std::string& buf, const papuga::ValueVariant& val)
	{
		return toslice( buf, val).ptr;
	}

	static std::basic_string<uint16_t> towstring( const papuga::ValueVariant& val);
	static SliceW16 towslice( std::basic_string<uint16_t>& buf, const papuga::ValueVariant& val);

	static double todouble( const papuga::ValueVariant& val);
	static float tofloat( const papuga::ValueVariant& val);
	static int toint( const papuga::ValueVariant& val);
	static unsigned int touint( const papuga::ValueVariant& val);
	static int64_t toint64( const papuga::ValueVariant& val);
	static uint64_t touint64( const papuga::ValueVariant& val);
	static bool tobool( const papuga::ValueVariant& val);
	static NumericVariant tonumeric( const papuga::ValueVariant& val);

	static ValueVariant fromnumeric( const NumericVariant& num)
	{
		switch (num.type)
		{
			case NumericVariant::Null: break;
			case NumericVariant::Int:
				return papuga::ValueVariant( (papuga::ValueVariant::IntType)num.variant.Int);
			case NumericVariant::UInt:
				return papuga::ValueVariant( (papuga::ValueVariant::UIntType)num.variant.UInt);
			case NumericVariant::Float:
				return papuga::ValueVariant( num.variant.Float);
		}
		return papuga::ValueVariant();
	}

	static bool isequal_ascii( const papuga::ValueVariant& val, const char* value);
	static bool try_convertToNumber( papuga::ValueVariant& val);

	static bool try_map2ascii( char* destbuf, std::size_t destbufsize, const uint16_t* src, std::size_t srcsize);
	static std::size_t map2ascii( char* destbuf, std::size_t destbufsize,const uint16_t* src, std::size_t srcsize, const char* where);
};

}}//namespace
#endif

