/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Code generator for the papuga interface of strusBindings
/// \file strusBindingsInterfaceGen.cpp
#include "interfaceParser.hpp"
#include "fillTypeTables.hpp"
#include "printFrame.hpp"
#include "strus/versionBindings.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/string_format.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

#define STRUS_BINDINGS_PRODUCT_NAME		"Strus"
#define STRUS_BINDINGS_PRODUCT_DESCRIPTION	"A set of libraries and tools to implement a text search engine"
#define STRUS_BINDINGS_AUTHOR			"Patrick P. Frey"
#define STRUS_BINDINGS_CONTRIBUTORS		""
#define STRUS_BINDINGS_COPYRIGHT		"Patrick P. Frey"
#define STRUS_BINDINGS_LICENSE			"Mozilla Public License v. 2.0 (MPLv2)"
#define STRUS_BINDINGS_URL			"project-strus.net"

/// \brief List of interface files parsed without path
std::vector<std::string> g_inputFiles;

typedef void (*PrintInterface)( std::ostream& out, const strus::InterfacesDef& interfaceDef);

static void printOutput( const std::string& filename, PrintInterface print, const strus::InterfacesDef& interfaceDef)
{
	try
	{
		std::ofstream out;
		out.open( filename.c_str(), std::ofstream::out);
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

static std::string printString( const std::string& content)
{
	std::string rt;
	std::string::const_iterator si = content.begin(), se = content.end();
	for (; si != se; ++si)
	{
		if (*si == '\n')
		{
			rt.push_back( '\\');
			rt.push_back( 'n');
		}
		else if ((unsigned char)*si <= 32)
		{
			rt.push_back( ' ');
		}
		else if (*si == '\"')
		{
			rt.push_back( '\\');
			rt.push_back( '"');
		}
		else
		{
			rt.push_back( *si);
		}
	}
	return rt;
}

static std::string structMembersFunctionName( const std::string& st)
{
	return "strus_bindings_struct_" + st;
}

static std::string methodFunctionName( const std::string& cl, const std::string& mt)
{
	return "strus_bindings_method_" + cl + "__" + mt;
}

static std::string constructorFunctionName( const std::string& cl)
{
	return "strus_bindings_constructor_" + cl;
}

static std::string destructorFunctionName( const std::string& cl)
{
	return "strus_bindings_destructor_" + cl;
}

static std::string docDeclaration( const std::string& globalname, const strus::DocTagMap& doc)
{
	std::ostringstream out;
	out << "static const papuga_Annotation " << globalname << "[] = " << std::endl
		<< "{" << std::endl;
	strus::DocTagMap::const_iterator di = doc.begin(), de = doc.end();
	for (; di != de; ++di)
	{
		std::string anntag;
		if (di->first == "brief")
		{
			anntag = "papuga_AnnotationType_Description";
		}
		else if (di->first == "example")
		{
			anntag = "papuga_AnnotationType_Example";
		}
		else if (di->first == "note")
		{
			anntag = "papuga_AnnotationType_Note";
		}
		else if (di->first == "remark")
		{
			anntag = "papuga_AnnotationType_Remark";
		}
		else
		{
			throw std::runtime_error( std::string("unknown annotation tag '") + di->first + "'");
		}
		out << "\t{" << anntag << ", \"" << printString(di->second) << "\"}," << std::endl;
	}
	out << "\t{(papuga_AnnotationType)0,NULL}" << std::endl;
	out << "};" << std::endl;
	return out.str();
}

static std::string docDeclarationNameMethodReturnDoc( const std::string& clname, const std::string& mtname)
{
	std::ostringstream out;
	out << "g_doc_return_method_" << "_" << clname << "__" << mtname;
	return out.str();
}

static std::string docDeclarationNameMethodParamDoc( const std::string& clname, const std::string& mtname, unsigned int idx)
{
	std::ostringstream out;
	out << "g_doc_parameter_method_" << idx << "_" << clname << "__" << mtname;
	return out.str();
}

static std::string docDeclarationNameMethodParam( const std::string& clname, const std::string& mtname)
{
	std::ostringstream out;
	out << "g_parameter_method_" << clname << "_" << mtname;
	return out.str();
}

static std::string docDeclarationNameConstructorParamDoc( const std::string& clname, unsigned int idx)
{
	std::ostringstream out;
	out << "g_doc_parameter_constructor_" << idx << "__" << clname;
	return out.str();
}

static std::string docDeclarationNameConstructorParam( const std::string& clname)
{
	std::ostringstream out;
	out << "g_parameter_constructor_" << clname;
	return out.str();
}

static std::string docDeclarationNameMethodDoc( const std::string& clname, const std::string& mtname)
{
	std::ostringstream out;
	out << "g_doc_method_" << clname << "__" << mtname;
	return out.str();
}

static std::string docDeclarationNameConstructorDoc( const std::string& clname)
{
	std::ostringstream out;
	out << "g_doc_constructor_" << clname;
	return out.str();
}

static std::string docDeclarationNameClassDoc( const std::string& clname)
{
	std::ostringstream out;
	out << "g_doc_class_" << clname;
	return out.str();
}

static std::string docDeclarationNameStructDoc( const std::string& stname)
{
	std::ostringstream out;
	out << "g_doc_struct_" << stname;
	return out.str();
}

static std::string docDeclarationNameStructMemberDoc( const std::string& stname, const std::string& mbname)
{
	std::ostringstream out;
	out << "g_doc_struct_member_" << stname << "__" << mbname;
	return out.str();
}

static std::string variableValueDeclaration( const std::string& indent, const std::string& docname, const strus::VariableValue& var)
{
	std::ostringstream out;
	if (var.name().empty())
	{
		throw std::runtime_error("empty parameter declaration");
	}
	bool optional = var.type().hasEvent( "argv_default");
	out << indent << "{" << std::endl;
	out << indent << "\t\"" << var.name() << "\"," << std::endl;
	out << indent << "\t" << docname << "," << std::endl;
	out << indent << "\t" << (optional ? "false":"true") << std::endl;
	out << indent << "}";
	return out.str();
}

static std::string methodParameterDeclaration( const std::string& clname, const std::string& mtname, const std::vector<strus::VariableValue>& parameters)
{
	std::ostringstream out;
	
	std::vector<strus::VariableValue>::const_iterator
		pi = parameters.begin(), pe = parameters.end();
	for (unsigned int pidx=0; pi != pe; ++pi,++pidx)
	{
		std::string docglobalname = docDeclarationNameMethodParamDoc( clname, mtname, pidx);
		out << docDeclaration( docglobalname, pi->doc());
	}
	out << "static const papuga_ParameterDescription " << docDeclarationNameMethodParam( clname, mtname) << "[] = " << std::endl
		<< "{" << std::endl;
	pi = parameters.begin();
	for (unsigned int pidx=0; pi != pe; ++pi,++pidx)
	{
		std::string docglobalname = docDeclarationNameMethodParamDoc( clname, mtname, pidx);
		out << variableValueDeclaration( "\t", docglobalname, *pi) << "," << std::endl;
	}
	out << "\t{NULL,NULL,false}" << std::endl;
	out << "};" << std::endl;
	return out.str();
}

static std::string constructorParameterDeclaration( const std::string& clname, const std::vector<strus::VariableValue>& parameters)
{
	std::ostringstream out;
	
	std::vector<strus::VariableValue>::const_iterator
		pi = parameters.begin(), pe = parameters.end();
	for (unsigned int pidx=0; pi != pe; ++pi,++pidx)
	{
		std::string docglobalname = docDeclarationNameConstructorParamDoc( clname, pidx);
		out << docDeclaration( docglobalname, pi->doc());
	}
	out << "static const papuga_ParameterDescription " << docDeclarationNameConstructorParam( clname) << "[] = " << std::endl
		<< "{" << std::endl;
	pi = parameters.begin();
	for (unsigned int pidx=0; pi != pe; ++pi,++pidx)
	{
		std::string docglobalname = docDeclarationNameConstructorParamDoc( clname, pidx);
		out << variableValueDeclaration( "\t", docglobalname, *pi) << "," << std::endl;
	}
	out << "\t{NULL,NULL,false}" << std::endl;
	out << "};" << std::endl;
	return out.str();
}

static void print_BindingInterfaceDescriptionHpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printHppFrameHeader( out, "bindings_description", "Strus interface description used for generating language bindings");
	out << "#include \"papuga/interfaceDescription.h\"" << std::endl;
	out << "#include <cstddef>" << std::endl;
	out << std::endl
		<< "namespace strus {" << std::endl
		<< std::endl << std::endl;

	out << "const papuga_InterfaceDescription* getBindingsInterfaceDescription();"
		<< std::endl << std::endl;

	out << "}//namespace" << std::endl;
	strus::printHppFrameTail( out);
}

static void print_BindingInterfaceDescriptionCpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printCppFrameHeader( out, "bindings_description", "Strus interface description used for generating language bindings");
	out << "#include \"strus/lib/bindings_description.hpp\"" << std::endl;
	out << "#include \"strus/base/dll_tags.hpp\"" << std::endl;
	out << "#include \"internationalization.hpp\"" << std::endl;
	out << "#include \"papuga/typedefs.h\"" << std::endl;

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
		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (; mi != me; ++mi)
		{
			out << methodParameterDeclaration( ci->name(), mi->name(), mi->parameters());

			if (!mi->returnValue().expand( "typename").empty())
			{
				std::string docreturn = docDeclarationNameMethodReturnDoc( ci->name(), mi->name());
				out << docDeclaration( docreturn, mi->returnValue().doc());

				out << "static const papuga_CallResultDescription g_result_method_" << ci->name() << "_" << mi->name() << " = " << std::endl
					<< "{" << docreturn << "};" << std::endl;
			}
			std::string docmethod = docDeclarationNameMethodDoc( ci->name(), mi->name());
			out << docDeclaration( docmethod, mi->doc());
		}
	}

	ci = interfaceDef.classDefs().begin();
	for (; ci != ce; ++ci)
	{
		out << "static const papuga_MethodDescription g_methods_" << ci->name() << "[" << (ci->methodDefs().size()+1) << "] = " << std::endl
			<< "{" << std::endl;
		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (; mi != me; ++mi)
		{
			std::string docmethod = docDeclarationNameMethodDoc( ci->name(), mi->name());
			std::string docparam = docDeclarationNameMethodParam( ci->name(), mi->name());
			out << "\t" << "{\""
				<< mi->name() <<  "\", "
				<< "\"" << methodFunctionName(ci->name(),mi->name()) << "\", "
				<< docmethod << ", ";
			if (!mi->returnValue().expand( "typename").empty())
			{
				out << "&g_result_method_" << ci->name() << "_" << mi->name() << ", ";
			}
			else
			{
				out << "NULL, ";
			}
			out << "true, " << docparam << "}," << std::endl;
		}
		out << "\t" << "{NULL,NULL,NULL,NULL,false,NULL}" << std::endl;
		out << "};" << std::endl;
	}
	ci = interfaceDef.classDefs().begin();
	for (; ci != ce; ++ci)
	{
		if (!ci->constructorDefs().empty())
		{
			const strus::ConstructorDef& cdef = ci->constructorDefs()[ 0];
			out << constructorParameterDeclaration( ci->name(), cdef.parameters());

			std::string docmethod = docDeclarationNameConstructorDoc( ci->name());
			std::string docparam = docDeclarationNameConstructorParam( ci->name());

			out << docDeclaration( docmethod, cdef.doc());

			out << "static const papuga_ConstructorDescription g_constructor_" << ci->name() << " = " << std::endl
				<< "{" << std::endl
				<< "\t\"" << constructorFunctionName(ci->name()) << "\"," << std::endl
				<< "\t" << docmethod << "," << std::endl
				<< "\t" << docparam << std::endl
				<< "};" << std::endl << std::endl;
		}
	}
	ci = interfaceDef.classDefs().begin();
	for (; ci != ce; ++ci)
	{
		out << docDeclaration( docDeclarationNameClassDoc( ci->name()), ci->doc());
	}
	out << "static const papuga_ClassDescription g_classes[" << (interfaceDef.classDefs().size()+1) << "] = " << std::endl
		<< "{" << std::endl;
	ci = interfaceDef.classDefs().begin();
	for (; ci != ce; ++ci)
	{
		std::string constructordescr = ci->constructorDefs().empty()?std::string("NULL"):(std::string("&g_constructor_") + ci->name());

		out << "\t" << "{ "
			<< "\"" << ci->name()
			<< "\", " << docDeclarationNameClassDoc( ci->name())
			<< ", " << constructordescr
			<< ", \"" << destructorFunctionName(ci->name())
			<< "\", g_methods_" << ci->name() << "}," << std::endl;
	}
	out << "\t" << "{0,NULL}" << std::endl;
	out << "};" << std::endl << std::endl;

	out << "static const char* g_includefiles[2] = {\"strus/bindingObjects.h\", 0};"
		<< std::endl << std::endl;

	std::vector<strus::StructDef>::const_iterator
		si = interfaceDef.structDefs().begin(),
		se = interfaceDef.structDefs().end();
	for (; si != se; ++si)
	{
		std::vector<strus::MemberDef>::const_iterator
			mi = si->memberDefs().begin(),
			me = si->memberDefs().end();
		for (; mi != me; ++mi)
		{
			out << docDeclaration( docDeclarationNameStructMemberDoc( si->name(), mi->name()), mi->doc());
		}
	}
	si = interfaceDef.structDefs().begin();
	for (; si != se; ++si)
	{
		out << "static const papuga_StructMemberDescription g_struct_members_" << si->name() << "[" << (si->memberDefs().size()+1) << "] = " << std::endl
			<< "{" << std::endl;
		std::vector<strus::MemberDef>::const_iterator
			mi = si->memberDefs().begin(),
			me = si->memberDefs().end();
		for (; mi != me; ++mi)
		{
			std::string docmember = docDeclarationNameStructMemberDoc( si->name(), mi->name());
			out << "\t" << "{\"" << mi->name() << "\", " << docmember << "}," << std::endl;
		}
		out << "\t" << "{NULL,NULL}" << std::endl;
		out << "};" << std::endl;
	}
	out << std::endl;
	si = interfaceDef.structDefs().begin();
	for (; si != se; ++si)
	{
		out << docDeclaration( docDeclarationNameStructDoc( si->name()), si->doc());
	}
	out << std::endl;
	out << "static const papuga_StructInterfaceDescription g_structs[" << (interfaceDef.structDefs().size()+1) << "] = " << std::endl
		<< "{" << std::endl;
	si = interfaceDef.structDefs().begin();
	for (; si != se; ++si)
	{
		out << "\t" << "{\"" << si->name() << "\", " << docDeclarationNameStructDoc( si->name()) << ", g_struct_members_" << si->name() << "}," << std::endl;
	}
	out << "\t" << "{NULL,NULL,NULL}" << std::endl;
	out << "};" << std::endl << std::endl;

	out << "static const papuga_AboutDescription g_about = {"
		<< "\"" << STRUS_BINDINGS_AUTHOR << "\","
		<< "\"" << STRUS_BINDINGS_CONTRIBUTORS << "\","
		<< "\"" << STRUS_BINDINGS_COPYRIGHT << "\","
		<< "\"" << STRUS_BINDINGS_LICENSE << "\","
		<< "\"" << STRUS_BINDINGS_VERSION_STRING << "\","
		<< "\"" << STRUS_BINDINGS_URL << "\","
		<< "};" << std::endl << std::endl;

	out << "static const papuga_InterfaceDescription g_descr = { \"" << STRUS_BINDINGS_PRODUCT_NAME << "\",\"" << STRUS_BINDINGS_PRODUCT_DESCRIPTION << "\", g_includefiles, g_classes, g_structs, &g_about };"
		<< std::endl << std::endl;

	out << "DLL_PUBLIC const papuga_InterfaceDescription* strus::getBindingsInterfaceDescription()" << std::endl;
	out << "{" << std::endl;
	out << "\t" << "return &g_descr;" << std::endl;
	out << "}" << std::endl;
}

