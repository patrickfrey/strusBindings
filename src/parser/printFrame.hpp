/*
 * Copyright (c) 2016 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Some function to print predefined elements of the Strus interface parser output
/// \file printFrame.hpp
#ifndef _STRUS_BINDINGS_PARSER_PRINT_FRAME_HPP_INCLUDED
#define _STRUS_BINDINGS_PARSER_PRINT_FRAME_HPP_INCLUDED
#include <iostream>

namespace strus
{

void printLicense( std::ostream& out);
void printHppFrameHeader( std::ostream& out, const char* name, const char* description);
void printHppFrameTail( std::ostream& out);
void printCppFrameHeader( std::ostream& out, const char* name, const char* description);

}//namespace
#endif

