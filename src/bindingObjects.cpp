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
#include "strus/lib/queryeval.hpp"
#include "strus/lib/queryproc.hpp"
#include "strus/lib/database_leveldb.hpp"
#include "strus/lib/storage.hpp"
#include "strus/lib/analyzer.hpp"
#include "strus/lib/textprocessor.hpp"
#include "strus/lib/segmenter_textwolf.hpp"
#include "strus/private/arithmeticVariantAsString.hpp"
#include "strus/private/configParser.hpp"
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

Variant::Variant()
	:m_type(EMPTY){}

Variant::Variant( const Variant& o)
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

Variant::Variant( unsigned int v)
	:m_type(UINT)
{
	m_value.UINT = v;
}

Variant::Variant( int v)
	:m_type(INT)
{
	m_value.INT = v;
}

Variant::Variant( float v)
	:m_type(FLOAT)
{
	m_value.FLOAT = v;
}

Variant::Variant( double v)
	:m_type(FLOAT)
{
	m_value.FLOAT = (float)v;
}

Variant::Variant( const char* v)
	:m_type(TEXT),m_buf(v)
{
	m_value.TEXT = m_buf.c_str();
}

Variant::Variant( const std::string& v)
	:m_type(TEXT),m_buf(v)
{
	m_value.TEXT = m_buf.c_str();
}

unsigned int Variant::getUInt() const
{
	if (m_type == UINT) return m_value.UINT;
	throw std::logic_error( "illegal access of variant value");
}

int Variant::getInt() const
{
	if (m_type == INT) return m_value.INT;
	throw std::logic_error( "illegal access of variant value");
}

float Variant::getFloat() const
{
	if (m_type == FLOAT) return m_value.FLOAT;
	throw std::logic_error( "illegal access of variant value");
}

const char* Variant::getText() const
{
	if (m_type == TEXT) return m_value.TEXT;
	throw std::logic_error( "illegal access of variant value");
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

void Document::setMetaData( const std::string& name_, Variant value_)
{
	m_metaData.push_back( MetaData( name_,value_));
}

void Document::setAttribute( const std::string& name_, const std::string& value_)
{
	m_attributes.push_back( Attribute( name_, value_));
}

void Document::setUserAccessRight( const std::string& username_)
{
	m_users.push_back( username_);
}

DocumentAnalyzer::DocumentAnalyzer()
	:m_textproc_impl(ReferenceDeleter<strus::TextProcessorInterface>::function)
	,m_analyzer_impl(ReferenceDeleter<strus::DocumentAnalyzerInterface>::function)
{
	std::auto_ptr<strus::SegmenterInterface>
		segmenter( strus::createSegmenter_textwolf());
	std::auto_ptr<strus::TextProcessorInterface>
		textproc( strus::createTextProcessor());

	m_analyzer_impl.reset( strus::createDocumentAnalyzer( textproc.get(), segmenter.get()));
	m_textproc_impl.reset( textproc.release());
	(void)segmenter.release();
}

DocumentAnalyzer::DocumentAnalyzer( const DocumentAnalyzer& o)
	:m_textproc_impl(o.m_textproc_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
{}

void DocumentAnalyzer::addSearchIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const FunctionDef& tokenizer,
	const FunctionDef& normalizer)
{
	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addSearchIndexFeature(
		type, selectexpr,
		strus::TokenizerConfig( tokenizer.name(), tokenizer.arguments()), 
		strus::NormalizerConfig( normalizer.name(), normalizer.arguments()));
}

void DocumentAnalyzer::addForwardIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const FunctionDef& tokenizer,
	const FunctionDef& normalizer)
{
	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addForwardIndexFeature(
		type, selectexpr,
		strus::TokenizerConfig( tokenizer.name(), tokenizer.arguments()), 
		strus::NormalizerConfig( normalizer.name(), normalizer.arguments()));
}

void DocumentAnalyzer::defineMetaData(
	const std::string& fieldname,
	const std::string& selectexpr,
	const FunctionDef& tokenizer,
	const FunctionDef& normalizer)
{
	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineMetaData(
		fieldname, selectexpr,
		strus::TokenizerConfig( tokenizer.name(), tokenizer.arguments()), 
		strus::NormalizerConfig( normalizer.name(), normalizer.arguments()));
}

