/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Scheme for document analysis
 * @file schemes_document.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMES_DOCUMENT_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMES_DOCUMENT_HPP_INCLUDED
#include "schemes_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemeDocumentPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineAnalyzer()
	{
		typedef bindings::method::DocumentAnalyzer A;
		typedef bindings::method::Context C;
		return {
			{"/doc/analyzer/class/segmenter", "()", Segmenter},
			{"/doc/analyzer/class/mime", "()", MimeType},
			{"/doc/analyzer/class/encoding", "()", Charset},
			{"/doc/analyzer/class/segmenter", "()", Segmenter},
			{"/doc/analyzer/class/scheme", "()", Scheme},
			{"/doc/analyzer/class", DocumentClassDef, {
					{"segmenter", Segmenter, '?'},
					{"mime", MimeType, '?'},
					{"encoding", Charset, '?'},
					{"scheme", Scheme, '?'}
				}
			},
			{"/doc/analyzer", "analyzer", "context", C::createDocumentAnalyzer(), {DocumentClassDef} },

			{"/doc/analyzer/feature/*/type", "()", FeatureTypeName},
			{"/doc/analyzer/feature/*/select", "()", SelectExpression},

			{"/doc/analyzer/feature/*/tokenizer/name", "()", TokenizerName},
			{"/doc/analyzer/feature/*/tokenizer/arg", "()", TokenizerArg},
			{"/doc/analyzer/feature/*/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"/doc/analyzer/feature/*/normalizer/name", "()", NormalizerName},
			{"/doc/analyzer/feature/*/normalizer/arg", "()", NormalizerArg},
			{"/doc/analyzer/feature/*/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"/doc/analyzer/feature/*/option/position", "()", FeatureOptionPosition},
			{"/doc/analyzer/feature/*/option", FeatureOptionDef, {
					{"position", FeatureOptionPosition, '?'},
				}
			},
			{"/doc/analyzer/feature/search", 0, "analyzer", A::addSearchIndexFeature(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeatureOptionDef,'*'}
				}
			},
			{"/doc/analyzer/feature/forward", 0, "analyzer", A::addForwardIndexFeature(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeatureOptionDef,'*'}
				}
			},
			{"/doc/analyzer/feature/lexem", 0, "analyzer", A::addPatternLexem(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			},
			{"/doc/analyzer/feature/metadata", 0, "analyzer", A::defineMetaData(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			},
			{"/doc/analyzer/feature/attribute", 0, "analyzer", A::defineAttribute(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			},
			{"/doc/analyzer/feature/aggregate/function/name", "()", AggregatorName},
			{"/doc/analyzer/feature/aggregate/function/arg", "()", AggregatorArg},
			{"/doc/analyzer/feature/aggregate/function", AggregatorDef, {
					{"name", AggregatorName, '!'},
					{"arg", AggregatorArg, '*'}
				}
			},
			{"/doc/analyzer/feature/aggregate", 0, "analyzer", A::defineAggregatedMetaData(), {
					{FeatureTypeName},
					{AggregatorDef}
				}
			},
			{"/doc/analyzer/feature/pattern/*/type", "()", FeatureTypeName},
			{"/doc/analyzer/feature/pattern/attribute", 0, "analyzer", A::defineAttributeFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'}
				}
			},
			
			{"/doc/analyzer/group/field", "()", FieldTypeName},
			{"/doc/analyzer/group/op", "()", JoinOperatorName},
			{"/doc/analyzer/group/range", "()", JoinOperatorRange},
			{"/doc/analyzer/group/cardinality", "()", JoinOperatorCardinality},
			{"/doc/analyzer/group@by", "", GroupBy},
			{"/doc/analyzer/group", 0, "analyzer", A::defineImplicitGroupBy(), {
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
			{"/doc/feature//term/variable", "()", ExpressionVariableName},
			{"/doc/feature//term/type", "()", FieldTypeName},
			{"/doc/feature//term/value", "()", TermValue},
			{"/doc/feature//term", TermExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"type", FieldTypeName, '!'},
					{"value", TermValue, '!'}
				}
			},
			{"/doc/feature//expr/variable", "()", ExpressionVariableName},
			{"/doc/feature//expr/op", "()", JoinOperatorName},
			{"/doc/feature//expr/range", "()", JoinOperatorRange},
			{"/doc/feature//expr/cardinality", "()", JoinOperatorCardinality},
			{"/doc/feature//expr/arg", "()", TermExpression},
			{"/doc/feature//expr", TermExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"op", JoinOperatorName, '!'},
					{"range", JoinOperatorRange, '?'},
					{"cardinality", JoinOperatorRange, '?'},
					{"arg", TermExpression, '*'}
				}
			},
			{"/doc/feature/set", "()", FeatureSet},
			{"/doc/feature/weight", "()", FeatureWeight},
		};
	}

	static papuga::RequestAutomaton_NodeList declareMetaData()
	{
		return {
			{"/doc/restriction//condition/op", "()", MetaDataConditionOp},
			{"/doc/restriction//condition/name", "()", MetaDataConditionName},
			{"/doc/restriction//condition/value", "()", MetaDataConditionValue},
			{"/doc/restriction//condition", MetaDataCondition, {
					{MetaDataConditionOp},
					{MetaDataConditionName},
					{MetaDataConditionValue}
				}
			},
			{"/doc/restriction/union", MetaDataCondition, {
					{MetaDataCondition, '*'}
				}
			},
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

}}//namespace
#endif


