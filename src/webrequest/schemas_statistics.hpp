/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schemas for storages
 * @file schemas_storage.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_STATISTICS_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_STATISTICS_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_PUT_StatisticsServer :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_StatisticsServer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/, false/*do not merge*/,{},
		{
			{"/statserver/storage", "()", StatisticsStorageServer, papuga_TypeString, "example.com:7184"},
			{"/statserver", "storage", StatisticsStorageServer, '*'},
			{"/statserver/blocks", "()", StatisticsMapBlocks, papuga_TypeString, "100K"},
			{"/statserver/proc", "()", StatisticsProc, papuga_TypeString, "std"},
			{"/statserver", StatisticsMapConfig, {
					{"proc", StatisticsProc, '?'},
					{"blocks", StatisticsMapBlocks, '?'},
				}
			},
			{"/", "statserver", "context", bindings::method::Context::createStatisticsMap(), {{StatisticsMapConfig}} }
		}
	) {}
};

}}//namespace
#endif

