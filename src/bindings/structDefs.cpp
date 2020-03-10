/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "structDefs.hpp"
#include "private/internationalization.hpp"
#include "deserializer.hpp"
#include "papuga/serialization.h"
#include "papuga/valueVariant.h"
#include "valueVariantWrap.hpp"
#include "strus/base/localErrorBuffer.hpp"
#include "strus/base/configParser.hpp"

using namespace strus;
using namespace strus::bindings;

static const papuga_ValueVariant* getValue(
		papuga_SerializationIter& seriter)
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		const papuga_ValueVariant* rt = papuga_SerializationIter_value( &seriter);
		papuga_SerializationIter_skip( &seriter);
		return rt;
	}
	else
	{
		throw strus::runtime_error( _TXT("unexpected token in serialization, expected value"));
	}
}

AnalyzerFunctionDef::AnalyzerFunctionDef( papuga_SerializationIter& seriter)
	:name(),args()
{
	static const char* context = _TXT("analyzer function");
	static const StructureNameMap namemap( "name,arg", ',');

	bool name_defined = false;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		do
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: name_defined = true; name = Deserializer::getString( seriter); break;
				case 1: args = Deserializer::getStringListAsValue( seriter); break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		} while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		name = Deserializer::getString( seriter);
		name_defined = true;
		while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			args.push_back( Deserializer::getString( seriter));
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("%s definition structure expected"), context);
	}
	if (!name_defined)
	{
		throw strus::runtime_error(_TXT("incomplete %s definition"), context);
	}
}

QueryTermDef::QueryTermDef( papuga_SerializationIter& seriter)
	:variable(),type(),value(),length(1),value_defined(false),length_defined(false)
{
	static const char* context = _TXT("term");
	static const StructureNameMap namemap( "type,value,variable,len", ',');

	bool type_defined = false;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		do
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: type_defined = true; type = Deserializer::getString( seriter); break;
				case 1: value_defined = true; value = Deserializer::getString( seriter); break;
				case 2: variable = Deserializer::getString( seriter); break;
				case 3: length_defined = true; length = Deserializer::getUint( seriter); break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		} while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
	}
	else
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter sernext;
			papuga_init_SerializationIter_copy( &sernext, &seriter);
			papuga_SerializationIter_skip( &sernext);
			const papuga_ValueVariant* variable_ = Deserializer::getOptionalDefinition( sernext, "variable");
			if (variable_)
			{
				variable = ValueVariantWrap::tostring( *variable_);
				Deserializer::consumeClose( sernext);
				papuga_init_SerializationIter_copy( &seriter, &sernext);
			}
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			type = Deserializer::getString( seriter);
			type_defined = true;

			if (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
			{
				value = Deserializer::getString( seriter);
				value_defined = true;

				if (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
				{
					length_defined = true;
					length = Deserializer::getUint( seriter);
				}
			}
			else
			{
				throw strus::runtime_error(_TXT("%s definition expected"), context);
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("%s definition expected"), context);
		}
	}
	if (!value_defined && length_defined)
	{
		throw strus::runtime_error(_TXT("incomplete %s definition"), context);
	}
	if (!type_defined)
	{
		throw strus::runtime_error(_TXT("incomplete %s definition"), context);
	}
}

KeyValueList::KeyValueList( const papuga_ValueVariant& def, const char* namemapdef)
	:items()
{
	static const char* context = _TXT("named key value pair list");
	if (!papuga_ValueVariant_defined( &def))
	{
		return;
	}
	else if (def.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error( _TXT("list of key value pairs expected"));
	}
	papuga_SerializationIter seriter;
	papuga_init_SerializationIter( &seriter, def.value.serialization);
	if (papuga_SerializationIter_eof( &seriter))
	{
		return;
	}
	init( seriter, namemapdef);
	if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);
}

KeyValueList::KeyValueList( papuga_SerializationIter& seriter, const char* namemapdef)
	:items()
{
	static const char* context = _TXT("named key value pair list");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		init( seriter, namemapdef);
		Deserializer::consumeClose( seriter);
	}
	else
	{
		throw strus::runtime_error( _TXT("structure expected for %s"), context);
	}
}

static void KeyValueList_appendKeyValueDefinition( std::vector<KeyValueList::Item>& res, const std::string& name, const papuga_ValueVariant* value)
{
	res.push_back( KeyValueList::Item( name, value));
}

