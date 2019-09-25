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
#include "strus/sentenceAnalyzerInstanceInterface.hpp"
#include "strus/sentenceLexerInstanceInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/reference.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/numericVariant.hpp"
#include "strus/index.hpp"
#include "impl/value/metadataExpression.hpp"
#include "expressionBuilder.hpp"
#include "papuga/serialization.h"
#include "papuga/valueVariant.h"
#include <string>
#include <utility>

namespace strus {
namespace bindings {

struct Deserializer
{
	static void consumeClose( papuga_SerializationIter& seriter);

	static bool skipStructure( papuga_SerializationIter& seriter);

	static std::string getString( papuga_SerializationIter& seriter);
	static bool compareName( const papuga_SerializationIter& seriter, const char* name, std::size_t namelen);
	static std::string getBlobBase64Decoded( papuga_SerializationIter& seriter);
	static std::string getBlobBase64Decoded( const papuga_ValueVariant& val);

	static const char* getCharpAscii( char* buf, std::size_t bufsize, papuga_SerializationIter& seriter);
	static int getCharUnicode( papuga_SerializationIter& seriter);
	static std::vector<int> getCharListUnicode( papuga_SerializationIter& seriter);
	static char getCharAscii( papuga_SerializationIter& seriter);

	static std::vector<std::string> getStringList( papuga_SerializationIter& seriter);
	static std::vector<std::string> getStringListAsValue( papuga_SerializationIter& seriter);

	static std::vector<std::string> getStringList( const papuga_ValueVariant& val);

	static unsigned int getUint( papuga_SerializationIter& seriter);

	static int getInt( const papuga_ValueVariant& val, int defaultval);
	static int getInt( papuga_SerializationIter& seriter);

	static Index getIndex( papuga_SerializationIter& seriter);

	static double getDouble( papuga_SerializationIter& seriter);
	static float getFloat( papuga_SerializationIter& seriter);

	static MetaDataRestrictionInterface::CompareOperator getMetaDataCmpOp( papuga_SerializationIter& seriter);

	static NumericVariant getNumeric( papuga_SerializationIter& seriter);

	static std::vector<int> getIntList( const papuga_ValueVariant& val);
	static std::vector<int> getIntListAsValue( const papuga_ValueVariant& val);

	static std::vector<Index> getIndexList( const papuga_ValueVariant& val);
	static std::vector<Index> getIndexListAsValue( const papuga_ValueVariant& val);

	static std::vector<double> getDoubleList( const papuga_ValueVariant& val);

	static std::vector<float> getFloatList( const papuga_ValueVariant& val);

	static const papuga_ValueVariant* getOptionalDefinition( papuga_SerializationIter& seriter, const char* name);

	static analyzer::DocumentClass getDocumentClass( const papuga_ValueVariant& val);

	static analyzer::FeatureOptions getFeatureOptions( papuga_SerializationIter& seriter);

	static analyzer::FeatureOptions getFeatureOptions( const papuga_ValueVariant& options);

	static TermStatistics getTermStatistics( const papuga_ValueVariant& stats);

	static GlobalStatistics getGlobalStatistics( const papuga_ValueVariant& stats);

	static TimeStamp getTimeStamp( papuga_SerializationIter& seriter);
	static TimeStamp getTimeStamp( const papuga_ValueVariant& tmstmp);

	static StatisticsMessage getStatisticsMessage( papuga_SerializationIter& seriter);
	static StatisticsMessage getStatisticsMessage( const papuga_ValueVariant& msg);

	static SummaryElement getSummaryElement( papuga_SerializationIter& seriter);
	static std::vector<SummaryElement> getSummaryElementList( papuga_SerializationIter& seriter);
	static std::vector<SummaryElement> getSummaryElementListValue( papuga_SerializationIter& seriter);
	static ResultDocument getResultDocument( papuga_SerializationIter& seriter);
	static std::vector<ResultDocument> getResultDocumentList( papuga_SerializationIter& seriter);
	static std::vector<ResultDocument> getResultDocumentListValue( papuga_SerializationIter& seriter);
	static QueryResult getQueryResult( papuga_SerializationIter& seriter);
	static QueryResult getQueryResult( const papuga_ValueVariant& res);
	static std::vector<QueryResult> getQueryResultList( papuga_SerializationIter& seriter);
	static std::vector<QueryResult> getQueryResultList( const papuga_ValueVariant& res);

	static std::vector<Reference<NormalizerFunctionInstanceInterface> > getNormalizers(
			papuga_SerializationIter& seriter,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static std::vector<Reference<NormalizerFunctionInstanceInterface> > getNormalizers(
			const papuga_ValueVariant& normalizers,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);
	
	static Reference<TokenizerFunctionInstanceInterface> getTokenizer(
			papuga_SerializationIter& seriter,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);

	static Reference<TokenizerFunctionInstanceInterface> getTokenizer(
			const papuga_ValueVariant& tokenizer,
			const TextProcessorInterface* textproc,
			ErrorBufferInterface* errorhnd);
	
	static Reference<AggregatorFunctionInstanceInterface> getAggregator(
			papuga_SerializationIter& seriter,
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
			const papuga_ValueVariant& features,
			const papuga_ValueVariant& resultnames,
			const QueryProcessorInterface* queryproc,
			ErrorBufferInterface* errorhnd);

	static void buildWeightingFunction(
			QueryEvalInterface* queryeval,
			const std::string& functionName,
			const papuga_ValueVariant& parameter,
			const papuga_ValueVariant& features,
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
			papuga_SerializationIter& seriter,
			bool allowList);

	static void buildExpression(
			ExpressionBuilder& builder,
			const papuga_ValueVariant& expression,
			ErrorBufferInterface* errorhnd,
			bool allowLists);

	static void buildPattern( ExpressionBuilder& builder,
			papuga_SerializationIter& seriter,
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

	static std::string getConfigString( papuga_SerializationIter& seriter);
	static std::string getConfigString( const papuga_ValueVariant& content);

	static int buildSentencePatternExpressionArguments(
			SentenceAnalyzerInstanceInterface* analyzer,
			papuga_SerializationIter& seriter,
			ErrorBufferInterface* errorhnd);

	static void buildSentencePatternExpression(
			SentenceAnalyzerInstanceInterface* analyzer,
			papuga_SerializationIter& seriter,
			ErrorBufferInterface* errorhnd);

	static void buildSentenceAnalyzer(
			SentenceAnalyzerInstanceInterface* analyzer,
			SentenceLexerInstanceInterface* lexer,
			const papuga_ValueVariant& content,
			ErrorBufferInterface* errorhnd);
};

}}//namespace
#endif

