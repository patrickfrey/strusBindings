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
#include "papuga/hostObjectReference.h"
#include "papuga/stringBuffer.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static void init_CallResult_structures( papuga_CallResult* self)
{
	papuga_init_HostObjectReference( &self->object, 0, 0);
	papuga_init_Serialization( &self->serialization);
	papuga_init_StringBuffer( &self->valuebuf);
	self->errorbuf[0] = 0;
}

void papuga_init_CallResult( papuga_CallResult* self)
{
	papuga_init_ValueVariant( &self->value);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_int( papuga_CallResult* self, papuga_IntType val)
{
	papuga_init_ValueVariant_int( &self->value, val);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_uint( papuga_CallResult* self, papuga_UIntType val)
{
	papuga_init_ValueVariant_uint( &self->value, val);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_double( papuga_CallResult* self, double val)
{
	papuga_init_ValueVariant_double( &self->value, val);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_bool( papuga_CallResult* self, bool val)
{
	papuga_init_ValueVariant_bool( &self->value, val);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_string( papuga_CallResult* self, const char* val, size_t valsize)
{
	char* val_copy = papuga_StringBuffer_copy_string( &self->valuebuf, val, valsize);
	papuga_init_ValueVariant_string( &self->value, val_copy, valsize);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_string_const( papuga_CallResult* self, const char* val, size_t valsize)
{
	papuga_init_ValueVariant_string( &self->value, val, valsize);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_charp( papuga_CallResult* self, const char* val)
{
	char* val_copy = papuga_StringBuffer_copy_charp( &self->valuebuf, val);
	papuga_init_ValueVariant_charp( &self->value, val_copy);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_charp_const( papuga_CallResult* self, const char* val)
{
	papuga_init_ValueVariant_charp( &self->value, val);
	init_CallResult_structures( self);
}

void papuga_init_CallResult_langstring_const( papuga_CallResult* self, papuga_StringEncoding enc, const void* val, size_t valsize)
{
	papuga_init_ValueVariant_langstring( &self->value, enc, val, valsize);
	init_CallResult_structures( self);
}


void papuga_destroy_CallResult( papuga_CallResult* self)
{
	papuga_destroy_HostObjectReference( &self->object);
	papuga_destroy_Serialization( &self->serialization);
	papuga_destroy_StringBuffer( &self->valuebuf);
}

void papuga_init_CallResult_serialization( papuga_CallResult* self)
{
	papuga_init_ValueVariant_serialization( &self->value, &self->serialization);
}

void papuga_init_CallResult_serialization_hostobject( papuga_CallResult* self, void* data, papuga_HostObjectDeleter destroy)
{
	papuga_init_HostObjectReference( &self->object, data, destroy);
	papuga_init_ValueVariant_serialization( &self->value, &self->serialization);
}

void papuga_init_CallResult_hostobject( papuga_CallResult* self, int classid, void* data, papuga_HostObjectDeleter destroy)
{
	papuga_init_HostObjectReference( &self->object, data, destroy);
	papuga_init_ValueVariant_hostobj( &self->value, data, classid);
}

void papuga_init_CallResult_langobject( papuga_CallResult* self, void* data, papuga_HostObjectDeleter destroy)
{
	papuga_init_HostObjectReference( &self->object, data, destroy);
	papuga_init_ValueVariant_hostobj( &self->value, data, 0);
}

void papuga_CallResult_reportError( papuga_CallResult* self, const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vsnprintf( self->errorbuf, sizeof(self->errorbuf), msg, ap);
	va_end(ap);
}




