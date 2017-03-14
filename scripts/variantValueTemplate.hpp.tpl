/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_VARIANT_VALUE_TEMPLATE_HPP_INCLUDED
#define _STRUS_BINDING_VARIANT_VALUE_TEMPLATE_HPP_INCLUDED
/// \file analyzerTermFilter.hpp
#include "strus/bindings/valueVariant.hpp"

/// \brief strus toplevel namespace
namespace strus {
namespace bindings {

template <typename TYPE>
struct VariantValueTemplate
{
	static bindings::ValueVariant get( const TYPE& val);
};

{% for tp in atomictypes %}
template <>
struct VariantValueTemplate<{{tp.fullname}}>
{
	static bindings::ValueVariant get( const TYPE& val)
	{
		return bindings::ValueVariant( static_cast<{{tp.basictype}}>( val));
	}
};
{% endfor %}

}}//namespace
#endif

