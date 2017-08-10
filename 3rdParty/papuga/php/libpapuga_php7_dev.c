/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library implementation for PHP (v7) bindings built by papuga
/// \file libpapuga_php7_dev.c

#include "papuga/lib/php7_dev.h"
#include "papuga/valueVariant.h"
#include "papuga/callResult.h"
#include "papuga/errors.h"
#include "papuga/serialization.h"
#include "papuga/hostObject.h"
#include "papuga/iterator.h"
#include "papuga/hostObject.h"
#include "private/dll_tags.h"
#include <stddef.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <inttypes.h>

// PHP & Zend includes:
typedef void siginfo_t;
#ifdef _MSC_VER
#include <zend_config.w32.h>
#else
#include <zend_config.nw.h>
#endif
#include <php.h>
#include <zend.h>
#include <zend_API.h>
#include <zend_interfaces.h>

static zend_class_entry* get_class_entry( const papuga_php_ClassEntryMap* cemap, unsigned int classid)
{
	--classid;
	return (classid > cemap->size) ? NULL : (zend_class_entry*)cemap->ar[ classid];
}

typedef struct ClassObject {
	papuga_HostObject hobj;
	int checksum;
	zend_object zobj;
} ClassObject;

#define KNUTH_HASH 2654435761U
static int calcObjectCheckSum( const ClassObject* cobj)
{
	return ((((cobj->hobj.classid+107) * KNUTH_HASH) ^ (uintptr_t)cobj->hobj.data) ^ (uintptr_t)cobj->hobj.destroy);
}

static ClassObject* getClassObject( zend_object* object)
{
	return (ClassObject*)((char *)object - XtOffsetOf( ClassObject, zobj));
}

typedef struct IteratorObject {
	papuga_Iterator iterator;
	zval resultval;
	int checksum;
	bool eof;
	long idx;
	zend_object zobj;
} IteratorObject;

static int calcIteratorCheckSum( const IteratorObject* iobj)
{
	return (((uintptr_t)iobj->iterator.data+107) * KNUTH_HASH) ^ (uintptr_t)iobj->iterator.destroy ^ ((uintptr_t)iobj->iterator.getNext << 6);
}

static IteratorObject* getIteratorObject( zend_object* object)
{
	return (IteratorObject*)((char *)object - XtOffsetOf( IteratorObject, zobj));
}

static zend_object_handlers g_papuga_object_ce_handlers;
static zend_object_handlers g_papuga_iterator_ce_handlers;
static zend_class_entry* g_zend_class_entry_iterator = 0;

DLL_PUBLIC papuga_zend_object* papuga_php_create_object(
	papuga_zend_class_entry* ce)
{
	ClassObject *cobj;

	cobj = (ClassObject*)ecalloc(1, sizeof(ClassObject) + zend_object_properties_size(ce));
	if (!cobj) return NULL;
	papuga_init_HostObject( &cobj->hobj, 0, NULL/*data*/, NULL/*destroy*/);
	cobj->checksum = calcObjectCheckSum( cobj);
	zend_object_std_init( &cobj->zobj, ce);
	object_properties_init( &cobj->zobj, ce);

	cobj->zobj.handlers = &g_papuga_object_ce_handlers;
	return &cobj->zobj;
}

DLL_PUBLIC bool papuga_php_init_object( void* selfzval, papuga_HostObject* hobj)
{
	zval* sptr = (zval*)selfzval;
	if (Z_TYPE_P(sptr) == IS_OBJECT)
	{
		zend_object* zobj = Z_OBJ_P( sptr);
		ClassObject *cobj = getClassObject( zobj);
		if (cobj->checksum != calcObjectCheckSum( cobj))
		{
			papuga_destroy_HostObject( hobj);
			return false;
		}
		papuga_init_HostObject( &cobj->hobj, hobj->classid, hobj->data, hobj->destroy);
		cobj->checksum = calcObjectCheckSum( cobj);
		return true;
	}
	else
	{
		papuga_destroy_HostObject( hobj);
		return false;
	}
}

static void destroy_papuga_object_zend_object( zend_object *object)
{
	ClassObject *cobj = getClassObject( object);
	if (cobj->checksum != calcObjectCheckSum( cobj))
	{
		fprintf( stderr, "bad free of papuga object in zend engine\n");
		return;
	}
	zend_objects_destroy_object( object);
}

