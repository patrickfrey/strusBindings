/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/analyzer.hpp"
#include "strus/documentAnalyzerInterface.hpp"
#include "strus/queryAnalyzerInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "papuga/serialization.hpp"
#include "papuga/errors.hpp"
#include "papuga/valueVariant.hpp"
#include "patternMatcherLoader.hpp"
#include "internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "utils.hpp"
#include "callResultUtils.hpp"
#include "structDefs.hpp"
#include "impl/analyzedQuery.hpp"

using namespace strus;
using namespace strus::bindings;

static SegmenterDef parseSegmenterDef( const ValueVariant& ctx)
{
	if (papuga_ValueVariant_isstring( &ctx))
	{
		papuga_ErrorCode err = papuga_Ok;
		SegmenterDef segdef( papuga::ValueVariant_tostring( ctx, err));
		if (err != papuga_Ok) throw papuga::error_exception( err, "segmenter definition");
		return segdef;
	}
	else if (ctx.valuetype == papuga_Serialized)
	{
		papuga::Serialization::const_iterator si = papuga::Serialization::begin( ctx.value.serialization);
		return SegmenterDef( si, papuga::Serialization::end( ctx.value.serialization));
	}
	else
	{
		throw strus::runtime_error(_TXT("expected string or document class structure for document segmenter definition"));
	}
}

DocumentAnalyzerImpl::DocumentAnalyzerImpl( const ObjectRef& objbuilder, const ObjectRef& trace, const ObjectRef& errorhnd_, const ValueVariant& doctype, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl()
	,m_textproc(textproc_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();

	SegmenterDef segdef( parseSegmenterDef( doctype));
	const SegmenterInterface* segmenter = 0;
	analyzer::SegmenterOptions segmenteropt;

	if (!segdef.segmenter.empty())
	{
		segmenter = m_textproc->getSegmenterByName( segdef.segmenter);
	}
	else
	{
		if (!segdef.mimetype.empty())
		{
			segmenter = m_textproc->getSegmenterByMimeType( segdef.mimetype);
		}
		if (!segdef.scheme.empty())
		{
			segmenteropt = m_textproc->getSegmenterOptions( segdef.scheme);
		}
	}
	if (!segmenter)
	{
		throw strus::runtime_error( _TXT("failed to get document document segmenter: %s"), errorhnd->fetchError());
	}
	m_analyzer_impl.resetOwnership( objBuilder->createDocumentAnalyzer( segmenter, segmenteropt));
	if (!m_analyzer_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create document analyzer: %s"), errorhnd->fetchError());
	}
}

struct FeatureFuncDef
{
	std::vector<Reference<NormalizerFunctionInstanceInterface> > normalizers_ref;
	std::vector<NormalizerFunctionInstanceInterface*> normalizers;
	Reference<TokenizerFunctionInstanceInterface> tokenizer;

	FeatureFuncDef( const ObjectRef& objbuilder_impl,
			const ValueVariant& tokenizer_,
			const ValueVariant& normalizers_,
			ErrorBufferInterface* errorhnd)
	{
		const AnalyzerObjectBuilderInterface* objBuilder = objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
		const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor object: %s"), errorhnd->fetchError());

		if (papuga_ValueVariant_defined( &tokenizer_))
		{
			tokenizer = Deserializer::getTokenizer( tokenizer_, textproc, errorhnd);
		}
		normalizers_ref = Deserializer::getNormalizers( normalizers_, textproc, errorhnd);
		std::vector<Reference<NormalizerFunctionInstanceInterface> >::iterator ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni)
		{
			normalizers.push_back( ni->get());
		}
	}

	void release()
	{
		(void)tokenizer.release();
		std::vector<Reference<NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) (void)ni->release();
	}
};

void DocumentAnalyzerImpl::addSearchIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();
	analyzer->addSearchIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::addForwardIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->addForwardIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::defineMetaData(
	const std::string& fieldname,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->defineMetaData(
		fieldname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzerImpl::defineAggregatedMetaData(
	const std::string& fieldname,
	const ValueVariant& function)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());

	Reference<AggregatorFunctionInstanceInterface> functioninst( Deserializer::getAggregator( function, textproc, errorhnd));
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->defineAggregatedMetaData(
		fieldname, functioninst.get());
	functioninst.release();
}

