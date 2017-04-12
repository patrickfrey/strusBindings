/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Code generator for strusBindings
/// \file strusTraceCodeGen.cpp
#include "interfaceParser.hpp"
#include "fillTypeTables.hpp"
#include "printFrame.hpp"
#include "strus/base/fileio.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

#define STRUS_LOWLEVEL_DEBUG

/// \brief List of interface files parsed without path
std::vector<std::string> g_inputFiles;

typedef void (*PrintInterface)( std::ostream& out, const strus::InterfacesDef& interfaceDef);

static void printOutput( const char* filename, PrintInterface print, const strus::InterfacesDef& interfaceDef)
{
	try
	{
		std::ofstream out;
		out.open( filename, std::ofstream::out);
		if (!out)
		{
			throw std::runtime_error( std::string("could not open file '") + filename + "' for writing: " + std::strerror( errno));
		}
		print( out, interfaceDef);
		out.close();
	}
	catch (const std::bad_alloc&)
	{
		throw std::runtime_error( std::string("out of memory writing file '") + filename + "'");
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string("could not write file '") + filename + "': " + err.what());
	}
	catch (const std::exception& err)
	{
		throw std::runtime_error( std::string("could not write file '") + filename + "'");
	}
}

static std::string expandIndent( const std::string& indent, const std::string& source)
{
	std::string rt ( indent);
	char const* si = source.c_str();
	for (; *si; ++si)
	{
		char const* seol = si;
		for (; *seol && *seol != '\n'; ++seol){}
		if (seol != si)
		{
			rt.append( "\n");
			++si;
		}
		else
		{
			rt.append( indent);
			rt.append( si, seol-si);
			if (!*seol) break;
			si = seol;
		}
	}
	return rt;
}

static void print_BindingObjectsHpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printHppFrameHeader( out, "bindingObjects", "Identifiers for objects and methods for serialization");
	out << "#include \"strus/bindings/valueVariant.hpp\"" << std::endl;
	out << "#include \"strus/bindings/callResult.hpp\"" << std::endl;
	out << "#include \"strus/bindings/serialization.hpp\"" << std::endl;
	out << "#include \"strus/bindings/hostObjectReference.hpp\"" << std::endl;
	out << "#include <cstddef>" << std::endl;
	out
		<< std::endl
		<< "namespace strus {" << std::endl
		<< "namespace bindings {" << std::endl
		<< std::endl;

	std::vector<strus::ClassDef>::const_iterator
		ci = interfaceDef.classDefs().begin(),
		ce = interfaceDef.classDefs().end();
	for (; ci != ce; ++ci)
	{
		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (; mi != me; ++mi)
		{
			out 
			<< "bool " << ci->name() << "__" << mi->name()
			<< "( const HostObjectReference& self, callResult& retval, "
			<< "std::size_t argc, ValueVariant* argv);" << std::endl;
		}
	}
	out
		<< std::endl
		<< "}}//namespace" << std::endl;
	strus::printHppFrameTail( out);
}

