#include <lua.h>
#include <lauxlib.h>
#include <stddef.h>
#include <math.h>
#include <float.h>
#include "papuga/lib/lua_dev.h"
#include "papuga/valueVariant.h"
#include "papuga/callResult.h"
#include "papuga/errors.h"
#include "papuga/serialization.h"
#include "papuga/hostObjectReference.h"
#include "private/dll_tags.h"

#define MAX_DOUBLE_INT            ((int64_t)1<<53)
#define MIN_DOUBLE_INT           -((int64_t)1<<53)
#define IS_CONVERTIBLE_TOINT( x)  ((x-floor(x) < 2*DBL_EPSILON) && x < MAX_DOUBLE_INT && x > -MIN_DOUBLE_INT)
#define NUM_EPSILON               (2*DBL_EPSILON)

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
	int len = lua_rawlen( ls, li);
	lua_rawgeti( ls, li-1, len);
	if (lua_isnil( ls, -1))
	{
		lua_pop( ls, 1);
		return false;
	}
	lua_pop( ls, 1);
	size_t start_size = result->arsize;
	int idx = 0;
	lua_pushvalue( ls, li);
	lua_pushnil( ls);
	while (lua_next(ls, -2))
	{
		if (!lua_isinteger( ls, -2) || lua_tointeger( ls, -2) != ++idx) goto ERROR;
		serialize_value( as, result, ls, -1);
		lua_pop(ls, 1);
	}
	lua_pop(ls, 1);
	return true;
ERROR:
	lua_pop(ls, 2);
	result->arsize = start_size;
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
	if (try_serialize_array( as, result, ls, li)) return true;

	lua_pushvalue( ls, li);
	lua_pushnil( ls);
	while (lua_next(ls, -2))
	{
		if (!serialize_key( as, result, ls, -2)) goto ERROR;
		if (!serialize_value( as, result, ls, -1)) goto ERROR;
		lua_pop(ls, 1);
	}
	lua_pop(ls, 1);
	return true;
ERROR:
	lua_pop(ls, 2);
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
	return serialize_node( as, result, ls, li);
}

static bool deserialize_root( papuga_lua_CallArgs* as, lua_State *ls, int li)
{
	papuga_Serialization* result = new_Serialization( as);
	return serialize_node( as, result, ls, li);
}

struct papuga_lua_UserData
{
	int classid;
	int checksum;
	void* objectref;
	papuga_lua_ClassDestructorFunc destructor;
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

static papuga_lua_UserData* get_UserData( lua_State* ls, int idx)
{
	papuga_lua_UserData* udata = (papuga_lua_UserData*)lua_touserdata( ls, 1);
	if (calcCheckSum(udata) != udata->checksum)
	{
		papuga_lua_error( ls, "destructor", papuga_InvalidAccess);
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

static const papuga_lua_ClassDef* get_classdef( const papuga_lua_ClassDefMap* classdefmap, unsigned int classid)
{
	return (classid >= classdefmap->size) ? NULL : &classdefmap->ar[ classid];
}

static void createMetaTable( lua_State* ls, const char* classname, unsigned int classid, const luaL_Reg* mt, papuga_HostObjectDeleter destructor)
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

	lua_pop( ls, 1);
}


DLL_PUBLIC void papuga_lua_declare_class( lua_State* ls, int classid, const char* classname,
				const luaL_Reg* mt, papuga_HostObjectDeleter destructor)
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

DLL_PUBLIC void papuga_lua_init_UserData( papuga_lua_UserData* udata, int classid, void* objectref, papuga_lua_ClassDestructorFunc destructor)
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

DLL_PUBLIC bool papuga_lua_init_CallArgs( lua_State *ls, papuga_lua_CallArgs* as, const char* classname, const papuga_lua_ClassDefMap* classdefmap)
{
	int argi = 0;

	as->erridx = -1;
	as->errcode = 0;
	as->self = 0;
	as->serc = 0;
	as->argc = lua_gettop(ls);
	if (classname)
	{
		if (as->argc == 0 || 0 == (as->self = luaL_testudata( ls, 1, classname)))
		{
			as->errcode = papuga_MissingSelf;
			return false;
		}
		++argi;
	}
	if (as->argc > papuga_LUA_MAX_NOF_ARGUMENTS)
	{
		as->errcode = papuga_NofArgsError;
		return false;
	}
	for (; argi != as->argc; ++argi)
	{
		switch (lua_type (ls, argi))
		{
			case LUA_TNIL:
				papuga_init_ValueVariant( &as->argv[argi]);
				break;
			case LUA_TNUMBER:
				init_ValueVariant_number( &as->argv[argi], lua_tonumber( ls, argi));
				break;
			case LUA_TBOOLEAN:
				papuga_init_ValueVariant_bool( &as->argv[argi], lua_toboolean( ls, argi));
				break;
			case LUA_TSTRING:
			{
				size_t strsize;
				const char* str = lua_tolstring( ls, argi, &strsize);
				papuga_init_ValueVariant_string( &as->argv[argi], str, strsize);
				break;
			}
			case LUA_TTABLE:
				if (!serialize_root( as, ls, argi)) goto ERROR;
				break;
			case LUA_TUSERDATA:
			{
				papuga_lua_UserData* udata = get_UserData( ls, argi);
				papuga_init_ValueVariant_hostobj( &as->argv[argi], udata->objectref, udata->classid);
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
	char* errorstr;
	const char* str;
	size_t strsize;

	if (papuga_CallResult_hasError( retval))
	{
		errorstr = retval->errorbuf;
		papuga_destroy_CallResult( retval);
		lua_pushstring( ls, errorstr);
		lua_error( ls);
	}
	switch (retval->value.valuetype)
	{
		case papuga_Void:
			rt = 0;
			break;
		case papuga_Double:
			lua_pushnumber( ls, retval->value.value.Double);
			rt = 1;
			break;
		case papuga_UInt:
			lua_pushnumber( ls, retval->value.value.UInt);
			rt = 1;
			break;
		case papuga_Int:
			lua_pushinteger( ls, retval->value.value.Int);
			rt = 1;
			break;
		case papuga_String:
			// MEMORY LEAK ON ERROR: papuga_destroy_CallResult( retval) not called when lua_pushlstring fails because of a memory allocation error
			lua_pushlstring( ls, retval->value.value.string, retval->value.length);
			rt = 1;
			break;
		case papuga_LangString:
		{
			str = papuga_ValueVariant_tostring( &retval->value, &retval->valuebuf, &strsize, errcode);
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
		case papuga_HostObject:
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
			papuga_release_HostObjectReference( &retval->object);
		}
		case papuga_Serialized:
			break;
	}
	papuga_destroy_CallResult( retval);
	return rt;
}


