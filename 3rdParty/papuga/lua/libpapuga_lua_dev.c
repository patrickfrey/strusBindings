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
#include "private/dll_tags.h"

#define MAX_DOUBLE_INT            ((int64_t)1<<53)
#define MIN_DOUBLE_INT           -((int64_t)1<<53)
#define IS_CONVERTIBLE_TOINT( x)  ((x-floor(x) < 2*DBL_EPSILON) && x < MAX_DOUBLE_INT && x > -MIN_DOUBLE_INT)
#define NUM_EPSILON               (2*DBL_EPSILON)

static bool serialize_key( papuga_lua_CallArgs* as, papuga_Serialization* result, lua_State* ls, int li)
{
	bool rt = true;
	const char* str;
	size_t strsize;
	double numval;

	switch (lua_type (ls, li))
	{
		case LUA_TNIL:
			rt &= papuga_Serialization_pushName_void( result);
			break;
		case LUA_TNUMBER:
			numval = lua_tonumber( ls, li);
			if (IS_CONVERTIBLE_TOINT( numval))
			{
				if (numval > 0.0)
				{
					rt &= papuga_Serialization_pushName_uint( result, (uint64_t)(numval + NUM_EPSILON));
				}
				else
				{
					rt &= papuga_Serialization_pushName_int( result, (int64_t)(numval - NUM_EPSILON));
				}
			}
			else
			{
				rt &= papuga_Serialization_pushName_double( result, lua_tonumber( ls, li));
			}
			break;
		case LUA_TBOOLEAN:
			rt &= papuga_Serialization_pushName_bool( result, lua_toboolean( ls, li));
			break;
		case LUA_TSTRING:
			str = lua_tolstring( ls, li, &strsize);
			rt &= papuga_Serialization_pushName_string( result, str, strsize);
			break;
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
	int isnum;
	lua_Integer intval;

	switch (lua_type (ls, li))
	{
		case LUA_TNIL:
			rt &= papuga_Serialization_pushValue_void( result);
			break;
		case LUA_TNUMBER:
			if (0!=(intval=lua_tointegerx( ls, li, &isnum)) && isnum)
			{
				rt &= papuga_Serialization_pushValue_int( result, intval);
			}
			else
			{
				rt &= papuga_Serialization_pushValue_double( result, lua_tonumber( ls, li));
			}
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

DLL_PUBLIC bool papuga_init_lua_state( lua_State* ls)
{
	return true;
}

DLL_PUBLIC void papuga_lua_error( lua_State* ls, const char* function, papuga_ErrorCode err)
{
	luaL_error( ls, "%s (%s)", papuga_ErrorCode_tostring( err), function);
}

DLL_PUBLIC void papuga_lua_error_str( lua_State* ls, const char* function, const char* errormsg)
{
	luaL_error( ls, "%s (%s)", errormsg, function);
}

DLL_PUBLIC bool papuga_lua_init_CallArgs( lua_State *ls, papuga_lua_CallArgs* as, const char* classname)
{
	const char* str;
	size_t strsize;
	int isnum;
	lua_Integer intval;
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
				if (0!=(intval=lua_tointegerx( ls, argi, &isnum)) && isnum)
				{
					papuga_init_ValueVariant_int( &as->argv[argi], intval);
				}
				else
				{
					papuga_init_ValueVariant_double( &as->argv[argi], lua_tonumber( ls, argi));
				}
				break;
			case LUA_TBOOLEAN:
				papuga_init_ValueVariant_bool( &as->argv[argi], lua_toboolean( ls, argi));
				break;
			case LUA_TSTRING:
				str = lua_tolstring( ls, argi, &strsize);
				papuga_init_ValueVariant_string( &as->argv[argi], str, strsize);
				break;
			case LUA_TTABLE:
				if (!serialize_root( as, ls, argi)) goto ERROR;
				break;
			case LUA_TFUNCTION:	goto ERROR;
			case LUA_TUSERDATA:	goto ERROR;
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

DLL_PUBLIC int papuga_lua_move_CallResult( lua_State *ls, papuga_CallResult* retval, const char** classid2mtmap, papuga_ErrorCode* errcode)
{
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
	*errcode = papuga_Ok;
	switch (retval->value.valuetype)
	{
		case papuga_Void:
			return 0;
		case papuga_Double:
			lua_pushnumber( ls, retval->value.value.Double);
			return 1;
		case papuga_UInt:
			lua_pushnumber( ls, retval->value.value.UInt);
			return 1;
		case papuga_Int:
			lua_pushinteger( ls, retval->value.value.Int);
			return 1;
		case papuga_String:
			// MEMORY LEAK ON ERROR: papuga_destroy_CallResult( retval) not called when lua_pushlstring fails because of a memory allocation error
			lua_pushlstring( ls, retval->value.value.string, retval->value.length);
			return 1;
		case papuga_LangString:
			str = papuga_ValueVariant_tostring( &retval->value, &retval->valuebuf, &strsize, errcode);
			// MEMORY LEAK ON ERROR: papuga_destroy_CallResult( retval) not called when lua_pushlstring fails because of a memory allocation error
			lua_pushlstring( ls, str, strsize);
			return 1;
		case papuga_HostObject:
		case papuga_Serialized:
			break;
	}
	return 0;
}


