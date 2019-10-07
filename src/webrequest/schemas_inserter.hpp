/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schema for inserter
 * @file schemas_inserter.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_INSERTER_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_INSERTER_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_PUT_Inserter :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_Inserter() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/,
		{},
		{
			{"docanalyzer","/inserter/include/analyzer()",true/*required*/},
			{"storage","/inserter/include/storage()",true/*required*/}},
		{
			{"/inserter", "inserter", "context", bindings::method::Context::createInserter(), {{"storage"},{"docanalyzer"}} }
		}
	) {}
};

}}//namespace
#endif


