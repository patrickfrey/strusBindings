/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Scheme for inserter
 * @file schemes_inserter.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMES_INSERTER_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMES_INSERTER_HPP_INCLUDED
#include "schemes_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Scheme_Context_PUT_Inserter :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Scheme_Context_PUT_Inserter() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{
			{"analyzer","/inserter/analyzer()",true/*required*/},
			{"storage","/inserter/storage()",true/*required*/}},
		{}
	) {}
};

}}//namespace
#endif


