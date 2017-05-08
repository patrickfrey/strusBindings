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
#include "papuga/typedefs.h"

#ifdef _MSC_VER
#error stdint definitions missing for Windows
#else
#include <stdint.h>
#endif
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Enumeration of value type identifiers
#define papuga_NumericTypeMask	((1U<<(unsigned int)papuga_UInt)|(1U<<(unsigned int)papuga_Int)|(1U<<(unsigned int)papuga_Double))
#define papuga_StringTypeMask	((1U<<(unsigned int)papuga_String) | (1U<<(unsigned int)papuga_LangString))
#define papuga_AtomicTypeMask	(((unsigned int)papuga_NumericTypeMask | (unsigned int)papuga_StringTypeMask))

/// \brief Value Variant initializer
#define papuga_init_ValueVariant(self)				{(self)->valuetype = (unsigned char)papuga_Void; (self)->encoding=0; (self)->classid=0; (self)->length=0; (self)->value.string=0;}
#define papuga_init_ValueVariant_double(self,val)		{(self)->valuetype = (unsigned char)papuga_Double; (self)->encoding=0; (self)->classid=0; (self)->length=0; (self)->value.Double=val;}
#define papuga_init_ValueVariant_bool(self,val)			{(self)->valuetype = (unsigned char)papuga_Int; (self)->encoding=0; (self)->classid=0; (self)->length=0; (self)->value.Int=!!val;}
#define papuga_init_ValueVariant_uint(self,val)			{(self)->valuetype = (unsigned char)papuga_UInt; (self)->encoding=0; (self)->classid=0; (self)->length=0; (self)->value.UInt=val;}
#define papuga_init_ValueVariant_int(self,val)			{(self)->valuetype = (unsigned char)papuga_Int; (self)->encoding=0; (self)->classid=0; (self)->length=0; (self)->value.Int=val;}
#define papuga_init_ValueVariant_charp(self,val)		{(self)->valuetype = (unsigned char)papuga_String; (self)->encoding=papuga_UTF8; (self)->classid=0; (self)->length=strlen(val); (self)->value.string=val;}
#define papuga_init_ValueVariant_string(self,val,sz)		{(self)->valuetype = (unsigned char)papuga_String; (self)->encoding=papuga_UTF8; (self)->classid=0; (self)->length=sz; (self)->value.string=val;}
#define papuga_init_ValueVariant_langstring(self,enc,val,sz)	{(self)->valuetype = (unsigned char)papuga_LangString; (self)->encoding=enc; (self)->classid=0; (self)->length=sz; (self)->value.langstring=val;}
#define papuga_init_ValueVariant_hostobj(self,objptr,clid)	{(self)->valuetype = (unsigned char)papuga_HostObject; (self)->encoding=0; (self)->classid=clid; (self)->length=0; (self)->value.hostObject=objptr;}
#define papuga_init_ValueVariant_serialization(self,ser)	{(self)->valuetype = (unsigned char)papuga_Serialized; (self)->encoding=0; (self)->classid=0; (self)->length=0; (self)->value.serialization=ser;}

#define papuga_init_ValueVariant_copy(self,o)			{(self)->valuetype=(o)->valuetype;(self)->encoding=(o)->encoding;(self)->classid=(o)->classid;(self)->length=(o)->length;(self)->value.string=(o)->value.string;}
#define papuga_ValueVariant_defined(self)			((self)->valuetype!=papuga_Void)

/// \brief Type check
#define papuga_ValueVariant_isnumeric(self)			(0!=((self)->valuetype & papuga_NumericTypeMask))
#define papuga_ValueVariant_isatomic(self)			(0!=((self)->valuetype & papuga_AtomicTypeMask))
#define papuga_ValueVariant_isstring(self)			(0!=((self)->valuetype & papuga_StringTypeMask))

/// \brief Conversion functions
const char* papuga_ValueVariant_tostring( const papuga_ValueVariant* value, papuga_StringBuffer* buf, size_t* len, papuga_ErrorCode* err);
const void* papuga_ValueVariant_tolangstring( const papuga_ValueVariant* value, papuga_StringEncoding enc, void* buf, size_t bufsize, size_t* len, papuga_ErrorCode* err);
int64_t papuga_ValueVariant_toint( const papuga_ValueVariant* value, papuga_ErrorCode* err);
uint64_t papuga_ValueVariant_touint( const papuga_ValueVariant* value, papuga_ErrorCode* err);
double papuga_ValueVariant_todouble( const papuga_ValueVariant* value, papuga_ErrorCode* err);
bool papuga_ValueVariant_tobool( const papuga_ValueVariant* value, papuga_ErrorCode* err);
papuga_ValueVariant* papuga_ValueVariant_tonumeric( const papuga_ValueVariant* value, papuga_ValueVariant* res, papuga_ErrorCode* err);

bool papuga_ValueVariant_isequal_ascii( const papuga_ValueVariant* val, const char* value);
bool papuga_ValueVariant_starts_ascii( const papuga_ValueVariant* val, const char* value);
char* papuga_ValueVariant_toascii( char* destbuf, size_t destbufsize, const papuga_ValueVariant* val);

#ifdef __cplusplus
}
#endif
#endif

