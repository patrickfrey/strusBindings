/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schema for document analysis
 * @file schemas_document.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_DOCUMENT_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_DOCUMENT_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaDocumentPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineAnalyzer()
	{
		typedef bindings::method::DocumentAnalyzer A;
		typedef bindings::method::Context C;
		return {
			{"docanalyzer/class/segmenter", "()", Segmenter},
			{"docanalyzer/class/mimetype", "()", MimeType},
			{"docanalyzer/class/encoding", "()", Charset},
			{"docanalyzer/class/segmenter", "()", Segmenter},
			{"docanalyzer/class/schema", "()", Schema},
			{"docanalyzer/class", DocumentClassDef, {
					{"segmenter", Segmenter, '?'},
					{"mimetype", MimeType, '?'},
					{"encoding", Charset, '?'},
					{"schema", Schema, '?'}
				}
			},
			{"docanalyzer", "docanalyzer", "context", C::createDocumentAnalyzer(), {DocumentClassDef} },

			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/type", "()", FeatureTypeName},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/select", "()", SelectExpression},

			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer/name", "()", TokenizerName},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer/arg", "()", TokenizerArg},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer/name", "()", NormalizerName},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer/arg", "()", NormalizerArg},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"docanalyzer/feature/{search,forward}/option/position", "()", FeatureOptionPosition},
			{"docanalyzer/feature/{search,forward}/option", FeatureOptionDef, {
					{"position", FeatureOptionPosition, '?'},
				}
			},
			{"docanalyzer/feature/search", 0, "docanalyzer", A::addSearchIndexFeature(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'*'}
				}
			},
			{"docanalyzer/feature/forward", 0, "docanalyzer", A::addForwardIndexFeature(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'*'}
				}
			},
			{"docanalyzer/feature/lexem", 0, "docanalyzer", A::addPatternLexem(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'}
				}
			},
			{"docanalyzer/feature/metadata", 0, "docanalyzer", A::defineMetaData(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			},
			{"docanalyzer/feature/attribute", 0, "docanalyzer", A::defineAttribute(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			},
			{"docanalyzer/feature/aggregate/type", "()", FeatureTypeName},
			{"docanalyzer/feature/aggregate/function/name", "()", AggregatorName},
			{"docanalyzer/feature/aggregate/function/arg", "()", AggregatorArg},
			{"docanalyzer/feature/aggregate/function", AggregatorDef, {
					{"name", AggregatorName, '!'},
					{"arg", AggregatorArg, '*'}
				}
			},
			{"docanalyzer/feature/aggregate", 0, "docanalyzer", A::defineAggregatedMetaData(), {
					{FeatureTypeName},
					{AggregatorDef}
				}
			},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/type", "()", FeatureTypeName},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/pattern", "()", PatternTypeName},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer/name", "()", NormalizerName},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer/arg", "()", NormalizerArg},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"docanalyzer/feature/pattern/{search,forward}/option/position", "()", FeatureOptionPosition},
			{"docanalyzer/feature/pattern/{search,forward}/option", FeatureOptionDef, {
					{"position", FeatureOptionPosition, '?'},
				}
			},
			{"docanalyzer/feature/pattern/search", 0, "docanalyzer", A::addSearchIndexFeatureFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'*'}
				}
			},
			{"docanalyzer/feature/pattern/forward", 0, "docanalyzer", A::addForwardIndexFeatureFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'*'}
				}
			},
			{"docanalyzer/feature/pattern/metadata", 0, "docanalyzer", A::defineMetaDataFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'}
				}
			},
			{"docanalyzer/feature/pattern/attribute", 0, "docanalyzer", A::defineAttributeFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'}
				}
			},
			{"docanalyzer/postmatcher/name", "()", PatternTypeName},
			{"docanalyzer/postmatcher/module", "()", PatternMatcherModule},
			{"docanalyzer/postmatcher/lexem", "()", PatternMatcherLexemTypes},
			{"docanalyzer/postmatcher/pattern/name", "()", PatternRuleName},
			{"docanalyzer/postmatcher/pattern/visible", "()", PatternRuleVisible},
			{"docanalyzer/postmatcher/pattern/expression", "()", PatternRuleExpression},
			{"docanalyzer/postmatcher/pattern", PatternMatcherPatternDef, {
					{"name", PatternRuleName, '!'},
					{"visible", PatternRuleVisible, '?'},
					{"expression", PatternRuleExpression, '!'}
				}
			},
			{"docanalyzer/postpattern", 0, "docanalyzer", A::defineTokenPatternMatcher(), {
					{PatternTypeName, '!'},
					{PatternMatcherModule, '!'},
					{PatternMatcherLexemTypes, '*'},
					{PatternMatcherPatternDef, '*'}
				}
			},
			{"docanalyzer/subdoc/name", "()", SubDocumentName},
			{"docanalyzer/subdoc/select", "()", SubDocumentSelect},
			{"docanalyzer/subdoc", 0, "docanalyzer", A::defineSubDocument(), {
					{SubDocumentName, '!'},
					{SubDocumentSelect, '!'}
				}
			},
			{"docanalyzer/subcontent/select", "()", SubContentSelect},
			{"docanalyzer/subcontent/class/segmenter", "()", Segmenter},
			{"docanalyzer/subcontent/class/mime", "()", MimeType},
			{"docanalyzer/subcontent/class/encoding", "()", Charset},
			{"docanalyzer/subcontent/class/segmenter", "()", Segmenter},
			{"docanalyzer/subcontent/class", SubContentClassDef, {
					{"segmenter", Segmenter, '?'},
					{"mime", MimeType, '?'},
					{"encoding", Charset, '?'},
					{"schema", Schema, '?'}
				}
			},
			{"docanalyzer/subcontent", 0, "docanalyzer", A::defineSubContent(), {
					{SubContentSelect, '!'},
					{SubContentClassDef, '!'}
				}
			}
		};
	}
};

class Schema_Context_INIT_DocumentAnalyzer :public papuga::RequestAutomaton, public SchemaDocumentPart
{
public:
	Schema_Context_INIT_DocumentAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineAnalyzer()}
		}
	) {}
};

class Schema_Context_PUT_DocumentAnalyzer :public papuga::RequestAutomaton, public SchemaDocumentPart
{
public:
	Schema_Context_PUT_DocumentAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineAnalyzer()}
		}
	) {}
};

}}//namespace
#endif


