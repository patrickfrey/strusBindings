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
#include "strus/base/string_format.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

#undef STRUS_LOWLEVEL_DEBUG

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
			<< "( void* self, CallResult& retval, "
			<< "std::size_t argc, ValueVariant const* argv);" << std::endl;
		}
	}
	out
		<< std::endl
		<< "}//namespace" << std::endl;
	strus::printHppFrameTail( out);
}

static std::string expandMethodCall(
	const std::string& methodname,
	std::size_t nofargs,
	const std::vector<std::string>& params_converted,
	const std::vector<std::string>& params_default)
{
	std::ostringstream out;
	out << "THIS->" << methodname << "(";
	std::size_t pi = 0, pe = params_converted.size();
	for (; pi != pe; ++pi)
	{
		if (pi) out << ", "; else out << " ";
		out << ((pi >= nofargs) ? params_default[ pi] : params_converted[ pi]);
	}
	out << ")";
	return out.str();
}

static void print_BindingObjectsCpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printCppFrameHeader( out, "bindingObjects", "Identifiers for objects and methods for serialization");
	out << "#include \"strus/bindingObjects.hpp\"" << std::endl;
	out << "#include \"strus/base/dll_tags.hpp\"" << std::endl;
	out << "#include \"impl/context.hpp\"" << std::endl;
	out << "#include \"impl/storage.hpp\"" << std::endl;
	out << "#include \"impl/vector.hpp\"" << std::endl;
	out << "#include \"impl/analyzer.hpp\"" << std::endl;
	out << "#include \"impl/query.hpp\"" << std::endl;
	out << "#include \"impl/statistics.hpp\"" << std::endl;
	out << "#include \"internationalization.hpp\"" << std::endl;

	out << std::endl
		<< "using namespace strus;" << std::endl
		<< "using namespace strus::bindings;" << std::endl
		<< std::endl;

	out << "#define CATCH_METHOD_CALL_ERROR( retval, classname, methodname)\\" << std::endl;
	out << "\tcatch (const std::runtime_error& err)\\" << std::endl;
	out << "\t{\\" << std::endl;
	out << "\t\tretval.reportError( _TXT(\"error calling method %s::%s(): %s\"), classnam.c_str(), methodnam.c_str(), err.what());\\" << std::endl;
	out << "\t} catch (const std::bad_alloc& err)\\" << std::endl;
	out << "\t{\\" << std::endl;
	out << "\t\tretval.reportError( _TXT(\"out of memory calling method %s::%s()\"), classnam.c_str(), methodnam.c_str());\\" << std::endl;
	out << "\t}\\" << std::endl;
	out << "\t} catch (const std::exception& err)\\" << std::endl;
	out << "\t{\\" << std::endl;
	out << "\t\tretval.reportError( _TXT(\"uncaught exception calling method %s::%s(): %s\"), classnam.c_str(), methodnam.c_str(), err.what());\\" << std::endl;
	out << "\t}\\" << std::endl;
	out << "\treturn false;\\" << std::endl;
	out << "}" << std::endl << std::endl;
	
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
			out << "DLL_PUBLIC bool bindings::" << ci->name() << "__" << mi->name()
				<< "( void* self, CallResult& retval, "
				<< "std::size_t argc, ValueVariant const* argv)" << std::endl;
			out << "{" << std::endl;
			out << "\ttry {" << std::endl;
			out << "\t\t" << ci->name() << "Impl* THIS = ("<< ci->name() << "Impl*)(self);" << std::endl;

			std::size_t min_nofargs = 0;
			std::vector<strus::VariableValue>::const_iterator
				pi = mi->parameters().begin(),
				pe = mi->parameters().end();
			for (; pi != pe; ++pi,++min_nofargs)
			{
				if (pi->type().hasEvent( "argv_default")) break;
			}
			if (min_nofargs)
			{
				out << "\t\tif (argc < " << min_nofargs << ") throw strus::runtime_error(_TXT(\"too few arguments\"));" << std::endl;
			}

			// Collect argument values:
			std::vector<std::string> params_converted;
			std::vector<std::string> params_default;
			pi = mi->parameters().begin();
			for (std::size_t pidx=0; pi != pe; ++pi,++pidx)
			{
				std::string val = strus::string_format( "argv[%u]", (unsigned int)pidx);
				params_converted.push_back( pi->expand( "argv_map", "", val.c_str()));
				params_default.push_back( min_nofargs <= pidx ? pi->expand( "argv_default", "", val.c_str()) : std::string());
			}
			// Print redirect calls:
			if (mi->parameters().empty())
			{
				out << "\t\tif (argc > " << mi->parameters().size() << ") throw strus::runtime_error(_TXT(\"too many arguments\"));" << std::endl;
				out << "\t\tretval = " << expandMethodCall( mi->name(), 0, params_converted, params_default) << ";" << std::endl;
			}
			else
			{
				out << "\t\tswitch (argc)" << std::endl;
				out << "\t\t{" << std::endl;
				for (std::size_t pidx=min_nofargs; pidx<=mi->parameters().size(); ++pidx)
				{
					out << "\t\t\tcase " << pidx << ": retval = " << expandMethodCall( mi->name(), pidx, params_converted, params_default) << "; break;" << std::endl;
				}
				out << "\t\t\tdefault: throw strus::runtime_error(_TXT(\"too many arguments\"));" << std::endl;
				out << "\t\t}" << std::endl;
			}
			out << "\t\treturn true;" << std::endl;
			out << "\t}" << std::endl;
			out << "\tCATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());" << std::endl;
			out << "}" << std::endl << std::endl;
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


