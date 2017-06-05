#include <lua.h>
#include <lauxlib.h>
#include "papuga/lib/lua_dev.h"
#include "papuga/valueVariant.h"
#include "papuga/callResult.h"
#include "papuga/errors.h"
#include "papuga/serialization.h"
#include "papuga/hostObject.h"
#include "private/dll_tags.h"
#include <stddef.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

#define MAX_DOUBLE_INT            ((int64_t)1<<53)
#define MIN_DOUBLE_INT           -((int64_t)1<<53)
#define IS_CONVERTIBLE_TOINT( x)  ((x-floor(x) <= 2*DBL_EPSILON) && x < MAX_DOUBLE_INT && x > MIN_DOUBLE_INT)
#define NUM_EPSILON               (2*DBL_EPSILON)

#undef PAPUGA_LOWLEVEL_DEBUG
#ifdef PAPUGA_LOWLEVEL_DEBUG
void STACKTRACE( lua_State* ls, const char* where)
{
	int ii;
	int top = lua_gettop( ls);

	fprintf( stderr, "CALLING %s STACK %d: ", where, top);

	for (ii = 1; ii <= top; ii++)
	{
		if (ii>1) fprintf( stderr, ", ");
		int t = lua_type( ls, ii);
		switch (t) {
			case LUA_TNIL:
				fprintf( stderr, "NIL");
				break;
			case LUA_TSTRING:
			{
				char strbuf[ 64];
				size_t len = snprintf( strbuf, sizeof(strbuf), "'%s'", lua_tostring( ls, ii));
				if (len >= sizeof(strbuf))
				{
					memcpy( strbuf + sizeof(strbuf) - 5, "...'", 4);
					strbuf[ sizeof(strbuf)-1] = 0;
				}
				fprintf( stderr, "'%s'", strbuf);
				break;
			}
			case LUA_TBOOLEAN:
				fprintf( stderr, "%s",lua_toboolean( ls, ii) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				fprintf( stderr, "%f", lua_tonumber( ls, ii));
				break;
			default:
				fprintf( stderr, "<%s>", lua_typename( ls, t));
				break;
		}
	}
	fprintf( stderr, "\n");
}
#else
#define STACKTRACE( ls, where)
#endif


static const papuga_lua_ClassDef* get_classdef( const papuga_lua_ClassDefMap* classdefmap, unsigned int classid)
{
	--classid;
	return (classid > classdefmap->size) ? NULL : &classdefmap->ar[ classid];
}

static bool Serialization_pushName_number( papuga_Serialization* result, double numval)
{
	if (IS_CONVERTIBLE_TOINT( numval))
	{
		if (numval > 0.0)
		{
			return papuga_Serialization_pushName_uint( result, (uint64_t)(numval + NUM_EPSILON));
		}
		else
		{
			return papuga_Serialization_pushName_int( result, (int64_t)(numval - NUM_EPSILON));
		}
	}
	else
	{
		return papuga_Serialization_pushName_double( result, numval);
	}
}

static bool Serialization_pushValue_number( papuga_Serialization* result, double numval)
{
	if (IS_CONVERTIBLE_TOINT( numval))
	{
		if (numval > 0.0)
		{
			return papuga_Serialization_pushValue_uint( result, (uint64_t)(numval + NUM_EPSILON));
		}
		else
		{
			return papuga_Serialization_pushValue_int( result, (int64_t)(numval - NUM_EPSILON));
		}
	}
	else
	{
		return papuga_Serialization_pushValue_double( result, numval);
	}
}

static void init_ValueVariant_number( papuga_ValueVariant* result, double numval)
{
	if (IS_CONVERTIBLE_TOINT( numval))
	{
		if (numval > 0.0)
		{
			papuga_init_ValueVariant_uint( result, (uint64_t)(numval + NUM_EPSILON));
		}
		else
		{
			papuga_init_ValueVariant_int( result, (int64_t)(numval - NUM_EPSILON));
		}
	}
	else
	{
		papuga_init_ValueVariant_double( result, numval);
	}
}


static bool serialize_key( papuga_lua_CallArgs* as, papuga_Serialization* result, lua_State* ls, int li)
{
	bool rt = true;
	switch (lua_type (ls, li))
	{
		case LUA_TNIL:
			rt &= papuga_Serialization_pushName_void( result);
			break;
		case LUA_TNUMBER:
			rt &= Serialization_pushName_number( result, lua_tonumber( ls, li));
			break;
		case LUA_TBOOLEAN:
			rt &= papuga_Serialization_pushName_bool( result, lua_toboolean( ls, li));
			break;
		case LUA_TSTRING:
		{
			size_t strsize;
			const char* str = lua_tolstring( ls, li, &strsize);
			rt &= papuga_Serialization_pushName_string( result, str, strsize);
			break;
		}
		case LUA_TTABLE:	as->errcode = papuga_TypeError; return false;
		case LUA_TFUNCTION:	as->errcode = papuga_TypeError; return false;
		case LUA_TUSERDATA:	as->errcode = papuga_TypeError; return false;
		case LUA_TTHREAD:	as->errcode = papuga_TypeError; return false;
		case LUA_TLIGHTUSERDATA:as->errcode = papuga_TypeError; return false;
		default:		as->errcode = papuga_TypeError; return false;
	}
	if (!rt)
	{
		as->errcode = papuga_NoMemError;
		return false;
	}
	return true;
}

static bool serialize_node( papuga_lua_CallArgs* as, papuga_Serialization* result, lua_State *ls, int li);

static bool serialize_value( papuga_lua_CallArgs* as, papuga_Serialization* result, lua_State* ls, int li)
{
	bool rt = true;
	const char* str;
	size_t strsize;

	switch (lua_type (ls, li))
	{
		case LUA_TNIL:
			rt &= papuga_Serialization_pushValue_void( result);
			break;
		case LUA_TNUMBER:
			rt &= Serialization_pushValue_number( result, lua_tonumber( ls, li));
			break;
		case LUA_TBOOLEAN:
			rt &= papuga_Serialization_pushValue_bool( result, lua_toboolean( ls, li));
			break;
		case LUA_TSTRING:
			str = lua_tolstring( ls, li, &strsize);
			rt &= papuga_Serialization_pushValue_string( result, str, strsize);
			break;
		case LUA_TTABLE:
			rt &= papuga_Serialization_pushOpen( result);
			rt &= serialize_node( as, result, ls, li);
			rt &= papuga_Serialization_pushClose( result);
			break;
		case LUA_TUSERDATA:	as->errcode = papuga_TypeError; return false;
		case LUA_TFUNCTION:	as->errcode = papuga_TypeError; return false;
		case LUA_TTHREAD:	as->errcode = papuga_TypeError; return false;
		case LUA_TLIGHTUSERDATA:as->errcode = papuga_TypeError; return false;
		default:		as->errcode = papuga_TypeError; return false;
	}
	if (!rt)
	{
		as->errcode = papuga_NoMemError;
		return false;
	}
	return true;
}

static bool try_serialize_array( papuga_lua_CallArgs* as, papuga_Serialization* result, lua_State* ls, int li)
{
	size_t start_size = result->arsize;
	int idx = 0;
	STACKTRACE( ls, "loop before serialize array");
	lua_pushvalue( ls, li);
	lua_pushnil( ls);
	while (lua_next( ls, -2))
	{
		STACKTRACE( ls, "loop next serialize array");
		if (!lua_isinteger( ls, -2) || lua_tointeger( ls, -2) != ++idx) goto ERROR;
		serialize_value( as, result, ls, -1);
		lua_pop(ls, 1);
	}
	lua_pop( ls, 1);
	STACKTRACE( ls, "loop after serialize array");
	return true;
ERROR:
	lua_pop( ls, 3);
	result->arsize = start_size;
	STACKTRACE( ls, "loop after try serialize array");
	return false;
}

static bool serialize_node( papuga_lua_CallArgs* as, papuga_Serialization* result, lua_State *ls, int li)
{
	size_t start_size = result->arsize;
	if (!lua_checkstack( ls, 8))
	{
		as->errcode = papuga_NoMemError;
		return false;
	}
	STACKTRACE( ls, "loop before serialize table");
	if (try_serialize_array( as, result, ls, li)) return true;

	lua_pushvalue( ls, li);
	lua_pushnil( ls);
	while (lua_next( ls, -2))
	{
		STACKTRACE( ls, "loop next serialize table");
		if (!serialize_key( as, result, ls, -2)) goto ERROR;
		if (!serialize_value( as, result, ls, -1)) goto ERROR;
		lua_pop( ls, 1);
	}
	lua_pop( ls, 1);
	STACKTRACE( ls, "loop after serialize table");
	return true;
ERROR:
	lua_pop(ls, 3);
	result->arsize = start_size;
	papuga_Serialization_pushValue_void( result);
	return false;
}

static papuga_Serialization* new_Serialization( papuga_lua_CallArgs* as)
{
	papuga_Serialization* rt = &as->serializations[ as->serc++];
	papuga_init_Serialization( rt);
	return rt;
}

static bool serialize_root( papuga_lua_CallArgs* as, lua_State *ls, int li)
{
	papuga_Serialization* result = new_Serialization( as);
	papuga_init_ValueVariant_serialization( &as->argv[as->argc], result);
	as->argc += 1;
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	rt &= serialize_node( as, result, ls, li);
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}

static int deserialize_root( papuga_CallResult* retval, papuga_Serialization* ser, lua_State *ls, const papuga_lua_ClassDefMap* classdefmap);

static void deserialize_key( papuga_ValueVariant* item, lua_State *ls)
{
	switch (item->valuetype)
	{
		case papuga_TypeVoid:
			lua_pushnil( ls);
			break;
		case papuga_TypeDouble:
			lua_pushnumber( ls, item->value.Double);
			break;
		case papuga_TypeUInt:
			lua_pushinteger( ls, item->value.UInt);
			break;
		case papuga_TypeInt:
			lua_pushinteger( ls, item->value.Int);
			break;
		case papuga_TypeString:
			lua_pushlstring( ls, item->value.string, item->length);
			break;
		case papuga_TypeLangString:
			if (item->encoding == papuga_UTF8)
			{
				lua_pushlstring( ls, item->value.langstring, item->length);
			}
			else
			{
				papuga_lua_error( ls, "deserialize result", papuga_TypeError);
			}
			break;
		case papuga_TypeSerialization:
		case papuga_TypeHostObject:
			papuga_lua_error( ls, "deserialize result", papuga_TypeError);
		case papuga_TypeIterator:
		default:
			papuga_lua_error( ls, "deserialize result", papuga_NotImplemented);
	}
}

static void deserialize_value( papuga_CallResult* retval, papuga_ValueVariant* item, lua_State *ls, const papuga_lua_ClassDefMap* classdefmap)
{
	switch (item->valuetype)
	{
		case papuga_TypeVoid:
			lua_pushnil( ls);
			break;
		case papuga_TypeDouble:
			lua_pushnumber( ls, item->value.Double);
			break;
		case papuga_TypeUInt:
			lua_pushinteger( ls, item->value.UInt);
			break;
		case papuga_TypeInt:
			lua_pushinteger( ls, item->value.Int);
			break;
		case papuga_TypeString:
			lua_pushlstring( ls, item->value.string, item->length);
			break;
		case papuga_TypeLangString:
			if (item->encoding == papuga_UTF8)
			{
				lua_pushlstring( ls, item->value.langstring, item->length);
			}
			else
			{
				papuga_lua_error( ls, "deserialize result", papuga_TypeError);
			}
			break;
		case papuga_TypeHostObject:
		{
			papuga_lua_UserData* udata;
			const papuga_lua_ClassDef* classdef = get_classdef( classdefmap, item->classid);
			if (!classdef || item->value.hostObjectData != retval->object.data)
			{
				papuga_lua_error( ls, "deserialize result", papuga_LogicError);
			}
			// MEMORY LEAK ON ERROR: papuga_destroy_CallResult( retval) not called when papuga_lua_new_userdata fails because of a memory allocation error
			udata = papuga_lua_new_userdata( ls, classdef->name);
			papuga_lua_init_UserData( udata, retval->value.classid, retval->object.data, retval->object.destroy);
			papuga_release_HostObject( &retval->object);
			break;
		}
		case papuga_TypeSerialization:
		{
			int nofitems = deserialize_root( retval, item->value.serialization, ls, classdefmap);
			if (nofitems == 0)
			{
				lua_pushnil( ls);
			}
			else if (nofitems > 1)
			{
				papuga_lua_error( ls, "deserialize result", papuga_TypeError);
			}
			break;
		}
		case papuga_TypeIterator:
		default:
			papuga_lua_error( ls, "deserialize result", papuga_NotImplemented);
	}
}

static void deserialize_node( papuga_CallResult* retval, papuga_Node** ni, papuga_Node* ne, lua_State *ls, const papuga_lua_ClassDefMap* classdefmap)
{
	unsigned int keyindex = 0;
	papuga_ValueVariant name;

	papuga_init_ValueVariant( &name);

	for (; (*ni) != ne; ++(*ni))
	{
		switch ((*ni)->tag)
		{
			case papuga_TagOpen:
				STACKTRACE( ls, "deserialize node open");
				lua_newtable( ls);
				++(*ni);
				deserialize_node( retval, ni, ne, ls, classdefmap);
				if (papuga_ValueVariant_defined( &name))
				{
					deserialize_key( &name, ls);
					papuga_init_ValueVariant( &name);
				}
				else
				{
					lua_pushinteger( ls, ++keyindex);
				}
				if ((*ni)->tag != papuga_TagClose)
				{
					papuga_lua_error( ls, "deserialize result", papuga_TypeError);
				}
				lua_insert( ls, -2);
				lua_rawset( ls, -3);
				break;
			case papuga_TagClose:
				STACKTRACE( ls, "deserialize node close");
				return;
			case papuga_TagName:
				STACKTRACE( ls, "deserialize node name");
				if (papuga_ValueVariant_defined( &name))
				{
					papuga_lua_error( ls, "deserialize result", papuga_TypeError);
				}
				papuga_init_ValueVariant_copy( &name, &(*ni)->value);
				break;
			case papuga_TagValue:
				STACKTRACE( ls, "deserialize node value");
				if (papuga_ValueVariant_defined( &name))
				{
					deserialize_key( &name, ls);
					papuga_init_ValueVariant( &name);
				}
				else
				{
					lua_pushinteger( ls, ++keyindex);
				}
				deserialize_value( retval, &(*ni)->value, ls, classdefmap);
				lua_rawset( ls, -3);
				break;
		}
	}
}

static int deserialize_root( papuga_CallResult* retval, papuga_Serialization* ser, lua_State *ls, const papuga_lua_ClassDefMap* classdefmap)
{
	int rt = 0;
	papuga_Node* ni = ser->ar;
	papuga_Node* ne = ni + ser->arsize;
	for (; ni != ne; ++ni,++rt)
	{
		if (ni->tag == papuga_TagOpen)
		{
			++ni;
			lua_newtable( ls);
			deserialize_node( retval, &ni, ne, ls, classdefmap);
			if (ni == ne) papuga_lua_error( ls, "deserialize result", papuga_UnexpectedEof);
			if (ni->tag != papuga_TagClose) papuga_lua_error( ls, "deserialize result", papuga_TypeError);
		}
		else if (ni->tag == papuga_TagValue)
		{
			deserialize_value( retval, &ni->value, ls, classdefmap);
		}
		else
		{
			papuga_lua_error( ls, "deserialize result", papuga_TypeError);
		}
	}
	return rt;
}

struct papuga_lua_UserData
{
	int classid;
	int checksum;
	void* objectref;
	papuga_Deleter destructor;
};

#define KNUTH_HASH 2654435761U
static int calcCheckSum( papuga_lua_UserData* udata)
{
	return (((udata->classid ^ (uintptr_t)udata->objectref) * KNUTH_HASH) ^ (uintptr_t)udata->destructor);
}

static int papuga_lua_destroy_UserData( lua_State* ls)
{
	papuga_lua_UserData* udata = (papuga_lua_UserData*)lua_touserdata( ls, 1);
	if (calcCheckSum(udata) != udata->checksum)
	{
		papuga_lua_error( ls, "destructor", papuga_InvalidAccess);
	}
	++udata->checksum;
	if (udata->destructor) udata->destructor( udata->objectref);
	return 0;
}

static papuga_lua_UserData* get_UserData( lua_State* ls, int idx, const papuga_lua_ClassDefMap* classdefmap)
{
	papuga_lua_UserData* udata = (papuga_lua_UserData*)lua_touserdata( ls, idx);
	const papuga_lua_ClassDef* cdef = get_classdef( classdefmap, udata->classid);
	if (!cdef || calcCheckSum(udata) != udata->checksum)
	{
		return 0;
	}
	if (!luaL_testudata( ls, idx, cdef->name))
	{
		return 0;
	}
	return udata;
}

static void release_UserData( papuga_lua_UserData* udata)
{
	udata->classid = 0;
	udata->objectref = 0;
	udata->destructor = 0;
	udata->checksum = 0;
}

static void createMetaTable( lua_State* ls, const char* classname, unsigned int classid, const luaL_Reg* mt, papuga_Deleter destructor)
{
	luaL_newmetatable( ls, classname);
	luaL_setfuncs( ls, mt, 0);
	lua_pushliteral( ls, "__index");
	lua_pushvalue( ls, -2);
	lua_rawset( ls, -3);

	lua_pushliteral( ls, "__newindex");
	lua_pushvalue( ls, -2);
	lua_rawset( ls, -3);

	lua_pushliteral( ls, "classname");
	lua_pushstring( ls, classname);
	lua_rawset( ls, -3);

	lua_pushliteral( ls, "classid");
	lua_pushinteger( ls, classid);
	lua_rawset( ls, -3);

	lua_pushliteral( ls, "__gc");
	lua_pushcfunction( ls, papuga_lua_destroy_UserData);
	lua_rawset( ls, -3);

	lua_setglobal( ls, classname);
}


DLL_PUBLIC void papuga_lua_declare_class( lua_State* ls, int classid, const char* classname,
				const luaL_Reg* mt, papuga_Deleter destructor)
{
	createMetaTable( ls, classname, classid, mt, destructor);
}

DLL_PUBLIC papuga_lua_UserData* papuga_lua_new_userdata( lua_State* ls, const char* classname)
{
	papuga_lua_UserData* rt = (papuga_lua_UserData*)lua_newuserdata( ls, sizeof(papuga_lua_UserData));
	release_UserData( rt);
	luaL_getmetatable( ls, classname);
	lua_setmetatable( ls, -2);
	return rt;
}

DLL_PUBLIC void papuga_lua_init_UserData( papuga_lua_UserData* udata, int classid, void* objectref, papuga_Deleter destructor)
{
	udata->classid = classid;
	udata->objectref = objectref;
	udata->destructor = destructor;
	udata->checksum = calcCheckSum( udata);
}

DLL_PUBLIC void papuga_lua_error( lua_State* ls, const char* function, papuga_ErrorCode err)
{
	luaL_error( ls, "%s (%s)", papuga_ErrorCode_tostring( err), function);
}

DLL_PUBLIC void papuga_lua_error_str( lua_State* ls, const char* function, const char* errormsg)
{
	luaL_error( ls, "%s (%s)", errormsg, function);
}

DLL_PUBLIC bool papuga_lua_init_CallArgs( lua_State *ls, int argc, papuga_lua_CallArgs* as, const char* classname, const papuga_lua_ClassDefMap* classdefmap)
{
	int argi = 1;
	as->erridx = -1;
	as->errcode = 0;
	as->self = 0;
	as->serc = 0;
	as->argc = 0;

	if (classname)
	{
		papuga_lua_UserData* udata = get_UserData( ls, 1, classdefmap);
		if (argc <= 0 || !udata)
		{
			as->errcode = papuga_MissingSelf;
			return false;
		}
		as->self = udata->objectref;
		++argi;
	}
	if (argc > papuga_LUA_MAX_NOF_ARGUMENTS)
	{
		as->errcode = papuga_NofArgsError;
		return false;
	}
	for (; argi <= argc; ++argi)
	{
		switch (lua_type (ls, argi))
		{
			case LUA_TNIL:
				papuga_init_ValueVariant( &as->argv[as->argc]);
				as->argc += 1;
				break;
			case LUA_TNUMBER:
				init_ValueVariant_number( &as->argv[as->argc], lua_tonumber( ls, argi));
				as->argc += 1;
				break;
			case LUA_TBOOLEAN:
				papuga_init_ValueVariant_bool( &as->argv[as->argc], lua_toboolean( ls, argi));
				as->argc += 1;
				break;
			case LUA_TSTRING:
			{
				size_t strsize;
				const char* str = lua_tolstring( ls, argi, &strsize);
				papuga_init_ValueVariant_string( &as->argv[as->argc], str, strsize);
				as->argc += 1;
				break;
			}
			case LUA_TTABLE:
				if (!serialize_root( as, ls, argi)) goto ERROR;
				break;
			case LUA_TUSERDATA:
			{
				papuga_lua_UserData* udata = get_UserData( ls, argi, classdefmap);
				papuga_init_ValueVariant_hostobj( &as->argv[as->argc], udata->objectref, udata->classid);
				as->argc += 1;
				break;
			}
			case LUA_TFUNCTION:	goto ERROR;
			case LUA_TTHREAD:	goto ERROR;
			case LUA_TLIGHTUSERDATA:goto ERROR;
			default:		goto ERROR;
		}
	}
	return true;
ERROR:
	as->erridx = argi;
	as->errcode = papuga_TypeError;
	papuga_lua_destroy_CallArgs( as);
	return false;
}

DLL_PUBLIC void papuga_lua_destroy_CallArgs( papuga_lua_CallArgs* arg)
{
	size_t si=0, se=arg->serc;
	for (; si != se; ++si)
	{
		papuga_destroy_Serialization( &arg->serializations[si]);
	}
}

DLL_PUBLIC int papuga_lua_move_CallResult( lua_State *ls, papuga_CallResult* retval, const papuga_lua_ClassDefMap* classdefmap, papuga_ErrorCode* errcode)
{
	int rt = 0;

	if (papuga_CallResult_hasError( retval))
	{
		char* errorstr = retval->errorbuf.ptr;
		papuga_destroy_CallResult( retval);
		lua_pushstring( ls, errorstr);
		lua_error( ls);
	}
	switch (retval->value.valuetype)
	{
		case papuga_TypeVoid:
			rt = 0;
			break;
		case papuga_TypeDouble:
			lua_pushnumber( ls, retval->value.value.Double);
			rt = 1;
			break;
		case papuga_TypeUInt:
			lua_pushnumber( ls, retval->value.value.UInt);
			rt = 1;
			break;
		case papuga_TypeInt:
			lua_pushinteger( ls, retval->value.value.Int);
			rt = 1;
			break;
		case papuga_TypeString:
			// MEMORY LEAK ON ERROR: papuga_destroy_CallResult( retval) not called when lua_pushlstring fails because of a memory allocation error
			lua_pushlstring( ls, retval->value.value.string, retval->value.length);
			rt = 1;
			break;
		case papuga_TypeLangString:
		{
			size_t strsize;
			const char* str = papuga_ValueVariant_tostring( &retval->value, &retval->allocator, &strsize, errcode);
			if (!str)
			{
				rt = -1;
			}
			else
			{
				// MEMORY LEAK ON ERROR: papuga_destroy_CallResult( retval) not called when lua_pushlstring fails because of a memory allocation error
				lua_pushlstring( ls, str, strsize);
				rt = 1;
			}
			break;
		}
		case papuga_TypeHostObject:
		{
			papuga_lua_UserData* udata;
			const papuga_lua_ClassDef* classdef = get_classdef( classdefmap, retval->value.classid);
			if (!classdef)
			{
				papuga_destroy_CallResult( retval);
				papuga_lua_error( ls, "move result", papuga_LogicError);
			}
			// MEMORY LEAK ON ERROR: papuga_destroy_CallResult( retval) not called when papuga_lua_new_userdata fails because of a memory allocation error
			udata = papuga_lua_new_userdata( ls, classdef->name);
			papuga_lua_init_UserData( udata, retval->value.classid, retval->object.data, retval->object.destroy);
			papuga_release_HostObject( &retval->object);
			rt = 1;
			break;
		}
		case papuga_TypeSerialization:
			// MEMORY LEAK ON ERROR: papuga_destroy_CallResult( retval) not called when papuga_lua_new_userdata fails because of a memory allocation error
			rt = deserialize_root( retval, retval->value.value.serialization, ls, classdefmap);
			break;
		case papuga_TypeIterator:
		default:
			papuga_destroy_CallResult( retval);
			papuga_lua_error( ls, "move result", papuga_NotImplemented);
			break;
	}
	papuga_destroy_CallResult( retval);
	return rt;
}


