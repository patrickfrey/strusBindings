/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Some utility functions that are centralised to control dependencies to boost
#ifndef _STRUS_UTILS_HPP_INCLUDED
#define _STRUS_UTILS_HPP_INCLUDED
#include <string>

namespace strus {
namespace utils {

std::string tolower( const std::string& val);
std::string trim( const std::string& val);
bool caseInsensitiveEquals( const std::string& val1, const std::string& val2);
bool caseInsensitiveStartsWith( const std::string& val, const std::string& prefix);
int toint( const std::string& val);
std::string tostring( long val);
std::string tostring( float val);

}} //namespace
#endif


