/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Serialization of structures for papuga language bindings
/// \file serialization.c
#include "papuga/serialization.h"
#include "papuga/valueVariant.h"
#include <stdlib.h>
#include <string.h>

static bool alloc_nodes( papuga_Serialization* self, size_t addsize)
{
	size_t newsize = self->arsize + addsize;
	if (newsize < self->arsize) return false;
	if (newsize > self->allocsize)
	{
		size_t mm = self->allocsize ? (self->allocsize * 2) : 256;
		while (mm > self->allocsize && mm < newsize) mm *= 2;
		size_t newallocsize = mm;
		mm *= sizeof(papuga_Node);
		if (mm < newsize) return false;
		papuga_Node* newmem = (papuga_Node*)realloc( self->ar, mm);
		if (newmem == NULL) return false;
		self->ar = newmem;
		self->allocsize = newallocsize;
	}
	return true;
}

static inline bool add_node( papuga_Serialization* self, const papuga_Node* nd)
{
	if (!alloc_nodes( self, 1)) return false;
	memcpy( self->ar + self->arsize, nd, sizeof(papuga_Node));
	self->arsize += 1;
	return true;
}

static inline bool add_node_copy( papuga_Serialization* self, size_t nodei)
{
	if (!alloc_nodes( self, 1)) return false;
	memcpy( self->ar + self->arsize, self->ar + nodei, sizeof(papuga_Node));
	self->arsize += 1;
	return true;
}

static inline bool add_nodes( papuga_Serialization* self, const papuga_Node* ar, size_t arsize)
{
	if (!alloc_nodes( self, arsize)) return false;
	memcpy( self->ar + self->arsize, ar, arsize * sizeof(papuga_Node));
	self->arsize += arsize;
	return true;
}

#define PUSH_NODE_0(self,TAG,CONV)\
	papuga_Node nd;\
	nd.tag = TAG;\
	CONV( &nd.value);\
	return add_node( self, &nd);

#define PUSH_NODE_1(self,TAG,CONV,p1)\
	papuga_Node nd;\
	nd.tag = TAG;\
	CONV( &nd.value, p1);\
	return add_node( self, &nd);

#define PUSH_NODE_2(self,TAG,CONV,p1,p2)\
	papuga_Node nd;\
	nd.tag = TAG;\
	CONV( &nd.value, p1, p2);\
	return add_node( self, &nd);

#define PUSH_NODE_3(self,TAG,CONV,p1,p2,p3)\
	papuga_Node nd;\
	nd.tag = TAG;\
	CONV( &nd.value, p1, p2, p3);\
	return add_node( self, &nd);


bool papuga_Serialization_push( papuga_Serialization* self, papuga_Node* node)
{
	return add_node( self, node);
}

bool papuga_Serialization_pushOpen( papuga_Serialization* self)
{
	papuga_Node nd;
	nd.tag = papuga_TagOpen;
	papuga_init_ValueVariant( &nd.value);
	return add_node( self, &nd);
}

bool papuga_Serialization_pushClose( papuga_Serialization* self)
{
	papuga_Node nd;
	nd.tag = papuga_TagClose;
	papuga_init_ValueVariant( &nd.value);
	return add_node( self, &nd);
}

bool papuga_Serialization_pushName_void( papuga_Serialization* self)
	{PUSH_NODE_0(self,papuga_TagName,papuga_init_ValueVariant)}

bool papuga_Serialization_pushName( papuga_Serialization* self, const papuga_ValueVariant* name)
	{PUSH_NODE_1(self,papuga_TagName,papuga_init_ValueVariant_copy,name)}

bool papuga_Serialization_pushName_string( papuga_Serialization* self, const char* name, int namelen)
	{PUSH_NODE_2(self,papuga_TagName,papuga_init_ValueVariant_string,name,namelen)}

bool papuga_Serialization_pushName_charp( papuga_Serialization* self, const char* name)
	{PUSH_NODE_1(self,papuga_TagName,papuga_init_ValueVariant_charp,name)}

bool papuga_Serialization_pushName_langstring( papuga_Serialization* self, papuga_StringEncoding enc, const char* name, int namelen)
	{PUSH_NODE_3(self,papuga_TagName,papuga_init_ValueVariant_langstring,enc,name,namelen)}

bool papuga_Serialization_pushName_int( papuga_Serialization* self, int64_t name)
	{PUSH_NODE_1(self,papuga_TagName,papuga_init_ValueVariant_int,name)}

bool papuga_Serialization_pushName_uint( papuga_Serialization* self, uint64_t name)
	{PUSH_NODE_1(self,papuga_TagName,papuga_init_ValueVariant_uint,name)}

