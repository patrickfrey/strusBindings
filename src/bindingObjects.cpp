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
#include "strus/strus.hpp"
#include "strus/lib/rpc_client.hpp"
#include "strus/lib/rpc_client_socket.hpp"
#include "strus/lib/module.hpp"
#include "strus/rpcClientInterface.hpp"
#include "strus/rpcClientMessagingInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/private/arithmeticVariantAsString.hpp"
#include "strus/private/configParser.hpp"
#include "private/dll_tags.hpp"
#include "utils.hpp"
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

DLL_PUBLIC Variant::Variant()
	:m_type(UNDEFINED){}

DLL_PUBLIC Variant::Variant( const Variant& o)
	:m_type(o.m_type),m_buf(o.m_buf)
{
	if (m_type == TEXT)
	{
		m_value.TEXT = m_buf.c_str();
	}
	else
	{
		std::memcpy( &m_value, &o.m_value, sizeof(m_value));
	}
}

DLL_PUBLIC Variant::Variant( unsigned int v)
	:m_type(UINT)
{
	m_value.UINT = v;
}

DLL_PUBLIC Variant::Variant( int v)
	:m_type(INT)
{
	m_value.INT = v;
}

DLL_PUBLIC Variant::Variant( float v)
	:m_type(FLOAT)
{
	m_value.FLOAT = v;
}

DLL_PUBLIC Variant::Variant( double v)
	:m_type(FLOAT)
{
	m_value.FLOAT = (float)v;
}

DLL_PUBLIC Variant::Variant( const std::string& v)
	:m_type(TEXT),m_buf(v)
{
	m_value.TEXT = m_buf.c_str();
}

DLL_PUBLIC Variant::Variant( const char* v)
	:m_type(TEXT),m_buf(v?v:"")
{
	m_value.TEXT = m_buf.c_str();
}

DLL_PUBLIC void Variant::init()
{
	m_type = UNDEFINED;
	m_value.INT = 0;
	m_buf.clear();
}

DLL_PUBLIC void Variant::assign( const Variant& o)
{
	m_type = o.m_type;
	m_buf = o.m_buf;
	if (m_type == TEXT)
	{
		m_value.TEXT = m_buf.c_str();
	}
	else
	{
		std::memcpy( &m_value, &o.m_value, sizeof(m_value));
	}
}

DLL_PUBLIC void Variant::assign( unsigned int v)
{
	m_type = UINT;
	m_value.UINT = v;
	m_buf.clear();
}

DLL_PUBLIC void Variant::assign( int v)
{
	m_type = INT;
	m_value.INT = v;
	m_buf.clear();
}

DLL_PUBLIC void Variant::assign( float v)
{
	m_type = FLOAT;
	m_value.FLOAT = v;
	m_buf.clear();
}

DLL_PUBLIC void Variant::assign( double v)
{
	m_type = FLOAT;
	m_value.FLOAT = v;
	m_buf.clear();
}

DLL_PUBLIC void Variant::assign( const std::string& v)
{
	m_buf = v;
	m_type = TEXT;
	m_value.TEXT = m_buf.c_str();
}

DLL_PUBLIC void Variant::assign( const char* v)
{
	m_type = TEXT;
	m_buf = v?v:"";
	m_value.TEXT = m_buf.c_str();
}

DLL_PUBLIC unsigned int Variant::getUInt() const
{
	if (m_type == UINT) return m_value.UINT;
	throw std::logic_error( "illegal access of variant value");
}

DLL_PUBLIC int Variant::getInt() const
{
	if (m_type == INT) return m_value.INT;
	throw std::logic_error( "illegal access of variant value");
}

DLL_PUBLIC float Variant::getFloat() const
{
	if (m_type == FLOAT) return m_value.FLOAT;
	throw std::logic_error( "illegal access of variant value");
}

DLL_PUBLIC const char* Variant::getText() const
{
	if (m_type == TEXT) return m_value.TEXT;
	throw std::logic_error( "illegal access of variant value");
}

DLL_PUBLIC Document::Document( const Document& o)
	:m_searchIndexTerms(o.m_searchIndexTerms)
	,m_forwardIndexTerms(o.m_forwardIndexTerms)
	,m_metaData(o.m_metaData)
	,m_attributes(o.m_attributes)
	,m_users(o.m_users)
{}

DLL_PUBLIC void Document::addSearchIndexTerm(
		const std::string& type_,
		const std::string& value_,
		const Index& position_)
{
	m_searchIndexTerms.push_back( Term( type_,value_,position_));
}

