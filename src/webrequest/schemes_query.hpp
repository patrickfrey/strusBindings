/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Scheme for query analysis and evaluation
 * @file schemes_query.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMES_QUERY_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMES_QUERY_HPP_INCLUDED
#include "schemes_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemeQueryPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineQueryEval()
	{
		typedef bindings::method::QueryEval E;
		typedef bindings::method::Context C;
		return {
			{"/query/eval", "queryeval", "context", C::createQueryEval(), {} },
			{"/query/eval/cterm/set", "()", FeatureSet},
			{"/query/eval/cterm/type", "()", TermType},
			{"/query/eval/cterm/name", "()", TermValue},
			{"/query/eval/cterm", 0, "queryeval", E::addTerm(), {{FeatureSet},{TermType},{TermValue}} },
			{"/query/eval/restriction", "()", FeatureSetRestrictionDef},
			{"/query/eval/restriction", 0, "queryeval", E::addRestrictionFeature(), {{FeatureSetRestrictionDef}} },
			{"/query/eval/selection", "()", FeatureSetSelectionDef},
			{"/query/eval/selection", 0, "queryeval", E::addSelectionFeature(), {{FeatureSetSelectionDef}} },
			{"/query/eval/exclusion", "()", FeatureSetExclusionDef},
			{"/query/eval/exclusion", 0, "queryeval", E::addExclusionFeature(), {{FeatureSetExclusionDef}} },

			{"/query/eval//result/name", "()", QueryEvalFunctionResultName},
			{"/query/eval//result/value", "()", QueryEvalFunctionResultValue},
			{"/query/eval//result", QueryEvalFunctionResult, {
					{"name", QueryEvalFunctionResultName},
					{"value", QueryEvalFunctionResultValue}
				}
			},
			{"/query/eval//param/name", "()", QueryEvalFunctionParameterName},
			{"/query/eval//param/value", "()", QueryEvalFunctionParameterValue},
			{"/query/eval//param", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionParameterName},
					{"value", QueryEvalFunctionParameterValue}
				}
			},
			{"/query/eval//feature/name", "()", QueryEvalFunctionFeatureName},
			{"/query/eval//feature/value", "()", QueryEvalFunctionFeatureValue},
			{"/query/eval//feature", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionFeatureName},
					{"feature", QueryEvalFunctionFeatureValue}
				}
			},
			{"/query/eval/summarizer/name", "()", QueryEvalFunctionName},
			{"/query/eval/summarizer", 0, "queryeval", E::addSummarizer(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'},
					{QueryEvalFunctionResult, '*'}}
			},
			{"/query/eval/weighting/name", "()", QueryEvalFunctionName},
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
			{"/query/analyzer/element/type", "()", FeatureTypeName},
			{"/query/analyzer/element/field", "()", FieldTypeName},

			{"/query/analyzer/element/tokenizer/name", "()", TokenizerName},
			{"/query/analyzer/element/tokenizer/arg", "()", TokenizerArg},
			{"/query/analyzer/element/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"/query/analyzer/element/normalizer/name", "()", NormalizerName},
			{"/query/analyzer/element/normalizer/arg", "()", NormalizerArg},
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
			{"/query/analyzer/group/field", "()", FieldTypeName},
			{"/query/analyzer/group/op", "()", JoinOperatorName},
			{"/query/analyzer/group/range", "()", JoinOperatorRange},
			{"/query/analyzer/group/cardinality", "()", JoinOperatorCardinality},
			{"/query/analyzer/group@by", "", GroupBy},
			{"/query/analyzer/group", 0, "analyzer", A::defineImplicitGroupBy(), {
					{FieldTypeName,'!'},
					{GroupBy,'!'},
					{JoinOperatorName,'!'},
					{JoinOperatorRange,'?'},
					{JoinOperatorCardinality,'?'}}
			}
		};
	}

	static papuga::RequestAutomaton_NodeList declareTermExpression()
	{
		return {
			{"/query/feature//term/variable", "()", ExpressionVariableName},
			{"/query/feature//term/type", "()", FieldTypeName},
			{"/query/feature//term/value", "()", TermValue},
			{"/query/feature//term", TermExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"type", FieldTypeName, '!'},
					{"value", TermValue, '!'}
				}
			},
			{"/query/feature//expr/variable", "()", ExpressionVariableName},
			{"/query/feature//expr/op", "()", JoinOperatorName},
			{"/query/feature//expr/range", "()", JoinOperatorRange},
			{"/query/feature//expr/cardinality", "()", JoinOperatorCardinality},
			{"/query/feature//expr/arg", "()", TermExpression},
			{"/query/feature//expr", TermExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"op", JoinOperatorName, '!'},
					{"range", JoinOperatorRange, '?'},
					{"cardinality", JoinOperatorRange, '?'},
					{"arg", TermExpression, '*'}
				}
			},
			{"/query/feature/set", "()", FeatureSet},
			{"/query/feature/weight", "()", FeatureWeight},
		};
	}

	static papuga::RequestAutomaton_NodeList declareMetaData()
	{
		return {
			{"/query/restriction//condition/op", "()", MetaDataConditionOp},
			{"/query/restriction//condition/name", "()", MetaDataConditionName},
			{"/query/restriction//condition/value", "()", MetaDataConditionValue},
			{"/query/restriction//condition", MetaDataCondition, {
					{MetaDataConditionOp},
					{MetaDataConditionName},
					{MetaDataConditionValue}
				}
			},
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
			{declareTermExpression()},
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
			{declareTermExpression()},
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
			{declareTermExpression()},
			{"/query/feature", 0, "query", Q::addFeature(), {{FeatureSet}, {TermExpression}, {FeatureWeight, '?'}} },

			{declareMetaData()},
			{"/query/restriction", 0, "query", Q::addMetaDataRestriction(),  {MetaDataCondition} }
		};
	}

	static papuga::RequestAutomaton_NodeList defineStatistics()
	{
		typedef bindings::method::Query Q;
		return {
			{"/query/termstats/type", "()", TermType},
			{"/query/termstats/value", "()", TermValue},
			{"/query/termstats/df", "()", TermDocumentFrequency},
			{"/query/termstats", TermStats, {
					{"df", TermDocumentFrequency}
				}
			},
			{"/query/termstats", 0, "query", Q::defineTermStatistics(), {{TermType},{TermValue},{TermStats}} },
			{"/query/globalstats/nofdocs", "()", CollectionNofDocs},
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
			{"/query/evalset/docno", "()", Docno},
			{"/query/evalset", 0, "query", Q::addDocumentEvaluationSet(), {{Docno, '*'}} },
			{"/query/nofranks", "()", NumberOfResults},
			{"/query/nofranks", 0, "query", Q::setMaxNofRanks(), {{NumberOfResults}} },
			{"/query/minrank", "()", FirstResult},
			{"/query/minrank", 0, "query", Q::setMinRank(), {{FirstResult}} },
			{"/query/access", "()", AccessRight},
			{"/query/access", 0, "query", Q::addAccess(), {{AccessRight, '*'}} },

			{"/query/weightvar/name", "()", VariableName},
			{"/query/weightvar/value", "()", VariableValue},
			{"/query/weightvar", VariableDef, {
					{VariableName},
					{VariableValue}
				}
			},
			{"/query", 0, "query", Q::setWeightingVariables(), {{VariableDef, '*'}} },
			{"/query/debugmode", "()", DebugModeFlag},
			{"/query/debugmode", 0, "query", Q::setDebugMode(), {{DebugModeFlag}} },
			{"/query/tostring", "dump", "query", Q::tostring(), {} }
		};
	}
};

class Scheme_QueryAnalyzer_GET_content :public papuga::RequestAutomaton, public SchemeQueryPart
{
public:
	Scheme_QueryAnalyzer_GET_content() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineAnalyzer()},
			{analyzeTermExpression()},
			{analyzeMetaData()}
		}
	) {}
};


class Scheme_Storage_QRYORG :public papuga::RequestAutomaton, public SchemeQueryPart
{
public:
	Scheme_Storage_QRYORG() :papuga::RequestAutomaton(
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

class Scheme_Storage_QRYANA :public papuga::RequestAutomaton, public SchemeQueryPart
{
public:
	Scheme_Storage_QRYANA() :papuga::RequestAutomaton(
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


