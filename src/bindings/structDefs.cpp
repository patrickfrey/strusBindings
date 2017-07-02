/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "structDefs.hpp"
#include "internationalization.hpp"
#include "deserializer.hpp"
#include "papuga/serialization.hpp"
#include "papuga/valueVariant.h"
#include "valueVariantWrap.hpp"
#include "strus/base/localErrorBuffer.hpp"
#include "strus/base/configParser.hpp"

using namespace strus;
using namespace strus::bindings;

static const papuga_ValueVariant* getValue(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	if (si != se && si->tag == papuga_TagValue)
	{
		const papuga_ValueVariant* rt = &si->value;
		++si;
		return rt;
	}
	else
	{
		throw strus::runtime_error(_TXT("unexpected token in serialization, expected value"));
	}
}

AnalyzerFunctionDef::AnalyzerFunctionDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
	:name(),args()
{
	static const char* context = _TXT("analyzer function");
	static const StructureNameMap namemap( "name,arg", ',');
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

	bool name_defined = false;
	if (si->tag == papuga_TagValue)
	{
		name_defined = true;
		name = Deserializer::getString( si, se);
	}
	else if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		if (si->tag == papuga_TagName)
		{
			do
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: name_defined = true;
						name = Deserializer::getString( si, se);
						break;
					case 1: args = Deserializer::getStringList( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				}
			} while (si != se && si->tag == papuga_TagName);
		}
		else if (si->tag == papuga_TagValue)
		{
			name = Deserializer::getString( si, se);
			name_defined = true;
			if (si != se && si->tag != papuga_TagClose)
			{
				args = Deserializer::getStringList( si, se);
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("%s definition structure expected"), context);
		}
		Deserializer::consumeClose( si, se);
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

