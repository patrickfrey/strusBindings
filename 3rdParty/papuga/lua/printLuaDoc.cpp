/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Module for printing the lua documentation (LDoc - ex. LuaDoc)
/// \file printLuaDoc.cpp
#include "printLuaDoc.hpp"
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>

using namespace papuga;

static void printDocumentationTagList(
		std::ostream& out,
		const char* tag,
		const char* value)
{
	if (!value || !value[0]) return;
	char const* si = value;
	char const* sn;
	while (0!=(sn = std::strchr( si,'\n')))
	{
		out << "-- @" << tag << std::string( si, sn-si) << std::endl;
		si = sn+1;
	}
	out << "-- @" << tag << std::string( si) << std::endl;
}

static void printDocumentationHdr(
		std::ostream& out,
		const char* tag,
		const std::string& value)
{
	if (tag)
	{
		out << "-- @" << tag << " " << value << std::endl;
	}
	else
	{
		out << "--- " << value << std::endl;
	}
}

static void printDocumentationTag(
		std::ostream& out,
		const char* tag,
		const char* value)
{
	if (!value || !value[0]) return;
	char const* si = value;
	char const* sn = std::strchr( si,'\n');
	if (sn)
	{
		printDocumentationHdr( out, tag, std::string( si, sn-si));
		for (sn = std::strchr( si=sn+1,'\n'); sn; sn = std::strchr( si=sn+1,'\n'))
		{
			out << "-- " << std::string( si, sn-si) << std::endl;
		}
		out << "-- " << std::string( si) << std::endl;
	}
	else
	{
		printDocumentationHdr( out, tag, si);
	}
}

static const char* skipSpaces( char const* ei)
{
	for (; *ei && *ei != '\n' && (unsigned char)*ei <= 32; ++ei){}
	return ei;
}

static void printCodeSnippetSeparator( std::ostream& out, char const* ei)
{
	ei = skipSpaces( ei);
	if (*ei && *ei != '(' && *ei != ')' && *ei != ']' && *ei != ',' && *ei != '\n')
	{
		out << ", ";
	}
}

static void printCodeSnippet( std::ostream& out, const char* tag, const char* examples)
{
	if (!examples || !examples[0]) return;
	unsigned int bcnt = 0;
	unsigned int ocnt = 0;
	char const* ei = examples;
	if (tag) { out << "-- @" << tag << " "; } else {out << "--- ";}
	while (*ei)
	{
		ei = skipSpaces( ei);
		if (!*ei) break;
		if (*ei == '\n')
		{
			++ei;
			out << std::endl;
			if (tag) { out << "-- @" << tag << " "; } else {out << "--- ";}
		}
		else if (*ei == '(')
		{
			ei = skipSpaces( ei+1);
			++ocnt;
			out << "(";
		}
		else if (*ei == ')')
		{
			ei = skipSpaces( ei+1);
			--ocnt;
			out << ")";
			printCodeSnippetSeparator( out, ei);
		}
		else if (*ei == '[')
		{
			ei = skipSpaces( ei+1);
			++bcnt;
			out << "{";
		}
		else if (*ei == ']')
		{
			ei = skipSpaces( ei+1);
			--bcnt;
			out << "}";
			printCodeSnippetSeparator( out, ei);
		}
		else if (*ei == '\"' || *ei == '\'')
		{
			const char* start = ei;
			char eb = *ei;
			++ei;
			for (; *ei && *ei != eb && *ei != '\n'; ++ei)
			{
				if (*ei == '\\' && *(ei+1)) ++ei;
			}
			out << std::string( start, ei-start) << (char)eb;
			if (*ei != eb) throw std::runtime_error("string not correctly terminated");
			++ei;
			printCodeSnippetSeparator( out, ei);
		}
		else if (*ei == '-' || (*ei >= '0' && *ei <= '9'))
		{
			out << *ei++;
			for (; *ei >= '0' && *ei <= '9'; ++ei) out << *ei;
			if (*ei == '.') out << *ei++;
			for (; *ei >= '0' && *ei <= '9'; ++ei) out << *ei;
			if (*ei == 'E') out << *ei++;
			if (*ei == '-') out << *ei++;
			for (; *ei >= '0' && *ei <= '9'; ++ei) out << *ei;
			printCodeSnippetSeparator( out, ei);
		}
		else if (*ei == '_' || ((*ei|32) >= 'a' && (*ei|32) <= 'z'))
		{
			out << *ei++;
			for (; (*ei >= '0' && *ei <= '9') || *ei == '_' || ((*ei|32) >= 'a' && (*ei|32) <= 'z') ; ++ei) out << *ei;
			for (; *ei && *ei <= 32; ++ei){}
			if (*ei == ':' || *ei == '=')
			{
				out << '=';
				++ei;
			}
			else
			{
				printCodeSnippetSeparator( out, ei);
			}
		}
		else if (*ei == ',')
		{
			out << ", ";
			ei = skipSpaces( ei+1);
		}
		else
		{
			char buf[ 1024];
			std::snprintf( buf, sizeof(buf), "syntax error in example, unexpected token '%c'", *ei);
			throw std::runtime_error( buf);
		}
	}
	out << std::endl;
}

