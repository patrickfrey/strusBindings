#include <lua.h>
#include <cstddef>
#include <list>
#include "papuga/valueVariant.hpp"
#include "papuga/callResult.hpp"
#include "papuga/serialization.hpp"

#define MAX_NOF_ARGUMENTS 32

using namespace papuga;

enum LuaErrorCode
{
	ErrLogic=1,
	ErrNoMem=2,
	ErrType=3,
	ErrNofArgs=4,
	ErrDeep=5
};

struct Arguments
{
	Arguments( lua_State *ls)
		:erridx(-1),errcode(0)
	{
		argv = reinterpret_cast<ValueVariant*>(&mem);
		int ai=0, argc = lua_gettop(ls);
		if (argc > MAX_NOF_ARGUMENTS)
		{
			errcode=ErrNofArgs;
			goto ERROR;
		}
		for (; ai != argc; ++ai)
		{
			switch (lua_type (ls, ai))
			{
				case LUA_TNIL:		argv[ai].init(); break;
				case LUA_TNUMBER:	argv[ai].init( lua_tonumber( ls, ai)); break;
				case LUA_TBOOLEAN:	argv[ai].init( (bool)lua_toboolean( ls, ai)); break;
				case LUA_TSTRING:	argv[ai].init( lua_tostring( ls, ai)); break;
				case LUA_TTABLE:	serialize( ls, ai); if (errcode) {if (errcode != ErrLogic && errcode != ErrNoMem) errcode=ErrDeep; goto ERROR;} break;
				case LUA_TFUNCTION:	goto ERROR;
				case LUA_TUSERDATA:	goto ERROR;
				case LUA_TTHREAD:	goto ERROR;
				case LUA_TLIGHTUSERDATA:goto ERROR;
				default:		goto ERROR;
			}
		}
	ERROR:
		erridx = ai;
		errcode = ErrType;
	}

	int erridx;
	int errcode;
	std::size_t argc;
	ValueVariant* argv;
	unsigned int mem[ sizeof(ValueVariant) / sizeof(unsigned int) + 1];
	std::list<Serialization> serializations;

private:
	void serialize( lua_State *ls, int ai)
	{
		try {
			serializations.push_back( Serialization());
			argv[ai].init( &serializations.back());
			serialize( serializations.back(), ls, ai);
		}
		catch (const std::bad_alloc&)
		{
			errcode = ErrNoMem;
		}
	}

	bool serialize_key( Serialization& result, lua_State* ls, int ai)
	{
		switch (lua_type (ls, ai))
		{
			case LUA_TNIL:		result.pushName( ValueVariant()); break;
			case LUA_TNUMBER:	result.pushName( lua_tonumber( ls, ai)); break;
			case LUA_TBOOLEAN:	result.pushName( (ValueVariant::IntType)lua_toboolean( ls, ai)); break;
			case LUA_TSTRING:	result.pushName( lua_tostring( ls, ai)); break;
			case LUA_TTABLE:	errcode = ErrType; return false;
			case LUA_TFUNCTION:	errcode = ErrType; return false;
			case LUA_TUSERDATA:	errcode = ErrType; return false;
			case LUA_TTHREAD:	errcode = ErrType; return false;
			case LUA_TLIGHTUSERDATA:errcode = ErrType; return false;
			default:		errcode = ErrType; return false;
		}
		return true;
	}

	bool serialize_value( Serialization& result, lua_State* ls, int ai)
	{
		switch (lua_type (ls, ai))
		{
			case LUA_TNIL:		result.pushValue( ValueVariant()); break;
			case LUA_TNUMBER:	result.pushValue( lua_tonumber( ls, ai)); break;
			case LUA_TBOOLEAN:	result.pushValue( (ValueVariant::IntType)lua_toboolean( ls, ai)); break;
			case LUA_TSTRING:	result.pushValue( lua_tostring( ls, ai)); break;
			case LUA_TTABLE:	result.pushOpen(); serialize( result, ls, ai); result.pushClose(); break;
			case LUA_TFUNCTION:	errcode = ErrType; return false;
			case LUA_TUSERDATA:	errcode = ErrType; return false;
			case LUA_TTHREAD:	errcode = ErrType; return false;
			case LUA_TLIGHTUSERDATA:errcode = ErrType; return false;
			default:		errcode = ErrType; return false;
		}
		return true;
	}
	
	static bool try_serialize_array( Serialization& result, lua_State* ls, int ai)
	{
		int len = lua_rawlen( ls, ai);
		lua_rawgeti( ls, ai-1, len);
		if (lua_isnil( ls, -1))
		{
			lua_pop( ls, 1);
			return false;
		}
		lua_pop( ls, 1);
		std::size_t start_size = result.size();
		int idx = 0;
		lua_pushvalue( ls, ai);
		lua_pushnil( ls);
		while (lua_next(ls, -2))
		{
			if (!lua_isinteger( ls, -2) || lua_tointeger( ls, -2) != ++idx) goto ERROR;
			serialize_value( result, ls, -1);
			lua_pop(ls, 1);
		}
		lua_pop(ls, 1);
		return true;
	ERROR:
		lua_pop(ls, 2);
		result.resize( start_size);
		return false;
	}
	
	void serialize( Serialization& result, lua_State *ls, int ai)
	{
		if (try_serialize_array( result, ls, ai)) return;
		if (errcode)
		std::size_t start_size = result.size();
		lua_pushvalue( ls, ai);
		lua_pushnil( ls);
		while (lua_next(ls, -2))
		{
			if (!serialize_key( result, ls, -2)) goto ERROR;
			if (!serialize_value( result, ls, -1)) goto ERROR;
			lua_pop(ls, 1);
		}
		lua_pop(ls, 1);
	ERROR:
		lua_pop(ls, 2);
		result.resize( start_size);
		result.push_value( ValueVariant());
	}
};