void KeyValueList::init( papuga_SerializationIter& seriter, const char* namemapdef)
{
	static const char* context = _TXT("key value pair list");
	static const StructureNameMap namemap( namemapdef, ',');

	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				// [A] tuple definition:
				std::string name = Deserializer::getString( seriter);
				const papuga_ValueVariant* value = getValue( seriter);
				KeyValueList_appendKeyValueDefinition( items, name, value);
			}
			else
			{
				// [B] meta definition:
				std::string name;
				const papuga_ValueVariant* value = 0;
				unsigned char defined[2] = {0,0};
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
				{
					int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
					papuga_SerializationIter_skip( &seriter);
					switch (idx)
					{
						case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
							name = Deserializer::getString( seriter);
							break;
						case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
							value = getValue( seriter);
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
					}
				}
				if (!defined[0] || !defined[1])
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				KeyValueList_appendKeyValueDefinition( items, name, value);
			}
			Deserializer::consumeClose( seriter);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			// [C] dictionary definition:
			std::string name = ValueVariantWrap::tostring( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			const papuga_ValueVariant* value = getValue( seriter);
			KeyValueList_appendKeyValueDefinition( items, name, value);
		}
		else
		{
			throw strus::runtime_error(_TXT("expected key/value pair in %s definition"), context);
		}
	}
}

static void ConfigDef_appendValueTypeDefinition( std::string& res, const std::string& name, const std::string& type)
{
	if (!res.empty()) res.append(", ");
	res.append( name);
	res.push_back( ' ');
	res.append( type);
}

std::string ConfigDef::parseValueTypeDefinitionList( papuga_SerializationIter& seriter) const
{
	std::string rt;
	static const char* context = _TXT("configuration value type definition list");
	static const StructureNameMap namemap( "name,type", ',');
	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				// [A] tuple definition:
				std::string name = Deserializer::getString( seriter);
				std::string type = Deserializer::getString( seriter);
				ConfigDef_appendValueTypeDefinition( rt, name, type);
			}
			else
			{
				// [B] meta definition:
				std::string name;
				std::string type;
				unsigned char defined[2] = {0,0};
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
				{
					int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
					papuga_SerializationIter_skip( &seriter);
					switch (idx)
					{
						case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
							name = Deserializer::getString( seriter);
							break;
						case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
							type = Deserializer::getString( seriter);
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
					}
				}
				if (!defined[0] || !defined[1])
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				ConfigDef_appendValueTypeDefinition( rt, name, type);
			}
			Deserializer::consumeClose( seriter);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			// [C] dictionary definition:
			std::string name = ValueVariantWrap::tostring( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			std::string type = Deserializer::getString( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("expected key/value pair in %s definition"), context);
		}
	}
	return rt;
}

std::string ConfigDef::parseValue( papuga_SerializationIter& seriter) const
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		std::string rt;
		rt.push_back( '\"');
		rt.append( parseValueTypeDefinitionList( seriter));
		rt.push_back( '\"');
		Deserializer::consumeClose( seriter);
		return rt;
	}
	else if (papuga_SerializationIter_value(&seriter)->valuetype == papuga_TypeString)
	{
		std::string rt;
		rt.push_back( '\"');
		rt.append( Deserializer::getString( seriter));
		rt.push_back( '\"');
		return rt;
	}
	else
	{
		return Deserializer::getString( seriter);
	}
}

static void ConfigDef_appendKeyValueDefinition( std::string& res, const std::string& name, const std::string& value)
{
	if (!res.empty()) res.append("; ");
	res.append( name);
	res.push_back( '=');
	res.append( value);
}

ConfigDef::ConfigDef( const papuga_ValueVariant& def)
{
	static const char* context = _TXT("configuration");
	if (def.valuetype != papuga_TypeSerialization)
	{
		cfgstring = ValueVariantWrap::tostring( def);
	}
	else
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, def.value.serialization);
		init( seriter);
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);
	}
}

ConfigDef::ConfigDef( papuga_SerializationIter& seriter)
	:cfgstring()
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		init( seriter);
		Deserializer::consumeClose( seriter);
	}
	else
	{
		cfgstring = Deserializer::getString( seriter);
	}
}

