/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Representation of a result of a call to papuga language bindings
/// \file callResult.c
#include "papuga/callResult.h"
#include "papuga/valueVariant.h"
#include "papuga/serialization.h"
#include "papuga/hostObject.h"
#include "papuga/iterator.h"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static void init_CallResult_structures( papuga_CallResult* self)
{
	papuga_init_ValueVariant( &self->value);
	papuga_init_Allocator( &self->allocator, self->allocbuf, sizeof(self->allocbuf));
}

void papuga_destroy_CallResult( papuga_CallResult* self)
{
	papuga_destroy_Allocator( &self->allocator);
}

void papuga_init_CallResult( papuga_CallResult* self, char* errorbuf, size_t errorbufsize)
{
	init_CallResult_structures( self);
	papuga_init_ErrorBuffer( &self->errorbuf, errorbuf, errorbufsize);
}

void papuga_set_CallResult_int( papuga_CallResult* self, papuga_Int val)
{
	papuga_init_ValueVariant_int( &self->value, val);
}

void papuga_set_CallResult_uint( papuga_CallResult* self, papuga_UInt val)
{
	papuga_init_ValueVariant_uint( &self->value, val);
}

void papuga_set_CallResult_double( papuga_CallResult* self, double val)
{
	papuga_init_ValueVariant_double( &self->value, val);
}

void papuga_set_CallResult_bool( papuga_CallResult* self, bool val)
{
	papuga_init_ValueVariant_bool( &self->value, val);
}

bool papuga_set_CallResult_string( papuga_CallResult* self, const char* val, size_t valsize)
{
	char* val_copy = papuga_Allocator_copy_string( &self->allocator, val, valsize);
	if (!val_copy) return false;
	papuga_init_ValueVariant_string( &self->value, val_copy, valsize);
	return true;
}

void papuga_set_CallResult_string_const( papuga_CallResult* self, const char* val, size_t valsize)
{
	papuga_init_ValueVariant_string( &self->value, val, valsize);
}

void papuga_set_CallResult_charp( papuga_CallResult* self, const char* val)
{
	if (val)
	{
		char* val_copy = papuga_Allocator_copy_charp( &self->allocator, val);
		papuga_init_ValueVariant_charp( &self->value, val_copy);
	}
	else
	{
		papuga_init_ValueVariant( &self->value);
	}
}

void papuga_set_CallResult_charp_const( papuga_CallResult* self, const char* val)
{
	if (val)
	{
		papuga_init_ValueVariant_charp( &self->value, val);
	}
	else
	{
		papuga_init_ValueVariant( &self->value);
	}
}

void papuga_set_CallResult_langstring_const( papuga_CallResult* self, papuga_StringEncoding enc, const void* val, size_t valsize)
{
	papuga_init_ValueVariant_langstring( &self->value, enc, val, valsize);
}

bool papuga_set_CallResult_serialization( papuga_CallResult* self)
{
	papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( &self->allocator);
	if (!ser) return false;
	papuga_init_ValueVariant_serialization( &self->value, ser);
	return true;
}

bool papuga_set_CallResult_hostobject( papuga_CallResult* self, int classid, void* data, papuga_Deleter destroy)
{
	papuga_HostObject* obj = papuga_Allocator_alloc_HostObject( &self->allocator, classid, data, destroy);
	if (!obj) return false;
	papuga_init_ValueVariant_hostobj( &self->value, obj);
	return true;
}

bool papuga_set_CallResult_iterator( papuga_CallResult* self, void* data, papuga_Deleter destroy, papuga_GetNext getNext)
{
	papuga_Iterator* itr = papuga_Allocator_alloc_Iterator( &self->allocator, data, destroy, getNext);
	if (!itr) return false;
	papuga_init_ValueVariant_iterator( &self->value, itr);
	return true;
}

void papuga_CallResult_reportError( papuga_CallResult* self, const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	size_t nn = vsnprintf( self->errorbuf.ptr, self->errorbuf.size, msg, ap);
	if (nn >= self->errorbuf.size-1)
	{
		self->errorbuf.ptr[ self->errorbuf.size-1] = 0;
	}
	va_end(ap);
}




