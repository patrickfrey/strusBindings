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
	static papuga::RequestAutomaton_NodeList declareFeature( const char* rootexpr)
	{
		return {rootexpr, {
			{declareSentence("sentence")},
			{SchemaExpressionPart::declareTermExpression( "content", ContentTermExpression)},
			{SchemaExpressionPart::declareTermExpression( "analyzed", AnalyzedTermExpression)},
			{"set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"weight", "()", FeatureWeight, papuga_TypeDouble, "0.75;1.0"},
			{"", FeatureDef, {
					{"set", FeatureSet},
					{"weight", FeatureWeight, '?'},
					{"sentence", SentenceDef, '?'},
					{"content", ContentTermExpression, '*'},
					{"analyzed", AnalyzedTermExpression, '*'},
				}
			}
		}};
	}

	static papuga::RequestAutomaton_NodeList declareRestriction( const char* rootexpr)
	{
		return {rootexpr, {
			{SchemaExpressionPart::declareMetaDataExpression( "content")},
			{SchemaExpressionPart::declareMetaDataExpression( "analyzed")},
			{"content", "()", MetaDataCondition, papuga_TypeVoid, NULL},
			{"analyzed", "()", MetaDataCondition, papuga_TypeVoid, NULL},
			{"", RestrictionDef, {
					{"content", MetaDataCondition, '*'},
					{"analyzed", MetaDataCondition, '*'}
				}
			}
		}};
	}

	static papuga::RequestAutomaton_NodeList declareSentence( const char* rootexpr)
	{
		return {rootexpr, {
			{"field", "()", FieldTypeName, papuga_TypeString, "sent"},
			{"content", "()", FieldValue, papuga_TypeString, "bla bla"},
			{"results", "()", NumberOfResults, papuga_TypeDouble, "1;2"},
			{"minweight", "()", MinWeight, papuga_TypeDouble, "0.75;1.0"},
			{"", SentenceDef, {
					{"field", FieldTypeName, '!'},
					{"content", FieldValue, '!'},
					{"results", NumberOfResults, '?'},
					{"minweight", MinWeight, '?'},
				}
			}
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
		}};
	}

	static papuga::RequestAutomaton_NodeList declareQuery( const char* rootexpr)
	{
		return {rootexpr, {
			{declareFeature("feature")},
			{declareRestriction("restriction")},
			{declareStatistics("")},
			{declareRankingParameter("")}
		}};
	}

	static papuga::RequestAutomaton_NodeList buildQuery( const char* rootexpr)
	{
		typedef bindings::method::QueryBuilder QB;
		return {rootexpr, {
			{ FeatureDef/*groupid*/, {
				{"feature", 0, "qrybuilder", QB::addFeature(), {{FeatureSet}, {"_analyzed"}, {FeatureWeight, '?'}} }
			}},
			{ RestrictionDef/*groupid*/, {
				{"restriction", 0, "qrybuilder", QB::addMetaDataRestriction(),  {"_analyzed"} }
			}},
			{"", "_feature", "qrybuilder", QB::getFeatures(), {} },
			{"", "_restriction", "qrybuilder", QB::getRestrictions(), {} }
		}};
	}

	static papuga::RequestAutomaton_NodeList buildQueryFromResult( const char* rootexpr)
	{
		typedef bindings::method::QueryBuilder QB;
		return {rootexpr, {
			{"summary", 0, "qrybuilder", QB::addCollectSummary(), {{QuerySummary}} }
		}};
	}

	static papuga::RequestAutomaton_NodeList defineQueryAnalyzed( const char* rootexpr)
	{
		return {rootexpr, {
			{ FeatureDef/*groupid*/, {
				{"feature", "analyzed", "_analyzed", AnalyzedTermExpression, '*'}
			}},
			{ RestrictionDef/*groupid*/, {
				{"restriction", "analyzed", "_analyzed", MetaDataCondition, '*'}
			}}
		}};
	}

	static papuga::RequestAutomaton_NodeList analyzeQuery( const char* rootexpr)
	{
		typedef bindings::method::QueryAnalyzer A;
		return {rootexpr, {
			{ FeatureDef/*groupid*/, {
				{"feature", "analyzed", "_analyzed", AnalyzedTermExpression, '*'},
				{"feature/content", "_analyzed", "qryanalyzer", A::analyzeSchemaTermExpression(), {{ContentTermExpression}} },
				{"feature/sentence", "_analyzed", "qryanalyzer", A::analyzeSentence(), {{FieldTypeName},{FieldValue},{NumberOfResults},{MinWeight}}}
			}},
			{ RestrictionDef/*groupid*/, {
				{"restriction", "analyzed", "_analyzed", MetaDataCondition, '*'},
				{"restriction/content/{union,condition}", "_analyzed", "qryanalyzer", A::analyzeMetaDataExpression(), {{MetaDataCondition, '!'}} }
			}}
		}};
	}

	static papuga::RequestAutomaton_NodeList defineQuery( const char* rootexpr)
	{
		typedef bindings::method::Query Q;
		typedef bindings::method::QueryEval QE;
		return {rootexpr, {
			{"", "query", "qryeval", QE::createQuery(), {{"storage"}} },
			/// Features:
			{ FeatureDef/*groupid*/, {
				{"feature", 0, "query", Q::addFeature(), {{FeatureSet}, {"_analyzed"}, {FeatureWeight, '?'}} }
			}},
			{ RestrictionDef/*groupid*/, {
				{"restriction", 0, "query", Q::addMetaDataRestriction(),  {"_analyzed"} }
			}},
			/// Statistics:
			{"termstats", 0, "query", Q::defineTermStatistics(), {{TermType},{TermValue},{TermDocumentFrequency}} },
			{"globalstats", 0, "query", Q::defineGlobalStatistics(), {{GlobalStats}} },
			/// Ranking parameter:
			{"evalset", 0, "query", Q::addDocumentEvaluationSet(), {{Docno, '*'}} },
			{"nofranks", 0, "query", Q::setMaxNofRanks(), {{NumberOfResults}} },
			{"minrank", 0, "query", Q::setMinRank(), {{FirstResult}} },
			{"mergeres", 0, "query", Q::useMergeResult(), {{MergeResult}} },
			{"access", 0, "query", Q::addAccess(), {{AccessRight, '*'}} },
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

	static papuga::RequestAutomaton_NodeList defineQueryBuilder( const char* rootexpr, const char* configvar)
	{
		typedef bindings::method::QueryBuilder QB;
		typedef bindings::method::Context C;
		return {rootexpr, {
			{"", "qrybuilder", "context", C::createQueryBuilder(), {{configvar}}},
			/// Ranking parameter needed for merging:
			{"nofranks", 0, "qrybuilder", QB::setMaxNofRanks(), {{NumberOfResults}} },
			{"minrank", 0, "qrybuilder", QB::setMinRank(), {{FirstResult}} }
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

			{"include", "feature", true},
			{"include/qryeval", "qryeval", IncludeContextName, '?'},
			{"include/storage", "storage", IncludeContextName, '?'},

			{"feature", "feature", true},
			{"feature/set", "set", FeatureSet, '!'},
			{"feature/weight", "weight", FeatureWeight, '?'},
			{"feature/content", "content", ContentTermExpression, '*'},
			{"feature", "analyzed", "_analyzed", '!'},

			{"feature/sentence", "sentence", false},
			{"feature/sentence/field", "field", FieldTypeName, '!'},
			{"feature/sentence/results", "results", NumberOfResults, '!'},
			{"feature/sentence/minweight", "minweight", MinWeight, '!'},
			{"feature/sentence/content", "content", FieldValue, '!'},

			{"restriction", "restriction", true},
			{"restriction/content", "content", MetaDataCondition, '!'},
			{"restriction", "analyzed", "_analyzed", '!'}
		});
	}

	static papuga::RequestAutomaton_ResultElementDefList resultQueryOrig( const char* rootexpr)
	{
		return papuga::RequestAutomaton_ResultElementDefList( rootexpr, {
			{"evalset", "evalset", false},
			{"evalset/docno", "docno", Docno, '*'},
			{"nofranks", "nofranks", NumberOfResults, '!'},
			{"minrank", "minrank", FirstResult, '!'},
			{"mergeres", "mergeres", MergeResult, '?'},
			{"access", "access", AccessRight, '*'},

			{"include", "feature", true},
			{"include/qryeval", "qryeval", IncludeContextName, '?'},
			{"include/storage", "storage", IncludeContextName, '?'},

			{"feature", "feature", true},
			{"feature/set", "set", FeatureSet, '!'},
			{"feature/weight", "weight", FeatureWeight, '?'},
			{"feature/content", "content", ContentTermExpression, '*'},
			{"feature/analyzed", "analyzed", AnalyzedTermExpression, '*'},

			{"feature/sentence", "sentence", false},
			{"feature/sentence/field", "field", FieldTypeName, '!'},
			{"feature/sentence/results", "results", NumberOfResults, '!'},
			{"feature/sentence/minweight", "minweight", MinWeight, '!'},
			{"feature/sentence/content", "content", FieldValue, '!'},

			{"restriction", "restriction", true},
			{"restriction/content", "content", MetaDataCondition, '!'},
			{"restriction/analyzed", "analyzed", MetaDataCondition, '!'},
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
			{"ranks/field/start", "()", QueryRankFieldStart, papuga_TypeInt, "0;1;127;1241"},
			{"ranks/field/end", "()", QueryRankFieldEnd, papuga_TypeInt, "0;2;327;2346;41241"},
			{"ranks/field", QueryRankField, {
				{"start", QueryRankFieldStart},
				{"end", QueryRankFieldEnd}
			}},
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
				{"field", QueryRankField},
				{"summary", QueryRankSummary, '*'}
			}},
			{"summary/name", "()", QuerySummaryName, papuga_TypeString, "title"},
			{"summary/value", "()", QuerySummaryValue, papuga_TypeString, "Tonight (David Bowie album)"},
			{"summary/weight", "()", QuerySummaryWeight, papuga_TypeDouble, "0.14841834;3.634931;101.98547"},
			{"summary/index", "()", QuerySummaryIndex, papuga_TypeInt, "-1;0;1;2"},
			{"summary", QuerySummary, {
				{"name", QuerySummaryName},
				{"value", QuerySummaryValue},
				{"weight", QuerySummaryWeight, '?'},
				{"index", QuerySummaryIndex, '?'}
			}},
			{"", QueryResult, {
				{"evalpass", QueryEvalPass, '?'},
				{"nofranked", QueryNofRanked},
				{"nofvisited", QueryNofVisited},
				{"ranks", QueryRank, '*'},
				{"summary", QuerySummary, '*'}
			}}
		}};
	}
};

}}//namespace
#endif