QueryTermDef::QueryTermDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
	:variable(),type(),value(),length(1),value_defined(false),length_defined(false)
{
	static const char* context = _TXT("term");
	static const StructureNameMap namemap( "type,value,variable,len", ',');
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

	bool type_defined = false;
	if (si->tag == papuga_TagValue)
	{
		type = Deserializer::getString( si, se);
		type_defined = true;
	}
	else if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		if (si->tag == papuga_TagName)
		{
			do
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: type_defined = true;
						type = Deserializer::getString( si, se);
						break;
					case 1: value_defined = true;
						value = Deserializer::getString( si, se);
						break;
					case 2: variable = Deserializer::getString( si, se);
						break;
					case 3: length_defined = true;
						length = Deserializer::getUint( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				}
			} while (si != se && si->tag == papuga_TagName);
		}
		else if (si->tag == papuga_TagClose)
		{
			throw strus::runtime_error(_TXT("%s definition expected"), context);
		}
		else
		{
			if (si->tag == papuga_TagOpen)
			{
				++si;
				const papuga_ValueVariant* variable_
					= Deserializer::getOptionalDefinition( si, se, "variable");
				if (variable_)
				{
					variable = ValueVariantWrap::tostring( *variable_);
				}
				Deserializer::consumeClose( si, se);
			}
			if (si->tag == papuga_TagValue)
			{
				type = Deserializer::getString( si, se);
				type_defined = true;
	
				if (si != se && si->tag != papuga_TagClose)
				{
					value = Deserializer::getString( si, se);
					value_defined = true;
	
					if (si != se && si->tag != papuga_TagClose)
					{
						length_defined = true;
						length = Deserializer::getUint( si, se);
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
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("%s definition expected"), context);
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

MetaDataRangeDef::MetaDataRangeDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
	:from(),to()
{
	static const char* context = _TXT("metadata range");
	static const StructureNameMap namemap( "from,to", ',');
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

	if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		if (si->tag == papuga_TagName)
		{
			do
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: from = Deserializer::getString( si, se);
						break;
					case 1:	to = Deserializer::getString( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				}
			} while (si != se && si->tag == papuga_TagName);
		}
		else if (si->tag == papuga_TagValue)
		{
			from = Deserializer::getString( si, se);
			if (si != se && si->tag == papuga_TagValue)
			{
				to = Deserializer::getString( si, se);
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("expected %s structure"), context);
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected %s structure"), context);
	}
}

void KeyValueList::parseMetaKeyValueList( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	static const char* context = _TXT("named key value pair list");
	static const StructureNameMap namemap( "name,value", ',');
	while (si != se && si->tag != papuga_TagClose)
	{
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);
		if (si->tag == papuga_TagOpen)
		{
			++si;
			if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

			std::string name;
			const papuga_ValueVariant* value;
			unsigned char defined[2] = {0,0};
			while (si->tag == papuga_TagName)
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "name", context);
						name = Deserializer::getString( si, se);
						break;
					case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "value", context);
						value = getValue( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'name' or 'value' expected"), context);
				}
			}
			Deserializer::consumeClose( si, se);
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

void KeyValueList::parseDictionary( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	static const char* context = _TXT("name value pair list");
	while (si != se && si->tag != papuga_TagClose)
	{
		if (si->tag == papuga_TagName)
		{
			std::string name = ValueVariantWrap::tostring( si->value);
			++si;
			const papuga_ValueVariant* value = getValue( si, se);
			items.push_back( Item( name, value));
		}
		else
		{
			throw strus::runtime_error(_TXT("expected only name value pairs in %s structure"), context);
		}
	}
}

void KeyValueList::parseValueTupleList( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	static const char* context = _TXT("value tuple list");
	while (si != se && si->tag != papuga_TagClose)
	{
		if (si->tag == papuga_TagOpen)
		{
			++si;
			if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

			if (si->tag == papuga_TagValue)
			{
				std::string name = Deserializer::getString( si, se);
				const papuga_ValueVariant* value = getValue( si, se);
				items.push_back( Item( name, value));
			}
			else
			{
				throw strus::runtime_error(_TXT("expected only value pairs in %s structure"), context);
			}
			Deserializer::consumeClose( si, se);
		}
		else
		{
			throw strus::runtime_error(_TXT("expected only value pairs in %s structure"), context);
		}
	}
}

KeyValueList::KeyValueList( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
	:items()

{
	static const char* context = _TXT("key value pair list");
	if (si == se) return;
	if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		if (si->tag == papuga_TagOpen)
		{
			if ((si+1)->tag == papuga_TagName)
			{
				parseMetaKeyValueList( si, se);
			}
			else if ((si+1)->tag == papuga_TagValue)
			{
				parseValueTupleList( si, se);
			}
			else
			{
				throw strus::runtime_error(_TXT("expected %s structure"), context);
			}
			Deserializer::consumeClose( si, se);
		}
		else if (si->tag == papuga_TagName)
		{
			parseDictionary( si, se);
		}
		else
		{
			throw strus::runtime_error(_TXT("expected %s structure"), context);
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected %s structure"), context);
	}
}

ConfigDef::ConfigDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
	:cfgstring()
{
	if (si == se) return;
	if (si->tag == papuga_TagOpen)
	{
		LocalErrorBuffer errorhnd;
		KeyValueList kvmap( si, se);
		KeyValueList::const_iterator ki = kvmap.begin(), ke = kvmap.end();
		for (; ki != ke; ++ki)
		{
			if (!addConfigStringItem( cfgstring, ki->first, ValueVariantWrap::tostring( *ki->second), &errorhnd))
			{
				throw strus::runtime_error( _TXT("error parsing configuration: %s"), errorhnd.fetchError());
			}
		}
	}
	else if (si->tag == papuga_TagValue)
	{
		cfgstring = Deserializer::getString( si, se);
	}
}


DfChangeDef::DfChangeDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	static const char* context = _TXT("document frequency change");
	static const StructureNameMap namemap( "type,value,increment", ',');
	if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		if (si->tag == papuga_TagValue)
		{
			termtype = Deserializer::getString( si, se);
			termvalue = Deserializer::getString( si, se);
			increment = Deserializer::getInt( si, se);
		}
		else
		{
			unsigned char defined[3] = {0,0};
			while (si->tag == papuga_TagName)
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "type", context);
						termtype = Deserializer::getString( si, se);
						break;
					case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "value", context);
						termvalue = Deserializer::getString( si, se);
						break;
					case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "increment", context);
						increment = Deserializer::getInt( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s definition, 'type', 'value' or 'increment' expected"), context);
				}
			}
			if (!defined[0] || !defined[1] || !defined[2])
			{
				throw strus::runtime_error(_TXT("incomplete %s definition"), context);
			}
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for %s definition"), context);
	}
}