DLL_PUBLIC void Document::addForwardIndexTerm(
		const std::string& type_,
		const std::string& value_,
		const Index& position_)
{
	m_forwardIndexTerms.push_back( Term( type_,value_,position_));
}

DLL_PUBLIC void Document::setMetaData( const std::string& name_, const Variant& value_)
{
	m_metaData.push_back( MetaData( name_,value_));
}

DLL_PUBLIC void Document::setAttribute( const std::string& name_, const std::string& value_)
{
	m_attributes.push_back( Attribute( name_, value_));
}

DLL_PUBLIC void Document::setDocid( const std::string& docid_)
{
	m_docid = docid_;
	m_attributes.push_back( Attribute( "docid", docid_));
}

DLL_PUBLIC void Document::setUserAccessRight( const std::string& username_)
{
	m_users.push_back( username_);
}

DLL_PUBLIC DocumentAnalyzer::DocumentAnalyzer( const Reference& objbuilder, const std::string& segmentername)
	:m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(ReferenceDeleter<strus::DocumentAnalyzerInterface>::function)
{
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	m_analyzer_impl.reset( objBuilder->createDocumentAnalyzer( segmentername));
}

DLL_PUBLIC DocumentAnalyzer::DocumentAnalyzer( const DocumentAnalyzer& o)
	:m_objbuilder_impl(o.m_objbuilder_impl)
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
			throw std::runtime_error( std::string( "unknown feature option '") + *oi + "'");
		}
	}
	return rt;
}

static std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > getNormalizers(
		const std::vector<Normalizer>& normalizers,
		const strus::TextProcessorInterface* textproc)
{
	std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > rt;
	std::vector<Normalizer>::const_iterator
		ni = normalizers.begin(), ne = normalizers.end();
	for (;ni != ne; ++ni)
	{
		const strus::NormalizerFunctionInterface* nf = textproc->getNormalizer( ni->name());
		strus::Reference<strus::NormalizerFunctionInstanceInterface> function(
				nf->createInstance( ne->arguments(), textproc));
		
		rt.push_back( function);
	}
	return rt;
}

static strus::Reference<strus::TokenizerFunctionInstanceInterface> getTokenizer(
		const Tokenizer& tokenizer,
		const strus::TextProcessorInterface* textproc)
{
	const strus::TokenizerFunctionInterface* nf = textproc->getTokenizer( tokenizer.name());
	strus::Reference<strus::TokenizerFunctionInstanceInterface> rt(
			nf->createInstance( tokenizer.arguments(), textproc));
	return rt;
}

struct FeatureFuncDef
{
	std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > normalizers_ref;
	std::vector<strus::NormalizerFunctionInstanceInterface*> normalizers;
	strus::Reference<strus::TokenizerFunctionInstanceInterface> tokenizer;

	FeatureFuncDef( const Reference& m_objbuilder_impl,
			const Tokenizer& tokenizer_,
			const std::vector<Normalizer>& normalizers_)
	{
		const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
		const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		normalizers_ref = getNormalizers( normalizers_, textproc);
		std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) normalizers.push_back( ni->get());
		tokenizer = getTokenizer( tokenizer_, textproc);
	}

	void release()
	{
		(void)tokenizer.release();
		std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) (void)ni->release();
	}
};

