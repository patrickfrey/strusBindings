/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "bindingObjectsImpl.hpp"
#include "bindingUtils.hpp"
#include "strus/strus.hpp"
#include "strus/lib/rpc_client.hpp"
#include "strus/lib/rpc_client_socket.hpp"
#include "strus/lib/module.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/lib/error.hpp"
#include "strus/lib/pattern_serialize.hpp"
#include "strus/rpcClientInterface.hpp"
#include "strus/rpcClientMessagingInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "strus/statisticsBuilderInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/scalarFunctionInterface.hpp"
#include "strus/scalarFunctionParserInterface.hpp"
#include "strus/queryAnalyzerContextInterface.hpp"
#include "strus/patternMatcherInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/patternTermFeederInterface.hpp"
#include "strus/patternTermFeederInstanceInterface.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/vectorStorageTransactionInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/utf8.hpp"
#include "strus/base/symbolTable.hpp"
#include "strus/base/fileio.hpp"
#include "internationalization.hpp"
#include "utils.hpp"
#include "traceUtils.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <limits>

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
		const AnalyzerObjectBuilderInterface* objBuilder = objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
		const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor object: %s"), errorhnd->fetchError());

		if (tokenizer_.defined())
		{
			tokenizer = Deserializer::getTokenizer( tokenizer_, textproc, errorhnd);
		}
		normalizers_ref = Deserializer::getNormalizers( normalizers_, textproc, errorhnd);
		std::vector<Reference<NormalizerFunctionInstanceInterface> >::const_iterator ni = normalizers_ref.begin(), ne = normalizers_ref.end();
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

	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->addSearchIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		getFeatureOptions( options));
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

	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->addForwardIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::defineMetaData(
	const std::string& fieldname,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineMetaData(
		fieldname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzer::defineAggregatedMetaData(
	const std::string& fieldname,
	const ValueVariant& function)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());

	Reference<AggregatorFunctionInstanceInterface> functioninst( Deserializer::getAggregator( function));
	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAggregatedMetaData(
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

	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAttribute(
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

	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->addSearchIndexFeatureFromPatternMatch(
		type, patternTypeName, funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::addForwardIndexFeatureFromPatternMatch(
	const std::string& type,
	const std::string& patternTypeName,
	const ValueVariant& normalizers,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);

	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->addForwardIndexFeatureFromPatternMatch(
		type, patternTypeName, funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::defineMetaDataFromPatternMatch(
	const std::string& fieldname,
	const std::string& patternTypeName,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);

	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineMetaDataFromPatternMatch(
		fieldname, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzer::defineAttributeFromPatternMatch(
	const std::string& attribname,
	const std::string& patternTypeName,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_objbuilder_impl, ValueVariant(), normalizers, errorhnd);

	((DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAttributeFromPatternMatch(
		attribname, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

enum {MaxPatternTermNameId=(1<<24)};

class PatternMatchLoader
{
public:
static void loadPatterMatcher(
	Reference<PatternMatcherInstanceInterface>& matcherInstance, 
	Reference<PatternTermFeederInstanceInterface>& feederInstance, 
	const TextProcessorInterface* textproc,
	const std::string& patternMatcherModule,
	const PatternMatcher& patternMatcher,
	ErrorBufferInterface* errorhnd)
{
	const PatternMatcherInterface* matcher = textproc->getPatternMatcher( patternMatcherModule);
	if (!matcher) throw strus::runtime_error(_TXT("failed to load matcher module: %s"), errorhnd->fetchError());
	const PatternTermFeederInterface* feeder = textproc->getPatternTermFeeder();
	matcherInstance.reset( matcher->createInstance());
	feederInstance.reset( feeder->createInstance());
	if (!feederInstance.get()) throw strus::runtime_error(_TXT("failed to create pattern feeder: %s"), errorhnd->fetchError());
	if (!matcherInstance.get()) throw strus::runtime_error(_TXT("failed to create pattern matcher: %s"), errorhnd->fetchError());

	SymbolTable termsymtab;
	SymbolTable termtypetab;

	std::vector<PatternMatcher::StackOp>::const_iterator oi = patternMatcher.ops().begin(), oe = patternMatcher.ops().end();
	for (; oi != oe; ++oi)
	{
		switch (oi->type)
		{
			case PatternMatcher::StackOp::PushTerm:
			{
				const char* type_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Term_type];
				const char* value_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Term_value];
				if (!value_[0] && type_[0] == '~')
				{
					matcherInstance->pushPattern( type_ +1);
				}
				else
				{
					uint32_t termtypeid = termtypetab.getOrCreate( type_);
					if (termtypetab.isNew())
					{
						if (termtypeid >= MaxPatternTermNameId) throw strus::runtime_error(_TXT("too many lexems defined in pattern match program"));
						feederInstance->defineLexem( termtypeid, type_);
					}
					if (!value_[0])
					{
						matcherInstance->pushTerm( termtypeid);
					}
					else
					{
						uint32_t termsymid = termsymtab.getOrCreate(
							termSymbolKey( termtypeid, value_)
							+ MaxPatternTermNameId);
						if (termsymtab.isNew())
						{
							feederInstance->defineSymbol( termsymid, termtypeid, value_);
						}
						matcherInstance->pushTerm( termsymid);
					}
				}
				break;
			}
			case PatternMatcher::StackOp::PushPattern:
			{
				const char* name_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Pattern_name];
				matcherInstance->pushPattern( name_);
				break;
			}
			case PatternMatcher::StackOp::PushExpression:
			{
				const char* opname_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Expression_opname];
				unsigned int argc_ = (unsigned int)oi->arg[ PatternMatcher::StackOp::Expression_argc];
				int range_ = (int)oi->arg[ PatternMatcher::StackOp::Expression_range];
				unsigned int cardinality_ = (unsigned int)oi->arg[ PatternMatcher::StackOp::Expression_cardinality];
				PatternMatcherInstanceInterface::JoinOperation joinop = patternMatcherJoinOp( opname_);
				matcherInstance->pushExpression( joinop, argc_, range_, cardinality_);
				break;
			}
			case PatternMatcher::StackOp::DefinePattern:
			{
				const char* name_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Pattern_name];
				bool visible_ = (int)oi->arg[ PatternMatcher::StackOp::Pattern_visible];
				matcherInstance->definePattern( name_, visible_);
				break;
			}
			case PatternMatcher::StackOp::AttachVariable:
			{
				const char* name_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Variable_name];
				matcherInstance->attachVariable( name_, 1.0);
				break;
			}
		}
	}
}
private:
static std::string termSymbolKey( unsigned int termid, const std::string& name)
{
	char termidbuf[ 16];
	std::size_t termidsize = utf8encode( termidbuf, termid+1);
	std::string symkey( termidbuf, termidsize);
	symkey.append( name);
	return symkey;
}
public:
static void loadPatterMatcherFromFile(
	Reference<PatternMatcherInstanceInterface>& matcherInstance, 
	Reference<PatternTermFeederInstanceInterface>& feederInstance, 
	const TextProcessorInterface* textproc,
	const std::string& patternMatcherModule,
	const std::string& filename,
	ErrorBufferInterface* errorhnd)
{
	const PatternMatcherInterface* matcher = textproc->getPatternMatcher( patternMatcherModule);
	if (!matcher) throw strus::runtime_error(_TXT("failed to load matcher module: %s"), errorhnd->fetchError());
	const PatternTermFeederInterface* feeder = textproc->getPatternTermFeeder();
	matcherInstance.reset( matcher->createInstance());
	feederInstance.reset( feeder->createInstance());
	if (!feederInstance.get()) throw strus::runtime_error(_TXT("failed to create pattern feeder: %s"), errorhnd->fetchError());
	if (!matcherInstance.get()) throw strus::runtime_error(_TXT("failed to create pattern matcher: %s"), errorhnd->fetchError());
	std::string filepath( textproc->getResourcePath( filename));
	if (filepath.empty())
	{
		throw strus::runtime_error(_TXT("path of file '%s' not found: %s"), filename.c_str(), errorhnd->fetchError());
	}
	std::string content;
	unsigned int ec = readFile( filepath, content);
	if (ec) throw strus::runtime_error(_TXT("failed to read serialized patterns from file '%s': %s"), filepath.c_str(), ::strerror(ec));
	if (!loadPatternMatcherFromSerialization( content, feederInstance.get(), matcherInstance.get(), errorhnd))
	{
		throw strus::runtime_error(_TXT("failed to load pattern matcher from serialization: %s"), errorhnd->fetchError());
	}
}
};

void DocumentAnalyzer::definePatternMatcherPostProc(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const PatternMatcher& patterns)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = (DocumentAnalyzerInterface*)m_analyzer_impl.get();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface*>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	Reference<PatternMatcherInstanceInterface> matcherInstance;
	Reference<PatternTermFeederInstanceInterface> feederInstance;
	PatternMatchLoader::loadPatterMatcher( matcherInstance, feederInstance, textproc, patternMatcherModule, patterns, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, matcherInstance.get(), feederInstance.get());
	matcherInstance.release();
	feederInstance.release();
}

void DocumentAnalyzer::definePatternMatcherPostProcFromFile(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const std::string& serializedPatternFile)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = (DocumentAnalyzerInterface*)m_analyzer_impl.get();
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

void DocumentAnalyzer::defineDocument(
	const std::string& subDocumentTypeName,
	const std::string& selectexpr)
{
	DocumentAnalyzerInterface* THIS = (DocumentAnalyzerInterface*)m_analyzer_impl.get();
	THIS->defineSubDocument( subDocumentTypeName, selectexpr);
}

static Document mapDocument( const analyzer::Document& doc)
{
	Document rt;
	std::vector<analyzer::Attribute>::const_iterator
		ai = doc.attributes().begin(), ae = doc.attributes().end();
	for (; ai != ae; ++ai)
	{
		if (ai->name() == Constants::attribute_docid())
		{
			rt.setDocid( ai->value());
		}
		else
		{
			rt.setAttribute( ai->name(), ai->value());
		}
	}
	std::vector<analyzer::MetaData>::const_iterator
		mi = doc.metadata().begin(), me = doc.metadata().end();
	for (; mi != me; ++mi)
	{
		Variant val = getNumericVariantFromDouble( mi->value());
		rt.setMetaData( mi->name(), val);
	}
	std::vector<analyzer::Term>::const_iterator
		ti = doc.searchIndexTerms().begin(), te = doc.searchIndexTerms().end();
	for (; ti != te; ++ti)
	{
		rt.addSearchIndexTerm( ti->type(), ti->value(), ti->pos());
	}
	std::vector<analyzer::Term>::const_iterator
		fi = doc.forwardIndexTerms().begin(), fe = doc.forwardIndexTerms().end();
	for (; fi != fe; ++fi)
	{
		rt.addForwardIndexTerm( fi->type(), fi->value(), fi->pos());
	}
	return rt;
}

static Document analyzeDocument( DocumentAnalyzerInterface* THIS, const std::string& content, const analyzer::DocumentClass& dclass, ErrorBufferInterface* errorhnd)
{
	analyzer::Document doc = THIS->analyze( content, dclass);
	return mapDocument( doc);
}

Document DocumentAnalyzer::analyze( const std::string& content)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = (DocumentAnalyzerInterface*)m_analyzer_impl.get();
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
	Document rt( analyzeDocument( THIS, content, dclass, errorhnd));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT( "failed to analyze document (%s)"), errorhnd->fetchError());
	}
	return rt;
}

