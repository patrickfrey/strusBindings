/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schema for query analysis and evaluation
 * @file schemas_query.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_QUERYEVAL_DECL_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_QUERYEVAL_DECL_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaQueryEvalDeclPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineQueryEval()
	{
		typedef bindings::method::QueryEval E;
		typedef bindings::method::Context C;
		return {
			{"/query/eval", "queryeval", "context", C::createQueryEval(), {} },
			{"/query/eval/cterm/set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"/query/eval/cterm/type", "()", TermType, papuga_TypeString, "word"},
			{"/query/eval/cterm/name", "()", TermValue, papuga_TypeString, "town"},
			{"/query/eval/cterm", 0, "queryeval", E::addTerm(), {{FeatureSet},{TermType},{TermValue}} },
			{"/query/eval/restriction", "()", FeatureSetRestrictionDef, papuga_TypeString, "restrict"},
			{"/query/eval/restriction", 0, "queryeval", E::addRestrictionFeature(), {{FeatureSetRestrictionDef}} },
			{"/query/eval/selection", "()", FeatureSetSelectionDef, papuga_TypeString, "select"},
			{"/query/eval/selection", 0, "queryeval", E::addSelectionFeature(), {{FeatureSetSelectionDef}} },
			{"/query/eval/exclusion", "()", FeatureSetExclusionDef, papuga_TypeString, "exclude"},
			{"/query/eval/exclusion", 0, "queryeval", E::addExclusionFeature(), {{FeatureSetExclusionDef}} },

			{"/query/eval//result/name", "()", QueryEvalFunctionResultName, papuga_TypeString, NULL},
			{"/query/eval//result/value", "()", QueryEvalFunctionResultValue, papuga_TypeString, NULL},
			{"/query/eval//result", QueryEvalFunctionResult, {
					{"name", QueryEvalFunctionResultName},
					{"value", QueryEvalFunctionResultValue}
				}
			},
			{"/query/eval//param/name", "()", QueryEvalFunctionParameterName, papuga_TypeString, NULL},
			{"/query/eval//param/value", "()", QueryEvalFunctionParameterValue, papuga_TypeString, NULL},
			{"/query/eval//param", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionParameterName},
					{"value", QueryEvalFunctionParameterValue}
				}
			},
			{"/query/eval//feature/name", "()", QueryEvalFunctionFeatureName, papuga_TypeString, NULL},
			{"/query/eval//feature/value", "()", QueryEvalFunctionFeatureValue, papuga_TypeString, NULL},
			{"/query/eval//feature", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionFeatureName},
					{"feature", QueryEvalFunctionFeatureValue}
				}
			},
			{"/query/eval/summarizer/name", "()", QueryEvalFunctionName, papuga_TypeString, "matchphrase"},
			{"/query/eval/summarizer/arg", "()", QueryEvalFunctionParameter, papuga_TypeVoid, NULL},
			{"/query/eval/summarizer/result", "()", QueryEvalFunctionResult, papuga_TypeVoid, NULL},
			{"/query/eval/summarizer", QueryEvalSummarizer, {
					{"name", QueryEvalFunctionName, '!'},
					{"param", QueryEvalFunctionParameter, '*', 2/*tag diff*/},
					{"result", QueryEvalFunctionResult, '*'}
				}
			},
			{"/query/eval/summarizer", 0, "queryeval", E::addSummarizer(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*', 2/*tag diff*/},
					{QueryEvalFunctionResult, '*'}}
			},
			{"/query/eval/weighting/name", "()", QueryEvalFunctionName, papuga_TypeString, "bm25"},
			{"/query/eval/weighting/arg", "()", QueryEvalFunctionParameter, papuga_TypeVoid, NULL},
			{"/query/eval/weighting", QueryEvalWeighting, {
					{"name", QueryEvalFunctionName, '!'},
					{"param", QueryEvalFunctionParameter, '*', 2/*tag diff*/},
				}
			},
			{"/query/eval/weighting", 0, "queryeval", E::addWeightingFunction(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*', 2/*tag diff*/}}
			},
			{"/query/eval/formula/source", "()", QueryEvalFormulaSource, papuga_TypeString, "_0 * factor * log(_2)"},
			{"/query/eval/formula/param/name", "()", QueryEvalFormulaParameterName, papuga_TypeString, "factor"},
			{"/query/eval/formula/param/value", "()", QueryEvalFormulaParameterValue, papuga_TypeString, "3.2"},
			{"/query/eval/formula/param", QueryEvalFormulaParameter, {
					{"name", QueryEvalFormulaParameterName},
					{"value", QueryEvalFormulaParameterValue}
				}
			},
			{"/query/eval/formula", 0, "queryeval", E::defineWeightingFormula(), {
					{QueryEvalFormulaSource},
					{QueryEvalFormulaParameter, '*'}}
			}
		};
	}
};

}}//namespace
#endif