static void print_BindingObjectsH( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printHFrameHeader( out, "bindingObjects", "Identifiers for objects and methods for serialization");
	out << "#include \"papuga/typedefs.h\"" << std::endl;
	out << "#include <stddef.h>" << std::endl;
	out << "#include <stdbool.h>" << std::endl;

	out << "#ifdef __cplusplus" << std::endl;
	out << "extern \"C\" {" << std::endl;
	out << "#endif" << std::endl << std::endl;

	std::vector<strus::ClassDef>::const_iterator
		ci = interfaceDef.classDefs().begin(),
		ce = interfaceDef.classDefs().end();
	int cidx = 0;
	for (; ci != ce; ++ci,++cidx)
	{
		out << "#define STRUS_BINDINGS_CLASSID_" << ci->name() << " " << (cidx+1) << std::endl;
	}
	out << "#define STRUS_BINDINGS_NOF_CLASSES " << cidx << std::endl;
	out << std::endl;

	std::vector<strus::StructDef>::const_iterator
		si = interfaceDef.structDefs().begin(),
		se = interfaceDef.structDefs().end();
	int sidx = 0;
	for (; si != se; ++si,++sidx)
	{
		out << "#define STRUS_BINDINGS_STRUCTID_" << si->name() << " " << (sidx+1) << std::endl;
	}
	out << "#define STRUS_BINDINGS_NOF_STRUCTS " << sidx << std::endl;
	out << std::endl;

	si = interfaceDef.structDefs().begin();
	for (; si != se; ++si)
	{
		out << "const char** " << structMembersFunctionName( si->name()) << "();" << std::endl;
	}
	out << std::endl;

	ci = interfaceDef.classDefs().begin();
	for (; ci != ce; ++ci)
	{
		out << "void " << destructorFunctionName( ci->name()) << "( void* self);" << std::endl;

		if (ci->constructorDefs().size() > 1) throw std::runtime_error( std::string("only one constructor allowed for '") + ci->name() + "'");
		std::vector<strus::ConstructorDef>::const_iterator
			ki = ci->constructorDefs().begin(),
			ke = ci->constructorDefs().end();
		for (; ki != ke; ++ki)
		{
			out 
			<< "void* " << constructorFunctionName( ci->name())
			<< "( papuga_ErrorBuffer* errbuf, size_t argc, const papuga_ValueVariant* argv);" << std::endl;
		}

		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (; mi != me; ++mi)
		{
			out 
			<< "bool " << methodFunctionName( ci->name(), mi->name())
			<< "( void* self, papuga_CallResult* retval, "
			<< "size_t argc, const papuga_ValueVariant* argv);" << std::endl;
		}
		out << std::endl;
	}
	out << "#ifdef __cplusplus" << std::endl;
	out << "}" << std::endl;
	out << "#endif" << std::endl;

	strus::printHFrameTail( out);
}

