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
#include "strus/numericVariant.h"
#include "internationalization.hpp"
#include "papugaErrorException.hpp"

namespace strus {
namespace bindings {

int64_t ValueVariantWrap::toint( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err;
	int64_t rt = papuga_ValueVariant_toint( &value, &err);
	if (err != papuga_Ok) throw error_exception( err, _TXT("toint of variant value"));
	return rt;
}
uint64_t ValueVariantWrap::touint( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err;
	uint64_t rt = papuga_ValueVariant_touint( &value, &err);
	if (err != papuga_Ok) throw error_exception( err, _TXT("touint of variant value"));
	return rt;
}
double ValueVariantWrap::todouble( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err;
	double rt = papuga_ValueVariant_todouble( &value, &err);
	if (err != papuga_Ok) throw error_exception( err, _TXT("todouble of variant value"));
	return rt;
}
bool ValueVariantWrap::tobool( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err;
	double rt = papuga_ValueVariant_tobool( &value, &err);
	if (err != papuga_Ok) throw error_exception( err, _TXT("tobool of variant value"));
	return rt;
}
strus::NumericVariant ValueVariantWrap::tonumeric( const papuga_ValueVariant& value)
{
	papuga_ValueVariant numval;
	papuga_init_ValueVariant_copy( &numval, &value);
	papuga_ErrorCode err = papuga_ValueVariant_convert_tonumeric( &numval);
	if (err != papuga_Ok) throw error_exception( err, _TXT("tonumeric of variant value"));
	if (numval.valuetype == papuga_Double)
	{
		return strus::NumericVariant( numval.value.Double);
	}
	if (numval.valuetype == papuga_UInt)
	{
		return strus::NumericVariant( numval.value.UInt);
	}
	if (numval.valuetype == papuga_Int)
	{
		return strus::NumericVariant( numval.value.Int);
	}
	throw error_exception( papuga_TypeError, _TXT("tonumeric of variant value"));
}
std::string ValueVariantWrap::tostring( const papuga_ValueVariant& value)
{
	papuga_ErrorCode errcode = papuga_Ok;
	std::string rt = papuga::ValueVariant_tostring( &value, errcode);
	if (err != papuga_Ok) throw error_exception( err, _TXT("tostring of variant value"));
	return rt;
}

