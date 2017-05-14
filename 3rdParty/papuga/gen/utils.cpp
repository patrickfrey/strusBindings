/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Some utility functions for generating language binding sources
/// \file gen/utils.cpp
#include "private/gen_utils.hpp"
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <stdint.h>

using namespace papuga;

std::runtime_error papuga::runtime_error( const char* msg, ...)
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

ClassDescriptionMap papuga::getClassDescriptionMap( const papuga_InterfaceDescription& descr)
{
	ClassDescriptionMap rt;
	std::size_t ci = 0;
	for (; descr.classes[ci].name; ++ci)
	{
		const char* cname = descr.classes[ci].name;
		unsigned int cid = descr.classes[ci].id;
		if (!cid) throw std::runtime_error( "class id must be non zero");
		ClassDescriptionMap::const_iterator mi = rt.find( cid);
		if (mi != rt.end())
		{
			throw papuga::runtime_error( "duplicate definition of class id %u (%s,%s)", cid, mi->second->name, cname);
		}
		rt[ descr.classes[ci].id] = &descr.classes[ci];
	}
	unsigned int max_classid = rt.size();
	ClassDescriptionMap::const_iterator mi = rt.begin(), me = rt.end();
	for (; mi != me; ++mi)
	{
		if (mi->first > max_classid)
		{
			throw papuga::runtime_error( "class id map has gaps");
		}
	}
	return rt;
}