struct ParameterSructureExpanded
{
	std::size_t min_nofargs;
	std::vector<std::string> param_converted;
	std::vector<std::string> param_defaults;
	std::vector<std::string> param_declarations;

	std::size_t size() const	{return param_converted.size();}

	ParameterSructureExpanded( const ParameterSructureExpanded& o)
		:min_nofargs(o.min_nofargs),param_converted(o.param_converted),param_defaults(o.param_defaults),param_declarations(o.param_declarations){}
	ParameterSructureExpanded()
		:min_nofargs(0){}
	ParameterSructureExpanded( const std::vector<strus::VariableValue>& parameters)
		:min_nofargs(0)
	{
		std::vector<strus::VariableValue>::const_iterator
			pi = parameters.begin(),
			pe = parameters.end();
		for (; pi != pe; ++pi,++min_nofargs)
		{
			if (pi->type().hasEvent( "argv_default")) break;
		}
		// Declare some conversions, if needed:
		pi = parameters.begin();
		for (std::size_t pidx=0; pi != pe; ++pi,++pidx)
		{
			if (pi->type().hasEvent( "argv_decl"))
			{
				std::string val = strus::string_format( "argv[%u]", (unsigned int)pidx);
				std::string nam = strus::string_format( "conv_argv%u", (unsigned int)pidx);
				param_declarations.push_back( pi->expand( "argv_decl", nam.c_str(), val.c_str()));
			}
			else
			{
				param_declarations.push_back(std::string());
			}
		}
		// Collect argument values:
		std::vector<std::string> params_converted;
		std::vector<std::string> params_default;
		pi = parameters.begin();
		for (std::size_t pidx=0; pi != pe; ++pi,++pidx)
		{
			std::string val = strus::string_format( "argv[%u]", (unsigned int)pidx);
			std::string nam = strus::string_format( "conv_argv%u", (unsigned int)pidx);
			param_converted.push_back( pi->expand( "argv_map", nam.c_str(), val.c_str()));
			param_defaults.push_back( min_nofargs <= pidx ? pi->expand( "argv_default", nam.c_str(), val.c_str()) : std::string());
		}
	}

