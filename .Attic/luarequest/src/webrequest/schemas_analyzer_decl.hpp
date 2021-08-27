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
		return { rootexpr,
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

			{"feature/{search,forward,metadata,attribute}/type", "()", FeatureTypeName, papuga_TypeString, "word"},
			{"feature/{search,forward,metadata,attribute}/select", "()", SelectExpression, papuga_TypeString, "/doc/employee/name()"},

			{"feature/{search,forward,metadata,attribute}/tokenizer/name", "()", TokenizerName, papuga_TypeString, "regex"},
			{"feature/{search,forward,metadata,attribute}/tokenizer/arg", "()", TokenizerArg, papuga_TypeString, "[A-Za-z]+"},
			{"feature/{search,forward,metadata,attribute}/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"feature/{search,forward,metadata,attribute}/normalizer/name", "()", NormalizerName, papuga_TypeString, "convdia"},
			{"feature/{search,forward,metadata,attribute}/normalizer/arg", "()", NormalizerArg, papuga_TypeString, "de"},
			{"feature/{search,forward,metadata,attribute}/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"feature/{search,forward,metadata,attribute}/priority", "()", FeaturePriority, papuga_TypeInt, "0;1;2;3"},
			{"feature/{search,forward}/option/position", "()", FeatureOptionPosition, papuga_TypeString, "succ;pred;content;unique"},
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
			},
			{"field/name", "()", SearchFieldName, papuga_TypeString, "title;text"},
			{"field/scope", "()", SearchFieldScope, papuga_TypeString, "title;h1;h2;table/row;table/col"},
			{"field/select", "()", SearchFieldSelect, papuga_TypeString, "p;li"},
			{"field/key", "()", SearchFieldKey, papuga_TypeString, "table/row@id;table/col@id"},
			{"field", SearchFieldDef, {
					{"name", SearchFieldName, '!'},
					{"scope", SearchFieldScope, '!'},
					{"select", SearchFieldSelect, '!'},
					{"key", SearchFieldKey, '?'}
				}
			},
			{"field", 0, "docanalyzer", A::addSearchIndexField(), {
					{SearchFieldName},
					{SearchFieldScope},
					{SearchFieldSelect},
					{SearchFieldKey, '?'}
				}
			},
			{"structure/name", "()", SearchStructureName, papuga_TypeString, "chapter;passage;table"},
			{"structure/header", "()", SearchStructureHeader, papuga_TypeString, "title;rowname;colname"},
			{"structure/content", "()", SearchStructureContent, papuga_TypeString, "text;coltext;rowtext"},
			{"structure/class", "()", SearchStructureClass, papuga_TypeString, "cover;label;header;footer"},
			{"structure", SearchStructureDef, {
					{"name", SearchStructureName, '!'},
					{"header", SearchStructureHeader, '!'},
					{"content", SearchStructureContent, '!'},
					{"class", SearchStructureClass, '!'}
				}
			},
			{"structure", 0, "docanalyzer", A::addSearchIndexStructure(), {
					{SearchStructureName},
					{SearchStructureHeader},
					{SearchStructureContent},
					{SearchStructureClass}
				}
			}
		}};
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
			{"vstorage/sentanalyzer/expansion/type", "()", FeatureExpansionTypeName, papuga_TypeString, "N;V;word"},
			{"vstorage/sentanalyzer/expansion/sim", "()", FeatureExpansionSimilarity, papuga_TypeDouble, "0.95;0.86;0.666"},
			{"vstorage/sentanalyzer/expansion", FeatureExpansionDef, {
					{"type", FeatureExpansionTypeName, '!'},
					{"sim", FeatureExpansionSimilarity, '!'}
				}
			},
			{"vstorage", "_sentanalyzer", "vstorage", V::createSentenceLexer(), {}},
			{"vstorage", 0, "qryanalyzer", A::addSentenceType(), {
					{FieldTypeName, '!', 2/*tag diff*/},
					{TokenizerDef, '!', 2/*tag diff*/},
					{NormalizerDef,'+', 2/*tag diff*/},
					{FeatureExpansionDef,'*', 2/*tag diff*/},
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

	static papuga::RequestAutomaton_NodeList defineDocumentAnalyzed( const char* rootexpr)
	{
		return { rootexpr, {
			{"id", "()", DocumentId, papuga_TypeString, "/company/ACME"},
			{"doctype", "()", SubDocumentName, papuga_TypeString, "article"},
			{"attribute/name", "()", DocumentAttributeName, papuga_TypeString, "title"},
			{"attribute/value", "()", DocumentAttributeValue, papuga_TypeString, "the king is back"},
			{"attribute", DocumentAttributeDef, {
					{DocumentAttributeName},
					{DocumentAttributeValue}
				}
			},

			{"metadata/name", "()", DocumentMetaDataName, papuga_TypeString, "title"},
			{"metadata/value", "()", DocumentMetaDataValue, papuga_TypeString, "12;32443;4324"},
			{"metadata", DocumentMetaDataDef, {
					{DocumentMetaDataName},
					{DocumentMetaDataValue}
				}
			},

			{"{forwardindex,searchindex}/type", "()", DocumentFeatureType, papuga_TypeString, "word"},
			{"{forwardindex,searchindex}/value", "()", DocumentFeatureValue, papuga_TypeString, "hello"},
			{"{forwardindex,searchindex}/pos", "()", DocumentFeaturePos, papuga_TypeInt, "1;3;13;3452"},
			{"{forwardindex,searchindex}/len", "()", DocumentFeatureLen, papuga_TypeInt, "1;2;5"},

			{"forwardindex", DocumentForwardIndexFeatureDef, {
					{DocumentFeatureType},
					{DocumentFeatureValue},
					{DocumentFeaturePos},
					{DocumentFeatureLen,'?'}
				}
			},
			{"searchindex", DocumentSearchIndexFeatureDef, {
					{DocumentFeatureType},
					{DocumentFeatureValue},
					{DocumentFeaturePos},
					{DocumentFeatureLen,'?'}
				}
			},
			{"access", "()", DocumentAccess, papuga_TypeString, "muller;all;doe"},
			{"", DocumentDef, {
					{"docid",DocumentId,'!'},
					{"doctype",SubDocumentName,'?'},
					{"searchindex",DocumentSearchIndexFeatureDef,'*'},
					{"forwardindex",DocumentForwardIndexFeatureDef,'*'},
					{"metadata",DocumentMetaDataDef,'*'},
					{"attribute",DocumentAttributeDef,'*'},
					{"access",DocumentAccess,'*'}
				}
			},
		}};
	}
};

}}//namespace
#endif


