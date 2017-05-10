/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Code generator for the papuga language modules of strusBindings
/// \file strusBindingsModuleGen.cpp
#include "papuga/lib/lua_gen.hpp"
#include "strus/lib/bindings_description.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/string_format.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>

#undef STRUS_LOWLEVEL_DEBUG

static void printUsage()
{
	std::cerr << "strusBindingsModuleGen <language> <what>" << std::endl;
	std::cerr << "Description : Prints one specific part of the generated laguage binding to stdout" << std::endl;
	std::cerr << "<language>  : language to generate language binding module for." << std::endl;
	std::cerr << "<what>      : what binding part to generate." << std::endl;
}

int main( int argc, const char* argv[])
{
	try
	{
		const papuga_InterfaceDescription* interface_description = strus::getBindingsInterfaceDescription();
		if (!interface_description)
		{
			throw std::runtime_error( "could not get interface description");
		}
		if (argc <= 2)
		{
			printUsage();
			throw std::runtime_error( "too few arguments");
		}
		if (std::strcmp( argv[1], "lua") == 0)
		{
			papuga::generateLuaSource( std::cout, std::cerr, argv[2], *interface_description);
		}
		else
		{
			throw std::runtime_error( "unknown language");
		}
		//Output:
#ifdef STRUS_LOWLEVEL_DEBUG
#endif
		std::cerr << "done." << std::endl;
		return 0;
	}
	catch (const std::bad_alloc&)
	{
		std::cerr << "memory allocation error in code generator" << std::endl;
		return 12/*ENOMEM*/;
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << "error in code generator: " << err.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cerr << "uncaught exception in code generator" << std::endl;
		return -1;
	}
}


