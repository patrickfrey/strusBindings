/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_PAPUGA_VALUE_VARIANT_WRAP_HPP_INCLUDED
#define _STRUS_PAPUGA_VALUE_VARIANT_WRAP_HPP_INCLUDED
/// \brief Some local value variant wrappers for the strus bindings in C++
/// \file valueVariantWrap.hpp
#include "papuga/typedefs.h"
#include "papuga/valueVariant.hpp"
#include "papuga/errors.hpp"
#include "strus/storage/index.hpp"
#include "strus/base/stdint.h"
#include "strus/numericVariant.hpp"
#include "private/internationalization.hpp"
#include "bindingClassTemplate.hpp"

namespace strus {
namespace bindings {

struct ValueVariantWrap
{
	static int64_t toint64( const papuga_ValueVariant& value);
	static uint64_t touint64( const papuga_ValueVariant& value);
	static int toint( const papuga_ValueVariant& value);
	static unsigned int touint( const papuga_ValueVariant& value);
	static Index toindex( const papuga_ValueVariant& value);
	static double todouble( const papuga_ValueVariant& value);
	static float tofloat( const papuga_ValueVariant& value);
	static bool tobool( const papuga_ValueVariant& value);
	static strus::NumericVariant tonumeric( const papuga_ValueVariant& value);
	static std::string tostring( const papuga_ValueVariant& value);
	static const char* tocharp( std::string& buf, const papuga_ValueVariant& value);

	template <class ClassImpl>
	static ClassImpl* toclass( const papuga_ValueVariant& val)
	{
		if (val.valuetype != papuga_TypeHostObject || val.value.hostObject->classid != BindingClassTemplate<ClassImpl>::classid())
		{
			throw strus::runtime_error(_TXT("expected class '%s'"), BindingClassTemplate<ClassImpl>::name());
		}
		return (ClassImpl*)const_cast<void*>( val.value.hostObject->data);
	}
};

}}//namespace
#endif

