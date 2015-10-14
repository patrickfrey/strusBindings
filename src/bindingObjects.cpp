/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#include "strus/bindingObjects.hpp"
#undef String
#undef StringVector
#undef NormalizerVector
#undef TermVector
#undef RankVector
#undef RankAttributeVector
#undef AttributeVector
#undef MetaDataVector

#include "strus/strus.hpp"
#include "strus/lib/rpc_client.hpp"
#include "strus/lib/rpc_client_socket.hpp"
#include "strus/lib/module.hpp"
#include "strus/lib/error.hpp"
#include "strus/rpcClientInterface.hpp"
#include "strus/rpcClientMessagingInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/private/configParser.hpp"
#include "internationalization.hpp"
#include "utils.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <limits>

template <class Object>
struct ReferenceDeleter
{
	static void function( void* obj)
	{
		delete (Object*)obj;
	}
};

void Tokenizer::addArgumentInt( long arg_)
{
	addArgument( strus::utils::tostring( arg_));
}

void Tokenizer::addArgumentFloat( double arg_)
{
	addArgument( strus::utils::tostring( (float)arg_));
}

void Normalizer::addArgumentInt( long arg_)
{
	addArgument( strus::utils::tostring( arg_));
}

void Normalizer::addArgumentFloat( double arg_)
{
	addArgument( strus::utils::tostring( (float)arg_));
}

void Aggregator::addArgumentInt( long arg_)
{
	addArgument( strus::utils::tostring( arg_));
}

void Aggregator::addArgumentFloat( double arg_)
{
	addArgument( strus::utils::tostring( (float)arg_));
}

Variant::Variant()
	:m_type(Variant_UNDEFINED){}

Variant::Variant( const Variant& o)
	:m_type(o.m_type),m_buf(o.m_buf)
{
	if (m_type == Variant_TEXT)
	{
		m_value.TEXT = m_buf.c_str();
	}
	else
	{
		std::memcpy( &m_value, &o.m_value, sizeof(m_value));
	}
}

Variant::Variant( unsigned int v)
	:m_type(Variant_UINT)
{
	m_value.UINT = v;
}

Variant::Variant( int v)
	:m_type(Variant_INT)
{
	m_value.INT = v;
}

Variant::Variant( double v)
	:m_type(Variant_FLOAT)
{
	m_value.FLOAT = v;
}

Variant::Variant( const std::string& v)
	:m_type(Variant_TEXT),m_buf(v)
{
	m_value.TEXT = m_buf.c_str();
}

Variant::Variant( const char* v)
	:m_type(Variant_TEXT),m_buf(v)
{
	m_value.TEXT = m_buf.c_str();
}

void Variant::init()
{
	m_type = Variant_UNDEFINED;
	m_value.INT = 0;
	m_buf.clear();
}

void Variant::assign( const Variant& o)
{
	m_type = o.m_type;
	m_buf = o.m_buf;
	if (m_type == Variant_TEXT)
	{
		m_value.TEXT = m_buf.c_str();
	}
	else
	{
		std::memcpy( &m_value, &o.m_value, sizeof(m_value));
	}
}

void Variant::assignUint( unsigned long v)
{
	m_type = Variant_UINT;
	m_value.UINT = v;
	m_buf.clear();
}

void Variant::assignInt( long v)
{
	m_type = Variant_INT;
	m_value.INT = v;
	m_buf.clear();
}

void Variant::assignFloat( double v)
{
	m_type = Variant_FLOAT;
	m_value.FLOAT = v;
	m_buf.clear();
}

void Variant::assignText( const std::string& v)
{
	m_buf = v;
	m_type = Variant_TEXT;
	m_value.TEXT = m_buf.c_str();
}

unsigned long Variant::getUInt() const
{
	if (m_type == Variant_UINT) return m_value.UINT;
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
}

long Variant::getInt() const
{
	if (m_type == Variant_INT) return m_value.INT;
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
}

double Variant::getFloat() const
{
	if (m_type == Variant_FLOAT) return m_value.FLOAT;
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
}

const char* Variant::getText() const
{
	if (m_type == Variant_TEXT) return m_value.TEXT;
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
}