static void free_papuga_object_zend_object( zend_object *object)
{
	ClassObject *cobj = getClassObject( object);
	if (cobj->checksum != calcObjectCheckSum( cobj))
	{
		fprintf( stderr, "bad free of papuga object in zend engine\n");
		return;
	}
	papuga_destroy_HostObject( &cobj->hobj);
	zend_object_std_dtor(object);
}

static void destroy_papuga_iterator_zend_object( zend_object *object)
{
	IteratorObject *iobj = getIteratorObject( object);
	if (iobj->checksum != calcIteratorCheckSum( iobj))
	{
		fprintf( stderr, "bad destroy of papuga iterator in zend engine\n");
		return;
	}
	zend_objects_destroy_object( object);
}

static void free_papuga_iterator_zend_object( zend_object *object)
{
	IteratorObject *iobj = getIteratorObject( object);
	if (iobj->checksum != calcIteratorCheckSum( iobj))
	{
		fprintf( stderr, "bad free of papuga iterator in zend engine\n");
		return;
	}
	papuga_destroy_Iterator( &iobj->iterator);
	zend_object_std_dtor(object);
}

static void initIteratorZendClassEntry();

DLL_PUBLIC void papuga_php_init()
{
	memcpy( &g_papuga_object_ce_handlers, zend_get_std_object_handlers(), sizeof(g_papuga_object_ce_handlers));
	g_papuga_object_ce_handlers.free_obj = &free_papuga_object_zend_object;
	g_papuga_object_ce_handlers.dtor_obj = &destroy_papuga_object_zend_object;
	g_papuga_object_ce_handlers.offset = XtOffsetOf( ClassObject, zobj);

	memcpy( &g_papuga_iterator_ce_handlers, zend_get_std_object_handlers(), sizeof(g_papuga_iterator_ce_handlers));
	g_papuga_iterator_ce_handlers.free_obj = &free_papuga_iterator_zend_object;
	g_papuga_iterator_ce_handlers.dtor_obj = &destroy_papuga_iterator_zend_object;
	g_papuga_iterator_ce_handlers.offset = XtOffsetOf( IteratorObject, zobj);

	initIteratorZendClassEntry();
}

static bool serializeValue( papuga_Serialization* ser, zval* langval, papuga_ErrorCode* errcode);

static bool serializeAssocArray( papuga_Serialization* ser, HashTable *hash, HashPosition* ptr, papuga_ErrorCode* errcode)
{
	bool rt = true;
	zval *data;
	zend_string *str_index;
	zend_ulong num_index;

	data = zend_hash_get_current_data_ex( hash, ptr);
	for( ;0!=(data = zend_hash_get_current_data_ex( hash, ptr));
		zend_hash_move_forward_ex( hash, ptr))
	{
		if (zend_hash_get_current_key_ex( hash, &str_index, &num_index, ptr) == HASH_KEY_IS_STRING)
		{
			rt &= papuga_Serialization_pushName_string( ser, ZSTR_VAL(str_index), ZSTR_LEN(str_index));
		}
		else
		{
			rt &= papuga_Serialization_pushName_int( ser, num_index);
		}
		rt &= serializeValue( ser, data, errcode);
	}
	return rt;
}

static bool serializeArray( papuga_Serialization* ser, zval* langval, papuga_ErrorCode* errcode)
{
	bool rt = papuga_Serialization_pushOpen( ser);
	size_t resultstart = ser->arsize;
	zval *data;
	HashTable *hash = Z_ARRVAL_P( langval);
	HashPosition ptr;
	zend_string *str_index;
	zend_ulong num_index;
	zend_ulong indexcount = 0;
	for(
		zend_hash_internal_pointer_reset_ex( hash, &ptr);
		0!=(data=zend_hash_get_current_data_ex(hash,&ptr));
		zend_hash_move_forward_ex(hash,&ptr))
	{
		if (zend_hash_get_current_key_ex( hash, &str_index, &num_index, &ptr) == HASH_KEY_IS_STRING)
		{
			if (indexcount && !papuga_Serialization_convert_array_assoc( ser, resultstart, 0/*start count*/, errcode))
			{
				return false;
			}
			rt &= papuga_Serialization_pushName_string( ser, ZSTR_VAL(str_index), ZSTR_LEN(str_index));
			rt &= serializeValue( ser, data, errcode);

			zend_hash_move_forward_ex( hash, &ptr);
			if (!serializeAssocArray( ser, hash, &ptr, errcode))
			{
				return false;
			}
		}
		else if (num_index == indexcount)
		{
			rt &= serializeValue( ser, data, errcode);
			++indexcount;
		}
		else
		{
			if (indexcount && !papuga_Serialization_convert_array_assoc( ser, resultstart, 0/*start count*/, errcode))
			{
				return false;
			}
			rt &= papuga_Serialization_pushName_int( ser, num_index);
			rt &= serializeValue( ser, data, errcode);

			zend_hash_move_forward_ex(hash,&ptr);
			if (!serializeAssocArray( ser, hash, &ptr, errcode))
			{
				return false;
			}
		}
	}
	rt &= papuga_Serialization_pushClose( ser);
	return rt;
}

