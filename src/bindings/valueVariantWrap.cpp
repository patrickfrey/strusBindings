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
#include "papuga/errors.hpp"
#include "papuga/serialization.h"
#include "strus/base/stdint.h"
#include "strus/numericVariant.hpp"
#include "private/internationalization.hpp"
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
float ValueVariantWrap::tofloat( const papuga_ValueVariant& value)
{
	papuga_ErrorCode err = papuga_Ok;
	float rt = papuga_ValueVariant_todouble( &value, &err);
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
	if (numval.valuetype == papuga_TypeDouble)
	{
		return strus::NumericVariant( numval.value.Double);
	}
	if (numval.valuetype == papuga_TypeInt)
	{
		return strus::NumericVariant( numval.value.Int);
	}
	if (numval.valuetype == papuga_TypeBool)
	{
		return strus::NumericVariant( (strus::NumericVariant::IntType)(numval.value.Bool?1:0));
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

int64_t ValueVariantWrap::toint64( const papuga_ValueVariant& value, int64_t default_)
{
	return papuga_ValueVariant_defined( &value) ? toint64( value) : default_;
}
uint64_t ValueVariantWrap::touint64( const papuga_ValueVariant& value, uint64_t default_)
{
	return papuga_ValueVariant_defined( &value) ? touint64( value) : default_;
}
int ValueVariantWrap::toint( const papuga_ValueVariant& value, int default_)
{
	return papuga_ValueVariant_defined( &value) ? toint( value) : default_;
}
unsigned int ValueVariantWrap::touint( const papuga_ValueVariant& value, unsigned int default_)
{
	return papuga_ValueVariant_defined( &value) ? touint( value) : default_;
}
Index ValueVariantWrap::toindex( const papuga_ValueVariant& value, Index default_)
{
	return papuga_ValueVariant_defined( &value) ? toindex( value) : default_;
}
double ValueVariantWrap::todouble( const papuga_ValueVariant& value, double default_)
{
	return papuga_ValueVariant_defined( &value) ? todouble( value) : default_;
}
float ValueVariantWrap::tofloat( const papuga_ValueVariant& value, float default_)
{
	return papuga_ValueVariant_defined( &value) ? tofloat( value) : default_;
}
bool ValueVariantWrap::tobool( const papuga_ValueVariant& value, bool default_)
{
	return papuga_ValueVariant_defined( &value) ? tobool( value) : default_;
}
strus::NumericVariant ValueVariantWrap::tonumeric( const papuga_ValueVariant& value, const strus::NumericVariant& default_)
{
	return papuga_ValueVariant_defined( &value) ? tonumeric( value) : default_;
}
std::string ValueVariantWrap::tostring( const papuga_ValueVariant& value, const std::string& default_)
{
	return papuga_ValueVariant_defined( &value) ? tostring( value) : default_;
}
const char* ValueVariantWrap::tocharp( std::string& buf, const papuga_ValueVariant& value, const char* default_)
{
	return papuga_ValueVariant_defined( &value) ? tocharp( buf, value) : default_;
}
