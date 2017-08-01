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

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Mask for checking a variant value type for being numeric
#define papuga_NumericTypeMask	((1U<<(unsigned int)papuga_TypeUInt)|(1U<<(unsigned int)papuga_TypeInt)|(1U<<(unsigned int)papuga_TypeBool)|(1U<<(unsigned int)papuga_TypeDouble))

/// \brief Mask for checking a variant value type for being a string
#define papuga_StringTypeMask	((1U<<(unsigned int)papuga_TypeString) | (1U<<(unsigned int)papuga_TypeLangString))

/// \brief Mask for checking a variant value type for being atomic (numeric or a string)
#define papuga_AtomicTypeMask	(((unsigned int)papuga_NumericTypeMask | (unsigned int)papuga_StringTypeMask))

/// \brief Variant value initializer as a NULL value
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant(self)				{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeVoid; s->encoding=0; s->length=0; s->value.string=0;}

/// \brief Variant value initializer as a double precision floating point value
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_double(self,val)		{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeDouble; s->encoding=0; s->length=0; s->value.Double=(val);}

/// \brief Variant value initializer as a boolean value
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_bool(self,val)			{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeBool; s->encoding=0; s->length=0; s->value.Bool=!!(val);}

/// \brief Variant value initializer as an unsigned integer value
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_uint(self,val)			{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeUInt; s->encoding=0; s->length=0; s->value.UInt=(val);}

/// \brief Variant value initializer as a signed integer value
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_int(self,val)			{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeInt; s->encoding=0; s->length=0; s->value.Int=(val);}

/// \brief Variant value initializer as c string (UTF-8) reference
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_charp(self,val)		{papuga_ValueVariant* s = self; const char* o = val; s->valuetype = (unsigned char)papuga_TypeString; s->encoding=papuga_UTF8; s->length=strlen(o); s->value.string=o;}

/// \brief Variant value initializer as c string (UTF-8) reference with size
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_string(self,val,sz)		{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeString; s->encoding=papuga_UTF8; s->length=(sz); s->value.string=(val);}

/// \brief Variant value initializer as unicode string reference with size and encoding
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_langstring(self,enc,val,sz)	{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeLangString; s->encoding=(enc); s->length=(sz); s->value.langstring=(val);}

/// \brief Variant value initializer as a reference to a host object
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_hostobj(self,hostobj)		{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeHostObject; s->encoding=0; s->length=0; s->value.hostObject=(hostobj);}

/// \brief Variant value initializer as a serialization of an object defined in the language binding
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_serialization(self,ser)	{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeSerialization; s->encoding=0; s->length=0; s->value.serialization=(ser);}

/// \brief Variant value initializer as a serialization of an object defined in the language binding
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_iterator(self,itr)		{papuga_ValueVariant* s = self; s->valuetype = (unsigned char)papuga_TypeIterator; s->encoding=0; s->length=0; s->value.iterator=(itr);}

/// \brief Variant value initializer as a shallow copy of another variant value
/// \param[out] self pointer to structure 
#define papuga_init_ValueVariant_copy(self,o)			{papuga_ValueVariant* s = self; const papuga_ValueVariant* v = o; s->valuetype=v->valuetype;s->encoding=v->encoding;s->length=v->length;s->value.string=v->value.string;}

/// \brief Test if the variant value is not NULL
/// \param[in] self pointer to structure 
/// \return true, if yes
#define papuga_ValueVariant_defined(self)			((self)->valuetype!=papuga_TypeVoid)

/// \brief Test if the variant value is numeric
/// \param[in] self pointer to structure 
/// \return true, if yes
#define papuga_ValueVariant_isnumeric(self)			(0!=((1U << (self)->valuetype) & papuga_NumericTypeMask))

/// \brief Test if the variant value is atomic
/// \param[in] self pointer to structure 
/// \return true, if yes
#define papuga_ValueVariant_isatomic(self)			(0!=((1U << (self)->valuetype) & papuga_AtomicTypeMask))

/// \brief Test if the variant value is a string
/// \param[in] self pointer to structure 
/// \return true, if yes
#define papuga_ValueVariant_isstring(self)			(0!=((1U << (self)->valuetype) & papuga_StringTypeMask))

/// \brief Convert a value variant to a null terminated C-string
/// \param[in] self pointer to structure 
/// \param[in,out] allocator allocator to use for deep copy of string
/// \param[out] len length of the string copied
/// \param[out] err error code in case of error (untouched if call succeeds)
/// \return the pointer to the result string
const char* papuga_ValueVariant_tostring( const papuga_ValueVariant* self, papuga_Allocator* allocator, size_t* len, papuga_ErrorCode* err);

/// \brief Convert a value variant to a unicode string of a specific encoding
/// \param[in] self pointer to structure 
/// \param[in] enc encoding of the result string
/// \param[out] buf pointer to character buffer to use for deep copies
/// \param[in] bufsize allocation size of 'buf' in bytes
/// \param[out] len length of the string copied
/// \param[out] err error code in case of error (untouched if call succeeds)
/// \return the pointer to the result string
const void* papuga_ValueVariant_tolangstring( const papuga_ValueVariant* self, papuga_StringEncoding enc, void* buf, size_t bufsize, size_t* len, papuga_ErrorCode* err);

/// \brief Convert a value variant to a signed integer
/// \param[in] self pointer to structure 
/// \param[out] err error code in case of error (untouched if call succeeds)
/// \return the result value in case the conversion succeeded
int64_t papuga_ValueVariant_toint( const papuga_ValueVariant* self, papuga_ErrorCode* err);

/// \brief Convert a value variant to an unsigned integer
/// \param[in] self pointer to structure 
/// \param[out] err error code in case of error (untouched if call succeeds)
/// \return the result value in case the conversion succeeded
uint64_t papuga_ValueVariant_touint( const papuga_ValueVariant* self, papuga_ErrorCode* err);

/// \brief Convert a value variant to a double precision floating point value
/// \param[in] self pointer to structure 
/// \param[out] err error code in case of error (untouched if call succeeds)
/// \return the result value in case the conversion succeeded
double papuga_ValueVariant_todouble( const papuga_ValueVariant* self, papuga_ErrorCode* err);

/// \brief Convert a value variant to a boolean value
/// \param[in] self pointer to structure 
/// \param[out] err error code in case of error (untouched if call succeeds)
/// \return the result value in case the conversion succeeded
bool papuga_ValueVariant_tobool( const papuga_ValueVariant* self, papuga_ErrorCode* err);

/// \brief Convert a value variant to a numeric value
/// \param[in] self pointer to structure 
/// \param[out] err error code in case of error (untouched if call succeeds)
/// \return pointer to the result value in case the conversion succeeded
papuga_ValueVariant* papuga_ValueVariant_tonumeric( const papuga_ValueVariant* self, papuga_ValueVariant* res, papuga_ErrorCode* err);

/// \brief Try to convert a variant value to an ASCII string in a buffer of restricted size
/// \param[out] destbuf buffer to use if a copy of the result string is needed to be made
/// \param[in] destbufsize allocation size of 'destbuf' in bytes
/// \param[in] self pointer to structure 
/// \return the pointer to the result string if succeeded, NULL else
const char* papuga_ValueVariant_toascii( char* destbuf, size_t destbufsize, const papuga_ValueVariant* self);

/// \brief Get the value of a type enum as string
/// \param[in] type the type enum value
/// \return the corresponding string
const char* papuga_Type_name( papuga_Type type);

#ifdef __cplusplus
}
#endif
#endif