	std::string expandCallParameter(
		std::size_t nofargs)
	{
		std::ostringstream out;
		std::size_t pi = 0, pe = param_converted.size();
		for (; pi != pe; ++pi)
		{
			if (pi) out << ", "; else out << " ";
			out << ((pi >= nofargs) ? param_defaults[ pi] : param_converted[ pi]);
		}
		return out.str();
	}

	void printConstructorCall( std::ostream& out, const std::string& indent, const std::string& classname)
	{
		if (param_converted.size() == 0)
		{
			out << indent << "if (argc > 0) throw strus::runtime_error( \"%s\", _TXT(\"no arguments expected\"));" << std::endl;
			out << indent << "return (void*) new " << classname << "Impl();" << std::endl;
		}
		else
		{
			out << indent << "switch (argc)" << std::endl;
			out << indent << "{" << std::endl;
			for (std::size_t pidx=min_nofargs; pidx<=param_converted.size(); ++pidx)
			{
				out << indent << "\t" << "case " << pidx << ": return new " << classname << "Impl(" << expandCallParameter( pidx) << ");" << std::endl;
			}
			out << indent << "\t" << "default: throw strus::runtime_error( \"%s\", _TXT(\"too many arguments\"));" << std::endl;
			out << indent << "}" << std::endl;
		}
	}

