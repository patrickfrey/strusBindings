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
	;
	typesystem.defineType( "std::size_t")
	;
	typesystem.defineType( "bool")
	;
	typesystem.defineType( "int")
	;
	typesystem.defineType( "int $name=0")
	;
	typesystem.defineType( "unsigned int")
	;
	typesystem.defineType( "unsigned int $name=0")
	;
	typesystem.defineType( "float")
	;
	typesystem.defineType( "double")
	;
	typesystem.defineType( "double $name=1")
	;
	typesystem.defineType( "const ValueVariant&")
	;
	typesystem.defineType( "ValueVariant")
	;
	typesystem.defineType( "const NumericVariant&")
	;
	typesystem.defineType( "NumericVariant")
	;
	typesystem.defineType( "std::string")
	;
	typesystem.defineType( "const std::string& $name=\"\"")
	;
	typesystem.defineType( "const std::string&")
	;
	typesystem.defineType( "const char*")
	;
	typesystem.defineType( "$objid~Impl")
	;
	typesystem.defineType( "const $objid~Impl&")
	;
	typesystem.defineType( "const TextProcessorInterface*")
	;
	typesystem.defineType( "const StatisticsProcessorInterface*")
	;
	typesystem.defineType( "const QueryProcessorInterface*")
	;
}

