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
#include "strus/analyzerInterface.hpp"
#include "strus/analyzerLib.hpp"
#include "strus/tokenMinerLib.hpp"
#include "strus/tokenMinerFactory.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <boost/scoped_ptr.hpp>

Document::Document( const std::string& docId_)
	:m_id(docId_)
{}

Document::Document( const Analyzer& analyzer, const std::string& docId_, const std::string& content)
	:m_id(docId_)
{
	strus::AnalyzerInterface* alz = (strus::AnalyzerInterface*)analyzer.m_impl;
	strus::analyzer::Document doc = alz->analyze( content);

	std::vector<strus::analyzer::Attribute>::const_iterator
		ai = doc.attributes().begin(), ae = doc.attributes().end();
	for (; ai != ae; ++ai)
	{
		setAttribute( ai->name(), ai->value());
	}
	std::vector<strus::analyzer::MetaData>::const_iterator
		mi = doc.metadata().begin(), me = doc.metadata().end();
	for (; mi != me; ++mi)
	{
		setMetaData( mi->name(), mi->value());
	}
	std::vector<strus::analyzer::Term>::const_iterator
		ti = doc.terms().begin(), te = doc.terms().end();
	for (; ti != te; ++ti)
	{
		addTerm( ti->type(), ti->value(), ti->pos());
	}
}

void Document::addTerm(
		const std::string& type_,
		const std::string& value_,
		const Index& position_)
{
	m_terms.push_back( Term( type_,value_,position_));
}

void Document::setMetaData( char name_, float value_)
{
	m_metaData.push_back( MetaData( name_,value_));
}

void Document::setAttribute( char name_, const std::string& value_)
{
	m_attributes.push_back( Attribute( name_, value_));
}


Storage::Storage( const std::string& config)
	:m_impl( strus::createStorageClient( config.c_str()))
{
	m_qp_impl = strus::createQueryProcessorInterface( (strus::StorageInterface*)m_impl);
}

Storage::~Storage()
{
	delete (strus::StorageInterface*)m_impl;
	delete (strus::QueryProcessorInterface*)m_qp_impl;
}

Index Storage::nofDocumentsInserted() const
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	return THIS->nofDocumentsInserted();
}

Index Storage::maxDocumentNumber() const
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	return THIS->maxDocumentNumber();
}

Index Storage::documentNumber( const std::string& docId) const
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	return THIS->maxDocumentNumber();
}

std::string Storage::documentAttribute( Index docno, char varname) const
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	return THIS->documentAttribute( docno, varname);
}

float Storage::documentMetaData( const Index& docno, char varname)
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	return THIS->documentMetaData( docno, varname);
}

void Storage::insertDocument( const Document& doc)
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	strus::StorageInserterInterface* inserter = THIS->createInserter( doc.id());
	boost::scoped_ptr<strus::StorageInserterInterface> inserter_ref( inserter);

	std::vector<Document::Attribute>::const_iterator
		ai = doc.attributes().begin(), ae = doc.attributes().end();
	for (; ai != ae; ++ai)
	{
		inserter->setAttribute( ai->name(), ai->value());
	}
	std::vector<Document::MetaData>::const_iterator
		mi = doc.metaData().begin(), me = doc.metaData().end();
	for (; mi != me; ++mi)
	{
		inserter->setMetaData( mi->name(), mi->value());
	}
	std::vector<Document::Term>::const_iterator
		ti = doc.terms().begin(), te = doc.terms().end();
	for (; ti != te; ++ti)
	{
		inserter->addTermOccurrence( ti->type(), ti->value(), ti->position(), 0.0/*weight*/);
	}
	inserter->done();
}

void Storage::deleteDocument( const std::string& docId)
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	THIS->deleteDocument( docId);
}

void Storage::flush()
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	THIS->flush();
}

void Storage::create( const char* config)
{
	strus::createStorageDatabase( config);
}

void Storage::destroy( const char* config)
{
	strus::destroyStorageDatabase( config);
}

Analyzer::Analyzer( const std::string& source)
	:m_tkminer_impl(0),m_impl(0)
{
	try
	{
		m_tkminer_impl = strus::createTokenMinerFactory("");
		m_impl = strus::createAnalyzer( *(strus::TokenMinerFactory*)m_tkminer_impl, source);
	}
	catch (const std::exception& e)
	{
		if (m_tkminer_impl) delete (strus::TokenMinerFactory*)m_tkminer_impl;
	}
}

std::string Analyzer::tostring() const
{
	std::ostringstream out;
	((strus::AnalyzerInterface*)m_impl)->print( out);
	return out.str();
}

QueryProgram::QueryProgram( const std::string& source)
{
	m_impl = strus::createQueryEval( source);
}

QueryProgram::~QueryProgram()
{
	delete (strus::QueryEvalInterface*)m_impl;
}


Query::Query( const Storage& storage, const QueryProgram& prg)
{
	m_impl = new strus::queryeval::Query();
	m_storage_impl = storage.m_impl;
	m_qp_impl = storage.m_qp_impl;
	m_prg_impl = prg.m_impl;
}

Query::Query( const Query& o)
{
	m_impl = new strus::queryeval::Query( *(strus::queryeval::Query*)o.m_impl);
	m_storage_impl = o.m_storage_impl;
	m_qp_impl = o.m_qp_impl;
	m_prg_impl = o.m_prg_impl;
}

Query::~Query()
{
	delete (strus::queryeval::Query*)m_impl;
}

void Query::addTerm(
		const std::string& set_,
		const std::string& type_,
		const std::string& value_)
{
	strus::queryeval::Query* THIS = (strus::queryeval::Query*)m_impl;
	THIS->addTerm( set_, type_, value_);
}

void Query::joinTerms(
		const std::string& set_,
		const std::string& opname_,
		int range_,
		std::size_t nofArgs_)
{
	strus::queryeval::Query* THIS = (strus::queryeval::Query*)m_impl;
	THIS->joinTerms( set_, opname_, range_, nofArgs_);
}

std::vector<ResultDocument>
	Query::evaluate(
		std::size_t fromRank,
		std::size_t maxNofRanks) const
{
	const strus::StorageInterface* STORAGE = (const strus::StorageInterface*)m_storage_impl;
	const strus::QueryEvalInterface* QEVAL = (const strus::QueryEvalInterface*)m_prg_impl;
	const strus::QueryProcessorInterface* QPROC = (const strus::QueryProcessorInterface*)m_qp_impl;

	std::vector<ResultDocument> rt;
	std::vector<strus::queryeval::ResultDocument> res
		= QEVAL->getRankedDocumentList(
			*STORAGE, *QPROC, *(strus::queryeval::Query*)m_impl,
			fromRank, maxNofRanks);
	std::vector<strus::queryeval::ResultDocument>::const_iterator
		ri = res.begin(), re = res.end();
	for (;ri != re; ++ri)
	{
		ResultDocument reselem;
		reselem.docno = (unsigned int)ri->docno();
		reselem.weight = ri->weight();
		std::vector<strus::queryeval::ResultDocument::Attribute>::const_iterator
			ai = ri->attributes().begin(), ae = ri->attributes().end();
	
		for (;ai != ae; ++ai)
		{
			ResultAttribute attr;
			attr.name = ai->name();
			attr.value = ai->value();
			reselem.attributes.push_back( attr);
		}
		rt.push_back( reselem);
	}
	return rt;
}