void DocumentAnalyzer::defineAttribute(
	const std::string& attribname,
	const std::string& selectexpr,
	const FunctionDef& tokenizer,
	const FunctionDef& normalizer)
{
	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAttribute(
		attribname, selectexpr,
		strus::TokenizerConfig( tokenizer.name(), tokenizer.arguments()), 
		strus::NormalizerConfig( normalizer.name(), normalizer.arguments()));
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
		case Variant::EMPTY:
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

Document DocumentAnalyzer::analyze( const std::string& content)
{
	Document rt;
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();

	strus::analyzer::Document doc = THIS->analyze( content);

	std::vector<strus::analyzer::Attribute>::const_iterator
		ai = doc.attributes().begin(), ae = doc.attributes().end();
	for (; ai != ae; ++ai)
	{
		rt.setAttribute( ai->name(), ai->value());
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


QueryAnalyzer::QueryAnalyzer()
	:m_textproc_impl(ReferenceDeleter<strus::TextProcessorInterface>::function)
	,m_analyzer_impl(ReferenceDeleter<strus::QueryAnalyzerInterface>::function)
{
	std::auto_ptr<strus::TextProcessorInterface> textproc( strus::createTextProcessor());

	m_analyzer_impl.reset( strus::createQueryAnalyzer( textproc.get()));
	m_textproc_impl.reset( textproc.release());
}

QueryAnalyzer::QueryAnalyzer( const QueryAnalyzer& o)
	:m_textproc_impl(o.m_textproc_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
{}

void QueryAnalyzer::definePhraseType(
		const std::string& phraseType,
		const std::string& featureType,
		const FunctionDef& tokenizer,
		const FunctionDef& normalizer)
{
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	THIS->definePhraseType(
		phraseType, featureType,
		strus::TokenizerConfig( tokenizer.name(), tokenizer.arguments()),
		strus::NormalizerConfig( normalizer.name(), normalizer.arguments()));
}

std::vector<Term> QueryAnalyzer::analyzePhrase(
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


Storage::Storage( const std::string& config)
	:m_database_impl(ReferenceDeleter<strus::DatabaseInterface>::function)
	,m_storage_impl(ReferenceDeleter<strus::StorageInterface>::function)
	,m_queryproc_impl(ReferenceDeleter<strus::QueryProcessorInterface>::function)
	,m_transaction_impl(ReferenceDeleter<strus::StorageTransactionInterface>::function)
{
	std::string databasecfg( config);
	std::string storagecfg( config);

	strus::removeKeysFromConfigString(
			databasecfg,
			strus::getStorageConfigParameters( strus::CmdCreateStorageClient));
	//... In database_cfg is now the pure database configuration without the storage settings

	strus::removeKeysFromConfigString(
			storagecfg,
			strus::getDatabaseConfigParameters_leveldb( strus::CmdCreateDatabaseClient));
	//... In storage_cfg is now the pure storage configuration without the database settings

	std::auto_ptr<strus::DatabaseInterface>
		database( strus::createDatabaseClient_leveldb( databasecfg));
	std::auto_ptr<strus::StorageInterface>
		storage( strus::createStorageClient( storagecfg, database.get()));
	std::auto_ptr<strus::QueryProcessorInterface>
		queryproc( strus::createQueryProcessor( storage.get()));

	m_database_impl.reset( database.release());
	m_storage_impl.reset( storage.release());
	m_queryproc_impl.reset( queryproc.release());
}

Storage::Storage( const Storage& o)
	:m_database_impl(o.m_database_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_queryproc_impl(o.m_queryproc_impl)
	,m_transaction_impl(ReferenceDeleter<strus::StorageTransactionInterface>::function)
{
	if (o.m_transaction_impl.get())
	{
		throw std::runtime_error("try to create a storage interface clone of a storage with an open transaction");
	}
}

GlobalCounter Storage::nofDocumentsInserted() const
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_storage_impl.get();
	return THIS->globalNofDocumentsInserted();
}

void Storage::insertDocument( const std::string& docid, const Document& doc)
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_storage_impl.get();
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

void Storage::deleteDocument( const std::string& docId)
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)THIS->createTransaction());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteDocument( docId);
}

void Storage::deleteUserAccessRights( const std::string& username)
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)THIS->createTransaction());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteUserAccessRights( username);
}

void Storage::flush()
{
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->commit();
	m_transaction_impl.reset();
}

