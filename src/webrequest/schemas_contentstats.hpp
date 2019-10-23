/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schema definition for document content statistics
 * @file schemas_document.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_CONTENT_STATISTICS_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_CONTENT_STATISTICS_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_contentstats_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_INIT_ContentStatistics :public papuga::RequestAutomaton, public SchemaContentStatisticsPart
{
public:
	Schema_Context_INIT_ContentStatistics() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{defineContentStatistics("/library")}
		}
	) {}
};

class Schema_Context_PUT_ContentStatistics :public Schema_Context_INIT_ContentStatistics
{
public:
	Schema_Context_PUT_ContentStatistics() :Schema_Context_INIT_ContentStatistics(){}
};


}}//namespace
#endif
