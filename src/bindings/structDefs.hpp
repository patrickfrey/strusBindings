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
#include "papuga/serialization.h"
#include "papuga/typedefs.h"
#include "strus/errorBufferInterface.hpp"
#include "strus/numericVariant.hpp"
#include "strus/metaDataRestrictionInterface.hpp"

namespace strus {
namespace bindings {

struct AnalyzerFunctionDef
{
	std::string name;
	std::vector<std::string> args;

	AnalyzerFunctionDef( papuga_SerializationIter& seriter);
	AnalyzerFunctionDef( const AnalyzerFunctionDef& o)
		:name(o.name),args(o.args){}
};

struct QueryTermDef
{
	std::string variable;
	std::string type;
	std::string value;
	unsigned int length;
	bool value_defined;
	bool length_defined;

	QueryTermDef( papuga_SerializationIter& seriter);
	QueryTermDef( const QueryTermDef& o)
		:variable(o.variable),type(o.type),value(o.value),length(o.length),value_defined(o.value_defined),length_defined(o.length_defined){}
};

struct MetaDataRangeDef
{
	std::string from;
	std::string to;

	MetaDataRangeDef( papuga_SerializationIter& seriter);
	MetaDataRangeDef( const MetaDataRangeDef& o)
		:from(o.from),to(o.to){}
};

struct ConfigDef
{
	std::string cfgstring;

	ConfigDef( papuga_SerializationIter& seriter);
	ConfigDef( const papuga_ValueVariant& def);
	ConfigDef( const ConfigDef& o)
		:cfgstring(o.cfgstring){}
private:
	void init( papuga_SerializationIter& seriter);
	std::string parseValue( papuga_SerializationIter& seriter) const;
	std::string parseValueTypeDefinitionList( papuga_SerializationIter& seriter) const;
};

struct KeyValueList
{
	typedef std::pair< std::string, const papuga_ValueVariant*> Item;
	std::vector<Item> items;

	/// \param[in] namemapdef string with comma separated key value name pair
	KeyValueList( const papuga_ValueVariant& def, const char* namemapdef);
	/// \param[in] namemapdef string with comma separated key value name pair
	KeyValueList( papuga_SerializationIter& seriter, const char* namemapdef);
	KeyValueList( const KeyValueList& o)
		:items(o.items){}

	typedef std::vector<Item>::const_iterator const_iterator;
	const_iterator begin() const	{return items.begin();}
	const_iterator end() const	{return items.end();}

private:
	void init( papuga_SerializationIter& seriter, const char* namemapdef);
	void parseMetaKeyValueList( papuga_SerializationIter& seriter);
	void parseDictionary( papuga_SerializationIter& seriter);
	void parseValueTupleList( papuga_SerializationIter& seriter);
};

struct DfChangeDef
{
	std::string termtype;
	std::string termvalue;
	int increment;

	DfChangeDef( papuga_SerializationIter& seriter);
	DfChangeDef( const DfChangeDef& o)
		:termtype(o.termtype),termvalue(o.termvalue),increment(o.increment){}
};

struct ContextDef
{
	unsigned int threads;
	std::string rpc;
	std::string trace;

	ContextDef()
		:threads(0),rpc(),trace(){}
	explicit ContextDef( const std::string& connstr)
		:threads(0),rpc(connstr),trace(){}
	ContextDef( papuga_SerializationIter& seriter);
	ContextDef( const papuga_ValueVariant& def);
	ContextDef( const ContextDef& o)
		:threads(o.threads),rpc(o.rpc),trace(o.trace){}

private:
	void init( papuga_SerializationIter& seriter);
};

struct SegmenterDef
{
	std::string segmenter;
	std::string mimetype;
	std::string encoding;
	std::string scheme;

	explicit SegmenterDef( const std::string& segmenter_)
		:segmenter(segmenter_){}
	SegmenterDef( const papuga_ValueVariant& def);
	SegmenterDef( papuga_SerializationIter& seriter);
	SegmenterDef( const SegmenterDef& o)
		:segmenter(o.segmenter),mimetype(o.mimetype),encoding(o.encoding),scheme(o.scheme){}
private:
	void init( papuga_SerializationIter& seriter);
};

struct MetaDataCompareDef
{
	MetaDataRestrictionInterface::CompareOperator cmpop;
	std::string name;
	papuga_ValueVariant value;

	MetaDataCompareDef( papuga_SerializationIter& seriter);
	MetaDataCompareDef( const MetaDataCompareDef& o)
		:cmpop(o.cmpop),name(o.name),value(o.value){}
};

/// \brief Patch according to https://tools.ietf.org/html/rfc6902
/// \note See also https://williamdurand.fr/2014/02/14/please-do-not-patch-like-an-idiot
/** \example for storageClient::patch
{
"patch": [
    {"op":"rename", "path":"metadata/code", "from":"cd"},
    {"op":"replace", "path":"metadata/id", "value":"INT32", "from":"id"},
    {"op":"add", "path":"metadata/doclen", "value":"INT32"},
    {"op":"replace", "path":"buffer_size", "value":"4K"}
]}
**/
struct PatchDef
{
	enum Op
	{
		OpAdd,
		OpRemove,
		OpReplace,
		OpMove,
		OpCopy,
		OpTest
	};
	Op op;
	std::vector<std::string> path;
	std::string value;
	std::vector<std::string> from;

	static Op getOpFromString( const char* val);
	static const char* opName( Op op)
	{
		static const char* ar[] = {"add","remove","replace","move","copy","test"};
		return ar[ (int)op];
	}
	static const StructureNameMap& opNameMap()
	{
		static const StructureNameMap namemap( "add,remove,replace,move,copy,test", ',');
		return namemap;
	}
	static std::vector<PatchDef> parseList( papuga_SerializationIter& seriter);
	static std::vector<PatchDef> parseList( const papuga_ValueVariant& value);

	PatchDef()
		:op(OpAdd),path(),value(),from(){}
	PatchDef( papuga_SerializationIter& seriter);
	PatchDef( const PatchDef& o)
		:op(o.op),path(o.path),value(o.value),from(o.from){}
};

}} //namespace
#endif


