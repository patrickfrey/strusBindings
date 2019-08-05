/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Defining common datastructures used to define schemas
 * @file schemas_base.hpp
 */
#include "schemas_base.hpp"
#include "private/internationalization.hpp"
using namespace strus;
using namespace strus::webrequest;

const char* AutomatonNameSpace::itemName( AutomatonNameSpace::Item item)
{
	struct ItemDef {Item id; const char* name;};
	static ItemDef ar[ NofItemDefinitions+1] = {
		{NullValue, "null value"},
		{ModuleDir, "module dir"},
		{ModuleName, "module name"},
		{ResourceDir, "resource dir"},
		{WorkDir, "work dir"},
		{ContextConfig, "context config"},
		{ContextThreads, "context threads"},
		{ContextRpc, "context rpc"},
		{ContextTrace, "context trace"},
		{TraceLogType, "trace log type"},
		{TraceLogFile, "trace log file"},
		{TraceGroupBy, "trace group by"},
		{TraceCall, "trace call"},
		{TraceCount, "trace count"},
		{StatisticsMapConfig, "statistics map config"},
		{StatisticsProc, "statistics proc"},
		{StatisticsMapBlocks, "statistics map blocks"},
		{StatisticsStorageServer, "statistics storage server"},
		{StorageConfig, "storage config"},
		{DatabasePath, "database path"},
		{StorageCachedTerms, "storage cached terms"},
		{StorageMetadata, "storage metadata"},
		{StorageMetadataName, "storage metadata name"},
		{StorageMetadataType, "storage metadata type"},
		{StorageEnableAcl, "storage enable acl"},
		{DatabaseEnableCompression, "database enable compression"},
		{DatabaseEnableAutoCompact, "database enable auto compact"},
		{DatabaseLruCacheSize, "database lru cache size"},
		{DatabaseMaxNofOpenFiles, "database max nof open files"},
		{DatabaseWriteBufferSize, "database write buffer size"},
		{DatabaseBlockSize, "database block size"},
		{VectorMemType, "vector mem type"},
		{VectorLexemPrunning, "vector lexem prunning"},
		{VectorDim, "vector dim"},
		{VectorBits, "vector bits"},
		{VectorVariations, "vector variations"},
		{VectorFeatureName, "vector feature name"},
		{VectorFeatureType, "vector feature type"},
		{VectorFeatureVector, "vector feature vector"},
		{VectorFeature, "vector feature"},
		{AnalyzerName, "analyzer name"},
		{Segmenter, "segmenter"},
		{MimeType, "mime type"},
		{Charset, "charset"},
		{Schema, "schema"},
		{DocumentClassDef, "document class def"},
		{SubDocumentName, "sub document name"},
		{SubDocumentSelect, "sub document select"},
		{SubContentClassDef, "sub content class def"},
		{SubContentSelect, "sub content select"},
		{AnalyzerElement, "analyzer element"},
		{FeatureTypeName, "feature type name"},
		{FieldTypeName, "field type name"},
		{FieldValue, "field value"},
		{SelectExpression, "select expression"},
		{FeatureOptionPosition, "feature option position"},
		{FeatureOptionDef, "feature option def"},
		{FeaturePriority, "feature priority"},
		{PatternTypeName, "pattern type name"},
		{PatternMatcherModule, "pattern matcher module"},
		{PatternMatcherLexemTypes, "pattern matcher lexem types"},
		{PatternMatcherPatternDef, "pattern matcher pattern def"},
		{PatternRuleName, "pattern rule name"},
		{PatternRuleVisible, "pattern rule visible"},
		{PatternRuleExpression, "pattern rule expression"},
		{AggregatorName, "aggregator name"},
		{AggregatorArg, "aggregator arg"},
		{AggregatorDef, "aggregator def"},
		{TokenizerName, "tokenizer name"},
		{TokenizerArg, "tokenizer arg"},
		{TokenizerDef, "tokenizer def"},
		{NormalizerName, "normalizer name"},
		{NormalizerArg, "normalizer arg"},
		{NormalizerDef, "normalizer def"},
		{JoinOperatorName, "join operator name"},
		{JoinOperatorRange, "join operator range"},
		{JoinOperatorCardinality, "join operator cardinality"},
		{GroupBy, "group by"},
		{MinWeight, "min weight"},
		{DocumentDef, "document def"},
		{DocumentDefList, "document def list"},
		{DocumentId, "document id"},
		{DocumentAccess, "document access"},
		{DocumentAttributeDef, "document attribute def"},
		{DocumentAttributeName, "document attribute name"},
		{DocumentAttributeValue, "document attribute value"},
		{DocumentMetaDataDef, "document meta data def"},
		{DocumentMetaDataName, "document meta data name"},
		{DocumentMetaDataValue, "document meta data value"},
		{DocumentSearchIndexFeatureDef, "document search index feature def"},
		{DocumentForwardIndexFeatureDef, "document forward index feature def"},
		{DocumentFeatureType, "document feature type"},
		{DocumentFeatureValue, "document feature value"},
		{DocumentFeaturePos, "document feature pos"},
		{DocumentFeatureLen, "document feature len"},
		{ContentAttribute, "content attribute"},
		{ContentRegexExpression, "content regex expression"},
		{ContentPriorityExpression, "content priority expression"},
		{ContentMinLength, "content min length"},
		{ContentMaxLength, "content max length"},
		{SentenceAnalyzerSeparatorChar, "sentence analyzer separator char"},
		{SentenceAnalyzerSpaceChar, "sentence analyzer space char"},
		{SentenceAnalyzerLinkChar, "sentence analyzer link char"},
		{SentenceAnalyzerLinkSubst, "sentence analyzer link subst"},
		{SentenceAnalyzerLinkDef, "sentence analyzer link def"},
		{SentenceAnalyzerSentenceTermType, "sentence analyzer sentence term type"},
		{SentenceAnalyzerSentenceTermValue, "sentence analyzer sentence term value"},
		{SentenceAnalyzerSentencePatternWeight, "sentence analyzer sentence pattern weight"},
		{SentenceAnalyzerSentencePatternOp, "sentence analyzer sentence pattern op"},
		{SentenceAnalyzerSentencePatternMinOccurrence, "sentence analyzer sentence pattern min occurrence"},
		{SentenceAnalyzerSentencePatternMaxOccurrence, "sentence analyzer sentence pattern max occurrence"},
		{SentenceAnalyzerSentencePattern, "sentence analyzer sentence pattern"},
		{SentenceAnalyzerSentenceConfig, "sentence analyzer sentence config"},
		{SentenceAnalyzerSentenceName, "sentence analyzer sentence name"},
		{SentenceAnalyzerSentenceWeight, "sentence analyzer sentence weight"},
		{SentenceAnalyzerConfig, "sentence analyzer config"},
		{QueryDef, "query def"},
		{FeatureDef, "feature def"},
		{FeatureSet, "feature set"},
		{FeatureWeight, "feature weight"},
		{FeatureSetRestrictionDef, "feature set restriction def"},
		{FeatureSetSelectionDef, "feature set selection def"},
		{FeatureSetExclusionDef, "feature set exclusion def"},
		{QueryEvalFunctionResultName, "query eval function result name"},
		{QueryEvalFunctionResultValue, "query eval function result value"},
		{QueryEvalFunctionResult, "query eval function result"},
		{QueryEvalFunctionParameterName, "query eval function parameter name"},
		{QueryEvalFunctionParameterValue, "query eval function parameter value"},
		{QueryEvalFunctionParameter, "query eval function parameter"},
		{QueryEvalFunctionFeatureRole, "query eval function feature role"},
		{QueryEvalFunctionFeatureSet, "query eval function feature set"},
		{QueryEvalFunctionFeature, "query eval function feature"},
		{QueryEvalFunctionName, "query eval function name"},
		{QueryEvalFormulaSource, "query eval formula source"},
		{QueryEvalSummarizer, "query eval summarizer"},
		{QueryEvalWeighting, "query eval weighting"},
		{QueryEvalFormulaParameterName, "query eval formula parameter name"},
		{QueryEvalFormulaParameterValue, "query eval formula parameter value"},
		{QueryEvalFormulaParameter, "query eval formula parameter"},
		{TermExpression, "term expression"},
		{ExpressionVariableName, "expression variable name"},
		{TermType, "term type"},
		{TermValue, "term value"},
		{TermLen, "term len"},
		{MetaDataCondition, "meta data condition"},
		{MetaDataUnionCondition, "meta data union condition"},
		{MetaDataConditionOp, "meta data condition op"},
		{MetaDataConditionName, "meta data condition name"},
		{MetaDataConditionValue, "meta data condition value"},
		{MetaDataRangeFrom, "meta data range from"},
		{MetaDataRangeTo, "meta data range to"},
		{TermStats, "term stats"},
		{TermDocumentFrequency, "term document frequency"},
		{CollectionNofDocs, "collection nof docs"},
		{GlobalStats, "global stats"},
		{Docno, "docno"},
		{NumberOfResults, "number of results"},
		{FirstResult, "first result"},
		{AccessRight, "access right"},
		{VariableName, "variable name"},
		{VariableValue, "variable value"},
		{VariableDef, "variable def"},
		{DebugModeFlag, "debug mode flag"},
		{NofItemDefinitions, "nof item definitions"}
	};
	const ItemDef& idef = ar[ item];
	if (idef.id != item) throw strus::runtime_error( _TXT("internal: badly defined table of automaton items"));
	return idef.name;
}