Document DocumentAnalyzer::analyze( const std::string& content, const DocumentClass& dclass)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInterface* THIS = (DocumentAnalyzerInterface*)m_analyzer_impl.get();
	analyzer::DocumentClass documentClass( dclass.mimeType(), dclass.encoding(), dclass.scheme());

	Document rt( analyzeDocument( THIS, content, documentClass, errorhnd));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT( "failed to analyze document (%s)"), errorhnd->fetchError());
	}
	return rt;
}

DocumentAnalyzeQueue DocumentAnalyzer::createQueue() const
{
	return DocumentAnalyzeQueue( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_analyzer_impl, m_textproc);
}

DocumentAnalyzeQueue::DocumentAnalyzeQueue( const DocumentAnalyzeQueue& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
	,m_result_queue(o.m_result_queue)
	,m_result_queue_idx(o.m_result_queue_idx)
	,m_analyzerctx_queue(o.m_analyzerctx_queue)
	,m_analyzerctx_queue_idx(o.m_analyzerctx_queue_idx)
	,m_textproc(o.m_textproc)
{}

DocumentAnalyzeQueue::DocumentAnalyzeQueue( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd, const HostObjectReference& analyzer, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(analyzer)
	,m_result_queue()
	,m_result_queue_idx(0)
	,m_analyzerctx_queue()
	,m_analyzerctx_queue_idx(0)
	,m_textproc(textproc_)
{}

void DocumentAnalyzeQueue::push( const std::string& content)
{
	const TextProcessorInterface* textproc = (const TextProcessorInterface*)m_textproc;
	analyzer::DocumentClass dclass;
	if (!textproc->detectDocumentClass( dclass, content.c_str(), content.size()))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to detect document class: %s"), errorhnd->fetchError());
	}
	DocumentAnalyzerInterface* analyzer = (DocumentAnalyzerInterface*)m_analyzer_impl.get();
	Reference analyzerContext_impl( ReferenceDeleter<DocumentAnalyzerContextInterface>::function);
	DocumentAnalyzerContextInterface* analyzerContext;
	analyzerContext_impl.reset( analyzerContext = analyzer->createContext( dclass));
	analyzerContext->putInput( content.c_str(), content.size(), true);
	m_analyzerctx_queue.push_back( analyzerContext_impl);
	analyzeNext();
}

void DocumentAnalyzeQueue::analyzeNext()
{
	if (m_result_queue_idx == m_result_queue.size())
	{
		m_result_queue.clear();
		while (m_analyzerctx_queue_idx < m_analyzerctx_queue.size())
		{
			DocumentAnalyzerContextInterface* analyzerContext = (DocumentAnalyzerContextInterface*)m_analyzerctx_queue[ m_analyzerctx_queue_idx].get();
			analyzer::Document doc;
			if (analyzerContext->analyzeNext( doc))
			{
				m_result_queue.push_back( mapDocument( doc));
			}
			else
			{
				ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
				if (errorhnd->hasError())
				{
					throw strus::runtime_error( _TXT( "failed to analyze document (%s)"), errorhnd->fetchError());
				}
				++m_analyzerctx_queue_idx;
			}
		}
	}
}

void DocumentAnalyzeQueue::push( const std::string& content, const DocumentClass& dclass_)
{
	analyzer::DocumentClass dclass( dclass_.mimeType(), dclass_.encoding(), dclass_.scheme());
	DocumentAnalyzerInterface* analyzer = (DocumentAnalyzerInterface*)m_analyzer_impl.get();
	Reference analyzerContext_impl( ReferenceDeleter<DocumentAnalyzerContextInterface>::function);
	DocumentAnalyzerContextInterface* analyzerContext;
	analyzerContext_impl.reset( analyzerContext = analyzer->createContext( dclass));
	analyzerContext->putInput( content.c_str(), content.size(), true);
	m_analyzerctx_queue.push_back( analyzerContext_impl);
	analyzeNext();
}

bool DocumentAnalyzeQueue::hasMore() const
{
	return (m_result_queue_idx < m_result_queue.size());
}

Document DocumentAnalyzeQueue::fetch()
{
	if (m_result_queue_idx < m_result_queue.size())
	{
		Document rt( m_result_queue[ m_result_queue_idx++]);
		analyzeNext();
		return rt;
	}
	else
	{
		m_result_queue.clear();
		analyzeNext();
		if (m_result_queue_idx == m_result_queue.size())
		{
			throw strus::runtime_error( _TXT("no results to fetch from query analyzer queue"));
		}
		Document rt( m_result_queue[ m_result_queue_idx++]);
		analyzeNext();
		return rt;
	}
}

void PatternMatcher::pushTerm( const std::string& type_, const std::string& value_)
{
	StackOp op( StackOp::PushTerm, allocid( type_), allocid( value_));
	m_ops.push_back( op);
	m_size += 1;
}

void PatternMatcher::pushPattern( const std::string& name_)
{
	StackOp op( StackOp::PushPattern, allocid( name_));
	m_ops.push_back( op);
	m_size += 1;
}

void PatternMatcher::pushExpression( const std::string& opname_, unsigned int argc_, int range_, unsigned int cardinality_)
{
	if (argc_ > (unsigned int)m_size)
	{
		throw strus::runtime_error( _TXT("illegal operation on stack of expression (%u > %u)"), argc_, (unsigned int)m_size);
	}
	StackOp op( StackOp::PushExpression, allocid( opname_), argc_, range_, cardinality_);
	m_ops.push_back( op);
	m_size -= argc_;
	m_size += 1;
}

void PatternMatcher::definePattern( const std::string& name_, bool visible_)
{
	StackOp op( StackOp::DefinePattern, allocid( name_), visible_?1:0);
	m_ops.push_back( op);
	m_size += 1;
}

void PatternMatcher::attachVariable( const std::string& name_)
{
	StackOp op( StackOp::AttachVariable, allocid( name_));
	m_ops.push_back( op);
	m_size += 1;
}

void PatternMatcher::add( const PatternMatcher& o)
{
	std::size_t strinc = m_strings.size();
	m_strings.append( o.m_strings);
	std::vector<StackOp>::const_iterator si = o.m_ops.begin(), se = o.m_ops.end();
	for (; si != se; ++si)
	{
		StackOp op(*si);
		switch (op.type)
		{
			case StackOp::PushTerm:
				op.arg[ StackOp::Term_type] += strinc;
				op.arg[ StackOp::Term_value] += strinc;
				break;
			case StackOp::PushPattern:
				op.arg[ StackOp::Pattern_name] += strinc;
				break;
			case StackOp::PushExpression:
				op.arg[ StackOp::Expression_opname] += strinc;
				break;
			case StackOp::DefinePattern:
				op.arg[ StackOp::Pattern_name] += strinc;
				break;
			case StackOp::AttachVariable:
				op.arg[ StackOp::Variable_name] += strinc;
				break;
		}
		m_ops.push_back( op);
	}
	m_size += o.m_size;
}

std::size_t PatternMatcher::allocid( const std::string& str)
{
	std::size_t rt = m_strings.size()+1;
	m_strings.push_back('\0');
	m_strings.append( str);
	return rt;
}

QueryAnalyzer::QueryAnalyzer( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd)
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

QueryAnalyzer::QueryAnalyzer( const QueryAnalyzer& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
{}

QueryAnalyzeContext QueryAnalyzer::createContext() const
{
	return QueryAnalyzeContext( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_analyzer_impl);
}

void QueryAnalyzer::addSearchIndexElement(
		const std::string& featureType,
		const std::string& fieldType,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->addSearchIndexElement(
		featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzer::addSearchIndexElementFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const std::vector<Normalizer>& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, normalizers, errorhnd);

	THIS->addSearchIndexElementFromPatternMatch( type, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzer::addPatternLexem(
		const std::string& featureType,
		const std::string& fieldType,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInterface* THIS = (QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->addPatternLexem( featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzer::definePatternMatcherPostProc(
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

void QueryAnalyzer::definePatternMatcherPostProcFromFile(
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

std::vector<Term> QueryAnalyzer::analyzeField(
		const std::string& fieldType,
		const std::string& fieldContent)
{
	std::vector<Term> rt;
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

QueryAnalyzeContext::QueryAnalyzeContext( const QueryAnalyzeContext& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
	,m_analyzer_ctx_impl(o.m_analyzer_ctx_impl)
{}

QueryAnalyzeContext::QueryAnalyzeContext( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd, const HostObjectReference& analyzer)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(analyzer)
	,m_analyzer_ctx_impl(ReferenceDeleter<QueryAnalyzerContextInterface>::function)
{
	const QueryAnalyzerInterface* qai = (const QueryAnalyzerInterface*)m_analyzer_impl.get();
	m_analyzer_ctx_impl.reset( qai->createContext());
	if (!m_analyzer_ctx_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create query analyzer context: %s"), errorhnd->fetchError());
	}
}

void QueryAnalyzeContext::putField(
		unsigned int fieldNo, 
		const std::string& fieldType,
		const std::string& fieldContent)
{
	QueryAnalyzerContextInterface* anactx = (QueryAnalyzerContextInterface*)m_analyzer_ctx_impl.get();
	anactx->putField( fieldNo, fieldType, fieldContent);
}

std::vector<QueryTerm> QueryAnalyzeContext::analyze()
{
	std::vector<QueryTerm> rt;
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerContextInterface* anactx = (QueryAnalyzerContextInterface*)m_analyzer_ctx_impl.get();
	analyzer::Query qry = anactx->analyze();
	if (qry.empty() && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error in analyze query: %s"), errorhnd->fetchError());
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
				rt.push_back( QueryTerm( ei->fieldNo(), md.name(), md.value().tostring().c_str(), ei->position(), ei->length()));
				break;
			}
			case analyzer::Query::Element::SearchIndexTerm:
			{
				const analyzer::Term& term = qry.searchIndexTerm( ei->idx());
				rt.push_back( QueryTerm( ei->fieldNo(), term.type(), term.value(), term.pos(), ei->position()));
				break;
			}
		}
	}
	return rt;
}


StorageClient::StorageClient( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_storage_impl(ReferenceDeleter<StorageClientInterface>::function)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = (const StorageObjectBuilderInterface*)m_objbuilder_impl.get();

	m_storage_impl.reset( createStorageClient( objBuilder, errorhnd, config_));
	if (!m_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create storage client: %s"), errorhnd->fetchError());
	}
}

StorageClient::StorageClient( const StorageClient& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
{}

Index StorageClient::nofDocumentsInserted() const
{
	StorageClientInterface* THIS = (StorageClientInterface*)m_storage_impl.get();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->nofDocumentsInserted();
}

StorageTransaction StorageClient::createTransaction() const
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return StorageTransaction( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl);
}

StatisticsIterator StorageClient::createInitStatisticsIterator( bool sign) const
{
	StorageClientInterface* storage = (StorageClientInterface*)m_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	Reference iter( ReferenceDeleter<StatisticsIteratorInterface>::function);
	iter.reset( storage->createInitStatisticsIterator( sign));
	if (!iter.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return StatisticsIterator( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter);
}

StatisticsIterator StorageClient::createUpdateStatisticsIterator() const
{
	StorageClientInterface* storage = (StorageClientInterface*)m_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	Reference iter( ReferenceDeleter<StatisticsIteratorInterface>::function);
	iter.reset( storage->createUpdateStatisticsIterator());
	if (!iter.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return StatisticsIterator( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter);
}

DocumentBrowser StorageClient::createDocumentBrowser()
{
	if (!m_objbuilder_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return DocumentBrowser( m_objbuilder_impl, m_trace_impl, m_storage_impl, m_errorhnd_impl);
}

void StorageClient::close()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcher::VectorStorageSearcher( const HostObjectReference& storageref, const HostObjectReference& trace, const Index& range_from, const Index& range_to, const HostObjectReference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_searcher_impl(ReferenceDeleter<VectorStorageSearchInterface>::function)
	,m_trace_impl( trace)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)storageref.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	m_searcher_impl.reset( storage->createSearcher( range_from, range_to));
	if (!m_searcher_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage searcher: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcher::VectorStorageSearcher( const VectorStorageSearcher& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_searcher_impl(o.m_searcher_impl)
	,m_trace_impl(o.m_trace_impl)
{}

std::vector<VecRank> VectorStorageSearcher::findSimilar( const std::vector<double>& vec, unsigned int maxNofResults) const
{
	VectorStorageSearchInterface* searcher = (VectorStorageSearchInterface*)m_searcher_impl.get();
	if (!searcher) throw strus::runtime_error( _TXT("calling vector storage searcher method after close"));

	std::vector<VectorStorageSearchInterface::Result> res = searcher->findSimilar( vec, maxNofResults);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	std::vector<VecRank> rt;
	std::vector<VectorStorageSearchInterface::Result>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( VecRank( ri->featidx(), ri->weight()));
	}
	return rt;
}

std::vector<VecRank> VectorStorageSearcher::findSimilarFromSelection( const IndexVector& featidxlist, const FloatVector& vec, unsigned int maxNofResults) const
{
	VectorStorageSearchInterface* searcher = (VectorStorageSearchInterface*)m_searcher_impl.get();
	if (!searcher) throw strus::runtime_error( _TXT("calling vector storage searcher method after close"));

	std::vector<VectorStorageSearchInterface::Result> res = searcher->findSimilarFromSelection( featidxlist, vec, maxNofResults);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	std::vector<VecRank> rt;
	std::vector<VectorStorageSearchInterface::Result>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( VecRank( ri->featidx(), ri->weight()));
	}
	return rt;
}

void VectorStorageSearcher::close()
{
	if (!m_searcher_impl.get()) throw strus::runtime_error( _TXT("calling storage searcher method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_searcher_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage searcher close: %s"), errorhnd->fetchError());
	}
}

VectorStorageClient::VectorStorageClient( const VectorStorageClient& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_vector_storage_impl(o.m_vector_storage_impl)
	,m_config(o.m_config)
{}

void VectorStorageClient::close()
{
	if (!m_vector_storage_impl.get()) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_vector_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcher VectorStorageClient::createSearcher( const Index& range_from, const Index& range_to) const
{
	return VectorStorageSearcher( m_vector_storage_impl, m_trace_impl, range_from, range_to, m_errorhnd_impl);
}

VectorStorageTransaction VectorStorageClient::createTransaction()
{
	return VectorStorageTransaction( m_objbuilder_impl, m_vector_storage_impl, m_trace_impl, m_errorhnd_impl, m_config);
}

std::vector<std::string> VectorStorageClient::conceptClassNames() const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	std::vector<std::string> rt = storage->conceptClassNames();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept class names: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::vector<Index> VectorStorageClient::conceptFeatures( const std::string& conceptClass, const Index& conceptid) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	std::vector<Index> res = storage->conceptFeatures( conceptClass, conceptid);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept features: %s"), errorhnd->fetchError());
	}
	std::vector<Index> rt;
	std::vector<Index>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( *ri);
	}
	return rt;
}

unsigned int VectorStorageClient::nofConcepts( const std::string& conceptClass) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofConcepts( conceptClass);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of concepts: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::vector<Index> VectorStorageClient::featureConcepts( const std::string& conceptClass, const Index& index) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::vector<Index> res = storage->featureConcepts( conceptClass, index);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature concepts: %s"), errorhnd->fetchError());
	}
	std::vector<Index> rt;
	std::vector<Index>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( *ri);
	}
	return rt;
}

std::vector<double> VectorStorageClient::featureVector( const Index& index) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::vector<double> rt = storage->featureVector( index);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature vector of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClient::featureName( const Index& index) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::string rt = storage->featureName( index);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

Index VectorStorageClient::featureIndex( const std::string& name) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	Index rt = storage->featureIndex( name);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %s: %s"), name.c_str(), errorhnd->fetchError());
	}
	return rt;
}

unsigned int VectorStorageClient::nofFeatures() const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofFeatures();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of features defined: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClient::config() const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::string rt = storage->config();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the storage configuration string: %s"), errorhnd->fetchError());
	}
	return rt;
}

VectorStorageClient::VectorStorageClient( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_vector_storage_impl(ReferenceDeleter<VectorStorageClientInterface>::function)
	,m_config(config)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = (const StorageObjectBuilderInterface*)m_objbuilder_impl.get();

	m_vector_storage_impl.reset( createVectorStorageClient( objBuilder, errorhnd, config));
	if (!m_vector_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage client: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransaction::VectorStorageTransaction( const VectorStorageTransaction& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_vector_storage_impl(o.m_vector_storage_impl)
	,m_vector_transaction_impl(o.m_vector_transaction_impl)
{}

void VectorStorageTransaction::addFeature( const std::string& name, const std::vector<double>& vec)
{
	VectorStorageTransactionInterface* transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!transaction)
	{
		StorageClientInterface* storage = (StorageClientInterface*)m_vector_storage_impl.get();
		m_vector_transaction_impl.reset( (StorageTransactionInterface*)storage->createTransaction());
		if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
		transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	}
	transaction->addFeature( name, vec);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

void VectorStorageTransaction::defineFeatureConceptRelation( const std::string& relationTypeName, const Index& featidx, const Index& conidx)
{
	VectorStorageTransactionInterface* transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!transaction)
	{
		StorageClientInterface* storage = (StorageClientInterface*)m_vector_storage_impl.get();
		m_vector_transaction_impl.reset( (StorageTransactionInterface*)storage->createTransaction());
		if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
		transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	}
	transaction->defineFeatureConceptRelation( relationTypeName, featidx, conidx);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

bool VectorStorageTransaction::commit()
{
	VectorStorageTransactionInterface* transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));

	bool rt = transaction->commit();
	if (!rt)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT("failed to complete vector storage building (done): %s"), errorhnd->fetchError());
		}
	}
	return rt;
}

void VectorStorageTransaction::rollback()
{
	VectorStorageTransactionInterface* transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));
	transaction->rollback();
}

