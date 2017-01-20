/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#include "strus/bindingObjects.hpp"
#include "objInitializers.hpp"
#include "private/internationalization.hpp"
#include "private/wcharString.hpp"

WString Term::ucvalue() const
{
	return convert_uft8string_to_wstring( m_value);
}

WString Attribute::ucvalue() const
{
	return convert_uft8string_to_wstring( m_value);
}

void Document::setAttribute_obj( const String& name, const StringObject& value_)
{
	std::string value;
	initString( value, value_.ptr());
	setAttribute( name, value);
}

void DocumentAnalyzer::addSearchIndexFeature_4(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	addSearchIndexFeature_5( type, selectexpr, tokenizer_, normalizers_, String());
}

void DocumentAnalyzer::addSearchIndexFeature_5(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_,
	const String& options)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addSearchIndexFeature( type, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers, options);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addSearchIndexFeature( type, selectexpr, (const Tokenizer&)tokenizer, normalizers, options);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addSearchIndexFeature( type, selectexpr, tokenizer, (const NormalizerVector&)normalizers, options);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addSearchIndexFeature( type, selectexpr, tokenizer, normalizers, options);
		}
	}
}

void DocumentAnalyzer::addForwardIndexFeature_4(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	addForwardIndexFeature_5( type, selectexpr, tokenizer_, normalizers_, String());
}

void DocumentAnalyzer::addForwardIndexFeature_5(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_,
	const String& options)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addForwardIndexFeature( type, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers, options);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addForwardIndexFeature( type, selectexpr, (const Tokenizer&)tokenizer, normalizers, options);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addForwardIndexFeature( type, selectexpr, tokenizer, (const NormalizerVector&)normalizers, options);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addForwardIndexFeature( type, selectexpr, tokenizer, normalizers, options);
		}
	}
}

void DocumentAnalyzer::defineMetaData_obj(
	const String& fieldname,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			defineMetaData( fieldname, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			defineMetaData( fieldname, selectexpr, (const Tokenizer&)tokenizer, normalizers);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			defineMetaData( fieldname, selectexpr, tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			defineMetaData( fieldname, selectexpr, tokenizer, normalizers);
		}
	}
}

void DocumentAnalyzer::defineAggregatedMetaData_obj(
	const String& fieldname,
	const FunctionObject& function_)
{
	boost::python::extract<Aggregator> function(function_);
	if (function.check())
	{
		defineAggregatedMetaData( fieldname, (const Aggregator&)function);
	}
	else
	{
		Aggregator function;
		initAggregator( function, function_.ptr());
		defineAggregatedMetaData( fieldname, function);
	}
}

void DocumentAnalyzer::defineAttribute_obj(
	const String& fieldname,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			defineAttribute( fieldname, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			defineAttribute( fieldname, selectexpr, (const Tokenizer&)tokenizer, normalizers);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			defineAttribute( fieldname, selectexpr, tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			defineAttribute( fieldname, selectexpr, tokenizer, normalizers);
		}
	}
}

void DocumentAnalyzer::addSearchIndexFeatureFromPatternMatch_3(
	const String& type,
	const String& patternTypeName,
	const FunctionObject& normalizers_)
{
	addSearchIndexFeatureFromPatternMatch_4( type, patternTypeName, normalizers_, String());
}

void DocumentAnalyzer::addSearchIndexFeatureFromPatternMatch_4(
	const String& type,
	const String& patternTypeName,
	const FunctionObject& normalizers_,
	const String& options)
{
	boost::python::extract<NormalizerVector> normalizers(normalizers_);
	if (normalizers.check())
	{
		addSearchIndexFeatureFromPatternMatch( type, patternTypeName, (const NormalizerVector&)normalizers, options);
	}
	else
	{
		NormalizerVector normalizers;
		initNormalizerList( normalizers, normalizers_.ptr());
		addSearchIndexFeatureFromPatternMatch( type, patternTypeName, normalizers, options);
	}
}