DLL_PUBLIC void DocumentAnalyzer::addSearchIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers,
	const std::vector<std::string>& options)
{
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addSearchIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

DLL_PUBLIC void DocumentAnalyzer::addForwardIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers,
	const std::vector<std::string>& options)
{
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addForwardIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

DLL_PUBLIC void DocumentAnalyzer::defineMetaData(
	const std::string& fieldname,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers)
{
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineMetaData(
		fieldname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

DLL_PUBLIC void DocumentAnalyzer::defineAttribute(
	const std::string& attribname,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers)
{
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAttribute(
		attribname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

static Variant getNumericVariantFromString( const std::string& value)
{
	strus::ArithmeticVariant val( strus::arithmeticVariantFromString( value));
	switch (val.type)
	{
		case strus::ArithmeticVariant::Null:
			return Variant();
		case strus::ArithmeticVariant::Int:
			return Variant( (int)val);
		case strus::ArithmeticVariant::UInt:
			return Variant( (unsigned int)val);
		case strus::ArithmeticVariant::Float:
			return Variant( (float)val);
	}
	throw std::runtime_error( std::string("cannot create numeric variant type from string: '") + value + "'");
}

static strus::ArithmeticVariant arithmeticVariant( const Variant& val)
{
	strus::ArithmeticVariant rt;
	switch (val.type())
	{
		case Variant::UNDEFINED:
			break;
		case Variant::INT:
			rt = val.getInt();
			break;
		case Variant::UINT:
			rt = val.getUInt();
			break;
		case Variant::FLOAT:
			rt = val.getFloat();
			break;
		case Variant::TEXT:
			rt = strus::arithmeticVariantFromString( val.getText());
			break;
	}
	return rt;
}

DLL_PUBLIC Document DocumentAnalyzer::analyze( const std::string& content)
{
	Document rt;
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();

	strus::analyzer::Document doc = THIS->analyze( content);

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
		Variant val = getNumericVariantFromString( mi->value());
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


DLL_PUBLIC QueryAnalyzer::QueryAnalyzer( const Reference& objbuilder)
	:m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(ReferenceDeleter<strus::QueryAnalyzerInterface>::function)
{
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	m_analyzer_impl.reset( objBuilder->createQueryAnalyzer());
}

DLL_PUBLIC QueryAnalyzer::QueryAnalyzer( const QueryAnalyzer& o)
	:m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
{}

DLL_PUBLIC void QueryAnalyzer::definePhraseType(
		const std::string& phraseType,
		const std::string& featureType,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers)
{
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers);

	THIS->definePhraseType(
		phraseType, featureType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

DLL_PUBLIC std::vector<Term> QueryAnalyzer::analyzePhrase(
		const std::string& phraseType,
		const std::string& phraseContent) const
{
	std::vector<Term> rt;
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	std::vector<strus::analyzer::Term>
		terms = THIS->analyzePhrase( phraseType, phraseContent);
	std::vector<strus::analyzer::Term>::const_iterator
		ti = terms.begin(), te = terms.end();
	for (; ti != te; ++ti)
	{
		rt.push_back( Term( ti->type(), ti->value(), ti->pos()));
	}
	return rt;
}


DLL_PUBLIC StorageClient::StorageClient( const Reference& objbuilder, const std::string& config_)
	:m_objbuilder_impl( objbuilder)
	,m_storage_impl(ReferenceDeleter<strus::StorageClientInterface>::function)
	,m_transaction_impl(ReferenceDeleter<strus::StorageTransactionInterface>::function)
{
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	m_storage_impl.reset( objBuilder->createStorageClient( config_));
}

DLL_PUBLIC StorageClient::StorageClient( const StorageClient& o)
	:m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_transaction_impl(ReferenceDeleter<strus::StorageTransactionInterface>::function)
{
	if (o.m_transaction_impl.get())
	{
		throw std::runtime_error("try to create a storage interface clone of a storage with an open transaction");
	}
}

DLL_PUBLIC GlobalCounter StorageClient::nofDocumentsInserted() const
{
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	return THIS->globalNofDocumentsInserted();
}

DLL_PUBLIC void StorageClient::insertDocument( const std::string& docid, const Document& doc)
{
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)THIS->createTransaction());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	std::auto_ptr<strus::StorageDocumentInterface> document( transaction->createDocument( docid));

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
		document->addSearchIndexTerm( ti->name(), ti->value(), ti->position());
	}
	std::vector<Term>::const_iterator
		fi = doc.forwardIndexTerms().begin(), fe = doc.forwardIndexTerms().end();
	for (; fi != fe; ++fi)
	{
		document->addForwardIndexTerm( fi->name(), fi->value(), fi->position());
	}
	std::vector<std::string>::const_iterator
		ui = doc.users().begin(), ue = doc.users().end();
	for (; ui != ue; ++ui)
	{
		document->setUserAccessRight( *ui);
	}
	document->done();
}

DLL_PUBLIC void StorageClient::deleteDocument( const std::string& docId)
{
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)THIS->createTransaction());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteDocument( docId);
}

DLL_PUBLIC void StorageClient::deleteUserAccessRights( const std::string& username)
{
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)THIS->createTransaction());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteUserAccessRights( username);
}

DLL_PUBLIC void StorageClient::flush()
{
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	if (transaction)
	{
		transaction->commit();
		m_transaction_impl.reset();
	}
}

DLL_PUBLIC void StorageClient::close()
{
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	THIS->close();
}

DLL_PUBLIC QueryEval::QueryEval( const Reference& objbuilder)
	:m_objbuilder_impl(objbuilder)
	,m_queryeval_impl(ReferenceDeleter<strus::QueryEvalInterface>::function)
{
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	m_queryeval_impl.reset( objBuilder->createQueryEval());
}