void ConfigDef::init( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("configuration");
	static const StructureNameMap namemap( "name,value", ',');

	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				// [A] tuple definition:
				std::string name = Deserializer::getString( seriter);
				std::string value = ConfigDef::parseValue( seriter);
				ConfigDef_appendKeyValueDefinition( cfgstring, name, value);
			}
			else
			{
				// [B] meta definition:
				std::string name;
				std::string value;
				unsigned char defined[2] = {0,0};
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
				{
					int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
					papuga_SerializationIter_skip( &seriter);
					switch (idx)
					{
						case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
							name = Deserializer::getString( seriter);
							break;
						case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
							value = ConfigDef::parseValue( seriter);
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
					}
				}
				if (!defined[0] || !defined[1])
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				ConfigDef_appendKeyValueDefinition( cfgstring, name, value);
			}
			Deserializer::consumeClose( seriter);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			// [C] dictionary definition:
			std::string name = ValueVariantWrap::tostring( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			std::string value = ConfigDef::parseValue( seriter);
			ConfigDef_appendKeyValueDefinition( cfgstring, name, value);
		}
		else
		{
			throw strus::runtime_error(_TXT("expected key/value pair in %s definition"), context);
		}
	}
}

DfChangeDef::DfChangeDef( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("document frequency change");
	static const StructureNameMap namemap( "type,value,increment", ',');
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		termtype = Deserializer::getString( seriter);
		termvalue = Deserializer::getString( seriter);
		increment = Deserializer::getInt( seriter);
	}
	else
	{
		unsigned char defined[3] = {0,0};
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					termtype = Deserializer::getString( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					termvalue = Deserializer::getString( seriter);
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					increment = Deserializer::getInt( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		}
		if (!defined[0] || !defined[1] || !defined[2])
		{
			throw strus::runtime_error(_TXT("incomplete %s definition"), context);
		}
	}
}

ContextDef::ContextDef( const ValueVariant& ctx)
	:threads(0),rpc(),trace()
{
	static const char* context = _TXT("context configuration");
	if (!papuga_ValueVariant_defined( &ctx))
	{
	}
	else if (papuga_ValueVariant_isstring( &ctx))
	{
		papuga_ErrorCode err = papuga_Ok;
		rpc = papuga::ValueVariant_tostring( ctx, err);
		if (err != papuga_Ok) throw papuga::error_exception( err, context);
	}
	else if (ctx.valuetype == papuga_TypeSerialization)
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, ctx.value.serialization);
		init( seriter);
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);
	}
	else
	{
		throw strus::runtime_error( _TXT("expected string or structure or an undefined value for %s"), context);
	}
}

ContextDef::ContextDef( papuga_SerializationIter& seriter)
	:threads(0),rpc(),trace()
{
	init( seriter);
}

void ContextDef::init( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("context configuration");
	static const StructureNameMap namemap( "threads,rpc,trace", ',');

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		rpc = Deserializer::getString( seriter);
	}
	else
	{
		unsigned char defined[3] = {0,0,0};
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					threads = Deserializer::getUint( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					rpc = ConfigDef( seriter).cfgstring;
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					trace = ConfigDef( seriter).cfgstring;
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		}
	}
}

SegmenterDef::SegmenterDef( papuga_SerializationIter& seriter)
	:segmenter(),mimetype(),encoding(),scheme()
{
	init( seriter);
}

SegmenterDef::SegmenterDef( const papuga_ValueVariant& def)
	:segmenter(),mimetype(),encoding(),scheme()
{
	static const char* context = _TXT("segmenter definition");
	if (papuga_ValueVariant_isstring( &def))
	{
		papuga_ErrorCode err = papuga_Ok;
		segmenter = papuga::ValueVariant_tostring( def, err);
		if (err != papuga_Ok) throw papuga::error_exception( err, "segmenter definition");
	}
	else if (def.valuetype == papuga_TypeSerialization)
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, def.value.serialization);
		init( seriter);
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);
	}
	else
	{
		throw strus::runtime_error( _TXT("expected string or document class structure for document segmenter definition"));
	}	
}

void SegmenterDef::init( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("segmenter configuration");
	static const StructureNameMap namemap( "segmenter,mimetype,encoding,scheme", ',');

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		mimetype = Deserializer::getString( seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			encoding = Deserializer::getString( seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			scheme = Deserializer::getString( seriter);
		}
	}
	else
	{
		unsigned char defined[4] = {0,0,0,0};
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					if (0!=(defined[1]|defined[2]|defined[3])) throw strus::runtime_error(_TXT("mixed definition of '%s' and %s in %s"), "segmenter", "'scheme', 'mimetype' and 'encoding'", context);
					segmenter = Deserializer::getString( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "mimetype", context);
					mimetype = Deserializer::getString( seriter);
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "encoding", context);
					encoding = Deserializer::getString( seriter);
					break;
				case 3:	if (defined[3]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "scheme", context);
					scheme = Deserializer::getString( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		}
	}
}

