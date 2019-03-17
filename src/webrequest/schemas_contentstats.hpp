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
	static papuga::RequestAutomaton_NodeList defineContentStatistics()
	{
		typedef bindings::method::ContentStatistics S;
		typedef bindings::method::Context C;
		return {
			{"library", "contentstats", "context", C::createContentStatistics(), {} },
			{"library/attribute", "()", ContentAttribute, papuga_TypeString, "id"},
			{"library/select", "()", SelectExpression, papuga_TypeString, "/doc/text()"},
			{"library/element/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"library/element/regex", "()", ContentRegexExpression, papuga_TypeString, "[A-Za-z0-9 ]+"},
			{"library/element/priority", "()", ContentPriorityExpression, papuga_TypeInt, "0;1;2"},
			{"library/element/minlen", "()", ContentMinLength, papuga_TypeInt, "10"},
			{"library/element/maxlen", "()", ContentMaxLength, papuga_TypeInt, "160"},
			{"library/element/tokenizer/name", "()", TokenizerName, papuga_TypeString, "text"},
			{"library/element/tokenizer/arg", "()", TokenizerArg, papuga_TypeVoid, NULL},
			{"library/element/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"library/element/normalizer/name", "()", NormalizerName, papuga_TypeString, "regex"},
			{"library/element/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "[A-Za-z0-9 ]+"},
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

class Schema_Context_INIT_ContentStatistics :public papuga::RequestAutomaton, public SchemaContentStatisticsPart
{
public:
	Schema_Context_INIT_ContentStatistics() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineContentStatistics()}
		}
	) {}
};

class Schema_Context_PUT_ContentStatistics :public papuga::RequestAutomaton, public SchemaContentStatisticsPart
{
public:
	Schema_Context_PUT_ContentStatistics() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineContentStatistics()}
		}
	) {}
};

}}//namespace
#endif
