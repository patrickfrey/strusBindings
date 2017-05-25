/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Some local value variant wrappers for the strus bindings in C++
/// \file valueVariantWrap.cpp
#include "valueVariantWrap.hpp"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.h"
#include "papuga/valueVariant.hpp"
#include "papuga/exceptions.hpp"
#include "papuga/serialization.hpp"
#include "strus/base/stdint.h"
#include "strus/numericVariant.hpp"
#include "internationalization.hpp"
#include <limits>

using namespace strus;
using namespace strus::bindings;

int64_t ValueVariantWrap::toint64( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	int64_t rt = papuga_ValueVariant_toint( &value, &err);
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("toint64 of variant value"));
	return rt;
}
uint64_t ValueVariantWrap::touint64( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	uint64_t rt = papuga_ValueVariant_touint( &value, &err);
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("touint64 of variant value"));
	return rt;
}
int ValueVariantWrap::toint( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	int64_t rt = papuga_ValueVariant_toint( &value, &err);
	if (rt > std::numeric_limits<int>::max() || rt < std::numeric_limits<int>::min()) err = papuga_OutOfRangeError;
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("toint of variant value"));
	return rt;
}
unsigned int ValueVariantWrap::touint( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	uint64_t rt = papuga_ValueVariant_toint( &value, &err);
	if (rt > std::numeric_limits<unsigned int>::max()) err = papuga_OutOfRangeError;
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("toint of variant value"));
	return rt;
}
Index ValueVariantWrap::toindex( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	int64_t rt = papuga_ValueVariant_toint( &value, &err);
	if (rt > std::numeric_limits<Index>::max() || rt < std::numeric_limits<Index>::min()) err = papuga_OutOfRangeError;
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("toindex of variant value"));
	return rt;
}
double ValueVariantWrap::todouble( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	double rt = papuga_ValueVariant_todouble( &value, &err);
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("todouble of variant value"));
	return rt;
}
bool ValueVariantWrap::tobool( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	double rt = papuga_ValueVariant_tobool( &value, &err);
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("tobool of variant value"));
	return rt;
}
strus::NumericVariant ValueVariantWrap::tonumeric( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	papuga_ValueVariant numval;
	if (!papuga_ValueVariant_tonumeric( &value, &numval, &err))
	{
		throw papuga::error_exception( err, _TXT("tonumeric of variant value"));
	}
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
	throw papuga::error_exception( papuga_TypeError, _TXT("tonumeric of variant value"));
}
std::string ValueVariantWrap::tostring( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	std::string rt = papuga::ValueVariant_tostring( value, err);
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("tostring of variant value"));
	return rt;
}

const char* ValueVariantWrap::tocharp( std::string& buf, const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	buf = papuga::ValueVariant_tostring( value, err);
	if (err != papuga_Ok) throw papuga::error_exception( err, _TXT("tocharp of variant value"));
	return buf.c_str();
}