void DocumentAnalyzer::addForwardIndexFeatureFromPatternMatch_3(
	const String& type,
	const String& patternTypeName,
	const FunctionObject& normalizers_)
{
	addForwardIndexFeatureFromPatternMatch_4( type, patternTypeName, normalizers_, String());
}

void DocumentAnalyzer::addForwardIndexFeatureFromPatternMatch_4(
	const String& type,
	const String& patternTypeName,
	const FunctionObject& normalizers_,
	const String& options)
{
	boost::python::extract<NormalizerVector> normalizers(normalizers_);
	if (normalizers.check())
	{
		addForwardIndexFeatureFromPatternMatch( type, patternTypeName, (const NormalizerVector&)normalizers, options);
	}
	else
	{
		NormalizerVector normalizers;
		initNormalizerList( normalizers, normalizers_.ptr());
		addForwardIndexFeatureFromPatternMatch( type, patternTypeName, normalizers, options);
	}
}

void DocumentAnalyzer::defineMetaDataFromPatternMatch_obj(
		const String& fieldname,
		const String& patternTypeName,
		const FunctionObject& normalizers_)
{
	boost::python::extract<NormalizerVector> normalizers(normalizers_);
	if (normalizers.check())
	{
		defineMetaDataFromPatternMatch( fieldname, patternTypeName, (const NormalizerVector&)normalizers);
	}
	else
	{
		NormalizerVector normalizers;
		initNormalizerList( normalizers, normalizers_.ptr());
		defineMetaDataFromPatternMatch( fieldname, patternTypeName, normalizers);
	}
}

void DocumentAnalyzer::defineAttributeFromPatternMatch_obj(
		const String& fieldname,
		const String& patternTypeName,
		const FunctionObject& normalizers_)
{
	boost::python::extract<NormalizerVector> normalizers(normalizers_);
	if (normalizers.check())
	{
		defineAttributeFromPatternMatch( fieldname, patternTypeName, (const NormalizerVector&)normalizers);
	}
	else
	{
		NormalizerVector normalizers;
		initNormalizerList( normalizers, normalizers_.ptr());
		defineAttributeFromPatternMatch( fieldname, patternTypeName, normalizers);
	}
}

Document DocumentAnalyzer::analyze_obj_1( const StringObject& value_)
{
	std::string value;
	initString( value, value_.ptr());
	return analyze( value);
}

Document DocumentAnalyzer::analyze_obj_2( const StringObject& value_, const DocumentClass& dclass)
{
	std::string value;
	initString( value, value_.ptr());
	return analyze( value, dclass);
}

void DocumentAnalyzer::definePatternMatcherPostProc_expr(
		const String& patternTypeName,
		const String& patternMatcherModule,
		const FunctionObject& expr_)
{
	boost::python::extract<PatternMatcher> expr(expr_);
	if (expr.check())
	{
		definePatternMatcherPostProc( patternTypeName, patternMatcherModule, (PatternMatcher&)expr);
	}
	else
	{
		PatternMatcher matcher;
		initPatternMatcher( matcher, expr_.ptr());
		definePatternMatcherPostProc( patternTypeName, patternMatcherModule, matcher);
	}
}

void DocumentAnalyzer::definePatternMatcherPostProcFromFile_obj(
		const String& patternTypeName,
		const String& patternMatcherModule,
		const StringObject& serializedPatternFile_)
{
	std::string serializedPatternFile;
	initString( serializedPatternFile, serializedPatternFile_.ptr());
	definePatternMatcherPostProcFromFile( patternTypeName, patternMatcherModule, serializedPatternFile);
}

void DocumentAnalyzeQueue::push_obj_1( const StringObject& value_)
{
	std::string value;
	initString( value, value_.ptr());
	return push( value);
}

void DocumentAnalyzeQueue::push_obj_2( const StringObject& value_, const DocumentClass& dclass)
{
	std::string value;
	initString( value, value_.ptr());
	return push( value, dclass);
}


