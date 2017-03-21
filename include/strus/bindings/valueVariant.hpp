/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_VALUE_VARIANT_HPP_INCLUDED
#define _STRUS_BINDINGS_VALUE_VARIANT_HPP_INCLUDED
/// \brief Representation of a typed value for language bindings
/// \file valueVariant.hpp
#include "strus/base/stdint.h"
#include "strus/numericVariant.hpp"
#include <cstring>

namespace strus {
namespace bindings {

/// \brief Representation of a typed value
struct ValueVariant
{
	typedef void* StrusObjectType;
	typedef void (*StrusObjectDeleter)( void* strusObject);

	/// \brief Enumeration of value type identifiers
	enum Type {
		Void		= 0x00,
		Double		= 0x01,
		UInt		= 0x02,
		Int		= 0x03,
		String		= 0x04,
		WString		= 0x05,
		StrusObject	= 0x06
	};
	typedef uint64_t UIntType;
	typedef int64_t IntType;
	typedef double FloatType;
	typedef uint16_t WCharType;

	ValueVariant()						{type = Void;}

	void init( double Double_)				{if (type == StrusObject && attr.deleter) attr.deleter(value.strusObject); value.Double = Double_; type = Double;}
	void init( uint64_t UInt_)				{if (type == StrusObject && attr.deleter) attr.deleter(value.strusObject); value.UInt = UInt_; type = UInt;}
	void init( int64_t Int_)				{if (type == StrusObject && attr.deleter) attr.deleter(value.strusObject); value.Int = Int_; type = Int;}
	void init( const char* string_)				{if (type == StrusObject && attr.deleter) attr.deleter(value.strusObject); value.string = string_; type = String; attr.length = std::strlen(string_);}
	void init( const char* s, std::size_t l)		{if (type == StrusObject && attr.deleter) attr.deleter(value.strusObject); value.string = s; type = String; attr.length = l;}
	void init( const uint16_t* s, std::size_t l)		{if (type == StrusObject && attr.deleter) attr.deleter(value.strusObject); value.wstring = s; type = WString; attr.length = l;}
	void init( StrusObjectType* s_, StrusObjectDeleter d_)	{if (type == StrusObject && attr.deleter) attr.deleter(value.strusObject); value.strusObject = s_; type = StrusObject; attr.deleter = d_;}
	void init( const NumericVariant& num)
	{
		switch (num.type)
		{
			case NumericVariant::Null: break;
			case NumericVariant::Int:
				value.Int = num.variant.Int; type = Int; attr.length = 0;
				return;
			case NumericVariant::UInt:
				value.UInt = num.variant.Int; type = UInt; attr.length = 0;
				return;
			case NumericVariant::Float:
				value.Double = num.variant.Float; type = Double; attr.length = 0;
				return;
		}
		value.Int = 0; type = Void; attr.length = 0; 
	}
	~ValueVariant()
	{
		if (type == StrusObject && attr.deleter)
		{
			attr.deleter( value.strusObject);
		}
	}

	void clear()					{if (type == StrusObject && attr.deleter) attr.deleter( value.strusObject); value.Int = 0; type = Void; attr.length = 0;}
	void release()					{if (type == StrusObject) attr.deleter = 0;}

	Type type;
	union {
		double Double;
		uint64_t UInt;
		int64_t Int;
		const char* string;
		const uint16_t* wstring;
		StrusObjectType* strusObject;
	} value;
	union {
		std::size_t length;
		StrusObjectDeleter deleter;
	} attr;

private:
	ValueVariant( const ValueVariant&){}		//< non copyable
	void operator=( const ValueVariant&){}		//< non copyable
};

}}//namespace
#endif

