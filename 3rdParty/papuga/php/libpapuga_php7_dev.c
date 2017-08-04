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
static int calcCheckSum( const ClassObject* cobj)
{
	return ((((cobj->hobj.classid+107) * KNUTH_HASH) ^ (uintptr_t)cobj->hobj.data) ^ (uintptr_t)cobj->hobj.destroy);
}

static ClassObject* getClassObject( zend_object* object)
{
	return (ClassObject*)((char *)object - XtOffsetOf( ClassObject, zobj));
}

static zend_object_handlers g_papuga_ce_handlers;

papuga_zend_object* papuga_php_create_object(
	papuga_zend_class_entry* ce)
{
	ClassObject *cobj;

	cobj = (ClassObject*)ecalloc(1, sizeof(ClassObject) + zend_object_properties_size(ce));
	if (!cobj) return NULL;
	papuga_init_HostObject( &cobj->hobj, 0, NULL/*data*/, NULL/*destroy*/);
	cobj->checksum = calcCheckSum( cobj);
	zend_object_std_init( &cobj->zobj, ce);
	object_properties_init( &cobj->zobj, ce);

	cobj->zobj.handlers = &g_papuga_ce_handlers;
	return &cobj->zobj;
}

bool papuga_php_init_object( void* selfzval, papuga_HostObject* hobj)
{
	zval* sptr = (zval*)selfzval;
	if (Z_TYPE_P(sptr) == IS_OBJECT)
	{
		zend_object* zobj = Z_OBJ_P( sptr);
		ClassObject *cobj = getClassObject( zobj);
		if (cobj->checksum != calcCheckSum( cobj))
		{
			papuga_destroy_HostObject( hobj);
			return false;
		}
		papuga_init_HostObject( &cobj->hobj, hobj->classid, hobj->data, hobj->destroy);
		cobj->checksum = calcCheckSum( cobj);
		return true;
	}
	else
	{
		papuga_destroy_HostObject( hobj);
		return false;
	}
}

static void destroy_papuga_zend_object( zend_object *object)
{
	zend_objects_destroy_object( object);
}

static void free_papuga_zend_object( zend_object *object)
{
	ClassObject *cobj = getClassObject( object);
	if (cobj->checksum != calcCheckSum( cobj))
	{
		fprintf( stderr, "bad free of papuga object in zend engine\n");
	}
	papuga_destroy_HostObject( &cobj->hobj);
	zend_object_std_dtor(object);
}

void papuga_php_init()
{
	memcpy( &g_papuga_ce_handlers, zend_get_std_object_handlers(), sizeof(g_papuga_ce_handlers));

	g_papuga_ce_handlers.free_obj = &free_papuga_zend_object;
	g_papuga_ce_handlers.dtor_obj = destroy_papuga_zend_object;
	g_papuga_ce_handlers.offset = XtOffsetOf( ClassObject, zobj);
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
	if (cobj->checksum != calcCheckSum( cobj))
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
	if (cobj->checksum != calcCheckSum( cobj))
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

static bool deserialize( zval* return_value, papuga_Allocator* allocator, const papuga_Serialization* serialization, const papuga_php_ClassEntryMap* cemap, papuga_ErrorCode* errcode);

static bool valueVariantToZval( zval* return_value, papuga_Allocator* allocator, papuga_ValueVariant* value, const papuga_php_ClassEntryMap* cemap, const char* context)
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
					papuga_php_error( "failed to convert unicode string in %s: %s", context, papuga_ErrorCode_tostring( errcode));
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
			zend_class_entry* ce = get_class_entry( cemap, hobj->classid);
			if (!ce)
			{
				papuga_php_error( "error in %s: %s", context, "undefined class id of object");
				return false;
			}
			papuga_zend_object* zobj = papuga_php_create_object( ce);
			if (!zobj)
			{
				papuga_php_error( "error in %s: %s", context, "failed to create zend object from host object");
				return false;
			}
			object_init(return_value);
			Z_OBJ_P(return_value) = zobj;
			if (papuga_php_init_object( return_value, hobj))
			{
				papuga_release_HostObject(hobj);
			}
			else
			{
				papuga_php_error( "error in %s: %s", context, "failed to initialize zend object with host object");
				return false;
			}
			break;
		}
		case papuga_TypeSerialization:
		{
			papuga_ErrorCode errcode = papuga_Ok;
			array_init(return_value);
			if (!deserialize( return_value, allocator, value->value.serialization, cemap, &errcode))
			{
				papuga_php_error( "error in %s: %s", context, papuga_ErrorCode_tostring( errcode));
				return false;
			}
			break;
		}
		case papuga_TypeIterator:
		{
			//papuga_Iterator* itr = value->value.iterator;
			papuga_php_error( "iterator not implemented for PHP");
			break;
		}
		default:
			papuga_php_error( "unknown value type in %s", context);
	}
	return true;
}