void DocumentAnalyzerImpl::defineAttribute(
	const std::string& attribname,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->defineAttribute(
		attribname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzerImpl::addSearchIndexFeatureFromPatternMatch(
	const std::string& type,
	const std::string& patternTypeName,
	const ValueVariant& normalizers,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->addSearchIndexFeatureFromPatternMatch(
		type, patternTypeName, funcdef.normalizers,
		Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::addForwardIndexFeatureFromPatternMatch(
	const std::string& type,
	const std::string& patternTypeName,
	const ValueVariant& normalizers,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->addForwardIndexFeatureFromPatternMatch(
		type, patternTypeName, funcdef.normalizers,
		Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::defineMetaDataFromPatternMatch(
	const std::string& fieldname,
	const std::string& patternTypeName,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->defineMetaDataFromPatternMatch(
		fieldname, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzerImpl::defineAttributeFromPatternMatch(
	const std::string& attribname,
	const std::string& patternTypeName,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->defineAttributeFromPatternMatch(
		attribname, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzerImpl::definePatternMatcherPostProc(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const ValueVariant& lexems,
		const ValueVariant& patterns)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	PatternMatcherPostProc pt = loadPatternMatcherPostProc( textproc, patternMatcherModule, lexems, patterns, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, pt.matcher.get(), pt.feeder.get());
	pt.release();
}

void DocumentAnalyzerImpl::definePatternMatcherPostProcFromFile(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const std::string& serializedPatternFile)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	PatternMatcherPostProc pt = loadPatternMatcherPostProcFromFile( textproc, patternMatcherModule, serializedPatternFile, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, pt.matcher.get(), pt.feeder.get());
	pt.release();
}

void DocumentAnalyzerImpl::defineDocument(
	const std::string& subDocumentTypeName,
	const std::string& selectexpr)
{
	DocumentAnalyzerInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();
	THIS->defineSubDocument( subDocumentTypeName, selectexpr);
}

static analyzer::Document* analyzeDoc( DocumentAnalyzerInterface* THIS, const std::string& content, const analyzer::DocumentClass& dclass, ErrorBufferInterface* errorhnd)
{
	Reference<analyzer::Document> doc( new analyzer::Document( THIS->analyze( content, dclass)));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT( "failed to analyze document (%s)"), errorhnd->fetchError());
	}
	return doc.release();
}

analyzer::Document* DocumentAnalyzerImpl::analyze( const std::string& content, const ValueVariant& dclass)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();
	if (papuga_ValueVariant_defined( &dclass))
	{
		return analyzeDoc( THIS, content, Deserializer::getDocumentClass( dclass), errorhnd);
	}
	else
	{
		analyzer::DocumentClass detected_dclass;
	
		const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
		const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		if (!textproc) throw runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
		if (!textproc->detectDocumentClass( detected_dclass, content.c_str(), content.size()))
		{
			if (errorhnd->hasError())
			{
				throw strus::runtime_error( _TXT( "failed to detect document class of document to analyze (%s)"), errorhnd->fetchError());
			}
			else
			{
				throw strus::runtime_error( _TXT( "could not detect document class of document to analyze"));
			}
		}
		return analyzeDoc( THIS, content, detected_dclass, errorhnd);
	}
}

QueryAnalyzerImpl::QueryAnalyzerImpl( const ObjectRef& objbuilder, const ObjectRef& trace, const ObjectRef& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl()
	,m_queryAnalyzerStruct()
{
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	m_analyzer_impl.resetOwnership( objBuilder->createQueryAnalyzer());
	if (!m_analyzer_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create query analyzer: %s"), errorhnd->fetchError());
	}
}

void QueryAnalyzerImpl::addSearchIndexElement(
		const std::string& featureType,
		const std::string& fieldType,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->addSearchIndexElement(
		featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzerImpl::addSearchIndexElementFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);

	THIS->addSearchIndexElementFromPatternMatch( type, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzerImpl::addPatternLexem(
		const std::string& featureType,
		const std::string& fieldType,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->addPatternLexem( featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzerImpl::definePatternMatcherPostProc(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const ValueVariant& lexems,
		const ValueVariant& patterns)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();

	PatternMatcherPostProc pt = loadPatternMatcherPostProc( textproc, patternMatcherModule, lexems, patterns, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, pt.matcher.get(), pt.feeder.get());
	pt.release();
}

void QueryAnalyzerImpl::definePatternMatcherPostProcFromFile(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const std::string& serializedPatternFile)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();

	PatternMatcherPostProc pt = loadPatternMatcherPostProcFromFile( textproc, patternMatcherModule, serializedPatternFile, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, pt.matcher.get(), pt.feeder.get());
	pt.release();
}

static QueryAnalyzerContextInterface::GroupBy getImplicitGroupBy( const std::string& name)
{
	if (name.empty() || utils::caseInsensitiveEquals( name, "all"))
	{
		return QueryAnalyzerContextInterface::GroupAll;
	}
	else if (utils::caseInsensitiveEquals( name, "position"))
	{
		return QueryAnalyzerContextInterface::GroupByPosition;
	}
	else if (utils::caseInsensitiveEquals( name, "every"))
	{
		throw strus::runtime_error(_TXT("'%s' does not make sense as implicit grouping operation"), name.c_str());
	}
	else
	{
		throw strus::runtime_error(_TXT("unknown group by operation '%s'"), name.c_str());
	}
}

void QueryAnalyzerImpl::defineImplicitGroupBy( const std::string& fieldtype, const std::string& opname, int range, unsigned int cardinality, const std::string& groupBy_)
{
	QueryAnalyzerContextInterface::GroupBy groupBy = getImplicitGroupBy( groupBy_);
	m_queryAnalyzerStruct.autoGroupBy( fieldtype, opname, range, cardinality, groupBy, false/*groupSingle*/);
}

AnalyzedQuery* QueryAnalyzerImpl::analyze(
		const ValueVariant& expression)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	std::auto_ptr<QueryAnalyzerContextInterface> anactx( THIS->createContext());
	if (!anactx.get()) throw strus::runtime_error( _TXT("failed to create query analyzer context: %s"), errorhnd->fetchError());

	QueryAnalyzerExpressionBuilder exprbuilder( &m_queryAnalyzerStruct, anactx.get(), errorhnd);
	Deserializer::buildExpression( exprbuilder, expression, errorhnd);

	bool output_labeled = (expression.valuetype == papuga_Serialized && papuga_Serialization_islabeled( expression.value.serialization));
	Reference<AnalyzedQuery> res( new AnalyzedQuery( anactx->analyze(), exprbuilder.operators(), output_labeled));
	return res.release();
}