	std::string mapInitReturnValue( const strus::VariableValue& returnvalue, const std::string& expression)
	{
		if (returnvalue.type().hasEvent("retv_map"))
		{
			return returnvalue.expand( "retv_map", "retval", expression.c_str());
		}
		else if (returnvalue.type().source() == "void")
		{
			return expression + ";";
		}
		else
		{
			throw std::runtime_error( std::string("no return value type map declared for '") + returnvalue.type().source() + "'");
		}
	}

	void printMethodCall( std::ostream& out, const std::string& indent, const std::string& methodname, const strus::VariableValue& returnvalue)
	{
		if (param_converted.size() == 0)
		{
			out << indent << "if (argc > 0) throw strus::runtime_error( \"%s\", _TXT(\"no arguments expected\"));" << std::endl;
			std::string expression = "THIS->" + methodname + "()";
			out << indent << mapInitReturnValue( returnvalue, expression) << std::endl;
		}
		else
		{
			out << indent << "switch (argc)" << std::endl;
			out << indent << "{" << std::endl;
			for (std::size_t pidx=min_nofargs; pidx<=param_converted.size(); ++pidx)
			{
				std::string expression = "THIS->" + methodname + "(" + expandCallParameter( pidx) + ")";
				out << indent << "\t" << "case " << pidx << ": " << mapInitReturnValue( returnvalue, expression) << " break;" << std::endl;
			}
			out << indent << "\t" << "default: throw strus::runtime_error( \"%s\", _TXT(\"too many arguments\"));" << std::endl;
			out << indent << "}" << std::endl;
		}
	}
};