static void printParameterDescription(
		std::ostream& out,
		const papuga_ParameterDescription* parameter)
{
	if (!parameter) return;
	papuga_ParameterDescription const* pi = parameter;
	for (; pi->name; ++pi)
	{
		char buf[ 4096];
		std::snprintf( buf, sizeof(buf), "%s %s%s",
				pi->name,
				pi->mandatory?"":"(optional) ",
				pi->description);
		printDocumentationTag( out, "param", buf);
		printCodeSnippet( out, "usage", pi->examples);
	}
}

static void printParameterList(
		std::ostream& out,
		const papuga_ParameterDescription* parameter)
{
	if (!parameter) return;
	papuga_ParameterDescription const* pi = parameter;
	for (int pidx=0; pi->name; ++pi,++pidx)
	{
		if (pidx) out << ",";
		out << pi->name;
	}
}

static std::string methodName( bool withself, const char* classname, const char* methodname)
{
	return std::string() + classname + (withself?":":".") + methodname;
}

static void printConstructor(
		std::ostream& out,
		const char* classname,
		const papuga_ConstructorDescription* cdef)
{
	if (!cdef) return;
	std::string methodname = methodName( false, classname, "new");
	printDocumentationTag( out, "constructor", "new");
	printDocumentationTag( out, "brief", cdef->description);
	printParameterDescription( out, cdef->parameter);
	printCodeSnippet( out, "usage", cdef->examples);
	out << "function " << methodname << "(";
	printParameterList( out, cdef->parameter);
	out << ")" << std::endl << "end" << std::endl << std::endl;
}

static void printMethod(
		std::ostream& out,
		const char* classname,
		const papuga_MethodDescription* mdef)
{
	if (!mdef) return;
	std::string methodname = methodName( mdef->self, classname, mdef->name);
	printDocumentationTag( out, "method", mdef->name);
	printDocumentationTag( out, "brief", mdef->description);
	printParameterDescription( out, mdef->parameter);
	printCodeSnippet( out, "usage", mdef->examples);
	out << "function " << methodname << "(";
	printParameterList( out, mdef->parameter);
	out << ")" << std::endl << "end" << std::endl << std::endl;
}

void papuga::printLuaDoc(
		std::ostream& out,
		const papuga_InterfaceDescription& descr)
{
	out << "-------" << std::endl;
	printDocumentationTag( out, "project", descr.name);
	if (descr.about)
	{
		printDocumentationTagList( out, "author ", descr.about->authors);
		printDocumentationTag( out, "copyright", descr.about->copyright);
		printDocumentationTag( out, "license", descr.about->license);
		printDocumentationTag( out, "release", descr.about->version);
	}
	std::size_t ci;
	for (ci=0; descr.classes[ci].name; ++ci)
	{
		const papuga_ClassDescription& classdef = descr.classes[ci];
		char classname[ 1024];
		std::snprintf( classname, sizeof(classname), "%s_%s", descr.name, classdef.name);

		printDocumentationTag( out, "class", classname);
		printDocumentationTag( out, "brief", classdef.description);

		printConstructor( out, classname, classdef.constructor);
		std::size_t mi = 0;
		for (; classdef.methodtable[mi].name; ++mi)
		{
			printMethod( out, classname, &classdef.methodtable[mi]);
		}
	}
}


