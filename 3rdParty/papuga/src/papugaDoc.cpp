/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Program for map a doxygen style file with a template
/// \file papugaDoc.cpp
#include "papuga/lib/doc_gen.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <cerrno>
#include <cstdio>
#include <cstring>

static void printUsage()
{
	std::cerr << "papugaDoc [-v][-o <outputfile> ] <template> <inputfile> {<variable>=<filepath>}" << std::endl;
	std::cerr << "Description : Program for generating documentation, mapping a doxygen like" << std::endl;
	std::cerr << "              documented source file with a template" << std::endl;
	std::cerr << "<outputfile>: File where to print output (default stdout)" << std::endl;
	std::cerr << "Option -v   : Verbose output" << std::endl;
	std::cerr << "<template>  : File with the template." << std::endl;
	std::cerr << "              Template language:" << std::endl;
	std::cerr << "                  comment <eolncomment>" << std::endl;
	std::cerr << "                  : specifies the end of line comment in the source to parse" << std::endl;
	std::cerr << "                    for interface documentation. Tags start with '@'" << std::endl;
	std::cerr << "                  variable [<name> '=']<tagname> ['[' <rangedef> ']'] [<encoder>]" << std::endl;
	std::cerr << "                  : Declares a documentation tag as variable" << std::endl;
	std::cerr << "                  namespace [<name> '=']<tagname> ['[' <rangedef> ']'] [<encoder>]" << std::endl;
	std::cerr << "                  : Declares a documentation tag as variable in the following scope" << std::endl;
	std::cerr << "                  template [<name> '=']<tagname> <em> <ob> <eb>" << std::endl;
	std::cerr << "                  : Declares a documentation tag to be interpreted as template." << std::endl;
	std::cerr << "                    The content of the template follows on the next line." << std::endl;
	std::cerr << "                    It ends with the character sequence specified with <em>." << std::endl;
	std::cerr << "                  group <tagname> { <tagname> }" << std::endl;
	std::cerr << "                  : Declares templates triggered by these input tags to be of" << std::endl;
	std::cerr << "                    the same group. Groups influence the end of scope." << std::endl;
	std::cerr << "                  ignore <tagname> { <tagname> }" << std::endl;
	std::cerr << "                  : Declares one or more input tags to be ignored." << std::endl;
	std::cerr << "              Meaning of elements in the grammar of the template language:" << std::endl;
	std::cerr << "              <eolncomment> sequence of characters starting an eoln comment" << std::endl;
	std::cerr << "              <name> replacement identifier of the variable template" << std::endl;
	std::cerr << "              <tagname> name of source documentation tag" << std::endl;
	std::cerr << "              <rangedef> specifies a substring (python syntax) of the content" << std::endl;
	std::cerr << "                 splitted by spaces to access. " << std::endl;
	std::cerr << "              <em> end marker of the content of a template" << std::endl;
	std::cerr << "              <ob> open bracket to mark variable references in a template" << std::endl;
	std::cerr << "              <eb> close bracket for variable references in a template" << std::endl;
	std::cerr << "<inputfile> : source file to map" << std::endl;
	std::cerr << "              Source contains program source with end of line comments" << std::endl;
	std::cerr << "              End of line comments starting with a '@' followed by" << std::endl;
	std::cerr << "              an identifier are mapped according the templates specified." << std::endl;
	std::cerr << "<variable>  : (optional) name of variable to define" << std::endl;
	std::cerr << "<filepath>  : (optional) content of file to assign to variable" << std::endl;
	std::cerr << "              Variable declarations passed with the command line are" << std::endl;
	std::cerr << "              mapped as variables into the main (first) template specified." << std::endl;
}

static std::string readFile( const std::string& filename)
{
	int err = 0;
	std::string rt;
	FILE* fh = ::fopen( filename.c_str(), "rb");
	if (!fh)
	{
		err = errno;
		goto ERROR;
	}
	unsigned int nn;
	enum {bufsize=(1<<12)};
	char buf[ bufsize];

	while (!!(nn=::fread( buf, 1/*nmemb*/, bufsize, fh)))
	{
		rt.append( buf, nn);
	}
	if (!feof( fh))
	{
		err = ::ferror( fh);
		::fclose( fh);
		goto ERROR;
	}
	::fclose( fh);
	return rt;
ERROR:
	std::snprintf( buf, sizeof(buf), "error reading file '%s': %s", filename.c_str(), std::strerror(err));
	throw std::runtime_error( buf);
}

