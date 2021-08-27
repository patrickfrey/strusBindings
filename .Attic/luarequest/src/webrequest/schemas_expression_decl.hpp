/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Partial schema definition to include for parsing expressions
 * @file schemas_expression_decl.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_EXPRESSION_DECL_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_EXPRESSION_DECL_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaExpressionPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList declareTermExpression( const char* rootexpr, int resultNodeType)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"", "()", resultNodeType, papuga_TypeVoid, NULL},
			{"", resultNodeType, {
					{"term", NodeTerm, '?'},
					{"expression", NodeExpression, '?'}
				}
			},
			{"//term/variable", "()", ExpressionVariableName, papuga_TypeString, "location"},
			{"//term/type", "()", TermType, papuga_TypeString, "word"},
			{"//term/value", "()", TermValue, papuga_TypeString, "city"},
			{"//term/len", "()", TermLen, papuga_TypeInt, "1;2;3"},
			{"//term", NodeTerm, {
					{"variable", ExpressionVariableName, '?'},
					{"type", TermType, '!'},
					{"value", TermValue, '!'},
					{"len", TermLen, '?'}
				}
			},
			{"//expression/variable", "()", ExpressionVariableName, papuga_TypeString, "location"},
			{"//expression/op", "()", JoinOperatorName, papuga_TypeString, "within_struct"},
			{"//expression/range", "()", JoinOperatorRange, papuga_TypeInt, "12"},
			{"//expression/cardinality", "()", JoinOperatorCardinality, papuga_TypeInt, "3"},
			{"//expression/arg", ExpressionArg, {
					{"term", NodeTerm, '?'},
					{"expression", NodeExpression, '?'}
				}
			},
			{"//expression", NodeExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"op", JoinOperatorName, '!'},
					{"range", JoinOperatorRange, '?'},
					{"cardinality", JoinOperatorCardinality, '?'},
					{"arg", ExpressionArg, '*'}
				}
			}
		});
	}

	static papuga::RequestAutomaton_NodeList declareMetaDataExpression( const char* rootexpr)
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
};

}}//namespace
#endif

