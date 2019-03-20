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
#ifndef _STRUS_WEBREQUEST_SCHEMAS_QUERY_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_QUERY_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_expression.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaQueryPart :public AutomatonNameSpace
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
			{"/query/eval/summarizer", 0, "queryeval", E::addSummarizer(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'},
					{QueryEvalFunctionResult, '*'}}
			},
			{"/query/eval/weighting/name", "()", QueryEvalFunctionName, papuga_TypeString, "bm25"},
			{"/query/eval/weighting", 0, "queryeval", E::addWeightingFunction(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'}}
			},
			{"/query/eval/formula", 0, "queryeval", E::defineWeightingFormula(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'}}
			}
		};
	}

	static papuga::RequestAutomaton_NodeList defineAnalyzer()
	{
		typedef bindings::method::QueryAnalyzer A;
		typedef bindings::method::Context C;
		return {
			{"/query/analyzer", "analyzer", "context", C::createQueryAnalyzer(), {} },
			{"/query/analyzer/element/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"/query/analyzer/element/field", "()", FieldTypeName, papuga_TypeString, "text"},

			{"/query/analyzer/element/tokenizer/name", "()", TokenizerName, papuga_TypeString, "regex"},
			{"/query/analyzer/element/tokenizer/arg", "()", TokenizerArg, papuga_TypeString, "[a-zA-Z0-9]+"},
			{"/query/analyzer/element/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"/query/analyzer/element/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"/query/analyzer/element/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "de"},
			{"/query/analyzer/element/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"/query/analyzer/element", 0, "analyzer", A::addElement(), {
					{FeatureTypeName},
					{FieldTypeName},
					{TokenizerDef},
					{NormalizerDef,'+'},
				}
			},
			{"/query/analyzer/group/field", "()", FieldTypeName, papuga_TypeString, "text"},
			{"/query/analyzer/group/op", "()", JoinOperatorName, papuga_TypeString, "sequence_imm"},
			{"/query/analyzer/group/range", "()", JoinOperatorRange, papuga_TypeInt, "5"},
			{"/query/analyzer/group/cardinality", "()", JoinOperatorCardinality, papuga_TypeInt, "3"},
			{"/query/analyzer/group@by", "", GroupBy, papuga_TypeString, "position;every;all;unique"},
			{"/query/analyzer/group", 0, "analyzer", A::defineImplicitGroupBy(), {
					{FieldTypeName,'!'},
					{GroupBy,'!'},
					{JoinOperatorName,'!'},
					{JoinOperatorRange,'?'},
					{JoinOperatorCardinality,'?'}}
			}
		};
	}

	static papuga::RequestAutomaton_NodeList declareMetaData()
	{
		return {
			{"/query/restriction//condition/op", "()", MetaDataConditionOp, papuga_TypeString, ">:gt:ne"},
			{"/query/restriction//condition/name", "()", MetaDataConditionName, papuga_TypeString, "date"},
			{"/query/restriction//condition/value", "()", MetaDataConditionValue, papuga_TypeString, "879236"},
			{"/query/restriction//condition", MetaDataCondition, {
					{MetaDataConditionOp},
					{MetaDataConditionName},
					{MetaDataConditionValue}
				}
			},
			{"/query/restriction/condition", "()", MetaDataCondition, papuga_TypeVoid, NULL},
			{"/query/restriction/union", MetaDataCondition, {
					{MetaDataCondition, '*'}
				}
			},
		};
	}

	static papuga::RequestAutomaton_NodeList analyzeTermExpression()
	{
		typedef bindings::method::QueryAnalyzer A;
		return {
			{SchemaExpressionPart::declarePostingsExpression().root("/query/feature")},
			{"/query/feature/set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"/query/feature/weight", "()", FeatureWeight, papuga_TypeDouble, "0.75:1.0"},
			{"/query/feature", "+feature", "analyzer", A::analyzeSingleTermExpression(), {{TermExpression}} },
		};
	}

	static papuga::RequestAutomaton_NodeList analyzeMetaData()
	{
		typedef bindings::method::QueryAnalyzer A;
		return {
			{declareMetaData()},
			{"/query/restriction", "+condition", "analyzer", A::analyzeMetaDataExpression(), {{MetaDataCondition, '*'}} },
		};
	}

	static papuga::RequestAutomaton_NodeList buildQueryOriginal()
	{
		typedef bindings::method::Query Q;
		typedef bindings::method::QueryAnalyzer A;
		return {
			{SchemaExpressionPart::declarePostingsExpression().root("/query/feature")},
			{"/query/feature/set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"/query/feature/weight", "()", FeatureWeight, papuga_TypeDouble, "0.75:1.0"},
			{"/query/feature", "_feature", "analyzer", A::analyzeSingleTermExpression(), {{TermExpression}} },
			{"/query/feature", 0, "query", Q::addFeature(), {{FeatureSet}, {"_feature"}, {FeatureWeight, '?'}} },

			{declareMetaData()},
			{"/query/restriction", "_condition", "analyzer", A::analyzeMetaDataExpression(), {{MetaDataCondition, '*'}} },
			{"/query/restriction", 0, "query", Q::addMetaDataRestriction(),  {"_condition"} }
		};
	}

	static papuga::RequestAutomaton_NodeList buildQueryAnalyzed()
	{
		typedef bindings::method::Query Q;
		return {
			{SchemaExpressionPart::declarePostingsExpression().root("/query/feature")},
			{"/query/feature/set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"/query/feature/weight", "()", FeatureWeight, papuga_TypeDouble, "0.75:1.0"},
			{"/query/feature", 0, "query", Q::addFeature(), {{FeatureSet}, {TermExpression}, {FeatureWeight, '?'}} },

			{declareMetaData()},
			{"/query/restriction", 0, "query", Q::addMetaDataRestriction(),  {MetaDataCondition} }
		};
	}

	static papuga::RequestAutomaton_NodeList defineStatistics()
	{
		typedef bindings::method::Query Q;
		return {
			{"/query/termstats/type", "()", TermType, papuga_TypeString, "word"},
			{"/query/termstats/value", "()", TermValue, papuga_TypeString, "country"},
			{"/query/termstats/df", "()", TermDocumentFrequency, papuga_TypeInt, "312367"},
			{"/query/termstats", TermStats, {
					{"df", TermDocumentFrequency}
				}
			},
			{"/query/termstats", 0, "query", Q::defineTermStatistics(), {{TermType},{TermValue},{TermStats}} },
			{"/query/globalstats/nofdocs", "()", CollectionNofDocs, papuga_TypeInt, "112739087"},
			{"/query/globalstats", GlobalStats, {
					{"nofdocs", CollectionNofDocs}
				}
			},
			{"/query/termstats", 0, "query", Q::defineGlobalStatistics(), {{GlobalStats}} }
		};
	}

	static papuga::RequestAutomaton_NodeList defineRankingParameter()
	{
		typedef bindings::method::Query Q;
		return {
			{"/query/evalset/docno", "()", Docno, papuga_TypeInt, "21345"},
			{"/query/evalset", 0, "query", Q::addDocumentEvaluationSet(), {{Docno, '*'}} },
			{"/query/nofranks", "()", NumberOfResults, papuga_TypeInt, "20"},
			{"/query/nofranks", 0, "query", Q::setMaxNofRanks(), {{NumberOfResults}} },
			{"/query/minrank", "()", FirstResult, papuga_TypeInt, "0:10"},
			{"/query/minrank", 0, "query", Q::setMinRank(), {{FirstResult}} },
			{"/query/access", "()", AccessRight, papuga_TypeString, "customer"},
			{"/query/access", 0, "query", Q::addAccess(), {{AccessRight, '*'}} },

			{"/query", 0, "query", Q::setWeightingVariables(), {{VariableDef, '*'}} }
		};
	}
};

class Schema_QueryAnalyzer_GET_content :public papuga::RequestAutomaton, public SchemaQueryPart
{
public:
	Schema_QueryAnalyzer_GET_content() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineAnalyzer()},
			{analyzeTermExpression()},
			{analyzeMetaData()}
		}
	) {}
};


class Schema_Storage_QRYORG :public papuga::RequestAutomaton, public SchemaQueryPart
{
public:
	Schema_Storage_QRYORG() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineQueryEval()},
			{defineAnalyzer()},
			{"/query", "query", "queryeval", bindings::method::QueryEval::createQuery(), {{"storage"}} },
			{buildQueryOriginal()},
			{defineRankingParameter()},
			{"/query", "ranklist", "query", bindings::method::Query::evaluate(), {} }
		}
	) {}
};

class Schema_Storage_QRYANA :public papuga::RequestAutomaton, public SchemaQueryPart
{
public:
	Schema_Storage_QRYANA() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineQueryEval()},
			{"/query", "query", "queryeval", bindings::method::QueryEval::createQuery(), {{"storage"}} },
			{buildQueryAnalyzed()},
			{defineStatistics()},
			{defineRankingParameter()},
			{"/query", "ranklist", "query", bindings::method::Query::evaluate(), {} }
		}
	) {}
};

}}//namespace
#endif


