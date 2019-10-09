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
#ifndef _STRUS_WEBREQUEST_SCHEMAS_ANALYZER_DECL_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_ANALYZER_DECL_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_expression_decl.hpp"
#include "schemas_query_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaAnalyzerPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineDocumentAnalyzer( const char* rootexpr)
	{
		typedef bindings::method::DocumentAnalyzer A;
		typedef bindings::method::Context C;
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"class/segmenter", "()", Segmenter, papuga_TypeString, "textwolf"},
			{"class/mimetype", "()", MimeType, papuga_TypeString, "application/xml"},
			{"class/encoding", "()", Charset, papuga_TypeString, "UTF-8"},
			{"class/schema", "()", Schema, papuga_TypeString, "company"},
			{"class", DocumentClassDef, {
					{"segmenter", Segmenter, '?'},
					{"mimetype", MimeType, '?'},
					{"encoding", Charset, '?'},
					{"schema", Schema, '?'}
				}
			},
			{"/", "docanalyzer", "context", C::createDocumentAnalyzer(), {DocumentClassDef} },

			{"feature/{search,forward,metadata,attribute,lexem}/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"feature/{search,forward,metadata,attribute,lexem}/select", "()", SelectExpression, papuga_TypeString, "/doc/employee/name()"},

			{"feature/{search,forward,metadata,attribute,lexem}/tokenizer/name", "()", TokenizerName, papuga_TypeString, "regex"},
			{"feature/{search,forward,metadata,attribute,lexem}/tokenizer/arg", "()", TokenizerArg, papuga_TypeString, "[A-Za-z]+"},
			{"feature/{search,forward,metadata,attribute,lexem}/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"feature/{search,forward,metadata,attribute,lexem}/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"feature/{search,forward,metadata,attribute,lexem}/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "de"},
			{"feature/{search,forward,metadata,attribute,lexem}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"feature/{search,forward,metadata,attribute,lexem}/priority", "()", FeaturePriority, papuga_TypeInt, "0:1:2:3"},
			{"feature/{search,forward}/option/position", "()", FeatureOptionPosition, papuga_TypeString, "succ:pred:content:unique"},
			{"feature/{search,forward}/option", FeatureOptionDef, {
					{"position", FeatureOptionPosition, '?'},
				}
			},
			{"feature/search", 0, "docanalyzer", A::addSearchIndexFeature(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'?'}
				}
			},
			{"feature/forward", 0, "docanalyzer", A::addForwardIndexFeature(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'?'}
				}
			},
			{"feature/lexem", 0, "docanalyzer", A::addPatternLexem(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'}
				}
			},
			{"feature/metadata", 0, "docanalyzer", A::defineMetaData(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			},
			{"feature/attribute", 0, "docanalyzer", A::defineAttribute(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'}
				}
			},
			{"feature/aggregator/type", "()", FeatureTypeName, papuga_TypeString, "nofdocs"},
			{"feature/aggregator/function/name", "()", AggregatorName, papuga_TypeString, "count"},
			{"feature/aggregator/function/arg", "()", AggregatorArg, papuga_TypeString, "word"},
			{"feature/aggregator/function", AggregatorDef, {
					{"name", AggregatorName, '!'},
					{"arg", AggregatorArg, '*'}
				}
			},
			{"feature/aggregator", 0, "docanalyzer", A::defineAggregatedMetaData(), {
					{FeatureTypeName},
					{AggregatorDef}
				}
			},
			{"feature/pattern/{search,forward,metadata,attribute}/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"feature/pattern/{search,forward,metadata,attribute}/pattern", "()", PatternTypeName, papuga_TypeString, "bibref"},
			{"feature/pattern/{search,forward,metadata,attribute}/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"feature/pattern/{search,forward,metadata,attribute}/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "en"},
			{"feature/pattern/{search,forward,metadata,attribute}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"feature/pattern/{search,forward}/option/position", "()", FeatureOptionPosition, papuga_TypeString, "succ;pred;content;unique"},
			{"feature/pattern/{search,forward}/option", FeatureOptionDef, {
					{"position", FeatureOptionPosition, '?'},
				}
			},
			{"feature/pattern/search", 0, "docanalyzer", A::addSearchIndexFeatureFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'?'}
				}
			},
			{"feature/pattern/forward", 0, "docanalyzer", A::addForwardIndexFeatureFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'?'}
				}
			},
			{"feature/pattern/metadata", 0, "docanalyzer", A::defineMetaDataFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'}
				}
			},
			{"feature/pattern/attribute", 0, "docanalyzer", A::defineAttributeFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'}
				}
			},
			{"postmatcher/name", "()", PatternTypeName, papuga_TypeString, "bibref"},
			{"postmatcher/module", "()", PatternMatcherModule, papuga_TypeString, "analyzer_pattern"},
			{"postmatcher/lexem", "()", PatternMatcherLexemTypes, papuga_TypeString, "lexem"},
			{SchemaExpressionPart::declareTermExpression( "postmatcher/pattern/analyzed")},
			{"postmatcher/pattern/name", "()", PatternRuleName, papuga_TypeString, "bibref"},
			{"postmatcher/pattern/visible", "()", PatternRuleVisible, papuga_TypeBool, "true"},
			{"postmatcher/pattern", PatternMatcherPatternDef, {
					{"name", PatternRuleName, '!'},
					{"visible", PatternRuleVisible, '?'},
					{"analyzed", TermExpression, '?'}
				}
			},
			{"postmatcher", 0, "docanalyzer", A::defineTokenPatternMatcher(), {
					{PatternTypeName, '!'},
					{PatternMatcherModule, '!'},
					{PatternMatcherLexemTypes, '*'},
					{PatternMatcherPatternDef, '*'}
				}
			},
			{"document/name", "()", SubDocumentName, papuga_TypeString, "annotation"},
			{"document/select", "()", SubDocumentSelect, papuga_TypeString, "/doc/annotation"},
			{"document", 0, "docanalyzer", A::defineSubDocument(), {
					{SubDocumentName, '!'},
					{SubDocumentSelect, '!'}
				}
			},
			{"content/select", "()", SubContentSelect, papuga_TypeString, "/doc/list/customer()"},
			{"content/class/segmenter", "()", Segmenter, papuga_TypeString, "cjson"},
			{"content/class/mime", "()", MimeType, papuga_TypeString, "application/json"},
			{"content/class/encoding", "()", Charset, papuga_TypeString, "isolatin-1"},
			{"content/class", SubContentClassDef, {
					{"segmenter", Segmenter, '?'},
					{"mime", MimeType, '?'},
					{"encoding", Charset, '?'},
					{"schema", Schema, '?'}
				}
			},
			{"content", 0, "docanalyzer", A::defineSubContent(), {
					{SubContentSelect, '!'},
					{SubContentClassDef, '!'}
				}
			}
		});
	}

	static papuga::RequestAutomaton_NodeList defineQueryAnalyzer( const char* rootexpr)
	{
		typedef bindings::method::QueryAnalyzer A;
		typedef bindings::method::Context C;
		typedef bindings::method::VectorStorageClient V;
		return { rootexpr,
		{
			{"", "qryanalyzer", "context", C::createQueryAnalyzer(), {} },
			{"element/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"element/field", "()", FieldTypeName, papuga_TypeString, "text"},

			{"element/tokenizer/name", "()", TokenizerName, papuga_TypeString, "regex"},
			{"element/tokenizer/arg", "()", TokenizerArg, papuga_TypeString, "[a-zA-Z0-9]+"},
			{"element/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"element/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"element/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "de"},
			{"element/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"element", 0, "qryanalyzer", A::addElement(), {
					{FeatureTypeName},
					{FieldTypeName},
					{TokenizerDef},
					{NormalizerDef,'+'},
				}
			},
			{"vstorage/sentanalyzer/separator", "()", SentenceAnalyzerSeparatorChar, papuga_TypeString, "0x22;0x3b;0xa"},
			{"vstorage/sentanalyzer/space", "()", SentenceAnalyzerSpaceChar, papuga_TypeString, "_;32;44;08;0xA0;0x2008;0x200B"},
			{"vstorage/sentanalyzer/link/chr", "()", SentenceAnalyzerLinkChar, papuga_TypeString, "0x22;0x2019;0x60;0x27;0x3f;0x21;0x2f;0x3b;0x3a;0x2e;0x2c;0x2d;0x2014;0x20;0x29;0x28;0x5b;0x5d;0x7b;0x7d;0x3c;0x3e;0x5f"},
			{"vstorage/sentanalyzer/link/subst", "()", SentenceAnalyzerLinkSubst, papuga_TypeString, "-;_"},
			{"vstorage/sentanalyzer/link", SentenceAnalyzerLinkDef, {
					{"chr", SentenceAnalyzerLinkChar, '+'},
					{"subst", SentenceAnalyzerLinkSubst, '!'}
				}
			},
			{"vstorage/sentanalyzer//sentterm/type", "()", SentenceAnalyzerSentenceTermType, papuga_TypeString, "word"},
			{"vstorage/sentanalyzer//sentterm/value", "()", SentenceAnalyzerSentenceTermValue, papuga_TypeString, "i;where;who"},
			{"vstorage/sentanalyzer//sentterm/weight", "()", SentenceAnalyzerSentencePatternWeight, papuga_TypeString, "i;where;who"},
			{"vstorage/sentanalyzer//sentterm", SentenceAnalyzerSentencePattern, {
					{"weight", SentenceAnalyzerSentencePatternWeight, '?'},
					{"type", SentenceAnalyzerSentenceTermType, '?'},
					{"value", SentenceAnalyzerSentenceTermValue, '?'},
				}
			},
			{"vstorage/sentanalyzer//sentpattern/op", "()", SentenceAnalyzerSentencePatternOp, papuga_TypeString, "seq;alt;repeat"},
			{"vstorage/sentanalyzer//sentpattern/min", "()", SentenceAnalyzerSentencePatternMinOccurrence, papuga_TypeString, "1;2;4"},
			{"vstorage/sentanalyzer//sentpattern/max", "()", SentenceAnalyzerSentencePatternMaxOccurrence, papuga_TypeString, "1;2;10"},
			{"vstorage/sentanalyzer//sentpattern/arg", "()", SentenceAnalyzerSentencePattern, papuga_TypeVoid, NULL},
			{"vstorage/sentanalyzer//sentpattern", SentenceAnalyzerSentencePattern, {
					{"op", SentenceAnalyzerSentencePatternOp, '?'},
					{"min", SentenceAnalyzerSentencePatternMinOccurrence, '?'},
					{"max", SentenceAnalyzerSentencePatternMaxOccurrence, '?'},
					{"arg", SentenceAnalyzerSentencePattern, '+', 2/*tag diff*/}
				}
			},
			{"vstorage/sentanalyzer/sentence/name", "()", SentenceAnalyzerSentenceName, papuga_TypeString, "question"},
			{"vstorage/sentanalyzer/sentence/weight", "()", SentenceAnalyzerSentenceWeight, papuga_TypeDouble, "0.3;0.9;1"},
			{"vstorage/sentanalyzer/sentence/sentpattern", "()", SentenceAnalyzerSentencePattern, papuga_TypeVoid, NULL},
			{"vstorage/sentanalyzer/sentence", SentenceAnalyzerSentenceConfig, {
					{"name", SentenceAnalyzerSentenceName},
					{"weight", SentenceAnalyzerSentenceWeight},
					{"sentpattern", SentenceAnalyzerSentencePattern}
				}
			},
			{"vstorage/sentanalyzer/field", "()", FieldTypeName, papuga_TypeString, "text"},
			
			{"vstorage/sentanalyzer/tokenizer/name", "()", TokenizerName, papuga_TypeString, "regex"},
			{"vstorage/sentanalyzer/tokenizer/arg", "()", TokenizerArg, papuga_TypeString, "[a-zA-Z0-9]+"},
			{"vstorage/sentanalyzer/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"vstorage/sentanalyzer/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"vstorage/sentanalyzer/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "de"},
			{"vstorage/sentanalyzer/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"vstorage/sentanalyzer", SentenceAnalyzerConfig, {
					{"separator", SentenceAnalyzerSeparatorChar, '*'},
					{"space", SentenceAnalyzerSpaceChar, '*'},
					{"link", SentenceAnalyzerLinkDef, '*'},
					{"sentence", SentenceAnalyzerSentenceConfig, '*'}
				}
			},
			{"vstorage", "_sentanalyzer", "vstorage", V::createSentenceAnalyzer(), {
					{SentenceAnalyzerConfig}
				}
			},
			{"vstorage", 0, "qryanalyzer", A::addSentenceType(), {
					{FieldTypeName, '!', 2/*tag diff*/},
					{TokenizerDef, '!', 2/*tag diff*/},
					{NormalizerDef,'+', 2/*tag diff*/},
					{"_sentanalyzer"}
				}
			},
			{"group/field", "()", FieldTypeName, papuga_TypeString, "text"},
			{"group/op", "()", JoinOperatorName, papuga_TypeString, "sequence_imm"},
			{"group/range", "()", JoinOperatorRange, papuga_TypeInt, "5"},
			{"group/cardinality", "()", JoinOperatorCardinality, papuga_TypeInt, "3"},
			{"group@by", "", GroupBy, papuga_TypeString, "position;every;all;unique"},
			{"group", 0, "qryanalyzer", A::defineImplicitGroupBy(), {
					{FieldTypeName,'!'},
					{GroupBy,'!'},
					{JoinOperatorName,'!'},
					{JoinOperatorRange,'?'},
					{JoinOperatorCardinality,'?'}}
			}
		}};
	}
};

}}//namespace
#endif