DLL_PUBLIC QueryEval::QueryEval( const QueryEval& o)
	:m_objbuilder_impl(o.m_objbuilder_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
{}

DLL_PUBLIC void QueryEval::addTerm(
		const std::string& set_,
		const std::string& type_,
		const std::string& value_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addTerm( set_, type_, value_);
}

DLL_PUBLIC void QueryEval::addSelectionFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addSelectionFeature( set_);
}

DLL_PUBLIC void QueryEval::addRestrictionFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addRestrictionFeature( set_);
}

DLL_PUBLIC void QueryEval::addSummarizer(
		const std::string& resultAttribute,
		const Summarizer& summarizer)
{
	typedef strus::QueryEvalInterface::SummarizerFeatureParameter SummarizerFeatureParameter;

	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
	const strus::SummarizerFunctionInterface* sf = queryproc->getSummarizerFunction( summarizer.m_name);
	strus::Reference<strus::SummarizerFunctionInstanceInterface> function( sf->createInstance( queryproc));

	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	std::map<std::string,Variant>::const_iterator
		pi = summarizer.m_parameters.begin(), pe = summarizer.m_parameters.end();
	for (; pi != pe; ++pi)
	{
		if (pi->second.m_type == Variant::TEXT)
		{
			function->addStringParameter( pi->first, pi->second.m_value.TEXT);
		}
		else
		{
			function->addNumericParameter( pi->first, arithmeticVariant( pi->second));
		}
	}
	std::vector<SummarizerFeatureParameter> featureParameters;
	std::map<std::string,std::string>::const_iterator
		fi = summarizer.m_features.begin(), fe = summarizer.m_features.end();
	for (; fi != fe; ++fi)
	{
		featureParameters.push_back( SummarizerFeatureParameter( fi->first, fi->second));
	}
	queryeval->addSummarizerFunction(
			summarizer.m_name, function.get(), featureParameters, resultAttribute);
	function.release();
}

DLL_PUBLIC void QueryEval::addWeightingFunction(
		const WeightingFunction& weightingFunction,
		const std::vector<std::string>& weightingFeatureSets)
{
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
	const strus::WeightingFunctionInterface* sf = queryproc->getWeightingFunction( weightingFunction.m_name);
	strus::Reference<strus::WeightingFunctionInstanceInterface> function( sf->createInstance());

	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	std::map<std::string,Variant>::const_iterator
		pi = weightingFunction.m_parameters.begin(), pe = weightingFunction.m_parameters.end();
	for (; pi != pe; ++pi)
	{
		function->addNumericParameter( pi->first, arithmeticVariant( pi->second));
	}
	queryeval->addWeightingFunction( weightingFunction.m_name, function.get(), weightingFeatureSets);
	function.release();
}


DLL_PUBLIC Query::Query( const QueryEval& queryeval, const StorageClient& storage)
	:m_objbuilder_impl(queryeval.m_objbuilder_impl)
	,m_storage_impl(storage.m_storage_impl)
	,m_queryeval_impl(queryeval.m_queryeval_impl)
	,m_query_impl( ReferenceDeleter<strus::QueryInterface>::function)
{
	strus::QueryEvalInterface* qe = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	strus::StorageClientInterface* st = (strus::StorageClientInterface*)m_storage_impl.get();
	m_query_impl.reset( qe->createQuery( st));
}

DLL_PUBLIC Query::Query( const Query& o)
	:m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_query_impl(o.m_query_impl)
{}

DLL_PUBLIC void Query::pushTerm( const std::string& type_, const std::string& value_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->pushTerm( type_, value_);
}

DLL_PUBLIC void Query::pushExpression( const std::string& opname_, unsigned int argc, int range_)
{
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
	const strus::PostingJoinOperatorInterface* joinopr = queryproc->getPostingJoinOperator( opname_);
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->pushExpression( joinopr, argc, range_);
}

DLL_PUBLIC void Query::attachVariable( const std::string& name_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->attachVariable( name_);
}

DLL_PUBLIC void Query::defineFeature( const std::string& set_, float weight_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->defineFeature( set_, weight_);
}

DLL_PUBLIC void Query::defineMetaDataRestriction(
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
		throw std::runtime_error( std::string("unknown compare operator '") + compareOp + "', expected one of '=','!=','>','<','<=','>='");
	}

	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->defineMetaDataRestriction( cmpop, name, arithmeticVariant(operand), newGroup);
}

DLL_PUBLIC void Query::setMaxNofRanks( unsigned int maxNofRanks_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setMaxNofRanks( maxNofRanks_);
}

DLL_PUBLIC void Query::setMinRank( unsigned int minRank_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setMinRank( minRank_);
}

DLL_PUBLIC void Query::setUserName( const std::string& username_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setUserName( username_);
}