void VectorStorageTransaction::close()
{
	if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_vector_transaction_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransaction::VectorStorageTransaction( const HostObjectReference& objbuilder, const HostObjectReference& storageref, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_vector_storage_impl(storageref)
	,m_vector_transaction_impl(ReferenceDeleter<VectorStorageTransactionInterface>::function)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();

	m_vector_transaction_impl.reset( storage->createTransaction());
	if (!m_vector_transaction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage transaction: %s"), errorhnd->fetchError());
	}
}

StorageTransaction::StorageTransaction( const HostObjectReference& objbuilder_, const HostObjectReference& trace_, const HostObjectReference& errorhnd_, const HostObjectReference& storage_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_storage_impl(storage_)
	,m_transaction_impl(ReferenceDeleter<StorageTransactionInterface>::function)
{}

void StorageTransaction::insertDocument( const std::string& docid, const Document& doc)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageClientInterface* storage = (StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (StorageTransactionInterface*)storage->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
	}
	StorageTransactionInterface* transaction = (StorageTransactionInterface*)m_transaction_impl.get();
	std::auto_ptr<StorageDocumentInterface> document( transaction->createDocument( docid));
	if (!document.get()) throw strus::runtime_error( _TXT("failed to create document with id '%s' to insert: %s"), docid.c_str(), errorhnd->fetchError());

	std::vector<Attribute>::const_iterator
		ai = doc.attributes().begin(), ae = doc.attributes().end();
	for (; ai != ae; ++ai)
	{
		document->setAttribute( ai->name(), ai->value());
	}
	std::vector<MetaData>::const_iterator
		mi = doc.metaData().begin(), me = doc.metaData().end();
	for (; mi != me; ++mi)
	{
		document->setMetaData( mi->name(), numericVariant( mi->value()));
	}
	std::vector<Term>::const_iterator
		ti = doc.searchIndexTerms().begin(), te = doc.searchIndexTerms().end();
	for (; ti != te; ++ti)
	{
		document->addSearchIndexTerm( ti->type(), ti->value(), ti->position());
	}
	std::vector<Term>::const_iterator
		fi = doc.forwardIndexTerms().begin(), fe = doc.forwardIndexTerms().end();
	for (; fi != fe; ++fi)
	{
		document->addForwardIndexTerm( fi->type(), fi->value(), fi->position());
	}
	std::vector<std::string>::const_iterator
		ui = doc.users().begin(), ue = doc.users().end();
	for (; ui != ue; ++ui)
	{
		document->setUserAccessRight( *ui);
	}
	document->done();
}

void StorageTransaction::deleteDocument( const std::string& docId)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageClientInterface* storage = (StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (StorageTransactionInterface*)storage->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for deleting document: %s"), errorhnd->fetchError());
	}
	StorageTransactionInterface* transaction = (StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteDocument( docId);
}

void StorageTransaction::deleteUserAccessRights( const std::string& username)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageClientInterface* storage = (StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (StorageTransactionInterface*)storage->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for deleting user access rights: %s"), errorhnd->fetchError());
	}
	StorageTransactionInterface* transaction = (StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteUserAccessRights( username);
}

void StorageTransaction::commit()
{
	StorageTransactionInterface* transaction = (StorageTransactionInterface*)m_transaction_impl.get();
	if (transaction)
	{
		if (!transaction->commit())
		{
			m_transaction_impl.reset();
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			throw strus::runtime_error( _TXT("error flushing storage operations: %s"), errorhnd->fetchError());
		}
		m_transaction_impl.reset();
	}
}

void StorageTransaction::rollback()
{
	m_transaction_impl.reset();
}

StatisticsIterator::StatisticsIterator( const StatisticsIterator& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_iter_impl(o.m_iter_impl){}

StatisticsIterator::StatisticsIterator( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const HostObjectReference& storage_, const HostObjectReference& iter_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_storage_impl(storage_)
	,m_iter_impl(iter_)
{}

std::string StatisticsIterator::getNext()
{
	StatisticsIteratorInterface* iter = (StatisticsIteratorInterface*)m_iter_impl.get();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const char* outmsg;
	std::size_t outmsgsize;
	if (!iter->getNext( outmsg, outmsgsize))
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( _TXT("error fetching statistics message: %s"), errorhnd->fetchError());
		}
	}
	return std::string( outmsg, outmsgsize);
}

StatisticsProcessor::StatisticsProcessor( const HostObjectReference& objbuilder_, const HostObjectReference& trace_, const std::string& name_, const HostObjectReference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_statsproc(0)
{
	const StorageObjectBuilderInterface* objBuilder = (const StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	m_statsproc = objBuilder->getStatisticsProcessor( name_);
	if (!m_statsproc)
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT( "error getting statistics message processor: %s"), errorhnd->fetchError());
		}
		throw strus::runtime_error(_TXT( "error statistics message processor not defined"));
	}
}

