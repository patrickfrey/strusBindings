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
#include "bindings/serializer.hpp"
#include "bindings/serialization.hpp"

using namespace strus;
using namespace strus::bindings;

DocumentAnalyzerImpl::DocumentAnalyzerImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd, const std::string& segmentername, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl()
	,m_textproc(textproc_)
{
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = m_textproc;
	const SegmenterInterface* segmenter = textproc->getSegmenterByName( segmentername);
	if (!segmenter)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to get document document segmenter by name: %s"), errorhnd->fetchError());
	}
	m_analyzer_impl.resetOwnership( objBuilder->createDocumentAnalyzer( segmenter));
	if (!m_analyzer_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create document analyzer: %s"), errorhnd->fetchError());
	}
}

DocumentAnalyzerImpl::DocumentAnalyzerImpl( const DocumentAnalyzerImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
	,m_textproc(o.m_textproc)
{}

struct FeatureFuncDef
{
	std::vector<Reference<NormalizerFunctionInstanceInterface> > normalizers_ref;
	std::vector<NormalizerFunctionInstanceInterface*> normalizers;
	Reference<TokenizerFunctionInstanceInterface> tokenizer;

	FeatureFuncDef( const HostObjectReference& objbuilder_impl,
			const ValueVariant& tokenizer_,
			const ValueVariant& normalizers_,
			ErrorBufferInterface* errorhnd)
	{
		const AnalyzerObjectBuilderInterface* objBuilder = objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
		const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor object: %s"), errorhnd->fetchError());

		if (tokenizer_.defined())
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
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());

	Reference<AggregatorFunctionInstanceInterface> functioninst( Deserializer::getAggregator( function));
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->defineAggregatedMetaData(
		fieldname, functioninst.get());
	functioninst.release();
}

void DocumentAnalyzer::defineAttribute(
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

void DocumentAnalyzer::addSearchIndexFeatureFromPatternMatch(
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
		const PatternMatcher& patterns)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	PatternMatcherPostProc pt = loadPatternMatcherPostProc( textproc, patternMatcherModule, patterns, errorhnd);
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
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	PatternMatcherPostProc pt = loadPatterMatcherPostProcFromFile( textproc, patternMatcherModule, serializedPatternFile, errorhnd);
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

static CallResult analyzeDoc( DocumentAnalyzerInterface* THIS, const std::string& content, const analyzer::DocumentClass& dclass)
{
	CallResult rt;
	analyzer::Document* doc;
	rt.object.resetOwnership( doc = new analyzer::Document( THIS->analyze( content, dclass)));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT( "failed to analyze document (%s)"), errorhnd->fetchError());
	}
	Serializer::serialize( rt.serialization, *doc);
	return rt;
}

Serialization DocumentAnalyzerImpl::analyze( const std::string& content)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();
	analyzer::DocumentClass dclass;

	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
	if (!textproc->detectDocumentClass( dclass, content.c_str(), content.size()))
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
	return analyzeDoc( THIS, content, dclass);
}

Serialization DocumentAnalyzerImpl::analyze( const std::string& content, const DocumentClass& dclass)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();
	analyzer::DocumentClass documentClass( dclass.mimeType(), dclass.encoding(), dclass.scheme());
	return analyzeDoc( THIS, content, dclass);
}

QueryAnalyzerImpl::QueryAnalyzerImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(ReferenceDeleter<QueryAnalyzerInterface>::function)
{
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	m_analyzer_impl.reset( objBuilder->createQueryAnalyzer());
	if (!m_analyzer_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create query analyzer: %s"), errorhnd->fetchError());
	}
}

QueryAnalyzerImpl::QueryAnalyzerImpl( const QueryAnalyzerImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
{}

QueryAnalyzeContextImpl QueryAnalyzerImpl::createContext() const
{
	return QueryAnalyzeContext( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_analyzer_impl);
}

void QueryAnalyzerImpl::addSearchIndexElement(
		const std::string& featureType,
		const std::string& fieldType,
		const Tokenizer& tokenizer,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
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
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, normalizers, errorhnd);

	THIS->addSearchIndexElementFromPatternMatch( type, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzerImpl::addPatternLexem(
		const std::string& featureType,
		const std::string& fieldType,
		const Tokenizer& tokenizer,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->addPatternLexem( featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzerImpl::definePatternMatcherPostProc(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const PatternMatcher& patterns)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	Reference<PatternMatcherInstanceInterface> matcherInstance;
	Reference<PatternTermFeederInstanceInterface> feederInstance;
	PatternMatchLoader::loadPatterMatcher( matcherInstance, feederInstance, textproc, patternMatcherModule, patterns, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, matcherInstance.get(), feederInstance.get());
	matcherInstance.release();
	feederInstance.release();
}

void QueryAnalyzerImpl::definePatternMatcherPostProcFromFile(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const std::string& serializedPatternFile)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	Reference<PatternMatcherInstanceInterface> matcherInstance;
	Reference<PatternTermFeederInstanceInterface> feederInstance;
	PatternMatchLoader::loadPatterMatcherFromFile(
		matcherInstance, feederInstance, 
		textproc, patternMatcherModule, serializedPatternFile, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, matcherInstance.get(), feederInstance.get());
	matcherInstance.release();
	feederInstance.release();
}

ValueVariant QueryAnalyzerImpl::analyzeField(
		const std::string& fieldType,
		const std::string& fieldContent)
{
	ValueVariant rt;
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
	std::auto_ptr<QueryAnalyzerContextInterface> anactx( THIS->createContext());
	if (!anactx.get()) throw strus::runtime_error( _TXT("failed to create query analyzer context: %s"), errorhnd->fetchError());
	anactx->putField( 1/*field no*/, fieldType, fieldContent);
	analyzer::Query qry = anactx->analyze();
	if (qry.empty() && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error in analyze query field: %s"), errorhnd->fetchError());
	}
	std::vector<analyzer::Query::Element>::const_iterator
		ei = qry.elements().begin(), ee = qry.elements().end();
	for (; ei != ee; ++ei)
	{
		switch (ei->type())
		{
			case analyzer::Query::Element::MetaData:
			{
				const analyzer::MetaData& md = qry.metadata( ei->idx());
				rt.push_back( Term( md.name(), md.value().tostring().c_str(), ei->position(), 1/*length*/));
				break;
			}
			case analyzer::Query::Element::SearchIndexTerm:
			{
				const analyzer::Term& term = qry.searchIndexTerm( ei->idx());
				rt.push_back( Term( term.type(), term.value(), term.pos(), term.len()));
				break;
			}
		}
	}
	std::sort( rt.begin(), rt.end());
	return rt;
}