void QueryAnalyzer::addSearchIndexElement_obj(
	const String& featureType,
	const String& fieldType,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addSearchIndexElement( featureType, fieldType, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addSearchIndexElement( featureType, fieldType, (const Tokenizer&)tokenizer, normalizers);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addSearchIndexElement( featureType, fieldType, tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addSearchIndexElement( featureType, fieldType, tokenizer, normalizers);
		}
	}
}

void QueryAnalyzer::addSearchIndexElementFromPatternMatch_obj(
	const String& type,
	const String& patternTypeName,
	const FunctionObject& normalizers_)
{
	boost::python::extract<NormalizerVector> normalizers(normalizers_);
	if (normalizers.check())
	{
		addSearchIndexElementFromPatternMatch( type, patternTypeName, (const NormalizerVector&)normalizers);
	}
	else
	{
		NormalizerVector normalizers;
		initNormalizerList( normalizers, normalizers_.ptr());
		addSearchIndexElementFromPatternMatch( type, patternTypeName, normalizers);
	}
}

void QueryAnalyzer::addPatternLexem_obj(
		const String& featureType,
		const String& fieldType,
		const FunctionObject& tokenizer_,
		const FunctionObject& normalizers_)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addPatternLexem( featureType, fieldType, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addPatternLexem( featureType, fieldType, (const Tokenizer&)tokenizer, normalizers);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addPatternLexem( featureType, fieldType, tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addPatternLexem( featureType, fieldType, tokenizer, normalizers);
		}
	}
}

void QueryAnalyzer::definePatternMatcherPostProc_expr(
		const String& patternTypeName,
		const String& patternMatcherModule,
		const FunctionObject& expr_)
{
	boost::python::extract<PatternMatcher> expr(expr_);
	if (expr.check())
	{
		definePatternMatcherPostProc( patternTypeName, patternMatcherModule, (PatternMatcher&)expr);
	}
	else
	{
		PatternMatcher matcher;
		initPatternMatcher( matcher, expr_.ptr());
		definePatternMatcherPostProc( patternTypeName, patternMatcherModule, matcher);
	}
}

void QueryAnalyzer::definePatternMatcherPostProcFromFile_obj(
		const String& patternTypeName,
		const String& patternMatcherModule,
		const StringObject& serializedPatternFile_)
{
	std::string serializedPatternFile;
	initString( serializedPatternFile, serializedPatternFile_.ptr());
	definePatternMatcherPostProcFromFile( patternTypeName, patternMatcherModule, serializedPatternFile);
}

TermVector QueryAnalyzer::analyzeField_obj(
		const String& fieldType,
		const StringObject& fieldContent)
{
	std::string value;
	initString( value, fieldContent.ptr());
	return analyzeField( fieldType, value);
}

void QueryAnalyzeContext::putField_obj( unsigned int fieldNo, const String& fieldType, const StringObject& fieldContent)
{
	std::string value;
	initString( value, fieldContent.ptr());
	putField( fieldNo, fieldType, value);
}

void StorageTransaction::deleteDocument_obj( const StringObject& docid)
{
	std::string value;
	initString( value, docid.ptr());
	deleteDocument( value);
}

void StorageTransaction::deleteUserAccessRights_obj( const StringObject& username)
{
	std::string value;
	initString( value, username.ptr());
	deleteUserAccessRights( value);
}

WString SummaryElement::ucvalue() const
{
	return convert_uft8string_to_wstring( m_value);
}


void QueryEval::addSummarizer_obj(
	const String& name,
	const FunctionObject& config_)
{
	boost::python::extract<SummarizerConfig> config(config_);
	if (config.check())
	{
		addSummarizer( name, (const SummarizerConfig&)config);
	}
	else
	{
		SummarizerConfig config;
		initSummarizerConfig( config, config_.ptr());
		addSummarizer( name, config);
	}
}

void QueryEval::addWeightingFunction_obj(
	const String& name,
	const FunctionObject& config_)
{
	boost::python::extract<WeightingConfig> config(config_);
	if (config.check())
	{
		addWeightingFunction( name, (const WeightingConfig&)config);
	}
	else
	{
		WeightingConfig config;
		initWeightingConfig( config, config_.ptr());
		addWeightingFunction( name, config);
	}
}