StatisticsMessage StatisticsProcessor::decode( const std::string& blob) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsProcessorInterface* proc = (const StatisticsProcessorInterface*)m_statsproc;
	std::auto_ptr<StatisticsViewerInterface> viewer( proc->createViewer( blob.c_str(), blob.size()));
	std::vector<DocumentFrequencyChange> dflist;
	StatisticsViewerInterface::DocumentFrequencyChange rec;
	while (viewer->nextDfChange( rec))
	{
		dflist.push_back( DocumentFrequencyChange( rec.type, rec.value, rec.increment));
	}
	int nofdocs = viewer->nofDocumentsInsertedChange();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "error statistics message structure from blob: %s"), errorhnd->fetchError());
	}
	return StatisticsMessage( dflist, nofdocs);
}

std::string StatisticsProcessor::encode( const StatisticsMessage& msg) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsProcessorInterface* proc = (const StatisticsProcessorInterface*)m_statsproc;
	StatisticsProcessorInterface::BuilderOptions options;
	std::auto_ptr<StatisticsBuilderInterface> builder( proc->createBuilder( options));
	std::vector<DocumentFrequencyChange>::const_iterator
			dfi = msg.documentFrequencyChangeList().begin(),
			dfe = msg.documentFrequencyChangeList().end();
	for (; dfi != dfe; ++dfi)
	{
		builder->addDfChange( dfi->type().c_str(), dfi->value().c_str(), dfi->increment());
	}
	builder->setNofDocumentsInsertedChange( msg.nofDocumentsInsertedChange());
	std::string rt;
	const char* blk;
	std::size_t blksize;
	if (builder->fetchMessage( blk, blksize))
	{
		rt.append( blk, blksize);
	}
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "error creating blob from statistics message structure: %s"), errorhnd->fetchError());
	}
	return rt;
}