static bool serializeObject( papuga_Serialization* ser, zval* langval, papuga_ErrorCode* errcode)
{
	zend_object* zobj = Z_OBJ_P( langval);
	ClassObject *cobj = getClassObject( zobj);
	if (cobj->checksum != calcObjectCheckSum( cobj))
	{
		*errcode = papuga_InvalidAccess;
		return false;
	}
	if (!papuga_Serialization_pushValue_hostobject( ser, &cobj->hobj))
	{
		*errcode = papuga_NoMemError;
		return false;
	}
	return true;
}

static bool serializeValue( papuga_Serialization* ser, zval* langval, papuga_ErrorCode* errcode)
{
	switch (Z_TYPE_P(langval))
	{
		case IS_UNDEF: *errcode = papuga_ValueUndefined; return false;
		case IS_FALSE: if (!papuga_Serialization_pushValue_bool( ser, false)) goto ERRNOMEM; return true;
		case IS_TRUE: if (!papuga_Serialization_pushValue_bool( ser, true)) goto ERRNOMEM; return true;
		case IS_LONG: if (!papuga_Serialization_pushValue_int( ser, Z_LVAL_P( langval))) goto ERRNOMEM; return true;
		case IS_CONSTANT:
		case IS_STRING: if (!papuga_Serialization_pushValue_string( ser, Z_STRVAL_P( langval), Z_STRLEN_P( langval))) goto ERRNOMEM; return true;
		case IS_DOUBLE: if (!papuga_Serialization_pushValue_double( ser, Z_DVAL_P( langval))) goto ERRNOMEM; return true;
		case IS_NULL: if (!papuga_Serialization_pushValue_void( ser)) goto ERRNOMEM; return true;
		case IS_ARRAY: if (!serializeArray( ser, langval, errcode)) goto ERRNOMEM; return true;
		case IS_OBJECT: if (!serializeObject( ser, langval, errcode)) goto ERRNOMEM; return true;
		case IS_RESOURCE:
		case IS_REFERENCE:
		default: *errcode = papuga_TypeError; return false;
	}
	return true;

ERRNOMEM:
	*errcode = papuga_NoMemError;
	return false;
}

static bool initArray( papuga_ValueVariant* hostval, papuga_Allocator* allocator, zval* langval, papuga_ErrorCode* errcode)
{
	papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( allocator);
	if (!ser)
	{
		*errcode = papuga_NoMemError;
		return false;
	}
	papuga_init_ValueVariant_serialization( hostval, ser);

	if (!serializeValue( ser, langval, errcode))
	{
		*errcode = papuga_NoMemError;
		return false;
	}
	return true;
}

static bool initObject( papuga_ValueVariant* hostval, zval* langval, papuga_ErrorCode* errcode)
{
	zend_object* zobj = Z_OBJ_P( langval);
	ClassObject *cobj = getClassObject( zobj);
	if (cobj->checksum != calcObjectCheckSum( cobj))
	{
		*errcode = papuga_InvalidAccess;
		return false;
	}
	papuga_init_ValueVariant_hostobj( hostval, &cobj->hobj);
	return true;
}

static bool initValue( papuga_ValueVariant* hostval, papuga_Allocator* allocator, zval* langval, papuga_ErrorCode* errcode)
{
	switch (Z_TYPE_P(langval))
	{
		case IS_UNDEF: *errcode = papuga_ValueUndefined; return false;
		case IS_FALSE: papuga_init_ValueVariant_bool( hostval, false); return true;
		case IS_TRUE: papuga_init_ValueVariant_bool( hostval, true); return true;
		case IS_LONG: papuga_init_ValueVariant_int( hostval, Z_LVAL_P( langval)); return true;
		case IS_CONSTANT:
		case IS_STRING: papuga_init_ValueVariant_string( hostval, Z_STRVAL_P( langval), Z_STRLEN_P( langval)); return true;
		case IS_DOUBLE: papuga_init_ValueVariant_double( hostval, Z_DVAL_P( langval)); return true;
		case IS_NULL: papuga_init_ValueVariant( hostval); return true;
		case IS_ARRAY: return initArray( hostval, allocator, langval, errcode);
		case IS_OBJECT: return initObject( hostval, langval, errcode);
		case IS_RESOURCE:
		case IS_REFERENCE:
		default: *errcode = papuga_TypeError; return false;
	}
}

