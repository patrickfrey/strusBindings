/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_PRINT_LUA_DOCUMENTATION_INCLUDED
#define _PAPUGA_PRINT_LUA_DOCUMENTATION_INCLUDED
/// \brief Module for printing the lua documentation
/// \file printLuaDoc.hpp
#include "papuga/interfaceDescription.h"
#include <iostream>

namespace papuga {

void printLuaDoc( std::ostream& out, const papuga_InterfaceDescription& descr);

}//namespace
#endif

