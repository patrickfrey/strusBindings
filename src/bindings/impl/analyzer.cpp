/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/analyzer.hpp"
#include "impl/value/analyzerIntrospection.hpp"
#include "impl/value/featureFuncDef.hpp"
#include "strus/documentAnalyzerInstanceInterface.hpp"
#include "strus/documentAnalyzerContextInterface.hpp"
#include "strus/queryAnalyzerInstanceInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/local_ptr.hpp"
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
		if (errorhnd->hasError())
		{
			throw std::runtime_error( errorhnd->fetchError());
		}
		else
		{
			throw std::runtime_error( _TXT("no segmenter or document class defined"));
		}
	}
	m_analyzer_impl.resetOwnership( objBuilder->createDocumentAnalyzer( segmenter, segmenteropt), "DocumentAnalyzer");
	if (!m_analyzer_impl.get())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

void DocumentAnalyzerImpl::addSearchIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers,
	const ValueVariant& priorityval,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	int priority = Deserializer::getInt( priorityval, 0);
	analyzer->addSearchIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		priority, Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::addForwardIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers,
	const ValueVariant& priorityval,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	int priority = Deserializer::getInt( priorityval, 0);
	analyzer->addForwardIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		priority, Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::addPatternLexem(
		const std::string& type,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers,
		const ValueVariant& priorityval)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	int priority = Deserializer::getInt( priorityval, 0);
	analyzer->addPatternLexem(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers, priority);
	funcdef.release();
}

