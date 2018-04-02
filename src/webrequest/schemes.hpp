/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Structure to build and map the Result of an XML/JSON request
 * @file requestResult.h
 */
#ifndef _STRUS_WEBREQUEST_SCHEMES_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMES_HPP_INCLUDED
#include "papuga.hpp"
#include "strus/bindingClasses.h"
#include "strus/bindingObjects.h"
#include "strus/bindingMethodIds.hpp"
#include "strus/lib/bindings_description.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

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
		StorageMetadata, StorageMetadataName, StorageMetadataValue,
		StorageEnableAcl, StorageEnableCompression, StorageLruCacheSize,
		StorageMaxNofOpenFiles, StorageWriteBufferSize, StorageBlockSize,
	
		AnalyzerElement,FeatureTypeName,FieldTypeName,
		TokenizerName,TokenizerArg,TokenizerDef,
		NormalizerName,NormalizerArg,NormalizerDef,
		JoinOperatorName,JoinOperatorRange,JoinOperatorCardinality,GroupBy,

		FeatureSet, FeatureWeight,
		FeatureSetRestrictionDef, FeatureSetSelectionDef, FeatureSetExclusionDef,
		QueryEvalFunctionResultName, QueryEvalFunctionResultValue, QueryEvalFunctionResult,
		QueryEvalFunctionParameterName, QueryEvalFunctionParameterValue, QueryEvalFunctionParameter,
		QueryEvalFunctionFeatureName, QueryEvalFunctionFeatureValue, QueryEvalFunctionFeature,
		QueryEvalFunctionName,

		TermExpression, ExpressionVariableName, TermType, TermValue,
		MetaDataCondition, MetaDataConditionOp, MetaDataConditionName, MetaDataConditionValue,

		TermStats, TermDocumentFrequency, CollectionNofDocs, GlobalStats,
		Docno,NumberOfResults,FirstResult,AccessRight,
		VariableName,VariableValue,VariableDef,
		DebugModeFlag
	};
};


class SchemeCreateContext :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	SchemeCreateContext() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"context",{},
		{
			{"/extensions/directory", "()", ModuleDir},
			{"/extensions/modules", "()", ModuleName},
			{"/data/workdir", "()", WorkDir},
			{"/data/resources", "()", ResourceDir},
			{"/context/trace", "log()", TraceLogType},
			{"/context/trace", "file()", TraceLogFile},
			{"/context/trace", "groupby()", TraceGroupBy},
			{"/context/trace", "call()", TraceCall},
			{"/context/trace", "count()", TraceCount},
			{"/context/trace", ContextTrace, {
					{"log",  TraceLogType, '!'},
					{"file", TraceLogFile, '?'},
					{"groupby", TraceGroupBy, '?'},
					{"call", TraceCall, '?'},
					{"count", TraceCount, '?'}
				}
			},
			{"/context/rpc", "()", ContextRpc},
			{"/context/threads", "()", ContextThreads},
			{"/context", ContextConfig, {
					{"rpc", ContextRpc, '?'},
					{"trace", ContextTrace, '?'},
					{"threads", ContextThreads, '?'}
				}
			},
			{"/", "context", "", bindings::method::Context::constructor(), {{(int)ContextConfig, '?'}} },
			{"/data/workdir", "", "context", bindings::method::Context::defineWorkingDirectory(), {{(int)WorkDir}} },
			{"/data/resources", "", "context", bindings::method::Context::addResourcePath(), {{(int)ResourceDir}} },
			{"/extensions/directory", "", "context", bindings::method::Context::addModulePath(), {{(int)ModuleDir}} },
			{"/extensions/modules", "", "context", bindings::method::Context::loadModule(), {{(int)ModuleName}} },
			{"/", "", "context", bindings::method::Context::endConfig(), {} }
		}
	) {}
};

class SchemeCreateStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	SchemeCreateStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"storage",{},
		{
			{"/storage", "path()", StoragePath},
			{"/storage/metadata", "name()", StorageMetadataName},
			{"/storage/metadata", "value()", StorageMetadataValue},
			{"/storage/metadata", StorageMetadata, {{"name", StorageMetadataName}, {"value", StorageMetadataValue}} },
			{"/storage", "acl()", StorageEnableAcl},
			{"/storage", "compression()", StorageEnableCompression},
			{"/storage", StorageConfig, {
					{"path", StoragePath},
					{"metadata", StorageMetadata, '*'},
					{"acl", StorageEnableAcl, '?'},
					{"compression", StorageEnableCompression, '?'}
				}
			},
			{"/storage", "success", "context", bindings::method::Context::createStorage(), {{StorageConfig}} }
		}
	) {}
};

class SchemeDestroyStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	SchemeDestroyStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"storage",{},
		{
			{"/storage", "path()", StoragePath},
			{"/storage", "success", "context", bindings::method::Context::destroyStorage(), {{StorageConfig}} }
		}
	) {}
};

class SchemeOpenStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	SchemeOpenStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"storage",{},
		{
			{"/storage/path", "()", StoragePath},
			{"/storage/cachedterms", "()", StorageCachedTerms},
			{"/storage/compression", "()", StorageEnableCompression},
			{"/storage/cache", "()", StorageLruCacheSize},
			{"/storage/max_open_files", "()", StorageMaxNofOpenFiles},
			{"/storage/write_buffer_size", "()", StorageWriteBufferSize},
			{"/storage/block_size", "()", StorageBlockSize},
			{"/storage", StorageConfig, {
					{"path", StoragePath},
					{"cachedterms", StorageCachedTerms, '?'},
					{"compression", StorageEnableCompression, '?'},
					{"cache", StorageLruCacheSize, '?'},
					{"max_open_files", StorageMaxNofOpenFiles, '?'},
					{"write_buffer_size", StorageWriteBufferSize, '?'},
					{"block_size", StorageBlockSize, '?'},
				}
			},
			{"/", "storage", "context", bindings::method::Context::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class SchemeQueryPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineQueryEval()
	{
		typedef bindings::method::QueryEval E;
		typedef bindings::method::Context C;
		return {
			{"/query", "queryeval", "context", C::createQueryEval(), {} },
			{"/query/cterm/set", "()", FeatureSet},
			{"/query/cterm/type", "()", TermType},
			{"/query/cterm/name", "()", TermValue},
			{"/query/cterm", 0, "queryeval", E::addTerm(), {{FeatureSet},{TermType},{TermValue}} },
			{"/query/restriction", "()", FeatureSetRestrictionDef},
			{"/query/restriction", 0, "queryeval", E::addRestrictionFeature(), {{FeatureSetRestrictionDef}} },
			{"/query/selection", "()", FeatureSetSelectionDef},
			{"/query/selection", 0, "queryeval", E::addSelectionFeature(), {{FeatureSetSelectionDef}} },
			{"/query/exclusion", "()", FeatureSetExclusionDef},
			{"/query/exclusion", 0, "queryeval", E::addExclusionFeature(), {{FeatureSetExclusionDef}} },

			{"/query//result/name", "()", QueryEvalFunctionResultName},
			{"/query//result/value", "()", QueryEvalFunctionResultValue},
			{"/query//result", QueryEvalFunctionResult, {
					{"name", QueryEvalFunctionResultName},
					{"value", QueryEvalFunctionResultValue}
				}
			},
			{"/query//param/name", "()", QueryEvalFunctionParameterName},
			{"/query//param/value", "()", QueryEvalFunctionParameterValue},
			{"/query//param", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionParameterName},
					{"value", QueryEvalFunctionParameterValue}
				}
			},
			{"/query//feature/name", "()", QueryEvalFunctionFeatureName},
			{"/query//feature/value", "()", QueryEvalFunctionFeatureValue},
			{"/query//feature", QueryEvalFunctionParameter, {
					{"name", QueryEvalFunctionFeatureName},
					{"feature", QueryEvalFunctionFeatureValue}
				}
			},
			{"/query/summarizer/name", "()", QueryEvalFunctionName},
			{"/query/summarizer", 0, "queryeval", E::addSummarizer(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'},
					{QueryEvalFunctionResult, '*'}}
			},
			{"/query/weighting/name", "()", QueryEvalFunctionName},
			{"/query/weighting", 0, "queryeval", E::addWeightingFunction(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'}}
			},
			{"/query/formula", 0, "queryeval", E::defineWeightingFormula(), {
					{QueryEvalFunctionName},
					{QueryEvalFunctionParameter, '*'}}
			}
		};
	}

	static papuga::RequestAutomaton_NodeList defineAnalyzer()
	{
		typedef bindings::method::QueryAnalyzer A;
		typedef bindings::method::Context C;
		return {
			{"/query", "analyzer", "context", C::createQueryAnalyzer(), {} },
			{"/query/element/type", "()", FeatureTypeName},
			{"/query/element/field", "()", FieldTypeName},
			{"/query/element/tokenizer/name", "()", TokenizerName},
			{"/query/element/tokenizer/arg", "()", TokenizerArg},
			{"/query/element/tokenizer", TokenizerDef, {
					{"name", TokenizerName, '!'},
					{"arg", TokenizerArg, '*'}
				}
			},
			{"/query/element/normalizer/name", "()", NormalizerName},
			{"/query/element/normalizer/arg", "()", NormalizerArg},
			{"/query/element/normalizer", NormalizerDef, {
					{"name", NormalizerName, '!'},
					{"arg", NormalizerArg, '*'}
				}
			},
			{"/query/element", 0, "analyzer", A::addElement(), {
					{FeatureTypeName},
					{FieldTypeName},
					{TokenizerDef},
					{NormalizerDef,'+'},
				}
			},
			{"/query/group/field", "()", FieldTypeName},
			{"/query/group/op", "()", JoinOperatorName},
			{"/query/group/range", "()", JoinOperatorRange},
			{"/query/group/cardinality", "()", JoinOperatorCardinality},
			{"/query/group@by", "", GroupBy},
			{"/query/group", 0, "analyzer", A::defineImplicitGroupBy(), {
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
			{"/query/feature//term/variable", "()", ExpressionVariableName},
			{"/query/feature//term/type", "()", FieldTypeName},
			{"/query/feature//term/value", "()", TermValue},
			{"/query/feature//term", TermExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"type", FieldTypeName, '!'},
					{"value", TermValue, '!'}
				}
			},
			{"/query/feature//expr/variable", "()", ExpressionVariableName},
			{"/query/feature//expr/op", "()", JoinOperatorName},
			{"/query/feature//expr/range", "()", JoinOperatorRange},
			{"/query/feature//expr/cardinality", "()", JoinOperatorCardinality},
			{"/query/feature//expr/arg", "()", TermExpression},
			{"/query/feature//expr", TermExpression, {
					{"variable", ExpressionVariableName, '?'},
					{"op", JoinOperatorName, '!'},
					{"range", JoinOperatorRange, '?'},
					{"cardinality", JoinOperatorRange, '?'},
					{"arg", TermExpression, '*'}
				}
			},
			{"/query/feature/set", "()", FeatureSet},
			{"/query/feature/weight", "()", FeatureWeight},
		};
	}

	static papuga::RequestAutomaton_NodeList declareMetaData()
	{
		return {
			{"/query/restriction//condition/op", "()", MetaDataConditionOp},
			{"/query/restriction//condition/name", "()", MetaDataConditionName},
			{"/query/restriction//condition/value", "()", MetaDataConditionValue},
			{"/query/restriction//condition", MetaDataCondition, {
					{MetaDataConditionOp},
					{MetaDataConditionName},
					{MetaDataConditionValue}
				}
			},
			{"/query/restriction/union", MetaDataCondition, {
					{MetaDataCondition, '*'}
				}
			},
		};
	}

	static papuga::RequestAutomaton_NodeList analyzeTermExpression()
	{
		typedef bindings::method::QueryAnalyzer A;
		return {
			{declareTermExpression()},
			{"/query/feature", "+feature", "analyzer", A::analyzeSingleTermExpression(), {{TermExpression}} },
		};
	}

	static papuga::RequestAutomaton_NodeList analyzeMetaData()
	{
		typedef bindings::method::QueryAnalyzer A;
		return {
			{declareMetaData()},
			{"/query/restriction", "+condition", "analyzer", A::analyzeMetaDataExpression(), {{MetaDataCondition, '*'}} },
		};
	}

	static papuga::RequestAutomaton_NodeList buildQueryOriginal()
	{
		typedef bindings::method::Query Q;
		typedef bindings::method::QueryAnalyzer A;
		return {
			{declareTermExpression()},
			{"/query/feature", "_feature", "analyzer", A::analyzeSingleTermExpression(), {{TermExpression}} },
			{"/query/feature", 0, "query", Q::addFeature(), {{FeatureSet}, {"_feature"}, {FeatureWeight, '?'}} },

			{declareMetaData()},
			{"/query/restriction", "_condition", "analyzer", A::analyzeMetaDataExpression(), {{MetaDataCondition, '*'}} },
			{"/query/restriction", 0, "query", Q::addMetaDataRestriction(),  {"_condition"} }
		};
	}

	static papuga::RequestAutomaton_NodeList buildQueryAnalyzed()
	{
		typedef bindings::method::Query Q;
		return {
			{declareTermExpression()},
			{"/query/feature", 0, "query", Q::addFeature(), {{FeatureSet}, {TermExpression}, {FeatureWeight, '?'}} },

			{declareMetaData()},
			{"/query/restriction", 0, "query", Q::addMetaDataRestriction(),  {MetaDataCondition} }
		};
	}

	static papuga::RequestAutomaton_NodeList defineStatistics()
	{
		typedef bindings::method::Query Q;
		return {
			{"/query/termstats/type", "()", TermType},
			{"/query/termstats/value", "()", TermValue},
			{"/query/termstats/df", "()", TermDocumentFrequency},
			{"/query/termstats", TermStats, {
					{"df", TermDocumentFrequency}
				}
			},
			{"/query/termstats", 0, "query", Q::defineTermStatistics(), {{TermType},{TermValue},{TermStats}} },
			{"/query/globalstats/nofdocs", "()", CollectionNofDocs},
			{"/query/globalstats", GlobalStats, {
					{"nofdocs", CollectionNofDocs}
				}
			},
			{"/query/termstats", 0, "query", Q::defineGlobalStatistics(), {{GlobalStats}} }
		};
	}

	static papuga::RequestAutomaton_NodeList defineRankingParameter()
	{
		typedef bindings::method::Query Q;
		return {
			{"/query/evalset/docno", "()", Docno},
			{"/query/evalset", 0, "query", Q::addDocumentEvaluationSet(), {{Docno, '*'}} },
			{"/query/nofranks", "()", NumberOfResults},
			{"/query/nofranks", 0, "query", Q::setMaxNofRanks(), {{NumberOfResults}} },
			{"/query/minrank", "()", FirstResult},
			{"/query/minrank", 0, "query", Q::setMinRank(), {{FirstResult}} },
			{"/query/access", "()", AccessRight},
			{"/query/access", 0, "query", Q::addAccess(), {{AccessRight, '*'}} },

			{"/query/weightvar/name", "()", VariableName},
			{"/query/weightvar/value", "()", VariableValue},
			{"/query/weightvar", VariableDef, {
					{VariableName},
					{VariableValue}
				}
			},
			{"/query", 0, "query", Q::setWeightingVariables(), {{VariableDef, '*'}} },
			{"/query/debugmode", "()", DebugModeFlag},
			{"/query/debugmode", 0, "query", Q::setDebugMode(), {{DebugModeFlag}} },
			{"/query/tostring", "dump", "query", Q::tostring(), {} }
		};
	}
};

