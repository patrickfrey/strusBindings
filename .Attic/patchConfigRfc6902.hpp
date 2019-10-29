/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Function to apply a list of patches according to RFC 6902 to a configuration content
/// \file "patchConfigRfc6902.hpp"
#ifndef _STRUS_BINDINGS_PATCH_CONFIG_RFC6902_HPP_INCLUDED
#define _STRUS_BINDINGS_PATCH_CONFIG_RFC6902_HPP_INCLUDED
#include "strus/errorCodes.hpp"
#include <string>

namespace strus {
///\brief Forward declaration
class ErrorBufferInterface;

namespace bindings {

std::string patchConfigRfc6902( const std::string& configsrc, const std::string& patchsrc, ErrorBufferInterface* errorhnd);

}}//namespace
#endif


