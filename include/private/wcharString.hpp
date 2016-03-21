/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_WCHAR_STRING_UTILITY_FUNCTIONS_HPP_INCLUDED
#define _STRUS_WCHAR_STRING_UTILITY_FUNCTIONS_HPP_INCLUDED
#include <string>

std::string convert_wstring_to_uft8string( const std::wstring& val);
std::wstring convert_uft8string_to_wstring( const std::string& val);

#endif