static bool deserialize( zval* return_value, papuga_Allocator* allocator, const papuga_Serialization* serialization, const papuga_php_ClassEntryMap* cemap, papuga_ErrorBuffer* errbuf);

static bool iteratorToZval( zval* return_value, papuga_Iterator* iterator, papuga_ErrorBuffer* errbuf);

static bool valueVariantToZval( zval* return_value, papuga_Allocator* allocator, papuga_ValueVariant* value, const papuga_php_ClassEntryMap* cemap, const char* context, papuga_ErrorBuffer* errbuf)
{
	switch (value->valuetype)
	{
		case papuga_TypeVoid:
			RETVAL_NULL();
			break;
		case papuga_TypeDouble:
			RETVAL_DOUBLE( value->value.Double);
			break;
		case papuga_TypeUInt:
			RETVAL_LONG( value->value.UInt);
			break;
		case papuga_TypeInt:
			RETVAL_LONG( value->value.UInt);
			break;
		case papuga_TypeBool:
			if (value->value.Bool)
			{
				RETVAL_TRUE;
			}
			else
			{
				RETVAL_FALSE;
			}
			break;
		case papuga_TypeString:
			if (value->length)
			{
				RETVAL_STRINGL( value->value.string, value->length);
			}
			else
			{
				RETVAL_EMPTY_STRING();
			}
			break;
		case papuga_TypeLangString:
		{
			if (value->length)
			{
				papuga_ErrorCode errcode = papuga_Ok;
				size_t strsize;
				const char* str = papuga_ValueVariant_tostring( value, allocator, &strsize, &errcode);
				if (!str)
				{
					papuga_ErrorBuffer_reportError( errbuf, "failed to convert unicode string in %s: %s", context, papuga_ErrorCode_tostring( errcode));
					return false;
				}
				else
				{
					RETVAL_STRINGL( str, strsize);
				}
			}
			else
			{
				RETVAL_EMPTY_STRING();
			}
			break;
		}
		case papuga_TypeHostObject:
		{
			papuga_HostObject* hobj = value->value.hostObject;
			papuga_zend_object* zobj;
			zend_class_entry* ce;

			if (!cemap)
			{
				papuga_ErrorBuffer_reportError( errbuf, "cannot create host object in this context");
				return false;
			}
			ce = get_class_entry( cemap, hobj->classid);
			if (!ce)
			{
				papuga_ErrorBuffer_reportError( errbuf, "error in %s: %s", context, "undefined class id of object");
				return false;
			}
			zobj = papuga_php_create_object( ce);
			if (!zobj)
			{
				papuga_ErrorBuffer_reportError( errbuf, "error in %s: %s", context, "failed to create zend object from host object");
				return false;
			}
			object_init(return_value);
			Z_OBJ_P(return_value) = zobj;
			if (papuga_php_init_object( return_value, hobj))
			{
				papuga_release_HostObject(hobj);
				Z_SET_REFCOUNT_P(return_value,1);
			}
			else
			{
				papuga_ErrorBuffer_reportError( errbuf, "error in %s: %s", context, "failed to initialize zend object with host object");
				return false;
			}
			break;
		}
		case papuga_TypeSerialization:
		{
			array_init(return_value);
			if (!deserialize( return_value, allocator, value->value.serialization, cemap, errbuf))
			{
				return false;
			}
			break;
		}
		case papuga_TypeIterator:
		{
			papuga_Iterator* itr = value->value.iterator;
			if (iteratorToZval( return_value, itr, errbuf))
			{
				papuga_release_Iterator( itr);
			}
			else
			{
				return false;
			}
			break;
		}
		default:
			papuga_ErrorBuffer_reportError( errbuf, "unknown value type in %s", context);
	}
	return true;
}

static bool deserialize_value( zval* return_value, papuga_Allocator* allocator, papuga_ValueVariant* value, const papuga_php_ClassEntryMap* cemap, papuga_ErrorBuffer* errbuf)
{
	return valueVariantToZval( return_value, allocator, value, cemap, "deserialization of structure", errbuf);
}

