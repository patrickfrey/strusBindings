/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_VALUE_VARIANT_HPP_INCLUDED
#define _PAPUGA_VALUE_VARIANT_HPP_INCLUDED
/// \brief Representation of a typed value for language bindings
/// \file valueVariant.hpp
#include <string>
#include <cstring>
#ifdef _MSC_VER
#error stdint definitions missing for Windows
#else
#include <stdint.h>
#endif

namespace papuga {

class Serialization;

/// \brief Representation of a typed value const reference or an owned object with reference count
struct ValueVariant
{
	typedef void HostObjectType;

	/// \brief Enumeration of value type identifiers
	enum Type {
		Void			= 0x00,
		Double			= 0x01,
		UInt			= 0x02,
		Int			= 0x03,
		String			= 0x04,
		WString			= 0x05,
		HostObject		= 0x06,
		Serialization		= 0x07
	};
	enum {
		NumericTypeMask = (1U<<(unsigned int)UInt)|(1U<<(unsigned int)Int)|(1U<<(unsigned int)Double),
		StringTypeMask  = (1U<<(unsigned int)UInt)|(1U<<(unsigned int)Int)|(1U<<(unsigned int)Double),
		AtomicTypeMask  = (unsigned int)NumericTypeMask | (unsigned int)StringTypeMask
	};

	typedef uint64_t UIntType;
	typedef int64_t IntType;
	typedef double FloatType;
	typedef uint16_t WCharType;

	ValueVariant()							{value.string = 0; type = Void; attribute.length = 0;}
	ValueVariant( double Double_)					{value.Double = Double_; type = Double; attribute.length = 0;}
	ValueVariant( uint64_t UInt_)					{value.UInt = UInt_; type = UInt; attribute.length = 0;}
	ValueVariant( int64_t Int_)					{value.Int = Int_; type = Int; attribute.length = 0;}
	ValueVariant( const char* string_)				{value.string = string_; type = String; attribute.length = std::strlen(string_);}
	ValueVariant( const char* s, std::size_t l)			{value.string = s; type = String; attribute.length = l;}
	ValueVariant( const std::string& s)				{value.string = s.c_str(); type = String; attribute.length = s.size();}
	ValueVariant( const uint16_t* s, std::size_t l)			{value.wstring = s; type = WString; attribute.length = l;}
	ValueVariant( const HostObjectType* s_, int classid_)		{value.hostObject = s_; type = HostObject; attribute.classid = classid_;}
	ValueVariant( const papuga::Serialization* s_)			{value.serialization = s_; type = Serialization; attribute.length = 0;}
	ValueVariant( const ValueVariant& o)
	{
		assign( o);
	}
	void operator=( const ValueVariant& o)
	{
		assign( o);
	}
	~ValueVariant(){}

	void init()							{value.Int = 0; type = Void; attribute.length=0;}
	void init( bool Boolean_)					{value.Int = Boolean_; type = Int; attribute.length=0;}
	void init( double Double_)					{value.Double = Double_; type = Double; attribute.length=0;}
	void init( uint64_t UInt_)					{value.UInt = UInt_; type = UInt; attribute.length=0;}
	void init( int64_t Int_)					{value.Int = Int_; type = Int; attribute.length=0;}
	void init( const char* string_)					{value.string = string_; type = String; attribute.length = std::strlen(string_);}
	void init( const char* s, std::size_t l)			{value.string = s; type = String; attribute.length = l;}
	void init( const std::string& s)				{value.string = s.c_str(); type = String; attribute.length = s.size();}
	void init( const uint16_t* s, std::size_t l)			{value.wstring = s; type = WString; attribute.length = l;}
	void init( const HostObjectType* s_, int classid_=-1)		{value.hostObject = s_; type = HostObject; attribute.classid=classid_;}
	void init( const papuga::Serialization* s)			{value.serialization = s; type = Serialization; attribute.length=0;} 

	void clear()
	{
		value.Int = 0; type = Void; attribute.length = 0;
	}
	bool defined() const
	{
		return type != Void;
	}

	Type type;
	union {
		double Double;
		uint64_t UInt;
		int64_t Int;
		const char* string;
		const uint16_t* wstring;
		const HostObjectType* hostObject;
		const papuga::Serialization* serialization;
	} value;
	union {
		int length;
		int classid;
	} attribute;

	int length() const	{return attribute.length;}
	int classid() const	{return attribute.classid;}

	bool isNumericType() const
	{
		return (type & NumericTypeMask) != 0;
	}
	bool isStringType() const
	{
		return (type & StringTypeMask) != 0;
	}
	bool isAtomicType() const
	{
		return (type & AtomicTypeMask) != 0;
	}

private:
	void assign( const ValueVariant& o)
	{
		type = o.type;
		value = o.value;
		attribute = o.attribute;
	}
};

}//namespace
#endif

