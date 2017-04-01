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

/// \brief Representation of a typed value const reference or an owned object with reference count
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

	ValueVariant()							{value.string = 0; type = Void; attr.length = 0; deleter = 0;}
	ValueVariant( double Double_)					{value.Double = Double_; type = Double; attr.length = 0; deleter = 0;}
	ValueVariant( uint64_t UInt_)					{value.UInt = UInt_; type = UInt; attr.length = 0; deleter = 0;}
	ValueVariant( int64_t Int_)					{value.Int = Int_; type = Int; attr.length = 0; deleter = 0;}
	ValueVariant( const char* string_)				{value.string = string_; type = String; attr.length = std::strlen(string_); deleter = 0;}
	ValueVariant( const char* s, std::size_t l)			{value.string = s; type = String; attr.length = l; deleter = 0;}
	ValueVariant( const std::string& s)				{value.string = s.c_str(); type = String; attr.length = s.size(); deleter = 0;}
	ValueVariant( const uint16_t* s, std::size_t l)			{value.wstring = s; type = WString; attr.length = l; deleter = 0;}
	ValueVariant( StrusObjectType* s_, StrusObjectDeleter d_)	{value.strusObject = s_; type = StrusObject; deleter = d_; attr.refcnt=!!deleter;}
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
	~ValueVariant()
	{
		reset();
	}

	void init( double Double_)					{reset(); value.Double = Double_; type = Double;}
	void init( uint64_t UInt_)					{reset(); value.UInt = UInt_; type = UInt;}
	void init( int64_t Int_)					{reset(); value.Int = Int_; type = Int;}
	void init( const char* string_)					{reset(); value.string = string_; type = String; attr.length = std::strlen(string_);}
	void init( const char* s, std::size_t l)			{reset(); value.string = s; type = String; attr.length = l;}
	void init( const std::string& s)				{reset(); value.string = s.c_str(); type = String; attr.length = s.size(); deleter = 0;}
	void init( const uint16_t* s, std::size_t l)			{reset(); value.wstring = s; type = WString; attr.length = l;}
	void init( StrusObjectType* s_, StrusObjectDeleter d_)		{reset(); value.strusObject = s_; type = StrusObject; deleter = d_; if (deleter) attr.refcnt=1;}
	void init( const NumericVariant& num)
	{
		reset();
		assign( num);
	}

	void clear()
	{
		reset(); value.Int = 0; type = Void; attr.length = 0;
	}
	void release()
	{
		if (type == StrusObject) {deleter = 0; attr.refcnt=0;}
	}

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
		int length;
		mutable int refcnt;
	} attr;
	StrusObjectDeleter deleter;

private:
	void reset()
	{
		if (type == StrusObject && deleter)
		{
			if (!--attr.refcnt) deleter(value.strusObject);
			deleter=0;
		}
	}
	void assign( const ValueVariant& o)
	{
		type = o.type;
		value = o.value;
		attr = o.attr;
		deleter = o.deleter;
		if (deleter) ++o.attr.refcnt;
		attr.refcnt = o.attr.refcnt;
	}
	void assign( const NumericVariant& num)
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
};

}}//namespace
#endif

