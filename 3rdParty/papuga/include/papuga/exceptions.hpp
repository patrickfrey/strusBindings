/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef _PAPUGA_ERROR_EXCEPTIONS_HPP_INCLUDED
#define _PAPUGA_ERROR_EXCEPTIONS_HPP_INCLUDED
/// \brief Map of error codes to std::runtime_error exception
/// \file exceptions.hpp
#include "papuga/typedefs.h"
#include <stdexcept>

namespace papuga {

std::runtime_error error_exception( const papuga_ErrorCode& ec, const char* where);

}//namespace
#endif

