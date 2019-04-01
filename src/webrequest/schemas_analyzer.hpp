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
					{FeatureOptionDef,'*'}
				}
			},
			{"feature/forward", 0, "docanalyzer", A::addForwardIndexFeature(), {
					{FeatureTypeName},
					{SelectExpression},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'*'}
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
			{"feature/aggregate/type", "()", FeatureTypeName, papuga_TypeString, "nofdocs"},
			{"feature/aggregate/function/name", "()", AggregatorName, papuga_TypeString, "count"},
			{"feature/aggregate/function/arg", "()", AggregatorArg, papuga_TypeString, "word"},
			{"feature/aggregate/function", AggregatorDef, {
					{"name", AggregatorName, '!'},
					{"arg", AggregatorArg, '*'}
				}
			},
			{"feature/aggregate", 0, "docanalyzer", A::defineAggregatedMetaData(), {
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
					{FeatureOptionDef,'*'}
				}
			},
			{"feature/pattern/forward", 0, "docanalyzer", A::addForwardIndexFeatureFromPatternMatch(), {
					{FeatureTypeName},
					{PatternTypeName},
					{NormalizerDef,'+'},
					{FeaturePriority,'?'},
					{FeatureOptionDef,'*'}
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
			{SchemaExpressionPart::declarePatternExpression( "postmatcher/pattern")},
			{"postmatcher/pattern/name", "()", PatternRuleName, papuga_TypeString, "bibref"},
			{"postmatcher/pattern/visible", "()", PatternRuleVisible, papuga_TypeBool, "true"},
			{"postmatcher/pattern", PatternMatcherPatternDef, {
					{"name", PatternRuleName, '!'},
					{"visible", PatternRuleVisible, '?'},
					{"expression", TermExpression, '!'}
				}
			},
			{"postmatcher", 0, "docanalyzer", A::defineTokenPatternMatcher(), {
					{PatternTypeName, '!'},
					{PatternMatcherModule, '!'},
					{PatternMatcherLexemTypes, '*'},
					{PatternMatcherPatternDef, '*'}
				}
			},
			{"subdoc/name", "()", SubDocumentName, papuga_TypeString, "annotation"},
			{"subdoc/select", "()", SubDocumentSelect, papuga_TypeString, "/doc/annotation"},
			{"subdoc", 0, "docanalyzer", A::defineSubDocument(), {
					{SubDocumentName, '!'},
					{SubDocumentSelect, '!'}
				}
			},
			{"subcontent/select", "()", SubContentSelect, papuga_TypeString, "/doc/list/customer()"},
			{"subcontent/class/segmenter", "()", Segmenter, papuga_TypeString, "cjson"},
			{"subcontent/class/mime", "()", MimeType, papuga_TypeString, "application/json"},
			{"subcontent/class/encoding", "()", Charset, papuga_TypeString, "isolatin-1"},
			{"subcontent/class", SubContentClassDef, {
					{"segmenter", Segmenter, '?'},
					{"mime", MimeType, '?'},
					{"encoding", Charset, '?'},
					{"schema", Schema, '?'}
				}
			},
			{"subcontent", 0, "docanalyzer", A::defineSubContent(), {
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
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"/", "qryanalyzer", "context", C::createQueryAnalyzer(), {} },
			{"element/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"{element,sentence}/field", "()", FieldTypeName, papuga_TypeString, "text"},

			{"{element,sentence}/tokenizer/name", "()", TokenizerName, papuga_TypeString, "regex"},
			{"{element,sentence}/tokenizer/arg", "()", TokenizerArg, papuga_TypeString, "[a-zA-Z0-9]+"},
			{"{element,sentence}/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"{element,sentence}/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"{element,sentence}/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "de"},
			{"{element,sentence}/normalizer", NormalizerDef, {
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
			{"sentence", 0, "qryanalyzer", A::addSentenceType(), {
					{FieldTypeName},
					{TokenizerDef},
					{NormalizerDef,'+'},
					{"sentanalyzer"}
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
		});
	}
};

class Schema_Context_INIT_DocumentAnalyzer :public papuga::RequestAutomaton
{
public:
	Schema_Context_INIT_DocumentAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{SchemaAnalyzerPart::defineDocumentAnalyzer( "/analyzer/doc")}
		}
	) {}
};

class Schema_Context_PUT_DocumentAnalyzer :public papuga::RequestAutomaton, public SchemaAnalyzerPart
{
public:
	Schema_Context_PUT_DocumentAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineDocumentAnalyzer( "/analyzer/doc")}
		}
	) {}
};

class Schema_Context_INIT_QueryAnalyzer :public papuga::RequestAutomaton, public SchemaAnalyzerPart
{
public:
	Schema_Context_INIT_QueryAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineQueryAnalyzer( "/analyzer/query")}
		}
	) {}
};

class Schema_Context_PUT_QueryAnalyzer :public papuga::RequestAutomaton, public SchemaAnalyzerPart
{
public:
	Schema_Context_PUT_QueryAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{defineQueryAnalyzer( "/analyzer/query")}
		}
	) {}
};

}}//namespace
#endif


