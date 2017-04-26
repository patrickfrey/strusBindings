/*
 * Copyright (c) 2016 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Filling tables for the Strus bindings interface parser
/// \file fillTypeTables.cpp
#include "fillTypeTables.hpp"
#include "interfaceParser.hpp"
#include <stdexcept>

using namespace strus;

void strus::fillTypeTables( TypeSystem& typesystem)
{
	typesystem.defineType( "std::size_t")
		("argv_map", "VariantValueConv::touint64( $value)")
	;
	typesystem.defineType( "bool")
		("argv_map", "VariantValueConv::tobool( $value)")
	;
	typesystem.defineType( "int")
		("argv_map", "VariantValueConv::toint( $value)")
	;
	typesystem.defineType( "unsigned int")
		("argv_map", "VariantValueConv::touint( $value)")
	;
	typesystem.defineType( "unsigned int $val=0")
		("argv_default", "0")
		("argv_map", "VariantValueConv::touint( $value)")
	;
	typesystem.defineType( "float")
		("argv_map", "VariantValueConv::tofloat( $value)")
	;
	typesystem.defineType( "double")
		("argv_map", "VariantValueConv::todouble( $value)")
	;
	typesystem.defineType( "double $val=1")
		("argv_default", "1.0")
		("argv_map", "VariantValueConv::todouble( $value)")
	;
	typesystem.defineType( "const ValueVariant&")
		("argv_map", "$value")
	;
	typesystem.defineType( "const NumericVariant&")
		("argv_map", "VariantValueConv::tonumeric( $value)")
	;
	typesystem.defineType( "const std::string& $val=\"\"")
		("argv_default", "std::string()")
		("argv_map", "VariantValueConv::tostring( $value)")
	;
	typesystem.defineType( "const std::string&")
		("argv_map", "VariantValueConv::tostring( $value)")
	;
	typesystem.defineType( "const char*")
		("argv_map", "VariantValueConv::tostring( $value)")
	;
}

