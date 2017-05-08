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
#include "serialization.hpp"

namespace strus {
namespace bindings {

struct AnalyzerFunctionDef
{
	std::string name;
	std::vector<std::string> args;

	AnalyzerFunctionDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se);
	AnalyzerFunctionDef( const AnalyzerFunctionDef& o)
		:name(o.name),args(o.args){}
};

struct TermDef
{
	std::string variable;
	std::string type;
	std::string value;
	unsigned int length;
	bool value_defined;
	bool length_defined;

	TermDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se);
	TermDef( const TermDef& o)
		:variable(o.variable),type(o.type),value(o.value),length(o.length),value_defined(o.value_defined),length_defined(o.length_defined){}
};

struct MetaDataRangeDef
{
	std::string from;
	std::string to;

	MetaDataRangeDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se);
	MetaDataRangeDef( const MetaDataRangeDef& o)
		:from(o.from),to(o.to){}
};

struct ConfigDef
{
	std::string name;
	papuga_ValueVariant value;

	ConfigDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se);
	ConfigDef( const ConfigDef& o)
		:name(o.name),value(o.value){}
};

struct DfChangeDef
{
	const char* termtype;
	const char* termvalue;
	int increment;

	DfChangeDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se);
	DfChangeDef( const DfChangeDef& o)
		:termtype(o.termtype),termvalue(o.termvalue),increment(o.increment){}
};

struct ContextDef
{
	unsigned int threads;
	std::string rpc;
	std::string trace;

	ContextDef( const std::string& connstr)
		:threads(0),rpc(connstr){}
	ContextDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se);
	ContextDef( const ContextDef& o)
		:threads(o.threads),rpc(o.rpc),trace(o.trace){}
};

struct SegmenterDef
{
	std::string segmenter;
	std::string mimetype;
	std::string encoding;
	std::string scheme;

	SegmenterDef( const std::string& segmenter_)
		:segmenter(segmenter_){}
	SegmenterDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se);
	SegmenterDef( const SegmenterDef& o)
		:segmenter(o.segmenter),mimetype(o.mimetype),encoding(o.encoding),scheme(o.scheme){}
};

}} //namespace
#endif


