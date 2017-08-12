/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library interface for libpapuga_php7_gen for generating PHP (v7) bindings
/// \file libpapuga_php7_gen.cpp
#include "papuga/lib/php7_gen.hpp"
#include "private/dll_tags.h"
#include "private/gen_utils.hpp"
#include "printPhp7Doc.hpp"
#include "printPhp7Mod.hpp"
#include "fmt/format.h"
#include <string>
#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <sstream>

using namespace papuga;

#if defined _WIN32
#define DEFAULT_MODULE_EXT ".dll"
#else
#define DEFAULT_MODULE_EXT ".so"
#endif

DLL_PUBLIC bool papuga::generatePhp7Source(
	std::ostream& out,
	std::ostream& err,
	const std::string& what,
	const std::multimap<std::string,std::string>& args,
	const papuga_InterfaceDescription& descr)
{
	try
	{
		if (what == "module")
		{
			printPhp7ModSource( out, descr, getGeneratorArguments( args, "include"));
		}
		else if (what == "ini")
		{
			std::string iniarg( getGeneratorArgument( args, "phpini", 0));
			std::string dllext( getGeneratorArgument( args, "dllext", DEFAULT_MODULE_EXT));
			printPhp7ModIni( out, descr, readFile( iniarg), dllext);
		}
		else if (what == "doc")
		{
			printPhp7Doc( out, descr);
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
		err << "format error generating PHP (v7) binding source '" << what << "': " << ex.what() << std::endl;
	}
	catch (const std::exception& ex)
	{
		err << "error generating PHP (v7) binding source '" << what << "': " << ex.what() << std::endl;
	}
	return false;
}