void QueryEval::addWeightingFormula_obj(
	const StringObject& source_,
	const FunctionObject& config_)
{
	boost::python::extract<FunctionVariableConfig> config(config_);
	if (config.check())
	{
		std::string source;
		initString( source, source_.ptr());
		addWeightingFormula( source, (const FunctionVariableConfig&)config);
	}
	else
	{
		FunctionVariableConfig config;
		initFunctionVariableConfig( config, config_.ptr());
		std::string source;
		initString( source, source_.ptr());
		addWeightingFormula( source, config);
	}
}

std::size_t QueryExpression::allocid_obj( const StringObject& str)
{
	std::string value;
	initString( value, str.ptr());
	return allocid( value);
}

void QueryExpression::pushTerm_obj( const String& type_, const StringObject& value_)
{
	StackOp op( StackOp::PushTerm, allocid( type_), allocid_obj( value_));
	m_ops.push_back(op);
	m_size += 1;
}

void PatternMatcher::pushPattern_obj( const StringObject& name_)
{
	StackOp op( StackOp::PushPattern, allocid_obj( name_));
	m_ops.push_back( op);
	m_size += 1;
}

void QueryExpression::attachVariable_obj( const StringObject& name_)
{
	StackOp op( StackOp::AttachVariable, allocid_obj( name_));
	m_ops.push_back(op);
}

std::size_t PatternMatcher::allocid_obj( const StringObject& str)
{
	std::string value;
	initString( value, str.ptr());
	return allocid( value);
}

void PatternMatcher::attachVariable_obj( const StringObject& name_)
{
	StackOp op( StackOp::AttachVariable, allocid_obj( name_));
	m_ops.push_back(op);
}

void PatternMatcher::definePattern_obj( const StringObject& name_, bool visible_)
{
	StackOp op( StackOp::PushPattern, allocid_obj( name_), visible_?1:0);
	m_ops.push_back(op);
}

void PatternMatcher::pushTerm_obj( const String& type_, const StringObject& value_)
{
	StackOp op( StackOp::PushTerm, allocid( type_), allocid_obj( value_));
	m_ops.push_back(op);
	m_size += 1;
}

void Query::addUserName_obj( const StringObject& username_)
{
	std::string value;
	initString( value, username_.ptr());
	addUserName( value);
}

void Query::defineFeature_expr_2( const String& set_, const FunctionObject& expr_)
{
	defineFeature_expr_3( set_, expr_, 1.0);
}

void Query::defineFeature_expr_3( const String& set_, const FunctionObject& expr_, double weight_)
{
	boost::python::extract<QueryExpression> expr(expr_);
	if (expr.check())
	{
		defineFeature( set_, (const QueryExpression&)expr, weight_);
	}
	else
	{
		QueryExpression expr;
		initQueryExpression( expr, expr_.ptr());
		defineFeature( set_, expr, weight_);
	}
}

void Query::defineTermStatistics_obj_struct( const String& type_, const StringObject& value_, const FunctionObject& stats_)
{
	std::string value;
	initString( value, value_.ptr());
 
	boost::python::extract<TermStatistics> stats(stats_);
	if (stats.check())
	{
		defineTermStatistics( type_, value, (const TermStatistics&)stats);
	}
	else
	{
		TermStatistics stats;
		initTermStatistics( stats, stats_.ptr());
		defineTermStatistics( type_, value, stats);
	}
}

void Query::defineGlobalStatistics_struct( const FunctionObject& expr_)
{
	boost::python::extract<GlobalStatistics> expr(expr_);
	if (expr.check())
	{
		defineGlobalStatistics( (const GlobalStatistics&)expr);
	}
	else
	{
		GlobalStatistics stats;
		initGlobalStatistics( stats, expr_.ptr());
		defineGlobalStatistics( stats);
	}
}

