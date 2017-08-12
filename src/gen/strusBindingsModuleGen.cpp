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
#include "papuga/lib/php7_gen.hpp"
#include "strus/lib/bindings_description.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/string_format.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <sstream>

#undef STRUS_LOWLEVEL_DEBUG

static void printUsage()
{
	std::cerr << "strusBindingsModuleGen <language> <what> <outputfile> {<name>=<value>}" << std::endl;
	std::cerr << "Description : Prints one specific part of the generated laguage binding to stdout" << std::endl;
	std::cerr << "<language>  : Language to generate language binding module for." << std::endl;
	std::cerr << "<what>      : What binding part to generate." << std::endl;
	std::cerr << "<outputfile>: Where to print the output ('-' for stdout)" << std::endl;
	std::cerr << "<name>      : Name of an argument to pass to the generator" << std::endl;
	std::cerr << "<value>     : Value of an argument to pass to the generator" << std::endl;
}

typedef bool (*GenerateSourceFunc)(
		std::ostream& out,
		std::ostream& err,
		const std::string& what,
		const std::multimap<std::string,std::string>& args,
		const papuga_InterfaceDescription& descr);

static void generateSource( GenerateSourceFunc printproc, const std::string& langname, const std::string& modname, std::multimap<std::string,std::string> args, const char* filename, const papuga_InterfaceDescription& descr)
{
	if (!filename || std::strcmp(filename,"-") == 0)
	{
		if (!printproc( std::cout, std::cerr, modname, args, descr))
		{
			throw std::runtime_error( std::string("failed to generate '") + langname + "' source");
		}
	}
	else
	{
		std::ostringstream out;
		if (!printproc( out, std::cerr, modname, args, descr))
		{
			throw std::runtime_error( std::string("failed to generate '") + langname + "' source");
		}
		int ec = strus::writeFile( filename, out.str());
		if (ec) throw std::runtime_error( std::string("failed to write output file '") + filename + "': " + std::strerror(ec));
	}
}

int main( int argc, const char* argv[])
{
	int ec = 0;
	try
	{
		const papuga_InterfaceDescription* interface_description = strus::getBindingsInterfaceDescription();
		if (!interface_description)
		{
			throw std::runtime_error( "could not get interface description");
		}
		if (argc < 4)
		{
			printUsage();
			throw std::runtime_error( "too few arguments");
		}
		const char* lang = argv[1];
		const char* what = argv[2];
		const char* filename = argv[3];
		std::multimap<std::string,std::string> args;
		int argi=4;
		for (; argi < argc; ++argi)
		{
			const char* eq = std::strchr( argv[argi], '=');
			if (!eq) throw std::runtime_error( std::string( "assignment (<name>=<value> expected as argument instead of '") + argv[argi] + "'");
			std::string name( argv[argi], eq-argv[argi]);
			std::string value( eq+1);
			args.insert( std::pair<std::string,std::string>( name, value));
		}

		GenerateSourceFunc printfunc;
		if (std::strcmp( lang, "lua") == 0)
		{
			printfunc = &papuga::generateLuaSource;
		}
		else if (std::strcmp( lang, "php7") == 0)
		{
			printfunc = &papuga::generatePhp7Source;
		}
		else
		{
			throw std::runtime_error( "unknown language");
		}
		//Output:
		generateSource( printfunc, lang, what, args, filename, *interface_description);
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
		return ec ? ec:-1;
	}
	catch (...)
	{
		std::cerr << "uncaught exception in code generator" << std::endl;
		return -1;
	}
}