Document::Document( const Document& o)
	:m_searchIndexTerms(o.m_searchIndexTerms)
	,m_forwardIndexTerms(o.m_forwardIndexTerms)
	,m_metaData(o.m_metaData)
	,m_attributes(o.m_attributes)
	,m_users(o.m_users)
{}

void Document::addSearchIndexTerm(
		const std::string& type_,
		const std::string& value_,
		const Index& position_)
{
	m_searchIndexTerms.push_back( Term( type_,value_,position_));
}

void Document::addForwardIndexTerm(
		const std::string& type_,
		const std::string& value_,
		const Index& position_)
{
	m_forwardIndexTerms.push_back( Term( type_,value_,position_));
}

void Document::setMetaData( const std::string& name_, const Variant& value_)
{
	m_metaData.push_back( MetaData( name_,value_));
}

void Document::setMetaData( const std::string& name_, double value_)
{
	m_metaData.push_back( MetaData( name_,Variant(value_)));
}

void Document::setMetaData( const std::string& name_, int value_)
{
	m_metaData.push_back( MetaData( name_,Variant(value_)));
}

void Document::setMetaData( const std::string& name_, unsigned int value_)
{
	m_metaData.push_back( MetaData( name_,Variant(value_)));
}

void Document::setAttribute( const std::string& name_, const std::string& value_)
{
	m_attributes.push_back( Attribute( name_, value_));
}

void Document::setDocid( const std::string& docid_)
{
	m_docid = docid_;
	m_attributes.push_back( Attribute( "docid", docid_));
}

void Document::setUserAccessRight( const std::string& username_)
{
	m_users.push_back( username_);
}

DocumentAnalyzer::DocumentAnalyzer( const Reference& objbuilder, const Reference& errorhnd, const std::string& segmentername)
	:m_errorhnd_impl(errorhnd)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(ReferenceDeleter<strus::DocumentAnalyzerInterface>::function)
{
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	m_analyzer_impl.reset( objBuilder->createDocumentAnalyzer( segmentername));
	if (!m_analyzer_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to create document analyzer: %s"), errorhnd->fetchError());
	}
}

DocumentAnalyzer::DocumentAnalyzer( const DocumentAnalyzer& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
{}


static strus::DocumentAnalyzerInterface::FeatureOptions getFeatureOptions(
	const std::vector<std::string>& options)
{
	strus::DocumentAnalyzerInterface::FeatureOptions rt;
	std::vector<std::string>::const_iterator
		oi = options.begin(), oe = options.end();
	for (; oi != oe; ++oi)
	{
		if (strus::utils::caseInsensitiveEquals( *oi, "BindPosSucc"))
		{
			rt.definePositionBind( strus::DocumentAnalyzerInterface::FeatureOptions::BindSuccessor);
		}
		else if (strus::utils::caseInsensitiveEquals( *oi, "BindPosPred"))
		{
			rt.definePositionBind( strus::DocumentAnalyzerInterface::FeatureOptions::BindPredecessor);
		}
		else
		{
			throw strus::runtime_error( _TXT( "unknown feature option '%s'"), oi->c_str());
		}
	}
	return rt;
}

static std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > getNormalizers(
		const std::vector<Normalizer>& normalizers,
		const strus::TextProcessorInterface* textproc,
		strus::ErrorBufferInterface* errorhnd)
{
	std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > rt;
	std::vector<Normalizer>::const_iterator
		ni = normalizers.begin(), ne = normalizers.end();
	for (;ni != ne; ++ni)
	{
		const strus::NormalizerFunctionInterface* nf = textproc->getNormalizer( ni->name());
		if (!nf) throw strus::runtime_error( _TXT("failed to get normalizer function '%s': %s"), ni->name().c_str(), errorhnd->fetchError());

		strus::Reference<strus::NormalizerFunctionInstanceInterface> function(
				nf->createInstance( ni->arguments(), textproc));
		if (!function.get()) throw strus::runtime_error( _TXT("failed to create normalizer function instance '%s': %s"), ni->name().c_str(), errorhnd->fetchError());

		rt.push_back( function);
	}
	return rt;
}

static strus::Reference<strus::TokenizerFunctionInstanceInterface> getTokenizer(
		const Tokenizer& tokenizer,
		const strus::TextProcessorInterface* textproc,
		strus::ErrorBufferInterface* errorhnd)
{
	const strus::TokenizerFunctionInterface* tf = textproc->getTokenizer( tokenizer.name());
	if (!tf) throw strus::runtime_error( _TXT("failed to get tokenizer function '%s': %s"), tokenizer.name().c_str(), errorhnd->fetchError());

	strus::Reference<strus::TokenizerFunctionInstanceInterface> rt(
			tf->createInstance( tokenizer.arguments(), textproc));
	if (!rt.get()) throw strus::runtime_error( _TXT("failed to create tokenizer function instance '%s': %s"), tokenizer.name().c_str(), errorhnd->fetchError());

	return rt;
}

struct FeatureFuncDef
{
	std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > normalizers_ref;
	std::vector<strus::NormalizerFunctionInstanceInterface*> normalizers;
	strus::Reference<strus::TokenizerFunctionInstanceInterface> tokenizer;

	FeatureFuncDef( const Reference& objbuilder_impl,
			const Tokenizer& tokenizer_,
			const std::vector<Normalizer>& normalizers_,
			strus::ErrorBufferInterface* errorhnd)
	{
		const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)objbuilder_impl.get();
		const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor object: %s"), errorhnd->fetchError());

		normalizers_ref = getNormalizers( normalizers_, textproc, errorhnd);
		std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) normalizers.push_back( ni->get());
		tokenizer = getTokenizer( tokenizer_, textproc, errorhnd);
	}

	void release()
	{
		(void)tokenizer.release();
		std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) (void)ni->release();
	}
};