void Query::addDocumentEvaluationSet_struct( const FunctionObject& docnolist_)
{
	boost::python::extract<std::vector<int> > expr(docnolist_);
	if (expr.check())
	{
		addDocumentEvaluationSet( (const std::vector<int>&)expr);
	}
	else
	{
		std::vector<int> docnolist;
		initIntVector( docnolist, docnolist_.ptr());
		addDocumentEvaluationSet( docnolist);
	}
}

void Query::setWeightingVariables_obj(
	const FunctionObject& config_)
{
	boost::python::extract<FunctionVariableConfig> config(config_);
	if (config.check())
	{
		setWeightingVariables( (const FunctionVariableConfig&)config);
	}
	else
	{
		FunctionVariableConfig config;
		initFunctionVariableConfig( config, config_.ptr());
		setWeightingVariables( config);
	}
}

VecRankVector VectorStorageSearcher::findSimilar_obj( const FunctionObject& vec_, unsigned int maxNofResults) const
{
	boost::python::extract<std::vector<double> > expr(vec_);
	if (expr.check())
	{
		return findSimilar( (const std::vector<double>&)expr, maxNofResults);
	}
	else
	{
		std::vector<double> vec;
		initFloatVector( vec, vec_.ptr());
		return findSimilar( vec, maxNofResults);
	}
}

void VectorStorageBuilder::addFeature_obj( const StringObject& name_, const FunctionObject& vec_)
{
	std::string name;
	initString( name, name_.ptr());

	boost::python::extract<std::vector<double> > expr(vec_);
	if (expr.check())
	{
		addFeature( name, (const std::vector<double>&)expr);
	}
	else
	{
		std::vector<double> vec;
		initFloatVector( vec, vec_.ptr());
		addFeature( name, vec);
	}
}

StorageClient Context::createStorageClient_0()
{
	return createStorageClient();
}

StorageClient Context::createStorageClient_obj( const StringObject& config_)
{
	std::string value;
	initString( value, config_.ptr());
	return createStorageClient( value);
}

VectorStorageClient Context::createVectorStorageClient_0()
{
	return createVectorStorageClient();
}

VectorStorageClient Context::createVectorStorageClient_obj( const StringObject& config_)
{
	std::string value;
	initString( value, config_.ptr());
	return createVectorStorageClient( value);
}

VectorStorageBuilder Context::createVectorStorageBuilder_obj( const StringObject& config_)
{
	std::string value;
	initString( value, config_.ptr());
	return createVectorStorageBuilder( value);
}

WString DocumentFrequencyChange::ucvalue() const
{
	return convert_uft8string_to_wstring( m_value);
}

void Context::addModulePath_obj( const StringObject& paths_)
{
	std::string value;
	initString( value, paths_.ptr());
	addModulePath( value);
}

void Context::addResourcePath_obj( const StringObject& paths_)
{
	std::string value;
	initString( value, paths_.ptr());
	addResourcePath( value);
}

void Context::createStorage_obj( const StringObject& config_)
{
	std::string value;
	initString( value, config_.ptr());
	return createStorage( value);
}

void Context::createVectorStorage_obj( const StringObject& config_)
{
	std::string value;
	initString( value, config_.ptr());
	return createVectorStorage( value);
}

void Context::destroyStorage_obj( const StringObject& config_)
{
	std::string value;
	initString( value, config_.ptr());
	return destroyStorage( value);
}

DocumentClass Context::detectDocumentClass_obj( const StringObject& content)
{
	std::string value;
	initString( value, content.ptr());
	return detectDocumentClass( value);
}

DocumentAnalyzer Context::createDocumentAnalyzer_obj( const StringObject& segmentername_)
{
	std::string value;
	initString( value, segmentername_.ptr());
	return createDocumentAnalyzer( value);
}

DocumentAnalyzer Context::createDocumentAnalyzer_0()
{
	return createDocumentAnalyzer();
}

StatisticsMessage StatisticsProcessor::decode_datablob( const DataBlob& datablob) const
{
	std::string stringblob;
	initDataBlob( stringblob, datablob.ptr());
	return decode( stringblob);
}


