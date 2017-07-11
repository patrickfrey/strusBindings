/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_PRINT_LUA_MODULE_INCLUDED
#define _PAPUGA_PRINT_LUA_MODULE_INCLUDED
/// \brief Module for printing the lua module C header and source
/// \file printLuaMod.hpp
#include "papuga/interfaceDescription.h"
#include <iostream>
#include <vector>
#include <string>

namespace papuga {

void printLuaModHeader(
		std::ostream& out,
		const papuga_InterfaceDescription& descr);

void printLuaModSource(
		std::ostream& out,
		const papuga_InterfaceDescription& descr,
		const std::vector<std::string>& includes);

}//namespace
#endif
