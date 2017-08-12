/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Some utility functions for generating language binding sources
/// \file utils.cpp
#include "private/gen_utils.hpp"
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <stdint.h>

using namespace papuga;

std::string papuga::cppCodeSnippet( unsigned int idntcnt, ...)
{
	std::ostringstream rt;
	std::string indent( idntcnt, '\t');
	va_list args;
	va_start( args, idntcnt);
	unsigned int ai = 0;
	for (;; ++ai)
	{
		const char* ln = va_arg( args,const char*);
		if (!(unsigned int)(uintptr_t)ln) break;
		std::size_t nn = std::strlen( ln);
		if (nn == 0)
		{
			rt << std::endl;
		}
		else if (ln[0] == '#')
		{
			rt << ln << std::endl;
		}
		else if (ln[nn-1] == '{')
		{
			rt << indent << ln << "{" << std::endl;
			indent += "\t";
		}
		else if (ln[0] == '}')
		{
			if (indent.empty()) throw std::runtime_error( "format string error");
			indent.resize( indent.size() -1);
			rt << indent << "}" << ln << std::endl;
		}
		else
		{
			rt << indent << ln << std::endl;
		}
	}
	va_end (args);
	return rt.str();
}

std::vector<std::string> papuga::getGeneratorArguments(
	const std::multimap<std::string,std::string>& args,
	const char* name)
{
	typedef std::multimap<std::string,std::string>::const_iterator ArgIterator;
	std::pair<ArgIterator,ArgIterator> argrange = args.equal_range( name);
	ArgIterator ai = argrange.first, ae = argrange.second;
	std::vector<std::string> rt;
	for (; ai != ae; ++ai)
	{
		rt.push_back( ai->second);
	}
	return rt;
}

std::string papuga::getGeneratorArgument(
	const std::multimap<std::string,std::string>& args,
	const char* name,
	const char* defaultval)
{
	typedef std::multimap<std::string,std::string>::const_iterator ArgIterator;
	std::pair<ArgIterator,ArgIterator> argrange = args.equal_range( name);
	ArgIterator ai = argrange.first, ae = argrange.second;
	std::string rt;
	if (ai == ae)
	{
		if (defaultval)
		{
			rt = defaultval;
		}
		else
		{
			char buf[ 256];
			std::snprintf( buf, sizeof(buf), "missing definition of argument '%s'", name);
			throw std::runtime_error( buf);
		}
	}
	for (int aidx=0; ai != ae; ++ai,++aidx)
	{
		if (aidx)
		{
			char buf[ 256];
			std::snprintf( buf, sizeof(buf), "too many arguments with name '%s' defined", name);
			throw std::runtime_error( buf);
		}
		rt = ai->second;
	}
	return rt;
}

std::string papuga::readFile( const std::string& filename)
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


