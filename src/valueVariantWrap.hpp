/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_VALUE_VARIANT_WRAP_HPP_INCLUDED
#define _PAPUGA_VALUE_VARIANT_WRAP_HPP_INCLUDED
/// \brief Some local value variant wrappers for the strus bindings in C++
/// \file valueVariantWrap.hpp
#include "papuga/typedefs.h"
#include "papuga/serialization.hpp"
#include "papuga/valueVariant.hpp"
#include "strus/base/stdint.h"
#include "strus/numericVariant.hpp"
#include "internationalization.hpp"
#include "papugaErrorException.hpp"

namespace strus {
namespace bindings {

struct ValueVariantWrap
{
	static int64_t toint( const papuga_ValueVariant& value);
	static uint64_t touint( const papuga_ValueVariant& value);
	static double todouble( const papuga_ValueVariant& value);
	static bool tobool( const papuga_ValueVariant& value);
	static strus::NumericVariant tonumeric( const papuga_ValueVariant& value);
	static std::string tostring( const papuga_ValueVariant& value);
};

}}//namespace
#endif

