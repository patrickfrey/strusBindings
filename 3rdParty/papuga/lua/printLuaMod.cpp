/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Module for printing the lua module C header and source
/// \file printLuaMod.cpp
#include "printLuaMod.hpp"
#include "private/dll_tags.h"
#include "private/gen_utils.hpp"
#include "fmt/format.h"
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <sstream>

using namespace papuga;

#define INDENT "\t"

static std::string namespace_classname( const std::string& modulename, const std::string& classname)
{
	return modulename + '_' + classname;
}

static void define_classdefmap(
		std::ostream& out,
		const papuga_InterfaceDescription& descr)
{
	std::string modulename = descr.name;
	std::transform( modulename.begin(), modulename.end(), modulename.begin(), ::tolower);
	ClassDescriptionMap descrmap = papuga::getClassDescriptionMap( descr);
	unsigned int cidx = 0;
	ClassDescriptionMap::const_iterator ci = descrmap.begin(), ce = descrmap.end();
	for (; ci != ce; ++ci,++cidx)
	{
		for (; cidx+1 < ci->first; ++cidx){}
	}
	out << "static const char* g_classnamear[" << (cidx+1) << "] = {" << std::endl;
	cidx = 0;
	ci = descrmap.begin(), ce = descrmap.end();
	for (; ci != ce; ++ci,++cidx)
	{
		out << "\"" << namespace_classname( modulename, ci->second->name) << "\", ";
	}
	out << "NULL };" << std::endl << std::endl;
	out << "static const papuga_lua_ClassNameMap g_classnamemap = { " << cidx << ", g_classnamear };"
		<< std::endl << std::endl;
}

static void define_method(
		std::ostream& out,
		const papuga_InterfaceDescription& descr,
		const papuga_ClassDescription& classdef,
		const papuga_MethodDescription& method)
{
	std::string modulename = descr.name;
	std::transform( modulename.begin(), modulename.end(), modulename.begin(), ::tolower);

	std::string selfname = (method.self) ? (std::string("\"") + namespace_classname( modulename, classdef.name) + "\""):std::string("NULL");

	out << fmt::format( papuga::cppCodeSnippet( 0,
		"static int l_{nsclassname}_{methodname}( lua_State *ls)",
		"{",
		"int rt;",
		"papuga_lua_CallArgs arg;",
		"papuga_CallResult retval;",
		"char errbuf[ 2048];",
		"if (!papuga_lua_init_CallArgs( ls, lua_gettop(ls), &arg, {selfname})) papuga_lua_error( ls, \"{nsclassname}.{methodname}\", arg.errcode);",
		"papuga_init_CallResult( &retval, errbuf, sizeof(errbuf));",
		"if (!{funcname}( arg.self, &retval, arg.argc, arg.argv)) goto ERROR_CALL;",
		"papuga_lua_destroy_CallArgs( &arg);",
		"rt = papuga_lua_move_CallResult( ls, &retval, &g_classnamemap, &arg.errcode);",
		"if (rt < 0) papuga_lua_error( ls, \"{nsclassname}.{methodname}\", arg.errcode);",
		"return rt;",
		"ERROR_CALL:",
		"papuga_destroy_CallResult( &retval);",
		"papuga_lua_destroy_CallArgs( &arg);",
		"papuga_lua_error_str( ls, \"{nsclassname}.{methodname}\", errbuf);",
		"return 0; //... never get here (papuga_lua_error_str exits)",
		"}",
		0),
			fmt::arg("methodname", method.name),
			fmt::arg("nsclassname", namespace_classname( modulename, classdef.name)),
			fmt::arg("classname", classdef.name),
			fmt::arg("selfname", selfname),
			fmt::arg("funcname", method.funcname)
		) << std::endl;
}

static void define_constructor(
		std::ostream& out,
		const papuga_InterfaceDescription& descr,
		const papuga_ClassDescription& classdef)
{
	std::string modulename = descr.name;
	std::transform( modulename.begin(), modulename.end(), modulename.begin(), ::tolower);

	out << fmt::format( papuga::cppCodeSnippet( 0,
		"static int l_new_{nsclassname}( lua_State *ls)",
		"{",
		"papuga_lua_CallArgs arg;",
		"papuga_ErrorBuffer errbufstruct;",
		"char errbuf[ 2048];",
		"papuga_lua_UserData* udata = papuga_lua_new_userdata( ls, \"{nsclassname}\");",
		"if (!papuga_lua_init_CallArgs( ls, lua_gettop(ls)-1, &arg, NULL)) papuga_lua_error( ls, \"{nsclassname}.new\", arg.errcode);",
		"papuga_init_ErrorBuffer( &errbufstruct, errbuf, sizeof(errbuf));",
		"void* objref = {constructor}( &errbufstruct, arg.argc, arg.argv);",
		"if (!objref) goto ERROR_CALL;",
		"papuga_lua_destroy_CallArgs( &arg);",
		"papuga_lua_init_UserData( udata, {classid}, objref, {destructor}, &g_classnamemap);",
		"return 1;",
		"ERROR_CALL:",
		"papuga_lua_destroy_CallArgs( &arg);",
		"lua_pop(ls, 1);//... pop udata",
		"papuga_lua_error_str( ls, \"{nsclassname}.new\", errbuf);",
		"return 0; //... never get here (papuga_lua_error_str exits)",
		"}",
		0),
			fmt::arg("classname", classdef.name),
			fmt::arg("nsclassname", namespace_classname( modulename, classdef.name)),
			fmt::arg("classid", classdef.id),
			fmt::arg("constructor", classdef.constructor->funcname),
			fmt::arg("destructor", classdef.funcname_destructor)
		) << std::endl;
}