static void print_BindingObjectsCpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printCppFrameHeader( out, "bindingObjects", "Identifiers for objects and methods for serialization");
	out << "#include \"strus/bindingObjects.h\"" << std::endl;
	out << "#include \"strus/base/dll_tags.hpp\"" << std::endl;
	out << "#include \"impl/strus.hpp\"" << std::endl;
	out << "#include \"papuga/valueVariant.h\"" << std::endl;
	out << "#include \"papuga/callResult.h\"" << std::endl;
	out << "#include \"papuga/serialization.h\"" << std::endl;
	out << "#include \"papuga/errors.h\"" << std::endl;
	out << "#include \"internationalization.hpp\"" << std::endl;
	out << "#include \"callResultUtils.hpp\"" << std::endl;
	out << "#include \"valueVariantWrap.hpp\"" << std::endl;
	out << std::endl
		<< "using namespace strus;" << std::endl
		<< "using namespace strus::bindings;" << std::endl
		<< std::endl;

	out << "#define CATCH_CONSTRUCTOR_CALL_ERROR( retval, classnam)\\" << std::endl;
	out << "\t" << "catch (const std::runtime_error& err)\\" << std::endl;
	out << "\t" << "{\\" << std::endl;
	out << "\t\t" << "papuga_ErrorBuffer_reportError( retval, _TXT(\"error calling constructor of %s: %s\"), classnam, err.what());\\" << std::endl;
	out << "\t" << "}\\" << std::endl;
	out << "\t" << "catch (const std::bad_alloc& err)\\" << std::endl;
	out << "\t" << "{\\" << std::endl;
	out << "\t\t" << "papuga_ErrorBuffer_reportError( retval, _TXT(\"out of memory calling constructor of %s\"), classnam);\\" << std::endl;
	out << "\t" << "}\\" << std::endl;
	out << "\t" << "catch (const std::exception& err)\\" << std::endl;
	out << "\t" << "{\\" << std::endl;
	out << "\t\t" << "papuga_ErrorBuffer_reportError( retval, _TXT(\"uncaught exception calling constructor of %s: %s\"), classnam, err.what());\\" << std::endl;
	out << "\t" << "}\\" << std::endl;
	out << "\t" << "return 0;" << std::endl << std::endl;

	out << "#define CATCH_METHOD_CALL_ERROR( retval, classnam, methodnam)\\" << std::endl;
	out << "\t" << "catch (const std::runtime_error& err)\\" << std::endl;
	out << "\t" << "{\\" << std::endl;
	out << "\t\t" << "papuga_CallResult_reportError( retval, _TXT(\"error calling method %s::%s(): %s\"), classnam, methodnam, err.what());\\" << std::endl;
	out << "\t" << "}\\" << std::endl;
	out << "\t" << "catch (const std::bad_alloc& err)\\" << std::endl;
	out << "\t" << "{\\" << std::endl;
	out << "\t\t" << "papuga_CallResult_reportError( retval, _TXT(\"out of memory calling method %s::%s()\"), classnam, methodnam);\\" << std::endl;
	out << "\t" << "}\\" << std::endl;
	out << "\t" << "catch (const std::exception& err)\\" << std::endl;
	out << "\t" << "{\\" << std::endl;
	out << "\t\t" << "papuga_CallResult_reportError( retval, _TXT(\"uncaught exception calling method %s::%s(): %s\"), classnam, methodnam, err.what());\\" << std::endl;
	out << "\t" << "}\\" << std::endl;
	out << "\t" << "return 0;" << std::endl << std::endl;

	std::vector<strus::StructDef>::const_iterator
		si = interfaceDef.structDefs().begin(),
		se = interfaceDef.structDefs().end();
	int sidx = 0;
	for (; si != se; ++si,++sidx)
	{
		out << "extern \"C\" DLL_PUBLIC const char** " << structMembersFunctionName( si->name()) << "()" << std::endl << "{" << std::endl;
		out << "\t" << "static const char* ar[] = {";
		std::vector<strus::MemberDef>::const_iterator mi = si->memberDefs().begin(), me = si->memberDefs().end();
		for (; mi != me; ++mi)
		{
			out << "\"" << mi->name() << "\", ";
		}
		out << "NULL};" << std::endl;
		out << "\t" << "return ar;" << std::endl << "}" << std::endl;
	}
	out << std::endl;

	std::vector<strus::ClassDef>::const_iterator
		ci = interfaceDef.classDefs().begin(),
		ce = interfaceDef.classDefs().end();
	for (; ci != ce; ++ci)
	{
		out << "extern \"C\" DLL_PUBLIC void " << destructorFunctionName( ci->name()) << "( void* self)" << std::endl;
		out << "{" << std::endl;
		out << "	delete reinterpret_cast<" << ci->name() << "Impl*>( self);" << std::endl;
		out << "}" << std::endl;
		out << std::endl;

		if (ci->constructorDefs().size() > 1) throw std::runtime_error( std::string("only one constructor allowed for '") + ci->name() + "'");
		std::vector<strus::ConstructorDef>::const_iterator
			ki = ci->constructorDefs().begin(),
			ke = ci->constructorDefs().end();
		for (; ki != ke; ++ki)
		{
			out 
			<< "extern \"C\" DLL_PUBLIC void* " << constructorFunctionName( ci->name())
			<< "( papuga_ErrorBuffer* errbuf, size_t argc, const papuga_ValueVariant* argv)" << std::endl
			<< "{" << std::endl;
			out << "\t" << "try {" << std::endl;
			ParameterSructureExpanded paramstruct( ki->parameters());
			if (paramstruct.min_nofargs)
			{
				out << "\t\t" << "if (argc < " << paramstruct.min_nofargs << ") throw strus::runtime_error( \"%s\", _TXT(\"too few arguments\"));" << std::endl;
			}
			// Do some declarations and conversions, if needed:
			for (std::size_t pidx=0; pidx < paramstruct.size(); ++pidx)
			{
				if (!paramstruct.param_declarations[pidx].empty())
				{
					out << "\t\t" << paramstruct.param_declarations[pidx] << std::endl;
				}
			}
			// Print call:
			paramstruct.printConstructorCall( out, "\t\t", ci->name());

			out << "\t\t" << "return 0;" << std::endl;
			out << "\t" << "}" << std::endl;
			out << "\t" << "CATCH_CONSTRUCTOR_CALL_ERROR( errbuf, \"" << ci->name().c_str() << "\")" << std::endl;
			out << "}" << std::endl << std::endl;
		}

		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (; mi != me; ++mi)
		{
			out << "extern \"C\" DLL_PUBLIC bool " << methodFunctionName( ci->name(), mi->name())
				<< "( void* self, papuga_CallResult* retval, "
				<< "size_t argc, const papuga_ValueVariant* argv)" << std::endl;
			out << "{" << std::endl;
			out << "\t" << "try {" << std::endl;
			out << "\t\t" << ci->name() << "Impl* THIS = ("<< ci->name() << "Impl*)(self);" << std::endl;

			ParameterSructureExpanded paramstruct( mi->parameters());
			if (paramstruct.min_nofargs)
			{
				out << "\t\t" << "if (argc < " << paramstruct.min_nofargs << ") throw strus::runtime_error( \"%s\", _TXT(\"too few arguments\"));" << std::endl;
			}
			// Do some declarations and conversions, if needed:
			for (std::size_t pidx=0; pidx < paramstruct.size(); ++pidx)
			{
				if (!paramstruct.param_declarations[pidx].empty())
				{
					out << "\t\t" << paramstruct.param_declarations[pidx] << std::endl;
				}
			}
			// Print call:
			paramstruct.printMethodCall( out, "\t\t", mi->name(), mi->returnValue());

			out << "\t\t" << "return true;" << std::endl;
			out << "\t" << "}" << std::endl;
			out << "\t" << "CATCH_METHOD_CALL_ERROR( retval, \"" << ci->name().c_str() << "\", \"" << mi->name().c_str() << "\")" << std::endl;
			out << "}" << std::endl << std::endl;
		}
	}
}

