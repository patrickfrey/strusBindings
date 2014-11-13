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
#include "strus/strusObjects.hpp"
#include "strus/strus.hpp"


Storage::Storage( const char* config)
	:m_impl( strus::createStorageClient( config))
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

Index Storage::documentNumber( const std::string& docid) const
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	return THIS->maxDocumentNumber();
}

std::string Storage::documentAttribute( Index docno, char varname) const
{
	strus::StorageInterface* THIS = (strus::StorageInterface*)m_impl;
	return THIS->documentAttribute( docno, varname);
}


MetaDataReader::MetaDataReader( const Storage& storage, char varname)
{
	strus::StorageInterface* STORAGE = (strus::StorageInterface*)storage.m_impl;
	m_impl = STORAGE->createMetaDataReader( varname);
}

MetaDataReader::~MetaDataReader()
{
	delete (strus::MetaDataReaderInterface*)m_impl;
}

float MetaDataReader::readValue( const Index& docno_)
{
	strus::MetaDataReaderInterface* THIS = (strus::MetaDataReaderInterface*)m_impl;
	return THIS->readValue( docno_);
}



Inserter::Inserter( const Storage& storage, const std::string& docid)
{
	strus::StorageInterface* STORAGE = (strus::StorageInterface*)storage.m_impl;
	m_impl = STORAGE->createInserter( docid);
}

Inserter::~Inserter()
{
	delete (strus::StorageInserterInterface*)m_impl;
}

void Inserter::addTerm( const std::string& type_,
		const std::string& value_,
		const Index& position_)
{
	strus::StorageInserterInterface* THIS = (strus::StorageInserterInterface*)m_impl;
	THIS->addTermOccurrence( type_, value_, position_, 0.0/*weight*/);
}

void Inserter::setMetaData( char name_, float value_)
{
	strus::StorageInserterInterface* THIS = (strus::StorageInserterInterface*)m_impl;
	THIS->setMetaData( name_, value_);
}

void Inserter::setAttribute( char name_, const std::string& value_)
{
	strus::StorageInserterInterface* THIS = (strus::StorageInserterInterface*)m_impl;
	THIS->setAttribute( name_, value_);
}

void Inserter::done()
{
	strus::StorageInserterInterface* THIS = (strus::StorageInserterInterface*)m_impl;
	THIS->done();
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


