/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "structDefs.hpp"
#include "wcharString.hpp"
#include "valueVariantConv.hpp"
#include "internationalization.hpp"
#include "deserializer.hpp"
#include "strus/bindings/serialization.hpp"

using namespace strus;
using namespace strus::bindings;

static const ValueVariant& getValue(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si != se && si->tag == Serialization::Value)
	{
		return *si++;
	}
	else
	{
		throw strus::runtime_error(_TXT("unexpected token in serialization, expected value"));
	}
}

AnalyzerFunctionDef::AnalyzerFunctionDef( Serialization::const_iterator& si, const Serialization::const_iterator& se)
	:name(),args()
{
	static const char* context = _TXT("analyzer function");
	static const StructureNameMap namemap( "name,arg", ',');
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

	bool name_defined = false;
	if (si->tag == Serialization::Name)
	{
		do
		{
			switch (namemap.index(*si++))
			{
				case 0: name_defined = true;
					name = Deserializer::getString( si, se);
					break;
				case 1: args = Deserializer::getStringList( si, se);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
			}
		} while (si != se && si->tag == Serialization::Name);
		Deserializer::consumeClose( si, se);
	}
	else if (si->tag == Serialization::Value)
	{
		name = Deserializer::getString( si, se);
		name_defined = true;
		if (si != se && si->tag != Serialization::Close)
		{
			args = Deserializer::getStringList( si, se);
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

TermDef::TermDef( Serialization::const_iterator& si, const Serialization::const_iterator& se)
	:variable(),type(),value(),length(1),value_defined(false),length_defined(false)
{
	static const char* context = _TXT("term");
	static const StructureNameMap namemap( "type,value,variable,len", ',');
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

	bool type_defined = false;
	if (si->tag == Serialization::Name)
	{
		do
		{
			switch (namemap.index( *si++))
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
		} while (si != se && si->tag == Serialization::Name);
		Deserializer::consumeClose( si, se);
	}
	else if (si->tag == Serialization::Value)
	{
		if (Deserializer::isStringWithPrefix( *si, '='))
		{
			variable = Deserializer::getPrefixStringValue( *si++, '=');
		}
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);

		if (si->tag == Serialization::Value)
		{
			type = Deserializer::getString( si, se);
			type_defined = true;
			if (si != se && si->tag != Serialization::Close)
			{
				value = Deserializer::getString( si, se);
				value_defined = true;
				if (si != se && si->tag != Serialization::Close)
				{
					length_defined = true;
					length = Deserializer::getUint( si, se);
				}
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("%s definition expected"), context);
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

MetaDataRangeDef::MetaDataRangeDef( Serialization::const_iterator& si, const Serialization::const_iterator& se)
	:from(),to()
{
	static const char* context = _TXT("metadata range");
	static const StructureNameMap namemap( "from,to", ',');
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition, structure expected"), context);

	if (si->tag == Serialization::Name)
	{
		do
		{
			switch (namemap.index( *si++))
			{
				case 0: from = Deserializer::getString( si, se);
					break;
				case 1:	to = Deserializer::getString( si, se);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
			}
		} while (si != se && si->tag == Serialization::Name);
		Deserializer::consumeClose( si, se);
	}
	else if (si->tag == Serialization::Value)
	{
		from = Deserializer::getPrefixStringValue( *si++, '@');
		if (si != se && si->tag != Serialization::Close)
		{
			to = Deserializer::getPrefixStringValue( *si++, '@');
		}
		Deserializer::consumeClose( si, se);
	}
}


ConfigDef::ConfigDef( Serialization::const_iterator& si, const Serialization::const_iterator& se)
	:name(),value()
{
	static const char* context = _TXT("configuration");
	static const StructureNameMap namemap( "name,value", ',');
	if (si->tag == Serialization::Open)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);
		if (si->tag == Serialization::Value)
		{
			name = Deserializer::getString( si, se);
			value = getValue( si, se);
			Deserializer::consumeClose( si, se);
		}
		else
		{
			unsigned char defined[2] = {0,0};
			while (si->tag == Serialization::Name)
			{
				switch (namemap.index( *si++))
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
		}
	}
	else if (si->tag == Serialization::Name)
	{
		name = Deserializer::getString( si, se);
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);

		value = *si++;
	}
	else
	{
		throw strus::runtime_error(_TXT("argument name expected as as start of a %s parameter"), context);
	}
}


DfChangeDef::DfChangeDef( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	static const char* context = _TXT("document frequency change");
	static const StructureNameMap namemap( "type,value,increment", ',');
	if (si->tag == Serialization::Open)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);
		if (si->tag == Serialization::Value)
		{
			termtype = Deserializer::getCharp( si, se);
			termvalue = Deserializer::getCharp( si, se);
			increment = Deserializer::getInt( si, se);
		}
		else
		{
			unsigned char defined[3] = {0,0};
			while (si->tag == Serialization::Name)
			{
				switch (namemap.index( *si++))
				{
					case 0: if (defined[0]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "type", context);
						termtype = Deserializer::getCharp( si, se);
						break;
					case 1:	if (defined[1]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "value", context);
						termvalue = Deserializer::getCharp( si, se);
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

