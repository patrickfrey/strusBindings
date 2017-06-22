/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_UTILS_HPP_INCLUDED
#define _STRUS_BINDING_UTILS_HPP_INCLUDED
#include "strus/analyzer/featureOptions.hpp"
#include "strus/normalizerFunctionInterface.hpp"
#include "strus/normalizerFunctionInstanceInterface.hpp"
#include "strus/tokenizerFunctionInterface.hpp"
#include "strus/tokenizerFunctionInstanceInterface.hpp"
#include "strus/aggregatorFunctionInterface.hpp"
#include "strus/aggregatorFunctionInstanceInterface.hpp"
#include "strus/summarizerFunctionInterface.hpp"
#include "strus/summarizerFunctionInstanceInterface.hpp"
#include "strus/weightingFunctionInterface.hpp"
#include "strus/weightingFunctionInstanceInterface.hpp"
#include "strus/scalarFunctionInterface.hpp"
#include "strus/scalarFunctionParserInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/queryEvalInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/storageDocumentInterface.hpp"
#include "strus/storageDocumentUpdateInterface.hpp"
#include "strus/statisticsBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/reference.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/numericVariant.hpp"
#include "strus/index.hpp"
#include "impl/metadataExpression.hpp"
#include "expressionBuilder.hpp"
#include "papuga/serialization.hpp"
#include "papuga/valueVariant.h"
#include <string>
#include <utility>

namespace strus {
namespace bindings {

struct Deserializer
{
	static void consumeClose(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static bool skipStructure(
			papuga::Serialization::const_iterator si,
			const papuga::Serialization::const_iterator& se);

	static std::string getString(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static const char* getCharpAscii(
			char* buf, std::size_t bufsize,
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static std::vector<std::string> getStringList(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static std::vector<std::string> getStringList(
			const papuga_ValueVariant& val);

	static unsigned int getUint(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static int getInt(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static Index getIndex(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static double getDouble(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static MetaDataRestrictionInterface::CompareOperator getMetaDataCmpOp(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static NumericVariant getNumeric(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static std::vector<int> getIntList(
			const papuga_ValueVariant& val);

	static std::vector<Index> getIndexList(
			const papuga_ValueVariant& val);

	static std::vector<double> getDoubleList(
			const papuga_ValueVariant& val);

	static bool hasDepth(
			papuga::Serialization::const_iterator si,
			const papuga::Serialization::const_iterator& se,
			int depth);

	static const papuga_ValueVariant* getOptionalDefinition(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se,
			const char* name);

	static analyzer::DocumentClass getDocumentClass(
			const papuga_ValueVariant& val);

	static analyzer::FeatureOptions getFeatureOptions(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static analyzer::FeatureOptions getFeatureOptions(
			const papuga_ValueVariant& options);

	static TermStatistics getTermStatistics(
			const papuga_ValueVariant& stats);

	static GlobalStatistics getGlobalStatistics(
			const papuga_ValueVariant& stats);

	static std::vector<Reference<NormalizerFunctionInstanceInterface> > getNormalizers(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static std::vector<Reference<NormalizerFunctionInstanceInterface> > getNormalizers(
			const papuga_ValueVariant& normalizers,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);
	
	static Reference<TokenizerFunctionInstanceInterface> getTokenizer(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static Reference<TokenizerFunctionInstanceInterface> getTokenizer(
			const papuga_ValueVariant& tokenizer,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);
	
	static Reference<AggregatorFunctionInstanceInterface> getAggregator(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static Reference<AggregatorFunctionInstanceInterface> getAggregator(
			const papuga_ValueVariant& aggregator,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static void buildSummarizerFunction(
			QueryEvalInterface* queryeval,
			const std::string& functionName,
			const papuga_ValueVariant& parameter,
			const papuga_ValueVariant& resultnames,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildWeightingFunction(
			QueryEvalInterface* queryeval,
			const std::string& functionName,
			const papuga_ValueVariant& parameter,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildWeightingFormula(
			QueryEvalInterface* queryeval,
			const std::string& source,
			const papuga_ValueVariant& parameter,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildExpression(
			ExpressionBuilder& builder,
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static void buildExpression(
			ExpressionBuilder& builder,
			const papuga_ValueVariant& expression,
			ErrorBufferInterface* errorhnd);

	static void buildPattern(
			ExpressionBuilder& builder,
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se,
			ErrorBufferInterface* errorhnd);

	static void buildPatterns(
			ExpressionBuilder& builder,
			const papuga_ValueVariant& patterns,
			ErrorBufferInterface* errorhnd);

	static void buildInsertDocument(
			StorageDocumentInterface* document,
			const papuga_ValueVariant& content,
			ErrorBufferInterface* errorhnd);

	static void buildUpdateDocument(
			StorageDocumentUpdateInterface* document,
			const papuga_ValueVariant& content,
			const papuga_ValueVariant& deletes,
			ErrorBufferInterface* errorhnd);

	static void buildStatistics(
			StatisticsBuilderInterface* builder,
			const papuga_ValueVariant& content,
			ErrorBufferInterface* errorhnd);

	static void buildMetaDataRestriction(
			MetaDataRestrictionInterface* builder,
			const papuga_ValueVariant& content,
			ErrorBufferInterface* errorhnd);

	static void buildMetaDataRestriction(
			QueryInterface* builder,
			const papuga_ValueVariant& content,
			ErrorBufferInterface* errorhnd);

	static void buildMetaDataRestriction(
			MetaDataExpression* termexpr,
			const papuga_ValueVariant& content,
			ErrorBufferInterface* errorhnd);

	static std::string getStorageConfigString(
			const papuga_ValueVariant& content,
			ErrorBufferInterface* errorhnd);
};

}}//namespace
#endif