static void define_methodtable(
		std::ostream& out,
		const papuga_InterfaceDescription& descr,
		const papuga_ClassDescription& classdef)
{
	std::string modulename = descr.name;
	std::transform( modulename.begin(), modulename.end(), modulename.begin(), ::tolower);

	out << fmt::format( papuga::cppCodeSnippet( 0,
			"static const luaL_Reg mt_{nsclassname}[] =", "{", 0),
			fmt::arg("classname", classdef.name),
			fmt::arg("nsclassname", namespace_classname( modulename, classdef.name)));
	std::size_t mi = 0;
	if (classdef.constructor)
	{
		out << fmt::format( papuga::cppCodeSnippet( 1, "{{ \"new\", &l_new_{nsclassname} }},", 0),
				fmt::arg("classname", classdef.name),
				fmt::arg("nsclassname", namespace_classname( modulename, classdef.name)));
	}
	for (; classdef.methodtable[mi].name; ++mi)
	{
		out << fmt::format( papuga::cppCodeSnippet( 1, "{{ \"{methodname}\", &l_{nsclassname}_{methodname} }},", 0),
				fmt::arg("classname", classdef.name),
				fmt::arg("nsclassname", namespace_classname( modulename, classdef.name)),
				fmt::arg("methodname", classdef.methodtable[mi].name));
	}
	out << "\t" << "{0,0}" << "};" << std::endl << std::endl;
}

static void define_main(
		std::ostream& out,
		const papuga_InterfaceDescription& descr)
{
	std::string modulename = descr.name;
	std::transform( modulename.begin(), modulename.end(), modulename.begin(), ::tolower);

	out << fmt::format( papuga::cppCodeSnippet( 0,
		"int luaopen_{modulename}( lua_State* ls )",
		"{",
		"papuga_lua_init( ls);",
		0),
		fmt::arg("modulename", modulename)
	);
	std::size_t ci = 0;
	for (; descr.classes[ci].name; ++ci)
	{
		const papuga_ClassDescription& classdef = descr.classes[ci];
		out << fmt::format( papuga::cppCodeSnippet( 1,
			"papuga_lua_declare_class( ls, {classid}, \"{nsclassname}\", mt_{nsclassname});",
			0),
			fmt::arg("modulename", modulename),
			fmt::arg("classid", classdef.id),
			fmt::arg("classname", classdef.name),
			fmt::arg("nsclassname", namespace_classname( modulename, classdef.name)),
			fmt::arg("destructor", classdef.funcname_destructor)
		);
	}
	out << "\t" << "return 0;" << std::endl << "}" << std::endl << std::endl;
}

void papuga::printLuaModHeader(
		std::ostream& out,
		const papuga_InterfaceDescription& descr)
{
	std::string modulename = descr.name;
	std::transform( modulename.begin(), modulename.end(), modulename.begin(), ::tolower);
	out << fmt::format( papuga::cppCodeSnippet( 0,
		"#ifndef _PAPUGA_{modulename}_LUA_INTERFACE__INCLUDED",
		"#define _PAPUGA_{modulename}_LUA_INTERFACE__INCLUDED",
		"///\\remark GENERATED FILE (libpapuga_lua_gen) - DO NOT MODIFY",
		"",
		"#include \"lua.h\"",
		"#ifdef __cplusplus",
		"extern \"C\" {",
		"#endif",
		"int luaopen_{modulename}( lua_State* ls);",
		"",
		"#ifdef __cplusplus",
		"}",
		"#endif",
		"#endif",
		0),
		fmt::arg("modulename", modulename)
	) << std::endl;
}

void papuga::printLuaModSource(
		std::ostream& out,
		const papuga_InterfaceDescription& descr,
		const std::vector<std::string>& includes)
{
	out << papuga::cppCodeSnippet( 0,
		"#include \"lauxlib.h\"",
		"#include \"papuga.h\"",
		"#include \"papuga/lib/lua_dev.h\"", 0);

	char const** fi = descr.includefiles;
	for (; *fi; ++fi)
	{
		out << "#include \"" << *fi << "\"" << std::endl;
	}
	std::vector<std::string>::const_iterator ai = includes.begin(), ae = includes.end();
	for (; ai != ae; ++ai)
	{
		out << "#include \"" << *ai << "\"" << std::endl;
	}
	out << "///\\remark GENERATED FILE (libpapuga_lua_gen) - DO NOT MODIFY" << std::endl;
	out << std::endl;

	define_classdefmap( out, descr);

	std::size_t ci;
	for (ci=0; descr.classes[ci].name; ++ci)
	{
		const papuga_ClassDescription& classdef = descr.classes[ci];
		if (classdef.constructor)
		{
			define_constructor( out, descr, classdef);
		}
		std::size_t mi = 0;
		for (; classdef.methodtable[mi].name; ++mi)
		{
			define_method( out, descr, classdef, classdef.methodtable[mi]);
		}
		define_methodtable( out, descr, classdef);
	}
	define_main( out, descr);
}

