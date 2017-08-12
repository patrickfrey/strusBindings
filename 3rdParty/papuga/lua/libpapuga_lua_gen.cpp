/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library interface for libpapuga_lua_gen for generating lua bindings
/// \file libpapuga_lua_gen.cpp
#include "papuga/lib/lua_gen.hpp"
#include "private/dll_tags.h"
#include "private/gen_utils.hpp"
#include "printLuaDoc.hpp"
#include "printLuaMod.hpp"
#include "fmt/format.h"
#include <string>
#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <sstream>

using namespace papuga;

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
			printLuaModHeader( out, descr);
		}
		else if (what == "module")
		{
			printLuaModSource( out, descr, getGeneratorArguments( args, "include"));
		}
		else if (what == "doc")
		{
			printLuaDoc( out, descr);
		}
		else
		{
			char buf[ 256];
			std::snprintf( buf, sizeof(buf), "unknown item '%s'", what.c_str());
			throw std::runtime_error( buf);
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


