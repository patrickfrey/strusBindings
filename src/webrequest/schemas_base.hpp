/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Defining common datastructures used to define schemas
 * @file schemas_base.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_BASE_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_BASE_HPP_INCLUDED
#include "papuga.hpp"
#include "strus/bindingClasses.h"
#include "strus/bindingObjects.h"
#include "strus/bindingMethodIds.hpp"
#include "strus/lib/bindings_description.hpp"

namespace strus {
namespace webrequest {

class AutomatonNameSpace
{
public:
	enum Item
	{
		NullValue,

		ModuleDir,ModuleName,ResourceDir,WorkDir,ContextConfig,ContextThreads,ContextRpc,
		ContextTrace,TraceLogType,TraceLogFile,TraceGroupBy,TraceCall,TraceCount,

		StatisticsMapConfig,StatisticsProc,StatisticsMapBlocks,StatisticsStorageServer,StatisticsBlob,

		StorageConfig, DatabaseEngine, DatabasePath, StorageCachedTerms,
		StorageMetadata, StorageMetadataName, StorageMetadataType,
		StorageEnableAcl, DatabaseEnableCompression, DatabaseEnableAutoCompact,
		DatabaseLruCacheSize, DatabaseMaxNofOpenFiles, DatabaseWriteBufferSize, DatabaseBlockSize,

		VectorMemType, VectorLexemPrunning, VectorDim, VectorBits, VectorVariations,
		VectorFeatureName,VectorFeatureType,VectorFeatureVector,VectorFeature,

		AnalyzerName, Segmenter, MimeType, Charset, Schema, DocumentClassDef,
		SubDocumentName, SubDocumentSelect, SubContentClassDef, SubContentSelect, 
		AnalyzerElement,FeatureTypeName,FieldTypeName,FieldValue,
		SelectExpression,FeatureOptionPosition,FeatureOptionDef,FeaturePriority,
		PatternTypeName, PatternMatcherModule, PatternMatcherLexemTypes, PatternMatcherPatternDef,
		PatternRuleName, PatternRuleVisible, PatternRuleExpression,
		AggregatorName,AggregatorArg,AggregatorDef,
		TokenizerName,TokenizerArg,TokenizerDef,
		NormalizerName,NormalizerArg,NormalizerDef,
		JoinOperatorName,JoinOperatorRange,JoinOperatorCardinality,GroupBy,
		MinWeight,

		DocumentDef,DocumentDefList,DocumentId,DocumentAccess,
		DocumentAttributeDef,DocumentAttributeName,DocumentAttributeValue,
		DocumentMetaDataDef,DocumentMetaDataName,DocumentMetaDataValue,
		DocumentSearchIndexFeatureDef, DocumentForwardIndexFeatureDef,
		DocumentFeatureType,DocumentFeatureValue,DocumentFeaturePos,DocumentFeatureLen,

		ContentAttribute, ContentRegexExpression, ContentPriorityExpression, ContentMinLength, ContentMaxLength,

		SentenceAnalyzerSeparatorChar,SentenceAnalyzerSpaceChar,
		SentenceAnalyzerLinkChar,SentenceAnalyzerLinkSubst,SentenceAnalyzerLinkDef,
		SentenceAnalyzerSentenceTermType,SentenceAnalyzerSentenceTermValue,SentenceAnalyzerSentencePatternWeight,
		SentenceAnalyzerSentencePatternOp,SentenceAnalyzerSentencePatternMinOccurrence,SentenceAnalyzerSentencePatternMaxOccurrence,
		SentenceAnalyzerSentencePattern,SentenceAnalyzerSentenceConfig,SentenceAnalyzerSentenceName,
		SentenceAnalyzerSentenceWeight,SentenceAnalyzerConfig,

		QueryDef, FeatureDef, FeatureSet, FeatureWeight, RestrictionDef,
		FeatureRestrictionDef, FeatureSelectionDef, FeatureExclusionDef,
		QueryEvalFunctionResultName, QueryEvalFunctionResultValue, QueryEvalFunctionResult,
		QueryEvalFunctionParameterName, QueryEvalFunctionParameterValue, QueryEvalFunctionParameter,
		QueryEvalFunctionFeatureRole, QueryEvalFunctionFeatureSet, QueryEvalFunctionFeature,
		QueryEvalFunctionName, QueryEvalFormulaSource,
		QueryEvalSummarizer, QueryEvalWeighting, 
		QueryEvalFormulaParameterName, QueryEvalFormulaParameterValue, QueryEvalFormulaParameter,

		QueryResult,QueryEvalPass,QueryNofRanked,QueryNofVisited,
		QueryRank,QueryRankDocno,QueryRankWeight,QueryRankSummary,
		QueryRankSummaryName,QueryRankSummaryValue,QueryRankSummaryWeight,QueryRankSummaryIndex,

		DistQueryEvalStorageServer, DistQueryEvalStatisticsServer, 

		TermExpression, NodeTerm, NodeExpression, ExpressionArg, ExpressionVariableName, TermType, TermValue, TermLen,
		MetaDataCondition, MetaDataUnionCondition, MetaDataConditionOp, MetaDataConditionName, MetaDataConditionValue,
		MetaDataRangeFrom, MetaDataRangeTo, 

		TermStats, TermDocumentFrequency, CollectionNofDocs, GlobalStats,
		Docno,NumberOfResults,FirstResult,MergeResult,AccessRight,
		VariableName,VariableValue,VariableDef,
		DebugModeFlag,

		IncludeContextName,

		NofItemDefinitions
	};

	static const char* itemName( int itemid);
};

}}//namespace
#endif