QueryEval::QueryEval( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_queryeval_impl(ReferenceDeleter<QueryEvalInterface>::function)
{
	const StorageObjectBuilderInterface* objBuilder = (const StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	m_queryproc = objBuilder->getQueryProcessor();
	if (!m_queryproc)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("error in get query processor: %s"), errorhnd->fetchError());
	}
	m_queryeval_impl.reset( objBuilder->createQueryEval());
	if (!m_queryeval_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("error creating query eval: %s"), errorhnd->fetchError());
	}
}

QueryEval::QueryEval( const QueryEval& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_queryproc(o.m_queryproc)
{}

void QueryEval::addTerm(
		const std::string& set_,
		const std::string& type_,
		const std::string& value_)
{
	QueryEvalInterface* queryeval = (QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addTerm( set_, type_, value_);
}

void QueryEval::addSelectionFeature( const std::string& set_)
{
	QueryEvalInterface* queryeval = (QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addSelectionFeature( set_);
}

void QueryEval::addRestrictionFeature( const std::string& set_)
{
	QueryEvalInterface* queryeval = (QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addRestrictionFeature( set_);
}

void QueryEval::addExclusionFeature( const std::string& set_)
{
	QueryEvalInterface* queryeval = (QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addExclusionFeature( set_);
}

void QueryEval::addSummarizer(
		const std::string& name,
		const SummarizerConfig& config,
		const std::string& debugAttributeName)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;

	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryProcessorInterface* queryproc = (const QueryProcessorInterface*)m_queryproc;

	const SummarizerFunctionInterface* sf = queryproc->getSummarizerFunction( name);
	if (!sf) throw strus::runtime_error( _TXT("summarizer function not defined: '%s'"), name.c_str());

	Reference<SummarizerFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating summarizer function: %s"), errorhnd->fetchError());

	QueryEvalInterface* queryeval = (QueryEvalInterface*)m_queryeval_impl.get();
	std::map<std::string,Variant>::const_iterator
		pi = config.m_parameters.begin(), pe = config.m_parameters.end();
	for (; pi != pe; ++pi)
	{
		if (pi->second.m_type == Variant_TEXT)
		{
			function->addStringParameter( pi->first, pi->second.m_value.TEXT);
		}
		else
		{
			function->addNumericParameter( pi->first, numericVariant( pi->second));
		}
	}
	std::vector<FeatureParameter> featureParameters;
	std::map<std::string,std::string>::const_iterator
		fi = config.m_features.begin(), fe = config.m_features.end();
	for (; fi != fe; ++fi)
	{
		featureParameters.push_back( FeatureParameter( fi->first, fi->second));
	}
	std::map<std::string,std::string>::const_iterator
		ri = config.m_resultnamemap.begin(), re = config.m_resultnamemap.end();
	for (; ri != re; ++ri)
	{
		function->defineResultName( ri->first, ri->second);
	}
	queryeval->addSummarizerFunction( name, function.get(), featureParameters, debugAttributeName);
	function.release();
}

void QueryEval::addWeightingFunction(
		const std::string& name,
		const WeightingConfig& config,
		const std::string& debugAttributeName)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;

	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryProcessorInterface* queryproc = (const QueryProcessorInterface*)m_queryproc;

	const WeightingFunctionInterface* sf = queryproc->getWeightingFunction( name);
	if (!sf) throw strus::runtime_error( _TXT("weighting function not defined: '%s'"), name.c_str());

	Reference<WeightingFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating weighting function instance '%s': '%s'"), name.c_str(), errorhnd->fetchError());

	QueryEvalInterface* queryeval = (QueryEvalInterface*)m_queryeval_impl.get();
	std::map<std::string,Variant>::const_iterator
		pi = config.m_parameters.begin(), pe = config.m_parameters.end();
	for (; pi != pe; ++pi)
	{
		if (pi->second.type() == Variant_TEXT)
		{
			function->addStringParameter( pi->first, pi->second.getText());
		}
		else
		{
			function->addNumericParameter( pi->first, numericVariant( pi->second));
		}
	}
	std::vector<FeatureParameter> featureParameters;
	std::map<std::string,std::string>::const_iterator
		fi = config.m_features.begin(), fe = config.m_features.end();
	for (; fi != fe; ++fi)
	{
		featureParameters.push_back( FeatureParameter( fi->first, fi->second));
	}
	queryeval->addWeightingFunction( name, function.get(), featureParameters, debugAttributeName);
	function.release();
}

void QueryEval::addWeightingFormula( const std::string& source, const FunctionVariableConfig& defaultParameter)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryEvalInterface* qe = (QueryEvalInterface*)m_queryeval_impl.get();

	const QueryProcessorInterface* queryproc = (const QueryProcessorInterface*)m_queryproc;
	const ScalarFunctionParserInterface* scalarfuncparser = queryproc->getScalarFunctionParser("");
	std::auto_ptr<ScalarFunctionInterface> scalarfunc( scalarfuncparser->createFunction( source, std::vector<std::string>()));
	if (!scalarfunc.get())
	{
		throw strus::runtime_error(_TXT( "failed to create scalar function (weighting formula) from source: %s"), errorhnd->fetchError());
	}
	std::map<std::pair<std::string,double> >::const_iterator
		vi = defaultParameter.m_variables.begin(),
		ve = defaultParameter.m_variables.end();
	for (; vi != ve; ++vi)
	{
		scalarfunc->setDefaultVariableValue( vi->first, vi->second);
	}
	qe->defineWeightingFormula( scalarfunc.get());
	scalarfunc.release();
}

Query QueryEval::createQuery( const StorageClient& storage) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryEvalInterface* qe = (QueryEvalInterface*)m_queryeval_impl.get();
	StorageClientInterface* st = (StorageClientInterface*)storage.m_storage_impl.get();
	Reference query( ReferenceDeleter<QueryInterface>::function);
	query.reset( qe->createQuery( st));
	if (!query.get()) throw strus::runtime_error( _TXT("failed to create query object: %s"), errorhnd->fetchError());

	return Query( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, storage.m_storage_impl, m_queryeval_impl, query, m_queryproc);
}


std::size_t QueryExpression::allocid( const std::string& str)
{
	std::size_t rt = m_strings.size()+1;
	m_strings.push_back('\0');
	m_strings.append( str);
	return rt;
}

void QueryExpression::pushTerm( const std::string& type_, const std::string& value_, const Index& length_)
{
	StackOp op( StackOp::PushTerm, allocid( type_), allocid( value_), length_);
	m_ops.push_back( op);
	m_size += 1;
}

void QueryExpression::pushDocField( const std::string& metadata_start_, const std::string& metadata_end_)
{
	StackOp op( StackOp::PushDocField, allocid( metadata_start_), allocid( metadata_end_));
	m_ops.push_back( op);
	m_size += 1;
}

void QueryExpression::pushExpression( const std::string& opname_, unsigned int argc_, int range_, unsigned int cardinality_)
{
	if (argc_ > (unsigned int)m_size)
	{
		throw strus::runtime_error( _TXT("illegal operation on stack of expression (%u > %u)"), argc_, (unsigned int)m_size);
	}
	StackOp op( StackOp::PushExpression, allocid( opname_), argc_, range_, cardinality_);
	m_ops.push_back( op);
	m_size -= argc_;
	m_size += 1;
}

void QueryExpression::attachVariable( const std::string& name_)
{
	StackOp op( StackOp::AttachVariable, allocid( name_));
	m_ops.push_back( op);
}

void QueryExpression::add( const QueryExpression& o)
{
	std::size_t strinc = m_strings.size();
	m_strings.append( o.m_strings);
	std::vector<StackOp>::const_iterator si = o.m_ops.begin(), se = o.m_ops.end();
	for (; si != se; ++si)
	{
		StackOp op(*si);
		switch (op.type)
		{
			case StackOp::PushTerm:
				op.arg[ StackOp::Term_type] += strinc;
				op.arg[ StackOp::Term_value] += strinc;
				break;
			case StackOp::PushDocField:
				op.arg[ StackOp::Term_metastart] += strinc;
				op.arg[ StackOp::Term_metaend] += strinc;
				break;
			case StackOp::PushExpression:
				op.arg[ StackOp::Expression_opname] += strinc;
				break;
			case StackOp::AttachVariable:
				op.arg[ StackOp::Variable_name] += strinc;
				break;
		}
		m_ops.push_back( op);
	}
	m_size += o.m_size;
}

Query::Query( const Query& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_query_impl(o.m_query_impl)
	,m_queryproc(o.m_queryproc)
{}

void Query::defineFeature( const std::string& set_, const QueryExpression& expr_, double weight_)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	const QueryProcessorInterface* queryproc = (const QueryProcessorInterface*)m_queryproc;

	if (expr_.size() != 1)
	{
		throw strus::runtime_error( _TXT("the argument expression of defineFeature does not contain exactly one node (%u)"), (unsigned int)expr_.size());
	}
	std::vector<QueryExpression::StackOp>::const_iterator ei = expr_.m_ops.begin(), ee = expr_.m_ops.end();
	for (; ei != ee; ++ei)
	{
		switch (ei->type)
		{
			case QueryExpression::StackOp::PushTerm:
			{
				const char* type_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Term_type];
				const char* value_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Term_value];
				Index length_ = ei->arg[ QueryExpression::StackOp::Term_length];
				THIS->pushTerm( type_, value_, length_);
				break;
			}
			case QueryExpression::StackOp::PushDocField:
			{
				const char* start_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Term_metastart];
				const char* end_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Term_metaend];
				THIS->pushDocField( start_, end_);
				break;
			}
			case QueryExpression::StackOp::PushExpression:
			{
				const char* opname_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Expression_opname];
				unsigned int argc_ = (unsigned int)ei->arg[ QueryExpression::StackOp::Expression_argc];
				int range_ = (int)ei->arg[ QueryExpression::StackOp::Expression_range];
				unsigned int cardinality_ = (unsigned int)ei->arg[ QueryExpression::StackOp::Expression_cardinality];

				const PostingJoinOperatorInterface* joinopr = queryproc->getPostingJoinOperator( opname_);
				if (!joinopr) throw strus::runtime_error( _TXT("posting join operator not defined: '%s'"), opname_);

				THIS->pushExpression( joinopr, argc_, range_, cardinality_);
				break;
			}
			case QueryExpression::StackOp::AttachVariable:
			{
				const char* name_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Variable_name];
				THIS->attachVariable( name_);
				break;
			}
		}
	}
	THIS->defineFeature( set_, weight_);
}

