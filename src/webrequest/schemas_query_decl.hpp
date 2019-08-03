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
#include "schemas_expression.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaQueryDeclPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList declareMetaDataCondition( const char* rootexpr)
	{
		return { rootexpr, {
			{"condition/op", "()", MetaDataConditionOp, papuga_TypeString, ">:gt:ne"},
			{"condition/name", "()", MetaDataConditionName, papuga_TypeString, "date"},
			{"condition/value", "()", MetaDataConditionValue, papuga_TypeString, "879236"},
			{"condition", MetaDataCondition, {
					{MetaDataConditionOp},
					{MetaDataConditionName},
					{MetaDataConditionValue}
				}
			},
			{"union/condition/op", "()", MetaDataConditionOp, papuga_TypeString, ">:gt:ne"},
			{"union/condition/name", "()", MetaDataConditionName, papuga_TypeString, "date"},
			{"union/condition/value", "()", MetaDataConditionValue, papuga_TypeString, "879236"},
			{"union/condition", MetaDataUnionCondition, {
					{MetaDataConditionOp},
					{MetaDataConditionName},
					{MetaDataConditionValue}
				}
			},
			{"union", MetaDataCondition, {
					{MetaDataUnionCondition, '*', 2/*tag diff*/}
				}
			},
		}};
	}

	static papuga::RequestAutomaton_NodeList declareOrigFeature( const char* rootexpr)
	{
		return {rootexpr, {
			{SchemaExpressionPart::declarePostingsExpression( "content")},
			{"set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"weight", "()", FeatureWeight, papuga_TypeDouble, "0.75;1.0"},
			{"content", "()", TermExpression, papuga_TypeVoid, NULL},
			{"", FeatureDef, {
					{"set", FeatureSet},
					{"weight", FeatureWeight, '?'},
					{"content", TermExpression, '!', 2/*tag diff*/},
				}
			},
		}};
	}

	static papuga::RequestAutomaton_NodeList declareAnalyzedFeature( const char* rootexpr)
	{
		return {rootexpr, {
			{SchemaExpressionPart::declarePostingsExpression( "content")},
			{"set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"weight", "()", FeatureWeight, papuga_TypeDouble, "0.75;1.0"},
			{"content", "()", TermExpression, papuga_TypeVoid, NULL},
			{"", FeatureDef, {
					{"set", FeatureSet},
					{"weight", FeatureWeight, '?'},
					{"content", TermExpression, '!', 2/*tag diff*/},
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

	static papuga::RequestAutomaton_NodeList defineStatistics( const char* rootexpr)
	{
		typedef bindings::method::Query Q;
		return {rootexpr, {
			{"termstats/type", "()", TermType, papuga_TypeString, "word"},
			{"termstats/value", "()", TermValue, papuga_TypeString, "country"},
			{"termstats/df", "()", TermDocumentFrequency, papuga_TypeInt, "312367"},
			{"termstats", TermStats, {
					{"df", TermDocumentFrequency}
				}
			},
			{"termstats", 0, "query", Q::defineTermStatistics(), {{TermType},{TermValue},{TermStats}} },
			{"globalstats/nofdocs", "()", CollectionNofDocs, papuga_TypeInt, "112739087"},
			{"globalstats", GlobalStats, {
					{"nofdocs", CollectionNofDocs}
				}
			},
			{"termstats", 0, "query", Q::defineGlobalStatistics(), {{GlobalStats}} }
		}};
	}

	static papuga::RequestAutomaton_NodeList defineRankingParameter( const char* rootexpr)
	{
		typedef bindings::method::Query Q;
		return {rootexpr, {
			{"evalset/docno", "()", Docno, papuga_TypeInt, "21345"},
			{"evalset", 0, "query", Q::addDocumentEvaluationSet(), {{Docno, '*'}} },
			{"nofranks", "()", NumberOfResults, papuga_TypeInt, "20"},
			{"nofranks", 0, "query", Q::setMaxNofRanks(), {{NumberOfResults}} },
			{"debug", "()", Docno, papuga_TypeBool, "true;false"},
			{"minrank", "()", FirstResult, papuga_TypeInt, "0;10"},
			{"minrank", 0, "query", Q::setMinRank(), {{FirstResult}} },
			{"access", "()", AccessRight, papuga_TypeString, "customer"},
			{"access", 0, "query", Q::addAccess(), {{AccessRight, '*'}} },

			{"debug", 0, "query", Q::setDebugMode(), {{DebugModeFlag, '?'}} },
			{"", 0, "query", Q::setWeightingVariables(), {{VariableDef, '*'}} }
		}};
	}
};

}}//namespace
#endif

