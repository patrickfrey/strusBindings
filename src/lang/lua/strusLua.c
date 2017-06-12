/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "strus/lib/lua.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#undef STRUS_LOWLEVEL_DEBUG

static void printUsage()
{
	fprintf( stderr, "%s",
		 "strusLua [<options>] <luascript>\n"
		"<options>:\n" 
		"   -h,--help   :Print this usage\n"
		"<luascript>    :Lua script to execute\n");
}

int main( int argc, const char* argv[])
{
	const char* inputfile = 0;
	lua_State* ls = 0;
	int argi = 1;
	int errcode = 0;

	// Parse command line arguments:
	for (; argi < argc; ++argi)
	{
		if (argv[0][0] != '-')
		{
			break;
		}
		if (0==strcmp( argv[argi], "--help") || 0==strcmp( argv[argi], "-h"))
		{
			printUsage();
			return 0;
		}
		if (0==strcmp( argv[argi], "--"))
		{
			++argi;
			break;
		}
	}
	if (argi == argc)
	{
		fprintf( stderr, "too few arguments (less than one)\n");
		return -1;
	}
	inputfile = argv[ argi];
#ifdef STRUS_LOWLEVEL_DEBUG
	fprintf( stderr, "create lua state\n");
#endif
	// Define program arguments for lua script:
	ls = luaL_newstate();
	luaL_openlibs( ls);
	luaopen_strus( ls);
	int ai = 0, ae = argc-argi;
	lua_newtable( ls);
	for (; ai != ae; ++ai)
	{
		lua_pushinteger( ls, ai);
		lua_pushstring( ls, argv[argi+ai]);
		lua_rawset( ls, -3);
	}
	lua_setglobal( ls, "arg");

	// Load the script:
#ifdef STRUS_LOWLEVEL_DEBUG
	fprintf( stderr, "load script file: '%s'\n", inputfile);
#endif
	errcode = luaL_loadfile( ls, inputfile);
	if (errcode != LUA_OK)
	{
		switch (errcode)
		{
			case LUA_ERRSYNTAX:
			{
				fprintf( stderr, "LUA_ERRSYNTAX %s loading script '%s'\n", lua_tostring( ls, -1), inputfile);
				break;
			}
			case LUA_ERRMEM:
			{
				fprintf( stderr, "out of memory loading script '%s'\n", inputfile);
				break;
			}
			case LUA_ERRFILE:
			{
				fprintf( stderr, "LUA_ERRFILE loading script '%s'\n", inputfile);
				break;
			}
			default:
				fprintf( stderr, "unknown error loading script '%s'\n", inputfile);
				break;
		}
		lua_close( ls);
		return -2;
	}
#ifdef STRUS_LOWLEVEL_DEBUG
	fprintf( stderr, "starting ...\n");
#endif
	// Run the script:
	errcode = lua_pcall( ls, 0, LUA_MULTRET, 0);
	if (errcode)
	{
		switch (errcode)
		{
			case LUA_OK:
				lua_close( ls);
				return 0;
			case LUA_ERRRUN:
				fprintf( stderr, "error in script '%s': '%s'\n", inputfile, lua_tostring( ls, -1));
				break;
			case LUA_ERRMEM:
				fprintf( stderr, "out of memory in script '%s'\n", inputfile);
				break;
			case LUA_ERRGCMM:
				fprintf( stderr, "internal error in destructor call (call of __gc metamethod) in script '%s'\n", inputfile);
				break;
			case LUA_ERRERR:
				fprintf( stderr, "error in lua error handler executing script '%s'\n", inputfile);
				break;
			default:
				fprintf( stderr, "unknown error in script '%s'\n", inputfile);
				break;
		}
		lua_close( ls);
		return -3;
	}
	lua_close( ls);
	return 0;
}

