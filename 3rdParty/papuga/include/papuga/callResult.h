/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_CALL_RESULT_H_INCLUDED
#define _PAPUGA_CALL_RESULT_H_INCLUDED
/// \brief Representation of a result of a call to papuga language bindings
/// \file callResult.h
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

void papuga_init_CallResult( papuga_CallResult* self);
void papuga_init_CallResult_int( papuga_CallResult* self, papuga_IntType val);
void papuga_init_CallResult_uint( papuga_CallResult* self, papuga_UIntType val);
void papuga_init_CallResult_double( papuga_CallResult* self, double val);
void papuga_init_CallResult_bool( papuga_CallResult* self, bool val);
void papuga_init_CallResult_string( papuga_CallResult* self, const char* val, size_t valsize);
void papuga_init_CallResult_string_const( papuga_CallResult* self, const char* val, size_t valsize);
void papuga_init_CallResult_charp( papuga_CallResult* self, const char* val);
void papuga_init_CallResult_charp_const( papuga_CallResult* self, const char* val);
void papuga_init_CallResult_langstring_const( papuga_CallResult* self, papuga_StringEncoding enc, const void* val, size_t valsize);
void papuga_init_CallResult_hostobject( papuga_CallResult* self, int classid, void* data, papuga_HostObjectDeleter destroy);
void papuga_init_CallResult_langobject( papuga_CallResult* self, void* data, papuga_HostObjectDeleter destroy);
void papuga_init_CallResult_serialization( papuga_CallResult* self);
void papuga_init_CallResult_serialization_hostobject( papuga_CallResult* self, void* data, papuga_HostObjectDeleter destroy);

void papuga_destroy_CallResult( papuga_CallResult* self);

void papuga_CallResult_reportError( papuga_CallResult* self, const char* msg, ...);
#define papuga_CallResult_hasError( self)		((self)->errorbuf[0] != 0)
#define papuga_CallResult_lastError( self)		((self)->errorbuf)

#ifdef __cplusplus
}
#endif
#endif



