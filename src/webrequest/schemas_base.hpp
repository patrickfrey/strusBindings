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
	enum
	{
		NullValue,
	
		ModuleDir,ModuleName,ResourceDir,WorkDir,ContextConfig,ContextThreads,ContextRpc,ContextTrace,TraceLogType,TraceLogFile,TraceGroupBy,TraceCall,TraceCount,
	
		StorageConfig, StoragePath, StorageCachedTerms,
		StorageMetadata, StorageMetadataName, StorageMetadataType,
		StorageEnableAcl, StorageEnableCompression, StorageLruCacheSize,
		StorageMaxNofOpenFiles, StorageWriteBufferSize, StorageBlockSize,
	
		Segmenter, MimeType, Charset, Schema, DocumentClassDef,
		SubDocumentName, SubDocumentSelect, SubContentClassDef, SubContentSelect, 
		AnalyzerElement,FeatureTypeName,FieldTypeName,SelectExpression,FeatureOptionPosition,FeatureOptionDef,FeaturePriority,
		PatternTypeName, PatternMatcherModule, PatternMatcherLexemTypes, PatternMatcherPatternDef,
		PatternRuleName, PatternRuleVisible, PatternRuleExpression,
		AggregatorName,AggregatorArg,AggregatorDef,
		TokenizerName,TokenizerArg,TokenizerDef,
		NormalizerName,NormalizerArg,NormalizerDef,
		JoinOperatorName,JoinOperatorRange,JoinOperatorCardinality,GroupBy,

		ContentAttribute, ContentRegexExpression, ContentPriorityExpression, ContentMinLength, ContentMaxLength,

		FeatureSet, FeatureWeight,
		FeatureSetRestrictionDef, FeatureSetSelectionDef, FeatureSetExclusionDef,
		QueryEvalFunctionResultName, QueryEvalFunctionResultValue, QueryEvalFunctionResult,
		QueryEvalFunctionParameterName, QueryEvalFunctionParameterValue, QueryEvalFunctionParameter,
		QueryEvalFunctionFeatureName, QueryEvalFunctionFeatureValue, QueryEvalFunctionFeature,
		QueryEvalFunctionName,

		TermExpression, ExpressionVariableName, TermType, TermValue, TermLen,
		MetaDataCondition, MetaDataConditionOp, MetaDataConditionName, MetaDataConditionValue, MetaDataRangeFrom, MetaDataRangeTo, 

		TermStats, TermDocumentFrequency, CollectionNofDocs, GlobalStats,
		Docno,NumberOfResults,FirstResult,AccessRight,
		VariableName,VariableValue,VariableDef,
		DebugModeFlag
	};
};

}}//namespace
#endif

