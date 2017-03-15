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
{% for incfile in includes %}#include {{incfile}}
{% endfor %}
#include <vector>

/// \brief strus toplevel namespace
namespace strus {
namespace bindings {

template <typename TYPE>
struct VariantValueTemplate
{
	static void init( bindings::ValueVariant& res, const TYPE& val);
};

{% for key,tp in atomictypes.items() %}
template <>
struct VariantValueTemplate<{{tp.fullname}}>
{
	static void init( bindings::ValueVariant& res, {% if "paramname" in tp %}{{tp.paramname}}{% else %}const {{tp.fullname}}& {% endif %}val)
	{
		res.init( {% if "basictype" in tp %}static_cast<{{tp.basictype}}>( val){% endif %}{% if "variantcast" in tp %}{{tp.variantcast}}{% endif %});
	}
};
{% endfor %}

}}//namespace
#endif

