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
#include <string.h>
#include <stdlib.h>

void papuga_destroy_Allocator_( papuga_Allocator* self)
{
	if (self->ar != NULL)
	{
		free( self->ar);
		self->ar = 0;
	}
	papuga_Allocator* next = self->next;
	self->next = 0;
	while (next != NULL)
	{
		if (next->ar != NULL)
		{
			free( next->ar);
			next->ar = 0;
		}
		papuga_Allocator* next_next = next->next;
		free( next);
		next = next_next;
	}
}

static int isPowerOfTwo (unsigned int x)
{
	return ((x != 0) && ((x & (~x + 1)) == x));
}
struct MaxAlignStruct {int _;};
#define MAXALIGN	(sizeof(struct MaxAlignStruct))
#define STDBLOCKSIZE	2048
#define MAXBLOCKSIZE	(1<<31)

void* papuga_Allocator_alloc( papuga_Allocator* self, size_t blocksize, unsigned int alignment)
{
	void* rt;

	if (!isPowerOfTwo( alignment)
		|| alignment > MAXALIGN
		|| blocksize == 0
		|| blocksize >= MAXBLOCKSIZE) return 0;
	if (self->ar != NULL)
	{
		unsigned int alignmentofs = alignment - (self->arsize & (alignment-1));
		if (self->allocsize - self->arsize < blocksize + alignmentofs)
		{
			self->arsize += alignmentofs;
			rt = self->ar + self->arsize;
			self->arsize += blocksize;
			return rt;
		}
		papuga_Allocator* next = (papuga_Allocator*)calloc( 1, sizeof( papuga_Allocator));
		if (next == NULL) return NULL;
		next->ar = self->ar;
		next->next = self->next;
		self->next = next;
	}
	/* Allocate new block: */
	self->allocsize = STDBLOCKSIZE;
	while (self->allocsize < blocksize)
	{
		self->allocsize *= 2;
	}
	self->ar = (char*)malloc( self->allocsize);
	if (self->ar == NULL) return NULL;
	self->arsize = blocksize;
	return self->ar;
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

