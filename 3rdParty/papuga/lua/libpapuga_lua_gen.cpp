/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library interface for libpapuga_lua for generating lua bindings
/// \file libpapuga_lua.cpp
#include "papuga/lib/lua_gen.hpp"
#include "private/dll_tags.h"
#include "private/gen_utils.hpp"
#include "fmt/format.h"
#include <string>
#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <sstream>

using namespace papuga;

#define INDENT "\t"

static void define_method(
		std::ostream& out,
		const papuga_ClassDescription& classdef,
		const papuga_MethodDescription& method)
{
	std::string selfname = (method.self) ? (std::string("\"") + classdef.name + "\""):std::string("NULL");

	out << fmt::format( papuga::cppCodeSnippet( 0,
		"static int l_{funcname}( lua_State *ls)",
		"{",
		"int rt;",
		"papuga_lua_CallArgs arg;",
		"papuga_CallResult retval;",
		"char errorbuf[ 2048];",
		"if (!papuga_lua_init_CallArgs( ls, &arg, {selfname})) papuga_lua_error( ls, \"{classname}.{methodname}\", arg.errcode);",
		"papuga_init_CallResult( &retval, errorbuf, sizeof(errorbuf));",
		"if (!{funcname}( arg.self, &retval, arg.argc, arg.argv)) goto ERROR_CALL;",
		"papuga_lua_destroy_CallArgs( &arg);",
		"rt = papuga_lua_move_CallResult( ls, &retval, &arg.errcode);",
		"if (rt < 0) papuga_lua_error( ls, \"{classname}.{methodname}\", arg.errcode);",
		"return rt;",
		"ERROR_CALL:",
		"papuga_destroy_CallResult( &retval);",
		"papuga_lua_destroy_CallArgs( &arg);",
		"papuga_lua_error_str( ls, \"{classname}.{methodname}\", errorbuf);",
		"}",
		0),
			fmt::arg("methodname", method.name),
			fmt::arg("classname", classdef.name),
			fmt::arg("selfname", selfname),
			fmt::arg("funcname", method.funcname)
		) << std::endl;
}

static void define_methodtable(
		std::ostream& out,
		const papuga_ClassDescription& classdef)
{
	out << fmt::format( papuga::cppCodeSnippet( 0,
			"static const luaL_Reg mt_{classname}[] =", "{", 0),
			fmt::arg("classname", classdef.name));
	std::size_t mi = 0;
	for (; classdef.methodtable[mi].name; ++mi)
	{
		out << fmt::format( papuga::cppCodeSnippet( 1, "{{ \"{methodmame}\", l_{funcname} }},", 0),
				fmt::arg("methodmame", classdef.methodtable[mi].name),
				fmt::arg("funcname", classdef.methodtable[mi].funcname));
	}
	out << "\t" << "{0,0}" << "};" << std::endl << std::endl;
}

typedef std::multimap<std::string,std::string>::const_iterator ArgIterator;

DLL_PUBLIC bool papuga::generateLuaSource(
	std::ostream& out,
	std::ostream& err,
	const std::string& what,
	const std::multimap<std::string,std::string>& args,
	const papuga_InterfaceDescription& descr)
{
	try
	{
		if (what == "header")
		{
			out << fmt::format( papuga::cppCodeSnippet( 0,
				"#ifndef _PAPUGA_{modulename}_LUA_INTERFACE__INCLUDED",
				"#define _PAPUGA_{modulename}_LUA_INTERFACE__INCLUDED",
				"///\\remark GENERATED FILE (papuga lua generator) - DO NOT MODIFY",
				"",
				"#include <lua.h>",
				"#ifdef __cplusplus",
				"extern \"C\" {",
				"#endif",
				"int luaopen_{modulename}( lua_State* L );",
				"",
				"#ifdef __cplusplus",
				"}",
				"#endif",
				"#endif",
				0),
				fmt::arg("modulename", descr.name)
			) << std::endl;
		}
		else if (what == "module")
		{
			out << papuga::cppCodeSnippet( 0,
				"#include <lauxlib.h>",
				"#include \"papuga.h\"",
				"#include \"papuga/lib/lua_dev.h\"",
				"#include \"strus/bindings/lua.h\"", 0) << std::endl;

			char const** fi = descr.includefiles;
			for (; *fi; ++fi)
			{
				out << "#include \"" << *fi << "\"" << std::endl;
			}
			std::pair<ArgIterator,ArgIterator> inclist = args.equal_range( "include");
			ArgIterator ai = inclist.first, ae = inclist.second;
			for (; ai != ae; ++ai)
			{
				out << "#include \"" << ai->second << "\"" << std::endl;
			}
			out << "///\\remark GENERATED FILE (papuga lua generator) - DO NOT MODIFY" << std::endl;
			out << std::endl;

			std::size_t ci = 0;
			for (; descr.classes[ci].name; ++ci)
			{
				std::size_t mi = 0;
				for (; descr.classes[ci].methodtable[mi].name; ++mi)
				{
					define_method( out, descr.classes[ci], descr.classes[ci].methodtable[mi]);
				}
				define_methodtable( out, descr.classes[ci]);
			}
		}
		else
		{
			throw papuga::runtime_error( "unknown item '%s'", what.c_str());
		}
		return true;
	}
	catch (const fmt::FormatError& ex)
	{
		err << "format error generating lua binding source '" << what << "': " << ex.what() << std::endl;
	}
	catch (const std::exception& ex)
	{
		err << "error generating lua binding source '" << what << "': " << ex.what() << std::endl;
	}
	return false;
}