static bool deserialize_value( zval* return_value, papuga_Allocator* allocator, papuga_ValueVariant* value, const papuga_php_ClassEntryMap* cemap)
{
	return valueVariantToZval( return_value, allocator, value, cemap, "deserialization of structure");
}

static bool zval_structure_addnode( zval* structure, papuga_Allocator* allocator, const papuga_ValueVariant* name, zval* value)
{
	if (name)
	{
		if (papuga_ValueVariant_isnumeric( name))
		{
			papuga_ErrorCode errcode = papuga_Ok;
			uint64_t index = papuga_ValueVariant_touint( name, &errcode);
			if (!index && errcode != papuga_Ok)
			{
				papuga_php_error( "cannot build uint index for key in serialization: %s", papuga_ErrorCode_tostring( errcode));
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
				papuga_php_error( "cannot build string key in serialization: %s", papuga_ErrorCode_tostring( errcode));
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

static bool deserialize_nodes( zval* return_value, papuga_Allocator* allocator, papuga_Node** ni_p, const papuga_Node* ne, const papuga_php_ClassEntryMap* cemap)
{
	papuga_Node* ni = *ni_p;
	const papuga_ValueVariant* name = 0;
	for (; ni != ne && ni->tag != papuga_TagClose; ++ni)
	{
		if (ni->tag == papuga_TagName)
		{
			if (name)
			{
				zend_error( E_ERROR, "duplicate name tag in serialization");
				*ni_p = ni;
				return false;
			}
			name = &ni->value;
			if (!papuga_ValueVariant_isatomic( name))
			{
				zend_error( E_ERROR, "atomic value expected for key element in serialization");
				*ni_p = ni;
				return false;
			}
		}
		else if (ni->tag == papuga_TagOpen)
		{
			zval substructure;
			array_init( &substructure);
			++ni;
			if (!deserialize_nodes( &substructure, allocator, &ni, ne, cemap))
			{
				zval_dtor( &substructure);
				*ni_p = ni;
				return false;
			}
			if (!zval_structure_addnode( return_value, allocator, name, &substructure))
			{
				zval_dtor( &substructure);
				*ni_p = ni;
				return false;
			}
			name = NULL;
			if (ni == ne) papuga_php_error( "structure deserialization failed: %s", papuga_ErrorCode_tostring( papuga_UnexpectedEof));
			if (ni->tag != papuga_TagClose) zend_error( E_ERROR, "close expected after structure in serialization");
			++ni;
		}
		else if (ni->tag == papuga_TagValue)
		{
			zval item;
			if (!deserialize_value( &item, allocator, &ni->value, cemap))
			{
				zval_dtor( &item);
				*ni_p = ni;
				return false;
			}
			if (!zval_structure_addnode( return_value, allocator, name, &item))
			{
				zval_dtor( &item);
				*ni_p = ni;
				return false;
			}
			name = NULL;
		}
		else
		{
			zend_error( E_ERROR, "unknown tag in structure deserialization");
		}
	}
	*ni_p = ni;
	return true;
}

static bool deserialize( zval* return_value, papuga_Allocator* allocator, const papuga_Serialization* serialization, const papuga_php_ClassEntryMap* cemap, papuga_ErrorCode* errcode)
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
	bool rt = deserialize_nodes( return_value, allocator, &ni, ne, cemap);
	if (rt && ni != ne)
	{
		zend_error( E_ERROR, "unexpected tokens at end of serialization");
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

			if (cobj->checksum != calcCheckSum( cobj))
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

DLL_PUBLIC void papuga_php_move_CallResult( void* zval_return_value, papuga_CallResult* retval, const papuga_php_ClassEntryMap* cemap)
{
	zval* return_value = (zval*)zval_return_value;
	if (papuga_CallResult_hasError( retval))
	{
		zend_error( E_ERROR, retval->errorbuf.ptr);
	}
	(void)valueVariantToZval( return_value, &retval->allocator, &retval->value, cemap, "assign return value");
	papuga_destroy_CallResult( retval);
}

DLL_PUBLIC void papuga_php_error( const char* fmt, ...)
{
	va_list al;
	va_start( al, fmt);
	char buf[ 2048];
	if (vsnprintf( buf, sizeof(buf), fmt, al) >= sizeof(buf))
	{
		buf[ sizeof(buf)-1] = 0;
	}
	zend_error( E_ERROR, buf);
	va_end( al);
}



