/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Map of papuga error codes to std::runtime_error exception
/// \file errors.hpp
#include "papuga/errors.hpp"
#include "papuga/errors.h"
#include <cstdio>

/// \note Internationalization support still missing
#define _TXT(str) str

using namespace papuga;

std::runtime_error papuga::error_exception( const papuga_ErrorCode& ec, const char* where)
{
	char buf[ 1024];
	snprintf( buf, sizeof(buf), _TXT("%s in %s"), papuga_ErrorCode_tostring( ec), where);
	buf[ sizeof(buf)-1] = 0;
	return std::runtime_error( buf);
}