void DocumentAnalyzer::addSearchIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers,
	const std::vector<std::string>& options)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addSearchIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::addForwardIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers,
	const std::vector<std::string>& options)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addForwardIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::defineMetaData(
	const std::string& fieldname,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineMetaData(
		fieldname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzer::defineAggregatedMetaData(
	const std::string& fieldname,
	const Aggregator& function)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());

	const strus::AggregatorFunctionInterface* functionimpl = textproc->getAggregator( function.name());
	if (!functionimpl) throw strus::runtime_error( _TXT("failed to get aggregator function '%s': %s"), function.name().c_str(), errorhnd->fetchError());

	strus::Reference<strus::AggregatorFunctionInstanceInterface> functioninst( functionimpl->createInstance( function.arguments()));
	if (!functioninst.get())
	{
		throw strus::runtime_error( _TXT("failed to create aggregator function '%s': %s"), function.name().c_str(), errorhnd->fetchError());
	}
	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAggregatedMetaData(
		fieldname, functioninst.get());
	functioninst.release();
}

void DocumentAnalyzer::defineAttribute(
	const std::string& attribname,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAttribute(
		attribname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

static Variant getNumericVariantFromDouble( double value)
{
	if (value - std::floor( value) < std::numeric_limits<double>::epsilon())
	{
		if (value < 0.0)
		{
			return Variant( (int)(std::floor( value) + std::numeric_limits<double>::epsilon()));
		}
		else
		{
			return Variant( (unsigned int)(std::floor( value) + std::numeric_limits<double>::epsilon()));
		}
	}
	else
	{
		return Variant( value);
	}
}

static strus::ArithmeticVariant arithmeticVariant( const Variant& val)
{
	strus::ArithmeticVariant rt;
	switch (val.type())
	{
		case Variant_UNDEFINED:
			break;
		case Variant_INT:
			rt = (int)val.getInt();
			break;
		case Variant_UINT:
			rt = (unsigned int)val.getUInt();
			break;
		case Variant_FLOAT:
			rt = (float)val.getFloat();
			break;
		case Variant_TEXT:
			if (!rt.initFromString( val.getText()))
			{
				throw strus::runtime_error( _TXT( "failed convert variant to numeric value"));
			}
			break;
	}
	return rt;
}

static Document analyzeDocument( strus::DocumentAnalyzerInterface* THIS, const std::string& content, const strus::DocumentClass& dclass, strus::ErrorBufferInterface* errorhnd)
{
	Document rt;
	strus::analyzer::Document doc = THIS->analyze( content, dclass);

	std::vector<strus::analyzer::Attribute>::const_iterator
		ai = doc.attributes().begin(), ae = doc.attributes().end();
	for (; ai != ae; ++ai)
	{
		if (ai->name() == strus::Constants::attribute_docid())
		{
			rt.setDocid( ai->value());
		}
		else
		{
			rt.setAttribute( ai->name(), ai->value());
		}
	}
	std::vector<strus::analyzer::MetaData>::const_iterator
		mi = doc.metadata().begin(), me = doc.metadata().end();
	for (; mi != me; ++mi)
	{
		Variant val = getNumericVariantFromDouble( mi->value());
		rt.setMetaData( mi->name(), val);
	}
	std::vector<strus::analyzer::Term>::const_iterator
		ti = doc.searchIndexTerms().begin(), te = doc.searchIndexTerms().end();
	for (; ti != te; ++ti)
	{
		rt.addSearchIndexTerm( ti->type(), ti->value(), ti->pos());
	}
	std::vector<strus::analyzer::Term>::const_iterator
		fi = doc.forwardIndexTerms().begin(), fe = doc.forwardIndexTerms().end();
	for (; fi != fe; ++fi)
	{
		rt.addForwardIndexTerm( fi->type(), fi->value(), fi->pos());
	}
	return rt;
}

Document DocumentAnalyzer::analyze( const std::string& content)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	strus::DocumentClass dclass;

	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
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
	return analyzeDocument( THIS, content, dclass, errorhnd);
}

Document DocumentAnalyzer::analyze( const std::string& content, const DocumentClass& dclass)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	strus::DocumentClass documentClass( dclass.mimeType(), dclass.encoding(), dclass.scheme());

	return analyzeDocument( THIS, content, documentClass, errorhnd);
}


