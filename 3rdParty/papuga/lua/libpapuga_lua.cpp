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

bool papuga::generateLuaSource(
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
			out << "///\remark GENERATED FILE (papuga lua generator) DO NOT MODIFY" << std::endl;
			out << std::endl;
			out << "#include <lua.h>" << std::endl;
			out << "extern \"C\" int luaopen_" << descr.name << "( lua_State* L );" << std::endl;
			out << std::endl;
			out << "#endif" << std::endl;
		}
		else if (what == "module")
		{
			
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


