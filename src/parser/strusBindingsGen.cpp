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

static std::string methodFunctionName( const std::string& cl, const std::string& mt)
{
	return cl + "__" + mt;
}

static void print_BindingInterfaceDescriptionHpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printHppFrameHeader( out, "bindings_description", "Strus interface description used for generating language bindings");
	out << "#include \"papuga/languageInterface.hpp\"" << std::endl;
	out << "#include <cstddef>" << std::endl;
	out << std::endl
		<< "namespace strus {" << std::endl
		<< std::endl << std::endl;

	out << "const papuga::LanguageInterface::InterfaceDescription* getBindingsInterfaceDescription();"
		<< std::endl << std::endl;

	out << "}//namespace" << std::endl;
	strus::printHppFrameTail( out);
}

static void print_BindingInterfaceDescriptionCpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printCppFrameHeader( out, "bindings_description", "Strus interface description used for generating language bindings");
	out << "#include \"strus/lib/bindings_description.hpp\"" << std::endl;
	out << "#include \"strus/bindingClassId.hpp\"" << std::endl;
	out << "#include \"strus/base/dll_tags.hpp\"" << std::endl;
	out << "#include \"internationaliation.hpp\"" << std::endl;

	out << "#include <cstddef>" << std::endl;
	out
		<< std::endl
		<< "using namespace strus;" << std::endl
		<< std::endl << std::endl;

	std::vector<strus::ClassDef>::const_iterator
		ci = interfaceDef.classDefs().begin(),
		ce = interfaceDef.classDefs().end();
	for (; ci != ce; ++ci)
	{
		out << "static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_" << ci->name() << "[" << (ci->methodDefs().size()+1) << "] = " << std::endl
			<< "{" << std::endl;
		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (; mi != me; ++mi)
		{
			
			out << "\t{\"" << mi->name() <<  "\", \"" << methodFunctionName(ci->name(),mi->name()) << "\", " << mi->parameters().size() << "}," << std::endl;
		}
		out << "\t{0,0,0}" << std::endl;
		out << "};" << std::endl;
	}

	out << "static const papuga::LanguageInterface::InterfaceDescription::Class g_classes[" << (interfaceDef.classDefs().size()+1) << "] = " << std::endl
		<< "{" << std::endl;
	ci = interfaceDef.classDefs().begin();
	for (; ci != ce; ++ci)
	{
		std::string classid = std::string("Class") + ci->name();
		out << "\t{ " << classid << ", \"" << ci->name() << "\", g_methods_" << ci->name() << "}," << std::endl;
	}
	out << "\t{0,0}" << std::endl;
	out << "};" << std::endl << std::endl;

	out << "static const papuga::LanguageInterface::InterfaceDescription g_descr = { \"strus\", g_classes };"
		<< std::endl << std::endl;

	out << "DLL_PUBLIC const papuga::LanguageInterface::InterfaceDescription* strus::getBindingsInterfaceDescription()" << std::endl;
	out << "{" << std::endl;
	out << "\treturn &g_descr;" << std::endl;
	out << "}" << std::endl;
}

