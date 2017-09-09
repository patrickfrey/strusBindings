/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_WCHAR_STRING_UTILITY_FUNCTIONS_HPP_INCLUDED
#define _STRUS_WCHAR_STRING_UTILITY_FUNCTIONS_HPP_INCLUDED
#include "strus/base/stdint.h"
#include <string>

namespace strus {

typedef uint16_t WChar;
typedef std::basic_string<uint16_t> WCharString;

std::basic_string<uint16_t> convert_uft8string_to_w16string( const std::string& val);
std::basic_string<uint16_t> convert_uft8string_to_w16string( const char* str, std::size_t strsize);
std::string convert_w16string_to_uft8string( const std::basic_string<uint16_t>& val);
std::string convert_w16string_to_uft8string( const uint16_t* str, std::size_t strsize);

}//namespace
#endif

