/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_STRUCTURE_DEFINITIONS_HPP_INCLUDED
#define _STRUS_BINDINGS_STRUCTURE_DEFINITIONS_HPP_INCLUDED
#include "structNameMap.hpp"
#include "strus/bindings/serialization.hpp"

namespace strus {
namespace bindings {

struct AnalyzerFunctionDef
{
	std::string name;
	std::vector<std::string> args;

	AnalyzerFunctionDef(){}
	AnalyzerFunctionDef( const AnalyzerFunctionDef& o)
		:name(o.name),args(o.args){}

	void deserialize( Serialization::const_iterator& si, const Serialization::const_iterator& se);
};

struct TermDef
{
	std::string variable;
	std::string type;
	std::string value;
	unsigned int length;
	bool value_defined;
	bool length_defined;

	TermDef()
		:variable(),type(),value(),length(1),value_defined(false),length_defined(false){}
	TermDef( const TermDef& o)
		:variable(o.variable),type(o.type),value(o.value),length(o.length),value_defined(o.value_defined),length_defined(o.length_defined){}

	void deserialize( Serialization::const_iterator& si, const Serialization::const_iterator& se);
};

struct MetaDataRangeDef
{
	std::string from;
	std::string to;

	MetaDataRangeDef(){}
	MetaDataRangeDef( const MetaDataRangeDef& o)
		:from(o.from),to(o.to){}

	void deserialize( Serialization::const_iterator& si, const Serialization::const_iterator& se);
};

struct QueryEvalFunctionParameterDef
{
	enum Type {Undefined,Value,Feature};

	Type type;
	std::string name;
	ValueVariant value;

	QueryEvalFunctionParameterDef()
		:type(Undefined),name(),value(){}
	QueryEvalFunctionParameterDef( const QueryEvalFunctionParameterDef& o)
		:type(o.type),name(o.name),value(o.value){}

	void deserialize( Serialization::const_iterator& si, const Serialization::const_iterator& se);
};


struct ConfigDef
{
	std::string name;
	ValueVariant value;

	ConfigDef()
		:name(),value(){}
	ConfigDef( const ConfigDef& o)
		:name(o.name),value(o.value){}

	void deserialize( Serialization::const_iterator& si, const Serialization::const_iterator& se);
};

}} //namespace
#endif


