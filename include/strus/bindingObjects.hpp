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

/// \brief Analyzer object representing one program for segmenting, 
///	tokenizing and normalizing a document into atomic parts, that 
///	can be inserted into a storage and be retrieved from there.
class Analyzer
{
public:
	/// \brief Constructor from the program source
	explicit Analyzer( const std::string& source);

	/// \brief Printing the program contents to a string
	std::string tostring() const;

private:
	friend class Document;
	friend class Query;
	void* m_tkminer_impl;
	void* m_impl;
};


/// \brief One typed term occurrence in a document
class DocumentTerm
{
public:
	DocumentTerm( const std::string& type_, const std::string& value_, const Index& position_)
		:m_type(type_),m_value(value_),m_position(position_){}
	DocumentTerm( const DocumentTerm& o)
		:m_type(o.m_type),m_value(o.m_value),m_position(o.m_position){}
	DocumentTerm()
		:m_position(0){}

	const std::string& type() const		{return m_type;}
	const std::string& value() const	{return m_value;}
	unsigned int position() const		{return m_position;}

private:
	std::string m_type;
	std::string m_value;
	Index m_position;
};

/// \brief Data object that represents single property of a document that
///	can be subject of retrieval or act as search restriction.
class DocumentMetaData
{
public:
	DocumentMetaData( char name_, float value_)
		:m_name(name_),m_value(value_){}
	DocumentMetaData( const DocumentMetaData& o)
		:m_name(o.m_name),m_value(o.m_value){}
	DocumentMetaData()
		:m_name(0),m_value(0.0){}

	char name() const		{return m_name;}
	const float value() const	{return m_value;}

private:
	char m_name;
	float m_value;
};

/// \brief Data object that describes a single property of a document
///	that is not subject of retrieval, but acts as description of the
///	document.
class DocumentAttribute
{
public:
	DocumentAttribute( char name_, const std::string& value_)
		:m_name(name_),m_value(value_){}
	DocumentAttribute( const DocumentAttribute& o)
		:m_name(o.m_name),m_value(o.m_value){}
	DocumentAttribute()
		:m_name(0){}

	char name() const			{return m_name;}
	const std::string& value() const	{return m_value;}

private:
	char m_name;
	std::string m_value;
};

/// \brief Document object representing one item of retrieval. A document can be
///	manually composed of its sub parts or it can be the result of an analyzer run.
class Document
{
public:
	/// \brief  Open a document to add elements manually
	explicit Document( const std::string& docId);

	/// \brief Open a document created from an analyzed source
	/// \note You can add additional elements manually
	Document( const Analyzer& analyzer,
			const std::string& docId_,
			const std::string& content);

	/// \brief Add a single term occurrence to the document
	void addTerm( const std::string& type_,
			const std::string& value_,
			const Index& position_);

	/// \brief Define a meta data value of the document
	void setMetaData( char name_, float value_);

	/// \brief Define an attribute of the document
	void setAttribute( char name_, const std::string& value_);

	/// \brief Get the unique id of the document
	const std::string& id() const					{return m_id;}
	/// \brief Get the list of terms of the document
	const std::vector<DocumentTerm>& terms() const			{return m_terms;}
	/// \brief Get the list of meta data of the document
	const std::vector<DocumentMetaData>& metaData() const		{return m_metaData;}
	/// \brief Get the list of attributes of the document
	const std::vector<DocumentAttribute>& attributes() const	{return m_attributes;}

private:
	std::string m_id;
	std::vector<DocumentTerm> m_terms;
	std::vector<DocumentMetaData> m_metaData;
	std::vector<DocumentAttribute> m_attributes;
};


/// \brief Singleton object representing the client to the storage 
///	of the information retrieval engine.
class Storage
{
public:
	/// \brief Constructor from a configuration description string
	/// \remark config is not a file name, but a string of semicolon separated key value assignments
	explicit Storage( const std::string& config);

	~Storage();

	/// \brief Get the number of documents inserted into the storage
	Index nofDocumentsInserted() const;
	/// \brief Get the largest document number ever assigned to a document in the storage
	/// \remark The largest document number may belong to a document deleted, because document numbers are not recycled
	Index maxDocumentNumber() const;
	/// \brief Get the document number of a document inserted
	Index documentNumber( const std::string& docId) const;

	/// \brief Get a specific document attribute of a document inserted
	std::string documentAttribute( Index docno, char varname) const;
	/// \brief Get a specific document metadata element of a document inserted
	float documentMetaData( const Index& docno, char varname);

	/// \brief Prepare inserting a document into the storage
	/// \remark The document is physically inserted with the next implicit or explicit call of 'flush()'
	void insertDocument( const Document& doc);

	/// \brief Prepare deletion of a document from the storage
	/// \remark The document is physically deleted with the next implicit or explicit call of 'flush()'
	void deleteDocument( const std::string& docId);

	/// \brief Commit all insert or delete statements open.
	void flush();

	/// \brief Create a new storage described by config
	/// \remark Fails if the storage already exists
	static void create( const char* config);

	/// \brief Delete the storage described by config
	/// \note Handle this carefully
	static void destroy( const char* config);

	/// \brief Close of the storage that throws in case of an error
	void close();

private:
	friend class Query;
	void* m_impl;
	void* m_qp_impl;
};


/// \brief Query program object representing a retrieval method for documents in a storage.
class QueryProgram
{
public:
	/// \brief Constructor from a program source
	/// \remark source is not a file name, but the program source as string.
	explicit QueryProgram( const std::string& source);
	~QueryProgram();

	/// \brief Printing the program contents to a string
	std::string tostring() const;

private:
	friend class Query;
	void* m_impl;
};


/// \brief Attribute of a query evaluation result element
struct RankAttribute
{
	RankAttribute(){}
	RankAttribute( const RankAttribute& o)
		:name(o.name),value(o.value){}

	std::string name;
	std::string value;
};

/// \brief Weighted document reference with attributes (result of a query evaluation)
struct Rank
{
	Index docno;
	float weight;
	std::vector<RankAttribute> attributes;

	Rank()
		:docno(0),weight(0.0){}
	Rank( const Rank& o)
		:docno(o.docno),weight(o.weight),attributes(o.attributes){}
};

/// \brief Object representing a query
class Query
{
public:
	Query();
	Query( const Analyzer& analyzer, const std::string& content);
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

	virtual std::vector<Rank>
		evaluate(
			const Storage& storage,
			const QueryProgram& prg,
			std::size_t fromRank,
			std::size_t maxNofRanks) const;
private:
	void* m_impl;
};

#endif