DLL_PUBLIC std::vector<Rank> Query::evaluate() const
{
	std::vector<Rank> rt;
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	std::vector<strus::ResultDocument> res = THIS->evaluate();

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


DLL_PUBLIC StrusContext::StrusContext()
	:m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
{
	m_moduleloader_impl.reset( strus::createModuleLoader());
}

DLL_PUBLIC StrusContext::StrusContext( const char* connectionstring)
	:m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
{
	std::auto_ptr<strus::RpcClientMessagingInterface> messaging;
	messaging.reset( strus::createRpcClientMessaging( connectionstring));
	m_rpc_impl.reset( strus::createRpcClient( messaging.get()));
	(void)messaging.release();
}

DLL_PUBLIC StrusContext::StrusContext( const StrusContext& o)
	:m_moduleloader_impl(o.m_moduleloader_impl)
	,m_rpc_impl(o.m_rpc_impl)
	,m_storage_objbuilder_impl(o.m_storage_objbuilder_impl)
	,m_analyzer_objbuilder_impl(o.m_analyzer_objbuilder_impl)
{}

DLL_PUBLIC void StrusContext::loadModule( const std::string& name_)
{
	if (!m_moduleloader_impl.get()) throw std::runtime_error( "cannot load modules in RPC client mode");
	if (m_storage_objbuilder_impl.get()) throw std::runtime_error( "tried to load modules after the first use of objects");
	if (m_analyzer_objbuilder_impl.get()) throw std::runtime_error( "tried to load modules after the first use of objects");
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->loadModule( name_);
}

DLL_PUBLIC void StrusContext::addModulePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw std::runtime_error( "cannot add a module path in RPC client mode");
	if (m_storage_objbuilder_impl.get()) throw std::runtime_error( "tried to set the module search path after the first use of objects");
	if (m_analyzer_objbuilder_impl.get()) throw std::runtime_error( "tried to set the module search path after the first use of objects");
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addModulePath( paths_);
}

DLL_PUBLIC void StrusContext::addResourcePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw std::runtime_error( "cannot add a resource path in RPC client mode");
	if (m_storage_objbuilder_impl.get()) throw std::runtime_error( "tried to load modules after the first use of objects");
	if (m_analyzer_objbuilder_impl.get()) throw std::runtime_error( "tried to load modules after the first use of objects");
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addResourcePath( paths_);
}

void StrusContext::initStorageObjBuilder()
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
		throw std::runtime_error( "bad state, no context initialized");
	}
}

void StrusContext::initAnalyzerObjBuilder()
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
		throw std::runtime_error( "bad state, no context initialized");
	}
}

DLL_PUBLIC StorageClient StrusContext::createStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, config_);
}

DLL_PUBLIC DocumentAnalyzer StrusContext::createDocumentAnalyzer( const std::string& segmentername_)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return DocumentAnalyzer( m_analyzer_objbuilder_impl, segmentername_);
}

DLL_PUBLIC QueryAnalyzer StrusContext::createQueryAnalyzer()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return QueryAnalyzer( m_analyzer_objbuilder_impl);
}

DLL_PUBLIC QueryEval StrusContext::createQueryEval()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return QueryEval( m_storage_objbuilder_impl);
}

DLL_PUBLIC void StrusContext::createStorage( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	strus::StorageObjectBuilderInterface* objBuilder = (strus::StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const strus::DatabaseInterface* dbi = objBuilder->getDatabase( config_);
	const strus::StorageInterface* sti = objBuilder->getStorage();

	std::string dbname;
	std::string databasecfg( config_);
	(void)strus::extractStringFromConfigString( dbname, databasecfg, "database");
	std::string storagecfg( databasecfg);

	strus::removeKeysFromConfigString(
			databasecfg,
			sti->getConfigParameters( strus::StorageInterface::CmdCreateClient));
	//... In database_cfg is now the pure database configuration without the storage settings

	strus::removeKeysFromConfigString(
			storagecfg,
			dbi->getConfigParameters( strus::DatabaseInterface::CmdCreateClient));
	//... In storage_cfg is now the pure storage configuration without the database settings

	dbi->createDatabase( databasecfg);

	std::auto_ptr<strus::DatabaseClientInterface>
		database( dbi->createClient( databasecfg));

	sti->createStorage( storagecfg, database.get());
}

DLL_PUBLIC void StrusContext::destroyStorage( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	strus::StorageObjectBuilderInterface* objBuilder = (strus::StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const strus::DatabaseInterface* dbi = objBuilder->getDatabase( config_);
	dbi->destroyDatabase( config_);
}


