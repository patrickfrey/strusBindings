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
#include "strus/analyzer/query.hpp"
#include <string>
#include "strus/numericVariant.hpp"

#include <vector>

/// \brief strus toplevel namespace
namespace strus {
namespace filter {

template <typename TYPE>
struct VariantValueTemplate
{
	static void init( bindings::ValueVariant& res, const TYPE& val);
};


template <>
struct VariantValueTemplate<unsigned int>
{
	static void init( bindings::ValueVariant& res, const unsigned int& val)
	{
		res.init( (bindings::ValueVariant::UIntType)val);
	}
	static bindings::ValueVariant get( const unsigned int& val)
	{
		return bindings::ValueVariant( (bindings::ValueVariant::UIntType)val);
	}
};

template <>
struct VariantValueTemplate<std::string>
{
	static void init( bindings::ValueVariant& res, const std::string& val)
	{
		res.init( val.c_str(), val.size());
	}
	static bindings::ValueVariant get( const std::string& val)
	{
		return bindings::ValueVariant( val.c_str(), val.size());
	}
};

template <>
struct VariantValueTemplate<const char*>
{
	static void init( bindings::ValueVariant& res, const char*val)
	{
		res.init( val);
	}
	static bindings::ValueVariant get( const char*val)
	{
		return bindings::ValueVariant( val);
	}
};

template <>
struct VariantValueTemplate<int>
{
	static void init( bindings::ValueVariant& res, const int& val)
	{
		res.init( (bindings::ValueVariant::IntType)val);
	}
	static bindings::ValueVariant get( const int& val)
	{
		return bindings::ValueVariant( (bindings::ValueVariant::IntType)val);
	}
};

template <>
struct VariantValueTemplate<double>
{
	static void init( bindings::ValueVariant& res, const double& val)
	{
		res.init( val);
	}
	static bindings::ValueVariant get( const double& val)
	{
		return bindings::ValueVariant( val);
	}
};

template <>
struct VariantValueTemplate<NumericVariant>
{
	static void init( bindings::ValueVariant& res, const NumericVariant& val)
	{
		res.init( val);
	}
	static bindings::ValueVariant get( const NumericVariant& val)
	{
		return bindings::ValueVariant( val);
	}
};

template <>
struct VariantValueTemplate<bool>
{
	static void init( bindings::ValueVariant& res, const bool& val)
	{
		res.init( (bindings::ValueVariant::IntType)val);
	}
	static bindings::ValueVariant get( const bool& val)
	{
		return bindings::ValueVariant( (bindings::ValueVariant::IntType)val);
	}
};

template <>
struct VariantValueTemplate<analyzer::Query::Element::Type>
{
	static void init( bindings::ValueVariant& res, const analyzer::Query::Element::Type& val)
	{
		res.init( analyzer::Query::Element::typeName( val));
	}
	static bindings::ValueVariant get( const analyzer::Query::Element::Type& val)
	{
		return bindings::ValueVariant( analyzer::Query::Element::typeName( val));
	}
};

template <>
struct VariantValueTemplate<analyzer::Query::Instruction::OpCode>
{
	static void init( bindings::ValueVariant& res, const analyzer::Query::Instruction::OpCode& val)
	{
		res.init( analyzer::Query::Instruction::opCodeName( val));
	}
	static bindings::ValueVariant get( const analyzer::Query::Instruction::OpCode& val)
	{
		return bindings::ValueVariant( analyzer::Query::Instruction::opCodeName( val));
	}
};


}}//namespace
#endif
