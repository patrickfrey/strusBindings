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
	static papuga::RequestAutomaton_NodeList defineQueryEval( const char* rootexpr)
	{
		typedef bindings::method::QueryEval E;
		typedef bindings::method::Context C;
		return { rootexpr,
		{
			{"", "queryeval", "context", C::createQueryEval(), {} },
			{"cterm/set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"cterm/type", "()", TermType, papuga_TypeString, "word"},
			{"cterm/name", "()", TermValue, papuga_TypeString, "town"},
			{"cterm", 0, "queryeval", E::addTerm(), {{FeatureSet},{TermType},{TermValue}} },
			{"restriction", "()", FeatureSetRestrictionDef, papuga_TypeString, "restrict"},
			{"restriction", 0, "queryeval", E::addRestrictionFeature(), {{FeatureSetRestrictionDef}} },
			{"selection", "()", FeatureSetSelectionDef, papuga_TypeString, "select"},
			{"selection", 0, "queryeval", E::addSelectionFeature(), {{FeatureSetSelectionDef}} },
			{"exclusion", "()", FeatureSetExclusionDef, papuga_TypeString, "exclude"},
			{"exclusion", 0, "queryeval", E::addExclusionFeature(), {{FeatureSetExclusionDef}} },

			{"//result/name", "()", QueryEvalFunctionResultName, papuga_TypeString, NULL},
			{"//result/value", "()", QueryEvalFunctionResultValue, papuga_TypeString, NULL},
			{"//result", QueryEvalFunctionResult, {
					{"name", QueryEvalFunctionResultName},
					{"value", QueryEvalFunctionResultValue}
				}
			},
			{"//param/name", "()", QueryEvalFunctionParameterName, papuga_TypeString, NULL},
			{"//param/value", "()", QueryEvalFunctionParameterValue, papuga_TypeString, NULL},
			{"//param", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionParameterName},
					{"value", QueryEvalFunctionParameterValue}
				}
			},
			{"//feature/name", "()", QueryEvalFunctionFeatureName, papuga_TypeString, NULL},
			{"//feature/value", "()", QueryEvalFunctionFeatureValue, papuga_TypeString, NULL},
			{"//feature", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionFeatureName},
					{"feature", QueryEvalFunctionFeatureValue}
				}
			},
			{"summarizer/name", "()", QueryEvalFunctionName, papuga_TypeString, "matchphrase"},
			{"summarizer/arg", "()", QueryEvalFunctionParameter, papuga_TypeVoid, NULL},
			{"summarizer/result", "()", QueryEvalFunctionResult, papuga_TypeVoid, NULL},
			{"summarizer", QueryEvalSummarizer, {
					{"name", QueryEvalFunctionName, '!'},
					{"param", QueryEvalFunctionParameter, '*', 2/*tag diff*/},
					{"result", QueryEvalFunctionResult, '*'}
				}
			},
			{"summarizer", 0, "queryeval", E::addSummarizer(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*', 2/*tag diff*/},
					{QueryEvalFunctionResult, '*'}}
			},
			{"weighting/name", "()", QueryEvalFunctionName, papuga_TypeString, "bm25"},
			{"weighting/arg", "()", QueryEvalFunctionParameter, papuga_TypeVoid, NULL},
			{"weighting", QueryEvalWeighting, {
					{"name", QueryEvalFunctionName, '!'},
					{"param", QueryEvalFunctionParameter, '*', 2/*tag diff*/},
				}
			},
			{"weighting", 0, "queryeval", E::addWeightingFunction(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*', 2/*tag diff*/}}
			},
			{"formula/source", "()", QueryEvalFormulaSource, papuga_TypeString, "_0 * factor * log(_2)"},
			{"formula/param/name", "()", QueryEvalFormulaParameterName, papuga_TypeString, "factor"},
			{"formula/param/value", "()", QueryEvalFormulaParameterValue, papuga_TypeString, "3.2"},
			{"formula/param", QueryEvalFormulaParameter, {
					{"name", QueryEvalFormulaParameterName},
					{"value", QueryEvalFormulaParameterValue}
				}
			},
			{"formula", 0, "queryeval", E::defineWeightingFormula(), {
					{QueryEvalFormulaSource},
					{QueryEvalFormulaParameter, '*'}}
			}
		}};
	}
};

}}//namespace
#endif

