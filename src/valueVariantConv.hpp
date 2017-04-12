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
#include "strus/bindings/valueVariant.hpp"
#include "strus/base/stdint.h"
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

	static std::string tostring( const ValueVariant& val);
	static Slice toslice( std::string& buf, const ValueVariant& val);

	static std::basic_string<uint16_t> towstring( const ValueVariant& val);
	static SliceW16 towslice( std::basic_string<uint16_t>& buf, const ValueVariant& val);

	static double todouble( const ValueVariant& val);
	static int toint( const ValueVariant& val);
	static unsigned int touint( const ValueVariant& val);
	static int64_t toint64( const ValueVariant& val);
	static uint64_t touint64( const ValueVariant& val);
	static bool tobool( const ValueVariant& val);

	static bool isequal_ascii( const ValueVariant& val, const char* value);
	static bool try_convertToNumber( ValueVariant& val);

	static bool try_map2ascii( char* destbuf, std::size_t destbufsize, const uint16_t* src, std::size_t srcsize);
	static std::size_t map2ascii( char* destbuf, std::size_t destbufsize,const uint16_t* src, std::size_t srcsize, const char* where);
};

}}//namespace
#endif

