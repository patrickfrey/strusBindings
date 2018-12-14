/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Scheme for document analysis
 * @file schemes_document.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMES_CONTENT_STATISTICS_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMES_CONTENT_STATISTICS_HPP_INCLUDED
#include "schemes_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemeContentStatisticsPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineContentStatistics()
	{
		typedef bindings::method::ContentStatistics S;
		typedef bindings::method::Context C;
		return {
			{"library", "contentstats", "context", C::createContentStatistics(), {} },
			{"library/attribute", "()", ContentAttribute},
			{"library/select", "()", SelectExpression},
			{"library/element/type", "()", FeatureTypeName},
			{"library/element/regex", "()", ContentRegexExpression},
			{"library/element/priority", "()", ContentPriorityExpression},
			{"library/element/minlen", "()", ContentMinLength},
			{"library/element/maxlen", "()", ContentMaxLength},
			{"library/element/tokenizer/name", "()", TokenizerName},
			{"library/element/tokenizer/arg", "()", TokenizerArg},
			{"library/element/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"library/element/normalizer/name", "()", NormalizerName},
			{"library/element/normalizer/arg", "()", NormalizerArg},
			{"library/element/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"library/attribute", 0, "contentstats", S::addVisibleAttribute(), {
					{ContentAttribute}
				}
			},
			{"library/select", 0, "contentstats", S::addSelectorExpression(), {
					{SelectExpression}
				}
			},
			{"library/element", 0, "contentstats", S::addLibraryElement(), {
					{FeatureTypeName},
					{ContentRegexExpression},
					{ContentPriorityExpression,'?'},
					{ContentMinLength,'?'},
					{ContentMaxLength,'?'},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			}
		};
	}
};

class Scheme_Context_INIT_ContentStatistics :public papuga::RequestAutomaton, public SchemeContentStatisticsPart
{
public:
	Scheme_Context_INIT_ContentStatistics() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineContentStatistics()}
		}
	) {}
};

class Scheme_Context_PUT_ContentStatistics :public papuga::RequestAutomaton, public SchemeContentStatisticsPart
{
public:
	Scheme_Context_PUT_ContentStatistics() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineContentStatistics()}
		}
	) {}
};

}}//namespace
#endif
