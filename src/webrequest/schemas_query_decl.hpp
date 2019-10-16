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
#ifndef _STRUS_WEBREQUEST_SCHEMAS_QUERY_DECL_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_QUERY_DECL_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_expression_decl.hpp"
#include "schemas_queryeval_decl.hpp"
#include "schemas_analyzer_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaQueryDeclPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList declareFeature( const char* rootexpr, const  char* contenttag)
	{
		return {rootexpr, {
			{SchemaExpressionPart::declareTermExpression( contenttag)},
			{"set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"weight", "()", FeatureWeight, papuga_TypeDouble, "0.75;1.0"},
			{"", FeatureDef, {
					{"set", FeatureSet},
					{"weight", FeatureWeight, '?'},
					{contenttag, TermExpression, '+'},
				}
			},
		}};
	}

	static papuga::RequestAutomaton_NodeList declareRestriction( const char* rootexpr, const  char* contenttag)
	{
		return {rootexpr, {
			{SchemaExpressionPart::declareMetaDataExpression( contenttag)},
			{contenttag, "()", MetaDataCondition, papuga_TypeVoid, NULL},
			{"", RestrictionDef, {
					{contenttag, MetaDataCondition, '+'}
				}
			},
		}};
	}

	static papuga::RequestAutomaton_NodeList declareSentence( const char* rootexpr)
	{
		return {rootexpr, {
			{"field", "()", FieldTypeName, papuga_TypeString, "sent"},
			{"content", "()", FieldValue, papuga_TypeString, "bla bla"},
			{"results", "()", NumberOfResults, papuga_TypeDouble, "1;2"},
			{"minweight", "()", MinWeight, papuga_TypeDouble, "0.75;1.0"},
		}};
	}

	static papuga::RequestAutomaton_NodeList declareStatistics( const char* rootexpr)
	{
		return {rootexpr, {
			{"termstats/type", "()", TermType, papuga_TypeString, "word"},
			{"termstats/value", "()", TermValue, papuga_TypeString, "country"},
			{"termstats/df", "()", TermDocumentFrequency, papuga_TypeInt, "312367"},
			{"termstats", TermStats, {
					{"type", TermType},
					{"value", TermValue},
					{"df", TermDocumentFrequency}
				}
			},
			{"globalstats/nofdocs", "()", CollectionNofDocs, papuga_TypeInt, "112739087"},
			{"globalstats", GlobalStats, {
					{"nofdocs", CollectionNofDocs}
				}
			},
		}};
	}

	static papuga::RequestAutomaton_NodeList declareRankingParameter( const char* rootexpr)
	{
		return {rootexpr, {
			{"evalset/docno", "()", Docno, papuga_TypeInt, "21345"},
			{"nofranks", "()", NumberOfResults, papuga_TypeInt, "5;10;20"},
			{"minrank", "()", FirstResult, papuga_TypeInt, "0;10"},
			{"mergeres", "()", MergeResult, papuga_TypeBool, "false;True;Y;n;1;0"},
			{"access", "()", AccessRight, papuga_TypeString, "customer"},
			{"debug", "()", DebugModeFlag, papuga_TypeBool, "true;false"},
		}};
	}

	static papuga::RequestAutomaton_NodeList declareQuery( const char* rootexpr, const char* contenttag)
	{
		return {rootexpr, {
			{declareFeature("feature",contenttag)},
			{declareRestriction("restriction",contenttag)},
			{declareStatistics("")},
			{declareRankingParameter("")}
		}};
	}

	static papuga::RequestAutomaton_NodeList analyzeQuery( const char* rootexpr)
	{
		typedef bindings::method::QueryAnalyzer A;
		return {rootexpr, {
			{SchemaExpressionPart::declareTermExpression( "feature/analyzed")},
			{SchemaExpressionPart::declareTermExpression( "sentence/analyzed")},
			{SchemaExpressionPart::declareMetaDataExpression( "restriction/analyzed")},

			{"feature", "analyzed", "_analyzed", TermExpression, '*'},
			{"feature/content", "_analyzed", "qryanalyzer", A::analyzeSchemaTermExpression(), {{TermExpression}} },

			{"sentence", "analyzed", "_analyzed", TermExpression, '*'},
			{"sentence", "_analyzed", "qryanalyzer", A::analyzeSentence(), {{FieldTypeName},{FieldValue},{NumberOfResults},{MinWeight}}},

			{"restriction", "analyzed", "_analyzed", MetaDataCondition, '*'},
			{"restriction/content/{union,condition}", "_analyzed", "qryanalyzer", A::analyzeMetaDataExpression(), {{MetaDataCondition, '!'}} }
		}};
	}

	static papuga::RequestAutomaton_NodeList defineQuery( const char* rootexpr)
	{
		typedef bindings::method::Query Q;
		typedef bindings::method::QueryEval QE;
		return {rootexpr, {
			{"", "query", "qryeval", QE::createQuery(), {{"storage"}} },
			/// Features:
			{"feature", 0, "query", Q::addFeature(), {{FeatureSet}, {"_analyzed"}, {FeatureWeight, '?'}} },
			{"restriction", 0, "query", Q::addMetaDataRestriction(),  {"_analyzed"} },
			/// Statistics:
			{"termstats", 0, "query", Q::defineTermStatistics(), {{TermType},{TermValue},{TermDocumentFrequency}} },
			{"globalstats", 0, "query", Q::defineGlobalStatistics(), {{GlobalStats}} },
			/// Ranking parameter:
			{"evalset", 0, "query", Q::addDocumentEvaluationSet(), {{Docno, '*'}} },
			{"nofranks", 0, "query", Q::setMaxNofRanks(), {{NumberOfResults}} },
			{"minrank", 0, "query", Q::setMinRank(), {{FirstResult}} },
			{"mergeres", 0, "query", Q::useMergeResult(), {{MergeResult}} },
			{"access", 0, "query", Q::addAccess(), {{AccessRight, '*'}} },
			{"debug", 0, "query", Q::setDebugMode(), {{DebugModeFlag, '?'}} },
			{"", 0, "query", Q::setWeightingVariables(), {{VariableDef, '*'}} }
		}};
	}

	static papuga::RequestAutomaton_NodeList defineResultMerger( const char* rootexpr)
	{
		typedef bindings::method::QueryResultMerger QM;
		typedef bindings::method::Context C;
		return {rootexpr, {
			{"", "merger", "context", C::createQueryResultMerger(), {}},
			/// Ranking parameter needed for merging:
			{"nofranks", 0, "merger", QM::setMaxNofRanks(), {{NumberOfResults}} },
			{"minrank", 0, "merger", QM::setMinRank(), {{FirstResult}} },
			{"mergeres", 0, "query", QM::useMergeResult(), {{MergeResult}} }
		}};
	}

	static papuga::RequestAutomaton_NodeList addQueryResult( const char* rootexpr)
	{
		typedef bindings::method::QueryResultMerger QM;
		return { rootexpr, {
			{"", "ranklist", "merger", QM::addQueryResult(), {{QueryResult}} }
		}};
	}

	static papuga::RequestAutomaton_NodeList mergeQueryResults( const char* rootexpr)
	{
		typedef bindings::method::QueryResultMerger QM;
		return { rootexpr, {
			{"", "ranklist", "merger", QM::evaluate(), {} }
		}};
	}

	static papuga::RequestAutomaton_NodeList evaluateQuery( const char* rootexpr)
	{
		typedef bindings::method::Query Q;
		return { rootexpr, {
			{"", "ranklist", "query", Q::evaluate(), {} }
		}};
	}

	static papuga::RequestAutomaton_ResultElementDefList resultQuery( const char* rootexpr)
	{
		return papuga::RequestAutomaton_ResultElementDefList( rootexpr, {
			{"evalset", "evalset", false},
			{"evalset/docno", "docno", Docno, '*'},
			{"nofranks", "nofranks", NumberOfResults, '!'},
			{"minrank", "minrank", FirstResult, '!'},
			{"mergeres", "mergeres", MergeResult, '?'},
			{"access", "access", AccessRight, '*'},
			{"debug", "debug", DebugModeFlag, '?'},

			{"include", "feature", true},
			{"include/qryeval", "qryeval", IncludeContextName, '?'},
			{"include/storage", "storage", IncludeContextName, '?'},

			{"feature", "feature", true},
			{"feature/set", "set", FeatureSet, '!'},
			{"feature/weight", "weight", FeatureWeight, '?'},
			{"feature/content", "content", TermExpression, '*'},
			{"feature", "analyzed", "_analyzed", '!'},
			
			{"sentence", "sentence", true},
			{"sentence/field", "field", FieldTypeName, '!'},
			{"sentence/results", "results", NumberOfResults, '!'},
			{"sentence/minweight", "minweight", MinWeight, '!'},
			{"sentence/content", "content", FieldValue, '!'},
			{"sentence", "analyzed", "_analyzed", '!'},
			
			{"restriction", "restriction", true},
			{"restriction/content", "content", MetaDataCondition, '!'},
			{"restriction", "analyzed", "_analyzed", '!'}
		});
	}

	static papuga::RequestAutomaton_NodeList declareQueryResult( const char* rootexpr)
	{
		return {rootexpr, {
			{"evalpass", "()", QueryEvalPass, papuga_TypeInt, "0;1;2"},
			{"nofranked", "()", QueryNofRanked, papuga_TypeInt, "0;2;12;20"},
			{"nofvisited", "()", QueryNofVisited, papuga_TypeInt, "312367"},
			{"ranks/docno", "()", QueryRankDocno, papuga_TypeInt, "1;133;1812473"},
			{"ranks/weight", "()", QueryRankWeight, papuga_TypeDouble, "0.7875834;1.234235;120.1241"},
			{"ranks/summary/name", "()", QueryRankSummaryName, papuga_TypeString, "title"},
			{"ranks/summary/value", "()", QueryRankSummaryValue, papuga_TypeString, "Tonight (David Bowie album)"},
			{"ranks/summary/weight", "()", QueryRankSummaryWeight, papuga_TypeDouble, "0.14841834;3.634931;101.98547"},
			{"ranks/summary/index", "()", QueryRankSummaryIndex, papuga_TypeInt, "-1;0;1;2"},
			{"ranks/summary", QueryRankSummary, {
				{"name", QueryRankSummaryName},
				{"value", QueryRankSummaryValue},
				{"weight", QueryRankSummaryWeight, '?'},
				{"index", QueryRankSummaryIndex, '?'}
			}},
			{"ranks", QueryRank, {
				{"docno", QueryRankDocno},
				{"weight", QueryRankWeight},
				{"summary", QueryRankSummary, '*'},
			}},
			{"", QueryResult, {
				{"evalpass", QueryEvalPass, '?'},
				{"nofranked", QueryNofRanked},
				{"nofvisited", QueryNofVisited},
				{"ranks", QueryRank, '*'}
			}}
		}};
	}
};

}}//namespace
#endif