QueryAnalyzer::QueryAnalyzer( const Reference& objbuilder, const Reference& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(ReferenceDeleter<strus::QueryAnalyzerInterface>::function)
{
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	m_analyzer_impl.reset( objBuilder->createQueryAnalyzer());
	if (!m_analyzer_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to create query analyzer: %s"), errorhnd->fetchError());
	}
}

QueryAnalyzer::QueryAnalyzer( const QueryAnalyzer& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)

{}

QueryAnalyzeQueue QueryAnalyzer::createQueue() const
{
	return QueryAnalyzeQueue( m_objbuilder_impl, m_errorhnd_impl, m_analyzer_impl);
}

void QueryAnalyzer::definePhraseType(
		const std::string& phraseType,
		const std::string& featureType,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->definePhraseType(
		phraseType, featureType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

std::vector<Term> QueryAnalyzer::analyzePhrase(
		const std::string& phraseType,
		const std::string& phraseContent) const
{
	std::vector<Term> rt;
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	std::vector<strus::analyzer::Term>
		terms = THIS->analyzePhrase( phraseType, phraseContent);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error in analyze phrase: %s"), errorhnd->fetchError());
	}
	std::vector<strus::analyzer::Term>::const_iterator
		ti = terms.begin(), te = terms.end();
	for (; ti != te; ++ti)
	{
		rt.push_back( Term( ti->type(), ti->value(), ti->pos()));
	}
	return rt;
}

QueryAnalyzeQueue::QueryAnalyzeQueue( const QueryAnalyzeQueue& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
	,m_phrase_queue(o.m_phrase_queue)
	,m_result_queue(o.m_result_queue)
	,m_result_queue_idx(o.m_result_queue_idx)
{}

QueryAnalyzeQueue::QueryAnalyzeQueue( const Reference& objbuilder, const Reference& errorhnd, const Reference& analyzer)
	:m_errorhnd_impl(errorhnd)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(analyzer)
	,m_result_queue_idx(0)
{}


void QueryAnalyzeQueue::push(
		const std::string& phraseType,
		const std::string& phraseContent)
{
	m_phrase_queue.push_back( Term( phraseType, phraseContent, 0));
}

std::vector<Term> QueryAnalyzeQueue::fetch()
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (m_result_queue_idx < m_result_queue.size())
	{
		return m_result_queue[ m_result_queue_idx++];
	}
	m_result_queue.clear();

	std::vector<strus::QueryAnalyzerInterface::Phrase> phraseBulk;
	std::vector<Term>::const_iterator pi = m_phrase_queue.begin(), pe = m_phrase_queue.end();
	for (; pi != pe; ++pi)
	{
		phraseBulk.push_back( strus::QueryAnalyzerInterface::Phrase( pi->type(), pi->value()));
	}
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	std::vector<strus::analyzer::TermVector>
		results = THIS->analyzePhraseBulk( phraseBulk);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error in query analyze queue fetch: %s"), errorhnd->fetchError());
	}
	std::vector<strus::analyzer::TermVector>::const_iterator
		ri = results.begin(), re = results.end();
	for (; ri != re; ++ri)
	{
		m_result_queue.push_back( std::vector<Term>());
		std::vector<strus::analyzer::Term>::const_iterator ti = ri->begin(), te = re->end();
		for (; ti != te; ++ti)
		{
			m_result_queue.back().push_back( Term( ti->type(), ti->value(), ti->pos()));
		}
	}
	if (m_result_queue_idx < m_result_queue.size())
	{
		return m_result_queue[ m_result_queue_idx++];
	}
	else
	{
		throw strus::runtime_error( _TXT("no results to fetch from query analyzer queue"));
	}
}


StorageClient::StorageClient( const Reference& objbuilder, const Reference& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_objbuilder_impl( objbuilder)
	,m_storage_impl(ReferenceDeleter<strus::StorageClientInterface>::function)
	,m_transaction_impl(ReferenceDeleter<strus::StorageTransactionInterface>::function)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	m_storage_impl.reset( objBuilder->createStorageClient( config_));
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("failed to create storage client: %s"), errorhnd->fetchError());
}

