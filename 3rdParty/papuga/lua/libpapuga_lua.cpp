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

static void define_method_call_helpers( std::ostream& out, const LanguageInterface::InterfaceDescription& descr)
{
	out << "#define MAX_NOF_ARGUMENTS " << (((descr.details.max_argc + 31) / 32) * 32) << std::endl;
}

static void define_method( std::ostream& out, const LanguageInterface::InterfaceDescription::Method& method)
{
	out << "static int l_" << method.funcname << "( lua_State *L )" << std::endl;
	out << "{" << std::endl;
	out << INDENT << "int argc = lua_gettop(L);" << std::endl;
	out << INDENT << std::endl;
	out << "}" << std::endl << std::endl;
}

static void define_methodtable( std::ostream& out, const LanguageInterface::InterfaceDescription::Class& classdef)
{
	out << "static const luaL_Reg mt_" << classdef.name << "[] = {" << std::endl;
	std::size_t mi = 0;
	for (; classdef.methodtable[mi].name; ++mi)
	{
		out << INDENT << "{\"" << classdef.methodtable[mi].name << "\", l_" << classdef.methodtable[mi].funcname << "}," << std::endl;
	}
	out << INDENT << "{0,0}" << std::endl;
	out << "};" << std::endl << std::endl;
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
			out << "#include \"lua_" << descr.name << ".h\"" << std::endl;
			define_method_call_helpers( out, descr);
			std::size_t ci = 0;
			for (; descr.classes[ci].name; ++ci)
			{
				std::size_t mi = 0;
				for (; descr.classes[ci].methodtable[mi].name; ++mi)
				{
					define_method( out, descr.classes[ci].methodtable[mi]);
				}
				define_methodtable( out, descr.classes[ci]);
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