static bool zval_structure_addnode( zval* structure, papuga_Allocator* allocator, const papuga_ValueVariant* name, zval* value, papuga_ErrorBuffer* errbuf)
{
	if (name)
	{
		if (papuga_ValueVariant_isnumeric( name))
		{
			papuga_ErrorCode errcode = papuga_Ok;
			uint64_t index = papuga_ValueVariant_touint( name, &errcode);
			if (!index && errcode != papuga_Ok)
			{
				papuga_ErrorBuffer_reportError( errbuf, "cannot build uint index for key in serialization: %s", papuga_ErrorCode_tostring( errcode));
				return false;
			}
			add_index_zval( structure, index, value);
		}
		else if (papuga_ValueVariant_isstring( name))
		{
			papuga_ErrorCode errcode = papuga_Ok;
			size_t propkeylen;
			const char* propkey = papuga_ValueVariant_tostring( name, allocator, &propkeylen, &errcode);
			if (!propkey)
			{
				papuga_ErrorBuffer_reportError( errbuf, "cannot build string key in serialization: %s", papuga_ErrorCode_tostring( errcode));
				return false;
			}
			add_assoc_zval_ex( structure, propkey, propkeylen, value);
		}
	}
	else
	{
		add_next_index_zval( structure, value);
	}
	return true;
}

static bool deserialize_nodes( zval* return_value, papuga_Allocator* allocator, papuga_Node** ni_p, const papuga_Node* ne, const papuga_php_ClassEntryMap* cemap, papuga_ErrorBuffer* errbuf)
{
	papuga_Node* ni = *ni_p;
	const papuga_ValueVariant* name = 0;
	for (; ni != ne && ni->tag != papuga_TagClose; ++ni)
	{
		if (ni->tag == papuga_TagName)
		{
			if (name)
			{
				papuga_ErrorBuffer_reportError( errbuf, "duplicate name tag in serialization");
				*ni_p = ni;
				return false;
			}
			name = &ni->value;
			if (!papuga_ValueVariant_isatomic( name))
			{
				papuga_ErrorBuffer_reportError( errbuf, "atomic value expected for key element in serialization");
				*ni_p = ni;
				return false;
			}
		}
		else if (ni->tag == papuga_TagOpen)
		{
			zval substructure;
			array_init( &substructure);
			++ni;
			if (!deserialize_nodes( &substructure, allocator, &ni, ne, cemap, errbuf))
			{
				zval_dtor( &substructure);
				*ni_p = ni;
				return false;
			}
			if (!zval_structure_addnode( return_value, allocator, name, &substructure, errbuf))
			{
				zval_dtor( &substructure);
				*ni_p = ni;
				return false;
			}
			name = NULL;
			if (ni == ne)
			{
				papuga_ErrorBuffer_reportError( errbuf, "structure deserialization failed: %s", papuga_ErrorCode_tostring( papuga_UnexpectedEof));
				*ni_p = ni;
				return false;
			}
			if (ni->tag != papuga_TagClose)
			{
				papuga_ErrorBuffer_reportError( errbuf, "close expected after structure in serialization");
				*ni_p = ni;
				return false;
			}
		}
		else if (ni->tag == papuga_TagValue)
		{
			zval item;
			if (!deserialize_value( &item, allocator, &ni->value, cemap, errbuf))
			{
				zval_dtor( &item);
				*ni_p = ni;
				return false;
			}
			if (!zval_structure_addnode( return_value, allocator, name, &item, errbuf))
			{
				zval_dtor( &item);
				*ni_p = ni;
				return false;
			}
			name = NULL;
		}
		else
		{
			papuga_ErrorBuffer_reportError( errbuf, "unknown tag in structure deserialization");
			*ni_p = ni;
			return false;
		}
	}
	*ni_p = ni;
	return true;
}

static bool deserialize( zval* return_value, papuga_Allocator* allocator, const papuga_Serialization* serialization, const papuga_php_ClassEntryMap* cemap, papuga_ErrorBuffer* errbuf)
{
#ifdef PAPUGA_LOWLEVEL_DEBUG
	char* str = papuga_Serialization_tostring( ser);
	if (ser)
	{
		fprintf( stderr, "DESERIALIZE STRUCT:\n%s\n", str);
	}
#endif
	papuga_Node* ni = serialization->ar;
	papuga_Node* ne = ni + serialization->arsize;
	bool rt = deserialize_nodes( return_value, allocator, &ni, ne, cemap, errbuf);
	if (rt && ni != ne)
	{
		papuga_ErrorBuffer_reportError( errbuf, "unexpected tokens at end of serialization");
		rt = false;
	}
	return rt;
}

