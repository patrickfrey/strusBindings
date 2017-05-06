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

#define papuga_init_Serialization(self)		{(self)->ar=NULL;(self)->allocsize=0;(self)->arsize=0;}
#define papuga_destroy_Serialization(self)	{if ((self)->ar){free( (self)->ar);(self)->ar=NULL;}}
bool papuga_init_Serialization_copy( papuga_Serialization* self, const papuga_Serialization* o);

bool papuga_Serialization_pushOpen( papuga_Serialization* self);
bool papuga_Serialization_pushClose( papuga_Serialization* self);
bool papuga_Serialization_pushName( papuga_Serialization* self, const papuga_ValueVariant* value);
bool papuga_Serialization_pushValue( papuga_Serialization* self, const papuga_ValueVariant* value);

bool papuga_Serialization_pushName_void( papuga_Serialization* self);
bool papuga_Serialization_pushName_string( papuga_Serialization* self, const char* name, int namelen);
bool papuga_Serialization_pushName_charp( papuga_Serialization* self, const char* name);
bool papuga_Serialization_pushName_langstring( papuga_Serialization* self, papuga_StringEncoding enc, const char* name, int namelen);
bool papuga_Serialization_pushName_int( papuga_Serialization* self, int64_t name);
bool papuga_Serialization_pushName_uint( papuga_Serialization* self, uint64_t name);
bool papuga_Serialization_pushName_double( papuga_Serialization* self, double name);
bool papuga_Serialization_pushName_bool( papuga_Serialization* self, bool name);

bool papuga_Serialization_pushValue_void( papuga_Serialization* self);
bool papuga_Serialization_pushValue_string( papuga_Serialization* self, const char* value, int valuelen);
bool papuga_Serialization_pushValue_charp( papuga_Serialization* self, const char* value);
bool papuga_Serialization_pushValue_langstring( papuga_Serialization* self, papuga_StringEncoding enc, const char* value, int valuelen);
bool papuga_Serialization_pushValue_int( papuga_Serialization* self, int64_t value);
bool papuga_Serialization_pushValue_uint( papuga_Serialization* self, uint64_t value);
bool papuga_Serialization_pushValue_double( papuga_Serialization* self, double value);
bool papuga_Serialization_pushValue_bool( papuga_Serialization* self, bool value);

bool papuga_Serialization_append( papuga_Serialization* self, const papuga_Serialization* o);
bool papuga_Serialization_islabeled( const papuga_Serialization* self);

#ifdef __cplusplus
}
#endif
#endif


