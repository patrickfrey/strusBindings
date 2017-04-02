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
#include <string>
#include <cstring>

namespace strus {
namespace bindings {

class Serialization;

/// \brief Representation of a typed value const reference or an owned object with reference count
struct ValueVariant
{
	typedef void* StrusObjectType;

	/// \brief Enumeration of value type identifiers
	enum Type {
		Void			= 0x00,
		Double			= 0x01,
		UInt			= 0x02,
		Int			= 0x03,
		String			= 0x04,
		WString			= 0x05,
		StrusObject		= 0x06,
		StrusSerialization	= 0x07
	};
	typedef uint64_t UIntType;
	typedef int64_t IntType;
	typedef double FloatType;
	typedef uint16_t WCharType;

	ValueVariant()							{value.string = 0; type = Void; length = 0;}
	ValueVariant( double Double_)					{value.Double = Double_; type = Double; length = 0;}
	ValueVariant( uint64_t UInt_)					{value.UInt = UInt_; type = UInt; length = 0;}
	ValueVariant( int64_t Int_)					{value.Int = Int_; type = Int; length = 0;}
	ValueVariant( const char* string_)				{value.string = string_; type = String; length = std::strlen(string_);}
	ValueVariant( const char* s, std::size_t l)			{value.string = s; type = String; length = l;}
	ValueVariant( const std::string& s)				{value.string = s.c_str(); type = String; length = s.size();}
	ValueVariant( const uint16_t* s, std::size_t l)			{value.wstring = s; type = WString; length = l;}
	ValueVariant( const StrusObjectType* s_)			{value.strusObject = s_; type = StrusObject; length = 0;}
	ValueVariant( const Serialization* s_)				{value.serialization = s_; type = StrusSerialization; length = 0;}
	ValueVariant( const NumericVariant& num)
	{
		assign( num);
	}
	ValueVariant( const ValueVariant& o)
	{
		assign( o);
	}
	void operator=( const ValueVariant& o)
	{
		assign( o);
	}
	~ValueVariant(){}

	void init( double Double_)					{value.Double = Double_; type = Double; length=0;}
	void init( uint64_t UInt_)					{value.UInt = UInt_; type = UInt; length=0;}
	void init( int64_t Int_)					{value.Int = Int_; type = Int; length=0;}
	void init( const char* string_)					{value.string = string_; type = String; length = std::strlen(string_);}
	void init( const char* s, std::size_t l)			{value.string = s; type = String; length = l;}
	void init( const std::string& s)				{value.string = s.c_str(); type = String; length = s.size();}
	void init( const uint16_t* s, std::size_t l)			{value.wstring = s; type = WString; length = l;}
	void init( const StrusObjectType* s_)				{value.strusObject = s_; type = StrusObject; length=0;}
	void init( const Serialization* s)				{value.serialization = s; type = StrusSerialization; length=0;} 
	void init( const NumericVariant& num)				{assign( num);}

	void clear()
	{
		value.Int = 0; type = Void; length = 0;
	}

	Type type;
	union {
		double Double;
		uint64_t UInt;
		int64_t Int;
		const char* string;
		const uint16_t* wstring;
		const StrusObjectType* strusObject;
		const Serialization* serialization;
	} value;
	int length;

private:
	void assign( const ValueVariant& o)
	{
		type = o.type;
		value = o.value;
		length = o.length;
	}
	void assign( const NumericVariant& num)
	{
		switch (num.type)
		{
			case NumericVariant::Null: break;
			case NumericVariant::Int:
				value.Int = num.variant.Int; type = Int; length = 0;
				return;
			case NumericVariant::UInt:
				value.UInt = num.variant.Int; type = UInt; length = 0;
				return;
			case NumericVariant::Float:
				value.Double = num.variant.Float; type = Double; length = 0;
				return;
		}
		clear(); 
	}
};

}}//namespace
#endif

