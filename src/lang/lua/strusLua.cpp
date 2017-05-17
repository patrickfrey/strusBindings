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

static bool appendstr( char* buf, size_t bufsize, size_t* bufpos, const char* str, size_t strsize=0)
{
	if (!strsize) strsize=strlen(str);
	if (strsize >= bufsize - bufpos) return false;
	memcpy( buf+bufpos, msg, strsize);
	bufpos += strsize;
	buf[ bufpos] = 0;
}

static size_t printf_luaerr( char* buf, size_t bufsize, lua_State* ls, int index, const char* path)
{
	const char* msg = lua_tostring( ls, index);
	if (!msg) msg = "";
	const char* fp = strstr( msg, "[string \"");
	const char* ep = 0;
	if (fp) ep = strchr( fp, ']');

	if (fp && ep)
	{
		size_t bufpos = 0;
		bool sc = true;
		sc &= appendstr( buf, bufsize, &bufpos, msg, fp-msg);
		sc &= appendstr( buf, bufsize, &bufpos, "[", 1);
		sc &= appendstr( buf, bufsize, &bufpos, path);
		sc &= appendstr( buf, bufsize, &bufpos, ep);
		if (!sc)
		{
			return 0;
		}
		return bufpos;
	}
	else
	{
		size_t rt = snprintf( buf, bufsize, "%s", msg);
		if (rt >= bufsize-1)
		{
			rt = bufsize-1;
			buf[ rt] = 0;
		}
		return rt;
	}
}

int main( int argc, const char* argv[])
{
	const char* inpufile = 0;
	try
	{
		int argi = 1;
		for (; argi < argc; ++argi)
		{
			if (argv[0][0] != '-')
			{
				break;
			}
			if (0==std::strcmp( argv[argi], "--help") || 0==std::strcmp( argv[argi], "-h"))
			{
				printUsage();
				return 0;
			}
			if (0==std::strcmp( argv[argi], "--"))
			{
				++argi;
				break;
			}
		}
		if (argi == argc)
		{
			std::cerr << "too few arguments";
			printUsage();
			return -1;
		}
		else if (argi+1 < argc)
		{
			std::cerr << "too many arguments";
			printUsage();
			return -1;
		}
		const char* inpufile = argv[argi];
		lua_State* ls = luaL_newstate();
		luaL_openlibs( ls);
		luaopen_strus( ls);
		switch (luaL_loadfile( ls, inputfile))
		{
			case LUA_ERRSYNTAX:
			{
				std::cerr << "syntax error in file '" << inputfile << "'" << std::endl;
				return -2;
			}
			case LUA_ERRMEM:
			{
				std::cerr << "out of memory loading file '" << inputfile << "'" << std::endl;
				return -2;
			}
			case LUA_ERRFILE:
			{
				std::cerr << "could not load file '" << inputfile << "'" << std::endl;
				return -2;
			}
			default:
				std::cerr << "unknown error loading file '" << inputfile << "'" << std::endl;
				return -2;
		}
	
		int errcode = lua_pcall( ls, 0, LUA_MULTRET, 0);
		if (errcode)
		{
			switch (errcode)
			{
				case LUA_ERRRUN:
					std::cerr << "error in lua script: " << getLuaErrorMessage( ls, -1, inputfile) << std::endl;
				case LUA_ERRMEM:
					std::cerr << "out of memory in lua script: " << getLuaErrorMessage( ls, -1, inputfile) << std::endl;
				case LUA_ERRERR:
					std::cerr << "error in lua error handler: " << getLuaErrorMessage( ls, -1, inputfile) << std::endl;
				default:
					std::cerr << "unknown error in lua script" << std::endl;
			}
			return -3;
		}
		lua_close( ls);
		std::cerr << "done" << std::endl;
		return 0;
	}
	catch (const std::bad_alloc&)
	{
		std::cerr << "out of memory";
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << "runtime error: "<< err.what() << std::endl;
	}
	catch (const std::exception& err)
	{
		std::cerr << "uncaught exception: "<< err.what() << std::endl;
	}
	return -4;
}