ContextDef::ContextDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
	:threads(0),rpc(),trace()
{
	static const char* context = _TXT("context configuration");
	static const StructureNameMap namemap( "threads,rpc,trace", ',');

	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);
	if (si->tag == papuga_TagValue)
	{
		rpc = Deserializer::getString( si, se);
	}
	else if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		if (si != se && si->tag == papuga_TagValue)
		{
			rpc = Deserializer::getString( si, se);
		}
		else
		{
			unsigned char defined[3] = {0,0,0};
			while (si != se && si->tag == papuga_TagName)
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "threads", context);
						threads = Deserializer::getUint( si, se);
						break;
					case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "rpc", context);
						rpc = Deserializer::getConfigString( si, se);
						break;
					case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "trace", context);
						trace = Deserializer::getConfigString( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'threads' or 'rpc' or 'trace' expected"), context);
				}
			}
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for %s definition"), context);
	}
}

SegmenterDef::SegmenterDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	static const char* context = _TXT("segmenter configuration");
	static const StructureNameMap namemap( "segmenter,mimetype,encoding,scheme", ',');

	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);
	if (si->tag == papuga_TagValue)
	{
		mimetype = Deserializer::getString( si, se);
	}
	else if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		if (si != se && si->tag == papuga_TagValue)
		{
			mimetype = Deserializer::getString( si, se);
			if (si != se && si->tag == papuga_TagValue)
			{
				encoding = Deserializer::getString( si, se);
			}
			if (si != se && si->tag == papuga_TagValue)
			{
				scheme = Deserializer::getString( si, se);
			}
		}
		else
		{
			unsigned char defined[4] = {0,0,0,0};
			while (si != se && si->tag == papuga_TagName)
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "segmenter", context);
						if (0!=(defined[1]|defined[2]|defined[3])) throw strus::runtime_error(_TXT("mixed definition of '%s' and %s in %s"), "segmenter", "'scheme', 'mimetype' and 'encoding'", context);
						segmenter = Deserializer::getString( si, se);
						break;
					case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "mimetype", context);
						if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "mimetype", context);
						mimetype = Deserializer::getString( si, se);
						break;
					case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "encoding", context);
						if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "encoding", context);
						encoding = Deserializer::getString( si, se);
						break;
					case 3:	if (defined[3]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "scheme", context);
						if (defined[0]) throw strus::runtime_error(_TXT("mixed definition of '%s' and '%s' in %s"), "segmenter", "scheme", context);
						scheme = Deserializer::getString( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'segmenter' or 'mimetype' or 'encoding' or 'scheme' expected"), context);
				}
			}
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure or single value expected for %s definition"), context);
	}
}


MetaDataCompareDef::MetaDataCompareDef( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	static const char* context = _TXT("meta data compare");
	static const StructureNameMap namemap( "op,name,value", ',');

	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);
	if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		if (si != se && si->tag == papuga_TagValue)
		{
			cmpop = Deserializer::getMetaDataCmpOp( si, se);
			name = Deserializer::getString( si, se);
			const papuga_ValueVariant* valueref = getValue( si, se);
			papuga_init_ValueVariant_copy( &value, valueref);
		}
		else
		{
			unsigned char defined[3] = {0,0,0};
			while (si != se && si->tag == papuga_TagName)
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "op", context);
						cmpop = Deserializer::getMetaDataCmpOp( si, se);
						break;
					case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "name", context);
						name = Deserializer::getString( si, se);
						break;
					case 2:	if (defined[2]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "value", context);
						papuga_init_ValueVariant_copy( &value, getValue( si, se));
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s, 'op' or 'name' or 'value' expected"), context);
				}
			}
			if (!defined[0]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "op", context);
			if (!defined[1]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "name", context);
			if (!defined[2]) throw strus::runtime_error(_TXT("undefined '%s' in %s"), "value", context);
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for %s definition"), context);
	}
}