StorageClient::StorageClient( const StorageClient& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_transaction_impl(ReferenceDeleter<strus::StorageTransactionInterface>::function)
{
	if (o.m_transaction_impl.get())
	{
		throw strus::runtime_error(_TXT("try to create a storage interface clone of a storage with an open transaction"));
	}
}

GlobalCounter StorageClient::nofDocumentsInserted() const
{
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	return THIS->globalNofDocumentsInserted();
}

void StorageClient::insertDocument( const std::string& docid, const Document& doc)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)THIS->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	std::auto_ptr<strus::StorageDocumentInterface> document( transaction->createDocument( docid));
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
		document->setMetaData( mi->name(), arithmeticVariant( mi->value()));
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

void StorageClient::deleteDocument( const std::string& docId)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)THIS->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for deleting document: %s"), errorhnd->fetchError());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteDocument( docId);
}

void StorageClient::deleteUserAccessRights( const std::string& username)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)THIS->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for deleting user access rights: %s"), errorhnd->fetchError());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteUserAccessRights( username);
}

void StorageClient::flush()
{
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	if (transaction)
	{
		if (!transaction->commit())
		{
			m_transaction_impl.reset();
			strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
			throw strus::runtime_error( _TXT("error flushing storage operations: %s"), errorhnd->fetchError());
		}
		m_transaction_impl.reset();
	}
}

void StorageClient::close()
{
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	THIS->close();
}

QueryEval::QueryEval( const Reference& objbuilder, const Reference& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_objbuilder_impl(objbuilder)
	,m_queryeval_impl(ReferenceDeleter<strus::QueryEvalInterface>::function)
{
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	m_queryeval_impl.reset( objBuilder->createQueryEval());
	if (!m_queryeval_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("error creating query eval: %s"), errorhnd->fetchError());
	}
}

QueryEval::QueryEval( const QueryEval& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
{}

void QueryEval::addTerm(
		const std::string& set_,
		const std::string& type_,
		const std::string& value_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addTerm( set_, type_, value_);
}

void QueryEval::addSelectionFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addSelectionFeature( set_);
}

void QueryEval::addRestrictionFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addRestrictionFeature( set_);
}

void QueryEval::addExclusionFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addExclusionFeature( set_);
}

