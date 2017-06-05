/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_STRING_BUFFER_H_INCLUDED
#define _PAPUGA_STRING_BUFFER_H_INCLUDED
/// \brief Allocator for memory blocks with ownership returned by papuga language binding functions
/// \file allocator.h
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Constructor of Allocator
/// \param[out] self pointer to structure initialized
#define papuga_init_Allocator(self)			{(self)->allocsize=0;(self)->arsize=0;(self)->ar=0;(self)->next=0;}

/// \brief Moving content of Allocator to another with ownership
/// \param[out] self pointer to structure initialized
/// \param[in,out] orig pointer to structure to move from
#define papuga_init_Allocator_move(self,orig)		{(self)->allocsize=(orig)->allocsize;(self)->arsize=(orig)->arsize;(self)->ar=(orig)->ar;(self)->next=(orig)->next; (orig)->allocsize=0;(orig)->arsize=0;(orig)->ar=0;(orig)->next=0;}

/// \brief Destructor of Allocator
/// \param[in] self pointer to structure destroyed
#define papuga_destroy_Allocator(self)			{if ((self)->ar != NULL||(self)->next != NULL) papuga_destroy_Allocator_( self);}

/// \brief Destructor of Allocator
/// \param[in] self pointer to structure 
void papuga_destroy_Allocator_( papuga_Allocator* self);

/// \brief Allocate a block of memory
/// \param[in,out] self pointer to structure 
/// \param[in] blocksize size of block to allocate
/// \param[in] alignment allocated block alingment in bytes
/// \remark currently no allignment bigger than the standard malloc alignment (sizeof non empty struct) is accepted
/// \return the pointer to the allocated block or NULL if alignment is invalid or malloc failed
void* papuga_Allocator_alloc( papuga_Allocator* self, size_t blocksize, unsigned int alignment);

/// \brief Allocate a string copy
/// \param[in,out] self pointer to structure 
/// \param[in] str pointer to string to copy
/// \param[in] len length of string to copy in bytes without 0 termination
/// \return the pointer to the string copy
char* papuga_Allocator_copy_string( papuga_Allocator* self, const char* str, size_t len);

/// \brief Allocate a string copy
/// \param[in,out] self pointer to structure 
/// \param[in] str pointer to 0-terminated string to copy
/// \return the pointer to the C-string to copy
char* papuga_Allocator_copy_charp( papuga_Allocator* self, const char* str);

#ifdef __cplusplus
}
#endif
#endif

