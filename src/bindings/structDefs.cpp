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
				case 1: args = Deserializer::getStringList( seriter); break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
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
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
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

MetaDataRangeDef::MetaDataRangeDef( papuga_SerializationIter& seriter)
	:from(),to()
{
	static const char* context = _TXT("metadata range");
	static const StructureNameMap namemap( "from,to", ',');

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		do
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0: from = Deserializer::getString( seriter); break;
				case 1:	to = Deserializer::getString( seriter); break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
			}
		} while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		from = Deserializer::getString( seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			to = Deserializer::getString( seriter);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("expected %s structure"), context);
	}
}

KeyValueList::KeyValueList( const papuga_ValueVariant& def)
	:items()
{
	static const char* context = _TXT("named key value pair list");
	if (def.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error( _TXT("list of key value pairs expected"));
	}
	papuga_SerializationIter seriter;
	papuga_init_SerializationIter( &seriter, def.value.serialization);
	init( seriter);
	if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);
}

KeyValueList::KeyValueList( papuga_SerializationIter& seriter)
	:items()
{
	init( seriter);
}

void KeyValueList::parseMetaKeyValueList( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("named key value pair list");
	static const StructureNameMap namemap( "name,value", ',');
	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);

			std::string name;
			const papuga_ValueVariant* value;
			unsigned char defined[2] = {0,0};
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
				papuga_SerializationIter_skip( &seriter);
				switch (idx)
				{
					case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "name", context);
						name = Deserializer::getString( seriter);
						break;
					case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "value", context);
						value = getValue( seriter);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'name' or 'value' expected"), context);
				}
			}
			Deserializer::consumeClose( seriter);
			if (!defined[0] || !defined[1])
			{
				throw strus::runtime_error(_TXT("incomplete %s definition"), context);
			}
			items.push_back( Item( name, value));
		}
		else
		{
			throw strus::runtime_error(_TXT("expected %s structure"), context);
		}
	}
}

void KeyValueList::parseDictionary( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("name value pair list");
	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			std::string name = ValueVariantWrap::tostring( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			const papuga_ValueVariant* value = getValue( seriter);
			items.push_back( Item( name, value));
		}
		else
		{
			throw strus::runtime_error(_TXT("expected only name value pairs in %s structure"), context);
		}
	}
}

void KeyValueList::parseValueTupleList( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("value tuple list");
	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);

			if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				std::string name = Deserializer::getString( seriter);
				const papuga_ValueVariant* value = getValue( seriter);
				items.push_back( Item( name, value));
			}
			else
			{
				throw strus::runtime_error(_TXT("expected only value pairs in %s structure"), context);
			}
			Deserializer::consumeClose( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("expected only value pairs in %s structure"), context);
		}
	}
}

void KeyValueList::init( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("key value pair list");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter sernext;
		papuga_init_SerializationIter_copy( &sernext, &seriter);
		papuga_SerializationIter_skip( &sernext);

		if (papuga_SerializationIter_tag( &sernext) == papuga_TagName)
		{
			parseMetaKeyValueList( seriter);
		}
		else if (papuga_SerializationIter_tag( &sernext) == papuga_TagValue)
		{
			parseValueTupleList( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("expected %s structure"), context);
		}
		Deserializer::consumeClose( seriter);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		parseDictionary( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected %s structure"), context);
	}
}

ConfigDef::ConfigDef( papuga_SerializationIter& seriter)
	:cfgstring()
{
	static const char* context = _TXT("configuration");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		cfgstring = Deserializer::getString( seriter);
	}
	else
	{
		LocalErrorBuffer errorhnd;
		KeyValueList kvmap( seriter);
		KeyValueList::const_iterator ki = kvmap.begin(), ke = kvmap.end();
		for (; ki != ke; ++ki)
		{
			if (!addConfigStringItem( cfgstring, ki->first, ValueVariantWrap::tostring( *ki->second), &errorhnd))
			{
				throw strus::runtime_error( _TXT("error parsing %s: %s"), context, errorhnd.fetchError());
			}
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
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "type", context);
					termtype = Deserializer::getString( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "value", context);
					termvalue = Deserializer::getString( seriter);
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "increment", context);
					increment = Deserializer::getInt( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s definition, 'type', 'value' or 'increment' expected"), context);
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
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "threads", context);
					threads = Deserializer::getUint( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "rpc", context);
					rpc = Deserializer::getSubConfigString( seriter);
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "trace", context);
					trace = Deserializer::getSubConfigString( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'threads' or 'rpc' or 'trace' expected"), context);
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
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "segmenter", context);
					if (0!=(defined[1]|defined[2]|defined[3])) throw strus::runtime_error(_TXT("mixed definition of '%s' and %s in %s"), "segmenter", "'scheme', 'mimetype' and 'encoding'", context);
					segmenter = Deserializer::getString( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "mimetype", context);
					if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "mimetype", context);
					mimetype = Deserializer::getString( seriter);
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "encoding", context);
					if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "encoding", context);
					encoding = Deserializer::getString( seriter);
					break;
				case 3:	if (defined[3]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "scheme", context);
					if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "scheme", context);
					scheme = Deserializer::getString( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'segmenter' or 'mimetype' or 'encoding' or 'scheme' expected"), context);
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
				case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "op", context);
					cmpop = Deserializer::getMetaDataCmpOp( seriter);
					break;
				case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "name", context);
					name = Deserializer::getString( seriter);
					break;
				case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "value", context);
					papuga_init_ValueVariant_value( &value, getValue( seriter));
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'op' or 'name' or 'value' expected"), context);
			}
		}
		if (!defined[0]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "op", context);
		if (!defined[1]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "name", context);
		if (!defined[2]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "value", context);
	}
}