void QueryEval::addSummarizer(
		const std::string& resultAttribute,
		const std::string& name,
		const SummarizerConfig& config)
{
	typedef strus::QueryEvalInterface::FeatureParameter FeatureParameter;

	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
	if (!queryproc) throw strus::runtime_error( _TXT("error in get query processor: %s"), errorhnd->fetchError());

	const strus::SummarizerFunctionInterface* sf = queryproc->getSummarizerFunction( name);
	if (!sf) throw strus::runtime_error( _TXT("summarizer function not defined: '%s'"), name.c_str());

	strus::Reference<strus::SummarizerFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating summarizer function: %s"), errorhnd->fetchError());

	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
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
			function->addNumericParameter( pi->first, arithmeticVariant( pi->second));
		}
	}
	std::vector<FeatureParameter> featureParameters;
	std::map<std::string,std::string>::const_iterator
		fi = config.m_features.begin(), fe = config.m_features.end();
	for (; fi != fe; ++fi)
	{
		featureParameters.push_back( FeatureParameter( fi->first, fi->second));
	}
	queryeval->addSummarizerFunction(
			name, function.get(), featureParameters, resultAttribute);
	function.release();
}

void QueryEval::addWeightingFunction(
		double weight,
		const std::string& name,
		const WeightingConfig& config)
{
	typedef strus::QueryEvalInterface::FeatureParameter FeatureParameter;

	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
	if (!queryproc) throw strus::runtime_error( _TXT("error in get query processor: %s"), errorhnd->fetchError());

	const strus::WeightingFunctionInterface* sf = queryproc->getWeightingFunction( name);
	if (!sf) throw strus::runtime_error( _TXT("weighting function not defined: '%s'"), name.c_str());

	strus::Reference<strus::WeightingFunctionInstanceInterface> function( sf->createInstance());
	if (!function.get()) throw strus::runtime_error( _TXT("error creating weighting function instance '%s': '%s'"), name.c_str(), errorhnd->fetchError());

	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
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
			function->addNumericParameter( pi->first, arithmeticVariant( pi->second));
		}
	}
	std::vector<FeatureParameter> featureParameters;
	std::map<std::string,std::string>::const_iterator
		fi = config.m_features.begin(), fe = config.m_features.end();
	for (; fi != fe; ++fi)
	{
		featureParameters.push_back( FeatureParameter( fi->first, fi->second));
	}
	queryeval->addWeightingFunction( name, function.get(), featureParameters, weight);
	function.release();
}

Query QueryEval::createQuery( const StorageClient& storage) const
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryEvalInterface* qe = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	strus::StorageClientInterface* st = (strus::StorageClientInterface*)storage.m_storage_impl.get();
	Reference query( ReferenceDeleter<strus::QueryInterface>::function);
	query.reset( qe->createQuery( st));
	if (!query.get()) throw strus::runtime_error( _TXT("failed to create query object: %s"), errorhnd->fetchError());

	return Query( m_objbuilder_impl, m_errorhnd_impl, storage.m_storage_impl, m_queryeval_impl, query);
}


Query::Query( const Query& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_query_impl(o.m_query_impl)
{}

void Query::pushTerm( const std::string& type_, const std::string& value_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->pushTerm( type_, value_);
}

void Query::pushExpression( const std::string& opname_, unsigned int argc, int range_, unsigned int cardinality_)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
	if (!queryproc) throw strus::runtime_error( _TXT("failed to get query processor: %s"), errorhnd->fetchError());
	const strus::PostingJoinOperatorInterface* joinopr = queryproc->getPostingJoinOperator( opname_);
	if (!joinopr) throw strus::runtime_error( _TXT("posting join operator not defined: '%s'"), opname_.c_str());

	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->pushExpression( joinopr, argc, range_, cardinality_);
}

void Query::pushDuplicate()
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->pushDuplicate();
}

void Query::attachVariable( const std::string& name_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->attachVariable( name_);
}

void Query::defineFeature( const std::string& set_, double weight_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->defineFeature( set_, weight_);
}

void Query::defineMetaDataRestriction(
		const char* compareOp, const std::string& name,
		const Variant& operand, bool newGroup)
{
	strus::QueryInterface::CompareOperator cmpop;
	if (std::strcmp( compareOp, "<") == 0)
	{
		cmpop = strus::QueryInterface::CompareLess;
	}
	else if (std::strcmp( compareOp, "<=") == 0)
	{
		cmpop = strus::QueryInterface::CompareLessEqual;
	}
	else if (std::strcmp( compareOp, "=") == 0)
	{
		cmpop = strus::QueryInterface::CompareEqual;
	}
	else if (std::strcmp( compareOp, "!=") == 0)
	{
		cmpop = strus::QueryInterface::CompareNotEqual;
	}
	else if (std::strcmp( compareOp, ">") == 0)
	{
		cmpop = strus::QueryInterface::CompareGreater;
	}
	else if (std::strcmp( compareOp, ">=") == 0)
	{
		cmpop = strus::QueryInterface::CompareGreaterEqual;
	}
	else
	{
		throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
	}

	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->defineMetaDataRestriction( cmpop, name, arithmeticVariant(operand), newGroup);
}

