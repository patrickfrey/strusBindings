/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Allocator for memory blocks with ownership returned by papuga language binding functions
/// \file allocator.h
#include "papuga/allocator.h"
#include "papuga/serialization.h"
#include "papuga/hostObject.h"
#include "papuga/iterator.h"
#include <string.h>
#include <stdlib.h>

typedef struct papuga_ReferenceHostObject
{
	papuga_ReferenceHeader header;
	papuga_HostObject hostObject;
} papuga_ReferenceHostObject;

typedef struct papuga_ReferenceSerialization
{
	papuga_ReferenceHeader header;
	papuga_Serialization serialization;
} papuga_ReferenceSerialization;

typedef struct papuga_ReferenceIterator
{
	papuga_ReferenceHeader header;
	papuga_Iterator iterator;
} papuga_ReferenceIterator;

void papuga_destroy_ReferenceHeader( papuga_ReferenceHeader* hdritr)
{
	while (hdritr != NULL)
	{
		switch (hdritr->type)
		{
			case papuga_RefTypeHostObject:
			{
				papuga_ReferenceHostObject* obj = (papuga_ReferenceHostObject*)hdritr;
				papuga_destroy_HostObject( &obj->hostObject);
				break;
			}
			case papuga_RefTypeSerialization:
			{
				papuga_ReferenceSerialization* obj = (papuga_ReferenceSerialization*)hdritr;
				papuga_destroy_Serialization( &obj->serialization);
				break;
			}
			case papuga_RefTypeIterator:
			{
				papuga_ReferenceIterator* obj = (papuga_ReferenceIterator*)hdritr;
				papuga_destroy_Iterator( &obj->iterator);
				break;
			}
			default:
			{
				return;
			}
		}
		hdritr = hdritr->next;
	}
}

void papuga_destroy_AllocatorNode( papuga_AllocatorNode* self)
{
	if (self->ar != NULL && self->allocated)
	{
		free( self->ar);
		self->ar = 0;
	}
	papuga_AllocatorNode* itr = self->next;
	self->next = 0;
	while (itr != NULL)
	{
		if (itr->ar != NULL && itr->allocated)
		{
			free( itr->ar);
			itr->ar = 0;
		}
		papuga_AllocatorNode* next = itr->next;
		free( itr);
		itr = next;
	}
}

static int isPowerOfTwo (unsigned int x)
{
	return (((x & (~x + 1)) == x));
}
struct MaxAlignStruct {int _;};
#define MAXALIGN	(sizeof(struct MaxAlignStruct))
#define STDBLOCKSIZE	2048
#define MAXBLOCKSIZE	(1<<31)

void* papuga_Allocator_alloc( papuga_Allocator* self, size_t blocksize, unsigned int alignment)
{
	void* rt;

	if (alignment == 0) alignment = MAXALIGN;
	if (!isPowerOfTwo( alignment)
		|| alignment > MAXALIGN
		|| blocksize == 0
		|| blocksize >= MAXBLOCKSIZE) return 0;
	if (self->root.ar != NULL)
	{
		unsigned int alignmentofs = (alignment - (self->root.arsize & (alignment-1))) & (MAXALIGN-1);
		if (self->root.allocsize - self->root.arsize < blocksize + alignmentofs)
		{
			self->root.arsize += alignmentofs;
			rt = self->root.ar + self->root.arsize;
			self->root.arsize += blocksize;
			return rt;
		}
		papuga_AllocatorNode* next = (papuga_AllocatorNode*)calloc( 1, sizeof( papuga_AllocatorNode));
		if (next == NULL) return 0;
		memcpy( next, &self->root, sizeof(self->root));
		memset( &self->root, 0, sizeof(self->root));
		self->root.next = next;
	}
	/* Allocate new block: */
	self->root.allocsize = STDBLOCKSIZE;
	while (self->root.allocsize < blocksize)
	{
		self->root.allocsize *= 2;
	}
	self->root.ar = (char*)malloc( self->root.allocsize);
	if (self->root.ar == NULL) return NULL;
	self->root.allocated = true;
	self->root.arsize = blocksize;
	return self->root.ar;
}

char* papuga_Allocator_copy_string( papuga_Allocator* self, const char* str, size_t len)
{
	char* rt = (char*)papuga_Allocator_alloc( self, len+1, 1);
	if (rt)
	{
		memcpy( rt, str, len);
		rt[ len] = 0;
	}
	return rt;
}

char* papuga_Allocator_copy_charp( papuga_Allocator* self, const char* str)
{
	return papuga_Allocator_copy_string( self, str, strlen(str));
}

papuga_HostObject* papuga_Allocator_alloc_HostObject( papuga_Allocator* self, int classid_, void* object_, papuga_Deleter destroy_)
{
	papuga_ReferenceHostObject* rt = (papuga_ReferenceHostObject*)papuga_Allocator_alloc( self, sizeof( papuga_ReferenceHostObject), 0);
	if (!rt) return 0;
	rt->header.type = papuga_RefTypeHostObject;
	rt->header.next = self->reflist;
	self->reflist = &rt->header;
	papuga_init_HostObject( &rt->hostObject, classid_, object_, destroy_);
	return &rt->hostObject;
}

papuga_Serialization* papuga_Allocator_alloc_Serialization( papuga_Allocator* self)
{
	papuga_ReferenceSerialization* rt = (papuga_ReferenceSerialization*)papuga_Allocator_alloc( self, sizeof( papuga_ReferenceSerialization), 0);
	if (!rt) return 0;
	rt->header.type = papuga_RefTypeSerialization;
	rt->header.next = self->reflist;
	self->reflist = &rt->header;
	papuga_init_Serialization( &rt->serialization);
	return &rt->serialization;
}

papuga_Iterator* papuga_Allocator_alloc_Iterator( papuga_Allocator* self, void* object_, papuga_Deleter destroy_, papuga_GetNext getNext_)
{
	papuga_ReferenceIterator* rt = (papuga_ReferenceIterator*)papuga_Allocator_alloc( self, sizeof( papuga_ReferenceIterator), 0);
	if (!rt) return 0;
	rt->header.type = papuga_RefTypeIterator;
	rt->header.next = self->reflist;
	self->reflist = &rt->header;
	papuga_init_Iterator( &rt->iterator, object_, destroy_, getNext_);
	return &rt->iterator;
}

