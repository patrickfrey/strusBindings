/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Partial schema definition to include for parsing query evaluation definitions
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
			{"", "qryeval", "context", C::createQueryEval(), {} },
			{"cterm/set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"cterm/type", "()", TermType, papuga_TypeString, "word"},
			{"cterm/name", "()", TermValue, papuga_TypeString, "town"},
			{"cterm", 0, "qryeval", E::addTerm(), {{FeatureSet},{TermType},{TermValue}} },
			{"restriction", "()", FeatureRestrictionDef, papuga_TypeString, "restrict"},
			{"restriction", 0, "qryeval", E::addRestrictionFeature(), {{FeatureRestrictionDef}} },
			{"selection", "()", FeatureSelectionDef, papuga_TypeString, "select"},
			{"selection", 0, "qryeval", E::addSelectionFeature(), {{FeatureSelectionDef}} },
			{"exclusion", "()", FeatureExclusionDef, papuga_TypeString, "exclude"},
			{"exclusion", 0, "qryeval", E::addExclusionFeature(), {{FeatureExclusionDef}} },

			{"{weighting,summarizer}/feature/role", "()", QueryEvalFunctionFeatureRole, papuga_TypeString, NULL},
			{"{weighting,summarizer}/feature/set", "()", QueryEvalFunctionFeatureSet, papuga_TypeString, NULL},
			{"{weighting,summarizer}/feature", QueryEvalFunctionFeature, {
					{"role", QueryEvalFunctionFeatureRole},
					{"set", QueryEvalFunctionFeatureSet}
				}
			},
			{"{weighting,summarizer}/param/name", "()", QueryEvalFunctionParameterName, papuga_TypeString, NULL},
			{"{weighting,summarizer}/param/value", "()", QueryEvalFunctionParameterValue, papuga_TypeString, NULL},
			{"{weighting,summarizer}/param", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionParameterName},
					{"value", QueryEvalFunctionParameterValue},
				}
			},
			{"summarizer/id", "()", QueryEvalSummaryId, papuga_TypeString, "matches"},
			{"summarizer/name", "()", QueryEvalFunctionName, papuga_TypeString, "matchphrase"},
			{"summarizer", QueryEvalSummarizer, {
					{"id", QueryEvalSummaryId, '?'},
					{"name", QueryEvalFunctionName, '!'},
					{"param", QueryEvalFunctionParameter, '*'},
					{"feature", QueryEvalFunctionFeature, '*'}
				}
			},
			{"summarizer", 0, "qryeval", E::addSummarizer(), {
					{QueryEvalSummaryId,'?'},
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'},
					{QueryEvalFunctionFeature, '*'}}
			},
			{"weighting/name", "()", QueryEvalFunctionName, papuga_TypeString, "bm25"},
			{"weighting", QueryEvalWeighting, {
					{"name", QueryEvalFunctionName, '!'},
					{"param", QueryEvalFunctionParameter, '*'},
					{"feature", QueryEvalFunctionFeature, '*'}
				}
			},
			{"weighting", 0, "qryeval", E::addWeightingFunction(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'},
					{QueryEvalFunctionFeature, '*'}}
			},
			{"formula/source", "()", QueryEvalFormulaSource, papuga_TypeString, "_0 * factor * log(_2)"},
			{"formula/param/name", "()", QueryEvalFormulaParameterName, papuga_TypeString, "factor"},
			{"formula/param/value", "()", QueryEvalFormulaParameterValue, papuga_TypeString, "3.2"},
			{"formula/param", QueryEvalFormulaParameter, {
					{"name", QueryEvalFormulaParameterName},
					{"value", QueryEvalFormulaParameterValue}
				}
			},
			{"formula", 0, "qryeval", E::defineWeightingFormula(), {
					{QueryEvalFormulaSource},
					{QueryEvalFormulaParameter, '*'}}
			}
		}};
	}

	static papuga::RequestAutomaton_NodeList declareQueryBuilderConfig( const char* rootexpr)
	{
		return { rootexpr,
		{
			{"separator", "()", QueryBuilderSeparator, papuga_TypeString, "#"},
			{"type", "()", QueryBuilderFeatureType, papuga_TypeString, "word"},
			{"set", "()", QueryBuilderFeatureSet, papuga_TypeString, "search"},
			{"expand", "()", QueryBuilderExpandSummary, papuga_TypeString, "expand"},
			{"docid", "()", QueryBuilderDocidSummary, papuga_TypeString, "docid"},
			{"rewrite/name", "()", QueryBuilderFeatureTypeRewriteDefName, papuga_TypeString, "N;E;A;V"},
			{"rewrite/value", "()", QueryBuilderFeatureTypeRewriteDefValue, papuga_TypeString, "word"},
			{"rewrite", QueryBuilderFeatureTypeRewriteDef, {
					{"name", QueryBuilderFeatureTypeRewriteDefName},
					{"value", QueryBuilderFeatureTypeRewriteDefValue}
				}
			},
			{"", QueryBuilderDef, {
					{"separator", QueryBuilderSeparator, '?'},
					{"type", QueryBuilderFeatureType, '?'},
					{"set", QueryBuilderFeatureSet, '?'},
					{"expand", QueryBuilderExpandSummary, '?'},
					{"docid", QueryBuilderDocidSummary, '?'},
					{"rewrite", QueryBuilderFeatureTypeRewriteDef, '*'}
				}
			}
		}};
	}
};

}}//namespace
#endif