void Query::defineMetaDataRestriction(
		const char* compareOp, const std::string& name,
		double value, bool newGroup)
{
	defineMetaDataRestriction( compareOp, name, Variant(value), newGroup);
}

void Query::defineMetaDataRestriction(
		const char* compareOp, const std::string& name,
		unsigned int value, bool newGroup)
{
	defineMetaDataRestriction( compareOp, name, Variant(value), newGroup);
}

void Query::defineMetaDataRestriction(
		const char* compareOp, const std::string& name,
		int value, bool newGroup)
{
	defineMetaDataRestriction( compareOp, name, Variant(value), newGroup);
}

void Query::addDocumentEvaluationSet(
		const std::vector<int>& docnolist_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	std::vector<strus::Index> docnolist;
	std::vector<int>::const_iterator di = docnolist_.begin(), de = docnolist_.end();
	for (; di != de; ++di) docnolist.push_back( *di);
	THIS->addDocumentEvaluationSet( docnolist);
}

void Query::setMaxNofRanks( unsigned int maxNofRanks_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setMaxNofRanks( maxNofRanks_);
}

void Query::setMinRank( unsigned int minRank_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setMinRank( minRank_);
}

void Query::addUserName( const std::string& username_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->addUserName( username_);
}

std::vector<Rank> Query::evaluate() const
{
	std::vector<Rank> rt;
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	std::vector<strus::ResultDocument> res = THIS->evaluate();
	if (res.empty() && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to create query object: %s"), errorhnd->fetchError());
	}
	std::vector<strus::ResultDocument>::const_iterator
		ri = res.begin(), re = res.end();
	for (;ri != re; ++ri)
	{
		Rank reselem;
		reselem.m_docno = (unsigned int)ri->docno();
		reselem.m_weight = ri->weight();
		std::vector<strus::ResultDocument::Attribute>::const_iterator
			ai = ri->attributes().begin(), ae = ri->attributes().end();
	
		for (;ai != ae; ++ai)
		{
			RankAttribute attr;
			attr.m_name = ai->name();
			attr.m_value = ai->value();
			reselem.m_attributes.push_back( attr);
		}
		rt.push_back( reselem);
	}
	return rt;
}