static void print_BindingClassesH( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printHFrameHeader( out, "bindingClasses", "Provides binding classes as structures");
	out << "#include \"papuga/classdef.h\"" << std::endl;

	out << "#ifdef __cplusplus" << std::endl;
	out << "extern \"C\" {" << std::endl;
	out << "#endif" << std::endl << std::endl;

	out << "const papuga_ClassDef* getStrusClassDefs();" << std::endl << std::endl;

	out << "#ifdef __cplusplus" << std::endl;
	out << "}" << std::endl;
	out << "#endif" << std::endl << std::endl;
	strus::printHFrameTail( out);
}

static void print_BindingClassesCpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printCppFrameHeader( out, "bindingClasses", "Provides binding classes as structures");
	out << "#include \"strus/bindingClasses.h\"" << std::endl;
	out << "#include \"strus/bindingObjects.h\"" << std::endl;
	out << "#include \"strus/base/dll_tags.hpp\"" << std::endl;
	out << "#include \"impl/strus.hpp\"" << std::endl;
	out << "#include \"papuga.h\"" << std::endl;
	out << std::endl
		<< "using namespace strus;" << std::endl
		<< "using namespace strus::bindings;" << std::endl
		<< std::endl;
	
	std::vector<strus::ClassDef>::const_iterator
		ci = interfaceDef.classDefs().begin(),
		ce = interfaceDef.classDefs().end();

	for (; ci != ce; ++ci)
	{
		out << "static const char* g_methodnames_" << ci->name() << "[" << (ci->methodDefs().size() + 1) << "] = {";
		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (; mi != me; ++mi)
		{
			out << "\"" << mi->name() << "\",";
		}
		out << "NULL};" << std::endl;

		out << "static papuga_ClassMethod g_methodtable_" << ci->name() << "[" << (ci->methodDefs().size() + 1) << "] = {";
		mi = ci->methodDefs().begin();
		for (; mi != me; ++mi)
		{
			out << "&" << methodFunctionName( ci->name(), mi->name()) << ",";
		}
		out << "NULL};" << std::endl;
	}
	out << std::endl;
	out << "static const papuga_ClassDef g_classdefs[" << (interfaceDef.classDefs().size() + 1) << "] = {" << std::endl;
	ci = interfaceDef.classDefs().begin();
	for (; ci != ce; ++ci)
	{
		out << "\t" << "{\"" << ci->name() << "\", ";
		if (ci->constructorDefs().empty())
		{
			out << "NULL,";
		}
		else
		{
			out << "&" << constructorFunctionName( ci->name()) << ", ";
		}
		out << "&" << destructorFunctionName( ci->name()) << ", "
			<< "g_methodtable_" << ci->name() << ", "
			<< "g_methodnames_" << ci->name() << ","
			<< ci->methodDefs().size() << "}," << std::endl;
	}
	out << "\t" << "{NULL,NULL,NULL,NULL,NULL,0}" << std::endl;
	out << "};" << std::endl;
	out << "extern \"C\" DLL_PUBLIC const papuga_ClassDef* getStrusClassDefs()" << std::endl;
	out << "{" << std::endl;
	out << "\t" << "return g_classdefs;" << std::endl;
	out << "}" << std::endl;
	out << std::endl;
}

static void print_BindingClassTemplatesHpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printHppFrameHeader( out, "bindingClassTemplate", "Template to map interface to some properties");
	out << "#include \"strus/bindingObjects.h\"" << std::endl;
	out << "#include \"papuga/typedefs.h\"" << std::endl;
	out << "#include \"impl/strus.hpp\"" << std::endl;
	out << std::endl
		<< "namespace strus {" << std::endl
		<< "namespace bindings {" << std::endl
		<< std::endl;

	out << "template<class ClassName>" << std::endl
		<< "struct BindingClassTemplate" << std::endl
		<< "{" << std::endl
		<< "\t" << "static void deleter( void* objref)" << std::endl
		<< "\t" << "{" << std::endl
		<< "\t\t" << "delete reinterpret_cast<ClassName*>( objref);" << std::endl
		<< "\t" << "}" << std::endl
		<< "\t" << "static papuga_Deleter getDestructor()	{return &deleter;}" << std::endl
		<< "\t" << "static int classid()			{return 0;}" << std::endl
		<< "};" << std::endl << std::endl;

	std::vector<strus::ClassDef>::const_iterator
		ci = interfaceDef.classDefs().begin(),
		ce = interfaceDef.classDefs().end();
	for (int classid=1; ci != ce; ++ci,++classid)
	{
		out << "template<>" << std::endl
			<< "struct BindingClassTemplate<" << ci->name() << "Impl>" << std::endl
			<< "{" << std::endl
			<< "\t" << "static papuga_Deleter getDestructor()	{return &" << destructorFunctionName( ci->name()) << ";}" << std::endl
			<< "\t" << "static int classid()			{return " << classid << ";}" << std::endl
			<< "\t" << "static const char* name()			{return \"" << ci->name() << "\";}" << std::endl
			<< "};" << std::endl << std::endl;
	}
	out << "}}//namespace" << std::endl;
	strus::printHppFrameTail( out);
}

