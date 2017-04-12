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
#include "strus/textProcessorInterface.hpp"
#include "strus/queryEvalInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/reference.hpp"
#include "expressionBuilder.hpp"
#include <string>

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
	
	static std::vector<std::string> getStringList(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static std::vector<std::string> getStringList(
			const ValueVariant& val);

	static unsigned int getUint(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static unsigned int getInt(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static analyzer::DocumentClass getDocumentClass(
			const ValueVariant& val);

	static analyzer::FeatureOptions getFeatureOptions(
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static analyzer::FeatureOptions getFeatureOptions(
			const ValueVariant& options);

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

	static void buildExpression(
			ExpressionBuilder& builder,
			Serialization::const_iterator& si,
			const Serialization::const_iterator& se);

	static void buildExpression(
			ExpressionBuilder& builder,
			const ValueVariant& expression);

	static void buildPatterns(
			ExpressionBuilder& builder,
			const ValueVariant& patterns);
};

}}//namespace
#endif

