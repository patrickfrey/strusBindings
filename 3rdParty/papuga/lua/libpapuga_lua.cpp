/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library interface for libpapuga_lua for generating lua bindings
/// \file libpapuga_lua.cpp
#include "papuga/lib/lua.hpp"
#include "private/dll_tags.hpp"
#include <string>
#include <cstdio>
#include <cstdarg>
#include <stdexcept>

using namespace papuga;

static std::runtime_error exception( const char* msg, ...)
{
	char buffer[ 2048];
	va_list args;
	va_start( args, msg);
	int buffersize = std::vsnprintf( buffer, sizeof(buffer), msg, args);
	buffer[ sizeof(buffer)-1] = 0;
	std::runtime_error rt( std::string( buffer, buffersize));
	va_end (args);
	return rt;
}

#define INDENT "\t"

static int define_method_call_helpers( std::ostream& out, const LanguageInterface::InterfaceDescription& descr)
{
	out << "#define MAX_NOF_ARGUMENTS " << (((descr.details.max_argc + 31) / 32) * 32) << std::endl;
	out << "enum LuaErrorCode {ErrLogic=1,ErrNoMem=2,ErrType=3,ErrNofArgs=4,ErrDeep=5};" << std::endl;
	out << std::endl;
	out << "struct Arguments" << std::endl;
	out << "{" << std::endl;
	out << INDENT << "Arguments( lua_State *L)" << std::endl;
	out << INDENT << INDENT << ":erridx(-1),errcode(0)" << std::endl;
	out << INDENT << "{" << std::endl;
	out << INDENT << INDENT << "argv = reinterpret_cast<ValueVariant*>(&mem);" << std::endl;
	out << INDENT << INDENT << "int ai=0, argc = lua_gettop(L);" << std::endl;
	out << INDENT << INDENT << "if (argc > MAX_NOF_ARGUMENTS)" << std::endl;
	out << INDENT << INDENT << "{" << std::endl;
	out << INDENT << INDENT << INDENT << "errcode=ErrNofArgs; goto ERROR;" << std::endl;
	out << INDENT << INDENT << "}" << std::endl;
	out << INDENT << INDENT << "for (; ai != argc; ++ai)" << std::endl;
	out << INDENT << INDENT << "{" << std::endl;
	switch (lua_type (L, ai))
	{
		case LUA_TNIL:		argv[ai].init(); break;
		case LUA_TNUMBER:	argv[ai].init( lua_tonumber( L, ai)); break;
		case LUA_TBOOLEAN:	argv[ai].init( (bool)lua_toboolean( L, ai)); break;
		case LUA_TSTRING:	argv[ai].init( lua_tostring( L, ai)); break;
		case LUA_TTABLE:	serialize( L, ai); if (errcode) {if (errcode != ErrLogic && errcode != ErrNoMem) errcode=ErrDeep; goto ERROR;} break;
		case LUA_TFUNCTION:	errcode = ErrType; goto ERROR;
		case LUA_TUSERDATA:	errcode = ErrType; goto ERROR;
		case LUA_TTHREAD:	errcode = ErrType; goto ERROR;
		case LUA_TLIGHTUSERDATA:errcode = ErrType; goto ERROR;
		default:		errcode = ErrType; goto ERROR;
	}
	out << INDENT << INDENT << "}" << std::endl;
	out << INDENT << INDENT << "return ae;" << std::endl;
	out << INDENT << "}" << std::endl;
	out << std::endl;
	out << INDENT << "int erridx" << std::endl;
	out << INDENT << "int errcode" << std::endl;
	out << INDENT << "std::size_t argc;" << std::endl;
	out << INDENT << "ValueVariant* argv;" << std::endl;
	out << INDENT << "unsigned int mem[ sizeof(ValueVariant) / sizeof(unsigned int) + 1];" << std::endl;
	out << INDENT << "std::list<Serialization> serializations;" << std::endl;
	out << std::endl;
	out << "private:" << std::endl;
	out << INDENT << "void serialize( lua_State *L, int ai)" << std::endl;
	out << INDENT << "{" << std::endl;
	out << INDENT << "try {" << std::endl;
	out << INDENT << INDENT << "serializations.push_back( Serialization());" << std::endl;
	out << INDENT << INDENT << "argv[ai].init( &serializations.back());" << std::endl;
	out << INDENT << INDENT << "serialize( serializations.back(), L, ai);" << std::endl;
	out << INDENT << "}" << std::endl;
	out << INDENT << "catch (const std::bad_alloc&)" << std::endl;
	out << INDENT << "{" << std::endl;
	out << INDENT << INDENT << "errcode = ErrNoMem;" << std::endl;
	out << INDENT << "}" << std::endl;
	out << INDENT << "}" << std::endl;
	static bool test_isarray( lua_State* L, int ai)
	{
		int len = lua_rawlen( L, ai);
		lua_rawgeti( L, ai-1, len);
		bool rt = !lua_isnil( L, -1);
		lua_pop( L, 1);
		return rt;
	}
	static bool serialize_key( Serialization& result, lua_State* L, int ai)
	{
		switch (lua_type (L, ai))
		{
			case LUA_TNIL:		result.pushName( ValueVariant()); break;
			case LUA_TNUMBER:	result.pushName( lua_tonumber( L, ai)); break;
			case LUA_TBOOLEAN:	result.pushName( (ValueVariant::IntType)lua_toboolean( L, ai)); break;
			case LUA_TSTRING:	result.pushName( lua_tostring( L, ai)); break;
			case LUA_TTABLE:	errcode = ErrType; return false;
			case LUA_TFUNCTION:	errcode = ErrType; return false;
			case LUA_TUSERDATA:	errcode = ErrType; return false;
			case LUA_TTHREAD:	errcode = ErrType; return false;
			case LUA_TLIGHTUSERDATA:errcode = ErrType; return false;
			default:		errcode = ErrType; return false;
		}
		return true;
	}
	static bool serialize_value( Serialization& result, lua_State* L, int ai)
	{
		switch (lua_type (L, ai))
		{
			case LUA_TNIL:		result.pushValue( ValueVariant()); break;
			case LUA_TNUMBER:	result.pushValue( lua_tonumber( L, ai)); break;
			case LUA_TBOOLEAN:	result.pushValue( (ValueVariant::IntType)lua_toboolean( L, ai)); break;
			case LUA_TSTRING:	result.pushValue( lua_tostring( L, ai)); break;
			case LUA_TTABLE:	result.pushOpen(); serialize( result, L, ai); result.pushClose(); break;
			case LUA_TFUNCTION:	errcode = ErrType; goto ERROR;
			case LUA_TUSERDATA:	errcode = ErrType; goto ERROR;
			case LUA_TTHREAD:	errcode = ErrType; goto ERROR;
			case LUA_TLIGHTUSERDATA:errcode = ErrType; goto ERROR;
			default:		errcode = ErrType; goto ERROR;
		}
	}
	static bool try_serialize_array( Serialization& result, lua_State* L, int ai)
	{
		if (!test_isarray( L, ai)) return false;
		std::size_t start_size = result.size();
		int idx = 0;
		lua_pushvalue( L, ai);
		lua_pushnil( L);
		while (lua_next(L, -2))
		{
			if (!lua_isinteger( L, -2) || lua_tointeger( L, -2) != ++idx) goto ERROR;
			serialize_value( result, L, -1);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
		return true;
	ERROR:
		lua_pop(L, 2);
		result.resize( start_size);
		return false;
	}
	out << INDENT << "void serialize( Serialization& result, lua_State *L, int ai)" << std::endl;
	out << INDENT << "{" << std::endl;
	if (try_serialize_array( result, L, ai)) return;
	if (errcode)
	std::size_t start_size = result.size();
	lua_pushvalue( L, ai);
	lua_pushnil( L);
	while (lua_next(L, -2))
	{
		if (!serialize_key( result, L, -2)) goto ERROR;
		if (!serialize_value( result, L, -1)) goto ERROR;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
ERROR:
	lua_pop(L, 2);
	result.resize( start_size);
	result.push_value( ValueVariant());
	out << INDENT << "}" << std::endl;
}


static void define_method( std::ostream& out, const LanguageInterface::InterfaceDescription::Method& method)
{
	out << "static int l_" << method.funcname << "( lua_State *L )" << std::endl;
	out << "{" << std::endl;
	out << INDENT << "int argc = lua_gettop(L);" << std::endl;
	    return luaL_error(L, "expecting exactly 2 arguments");
	  }
	out << INDENT << std::endl;
	out << "}" << std::endl << std::endl;
}

DLL_PUBLIC bool papuga::generateLuaSource(
	std::ostream& out,
	std::ostream& err,
	const std::string& what,
	const LanguageInterface::InterfaceDescription& descr)
{
	try
	{
		if (what == "header")
		{
			out << "#ifndef _PAPUGA_VALUE_VARIANT_HPP_INCLUDED" << std::endl;
			out << "#define _PAPUGA_" << descr.name << "_LUA_INTERFACE__INCLUDED" << std::endl;
			out << "///\remark GENERATED FILE (papuga lua generator) - DO NOT MODIFY" << std::endl;
			out << std::endl;
			out << "#include <lua.h>" << std::endl;
			out << "extern \"C\" int luaopen_" << descr.name << "( lua_State* L );" << std::endl;
			out << std::endl;
			out << "#endif" << std::endl;
		}
		else if (what == "module")
		{
			out << "#include \"lua_foo.h\"" << std::endl;
			
static int l_bar( lua_State *L )
{
    puts( "in bar()" );
    return 1;
}

int luaopen_foo( lua_State *L ) 
{
    static const luaL_Reg foo[] = {
        { "bar", l_bar },
        { NULL, NULL }
    };

    luaL_newlib( L, foo );
    return 1;
}
		}
		else
		{
			throw exception( "unknown item '%s'", what.c_str());
		}
		return true;
	}
	catch (const std::exception& ex)
	{
		err << "error generating lua binding source '" << what << "': " << ex.what();
	}
	return false;
}