static void print_methodIdsHpp( std::ostream& out, const strus::InterfacesDef& interfaceDef)
{
	strus::printHppFrameHeader( out, "methodIds", "Provides identifiers for methods to address in request definitions");
	out << "#include \"strus/bindingObjects.h\"" << std::endl;
	out << "#include \"strus/base/dll_tags.hpp\"" << std::endl;
	out << "#include \"papuga.h\"" << std::endl;
	out << std::endl
		<< "namespace strus {" << std::endl
		<< "namespace bindings {" << std::endl
		<< std::endl << std::endl;

	out << "struct MethodId" << std::endl
		<< "{" << std::endl
		<< "\t" << "int classid;" << std::endl
		<< "\t" << "int methodid;" << std::endl
		<< "\t" << "MethodId( int classid_, int methodid_)" << std::endl
		<< "\t\t" << ":classid(classid_),methodid(methodid_){}" << std::endl
		<< "\t" << "MethodId( const MethodId& o)" << std::endl
		<< "\t\t" << ":classid(o.classid),methodid(o.methodid){}" << std::endl
		<< "};" << std::endl << std::endl;
	
	out << "namespace method {" << std::endl << std::endl;

	std::vector<strus::ClassDef>::const_iterator
		ci = interfaceDef.classDefs().begin(),
		ce = interfaceDef.classDefs().end();
	for (int cidx=1; ci != ce; ++ci,++cidx)
	{
		out << "struct " << ci->name() << std::endl;
		out << "{" << std::endl;
		if (!ci->constructorDefs().empty())
		{
			out << "\t" << "static const MethodId& constructor() {return MethodId( " << cidx << "," << 0 << ");}" << std::endl;
		}
		std::vector<strus::MethodDef>::const_iterator
			mi = ci->methodDefs().begin(),
			me = ci->methodDefs().end();
		for (int midx=1; mi != me; ++mi,++midx)
		{
			out << "\t" << "static const MethodId& " << mi->name() << "() {return MethodId( " << cidx << "," << midx << ");}" << std::endl;
		}
		out << "};" << std::endl;
	}
	out << "}}}//namespace" << std::endl << std::endl;
	strus::printHppFrameTail( out);
}


int main( int argc, const char* argv[])
{
	int ec = 0;
	try
	{
		std::string outputdir = ".";
		bool verbose = false;
		int argi = 1;
		for (; argi < argc; ++argi)
		{
			if (argv[argi][0] == '-')
			{
				if (std::strcmp(argv[argi],"-h") == 0 || std::strcmp(argv[argi],"--help") == 0)
				{
					std::cerr << "Usage: strusBindingsInterfaceGen <outputroot> { <inputfile> }" << std::endl;
					std::cerr << "<outputroot> :Root director for output" << std::endl;
					std::cerr << "<inputfile>  :input file path" << std::endl;
					return 0;
				}
				else if (std::strcmp(argv[argi],"-v") == 0 || std::strcmp(argv[argi],"--verbose") == 0)
				{
					verbose = true;
				}
				else if (std::strcmp(argv[argi],"--") == 0)
				{
					++argi;
					break;
				}
				else
				{
					throw std::runtime_error( std::string("unknown option ") + argv[argi]);
				}
			}
			else
			{
				break;
			}
		}
		if (argi < argc)
		{
			outputdir = argv[ argi++];
		}
		std::cerr << "Output directory root is '" << outputdir << "'" << std::endl;
		strus::TypeSystem typeSystem;
		strus::fillTypeTables( typeSystem);

		if (verbose)
		{
			std::cout << "TypeSystem:" << std::endl << typeSystem.tostring() << std::endl;
		}
		strus::InterfacesDef interfaceDef( &typeSystem);
		for (; argi < argc; ++argi)
		{
			std::string source;
			if (verbose)
			{
				std::cout << "read file:" << argv[ argi] << std::endl;
			}
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
		if (verbose)
		{
			std::cout << interfaceDef.tostring() << std::endl;
		}
		printOutput( outputdir + "/include/strus/bindingObjects.h", &print_BindingObjectsH, interfaceDef);
		printOutput( outputdir + "/src/bindings/bindingObjects.cpp", &print_BindingObjectsCpp, interfaceDef);
		printOutput( outputdir + "/include/strus/bindingClasses.h", &print_BindingClassesH, interfaceDef);
		printOutput( outputdir + "/src/bindings/bindingClasses.cpp", &print_BindingClassesCpp, interfaceDef);
		printOutput( outputdir + "/src/bindings/bindingClassTemplate.hpp", &print_BindingClassTemplatesHpp, interfaceDef);
		printOutput( outputdir + "/src/bindings/methodIds.hpp", &print_methodIdsHpp, interfaceDef);
		printOutput( outputdir + "/include/strus/lib/bindings_description.hpp", &print_BindingInterfaceDescriptionHpp, interfaceDef);
		printOutput( outputdir + "/src/bindings/libstrus_bindings_description.cpp", &print_BindingInterfaceDescriptionCpp, interfaceDef);

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


