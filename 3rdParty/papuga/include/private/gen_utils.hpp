/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Some utility functions for generating language binding sources
/// \file private/gen_utils.hpp
#ifndef _PAPUGA_GEN_UTILS_HPP_INCLUDED
#define _PAPUGA_GEN_UTILS_HPP_INCLUDED
#include "papuga/interfaceDescription.h"
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace papuga {

std::string cppCodeSnippet( unsigned int idntcnt, ...);

std::vector<std::string> getGeneratorArguments(
	const std::multimap<std::string,std::string>& args,
	const char* name);
std::string getGeneratorArgument(
	const std::multimap<std::string,std::string>& args,
	const char* name,
	const char* defaultval);

std::string readFile( const std::string& path);

}//namespace
#endif