DLL_PUBLIC bool papuga_php_init_CallArgs( void* selfzval, int argc, papuga_php_CallArgs* as)
{
	zval args[ papuga_PHP_MAX_NOF_ARGUMENTS];
	int argi = -1;
	as->erridx = -1;
	as->errcode = 0;
	as->self = 0;
	as->argc = 0;

	if (selfzval)
	{
		zval* sptr = (zval*)selfzval;
		if (Z_TYPE_P(sptr) == IS_OBJECT)
		{
			zend_object* zobj = Z_OBJ_P( sptr);
			ClassObject *cobj = getClassObject( zobj);

			if (cobj->checksum != calcObjectCheckSum( cobj))
			{
				as->errcode = papuga_InvalidAccess;
				return false;
			}
			as->self = cobj->hobj.data;
		}
		else
		{
			as->errcode = papuga_LogicError;
			return false;
		}
	}
	if (argc > papuga_PHP_MAX_NOF_ARGUMENTS)
	{
		as->errcode = papuga_NofArgsError;
		return false;
	}
	papuga_init_Allocator( &as->allocator, as->allocbuf, sizeof( as->allocbuf));
	if (zend_get_parameters_array_ex( argc, args) == FAILURE) goto ERROR;

	for (argi=0; argi < argc; ++argi)
	{
		if (!initValue( &as->argv[ as->argc], &as->allocator, &args[argi], &as->errcode))
		{
			goto ERROR;
		}
		++as->argc;
	}
	return true;
ERROR:
	as->erridx = argi;
	as->errcode = papuga_TypeError;
	papuga_php_destroy_CallArgs( as);
	return false;
}

DLL_PUBLIC void papuga_php_destroy_CallArgs( papuga_php_CallArgs* arg)
{
	papuga_destroy_Allocator( &arg->allocator);
}

DLL_PUBLIC bool papuga_php_move_CallResult( void* zval_return_value, papuga_CallResult* retval, const papuga_php_ClassEntryMap* cemap, papuga_ErrorBuffer* errbuf)
{
	bool rt = true;
	zval* return_value = (zval*)zval_return_value;
	if (retval->value.valuetype == papuga_TypeSerialization)
	{
		papuga_Node* ni = retval->value.value.serialization->ar;
		papuga_Node* ne = ni + retval->value.value.serialization->arsize;
	
		if (ni == ne)
		{
			RETVAL_FALSE;
		}
		// Complicated decision making because PHP does not support multiple return values:
		else if (ni->tag == papuga_TagValue)
		{
			if (ni + 1 == ne)
			{
				//... one value in serialization => one return value
				rt &= deserialize_value( return_value, &retval->allocator, &ni->value, cemap, errbuf);
			}
			else
			{
				//... more than one value in serialization => array with values as return value
				rt &= valueVariantToZval( return_value, &retval->allocator, &retval->value, cemap, "assign return values", errbuf);
			}
		}
		else if (ni->tag == papuga_TagOpen)
		{
			zval substructure;
			array_init( &substructure);
			++ni;
			if (!deserialize_nodes( &substructure, &retval->allocator, &ni, ne, cemap, errbuf))
			{
				zval_dtor( &substructure);
				rt = false;
			}
			else if (ni == ne)
			{
				papuga_ErrorBuffer_reportError( errbuf, "structure deserialization failed: %s", papuga_ErrorCode_tostring( papuga_UnexpectedEof));
				rt = false;
			}
			else if (ni->tag != papuga_TagClose)
			{
				papuga_ErrorBuffer_reportError( errbuf, "close expected after structure in serialization");
				rt = false;
			}
			else
			{
				++ni;
				if (ni == ne)
				{
					//... one structure in serialization => one return structure
					RETVAL_ZVAL( &substructure, 0, 1);
				}
				else
				{
					//... more than one structure in serialization => array with values as return value
					array_init(return_value);
					if (!zval_structure_addnode( return_value, &retval->allocator, 0, &substructure, errbuf))
					{
						zval_dtor( &substructure);
						rt = false;
					}
					else if (!deserialize_nodes( return_value, &retval->allocator, &ni, ne, cemap, errbuf))
					{
						rt = false;
					}
				}
			}
		}
		else
		{
			rt &= valueVariantToZval( return_value, &retval->allocator, &retval->value, cemap, "assign return value", errbuf);
		}
	}
	else
	{
		rt &= valueVariantToZval( return_value, &retval->allocator, &retval->value, cemap, "assign return value", errbuf);
	}
	papuga_destroy_CallResult( retval);
	return rt;
}