void DocumentAnalyzerImpl::defineMetaData(
	const std::string& fieldname,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

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
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

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
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

	analyzer->defineAttribute(
		attribname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzerImpl::addSearchIndexFeatureFromPatternMatch(
	const std::string& type,
	const std::string& patternTypeName,
	const ValueVariant& normalizers,
	const ValueVariant& priorityval,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, ValueVariant(), normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	int priority = Deserializer::getInt( priorityval, 0);
	analyzer->addSearchIndexFeatureFromPatternMatch(
		type, patternTypeName, funcdef.normalizers,
		priority, Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::addForwardIndexFeatureFromPatternMatch(
	const std::string& type,
	const std::string& patternTypeName,
	const ValueVariant& normalizers,
	const ValueVariant& priorityval,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, ValueVariant(), normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	int priority = Deserializer::getInt( priorityval, 0);
	analyzer->addForwardIndexFeatureFromPatternMatch(
		type, patternTypeName, funcdef.normalizers,
		priority, Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::defineMetaDataFromPatternMatch(
	const std::string& fieldname,
	const std::string& patternTypeName,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, ValueVariant(), normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

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
	FeatureFuncDef funcdef( m_textproc, ValueVariant(), normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

	analyzer->defineAttributeFromPatternMatch(
		attribname, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzerImpl::defineTokenPatternMatcher(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const ValueVariant& lexems,
		const ValueVariant& patterns)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	PatternMatcherPostProc pt = loadPatternMatcherPostProc( textproc, patternMatcherModule, lexems, patterns, errorhnd);
	THIS->defineTokenPatternMatcher( patternTypeName, pt.matcher.get(), pt.feeder.get());
	pt.release();
}

void DocumentAnalyzerImpl::defineSubDocument(
	const std::string& subDocumentTypeName,
	const std::string& selectexpr)
{
	DocumentAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	THIS->defineSubDocument( subDocumentTypeName, selectexpr);
}

void DocumentAnalyzerImpl::defineSubContent(
		const std::string& selectexpr,
		const ValueVariant& dclass)
{
	analyzer::DocumentClass documentClass = Deserializer::getDocumentClass( dclass);
	DocumentAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	THIS->defineSubContent( selectexpr, documentClass);
}

static analyzer::Document* analyzeDoc( const DocumentAnalyzerInstanceInterface* THIS, const std::string& content, const analyzer::DocumentClass& dclass, ErrorBufferInterface* errorhnd)
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
	const DocumentAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<const DocumentAnalyzerInstanceInterface>();
	analyzer::DocumentClass documentClass = getDocumentClass( content, dclass);
	return analyzeDoc( THIS, content, documentClass, errorhnd);
}

class DocumentAnalyzeIterator
{
public:
	DocumentAnalyzeIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& analyzer_, const ObjectRef& errorhnd_, const std::string& content, const analyzer::DocumentClass& dclass)
		:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_analyzer_impl(analyzer_),m_errorhnd_impl(errorhnd_),m_analyzercontext_impl()
	{
		const DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<const DocumentAnalyzerInstanceInterface>();
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

Struct DocumentAnalyzerImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new DocumentAnalyzerIntrospection( errorhnd, analyzer));
	ictx->getPathContent( rt.serialization, path);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

QueryAnalyzerImpl::QueryAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl()
	,m_queryAnalyzerStruct()
	,m_textproc(textproc_)
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
	const ValueVariant& normalizers,
	const ValueVariant& priorityval)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	int priority = Deserializer::getInt( priorityval, 0);
	THIS->addElement( featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers, priority);
	funcdef.release();
}

void QueryAnalyzerImpl::addElementFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const ValueVariant& normalizers,
		const ValueVariant& priorityval)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
	FeatureFuncDef funcdef( m_textproc, ValueVariant(), normalizers, errorhnd);
	int priority = Deserializer::getInt( priorityval, 0);
	THIS->addElementFromPatternMatch( type, patternTypeName, funcdef.normalizers, priority);
	funcdef.release();
}

void QueryAnalyzerImpl::addPatternLexem(
		const std::string& featureType,
		const std::string& fieldType,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers,
		const ValueVariant& priorityval)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	int priority = Deserializer::getInt( priorityval, 0);
	THIS->addPatternLexem( featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers, priority);
	funcdef.release();
}

void QueryAnalyzerImpl::defineTokenPatternMatcher(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const ValueVariant& lexems,
		const ValueVariant& patterns)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();

	PatternMatcherPostProc pt = loadPatternMatcherPostProc( textproc, patternMatcherModule, lexems, patterns, errorhnd);
	THIS->defineTokenPatternMatcher( patternTypeName, pt.matcher.get(), pt.feeder.get());
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
	else if (strus::caseInsensitiveEquals( name, "unique"))
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

TermExpression* QueryAnalyzerImpl::analyzeTermExpression_( const ValueVariant& expression, bool unique) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
	Reference<TermExpression> termexpr( new TermExpression( &m_queryAnalyzerStruct, analyzer, unique, errorhnd));
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

TermExpression* QueryAnalyzerImpl::analyzeTermExpression( const ValueVariant& expression) const
{
	return analyzeTermExpression_( expression, false/*unique*/);
}

TermExpression* QueryAnalyzerImpl::analyzeSingleTermExpression( const ValueVariant& expression) const
{
	return analyzeTermExpression_( expression, true/*unique*/);
}

MetaDataExpression* QueryAnalyzerImpl::analyzeMetaDataExpression( const ValueVariant& expression) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
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

struct SentenceAnalyzerPrivateImpl
{
	std::string fieldType;
	ObjectRef analyzer_impl;
	ObjectRef lexer_impl;
	FeatureFuncDef featureFuncDef;

	SentenceAnalyzerPrivateImpl( const std::string& fieldType_, const ObjectRef& analyzer_impl_, const ObjectRef& lexer_impl_, const FeatureFuncDef& featureFuncDef_)
		:fieldType(fieldType_),analyzer_impl(analyzer_impl_),lexer_impl(lexer_impl_),featureFuncDef(featureFuncDef_){}
	SentenceAnalyzerPrivateImpl( const SentenceAnalyzerPrivateImpl& o)
		:fieldType(o.fieldType),analyzer_impl(o.analyzer_impl),lexer_impl(o.lexer_impl),featureFuncDef(o.featureFuncDef){}
};

typedef std::vector<SentenceAnalyzerPrivateImpl> SentenceAnalyzerArrayPrivateImpl;

void QueryAnalyzerImpl::addSentenceType(
		const std::string& fieldType,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers,
		SentenceAnalyzerImpl* analyzer)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	ObjectRef sentence_analyzer_map_impl_copy = m_sentence_analyzer_map_impl;
	ObjectRef sentence_analyzer_map_impl_new;
	SentenceAnalyzerArrayPrivateImpl* sar = sentence_analyzer_map_impl_copy.getObject<SentenceAnalyzerArrayPrivateImpl>();
	if (sar)
	{
		sentence_analyzer_map_impl_new.resetOwnership( sar = new SentenceAnalyzerArrayPrivateImpl( *sar), "array of sentence analyzers");
	}
	else
	{
		if (sentence_analyzer_map_impl_copy.get()) throw strus::runtime_error(_TXT( "logic error: type mismatch accessing '%s'"), "sentence analyzer");
		sentence_analyzer_map_impl_new.resetOwnership( sar = new SentenceAnalyzerArrayPrivateImpl(), "array of sentence analyzers");
	}
	sar->push_back( SentenceAnalyzerPrivateImpl(
				fieldType, analyzer->m_analyzer_impl, analyzer->m_lexer_impl,
				FeatureFuncDef( m_textproc, tokenizer, normalizers, errorhnd)));
	m_sentence_analyzer_map_impl = sentence_analyzer_map_impl_new;
}

static std::string normalize_field(
	const char* tok, std::size_t toksize, 
	std::vector<NormalizerFunctionInstanceInterface*>::const_iterator ci,
	const std::vector<NormalizerFunctionInstanceInterface*>::const_iterator& ce)
{
	if (ci == ce) return std::string( tok, toksize);

	std::string rt;
	std::string origstr;
	for (; ci != ce; ++ci)
	{
		rt = (*ci)->normalize( tok, toksize);
		if (ci + 1 != ce)
		{
			if (!rt.empty() && rt[0] == '\0')
			{
				std::string reslist;
				char const* vi = rt.c_str();
				char const* ve = vi + rt.size();
				for (++vi; vi < ve; vi = std::strchr( vi, '\0')+1)
				{
					std::string partres = normalize_field( vi, std::strlen(vi), ci+1, ce);
					if (!partres.empty() && partres[0] == '\0')
					{
						reslist.append( partres);
					}
					else if (reslist.empty())
					{
						reslist.push_back( '\0');
						reslist.append( partres);
					}
					else
					{
						reslist.append( partres);
					}
				}
				return reslist;
			}
			else
			{
				origstr.swap( rt);
				tok = origstr.c_str();
				toksize = origstr.size();
			}
		}
	}
	return rt;
}

static void appendNormTokenToSentenceString( std::string& result, const std::string& normtok, char separator)
{
	if (!normtok.empty() && normtok[0] == '\0')
	{
		char const* vi = normtok.c_str();
		char const* ve = vi + normtok.size();
		for (++vi; vi < ve; vi = std::strchr( vi, '\0')+1)
		{
			if (!result.empty()) result.push_back( separator);
			result.append( vi);
		}
	}
	else
	{
		if (!result.empty()) result.push_back( separator);
		result.append( normtok);
	}
	
}

Struct QueryAnalyzerImpl::analyzeSentence(
		const std::string& fieldType,
		const std::string& fieldContent,
		int maxNofResults,
		double minWeight)
{
	ObjectRef sentence_analyzer_map_impl_copy = m_sentence_analyzer_map_impl;
	SentenceAnalyzerArrayPrivateImpl* sar = sentence_analyzer_map_impl_copy.getObject<SentenceAnalyzerArrayPrivateImpl>();
	if (!sar) throw strus::runtime_error(_TXT( "unknown '%s' (none defined)"), "sentence analyzer");
	SentenceAnalyzerArrayPrivateImpl::const_iterator si = sar->begin(), se = sar->end();
	for (; si != se && !strus::caseInsensitiveEquals( si->fieldType, fieldType); ++si){}
	if (si == se) throw strus::runtime_error(_TXT( "unknown '%s' (not found)"), "sentence analyzer");

	const SentenceAnalyzerInstanceInterface* analyzer = si->analyzer_impl.getObject<SentenceAnalyzerInstanceInterface>();
	const SentenceLexerInstanceInterface* lexer = si->lexer_impl.getObject<SentenceLexerInstanceInterface>();

	std::string source;
	const TokenizerFunctionInstanceInterface* tokenizer = si->featureFuncDef.tokenizer.get();
	if (tokenizer)
	{
		std::vector<analyzer::Token> tokens = tokenizer->tokenize( fieldContent.c_str(), fieldContent.size());
		std::vector<analyzer::Token>::const_iterator ti = tokens.begin(), te = tokens.end();
		for (; ti != te; ++ti)
		{
			std::string normtok
				= normalize_field(
					fieldContent.c_str() + ti->origpos().ofs(), ti->origsize(),
					si->featureFuncDef.normalizers.begin(), si->featureFuncDef.normalizers.end());
			appendNormTokenToSentenceString( source, normtok, ' ');
		}
	}
	else
	{
		std::string normtok
				= normalize_field(
					fieldContent.c_str(), fieldContent.size(),
					si->featureFuncDef.normalizers.begin(), si->featureFuncDef.normalizers.end());
		appendNormTokenToSentenceString( source, normtok, ' ');
	}
	std::vector<SentenceGuess> res = analyzer->analyzeSentence( lexer, source, maxNofResults, minWeight);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError()) throw strus::runtime_error( "%s", errorhnd->fetchError());

	Struct rt;
	strus::bindings::Serializer::serialize( &rt.serialization, res, true/*deep*/);
	rt.release();
	return rt;
	
}

Struct QueryAnalyzerImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new QueryAnalyzerIntrospection( errorhnd, analyzer));
	ictx->getPathContent( rt.serialization, path);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