static void print_BindingObjectsCpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printCppFrameHeader( out, "bindingObjects", "Identifiers for objects and methods for serialization");
	out << "#include \"strus/bindingObjects.hpp\"" << std::endl;
	out << "#include \"strus/base/dll_tags.hpp\"" << std::endl;
	out
		<< std::endl
		<< "using namespace strus;" << std::endl
		<< std::endl;

	std::vector<strus::ClassDef>::const_iterator
		ci = interfaceDef.classDefs().begin(),
		ce = interfaceDef.classDefs().end();
	for (; ci != ce; ++ci)
	{
		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (; mi != me; ++mi)
		{
			out 
			<< "DLL_PUBLIC bool bindings::" << ci->name() << "__" << mi->name()
			<< "( const HostObjectReference& self, callResult& retval, "
			<< "std::size_t argc, ValueVariant* argv)" << std::endl;
			out
			<< "{" << std::endl
			<< "\t" << ci->name() << "Impl* THIS = self.getObject<" << ci->name() << "Impl>();" << std::endl;
			std::vector<strus::VariableValue>::const_iterator
				pi = mi->parameters().begin(),
				pe = mi->parameters().end();
			std::vector<bool> param_converted;
			for (int pidx=0; pi != pe; ++pi,++pidx)
			{
				char namebuf[ 128];
				char valuebuf[ 128];
				snprintf( namebuf, sizeof( namebuf), "p%d", pidx);
				snprintf( valuebuf, sizeof( valuebuf), "argv[ %d]", pidx);
				std::string param_decl( pi->expand( "param_decl", namebuf, valuebuf));
				if (param_decl.empty())
				{
					param_converted.push_back( false);
				}
				else
				{
					param_converted.push_back( true);
					out << expandIndent( "\t", param_decl);
				}
			}
			out
			<< "\tTHIS->" << mi->name() << "( ";
			pi = mi->parameters().begin();
			for (int pidx=0; pi != pe; ++pi,++pidx)
			{
				if (pidx)
				{
					out << ", ";
				}
				char namebuf[ 128];
				char valuebuf[ 128];
				snprintf( namebuf, sizeof( namebuf), "p%d", pidx);
				snprintf( valuebuf, sizeof( valuebuf), "argv[ %d]", pidx);
				std::string param_value( pi->expand( "param_value", namebuf, valuebuf));
				if (param_value.empty())
				{
					if (!param_converted[pidx])
					{
						throw std::runtime_error( std::string("member 'param_value' not defined for '") + valuebuf + "' in method '" + mi->name());
					}
					out << namebuf;
				}
				else
				{
					out << param_value;
				}
			}
			out
			<< ");" << std::endl;
			out
			<< "}" << std::endl;
		}
	}
}

int main( int argc, const char* argv[])
{
	int ec = 0;
	try
	{
		strus::TypeSystem typeSystem;
		strus::fillTypeTables( typeSystem);

#ifdef STRUS_LOWLEVEL_DEBUG
		std::cout << "TypeSystem:" << std::endl << typeSystem.tostring() << std::endl;
#endif
		strus::InterfacesDef interfaceDef( &typeSystem);
		int argi=1;
		for (; argi < argc; ++argi)
		{
			std::string source;
			ec = strus::readFile( argv[ argi], source);
			if (ec)
			{
				std::ostringstream msg;
				msg << ec;
				throw std::runtime_error( std::string( "error '") + ::strerror(ec) + "' (errno " + msg.str() + ") reading file " + argv[argi]);
			}
			try
			{
				char const* fi = argv[ argi];
				char const* fn = std::strchr( fi, strus::dirSeparator());
				for (;fn; fi=fn+1,fn=std::strchr( fi, strus::dirSeparator())){}

				g_inputFiles.push_back( fi);
				interfaceDef.addSource( source);
			}
			catch (const std::runtime_error& err)
			{
				throw std::runtime_error( std::string( "error parsing interface file ") + argv[argi] + ": " + err.what());
			}
			std::cerr << "processed file " << argv[argi] << std::endl;
		}
		interfaceDef.checkUnresolved();

		//Output:
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cout << interfaceDef.tostring() << std::endl;
#endif
		printOutput( "include/strus/bindingObjects.hpp", &print_BindingObjectsHpp, interfaceDef);
		printOutput( "src/bindingObjects.cpp", &print_BindingObjectsCpp, interfaceDef);

		std::cerr << "done." << std::endl;
		return 0;
	}
	catch (const std::bad_alloc&)
	{
		std::cerr << "ERROR memory allocation error in code generator" << std::endl;
		return 12/*ENOMEM*/;
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << "ERROR in code generator: " << err.what() << std::endl;
		return (ec == 0)?-1:ec;
	}
	catch (...)
	{
		std::cerr << "ERROR uncaught exception in code generator" << std::endl;
		return -1;
	}
}


