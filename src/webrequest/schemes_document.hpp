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

			{"/doc/analyzer/feature/{search,forward,metadata,attribute,lexem}/type", "()", FeatureTypeName},
			{"/doc/analyzer/feature/{search,forward,metadata,attribute,lexem}/select", "()", SelectExpression},

			{"/doc/analyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer/name", "()", TokenizerName},
			{"/doc/analyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer/arg", "()", TokenizerArg},
			{"/doc/analyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"/doc/analyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer/name", "()", NormalizerName},
			{"/doc/analyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer/arg", "()", NormalizerArg},
			{"/doc/analyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"/doc/analyzer/feature/{search,forward}/option/position", "()", FeatureOptionPosition},
			{"/doc/analyzer/feature/{search,forward}/option", FeatureOptionDef, {
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
			{"/doc/analyzer/feature/pattern/{search,forward,metadata,attribute}/type", "()", FeatureTypeName},
			{"/doc/analyzer/feature/pattern/{search,forward,metadata,attribute}/pattern", "()", PatternTypeName},
			{"/doc/analyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer/name", "()", NormalizerName},
			{"/doc/analyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer/arg", "()", NormalizerArg},
			{"/doc/analyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"/doc/analyzer/feature/pattern/{search,forward}/option/position", "()", FeatureOptionPosition},
			{"/doc/analyzer/feature/pattern/{search,forward}/option", FeatureOptionDef, {
					{"position", FeatureOptionPosition, '?'},
				}
			},
			{"/doc/analyzer/feature/pattern/search", 0, "analyzer", A::addSearchIndexFeatureFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'},
					{FeatureOptionDef,'*'}
				}
			},
			{"/doc/analyzer/feature/pattern/forward", 0, "analyzer", A::addForwardIndexFeatureFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'},
					{FeatureOptionDef,'*'}
				}
			},
			{"/doc/analyzer/feature/pattern/metadata", 0, "analyzer", A::defineMetaDataFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'}
				}
			},
			{"/doc/analyzer/feature/pattern/attribute", 0, "analyzer", A::defineAttributeFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'}
				}
			},
			{"/doc/analyzer/postmatcher/name", "()", PatternTypeName},
			{"/doc/analyzer/postmatcher/module", "()", PatternMatcherModule},
			{"/doc/analyzer/postmatcher/lexem", "()", PatternMatcherLexemTypes},
			{"/doc/analyzer/postmatcher/pattern/name", "()", PatternRuleName},
			{"/doc/analyzer/postmatcher/pattern/visible", "()", PatternRuleVisible},
			{"/doc/analyzer/postmatcher/pattern/expression", "()", PatternRuleExpression},
			{"/doc/analyzer/postmatcher/pattern", PatternMatcherPatternDef, {
					{"name", PatternRuleName, '!'},
					{"visible", PatternRuleVisible, '?'},
					{"expression", PatternRuleExpression, '!'}
				}
			},
			{"/doc/analyzer/postpattern", 0, "analyzer", A::definePatternMatcherPostProc(), {
					{PatternTypeName, '!'},
					{PatternMatcherModule, '!'},
					{PatternMatcherLexemTypes, '*'},
					{PatternMatcherPatternDef, '*'}
				}
			}
		};
	}
};

class Scheme_Context_INIT_DocumentAnalyzer :public papuga::RequestAutomaton, public SchemeDocumentPart
{
public:
	Scheme_Context_INIT_DocumentAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineAnalyzer()}
		}
	) {}
};

class Scheme_Context_PUT_DocumentAnalyzer :public papuga::RequestAutomaton, public SchemeDocumentPart
{
public:
	Scheme_Context_PUT_DocumentAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineAnalyzer()}
		}
	) {}
};

}}//namespace
#endif


