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
	typesystem.defineType( "void")
		("retv_map", "CallResult()")
	;
	typesystem.defineType( "CallResult")
		("retv_map", "CallResult( $name)")
	;
	typesystem.defineType( "std::size_t")
		("argv_map", "VariantValueConv::touint64( argv[ $idx])")
	;
	typesystem.defineType( "bool")
		("argv_map", "VariantValueConv::tobool( argv[ $idx])")
	;
	typesystem.defineType( "int")
		("argv_map", "VariantValueConv::toint( argv[ $idx])")
	;
	typesystem.defineType( "unsigned int")
		("argv_map", "VariantValueConv::touint( argv[ $idx])")
	;
	typesystem.defineType( "unsigned int $val=0")
		("argv_map", "(argc > $idx && argv[ $idx].defined()) ? VariantValueConv::touint( argv[ $idx]) : 0")
	;
	typesystem.defineType( "float")
		("argv_map", "VariantValueConv::tofloat( argv[ $idx])")
	;
	typesystem.defineType( "double")
		("argv_map", "VariantValueConv::todouble( argv[ $idx])")
	;
	typesystem.defineType( "double $val=1")
		("argv_map", "(argc > $idx && argv[ $idx].defined()) ? VariantValueConv::todouble( argv[ $idx]) : 1.0")
	;
	typesystem.defineType( "const ValueVariant&")
		("argv_map", "argv[ $idx]")
	;
	typesystem.defineType( "const NumericVariant&")
		("argv_map", "VariantValueConv::tonumeric( argv[ $idx])")
	;
	typesystem.defineType( "const std::string& $val=\"\"")
		("argv_map", "(argc > $idx && argv[ $idx].defined()) ? VariantValueConv::tostring( argv[ $idx]) : std::string()")
	;
	typesystem.defineType( "const std::string&")
		("argv_map", "VariantValueConv::tostring( argv[ $idx])")
	;
	typesystem.defineType( "const char*")
		("argv_map", "VariantValueConv::tostring( argv[ $idx])")
	;
}

