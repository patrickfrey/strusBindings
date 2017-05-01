/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_STRINGBUFFER_H_INCLUDED
#define _PAPUGA_STRINGBUFFER_H_INCLUDED
/// \brief Buffer for strings returned by papuga language binding functions
/// \file stringBuffer.h
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define papuga_init_StringBuffer(self)		{(self)->ar=0;(self)->next=0;}
#define papuga_destroy_StringBuffer(self)	{if ((self)->ar != NULL||(self)->next != NULL) papuga_dealloc_StringBuffer( self);}

void papuga_dealloc_StringBuffer( papuga_StringBuffer* self);

char* papuga_StringBuffer_copy_string( papuga_StringBuffer* self, const char* str, size_t len);
char* papuga_StringBuffer_copy_charp( papuga_StringBuffer* self, const char* str);

char* papuga_StringBuffer_append_string( papuga_StringBuffer* self, const char* str, size_t len);

#ifdef __cplusplus
}
#endif
#endif

