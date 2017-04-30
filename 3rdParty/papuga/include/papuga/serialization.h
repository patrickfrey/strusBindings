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
#include "papuga/valueVariant.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum papuga_Tag
{
	papuga_TagOpen,		///< Open a scope
	papuga_TagClose,	///< Closes a scope
	papuga_TagName,		///< The name of the following value (Value) or structure (Open)
	papuga_TagValue		///< Atomic value
} papuga_Tag;

typedef struct papuga_Node
{
	papuga_SerializationTag tag;
	papuga_ValueVariant value;
} papuga_Node;

#define papuga_NodeOpen()	{tag:papuga_TagOpen, value:papuga_ValueVariant_()}
#define papuga_NodeClose()	{tag:papuga_TagClose, value:papuga_ValueVariant_()}
#define papuga_NodeName(val)	{tag:papuga_TagName, value:val}
#define papuga_NodeValue(val)	{tag:papuga_TagValue, value:val}

typedef struct papuga_Serialization
{
	unsigned int arsize;
	papuga_Node* ar;
} papuga_Serialization;

bool papuga_Serialization_pushOpen( papuga_Serialization* self);
bool papuga_Serialization_pushClose( papuga_Serialization* self);
bool papuga_Serialization_pushName( papuga_Serialization* self, const papuga_ValueVariant* value);
bool papuga_Serialization_pushValue( papuga_Serialization* self, const papuga_ValueVariant* value);

bool papuga_Serialization_pushName_string( papuga_Serialization* self, const char* name, int namelen);
bool papuga_Serialization_pushName_charp( papuga_Serialization* self, const char* name);
bool papuga_Serialization_pushName_langstring( papuga_Serialization* self, papuga_StringEncoding enc, const char* name, int namelen);
bool papuga_Serialization_pushName_int( papuga_Serialization* self, int64_t value);
bool papuga_Serialization_pushName_uint( papuga_Serialization* self, uint64_t value);
bool papuga_Serialization_pushName_double( papuga_Serialization* self, double value);

bool papuga_Serialization_pushValue_string( papuga_Serialization* self, const char* value, int valuelen);
bool papuga_Serialization_pushValue_charp( papuga_Serialization* self, const char* value);
bool papuga_Serialization_pushValue_langstring( papuga_Serialization* self, papuga_StringEncoding enc, const char* value, int valuelen);
bool papuga_Serialization_pushValue_int( papuga_Serialization* self, int64_t value);
bool papuga_Serialization_pushValue_uint( papuga_Serialization* self, uint64_t value);
bool papuga_Serialization_pushValue_double( papuga_Serialization* self, double value);

bool papuga_Serialization_append( papuga_Serialization* self, const papuga_Serialization* o);
bool papuga_Serialization_islabeled( const papuga_Serialization* self);

	static bool isLabeled( const_iterator si, const const_iterator& se)
	{
		for (; si != se; ++si)
		{
			if (si->tag == Name) return true;
			if (si->tag == Value) return false;
		}
		return false;
	}
	bool isLabeled() const
	{
		return isLabeled( m_ar.begin(), m_ar.end());
	}


#ifdef __cplusplus
}
#endif
#endif