void Query::defineDocFieldFeature( const std::string& set_, const std::string& metadataStart, const std::string& metadataEnd)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	THIS->pushDocField( metadataStart, metadataEnd);
	THIS->defineFeature( set_, 1.0);
}

static MetaDataRestrictionInterface::CompareOperator getCompareOp( const char* compareOp)
{
	MetaDataRestrictionInterface::CompareOperator cmpop;
	if (compareOp[0] == '<')
	{
		if (compareOp[1] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareLess;
		}
		else if (compareOp[1] == '=' && compareOp[2] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareLessEqual;
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
		}
	}
	else if (compareOp[0] == '>')
	{
		if (compareOp[1] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareGreater;
		}
		else if (compareOp[1] == '=' && compareOp[2] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareGreaterEqual;
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
		}
	}
	else if (compareOp[0] == '=' && compareOp[1] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareEqual;
	}
	else if (compareOp[0] == '=' && compareOp[1] == '=' && compareOp[2] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareEqual;
	}
	else if (compareOp[0] == '!' && compareOp[1] == '=' && compareOp[2] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareNotEqual;
	}
	else
	{
		throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
	}
	return cmpop;
}

void Query::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		const Variant& operand, bool newGroup)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	MetaDataRestrictionInterface::CompareOperator cmpop = getCompareOp( compareOp);
	THIS->addMetaDataRestrictionCondition( cmpop, name, numericVariant(operand), newGroup);
}