Context::Context( unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<strus::ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
{
	strus::ErrorBufferInterface* errorhnd;
	m_errorhnd_impl.reset( errorhnd=strus::createErrorBuffer_standard( stderr, maxNofThreads));
	if (!m_errorhnd_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create error buffer object: %s"), errorhnd->fetchError());
	}
	m_moduleloader_impl.reset( strus::createModuleLoader( errorhnd));
	if (!m_moduleloader_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create module loader object: %s"), errorhnd->fetchError());
	}
}

Context::Context( const char* connectionstring, unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<strus::ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
{
	strus::ErrorBufferInterface* errorhnd;
	m_errorhnd_impl.reset( errorhnd=strus::createErrorBuffer_standard( stderr, maxNofThreads));
	if (!errorhnd) throw strus::runtime_error(_TXT("failed to create error buffer"));

	std::auto_ptr<strus::RpcClientMessagingInterface> messaging;
	messaging.reset( strus::createRpcClientMessaging( connectionstring, errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( strus::createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

Context::Context( const Context& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_moduleloader_impl(o.m_moduleloader_impl)
	,m_rpc_impl(o.m_rpc_impl)
	,m_storage_objbuilder_impl(o.m_storage_objbuilder_impl)
	,m_analyzer_objbuilder_impl(o.m_analyzer_objbuilder_impl)
{}

void Context::loadModule( const std::string& name_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot load modules in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	if (!moduleLoader->loadModule( name_))
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error(_TXT("failed to load module: %s"), errorhnd->fetchError());
	}
}

void Context::addModulePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a module path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addModulePath( paths_);
}

void Context::addResourcePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a resource path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addResourcePath( paths_);
}

void Context::initStorageObjBuilder()
{
	if (m_rpc_impl.get())
	{
		strus::RpcClientInterface* client = (strus::RpcClientInterface*)m_rpc_impl.get();
		m_storage_objbuilder_impl.reset( client->createStorageObjectBuilder());
	}
	else if (m_moduleloader_impl.get())
	{
		strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
		m_storage_objbuilder_impl.reset( moduleLoader->createStorageObjectBuilder());
	}
	else
	{
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
	}
	if (!m_storage_objbuilder_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error(_TXT("failed to create storage object builder: %s"), errorhnd->fetchError());
	}
}

void Context::initAnalyzerObjBuilder()
{
	if (m_rpc_impl.get())
	{
		strus::RpcClientInterface* client = (strus::RpcClientInterface*)m_rpc_impl.get();
		m_analyzer_objbuilder_impl.reset( client->createAnalyzerObjectBuilder());
	}
	else if (m_moduleloader_impl.get())
	{
		strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
		m_analyzer_objbuilder_impl.reset( moduleLoader->createAnalyzerObjectBuilder());
	}
	else
	{
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
	}
	if (!m_analyzer_objbuilder_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error(_TXT("failed to create analyzer object builder: %s"), errorhnd->fetchError());
	}
}

DocumentClass Context::detectDocumentClass( const std::string& content)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
	strus::DocumentClass dclass;
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
	return StorageClient( m_storage_objbuilder_impl, m_errorhnd_impl, config_);
}

DocumentAnalyzer Context::createDocumentAnalyzer( const std::string& segmentername_)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return DocumentAnalyzer( m_analyzer_objbuilder_impl, m_errorhnd_impl, segmentername_);
}

QueryAnalyzer Context::createQueryAnalyzer()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return QueryAnalyzer( m_analyzer_objbuilder_impl, m_errorhnd_impl);
}

QueryEval Context::createQueryEval()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return QueryEval( m_storage_objbuilder_impl, m_errorhnd_impl);
}

void Context::createStorage( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();

	strus::StorageObjectBuilderInterface* objBuilder = (strus::StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const strus::DatabaseInterface* dbi = objBuilder->getDatabase( config_);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const strus::StorageInterface* sti = objBuilder->getStorage();
	if (!sti) throw strus::runtime_error( _TXT("failed to get storage: %s"), errorhnd->fetchError());

	std::string dbname;
	std::string databasecfg( config_);
	(void)strus::extractStringFromConfigString( dbname, databasecfg, "database", errorhnd);
	std::string storagecfg( databasecfg);

	strus::removeKeysFromConfigString(
			databasecfg,
			sti->getConfigParameters( strus::StorageInterface::CmdCreateClient), errorhnd);
	//... In database_cfg is now the pure database configuration without the storage settings

	strus::removeKeysFromConfigString(
			storagecfg,
			dbi->getConfigParameters( strus::DatabaseInterface::CmdCreateClient), errorhnd);
	//... In storage_cfg is now the pure storage configuration without the database settings
	if (errorhnd->hasError()) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());

	if (!dbi->createDatabase( databasecfg)) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());

	std::auto_ptr<strus::DatabaseClientInterface>
		database( dbi->createClient( databasecfg));
	if (!database.get()) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());

	if (!sti->createStorage( storagecfg, database.get())) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());
}

void Context::destroyStorage( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::StorageObjectBuilderInterface* objBuilder = (strus::StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	if (!objBuilder) throw strus::runtime_error( _TXT("failed to get object builder: %s"), errorhnd->fetchError());
	const strus::DatabaseInterface* dbi = objBuilder->getDatabase( config_);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	if (!dbi->destroyDatabase( config_)) throw strus::runtime_error( _TXT("failed to destroy database: %s"), errorhnd->fetchError());
}

void Context::close()
{
	m_analyzer_objbuilder_impl.reset();
	m_storage_objbuilder_impl.reset();
	if (m_rpc_impl.get()) ((strus::RpcClientInterface*)m_rpc_impl.get())->close();
	m_moduleloader_impl.reset();
}