MetaDataCompareDef::MetaDataCompareDef( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("meta data compare");
	static const StructureNameMap namemap( "op,name,value", ',');

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		cmpop = Deserializer::getMetaDataCmpOp( seriter);
		name = Deserializer::getString( seriter);
		const papuga_ValueVariant* valueref = getValue( seriter);
		papuga_init_ValueVariant_value( &value, valueref);
	}
	else
	{
		unsigned char defined[3] = {0,0,0};
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					cmpop = Deserializer::getMetaDataCmpOp( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					name = Deserializer::getString( seriter);
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					papuga_init_ValueVariant_value( &value, getValue( seriter));
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		}
		if (!defined[0]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "op", context);
		if (!defined[1]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "name", context);
		if (!defined[2]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "value", context);
	}
}

static MetaDataTableCommand::Id getMetaDataTableCommandIdFromName( const std::string& name)
{
	static const StructureNameMap namemap( "add,replace,remove,clear", ',');
	int idx = namemap.index( name);
	if (idx < 0) throw std::runtime_error(_TXT("undefined meta table command op"));
	return (MetaDataTableCommand::Id)idx;
}

MetaDataTableCommand::MetaDataTableCommand( papuga_SerializationIter& seriter)
	:id(Remove),name(),type(),oldname()
{
	static const char* context = _TXT("meta data table command");
	static const StructureNameMap namemap( "op,name,type,oldname", ',');

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		id = getMetaDataTableCommandIdFromName( Deserializer::getString( seriter));
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			name = Deserializer::getString( seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			if (papuga_ValueVariant_defined( papuga_SerializationIter_value( &seriter)))
			{
				type = Deserializer::getString( seriter);
			}
			else
			{
				papuga_SerializationIter_skip( &seriter);
			}
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			oldname = Deserializer::getString( seriter);
		}
	}
	else
	{
		unsigned char defined[4] = {0,0,0,0};
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					id = getMetaDataTableCommandIdFromName( Deserializer::getString( seriter));
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					name = Deserializer::getString( seriter);
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					type = Deserializer::getString( seriter);
					break;
				case 3:	if (defined[3]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					oldname = Deserializer::getString( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		}
		if (!defined[0]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "op", context);
	}
}

static MetaDataTableCommand parseMetaDataTableEntryDef( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("meta data table entry");
	static const StructureNameMap namemap( "name,type", ',');

	std::string name;
	std::string type;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		name = Deserializer::getString( seriter);
		type = Deserializer::getString( seriter);
	}
	else
	{
		unsigned char defined[2] = {0,0};
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0:	if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					name = Deserializer::getString( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					type = Deserializer::getString( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		}
		if (!defined[0]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "name", context);
		if (!defined[1]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "type", context);
	}
	return MetaDataTableCommand( MetaDataTableCommand::Add, name, type, std::string());
}

std::vector<MetaDataTableCommand> MetaDataTableCommand::getList( const papuga_ValueVariant& arg)
{
	static const char* context = _TXT("meta data table command list");
	if (!papuga_ValueVariant_defined( &arg))
	{
		return std::vector<MetaDataTableCommand>();
	}
	else if (arg.valuetype == papuga_TypeSerialization)
	{
		std::vector<MetaDataTableCommand> rt;
		papuga_SerializationIter seriter;
	
		papuga_init_SerializationIter( &seriter, arg.value.serialization);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue
		||  papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			MetaDataTableCommand cmd( seriter);
			rt.push_back( cmd);
			if (!papuga_SerializationIter_eof( &seriter))
			{
				throw strus::runtime_error(_TXT("expected eof after definition of '%s'"), context);
			}
		}
		else if (papuga_SerializationIter_eof( &seriter))
		{}
		else
		{
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				papuga_SerializationIter_skip( &seriter);
				MetaDataTableCommand cmd( seriter);
				rt.push_back( cmd);
				Deserializer::consumeClose( seriter);
			}
			if (!papuga_SerializationIter_eof( &seriter))
			{
				throw strus::runtime_error(_TXT("expected eof after definition of '%s'"), context);
			}
		}
		return rt;
	}
	else
	{
		throw strus::runtime_error(_TXT("expected structure for '%s'"), context);
	}
}