bool papuga_Serialization_pushName_double( papuga_Serialization* self, double name)
	{PUSH_NODE_1(self,papuga_TagName,papuga_init_ValueVariant_double,name)}

bool papuga_Serialization_pushName_bool( papuga_Serialization* self, bool name)
	{PUSH_NODE_1(self,papuga_TagName,papuga_init_ValueVariant_bool,name)}


bool papuga_Serialization_pushValue_void( papuga_Serialization* self)
	{PUSH_NODE_0(self,papuga_TagValue,papuga_init_ValueVariant)}

bool papuga_Serialization_pushValue( papuga_Serialization* self, const papuga_ValueVariant* value)
	{PUSH_NODE_1(self,papuga_TagValue,papuga_init_ValueVariant_copy,value)}

bool papuga_Serialization_pushValue_string( papuga_Serialization* self, const char* value, int valuelen)
	{PUSH_NODE_2(self,papuga_TagValue,papuga_init_ValueVariant_string,value,valuelen)}

bool papuga_Serialization_pushValue_charp( papuga_Serialization* self, const char* value)
	{PUSH_NODE_1(self,papuga_TagValue,papuga_init_ValueVariant_charp,value)}

bool papuga_Serialization_pushValue_langstring( papuga_Serialization* self, papuga_StringEncoding enc, const char* value, int valuelen)
	{PUSH_NODE_3(self,papuga_TagValue,papuga_init_ValueVariant_langstring,enc,value,valuelen)}

bool papuga_Serialization_pushValue_int( papuga_Serialization* self, int64_t value)
	{PUSH_NODE_1(self,papuga_TagValue,papuga_init_ValueVariant_int,value)}

bool papuga_Serialization_pushValue_uint( papuga_Serialization* self, uint64_t value)
	{PUSH_NODE_1(self,papuga_TagValue,papuga_init_ValueVariant_uint,value)}

bool papuga_Serialization_pushValue_double( papuga_Serialization* self, double value)
	{PUSH_NODE_1(self,papuga_TagValue,papuga_init_ValueVariant_double,value)}

bool papuga_Serialization_pushValue_bool( papuga_Serialization* self, bool value)
	{PUSH_NODE_1(self,papuga_TagValue,papuga_init_ValueVariant_bool,value)}
bool papuga_Serialization_pushValue_hostobject( papuga_Serialization* self, papuga_HostObject* value)
	{PUSH_NODE_1(self,papuga_TagValue,papuga_init_ValueVariant_hostobj,value)}


bool papuga_Serialization_append( papuga_Serialization* self, const papuga_Serialization* o)
{
	return add_nodes( self, o->ar, o->arsize);
}

bool papuga_Serialization_convert_array_assoc( papuga_Serialization* self, size_t arraystart, unsigned int countfrom, papuga_ErrorCode* errcode)
{
	bool rt = true;
	int bcnt = 0;
	size_t ae = self->arsize;
	size_t ai = arraystart;
	while (ai != ae)
	{
		switch (self->ar[ ai].tag)
		{
			case papuga_TagOpen:
				rt &= papuga_Serialization_pushName_uint( self, countfrom++);
				rt &= papuga_Serialization_pushOpen( self);
				for (bcnt=1,++ai; ai != ae && bcnt; ++ai)
				{
					if (!add_node_copy( self, ai))
					{
						self->arsize = ae;
						return false;
					}
					if (self->ar[ ai].tag == papuga_TagClose)
					{
						--bcnt;
					}
					else if (self->ar[ ai].tag == papuga_TagOpen)
					{
						++bcnt;
					}
				}
				if (bcnt) goto CONVERR;
				break;
			case papuga_TagClose:
			case papuga_TagName:
				goto CONVERR;
			case papuga_TagValue:
				rt &= papuga_Serialization_pushName_uint( self, countfrom++);
				if (!add_node_copy( self, ai))
				{
					self->arsize = ae;
					return false;
				}
				break;
		}
	}
	if (!rt)
	{
		self->arsize = ae;
		return false;
	}
	size_t nn = self->arsize - ae; /*... number of new elements added */
	memmove( self->ar + arraystart, self->ar + ae, nn);
	self->arsize = arraystart + nn;
	return true;
CONVERR:
	self->arsize = ae;
	if (rt) *errcode = papuga_TypeError;
	return false;
}

bool papuga_init_Serialization_copy( papuga_Serialization* self, const papuga_Serialization* o)
{
	papuga_init_Serialization( self);
	return add_nodes( self, o->ar, o->arsize);
}