void Query::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		double value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void Query::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		unsigned int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void Query::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void Query::defineTermStatistics( const std::string& type_, const std::string& value_, const TermStatistics& stats_)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	TermStatistics stats;
	stats.setDocumentFrequency( stats_.df());
	THIS->defineTermStatistics( type_, value_, stats);
}

void Query::defineGlobalStatistics( const GlobalStatistics& stats_)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	GlobalStatistics stats;
	stats.setNofDocumentsInserted( stats_.nofdocs());
	THIS->defineGlobalStatistics( stats);
}

void Query::addDocumentEvaluationSet(
		const std::vector<Index>& docnolist_)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	std::vector<Index> docnolist;
	std::vector<Index>::const_iterator di = docnolist_.begin(), de = docnolist_.end();
	for (; di != de; ++di) docnolist.push_back( *di);
	THIS->addDocumentEvaluationSet( docnolist);
}

void Query::setMaxNofRanks( unsigned int maxNofRanks_)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	THIS->setMaxNofRanks( maxNofRanks_);
}

void Query::setMinRank( unsigned int minRank_)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	THIS->setMinRank( minRank_);
}

void Query::addUserName( const std::string& username_)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	THIS->addUserName( username_);
}

void Query::setWeightingVariables(
		const FunctionVariableConfig& parameter)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	std::map<std::pair<std::string,double> >::const_iterator
		vi = parameter.m_variables.begin(),
		ve = parameter.m_variables.end();
	for (; vi != ve; ++vi)
	{
		THIS->setWeightingVariableValue( vi->first, vi->second);
	}
}

void Query::setDebugMode( bool debug)
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	THIS->setDebugMode( debug);
}

QueryResult Query::evaluate() const
{
	std::vector<Rank> ranks;
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	QueryResult res = THIS->evaluate();
	if (res.ranks().empty() && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to evaluate query: %s"), errorhnd->fetchError());
	}
	QueryResult rt( res.evaluationPass(), res.nofDocumentsRanked(), res.nofDocumentsVisited());
	std::vector<ResultDocument>::const_iterator
		ri = res.ranks().begin(), re = res.ranks().end();
	for (;ri != re; ++ri)
	{
		Rank reselem;
		reselem.m_docno = (unsigned int)ri->docno();
		reselem.m_weight = ri->weight();
		std::vector<SummaryElement>::const_iterator
			ai = ri->summaryElements().begin(), ae = ri->summaryElements().end();
	
		for (;ai != ae; ++ai)
		{
			SummaryElement elem( ai->name(), ai->value(), ai->weight(), ai->index());
			reselem.m_summaryElements.push_back( elem);
		}
		rt.m_ranks.push_back( reselem);
	}
	return QueryResult( rt);
}

std::string Query::tostring() const
{
	QueryInterface* THIS = (QueryInterface*)m_query_impl.get();
	std::string rt( THIS->tostring());
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to map query to string: %s"), errorhnd->fetchError());
	}
	return rt;
}

DocumentBrowser::DocumentBrowser( const DocumentBrowser& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_restriction_impl(o.m_restriction_impl)
	,m_postingitr_impl(o.m_postingitr_impl)
	,m_attributereader_impl(o.m_attributereader_impl)
	,m_docno(o.m_docno)
{}

DocumentBrowser::DocumentBrowser( const HostObjectReference& objbuilder_impl_, const HostObjectReference& trace_impl_, const HostObjectReference& storage_impl_, const HostObjectReference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_impl_)
	,m_objbuilder_impl(objbuilder_impl_)
	,m_storage_impl(storage_impl_)
	,m_restriction_impl( ReferenceDeleter<MetaDataRestrictionInterface>::function)
	,m_postingitr_impl( ReferenceDeleter<PostingIteratorInterface>::function)
	,m_attributereader_impl( ReferenceDeleter<AttributeReaderInterface>::function)
	,m_docno(0)
{
	const StorageClientInterface* storage = (const StorageClientInterface*)m_storage_impl.get();
	m_restriction_impl.reset( storage->createMetaDataRestriction());
	if (!m_restriction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create meta data restriction interface for browsing documents"));
	}
}

void DocumentBrowser::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		const Variant& value, bool newGroup)
{
	MetaDataRestrictionInterface* restriction = (MetaDataRestrictionInterface*)m_restriction_impl.get();
	if (!restriction)
	{
		throw strus::runtime_error( _TXT("it is not allowed to add more restrictions to a document browser after the first call of next()"));
	}
	MetaDataRestrictionInterface::CompareOperator cmpop = getCompareOp( compareOp);
	restriction->addCondition( cmpop, name, numericVariant(value), newGroup);
}

void DocumentBrowser::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		double value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void DocumentBrowser::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		unsigned int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void DocumentBrowser::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

Index DocumentBrowser::skipDoc( const Index& docno_)
{
	if (!m_postingitr_impl.get())
	{
		const StorageClientInterface* storage = (const StorageClientInterface*)m_storage_impl.get();
		MetaDataRestrictionInterface* restriction = (MetaDataRestrictionInterface*)m_restriction_impl.get();
		m_postingitr_impl.reset( storage->createBrowsePostingIterator( restriction, 1));
		if (!m_postingitr_impl.get())
		{
			throw strus::runtime_error( _TXT("failed to create posting iterator for document browser"));
		}
	}
	PostingIteratorInterface* itr = (PostingIteratorInterface*)m_postingitr_impl.get();
	return m_docno = itr->skipDoc( docno_);
}

std::string DocumentBrowser::attribute( const std::string& name)
{
	if (m_docno)
	{
		if (!m_attributereader_impl.get())
		{
			const StorageClientInterface* storage = (const StorageClientInterface*)m_storage_impl.get();
			m_attributereader_impl.reset( storage->createAttributeReader());
			if (!m_attributereader_impl.get())
			{
				throw strus::runtime_error( _TXT("failed to create attribute reader for document browser"));
			}
		}
		const AttributeReaderInterface* reader = (AttributeReaderInterface*)m_attributereader_impl.get();
		Index elemhnd = reader->elementHandle( name.c_str());
		if (!elemhnd)
		{
			throw strus::runtime_error( _TXT("document attribute name %s is not defined"), name.c_str());
		}
		return reader->getValue( elemhnd);
	}
	else
	{
		return std::string();
	}
}


