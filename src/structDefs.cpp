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
#include "bindingUtils.hpp"
#include "strus/bindings/serialization.hpp"

using namespace strus;
using namespace strus::bindings;

void AnalyzerFunctionDef::deserialize( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	static const StructureNameMap namemap( "name,arg", ',');
	if (si == se) throw strus::runtime_error("unexpected end of serialization: analyzer function structure expected");

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
				default: throw strus::runtime_error("unknown tag name in analyzer function structure");
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
		throw strus::runtime_error("analyzer function definition structure expected");
	}
	if (!name_defined)
	{
		throw strus::runtime_error(_TXT("name not defined in analyzer function"));
	}
}

void TermDef::deserialize( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	static const StructureNameMap namemap( "type,value,variable,len", ',');
	if (si == se) throw strus::runtime_error("unexpected end of serialization: term structure expected");

	bool type_defined = false;
	if (si->tag == Serialization::Name)
	{
		do
		{
			switch (namemap.index( *si++))
			{
				case -1: throw strus::runtime_error("unknown tag name in term structure");
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
		if (si == se) throw strus::runtime_error("unexpected end of serialization: term structure expected");

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
			throw strus::runtime_error(_TXT("term definition expected"));
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("term definition expected"));
	}
	if (!value_defined && length_defined)
	{
		throw strus::runtime_error(_TXT("terms with length defined without value defined are not accepted"));
	}
	if (!type_defined)
	{
		throw strus::runtime_error(_TXT("type not defined in term expression"));
	}
}

void MetaDataRangeDef::deserialize( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	static const StructureNameMap namemap( "from,to", ',');
	if (si == se) throw strus::runtime_error("unexpected end of serialization: term structure expected");

	if (si->tag == Serialization::Name)
	{
		do
		{
			switch (namemap.index( *si++))
			{
				case -1: throw strus::runtime_error("unknown tag name in term structure");
				case 0: from = Deserializer::getString( si, se);
					break;
				case 1:	to = Deserializer::getString( si, se);
					break;
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