void Storage::create( const char* config)
{
	std::string databasecfg( config);
	std::string storagecfg( config);

	strus::removeKeysFromConfigString(
			databasecfg,
			strus::getStorageConfigParameters( strus::CmdCreateStorageClient));
	//... In database_cfg is now the pure database configuration without the storage settings

	strus::removeKeysFromConfigString(
			storagecfg,
			strus::getDatabaseConfigParameters_leveldb( strus::CmdCreateDatabaseClient));
	//... In storage_cfg is now the pure storage configuration without the database settings

	strus::createDatabase_leveldb( databasecfg);

	std::auto_ptr<strus::DatabaseInterface>
		database( strus::createDatabaseClient_leveldb( databasecfg));

	strus::createStorage( storagecfg, database.get());
}

void Storage::destroy( const char* config)
{
	strus::destroyDatabase_leveldb( config);
}

void Storage::close()
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_storage_impl.get();
	THIS->close();
}


QueryEval::QueryEval( const Storage& storage)
	:m_database_impl(storage.m_database_impl)
	,m_storage_impl(storage.m_storage_impl)
	,m_queryproc_impl(storage.m_queryproc_impl)
	,m_queryeval_impl(ReferenceDeleter<strus::QueryEvalInterface>::function)
{
	m_queryeval_impl.reset( strus::createQueryEval( (strus::QueryProcessorInterface*)m_queryproc_impl.get()));
}

QueryEval::QueryEval( const QueryEval& o)
	:m_database_impl(o.m_database_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_queryproc_impl(o.m_queryproc_impl)
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

void QueryEval::addWeightingFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addWeightingFeature( set_);
}

void QueryEval::addSummarizer(
		const std::string& resultAttribute,
		const Summarizer& summarizer)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	strus::SummarizerConfig config;
	std::map<std::string,Variant>::const_iterator
		pi = summarizer.m_parameters.begin(), pe = summarizer.m_parameters.end();
	for (; pi != pe; ++pi)
	{
		if (pi->second.m_type == Variant::TEXT)
		{
			config.defineTextualParameter( pi->first, pi->second.m_value.TEXT);
		}
		else
		{
			config.defineNumericParameter( pi->first, arithmeticVariant( pi->second));
		}
	}
	std::map<std::string,std::string>::const_iterator
		fi = summarizer.m_features.begin(), fe = summarizer.m_features.end();
	for (; fi != fe; ++fi)
	{
		config.defineFeatureParameter( fi->first, fi->second);
	}
	queryeval->addSummarizer( resultAttribute, summarizer.m_name, config);
}

void QueryEval::defineWeightingFunction(
		const WeightingFunction& weightingFunction)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	strus::WeightingConfig config;
	std::map<std::string,Variant>::const_iterator
		pi = weightingFunction.m_parameters.begin(), pe = weightingFunction.m_parameters.end();
	for (; pi != pe; ++pi)
	{
		config.defineNumericParameter( pi->first, arithmeticVariant( pi->second));
	}
	queryeval->setWeighting( weightingFunction.m_name, config);
}


Query::Query( const QueryEval& queryeval)
	:m_database_impl(queryeval.m_database_impl)
	,m_storage_impl(queryeval.m_storage_impl)
	,m_queryeval_impl(queryeval.m_queryeval_impl)
	,m_queryproc_impl(queryeval.m_queryproc_impl)
	,m_query_impl( ReferenceDeleter<strus::QueryInterface>::function)
{
	strus::QueryEvalInterface* qe = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	strus::StorageInterface* st = (strus::StorageInterface*)m_storage_impl.get();
	m_query_impl.reset( qe->createQuery( st));
}

Query::Query( const Query& o)
	:m_database_impl(o.m_database_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_queryproc_impl(o.m_queryproc_impl)
	,m_query_impl(o.m_query_impl)
{}

void Query::pushTerm( const std::string& type_, const std::string& value_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->pushTerm( type_, value_);
}

void Query::pushExpression( const std::string& opname_, std::size_t argc, int range_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->pushExpression( opname_, argc, range_);
}

void Query::defineFeature( const std::string& set_, float weight_)
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
		throw std::runtime_error( std::string("unknown compare operator '") + compareOp + "', expected one of '=','!=','>','<','<=','>='");
	}

	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->defineMetaDataRestriction( cmpop, name, arithmeticVariant(operand), newGroup);
}

void Query::setMaxNofRanks( std::size_t maxNofRanks_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setMaxNofRanks( maxNofRanks_);
}

void Query::setMinRank( std::size_t minRank_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setMinRank( minRank_);
}

void Query::setUserName( const std::string& username_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setUserName( username_);
}

std::vector<Rank> Query::evaluate() const
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


