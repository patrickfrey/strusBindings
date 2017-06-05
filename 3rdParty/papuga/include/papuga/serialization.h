/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_SERIALIZATION_H_INCLUDED
#define _PAPUGA_SERIALIZATION_H_INCLUDED
/// \brief Serialization of structures for papuga language bindings
/// \file serialization.h
#include "papuga/typedefs.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Constructor
/// \param[out] self pointer to structure 
#define papuga_init_Serialization(self)			{(self)->ar=NULL;(self)->allocsize=0;(self)->arsize=0;}

/// \brief Destructor
/// \param[in] self pointer to structure 
#define papuga_destroy_Serialization(self)		{if ((self)->ar){free( (self)->ar);(self)->ar=NULL;}}

/// \brief Constructor of an iterator reference as copy of another taking ownership
/// \param[out] self pointer to structure initialized by constructor
/// \param[in,out] orig pointer to iterator to take data from
#define papuga_init_Serialization_move( self, orig)	{(self)->ar=(orig)->ar; (self)->arsize=(orig)->arsize; (self)->allocsize=(orig)->allocsize; (orig)->ar=0;(orig)->arsize=0;(orig)->allocsize=0;}

/// \brief Test if serialization is empty
#define papuga_Serialization_empty(self)		!!(self)->ar

/// \brief Copy constructor
/// \param[out] self pointer to structure 
bool papuga_init_Serialization_copy( papuga_Serialization* self, const papuga_Serialization* o);

/// \brief Add an 'open' element to the serialization
/// \param[in,out] self pointer to structure 
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushOpen( papuga_Serialization* self);

/// \brief Add a 'close' element to the serialization
/// \param[in,out] self pointer to structure 
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushClose( papuga_Serialization* self);

/// \brief Add a 'name' element to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] value pointer to value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName( papuga_Serialization* self, const papuga_ValueVariant* value);

/// \brief Add a 'value' element to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] value pointer to value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue( papuga_Serialization* self, const papuga_ValueVariant* value);

/// \brief Add a 'name' element with a NULL value to the serialization
/// \param[in,out] self pointer to structure 
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName_void( papuga_Serialization* self);

/// \brief Add a 'name' element as an UTF-8 string with length to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] name pointer to name of the added node
/// \param[in] namelen length of the name of the added node in bytes
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName_string( papuga_Serialization* self, const char* name, int namelen);

/// \brief Add a 'name' element as an UTF-8 string to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] name pointer to name of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName_charp( papuga_Serialization* self, const char* name);

/// \brief Add a 'name' element as a unicode string to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] enc character set encoding of the name of the added node
/// \param[in] name pointer to name of the added node
/// \param[in] namelen length of the name of the added node in bytes
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName_langstring( papuga_Serialization* self, papuga_StringEncoding enc, const char* name, int namelen);

/// \brief Add a 'name' element as a signed integer to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] name numeric name value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName_int( papuga_Serialization* self, int64_t name);

/// \brief Add a 'name' element as an unsigned signed integer to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] name numeric name value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName_uint( papuga_Serialization* self, uint64_t name);

/// \brief Add a 'name' element as a double precision floating point value to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] name numeric name value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName_double( papuga_Serialization* self, double name);

/// \brief Add a 'name' element as a boolean value to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] name boolean name value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushName_bool( papuga_Serialization* self, bool name);

/// \brief Add a 'value' element with a NULL value to the serialization
/// \param[in,out] self pointer to structure 
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue_void( papuga_Serialization* self);

/// \brief Add a 'value' element as an UTF-8 string with length to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] value pointer to value of the added node
/// \param[in] valuelen length of the value of the added node in bytes
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue_string( papuga_Serialization* self, const char* value, int valuelen);

/// \brief Add a 'value' element as an UTF-8 string to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] value pointer to value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue_charp( papuga_Serialization* self, const char* value);

/// \brief Add a 'value' element as a unicode string to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] enc character set encoding of the value of the added node
/// \param[in] value pointer to value of the added node
/// \param[in] valuelen length of the value of the added node in bytes
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue_langstring( papuga_Serialization* self, papuga_StringEncoding enc, const char* value, int valuelen);

/// \brief Add a 'value' element as a signed integer to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] value numeric value value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue_int( papuga_Serialization* self, int64_t value);

/// \brief Add a 'value' element as an unsigned signed integer to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] value numeric value value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue_uint( papuga_Serialization* self, uint64_t value);

/// \brief Add a 'value' element as a double precision floating point value to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] value numeric value value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue_double( papuga_Serialization* self, double value);

/// \brief Add a 'value' element as a boolean value to the serialization
/// \param[in,out] self pointer to structure 
/// \param[in] value boolean value value of the added node
/// \return true on success, false on memory allocation error
bool papuga_Serialization_pushValue_bool( papuga_Serialization* self, bool value);

/// \brief Append a serialization to another one
/// \param[in,out] self pointer to structure 
/// \param[in] o serialization to append
/// \return true on success, false on memory allocation error
bool papuga_Serialization_append( papuga_Serialization* self, const papuga_Serialization* o);

/// \brief Test of the serialization is a labeled structure or a positional one
/// \param[in] self pointer to structure
/// \return true, if the serialization starts as labeled structure, false if it starts as positional structure
/// \remark This method does not check if the serialization is consistently labeled or positional, if just looks for the first evidence for the one or the other, assuming that the rest of the serialization follows the pattern too.
bool papuga_Serialization_islabeled( const papuga_Serialization* self);

#ifdef __cplusplus
}
#endif
#endif