static void writeFile( const std::string& filename, const std::string& content)
{
	unsigned char ch;
	FILE* fh = ::fopen( filename.c_str(), "wb");
	if (!fh)
	{
		throw std::runtime_error( std::strerror( errno));
	}
	std::string::const_iterator fi = content.begin(), fe = content.end();
	for (; fi != fe; ++fi)
	{
		ch = *fi;
		if (1 > ::fwrite( &ch, 1, 1, fh))
		{
			int ec = ::ferror( fh);
			if (ec)
			{
				::fclose( fh);
				throw std::runtime_error( std::strerror(ec));
			}
		}
	}
	::fclose( fh);
}

int main( int argc, const char** argv)
{
	int ec = 0;
	try
	{
		const char* outputfile = 0;
		bool verbose = false;
		int argi = 1;
		while (argc > argi && argv[argi][0] == '-')
		{
			char opt = argv[argi][1];
			const char* optarg = 0;
			if (opt == 'o')
			{
				if (argv[argi][2])
				{
					optarg = argv[argi]+2;
					argi += 1;
				}
				else if (argi+1 < argc)
				{
					optarg = argv[argi+1];
					argi += 2;
				}
				else
				{
					throw std::runtime_error( "missing argument of option");
				}
				if (opt == 'o')
				{
					outputfile = optarg;
				}
			}
			else
			{
				if (argv[argi][2])
				{
					printUsage();
					throw std::runtime_error( "no cascading of option characters allowed and not argument expected for this option");
				}
				if (opt == 'h')
				{
					printUsage();
					return 0;
				}
				else if (opt == 'v')
				{
					verbose = true;
					++argi;
				}
				else
				{
					printUsage();
					throw std::runtime_error( "unknown option option");
				}
			}
		}
		if (argc < argi+2)
		{
			printUsage();
			throw std::runtime_error( "too few arguments");
		}
		std::string source = readFile( argv[argi+0]);
		std::map<std::string,std::string> varmap;
		std::string docsrc = readFile( argv[argi+1]);
		argi += 2;
		for (; argi != argc; ++argi)
		{
			char const* si = argv[argi];
			const char* start = si;
			for (; *si && *si != '='; ++si){}
			if (!*si)
			{
				throw std::runtime_error( "expected assignment <var>=<value> as argument");
			}
			std::string name( start, si-start);
			std::string filename( si+1);
			if (name.empty() || filename.empty())
			{
				throw std::runtime_error( "expected assignment <var>=<value> as argument");
			}
			std::string value = readFile( filename);
			if (varmap.find( name) != varmap.end())
			{
				char buf[ 1024];
				std::snprintf( buf, sizeof(buf), "duplicate assignment: %s", argv[argi]);
				throw std::runtime_error( buf);
			}
			varmap[ name] = value;
		}
		if (verbose)
		{
			std::cerr << "verbose output enabled" << std::endl;
		}
		std::map<std::string,std::string> outputfiles;
		if (outputfile)
		{
			try
			{
				std::ofstream outs( outputfile);
				outs.exceptions( std::ios::badbit);
	
				if (!papuga::generateDoc( outs, std::cerr, outputfiles, source, docsrc, varmap, verbose))
				{
					throw std::runtime_error( "generate documentation failed");
				}
				outs.close();
			}
			catch (std::ofstream::failure e)
			{
				char buf[ 2048];
				std::snprintf( buf, sizeof(buf), "error in output to file: %s", e.what());
				throw std::runtime_error( buf);
			}
		}
		else
		{
			if (!papuga::generateDoc( std::cout, std::cerr, outputfiles, source, docsrc, varmap, verbose))
			{
				throw std::runtime_error( "generate documentation failed");
			}
		}
		std::map<std::string,std::string>::const_iterator
			oi = outputfiles.begin(), oe = outputfiles.end();
		for (; oi != oe; ++oi)
		{
			std::cerr << "writing output file '" << oi->first << "'" << std::endl;
			writeFile( oi->first, oi->second);
		}
		std::cerr << "done" << std::endl;
		return 0;
	}
	catch (const std::bad_alloc&)
	{
		std::cerr << "memory allocation error" << std::endl;
		return 12/*ENOMEM*/;
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << "error: " << err.what() << std::endl;
		return ec ? ec:-1;
	}
	catch (...)
	{
		std::cerr << "uncaught exception in code generator" << std::endl;
		return -1;
	}
}

