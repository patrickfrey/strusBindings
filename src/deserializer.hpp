/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_UTILS_HPP_INCLUDED
#define _STRUS_BINDING_UTILS_HPP_INCLUDED
#include "papuga/valueVariant.hpp"
#include "papuga/serialization.hpp"
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
#include "strus/index.hpp"
#include "expressionBuilder.hpp"
#include <string>
#include <utility>

namespace strus {
namespace bindings {

struct Deserializer
{
	static void consumeClose(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static bool isStringWithPrefix(
			const papuga::ValueVariant& val,
			unsigned char prefix);

	static int getStringPrefix(
			const papuga::ValueVariant& val,
			const char* prefixList);

	static std::string getPrefixStringValue(
			const papuga::ValueVariant& val,
			unsigned char prefix);

	static bool skipStructure(
			papuga::Serialization::const_iterator si,
			const papuga::Serialization::const_iterator& se);

	static std::string getString(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static const char* getCharp(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static std::vector<std::string> getStringList(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static std::vector<std::string> getStringList(
			const papuga::ValueVariant& val);

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

	static std::vector<int> getIntList(
			const papuga::ValueVariant& val);

	static std::vector<Index> getIndexList(
			const papuga::ValueVariant& val);

	static std::vector<double> getDoubleList(
			const papuga::ValueVariant& val);

	static analyzer::DocumentClass getDocumentClass(
			const papuga::ValueVariant& val);

	static analyzer::FeatureOptions getFeatureOptions(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static analyzer::FeatureOptions getFeatureOptions(
			const papuga::ValueVariant& options);

	static TermStatistics getTermStatistics(
			const papuga::ValueVariant& stats);

	static GlobalStatistics getGlobalStatistics(
			const papuga::ValueVariant& stats);

	static std::vector<Reference<NormalizerFunctionInstanceInterface> > getNormalizers(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static std::vector<Reference<NormalizerFunctionInstanceInterface> > getNormalizers(
			const papuga::ValueVariant& normalizers,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);
	
	static Reference<TokenizerFunctionInstanceInterface> getTokenizer(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static Reference<TokenizerFunctionInstanceInterface> getTokenizer(
			const papuga::ValueVariant& tokenizer,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);
	
	static Reference<AggregatorFunctionInstanceInterface> getAggregator(
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static Reference<AggregatorFunctionInstanceInterface> getAggregator(
			const papuga::ValueVariant& aggregator,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static void buildSummarizerFunction(
			QueryEvalInterface* queryeval,
			const std::string& functionName,
			const papuga::ValueVariant& parameter,
			const papuga::ValueVariant& resultnames,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildWeightingFunction(
			QueryEvalInterface* queryeval,
			const std::string& functionName,
			const papuga::ValueVariant& parameter,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildWeightingFormula(
			QueryEvalInterface* queryeval,
			const std::string& source,
			const papuga::ValueVariant& parameter,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildExpression(
			ExpressionBuilder& builder,
			papuga::Serialization::const_iterator& si,
			const papuga::Serialization::const_iterator& se);

	static void buildExpression(
			ExpressionBuilder& builder,
			const papuga::ValueVariant& expression,
			ErrorBufferInterface* errorhnd);

	static void buildPatterns(
			ExpressionBuilder& builder,
			const papuga::ValueVariant& patterns,
			ErrorBufferInterface* errorhnd);

	static void buildInsertDocument(
			StorageDocumentInterface* document,
			const papuga::ValueVariant& content,
			ErrorBufferInterface* errorhnd);

	static void buildUpdateDocument(
			StorageDocumentUpdateInterface* document,
			const papuga::ValueVariant& content,
			const papuga::ValueVariant& deletes,
			ErrorBufferInterface* errorhnd);

	static void buildStatistics(
			StatisticsBuilderInterface* builder,
			const papuga::ValueVariant& content,
			ErrorBufferInterface* errorhnd);

	static std::string getStorageConfigString(
			const papuga::ValueVariant& content,
			ErrorBufferInterface* errorhnd);
};

}}//namespace
#endif

