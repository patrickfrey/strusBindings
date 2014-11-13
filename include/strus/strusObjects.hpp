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
#ifndef _STRUS_BINDING_OBJECTS_HPP_INCLUDED
#define _STRUS_BINDING_OBJECTS_HPP_INCLUDED
#include <string>
#include <vector>

typedef unsigned int Index;

class Storage
{
public:
	Storage( const char* config);

	~Storage();

	Index nofDocumentsInserted() const;
	Index maxDocumentNumber() const;
	Index documentNumber( const std::string& docid) const;

	std::string documentAttribute( Index docno, char varname) const;

private:
	friend class MetaDataReader;
	friend class Inserter;
	friend class Query;
	void* m_impl;
	void* m_qp_impl;
};



class MetaDataReader
{
public:
	MetaDataReader( const Storage& storage, char varname);
	~MetaDataReader();

	float readValue( const Index& docno_);

private:
	void* m_impl;
};


class Inserter
{
	Inserter( const Storage& storage, const std::string& docid);
	~Inserter();

	void addTerm( const std::string& type_,
			const std::string& value_,
			const Index& position_);

	void setMetaData( char name_, float value_);

	void setAttribute( char name_, const std::string& value_);

	void done();
	
private:
	void* m_impl;
};


class QueryProgram
{
public:
	QueryProgram( const std::string& source);
	~QueryProgram();

private:
	friend class Query;
	void* m_impl;
};


struct ResultAttribute
{
	ResultAttribute(){}
	ResultAttribute( const ResultAttribute& o)
		:name(o.name),value(o.value){}

	std::string name;
	std::string value;
};

struct ResultDocument
{
	Index docno;
	float weight;
	std::vector<ResultAttribute> attributes;

	ResultDocument()
		:docno(0),weight(0.0){}
	ResultDocument( const ResultDocument& o)
		:docno(o.docno),weight(o.weight),attributes(o.attributes){}
};

class Query
{
public:
	Query( const Storage& storage, const QueryProgram& prg);
	Query( const Query& o);
	~Query();

	void addTerm(
		const std::string& set_,
		const std::string& type_,
		const std::string& value_);

	void joinTerms(
		const std::string& set_,
		const std::string& opname_,
		int range_,
		std::size_t nofArgs_);

	virtual std::vector<ResultDocument>
		evaluate(
			std::size_t fromRank,
			std::size_t maxNofRanks) const=0;
private:
	void* m_impl;
	const void* m_storage_impl;
	const void* m_qp_impl;
	const void* m_prg_impl;
};

#endif