static bool iteratorFetchNext( IteratorObject* iobj, papuga_ErrorBuffer* errbuf)
{
	papuga_CallResult retstruct;
	char errstr[ 2048];
	const char* msg;

	papuga_init_CallResult( &retstruct, errstr, sizeof(errstr));
	if (iobj->eof) return false;

	if (iobj->iterator.getNext( iobj->iterator.data, &retstruct))
	{
		zval_dtor( &iobj->resultval);
		if (papuga_php_move_CallResult( &iobj->resultval, &retstruct, 0/*classentry map*/, errbuf))
		{
			iobj->eof = false;
			iobj->idx += 1;
			return true;
		}
		else
		{
			iobj->eof = true;
			return false;
		}
	}
	else
	{
		if (papuga_CallResult_hasError( &retstruct))
		{
			msg = papuga_CallResult_lastError( &retstruct);
			papuga_destroy_CallResult( &retstruct);
			papuga_ErrorBuffer_reportError( errbuf, "error calling method %s: %s", "PapugaIterator::next", msg?msg:"unknown error");
		}
		else
		{
			iobj->idx += 1;
		}
		iobj->eof = true;
		return false;
	}
}

static bool iteratorToZval( zval* return_value, papuga_Iterator* iterator, papuga_ErrorBuffer* errbuf)
{
	IteratorObject* iobj = (IteratorObject*)ecalloc(1, sizeof(IteratorObject) + zend_object_properties_size(g_zend_class_entry_iterator));
	if (!iobj)
	{
		papuga_ErrorBuffer_reportError( errbuf, "out of memory creating zval from iterator");
		papuga_destroy_Iterator( iterator);
		return false;
	}
	papuga_init_Iterator( &iobj->iterator, iterator->data, iterator->destroy, iterator->getNext);
	iobj->checksum = calcIteratorCheckSum( iobj);
	zend_object_std_init( &iobj->zobj, g_zend_class_entry_iterator);
	object_properties_init( &iobj->zobj, g_zend_class_entry_iterator);
	iobj->zobj.handlers = &g_papuga_iterator_ce_handlers;
	ZVAL_FALSE(&iobj->resultval);
	iobj->eof = false;
	if (!iteratorFetchNext( iobj, errbuf) && papuga_ErrorBuffer_hasError( errbuf))
	{
		return false;
	}
	object_init(return_value);
	Z_OBJ_P(return_value) = &iobj->zobj;
	Z_SET_REFCOUNT_P(return_value,1);
	return true;
}

#define PHP_ERROR(msg) {TSRMLS_FETCH();zend_error( E_ERROR, "%s", msg);return;}
#define PHP_FAIL(msg) {TSRMLS_FETCH();zend_error( E_ERROR, "%s", msg);RETVAL_FALSE;return;}


PHP_METHOD( PapugaIterator, current)
{
	/*[-]*/fprintf( stderr, "CALL PapugaIterator::current \n");
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	zend_object* zobj = Z_OBJ_P( getThis());
	IteratorObject *iobj = getIteratorObject( zobj);
	if (iobj->eof)
	{
		RETVAL_FALSE;
	}
	else
	{
		RETVAL_ZVAL( &iobj->resultval, 0, 1);
	}
}

PHP_METHOD( PapugaIterator, key)
{
	/*[-]*/fprintf( stderr, "CALL PapugaIterator::key \n");
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETVAL_NULL();
}

PHP_METHOD( PapugaIterator, next)
{
	/*[-]*/fprintf( stderr, "CALL PapugaIterator::next \n");
	papuga_ErrorBuffer errbuf;
	char errmsgbuf[ 2048];
	papuga_init_ErrorBuffer( &errbuf, errmsgbuf, sizeof(errmsgbuf));

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	zend_object* zobj = Z_OBJ_P( getThis());
	IteratorObject *iobj = getIteratorObject( zobj);
	if (!iteratorFetchNext( iobj, &errbuf))
	{
		if (papuga_ErrorBuffer_hasError( &errbuf))
		{
			PHP_FAIL( errbuf.ptr);
		}
	}
}

PHP_METHOD( PapugaIterator, rewind)
{
	PHP_FAIL( "calling non implemented method PapugaIterator::rewind");
}

PHP_METHOD( PapugaIterator, valid)
{
	/*[-]*/fprintf( stderr, "CALL PapugaIterator::rewind \n");
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	zend_object* zobj = Z_OBJ_P( getThis());
	IteratorObject *iobj = getIteratorObject( zobj);
	if (iobj->eof)
	{
		RETVAL_FALSE;
	}
	else
	{
		RETVAL_TRUE;
	}
}

