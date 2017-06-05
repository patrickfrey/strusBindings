/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_STRING_BUFFER_H_INCLUDED
#define _PAPUGA_STRING_BUFFER_H_INCLUDED
/// \brief Buffer for strings returned by papuga language binding functions
/// \file stringBuffer.h
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Constructor of StringBuffer
/// \param[out] self pointer to structure initialized
#define papuga_init_StringBuffer(self)			{(self)->allocsize=0;(self)->arsize=0;(self)->ar=0;(self)->next=0;}

/// \brief Moving content of StringBuffer to another with ownership
/// \param[out] self pointer to structure initialized
/// \param[in,out] orig pointer to structure to move from
#define papuga_init_StringBuffer_move(self,orig)	{(self)->allocsize=(orig)->allocsize;(self)->arsize=(orig)->arsize;(self)->ar=(orig)->ar;(self)->next=(orig)->next; (orig)->allocsize=0;(orig)->arsize=0;(orig)->ar=0;(orig)->next=0;}

/// \brief Destructor of StringBuffer
/// \param[in] self pointer to structure destroyed
#define papuga_destroy_StringBuffer(self)		{if ((self)->ar != NULL||(self)->next != NULL) papuga_destroy_StringBuffer_( self);}

/// \brief Destructor of StringBuffer
/// \param[in] self pointer to structure 
void papuga_destroy_StringBuffer_( papuga_StringBuffer* self);

/// \brief Add string copy to string buffer
/// \param[in,out] self pointer to structure 
/// \param[in] str pointer to string to copy
/// \param[in] len length of string to copy in bytes
/// \return the pointer to the result string
char* papuga_StringBuffer_copy_string( papuga_StringBuffer* self, const char* str, size_t len);

/// \brief Add a copy of a C-string to string buffer
/// \param[in,out] self pointer to structure 
/// \param[in] str pointer to null-terminated string to copy
/// \return the pointer to the result string
char* papuga_StringBuffer_copy_charp( papuga_StringBuffer* self, const char* str);

/// \brief Append a string to the current top string of the string buffer
/// \param[in,out] self pointer to structure 
/// \param[in] str pointer to string to append
/// \param[in] len length of string to append in bytes
/// \return the pointer to the result string
char* papuga_StringBuffer_append_string( papuga_StringBuffer* self, const char* str, size_t len);

#ifdef __cplusplus
}
#endif
#endif

