/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Scheme for initial configuration
 * @file schemes_context.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMES_CONTEXT_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMES_CONTEXT_HPP_INCLUDED
#include "schemes_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Scheme_INIT_Context :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Scheme_INIT_Context() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"context",{},
		{
			{"/extensions/directory", "()", ModuleDir},
			{"/extensions/modules", "()", ModuleName},
			{"/data/workdir", "()", WorkDir},
			{"/data/resources", "()", ResourceDir},
			{"/context/trace", "log()", TraceLogType},
			{"/context/trace", "file()", TraceLogFile},
			{"/context/trace", "groupby()", TraceGroupBy},
			{"/context/trace", "call()", TraceCall},
			{"/context/trace", "count()", TraceCount},
			{"/context/trace", ContextTrace, {
					{"log",  TraceLogType, '!'},
					{"file", TraceLogFile, '?'},
					{"groupby", TraceGroupBy, '?'},
					{"call", TraceCall, '?'},
					{"count", TraceCount, '?'}
				}
			},
			{"/context/rpc", "()", ContextRpc},
			{"/context/threads", "()", ContextThreads},
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

