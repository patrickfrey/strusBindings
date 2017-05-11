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
		("retv_map", "papuga_set_CallResult_uint( $name, $value);")
		("argv_map", "ValueVariantWrap::touint64( $value)")
	;
	typesystem.defineType( "bool")
		("retv_map", "papuga_set_CallResult_bool( $name, $value);")
		("argv_map", "ValueVariantWrap::tobool( $value)")
	;
	typesystem.defineType( "Index")
		("retv_map", "papuga_set_CallResult_int( $name, $value);")
		("argv_map", "ValueVariantWrap::toint( $value)")
	;
	typesystem.defineType( "int")
		("retv_map", "papuga_set_CallResult_int( $name, $value);")
		("argv_map", "ValueVariantWrap::toint( $value)")
	;
	typesystem.defineType( "unsigned int")
		("retv_map", "papuga_set_CallResult_uint( $name, $value);")
		("argv_map", "ValueVariantWrap::touint( $value)")
	;
	typesystem.defineType( "unsigned int $val=0")
		("argv_default", "0")
		("argv_map", "ValueVariantWrap::touint( $value)")
	;
	typesystem.defineType( "float")
		("retv_map", "papuga_set_CallResult_double( $name, $value);")
		("argv_map", "ValueVariantWrap::tofloat( $value)")
	;
	typesystem.defineType( "double")
		("retv_map", "papuga_set_CallResult_double( $name, $value);")
		("argv_map", "ValueVariantWrap::todouble( $value)")
	;
	typesystem.defineType( "double $val=1")
		("argv_default", "1.0")
		("argv_map", "ValueVariantWrap::todouble( $value)")
	;
	typesystem.defineType( "const ValueVariant& $val=ValueVariant()")
		("argv_default", "ValueVariant()")
		("argv_map", "$value")
	;
	typesystem.defineType( "const ValueVariant&")
		("argv_map", "$value")
	;
	typesystem.defineType( "const NumericVariant&")
		("argv_map", "ValueVariantWrap::tonumeric( $value)")
	;
	typesystem.defineType( "const std::string& $val=\"\"")
		("argv_default", "std::string()")
		("argv_map", "ValueVariantWrap::tostring( $value)")
	;
	typesystem.defineType( "const std::string&")
		("retv_map", "{const std::string& retvalstr = $value; papuga_set_CallResult_string_const( $name, retvalstr.c_str(), retvalstr.size());}")
		("argv_map", "ValueVariantWrap::tostring( $value)")
	;
	typesystem.defineType( "std::string")
		("retv_map", "{std::string retvalstr = $value; papuga_set_CallResult_string( $name, retvalstr.c_str(), retvalstr.size());}")
	;
	typesystem.defineType( "std::string*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
	typesystem.defineType( "const char*")
		("retv_map", "papuga_set_CallResult_charp_const( $name, $value);")
		("argv_decl", "std::string $name;")
		("argv_map", "ValueVariantWrap::tocharp( $name, $value)")
	;
	typesystem.defineType( "const $objid~Impl*")
		("retv_map", "initCallResultObjectConst( $name, $value);")
		("argv_map", "implObjectCast<const $objid~Impl>( $value)")
	;
	typesystem.defineType( "$objid~Impl*")
		("retv_map", "initCallResultObjectOwnership( $name, $value);")
		("argv_map", "implObjectCast<$objid~Impl>( $value)")
	;
	typesystem.defineType( "analyzer::DocumentClass*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
	typesystem.defineType( "analyzer::Document*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
	typesystem.defineType( "AnalyzedQuery*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
	typesystem.defineType( "std::vector<std::pair<std::string,std::string> >*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
	typesystem.defineType( "std::vector<std::string>*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
	typesystem.defineType( "Struct*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
	typesystem.defineType( "QueryResult*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
	typesystem.defineType( "std::vector<VectorStorageSearchInterface::Result>")
		("retv_map", "initCallResultNumericValues( $name, $value);")
	;
	typesystem.defineType( "std::vector<Index>")
		("retv_map", "initCallResultNumericValues( $name, $value);")
	;
	typesystem.defineType( "std::vector<double>")
		("retv_map", "initCallResultNumericValues( $name, $value);")
	;
	typesystem.defineType( "$objid~Interface*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
	;
}

