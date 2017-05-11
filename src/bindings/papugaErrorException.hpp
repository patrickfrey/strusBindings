/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef _STRUS_BINDINGS_PAPUGA_ERROR_EXCEPTIONS_HPP_INCLUDED
#define _STRUS_BINDINGS_PAPUGA_ERROR_EXCEPTIONS_HPP_INCLUDED
/// \brief Map of papuga error codes to std::runtime_error exception
/// \file papugaErrorException.hpp
#include "papuga/typedefs.h"
#include "internationalization.hpp"

namespace strus {
namespace bindings {

std::runtime_error papuga_error_exception( const papuga_ErrorCode& ec, const char* where);

}}//namespace
#endif

