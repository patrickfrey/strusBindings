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
#include "schemas_expression.hpp"

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
			{"docanalyzer/class/segmenter", "()", Segmenter, papuga_TypeString, "textwolf"},
			{"docanalyzer/class/mimetype", "()", MimeType, papuga_TypeString, "application/xml"},
			{"docanalyzer/class/encoding", "()", Charset, papuga_TypeString, "UTF-8"},
			{"docanalyzer/class/schema", "()", Schema, papuga_TypeString, "company"},
			{"docanalyzer/class", DocumentClassDef, {
					{"segmenter", Segmenter, '?'},
					{"mimetype", MimeType, '?'},
					{"encoding", Charset, '?'},
					{"schema", Schema, '?'}
				}
			},
			{"docanalyzer", "docanalyzer", "context", C::createDocumentAnalyzer(), {DocumentClassDef} },

			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/select", "()", SelectExpression, papuga_TypeString, "/doc/employee/name()"},

			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer/name", "()", TokenizerName, papuga_TypeString, "regex"},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer/arg", "()", TokenizerArg, papuga_TypeString, "[A-Za-z]+"},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "de"},
			{"docanalyzer/feature/{search,forward,metadata,attribute,lexem}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"docanalyzer/feature/{search,forward}/option/position", "()", FeatureOptionPosition, papuga_TypeString, "succ:pred:content:unique"},
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
			{"docanalyzer/feature/aggregate/type", "()", FeatureTypeName, papuga_TypeString, "nofdocs"},
			{"docanalyzer/feature/aggregate/function/name", "()", AggregatorName, papuga_TypeString, "count"},
			{"docanalyzer/feature/aggregate/function/arg", "()", AggregatorArg, papuga_TypeString, "word"},
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
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/pattern", "()", PatternTypeName, papuga_TypeString, "bibref"},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "en"},
			{"docanalyzer/feature/pattern/{search,forward,metadata,attribute}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"docanalyzer/feature/pattern/{search,forward}/option/position", "()", FeatureOptionPosition, papuga_TypeString, "succ;pred;content;unique"},
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
			{"docanalyzer/postmatcher/name", "()", PatternTypeName, papuga_TypeString, "bibref"},
			{"docanalyzer/postmatcher/module", "()", PatternMatcherModule, papuga_TypeString, "analyzer_pattern"},
			{"docanalyzer/postmatcher/lexem", "()", PatternMatcherLexemTypes, papuga_TypeString, "lexem"},
			{SchemaExpressionPart::declarePatternExpression().root("docanalyzer/postmatcher/pattern")},
			{"docanalyzer/postmatcher/pattern/name", "()", PatternRuleName, papuga_TypeString, "bibref"},
			{"docanalyzer/postmatcher/pattern/visible", "()", PatternRuleVisible, papuga_TypeBool, "true"},
			{"docanalyzer/postmatcher/pattern", PatternMatcherPatternDef, {
					{"name", PatternRuleName, '!'},
					{"visible", PatternRuleVisible, '?'},
					{"expression", TermExpression, '!'}
				}
			},
			{"docanalyzer/postmatcher", 0, "docanalyzer", A::defineTokenPatternMatcher(), {
					{PatternTypeName, '!'},
					{PatternMatcherModule, '!'},
					{PatternMatcherLexemTypes, '*'},
					{PatternMatcherPatternDef, '*'}
				}
			},
			{"docanalyzer/subdoc/name", "()", SubDocumentName, papuga_TypeString, "annotation"},
			{"docanalyzer/subdoc/select", "()", SubDocumentSelect, papuga_TypeString, "/doc/annotation"},
			{"docanalyzer/subdoc", 0, "docanalyzer", A::defineSubDocument(), {
					{SubDocumentName, '!'},
					{SubDocumentSelect, '!'}
				}
			},
			{"docanalyzer/subcontent/select", "()", SubContentSelect, papuga_TypeString, "/doc/list/customer()"},
			{"docanalyzer/subcontent/class/segmenter", "()", Segmenter, papuga_TypeString, "cjson"},
			{"docanalyzer/subcontent/class/mime", "()", MimeType, papuga_TypeString, "application/json"},
			{"docanalyzer/subcontent/class/encoding", "()", Charset, papuga_TypeString, "isolatin-1"},
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