static void print_BindingObjectsHpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printHppFrameHeader( out, "bindingObjects", "Identifiers for objects and methods for serialization");
	out << "#include \"papuga/valueVariant.hpp\"" << std::endl;
	out << "#include \"papuga/callResult.hpp\"" << std::endl;
	out << "#include \"papuga/serialization.hpp\"" << std::endl;
	out << "#include \"papuga/hostObjectReference.hpp\"" << std::endl;
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
			<< "bool " << methodFunctionName( ci->name(), mi->name())
			<< "( void* self, papuga::CallResult& retval, "
			<< "std::size_t argc, papuga::ValueVariant const* argv);" << std::endl;
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
	out << "#include \"valueVariantConv.hpp\"" << std::endl;

	out << std::endl
		<< "using namespace strus;" << std::endl
		<< "using namespace strus::bindings;" << std::endl
		<< std::endl;

	out << "#define CATCH_METHOD_CALL_ERROR( retval, classnam, methodnam)\\" << std::endl;
	out << "\tcatch (const std::runtime_error& err)\\" << std::endl;
	out << "\t{\\" << std::endl;
	out << "\t\tretval.reportError( _TXT(\"error calling method %s::%s(): %s\"), classnam, methodnam, err.what());\\" << std::endl;
	out << "\t}\\" << std::endl;
	out << "\tcatch (const std::bad_alloc& err)\\" << std::endl;
	out << "\t{\\" << std::endl;
	out << "\t\tretval.reportError( _TXT(\"out of memory calling method %s::%s()\"), classnam, methodnam);\\" << std::endl;
	out << "\t}\\" << std::endl;
	out << "\tcatch (const std::exception& err)\\" << std::endl;
	out << "\t{\\" << std::endl;
	out << "\t\tretval.reportError( _TXT(\"uncaught exception calling method %s::%s(): %s\"), classnam, methodnam, err.what());\\" << std::endl;
	out << "\t}\\" << std::endl;
	out << "\treturn false;" << std::endl;

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
			out << "DLL_PUBLIC bool strus::" << ci->name() << "__" << mi->name()
				<< "( void* self, papuga::CallResult& retval, "
				<< "std::size_t argc, papuga::ValueVariant const* argv)" << std::endl;
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
			// Do some conversions, if needed:
			pi = mi->parameters().begin();
			for (std::size_t pidx=0; pi != pe; ++pi,++pidx)
			{
				if (pi->type().hasEvent( "argv_decl"))
				{
					std::string val = strus::string_format( "argv[%u]", (unsigned int)pidx);
					std::string nam = strus::string_format( "conv_argv%u", (unsigned int)pidx);
					out << "\t\t" << pi->expand( "argv_decl", nam.c_str(), val.c_str()) << std::endl;
				}
			}
			// Collect argument values:
			std::vector<std::string> params_converted;
			std::vector<std::string> params_default;
			pi = mi->parameters().begin();
			for (std::size_t pidx=0; pi != pe; ++pi,++pidx)
			{
				std::string val = strus::string_format( "argv[%u]", (unsigned int)pidx);
				std::string nam = strus::string_format( "conv_argv%u", (unsigned int)pidx);
				params_converted.push_back( pi->expand( "argv_map", nam.c_str(), val.c_str()));
				params_default.push_back( min_nofargs <= pidx ? pi->expand( "argv_default", nam.c_str(), val.c_str()) : std::string());
			}
			// Print redirect calls:
			if (mi->parameters().empty())
			{
				out << "\t\tif (argc > " << mi->parameters().size() << ") throw strus::runtime_error(_TXT(\"too many arguments\"));" << std::endl;
				if (mi->returnValue() == "void")
				{
					out << "\t\t" << expandMethodCall( mi->name(), 0, params_converted, params_default) << ";" << std::endl;
				}
				else
				{
					out << "\t\tretval = " << expandMethodCall( mi->name(), 0, params_converted, params_default) << ";" << std::endl;
				}
			}
			else
			{
				out << "\t\tswitch (argc)" << std::endl;
				out << "\t\t{" << std::endl;
				for (std::size_t pidx=min_nofargs; pidx<=mi->parameters().size(); ++pidx)
				{
					if (mi->returnValue() == "void")
					{
						out << "\t\t\tcase " << pidx << ": " << expandMethodCall( mi->name(), pidx, params_converted, params_default) << "; break;" << std::endl;
					}
					else
					{
						out << "\t\t\tcase " << pidx << ": retval = " << expandMethodCall( mi->name(), pidx, params_converted, params_default) << "; break;" << std::endl;
					}
				}
				out << "\t\t\tdefault: throw strus::runtime_error(_TXT(\"too many arguments\"));" << std::endl;
				out << "\t\t}" << std::endl;
			}
			out << "\t\treturn true;" << std::endl;
			out << "\t}" << std::endl;
			out << "\tCATCH_METHOD_CALL_ERROR( retval, \"" << ci->name().c_str() << "\", \"" << mi->name().c_str() << "\")" << std::endl;
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

		printOutput( "include/strus/lib/bindings_description.hpp", &print_BindingInterfaceDescriptionHpp, interfaceDef);
		printOutput( "src/libstrus_bindings_description.cpp", &print_BindingInterfaceDescriptionCpp, interfaceDef);

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


