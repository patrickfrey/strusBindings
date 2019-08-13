/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schema for document analysis
 * @file schemas_document.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_CONTENT_STATISTICS_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_CONTENT_STATISTICS_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaContentStatisticsPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineContentStatistics( const char* rootexpr)
	{
		typedef bindings::method::ContentStatistics S;
		typedef bindings::method::Context C;
		return { rootexpr, {
			{"", "contentstats", "context", C::createContentStatistics(), {} },
			{"attribute", "()", ContentAttribute, papuga_TypeString, "id"},
			{"select", "()", SelectExpression, papuga_TypeString, "/doc/text()"},
			{"element/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"element/regex", "()", ContentRegexExpression, papuga_TypeString, "[A-Za-z0-9 ]+"},
			{"element/priority", "()", ContentPriorityExpression, papuga_TypeInt, "0;1;2"},
			{"element/minlen", "()", ContentMinLength, papuga_TypeInt, "10"},
			{"element/maxlen", "()", ContentMaxLength, papuga_TypeInt, "160"},
			{"element/tokenizer/name", "()", TokenizerName, papuga_TypeString, "text"},
			{"element/tokenizer/arg", "()", TokenizerArg, papuga_TypeString, "de"},
			{"element/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"element/normalizer/name", "()", NormalizerName, papuga_TypeString, "regex"},
			{"element/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "[A-Za-z0-9 ]+"},
			{"element/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"attribute", 0, "contentstats", S::addVisibleAttribute(), {
					{ContentAttribute}
				}
			},
			{"select", 0, "contentstats", S::addSelectorExpression(), {
					{SelectExpression}
				}
			},
			{"element", 0, "contentstats", S::addLibraryElement(), {
					{FeatureTypeName},
					{ContentRegexExpression},
					{ContentPriorityExpression,'?'},
					{ContentMinLength,'?'},
					{ContentMaxLength,'?'},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			}
		}};
	}
};

class Schema_Context_INIT_ContentStatistics :public papuga::RequestAutomaton, public SchemaContentStatisticsPart
{
public:
	Schema_Context_INIT_ContentStatistics() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
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
