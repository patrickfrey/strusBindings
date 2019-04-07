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
	static papuga::RequestAutomaton_NodeList declareMetaData()
	{
		return {
			{"/query/restriction/condition/op", "()", MetaDataConditionOp, papuga_TypeString, ">:gt:ne"},
			{"/query/restriction/condition/name", "()", MetaDataConditionName, papuga_TypeString, "date"},
			{"/query/restriction/condition/value", "()", MetaDataConditionValue, papuga_TypeString, "879236"},
			{"/query/restriction/condition", MetaDataCondition, {
					{MetaDataConditionOp},
					{MetaDataConditionName},
					{MetaDataConditionValue}
				}
			},
			{"/query/restriction/union/condition/op", "()", MetaDataConditionOp, papuga_TypeString, ">:gt:ne"},
			{"/query/restriction/union/condition/name", "()", MetaDataConditionName, papuga_TypeString, "date"},
			{"/query/restriction/union/condition/value", "()", MetaDataConditionValue, papuga_TypeString, "879236"},
			{"/query/restriction/union/condition", MetaDataCondition, {
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

	static papuga::RequestAutomaton_NodeList declareFeature()
	{
		return {
			{SchemaExpressionPart::declarePostingsExpression( "/query/feature/content")},
			{"/query/feature/set", "()", FeatureSet, papuga_TypeString, "weighted"},
			{"/query/feature/weight", "()", FeatureWeight, papuga_TypeDouble, "0.75;1.0"},
			{"/query/feature/content", "()", TermExpression, papuga_TypeVoid, NULL},
			{"/query/feature", FeatureDef, {
					{"set", FeatureSet},
					{"weight", FeatureWeight, '?'},
					{"expression", TermExpression, '!', 2/*tag diff*/},
				}
			},
		};
	}

	static papuga::RequestAutomaton_NodeList declareSentence()
	{
		return {
			{"/query/sentence/field", "()", FieldTypeName, papuga_TypeString, "sent"},
			{"/query/sentence/content", "()", FieldValue, papuga_TypeString, "bla bla"},
			{"/query/sentence/results", "()", NumberOfResults, papuga_TypeDouble, "1;2"},
			{"/query/sentence/minweight", "()", MinWeight, papuga_TypeDouble, "0.75;1.0"},
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
			{"/query/minrank", "()", FirstResult, papuga_TypeInt, "0;10"},
			{"/query/minrank", 0, "query", Q::setMinRank(), {{FirstResult}} },
			{"/query/access", "()", AccessRight, papuga_TypeString, "customer"},
			{"/query/access", 0, "query", Q::addAccess(), {{AccessRight, '*'}} },

			{"/query", 0, "query", Q::setWeightingVariables(), {{VariableDef, '*'}} }
		};
	}
};

}}//namespace
#endif

