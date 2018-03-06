/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/analyzer.hpp"
#include "strus/documentAnalyzerInterface.hpp"
#include "strus/documentAnalyzerContextInterface.hpp"
#include "strus/queryAnalyzerInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/string_conv.hpp"
#include "papuga/serialization.h"
#include "papuga/errors.hpp"
#include "papuga/valueVariant.hpp"
#include "patternMatcherLoader.hpp"
#include "private/internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "callResultUtils.hpp"
#include "structDefs.hpp"

using namespace strus;
using namespace strus::bindings;

DocumentAnalyzerImpl::DocumentAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const ValueVariant& doctype, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl()
	,m_textproc(textproc_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();

	SegmenterDef segdef( doctype);
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
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	m_analyzer_impl.resetOwnership( objBuilder->createDocumentAnalyzer( segmenter, segmenteropt), "DocumentAnalyzer");
	if (!m_analyzer_impl.get())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
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
		if (!textproc) throw strus::runtime_error( "%s", errorhnd->fetchError());

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

void DocumentAnalyzerImpl::addPatternLexem(
		const std::string& type,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInterface>();

	analyzer->addPatternLexem(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
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
	if (!textproc) throw strus::runtime_error( "%s", errorhnd->fetchError());

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

static analyzer::Document* analyzeDoc( const DocumentAnalyzerInterface* THIS, const std::string& content, const analyzer::DocumentClass& dclass, ErrorBufferInterface* errorhnd)
{
	Reference<analyzer::Document> doc( new analyzer::Document( THIS->analyze( content, dclass)));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return doc.release();
}

analyzer::DocumentClass DocumentAnalyzerImpl::getDocumentClass( const std::string& content, const ValueVariant& dclass) const
{
	if (papuga_ValueVariant_defined( &dclass))
	{
		return Deserializer::getDocumentClass( dclass);
	}
	else
	{
		analyzer::DocumentClass detected_dclass;

		const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
		const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (!textproc) throw runtime_error( "%s", errorhnd->fetchError());

		enum {MaxHdrSize = 8092};
		std::size_t hdrsize = content.size() > MaxHdrSize ? MaxHdrSize : content.size();
		if (!textproc->detectDocumentClass( detected_dclass, content.c_str(), hdrsize, MaxHdrSize < content.size()))
		{
			if (errorhnd->hasError())
			{
				throw strus::runtime_error( "%s", errorhnd->fetchError());
			}
			else
			{
				throw strus::runtime_error( _TXT("could not detect document class of document to analyze"));
			}
		}
		return detected_dclass;
	}
}

analyzer::Document* DocumentAnalyzerImpl::analyzeSingle( const std::string& content, const ValueVariant& dclass) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const DocumentAnalyzerInterface* THIS = m_analyzer_impl.getObject<const DocumentAnalyzerInterface>();
	analyzer::DocumentClass documentClass = getDocumentClass( content, dclass);
	return analyzeDoc( THIS, content, documentClass, errorhnd);
}

class DocumentAnalyzeIterator
{
public:
	DocumentAnalyzeIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& analyzer_, const ObjectRef& errorhnd_, const std::string& content, const analyzer::DocumentClass& dclass)
		:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_analyzer_impl(analyzer_),m_errorhnd_impl(errorhnd_),m_analyzercontext_impl()
	{
		const DocumentAnalyzerInterface* analyzer = m_analyzer_impl.getObject<const DocumentAnalyzerInterface>();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		DocumentAnalyzerContextInterface* analyzerContext;
		m_analyzercontext_impl.resetOwnership( analyzerContext = analyzer->createContext( dclass), "DocumentAnalyzerContext");
		if (!analyzerContext) throw strus::runtime_error( "%s", errorhnd->fetchError());
		analyzerContext->putInput( content.c_str(), content.size(), true);
	}
	virtual ~DocumentAnalyzeIterator(){}

	bool getNext( papuga_CallResult* result)
	{
		try
		{
			DocumentAnalyzerContextInterface* analyzerContext = m_analyzercontext_impl.getObject<DocumentAnalyzerContextInterface>();
			Reference<analyzer::Document> doc( new analyzer::Document());
			if (!analyzerContext->analyzeNext( *doc))
			{
				ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
				if (errorhnd->hasError())
				{
					throw strus::runtime_error( "%s", errorhnd->fetchError());
				}
				return false;
			}
			initCallResultStructureOwnership( result, doc.release());
			return true;
		}
		catch (const std::bad_alloc& err)
		{
			papuga_CallResult_reportError( result, _TXT("memory allocation error in document analyze multipart iterator get next"));
			return false;
		}
		catch (const std::runtime_error& err)
		{
			papuga_CallResult_reportError( result, _TXT("error in document analyze multipart iterator get next: %s"), err.what());
			return false;
		}
	}

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_analyzer_impl;
	ObjectRef m_errorhnd_impl;
	ObjectRef m_analyzercontext_impl;
};

static bool DocumentAnalyzeGetNext( void* self, papuga_CallResult* result)
{
	return ((DocumentAnalyzeIterator*)self)->getNext( result);
}

static void DocumentAnalyzeDeleter( void* obj)
{
	delete (DocumentAnalyzeIterator*)obj;
}

Iterator DocumentAnalyzerImpl::analyzeMultiPart(
		const std::string& content,
		const ValueVariant& dclass) const
{
	analyzer::DocumentClass documentClass = getDocumentClass( content, dclass);
	Iterator rt( new DocumentAnalyzeIterator( m_trace_impl, m_objbuilder_impl, m_analyzer_impl, m_errorhnd_impl, content, documentClass), &DocumentAnalyzeDeleter, &DocumentAnalyzeGetNext);
	rt.release();
	return rt;
}

QueryAnalyzerImpl::QueryAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl()
	,m_queryAnalyzerStruct()
{
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	m_analyzer_impl.resetOwnership( objBuilder->createQueryAnalyzer(), "QueryAnalyzer");
	if (!m_analyzer_impl.get())
	{
		ErrorBufferInterface* ehnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", ehnd->fetchError());
	}
}

void QueryAnalyzerImpl::addElement(
		const std::string& featureType,
		const std::string& fieldType,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->addElement( featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzerImpl::addElementFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);

	THIS->addElementFromPatternMatch( type, patternTypeName, funcdef.normalizers);
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
	if (name.empty() || strus::caseInsensitiveEquals( name, "all"))
	{
		return QueryAnalyzerContextInterface::GroupAll;
	}
	else if (strus::caseInsensitiveEquals( name, "position"))
	{
		return QueryAnalyzerContextInterface::GroupByPosition;
	}
	else if (strus::caseInsensitiveEquals( name, "every"))
	{
		throw strus::runtime_error(_TXT("'%s' does not make sense as implicit grouping operation"), name.c_str());
	}
	else
	{
		throw strus::runtime_error(_TXT("unknown group by operation '%s'"), name.c_str());
	}
}

void QueryAnalyzerImpl::defineImplicitGroupBy( const std::string& fieldtype, const std::string& groupBy_, const std::string& opname, int range, unsigned int cardinality)
{
	QueryAnalyzerContextInterface::GroupBy groupBy = getImplicitGroupBy( groupBy_);
	m_queryAnalyzerStruct.autoGroupBy( fieldtype, opname, range, cardinality, groupBy, false/*groupSingle*/);
}

TermExpression* QueryAnalyzerImpl::analyzeTermExpression( const ValueVariant& expression)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* analyzer = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	Reference<TermExpression> termexpr( new TermExpression( &m_queryAnalyzerStruct, analyzer, errorhnd));
	if (!termexpr.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());

	QueryAnalyzerTermExpressionBuilder exprbuilder( termexpr.get());
	Deserializer::buildExpression( exprbuilder, expression, errorhnd, true);
	termexpr->analyze();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return termexpr.release();
}

MetaDataExpression* QueryAnalyzerImpl::analyzeMetaDataExpression( const ValueVariant& expression)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* analyzer = m_analyzer_impl.getObject<QueryAnalyzerInterface>();
	Reference<MetaDataExpression> metaexpr( new MetaDataExpression( analyzer, errorhnd));
	if (!metaexpr.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());

	Deserializer::buildMetaDataRestriction( metaexpr.get(), expression, errorhnd);
	metaexpr->analyze();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return metaexpr.release();
}

