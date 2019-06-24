/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schema for initial configuration
 * @file schemas_context.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_CONTEXT_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_CONTEXT_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_INIT_Context :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_INIT_Context() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		{},
		{},
		{
			{"/extensions/directory", "()", ModuleDir, papuga_TypeString, "/usr/lib/modules"},
			{"/extensions/modules", "()", ModuleName, papuga_TypeString, "analyzer_pattern"},
			{"/data/workdir", "()", WorkDir, papuga_TypeString, "/srv/strus"},
			{"/data/resources", "()", ResourceDir, papuga_TypeString, "/usr/share/strus"},
			{"/context/trace", "log()", TraceLogType, papuga_TypeString, "dump;count"},
			{"/context/trace", "file()", TraceLogFile, papuga_TypeString, "stdout;stderr;/run/log/strus"},
			{"/context/trace", "groupby()", TraceGroupBy, papuga_TypeString, "Query::pushTerm"},
			{"/context/trace", "call()", TraceCall, papuga_TypeString, "213\\,214\\,512"},
			{"/context/trace", "count()", TraceCount, papuga_TypeString, "Query::pushTerm"},
			{"/context/trace", ContextTrace, {
					{"log",  TraceLogType, '!'},
					{"file", TraceLogFile, '?'},
					{"groupby", TraceGroupBy, '?'},
					{"call", TraceCall, '?'},
					{"count", TraceCount, '?'}
				}
			},
			{"/context/rpc", "()", ContextRpc, papuga_TypeString, "localhost:1313"},
			{"/context/threads", "()", ContextThreads, papuga_TypeInt, "16"},
			{"/context", ContextConfig, {
					{"rpc", ContextRpc, '?'},
					{"trace", ContextTrace, '?'},
					{"threads", ContextThreads, '?'}
				}
			},
			{"/", "context", "", bindings::method::Context::constructor(), {{(int)ContextConfig, '?'}} },
			{"/data/workdir", "", "context", bindings::method::Context::defineWorkingDirectory(), {{(int)WorkDir}} },
			{"/data/resources", "", "context", bindings::method::Context::addResourcePath(), {{(int)ResourceDir}} },
			{"/extensions/directory", "", "context", bindings::method::Context::addModulePath(), {{(int)ModuleDir}} },
			{"/extensions/modules", "", "context", bindings::method::Context::loadModule(), {{(int)ModuleName}} },
			{"/", "", "context", bindings::method::Context::endConfig(), {} }
		}
	) {}
};

}}//namespace
#endif

