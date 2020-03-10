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
		("typename", "")
	;
	typesystem.defineType( "std::size_t")
		("retv_map", "(void)papuga_add_CallResult_int( $name, $value);")
		("argv_map", "ValueVariantWrap::touint64( $value)")
		("typename", "integer")
	;
	typesystem.defineType( "bool")
		("retv_map", "(void)papuga_add_CallResult_bool( $name, $value);")
		("argv_map", "ValueVariantWrap::tobool( $value)")
		("typename", "boolean")
	;
	typesystem.defineType( "bool $val=true")
		("argv_default", "true")
		("argv_map", "ValueVariantWrap::tobool( $value)")
		("typename", "boolean")
		("varname", "$val")
	;
	typesystem.defineType( "bool $val=false")
		("argv_default", "false")
		("argv_map", "ValueVariantWrap::tobool( $value)")
		("typename", "boolean")
		("varname", "$val")
	;
	typesystem.defineType( "Index")
		("retv_map", "(void)papuga_add_CallResult_int( $name, $value);")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
	;
	typesystem.defineType( "const Index&")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
	;
	typesystem.defineType( "const Index& $val=0")
		("argv_default", "0")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
		("varname", "$val")
	;
	typesystem.defineType( "GlobalCounter")
		("retv_map", "(void)papuga_add_CallResult_int( $name, $value);")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
	;
	typesystem.defineType( "const GlobalCounter&")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
	;
	typesystem.defineType( "const GlobalCounter& $val=0")
		("argv_default", "0")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
		("varname", "$val")
	;
	typesystem.defineType( "int")
		("retv_map", "(void)papuga_add_CallResult_int( $name, $value);")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
	;
	typesystem.defineType( "int $val=0")
		("argv_default", "0")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
		("varname", "$val")
	;
	typesystem.defineType( "unsigned int")
		("retv_map", "(void)papuga_add_CallResult_int( $name, $value);")
		("argv_map", "ValueVariantWrap::touint( $value)")
		("typename", "integer")
	;
	typesystem.defineType( "unsigned int $val=0")
		("argv_default", "0")
		("argv_map", "ValueVariantWrap::touint( $value)")
		("typename", "integer")
		("varname", "$val")
	;
	typesystem.defineType( "long")
		("retv_map", "(void)papuga_add_CallResult_int( $name, $value);")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
	;
	typesystem.defineType( "long $val=0")
		("argv_default", "0")
		("argv_map", "ValueVariantWrap::toint( $value)")
		("typename", "integer")
		("varname", "$val")
	;
	typesystem.defineType( "float")
		("retv_map", "(void)papuga_add_CallResult_double( $name, $value);")
		("argv_map", "ValueVariantWrap::tofloat( $value)")
		("typename", "float")
	;
	typesystem.defineType( "double")
		("retv_map", "(void)papuga_add_CallResult_double( $name, $value);")
		("argv_map", "ValueVariantWrap::todouble( $value)")
		("typename", "float")
	;
	typesystem.defineType( "double $val=1")
		("argv_default", "1.0")
		("argv_map", "ValueVariantWrap::todouble( $value)")
		("typename", "float")
		("varname", "$val")
	;
	typesystem.defineType( "const ValueVariant& $val=ValueVariant()")
		("argv_default", "ValueVariant()")
		("argv_map", "$value")
		("typename", "any")
		("varname", "$val")
	;
	typesystem.defineType( "const ValueVariant&")
		("argv_map", "$value")
		("typename", "any")
	;
	typesystem.defineType( "const NumericVariant&")
		("argv_map", "ValueVariantWrap::tonumeric( $value)")
		("typename", "number")
	;
	typesystem.defineType( "const std::string& $val=\"\"")
		("argv_default", "std::string()")
		("argv_map", "ValueVariantWrap::tostring( $value)")
		("typename", "string")
		("varname", "$val")
	;
	typesystem.defineType( "const std::string&")
		("retv_map", "{const std::string& retvalstr = $value; (void)papuga_add_CallResult_string( $name, retvalstr.c_str(), retvalstr.size());}")
		("argv_map", "ValueVariantWrap::tostring( $value)")
		("typename", "string")
	;
	typesystem.defineType( "std::string")
		("retv_map", "{std::string retvalstr = $value; if (!papuga_add_CallResult_string_copy( $name, retvalstr.c_str(), retvalstr.size())) throw std::bad_alloc();}")
		("typename", "string")
	;
	typesystem.defineType( "std::string*")
		("retv_map", "{std::string* ptr_$name = $value; if (ptr_$name) initCallResultStructureOwnership( $name, ptr_$name);}")
		("typename", "string")
	;
	typesystem.defineType( "const char*")
		("retv_map", "{const char* ptr_$name = $value; if (ptr_$name) (void)papuga_add_CallResult_charp( $name, ptr_$name);}")
		("argv_decl", "std::string $name;")
		("argv_map", "ValueVariantWrap::tocharp( $name, $value)")
		("typename", "string")
	;
	typesystem.defineType( "const $objid~Impl*")
		("retv_map", "initCallResultObjectConst( $name, $value);")
		("argv_map", "ValueVariantWrap::toclass<const $objid~Impl>( $value)")
		("typename", "interface")
	;
	typesystem.defineType( "$objid~Impl*")
		("retv_map", "initCallResultObjectOwnership( $name, $value);")
		("argv_map", "ValueVariantWrap::toclass<$objid~Impl>( $value)")
		("typename", "interface")
	;
	typesystem.defineType( "analyzer::DocumentClass*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "analyzer::Document*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "TermExpression*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "SentenceTermExpression*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "MetaDataExpression*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "std::vector<std::pair<std::string,std::string> >*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "std::vector<std::string>*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "Struct")
		("retv_map", "initCallResultStruct( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "Iterator")
		("retv_map", "initCallResultIterator( $name, $value);")
		("typename", "iterator")
	;
	typesystem.defineType( "analyzer::ContentStatisticsResult*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "QueryResult*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "std::vector<VectorQueryResult>")
		("retv_map", "initCallResultAtomic( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "std::vector<Index>")
		("retv_map", "initCallResultAtomic( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "std::vector<double>")
		("retv_map", "initCallResultAtomic( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "std::vector<float>")
		("retv_map", "initCallResultAtomic( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "WordVector")
		("retv_map", "initCallResultAtomic( $name, $value);")
		("typename", "structure")
	;
	typesystem.defineType( "$objid~Interface*")
		("retv_map", "initCallResultStructureOwnership( $name, $value);")
		("typename", "interface")
	;
}

