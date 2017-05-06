/*
 * Copyright (c) 2016 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Some function to print predefined elements of the Strus interface parser output
/// \file printFrame.hpp
#include "printFrame.hpp"

using namespace strus;

void strus::printLicense( std::ostream& out)
{
	out
	<< "/*" << std::endl
	<< "* Copyright (c) 2017 Patrick P. Frey" << std::endl
	<< "*" << std::endl
	<< "* This Source Code Form is subject to the terms of the Mozilla Public" << std::endl
	<< "* License, v. 2.0. If a copy of the MPL was not distributed with this" << std::endl
	<< "* file, You can obtain one at http://mozilla.org/MPL/2.0/." << std::endl
	<< "*/" << std::endl;
}

static void printFrameFileDescription( std::ostream& out, const char* name, const char* ext, const char* description)
{
	out
	<< "/// \\brief " << description << std::endl
	<< "/// \\note PROGRAM (strusBindingsGen) GENERATED FILE! DO NOT MODIFY!!!" << std::endl
	<< "/// \\file " << name << "." << ext << std::endl;
}

void strus::printHppFrameHeader( std::ostream& out, const char* name, const char* description)
{
	printLicense( out);
	printFrameFileDescription( out, name, "hpp", description);
	out
	<< "#ifndef _STRUS_BINDINGS_" << name << "_HPP_INCLUDED" << std::endl
	<< "#define _STRUS_BINDINGS_" << name << "_HPP_INCLUDED" << std::endl;
}

void strus::printHppFrameTail( std::ostream& out)
{
	out << "#endif" << std::endl << std::endl;
}

void strus::printHFrameHeader( std::ostream& out, const char* name, const char* description)
{
	printLicense( out);
	printFrameFileDescription( out, name, "h", description);
	out
	<< "#ifndef _STRUS_BINDINGS_" << name << "_H_INCLUDED" << std::endl
	<< "#define _STRUS_BINDINGS_" << name << "_H<_INCLUDED" << std::endl;
}

void strus::printHFrameTail( std::ostream& out)
{
	out << "#endif" << std::endl << std::endl;
}

void strus::printCppFrameHeader( std::ostream& out, const char* name, const char* description)
{
	printLicense( out);
	printFrameFileDescription( out, name, "cpp", description);
}