std::vector<MetaDataTableCommand> MetaDataTableCommand::getListFromNameTypePairs( const papuga_ValueVariant& arg)
{
	static const char* context = _TXT("meta data table definiton");
	if (!papuga_ValueVariant_defined( &arg))
	{
		return std::vector<MetaDataTableCommand>();
	}
	else if (arg.valuetype == papuga_TypeSerialization)
	{
		std::vector<MetaDataTableCommand> rt;
		papuga_SerializationIter seriter;
	
		papuga_init_SerializationIter( &seriter, arg.value.serialization);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue
		||  papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			MetaDataTableCommand cmd = parseMetaDataTableEntryDef( seriter);
			rt.push_back( cmd);
			if (!papuga_SerializationIter_eof( &seriter))
			{
				throw strus::runtime_error(_TXT("expected eof after definition of '%s'"), context);
			}
		}
		else if (papuga_SerializationIter_eof( &seriter))
		{}
		else
		{
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				papuga_SerializationIter_skip( &seriter);
				MetaDataTableCommand cmd = parseMetaDataTableEntryDef( seriter);
				rt.push_back( cmd);
				Deserializer::consumeClose( seriter);
			}
			if (!papuga_SerializationIter_eof( &seriter))
			{
				throw strus::runtime_error(_TXT("expected eof after definition of '%s'"), context);
			}
		}
		return rt;
	}
	else
	{
		throw strus::runtime_error(_TXT("expected structure for '%s'"), context);
	}
}


static std::pair<std::string,QueryFeatureExpansionDef> parseFeatureExpansionSimilarityDef( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("feature expansion similarity declaration");
	static const StructureNameMap namemap( "type,sim,results,cut", ',');

	std::pair<std::string,QueryFeatureExpansionDef> rt;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		rt.first = Deserializer::getString( seriter);
		rt.second.similarity = Deserializer::getDouble( seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			rt.second.maxNofResults = Deserializer::getInt( seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			rt.second.minNormalizedWeight = Deserializer::getDouble( seriter);
		}
	}
	else
	{
		unsigned char defined[4] = {0,0,0,0};
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0:	if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					rt.first = Deserializer::getString( seriter);
					break;
				case 1:	if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					rt.second.similarity = Deserializer::getDouble( seriter);
					break;
				case 2:	if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					rt.second.maxNofResults = Deserializer::getInt( seriter);
					break;
				case 3:	if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), namemap.name(idx), context);
					rt.second.minNormalizedWeight = Deserializer::getDouble( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, one of {%s} expected"), context, namemap.names());
			}
		}
		if (!defined[0]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), namemap.name(0), context);
		if (!defined[1]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), namemap.name(1), context);
	}
	if (rt.second.similarity < 0.0 || rt.second.similarity > 1.0) throw strus::runtime_error(_TXT("value between 0.0 and 1.0 expected for '%s' (got %.5f)"), namemap.name(1), rt.second.similarity);
	if (rt.second.maxNofResults <= 0) throw strus::runtime_error(_TXT("value greater than 0 expected for '%s' (got %d)"), namemap.name(2), rt.second.maxNofResults);
	if (rt.second.minNormalizedWeight < 0.0 || rt.second.minNormalizedWeight > 1.0) throw strus::runtime_error(_TXT("value between 0.0 and 1.0 expected for '%s' (got %.5f)"), namemap.name(3), rt.second.minNormalizedWeight);
	return rt;
}

void QueryFeatureExpansionMap::init( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("feature expansion similarity declaration");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue
	||  papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		QueryFeatureExpansionMap::value_type elem = parseFeatureExpansionSimilarityDef( seriter);
		insert( elem);
		if (!papuga_SerializationIter_eof( &seriter))
		{
			throw strus::runtime_error(_TXT("expected eof after definition of '%s'"), context);
		}
	}
	else if (papuga_SerializationIter_eof( &seriter))
	{}
	else
	{
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			QueryFeatureExpansionMap::value_type elem = parseFeatureExpansionSimilarityDef( seriter);
			insert( elem);
			Deserializer::consumeClose( seriter);
		}
		if (!papuga_SerializationIter_eof( &seriter))
		{
			throw strus::runtime_error(_TXT("expected eof after definition of '%s'"), context);
		}
	}
}

QueryFeatureExpansionMap::QueryFeatureExpansionMap( const papuga_ValueVariant& value)
{
	static const char* context = _TXT("feature expansion similarity declaration");
	if (!papuga_ValueVariant_defined( &value))
	{}
	else if (value.valuetype == papuga_TypeSerialization)
	{
		std::vector<MetaDataTableCommand> rt;
		papuga_SerializationIter seriter;
	
		papuga_init_SerializationIter( &seriter, value.value.serialization);
		init( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected structure for '%s'"), context);
	}
}
