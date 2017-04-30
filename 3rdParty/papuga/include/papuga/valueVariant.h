/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_VALUE_VARIANT_H_INCLUDED
#define _PAPUGA_VALUE_VARIANT_H_INCLUDED
/// \brief Representation of a typed value for language bindings
/// \file valueVariant.h

#ifdef _MSC_VER
#error stdint definitions missing for Windows
#else
#include <stdint.h>
#endif
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct papuga_Node papuga_Node;
typedef void papuga_HostObjectType;
typedef enum {
	papuga_Void			= 0x00,
	papuga_Double			= 0x01,
	papuga_UInt			= 0x02,
	papuga_Int			= 0x03,
	papuga_String			= 0x04,
	papuga_HostObject		= 0x05,
	papuga_Serialization		= 0x06
} papuga_Type;

/// \brief Enumeration of value type identifiers
#define papuga_NumericTypeMask = (1U<<(unsigned int)papuga_UInt)|(1U<<(unsigned int)papuga_Int)|(1U<<(unsigned int)papuga_Double)
#define papuga_AtomicTypeMask  = (unsigned int)papuga_NumericTypeMask | (1U<<(unsigned int)papuga_String)

typedef uint64_t papuga_UIntType;
typedef int64_t papuga_IntType;
typedef double papuga_FloatType;

typedef enum {
	papuga_UTF8,
	papuga_UTF16BE,
	papuga_UTF16LE,
	papuga_UTF16,
	papuga_UTF32BE,
	papuga_UTF32LE,
	papuga_UTF32
} papuga_StringEncoding;

/// \brief Representation of a typed value const reference or an owned object with reference count
typedef struct papuga_ValueVariant
{
	int valuetype:8;
	int encoding:8;
	int classid:16;
	int32_t length;
	union {
		double Double;
		uint64_t UInt;
		int64_t Int;
		const char* string;
		const void* langstring;
		const papuga_HostObjectType* hostObject;
		const papuga_Node* serialization;
	} value;
} papuga_ValueVariant;

#define papuga_init_ValueVariant(self)				{self.valuetype = (unsigned char)papuga_Void; self.encoding=0; self.classid=0; self.length=0; value.string=0;}
#define papuga_init_ValueVariant_double(self,val)		{self.valuetype = (unsigned char)papuga_Double; self.encoding=0; self.classid=0; self.length=0; value.Double=val;}
#define papuga_init_ValueVariant_uint(self,val)			{self.valuetype = (unsigned char)papuga_UInt; self.encoding=0; self.classid=0; self.length=0; value.UInt=val;}
#define papuga_init_ValueVariant_int(self,val)			{self.valuetype = (unsigned char)papuga_Int; self.encoding=0; self.classid=0; self.length=0; value.Int=val;}
#define papuga_init_ValueVariant_charp(self,val)		{self.valuetype = (unsigned char)papuga_String; self.encoding=papuga_UTF8; self.classid=0; self.length=strlen(val); value.string=val;}
#define papuga_init_ValueVariant_string(self,val,sz)		{self.valuetype = (unsigned char)papuga_String; self.encoding=papuga_UTF8; self.classid=0; self.length=sz; value.string=val;}
#define papuga_init_ValueVariant_langstring(self,enc,val,sz)	{self.valuetype = (unsigned char)papuga_String; self.encoding=enc; self.classid=0; self.length=sz; value.string=val;}
#define papuga_init_ValueVariant_hostobj(self,objptr,clid)	{self.valuetype = (unsigned char)papuga_HostObject; self.encoding=0; self.classid=clid; self.length=0; value.hostObject=objptr;}
#define papuga_init_ValueVariant_serialization(self,ar,sz)	{self.valuetype = (unsigned char)papuga_Serialization; self.encoding=0; self.classid=0; self.length=sz; value.serialization=ar;}

#define papuga_assign_ValueVariant(self,o)			{self.valuetype=o.valuetype;self.encoding=o.encoding;self.classid=o.classid;self.length=o.length;self.value=o.value;}
#define papuga_ValueVariant_defined(self)			(self.valuetype!=papuga_Void)

#define papuga_ValueVariant_isnumeric(self)			(0!=(self.valuetype & papuga_NumericTypeMask))
#define papuga_ValueVariant_isatomic(self)			(0!=(self.valuetype & papuga_AtomicTypeMask))
#define papuga_ValueVariant_isstring(self)			(self.valuetype==papuga_String)

#ifdef __cplusplus
}
#endif
#endif

