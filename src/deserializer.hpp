/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_UTILS_HPP_INCLUDED
#define _STRUS_BINDING_UTILS_HPP_INCLUDED
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/serialization.hpp"
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
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static bool isStringWithPrefix(
			const ValueVariant& val,
			unsigned char prefix);

	static int getStringPrefix(
			const ValueVariant& val,
			const char* prefixList);

	static std::string getPrefixStringValue(
			const ValueVariant& val,
			unsigned char prefix);

	static bool skipStructure(
			Serialization::const_iterator si,
			const Serialization::const_iterator& se);

	static std::string getString(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static const char* getCharp(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static std::vector<std::string> getStringList(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static std::vector<std::string> getStringList(
			const ValueVariant& val);

	static unsigned int getUint(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static int getInt(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static Index getIndex(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static double getDouble(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static std::vector<int> getIntList(
			const ValueVariant& val);

	static std::vector<Index> getIndexList(
			const ValueVariant& val);

	static std::vector<double> getDoubleList(
			const ValueVariant& val);

	static analyzer::DocumentClass getDocumentClass(
			const ValueVariant& val);

	static analyzer::FeatureOptions getFeatureOptions(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static analyzer::FeatureOptions getFeatureOptions(
			const ValueVariant& options);

	static TermStatistics getTermStatistics(
			const ValueVariant& stats);

	static GlobalStatistics getGlobalStatistics(
			const ValueVariant& stats);

	static std::vector<Reference<NormalizerFunctionInstanceInterface> > getNormalizers(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static std::vector<Reference<NormalizerFunctionInstanceInterface> > getNormalizers(
			const ValueVariant& normalizers,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);
	
	static Reference<TokenizerFunctionInstanceInterface> getTokenizer(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static Reference<TokenizerFunctionInstanceInterface> getTokenizer(
			const ValueVariant& tokenizer,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);
	
	static Reference<AggregatorFunctionInstanceInterface> getAggregator(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static Reference<AggregatorFunctionInstanceInterface> getAggregator(
			const ValueVariant& aggregator,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static void buildSummarizerFunction(
			QueryEvalInterface* queryeval,
			const std::string& functionName,
			const ValueVariant& parameter,
			const ValueVariant& resultnames,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildWeightingFunction(
			QueryEvalInterface* queryeval,
			const std::string& functionName,
			const ValueVariant& parameter,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildWeightingFormula(
			QueryEvalInterface* queryeval,
			const std::string& source,
			const ValueVariant& parameter,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildExpression(
			ExpressionBuilder& builder,
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static void buildExpression(
			ExpressionBuilder& builder,
			const ValueVariant& expression,
			ErrorBufferInterface* errorhnd);

	static void buildPatterns(
			ExpressionBuilder& builder,
			const ValueVariant& patterns,
			ErrorBufferInterface* errorhnd);

	static void buildInsertDocument(
			StorageDocumentInterface* document,
			const ValueVariant& content,
			ErrorBufferInterface* errorhnd);

	static void buildUpdateDocument(
			StorageDocumentUpdateInterface* document,
			const ValueVariant& content,
			const ValueVariant& deletes,
			ErrorBufferInterface* errorhnd);

	static void buildStatistics(
			StatisticsBuilderInterface* builder,
			const ValueVariant& content,
			ErrorBufferInterface* errorhnd);
};

}}//namespace
#endif

