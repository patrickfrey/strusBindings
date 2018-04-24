/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "structParser.hpp"
#include "strus/errorCodes.hpp"
#include "private/internationalization.hpp"
#include "papuga/valueVariant.h"
#include <cstring>
#include <cstdarg>

using namespace strus;
using namespace strus::bindings;

static const StructureNameMap g_meta_namemap( "name,value", ',');

StructureParser::StructureParser( const StructureNameMap* namemap, papuga_SerializationIter* iter, int structid)
	:m_mask(0),m_idx(0),m_class(ValueTuple),m_namemap(namemap),m_iter(iter),m_structid(structid)
{
	if (structid)
	{
		m_class = StructDictionary;
	}
	else if (papuga_SerializationIter_tag(iter) == papuga_TagName)
	{
		m_class = NameDictionary;
	}
	else if (papuga_SerializationIter_tag(iter) == papuga_TagValue)
	{
		m_class = ValueTuple;
	}
	else if (papuga_SerializationIter_tag(iter) == papuga_TagOpen)
	{
		papuga_SerializationIter next;
		papuga_init_SerializationIter_copy( &next, iter);
		papuga_SerializationIter_skip( &next);
		if (papuga_SerializationIter_tag(iter) == papuga_TagValue)
		{
			m_class = TupleDictionary;
		}
		else if (papuga_SerializationIter_tag(iter) == papuga_TagName)
		{
			m_class = MetaDictionary;
		}
		else
		{
			throw strus::runtime_error( ErrorCodeSyntax, _TXT("cannot determine format of structure"));
		}
	}
	else
	{
		throw strus::runtime_error( ErrorCodeSyntax, _TXT("cannot determine format of structure"));
	}
}

int StructureParser::next()
{
	int rt = -1;
	switch (m_class)
	{
		case StructDictionary:
			rt = m_idx++;
			break;
		case NameDictionary:
			if (papuga_SerializationIter_tag( m_iter) != papuga_TagName) return -1;
			rt = m_namemap->index( *papuga_SerializationIter_value( m_iter));
			papuga_SerializationIter_skip( m_iter);
			break;
		case TupleDictionary:
			if (papuga_SerializationIter_tag( m_iter) != papuga_TagOpen) return -1;
			papuga_SerializationIter_skip( m_iter);
			break;
		case MetaDictionary:
			if (papuga_SerializationIter_tag( m_iter) != papuga_TagOpen) return -1;
			papuga_SerializationIter_skip( m_iter);
			if (papuga_SerializationIter_tag( m_iter) != papuga_TagName)
			{
				throw strus::runtime_error( ErrorCodeSyntax, _TXT("name expected as first element of meta dictionary structure"));
			}
			{
				int mtkey = g_meta_namemap.index( *papuga_SerializationIter_value( m_iter));
				if (mtkey)
			rt = m_namemap->index( *papuga_SerializationIter_value( m_iter));
			break;
		case ValueTuple:
			if (papuga_SerializationIter_tag( m_iter) == papuga_TagClose) return -1;
			rt = m_idx++;
			break;
	};
}

bool StructureParser::hasMore()
{
}

bool StructureParser::testRequired( int nargs, ...)
{
}

private:
	enum Class {
		NameDictionary,		//< Dictionary with Name=Value assignments referring to namemap
		StructDictionary,	//< Dictionary defined by ValueTuple and structid
		TupleDictionary,	//< Dictionary with Name,Value pairs referring to namemap
		MetaDictionary,		//< Dictionary with structures as name=Name,value=Value pairs with Name=Value referring to namemap
		ValueTuple		//< Tuple of values in the order of definition in namemap
	};
	int m_mask;
	Class m_class;
	const StructureNameMap* m_namemap;
	papuga_SerializationIter* m_iter;
	int m_structid;
};