class SchemeAnalyzeQuery :public papuga::RequestAutomaton, public SchemeQueryPart
{
public:
	SchemeAnalyzeQuery() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineAnalyzer()},
			{analyzeTermExpression()},
			{analyzeMetaData()}
		}
	) {}
};


class SchemeQueryStorageOriginal :public papuga::RequestAutomaton, public SchemeQueryPart
{
public:
	SchemeQueryStorageOriginal() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineQueryEval()},
			{defineAnalyzer()},
			{"/query", "query", "queryeval", bindings::method::QueryEval::createQuery(), {{"storage"}} },
			{buildQueryOriginal()},
			{defineRankingParameter()},
			{"/query", "ranklist", "query", bindings::method::Query::evaluate(), {} }
		}
	) {}
};

class SchemeQueryStorageAnalyzed :public papuga::RequestAutomaton, public SchemeQueryPart
{
public:
	SchemeQueryStorageAnalyzed() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		"result",{},
		{
			{defineQueryEval()},
			{"/query", "query", "queryeval", bindings::method::QueryEval::createQuery(), {{"storage"}} },
			{buildQueryAnalyzed()},
			{defineStatistics()},
			{defineRankingParameter()},
			{"/query", "ranklist", "query", bindings::method::Query::evaluate(), {} }
		}
	) {}
};

}}//namespace
#endif


