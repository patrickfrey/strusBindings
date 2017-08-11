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

/// \brief Constructor of AllocatorNode
/// \param[out] self pointer to structure initialized
/// \param[in] buf pointer to preallocated (local) buffer
/// \param[in] bufsize allocation size of buf in bytes
#define papuga_init_AllocatorNode(self,buf,bufsize)	{papuga_Allocator* s = self; s->allocsize=bufsize;s->arsize=0;s->allocated=!bufsize;s->ar=(char*)buf;s->next=0;}

/// \brief Constructor of Allocator
/// \param[out] self pointer to structure initialized
/// \param[in] buf pointer to preallocated (local) buffer
/// \param[in] bufsize allocation size of buf in bytes
#define papuga_init_Allocator(self,buf,bufsize)		{papuga_Allocator* s = self; s->root.allocsize=bufsize;s->root.arsize=0;s->root.allocated=!buf;s->root.ar=(char*)buf;s->root.next=0;s->reflist=0;}

/// \brief Destructor of Allocator
/// \param[in] self pointer to structure destroyed
#define papuga_destroy_Allocator(self)			{papuga_Allocator* s = self; if (s->reflist != NULL) papuga_destroy_ReferenceHeader( s->reflist); if ((s->root.ar != NULL && s->root.allocated) || s->root.next != NULL) papuga_destroy_AllocatorNode( &s->root);}

/// \brief Destructor of linked list of AllocatorNode
/// \param[in] nd pointer to node
void papuga_destroy_AllocatorNode( papuga_AllocatorNode* nd);

/// \brief Destructor of linked list of AllocatorNode
/// \param[in] nd pointer to node
void papuga_destroy_ReferenceHeader( papuga_ReferenceHeader* ref);

/// \brief Allocate a block of memory
/// \param[in,out] self pointer to structure 
/// \param[in] blocksize size of block to allocate
/// \param[in] alignment allocated block alingment in bytes or 0, if the default alignment (sizeof non empty struct) should be used
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
/// \param[in,out] self pointer to allocator structure 
/// \param[in] str pointer to 0-terminated string to copy
/// \return the pointer to the C-string to copy
char* papuga_Allocator_copy_charp( papuga_Allocator* self, const char* str);

/// \brief Allocate a host object reference
/// \param[out] self pointer to allocator structure
/// \param[in] object_ pointer to host object
/// \param[in] destroy_ destructor of the host object in case of ownership
/// \return the pointer to the allocated host object reference
papuga_HostObject* papuga_Allocator_alloc_HostObject( papuga_Allocator* self, int classid_, void* object_, papuga_Deleter destroy_);

/// \brief Allocate a serialization object
/// \param[out] self pointer to structure initialized by constructor
/// \return the pointer to the allocated serialization object
papuga_Serialization* papuga_Allocator_alloc_Serialization( papuga_Allocator* self);

/// \brief Allocate an iterator
/// \param[out] self pointer to allocator structure
/// \param[in] object_ pointer to iterator context object
/// \param[in] destroy_ destructor of the iterated object in case of ownership
/// \param[in] getNext_ method of the iterated object to fetch the next element
/// \return the pointer to the allocated iterator object
papuga_Iterator* papuga_Allocator_alloc_Iterator( papuga_Allocator* self, void* object_, papuga_Deleter destroy_, papuga_GetNext getNext_);

/// \brief Allocate an iterator
/// \param[out] self pointer to allocator structure
/// \return the pointer to the allocated allocator object
papuga_Allocator* papuga_Allocator_alloc_Allocator( papuga_Allocator* self);

#ifdef __cplusplus
}
#endif
#endif