static const zend_function_entry g_iterator_methods[] = {
	PHP_ME( PapugaIterator, current, NULL, ZEND_ACC_PUBLIC)
	PHP_ME( PapugaIterator, key, NULL, ZEND_ACC_PUBLIC)
	PHP_ME( PapugaIterator, next, NULL, ZEND_ACC_PUBLIC)
	PHP_ME( PapugaIterator, rewind, NULL, ZEND_ACC_PUBLIC)
	PHP_ME( PapugaIterator, valid, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* release all resources associated with this iterator instance */
static void zend_papuga_iterator_dtor( zend_object_iterator *iter)
{
	zval_dtor( &iter->data);
}

/* check for end of iteration (FAILURE or SUCCESS if data is valid) */
static int zend_papuga_iterator_valid(zend_object_iterator *iter)
{
	zend_object* zobj = Z_OBJ_P( &iter->data);
	IteratorObject *iobj = getIteratorObject( zobj);
	return (iobj->eof) ? FAILURE : SUCCESS;
}

/* fetch the item data for the current element */
static zval* zend_papuga_iterator_get_current_data(zend_object_iterator *iter)
{
	zend_object* zobj = Z_OBJ_P( &iter->data);
	IteratorObject *iobj = getIteratorObject( zobj);
	if (iobj->eof)
	{
		return NULL;
	}
	else
	{
		return &iobj->resultval;
	}
}

/* fetch the key for the current element (optional, may be NULL). The key
 * should be written into the provided zval* using the ZVAL_* macros. If
 * this handler is not provided auto-incrementing integer keys will be
 * used. */
static void zend_papuga_iterator_get_current_key( zend_object_iterator *iter, zval *key)
{
	zend_object* zobj = Z_OBJ_P( &iter->data);
	IteratorObject *iobj = getIteratorObject( zobj);
	ZVAL_LONG( key, iobj->idx);
}

/* step forwards to next element */
static void zend_papuga_iterator_move_forward( zend_object_iterator *iter)
{
	papuga_ErrorBuffer errbuf;
	char errmsgbuf[ 2048];
	papuga_init_ErrorBuffer( &errbuf, errmsgbuf, sizeof(errmsgbuf));
	
	zend_object* zobj = Z_OBJ_P( &iter->data);
	IteratorObject *iobj = getIteratorObject( zobj);
	if (!iteratorFetchNext( iobj, &errbuf))
	{
		if (papuga_ErrorBuffer_hasError( &errbuf))
		{
			PHP_ERROR( errbuf.ptr);
		}
	}
}

static zend_object_iterator_funcs g_iterator_funcs = {
	&zend_papuga_iterator_dtor,
	&zend_papuga_iterator_valid,
	&zend_papuga_iterator_get_current_data,
	&zend_papuga_iterator_get_current_key,
	&zend_papuga_iterator_move_forward,
	NULL/*rewind*/,
	NULL/*invalidate*/
};

static zend_object_iterator* zend_papuga_get_iterator( zend_class_entry *ce, zval *object, int by_ref)
{
	zend_object_iterator* rt;
	if (by_ref) {
		zend_error( E_ERROR, "iteration by reference not supported");
		return NULL;
	}
	rt = ecalloc(1, sizeof(zend_object_iterator));

	zend_iterator_init( rt);
	if (Z_TYPE_P(object) != IS_OBJECT)
	{
		zend_error( E_ERROR, "object expected as this get iterator argument");
		return NULL;
	}
	zend_object* zobj = Z_OBJ_P( object);
	IteratorObject *iobj = getIteratorObject( zobj);

	if (iobj->checksum != calcIteratorCheckSum( iobj))
	{
		zend_error( E_ERROR, "checksum mismatch for a get iterator argument");
		return NULL;
	}
	ZVAL_ZVAL( &rt->data, object, 1, 0);
	rt->funcs = &g_iterator_funcs;
	return rt;
}

static void initIteratorZendClassEntry()
{
	zend_class_entry tmp_ce;
	INIT_CLASS_ENTRY(tmp_ce, "PapugaIterator", g_iterator_methods);
	g_zend_class_entry_iterator = zend_register_internal_class( &tmp_ce TSRMLS_CC);
	g_zend_class_entry_iterator->get_iterator = &zend_papuga_get_iterator;
	g_zend_class_entry_iterator->iterator_funcs.funcs = &g_iterator_funcs;
	zend_class_implements( g_zend_class_entry_iterator TSRMLS_CC, 1, zend_ce_traversable);
}

