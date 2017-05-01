/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Buffer for strings returned by papuga language binding functions
/// \file stringBuffer.h
#include "papuga/stringBuffer.h"
#include <string.h>
#include <stdlib.h>

void papuga_dealloc_StringBuffer( papuga_StringBuffer* self)
{
	if (self->ar != NULL)
	{
		free( self->ar);
		self->ar = 0;
	}
	papuga_StringBuffer* next = self->next;
	self->next = 0;
	while (next != NULL)
	{
		if (next->ar != NULL)
		{
			free( next->ar);
			next->ar = 0;
		}
		papuga_StringBuffer* next_next = next->next;
		free( next);
		next = next_next;
	}
}

char* papuga_StringBuffer_copy_string( papuga_StringBuffer* self, const char* str, size_t len)
{
	if (self->ar != NULL)
	{
		papuga_StringBuffer* next = (papuga_StringBuffer*)calloc( 1, sizeof( papuga_StringBuffer));
		if (next == NULL) return NULL;
		next->ar = self->ar;
		next->next = self->next;
		self->next = next;
	}
	self->ar = (char*)malloc( len+1);
	if (self->ar == NULL) return NULL;
	self->arsize = len;
	self->allocsize = len;
	memcpy( self->ar, str, len);
	self->ar[ len] = 0;
	return self->ar;
}

char* papuga_StringBuffer_copy_charp( papuga_StringBuffer* self, const char* str)
{
	return papuga_StringBuffer_copy_string( self, str, strlen(str));
}

static bool expand_string( papuga_StringBuffer* self, size_t addsize)
{
	size_t newsize = self->arsize + addsize;
	if (newsize < self->arsize) return false;
	if (newsize > self->allocsize)
	{
		size_t mm = self->allocsize ? (self->allocsize * 2) : 256;
		while (mm >= self->allocsize && mm < newsize) mm *= 2;
		if (mm < newsize) return false;
		char* newar = (char*)realloc( self->ar, mm);
		if (newar == NULL) return false;
		self->ar = newar;
		self->allocsize = mm-1;
	}
	return true;
}

static inline bool append_string( papuga_StringBuffer* self, const char* str, size_t len)
{
	if (!expand_string( self, len)) return false;
	memcpy( self->ar + self->arsize, str, len);
	self->arsize += len;
	self->ar[ self->arsize] = 0;
	return true;
}

char* papuga_StringBuffer_append_string( papuga_StringBuffer* self, const char* str, size_t len)
{
	if (!append_string( self, str, len))
	{
		return 0;
	}
	else
	{
		return self->ar;
	}
}

