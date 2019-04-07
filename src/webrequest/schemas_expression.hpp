/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Partial schema definition to include for parsing expressions
 * @file schemas_expression.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_EXPRESSION_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_EXPRESSION_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaExpressionPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList declareTermExpression( const char* rootexpr)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"//term/variable", "()", ExpressionVariableName, papuga_TypeString, "location"},
			{"//term/type", "()", TermType, papuga_TypeString, "word"},
			{"//term/value", "()", TermValue, papuga_TypeString, "city"},
			{"//term", TermExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"len", TermLen, '?'},
					{"type", TermType, '!'},
					{"value", TermValue, '!'}
				}
			},
			{"//expression/variable", "()", ExpressionVariableName, papuga_TypeString, "location"},
			{"//expression/op", "()", JoinOperatorName, papuga_TypeString, "within_struct"},
			{"//expression/range", "()", JoinOperatorRange, papuga_TypeInt, "12"},
			{"//expression/cardinality", "()", JoinOperatorCardinality, papuga_TypeInt, "3"},
			{"//expression/arg", "()", TermExpression, papuga_TypeVoid, NULL},
			{"//expression", TermExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"op", JoinOperatorName, '!'},
					{"range", JoinOperatorRange, '?'},
					{"cardinality", JoinOperatorCardinality, '?'},
					{"arg", TermExpression, '*'}
				}
			}
		});
	}

	static papuga::RequestAutomaton_NodeList declarePostingsExpression( const char* rootexpr)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{declareTermExpression("")},
			{"//meta/from", "()", MetaDataRangeFrom, papuga_TypeString, "title_start"},
			{"//meta/to", "()", MetaDataRangeTo, papuga_TypeString, "title_end"},
			{"//meta", TermExpression, {
					{"from", MetaDataRangeFrom, '?'},
					{"to", MetaDataRangeTo, '?'}
				}
			}
		});
	}

	static papuga::RequestAutomaton_NodeList declarePatternExpression( const char* rootexpr)
	{
		return declareTermExpression( rootexpr);
	}
};

}}//namespace
#endif