static ErrorBufferInterface* createErrorBuffer_( unsigned int maxNofThreads)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_standard( 0, maxNofThreads);
	if (!errorhnd)
	{
		throw strus::runtime_error( _TXT("failed to create error buffer object: %s"), errorhnd->fetchError());
	}
	return errorhnd;
}

static ModuleLoaderInterface* createModuleLoader_( ErrorBufferInterface* errorhnd)
{
	ModuleLoaderInterface* rt = createModuleLoader( errorhnd);
	if (!rt)
	{
		throw strus::runtime_error( _TXT("failed to create module loader object: %s"), errorhnd->fetchError());
	}
	return rt;
}

Context::Context()
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
}

Context::Context( unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
}

Context::Context( unsigned int maxNofThreads, const std::string& tracecfg)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
	if (!tracecfg.empty())
	{
		m_trace_impl.reset( new TraceProxy( moduleLoader, tracecfg, errorhnd));
	}
}

Context::Context( const std::string& connectionstring)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.reset( errorhnd);

	std::auto_ptr<RpcClientMessagingInterface> messaging;
	messaging.reset( createRpcClientMessaging( connectionstring.c_str(), errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

Context::Context( const std::string& connectionstring, unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);

	std::auto_ptr<RpcClientMessagingInterface> messaging;
	messaging.reset( createRpcClientMessaging( connectionstring.c_str(), errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

Context::Context( const Context& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_moduleloader_impl(o.m_moduleloader_impl)
	,m_rpc_impl(o.m_rpc_impl)
	,m_trace_impl( o.m_trace_impl)
	,m_storage_objbuilder_impl(o.m_storage_objbuilder_impl)
	,m_analyzer_objbuilder_impl(o.m_analyzer_objbuilder_impl)
	,m_textproc(o.m_textproc)
{}

void Context::checkErrors() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("unhandled error: %s"), errorhnd->fetchError());
	}
}

void Context::loadModule( const std::string& name_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot load modules in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
	if (!moduleLoader->loadModule( name_))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error(_TXT("failed to load module: %s"), errorhnd->fetchError());
	}
}

void Context::addModulePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a module path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addModulePath( paths_);
}

void Context::addResourcePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a resource path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addResourcePath( paths_);
}

StatisticsProcessor Context::createStatisticsProcessor( const std::string& name)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StatisticsProcessor( m_storage_objbuilder_impl, m_trace_impl, name, m_errorhnd_impl);
}

void Context::initStorageObjBuilder()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageObjectBuilderInterface* storageObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		RpcClientInterface* client = (RpcClientInterface*)m_rpc_impl.get();
		storageObjectBuilder = client->createStorageObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
		storageObjectBuilder = moduleLoader->createStorageObjectBuilder();
	}
	else
	{
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
	}
	if (!storageObjectBuilder)
	{
		throw strus::runtime_error(_TXT("failed to create storage object builder: %s"), errorhnd->fetchError());
	}
	TraceProxy* tp = (TraceProxy*)m_trace_impl.get();
	if (tp)
	{
		StorageObjectBuilderInterface* storageObjectBuilder_proxy = tp->createProxy( storageObjectBuilder);
		if (!storageObjectBuilder_proxy)
		{
			delete storageObjectBuilder;
			throw strus::runtime_error(_TXT("failed to create storage object builder trace proxy: %s"), errorhnd->fetchError());
		}
		storageObjectBuilder = storageObjectBuilder_proxy;
	}
	m_storage_objbuilder_impl.reset( storageObjectBuilder);
}

void Context::initAnalyzerObjBuilder()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	AnalyzerObjectBuilderInterface* analyzerObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		RpcClientInterface* client = (RpcClientInterface*)m_rpc_impl.get();
		analyzerObjectBuilder = client->createAnalyzerObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
		analyzerObjectBuilder = moduleLoader->createAnalyzerObjectBuilder();
	}
	else
	{
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
	}
	if (!analyzerObjectBuilder)
	{
		throw strus::runtime_error( _TXT("failed to create analyzer object builder: %s"), errorhnd->fetchError());
	}
	TraceProxy* tp = (TraceProxy*)m_trace_impl.get();
	if (tp)
	{
		AnalyzerObjectBuilderInterface* analyzerObjectBuilder_proxy = tp->createProxy( analyzerObjectBuilder);
		if (!analyzerObjectBuilder_proxy)
		{
			delete analyzerObjectBuilder;
			throw strus::runtime_error(_TXT("failed to create storage object builder trace proxy: %s"), errorhnd->fetchError());
		}
		analyzerObjectBuilder = analyzerObjectBuilder_proxy;
	}
	m_analyzer_objbuilder_impl.reset( analyzerObjectBuilder);
}

DocumentClass Context::detectDocumentClass( const std::string& content)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = (const AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
	const TextProcessorInterface* textproc;
	if (m_textproc)
	{
		textproc = (const TextProcessorInterface*)m_textproc;
	}
	else
	{
		textproc = objBuilder->getTextProcessor();
		m_textproc = textproc;
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
	}
	analyzer::DocumentClass dclass;
	if (textproc->detectDocumentClass( dclass, content.c_str(), content.size()))
	{
		return DocumentClass( dclass.mimeType(), dclass.encoding(), dclass.scheme());
	}
	else
	{
		if (errorhnd->hasError()) throw strus::runtime_error( _TXT("failed to detect document class: %s"), errorhnd->fetchError());
		return DocumentClass();
	}
}

StorageClient Context::createStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, config_);
}

StorageClient Context::createStorageClient()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, std::string());
}

VectorStorageClient Context::createVectorStorageClient()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return VectorStorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, std::string());
}

VectorStorageClient Context::createVectorStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return VectorStorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, config_);
}

DocumentAnalyzer Context::createDocumentAnalyzer( const std::string& segmentername_)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	if (!m_textproc)
	{
		AnalyzerObjectBuilderInterface* objBuilder = (AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
		m_textproc = objBuilder->getTextProcessor();
		if (!m_textproc)
		{
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
		}
	}
	return DocumentAnalyzer( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl, segmentername_, m_textproc);
}

QueryAnalyzer Context::createQueryAnalyzer()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return QueryAnalyzer( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl);
}

QueryEval Context::createQueryEval()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return QueryEval( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl);
}

void Context::createStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const StorageInterface* sti = objBuilder->getStorage();
	if (!sti) throw strus::runtime_error( _TXT("failed to get storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());
}

void Context::createVectorStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagename;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);
	if (!extractStringFromConfigString( dbname, storagename, "storage", errorhnd))
	{
		storagename = Constants::standard_vector_storage();
	}
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const VectorStorageInterface* sti = objBuilder->getVectorStorage( storagename);
	if (!sti) throw strus::runtime_error( _TXT("failed to get vector storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create vector storage: %s"), errorhnd->fetchError());
}

void Context::destroyStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	if (!objBuilder) throw strus::runtime_error( _TXT("failed to get object builder: %s"), errorhnd->fetchError());
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	if (!dbi->destroyDatabase( storagecfg)) throw strus::runtime_error( _TXT("failed to destroy database: %s"), errorhnd->fetchError());
}

void Context::close()
{
	m_analyzer_objbuilder_impl.reset();
	m_storage_objbuilder_impl.reset();
	if (m_rpc_impl.get()) ((RpcClientInterface*)m_rpc_impl.get())->close();
	m_moduleloader_impl.reset();
}


