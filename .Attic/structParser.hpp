/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_STRUCTURE_PARSER_HPP_INCLUDED
#define _STRUS_BINDINGS_STRUCTURE_PARSER_HPP_INCLUDED
#include "structNameMap.hpp"

namespace strus {
namespace bindings {

class StructureParser
{
public:
	StructureParser( const StructureNameMap* namemap, papuga_SerializationIter* iter, int structid);
	int next();
	bool hasMore();
	bool testRequired( int nargs, ...);

private:
	enum Class {
		StructDictionary,	//< Dictionary defined by ValueTuple and structid
		NameDictionary,		//< Dictionary with Name=Value assignments referring to namemap
		TupleDictionary,	//< Dictionary with Name,Value pairs referring to namemap
		MetaDictionary,		//< Dictionary with structures as name=Name,value=Value pairs with Name=Value referring to namemap
		ValueTuple		//< Tuple of values in the order of definition in namemap
	};
	int m_mask;
	int m_idx;
	Class m_class;
	const StructureNameMap* m_namemap;
	papuga_SerializationIter* m_iter;
	int m_structid;
};

}}//namespace
#endif

