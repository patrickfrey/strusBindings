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
#include <map>
#include <boost/shared_ptr.hpp>

typedef unsigned int Index;
typedef unsigned long GlobalCounter;

/// \brief Reference to an object used for making objects independent and save from garbage collecting in an interpreter context
class Reference
{
public:
	typedef void (*Deleter)( void* obj);

	/// \brief Default constructor
	explicit Reference( Deleter deleter_)
		:m_ptr(),m_deleter(deleter_){}

	/// \brief Copy constructor
	Reference( const Reference& o)
		:m_ptr(o.m_ptr),m_deleter(o.m_deleter){}

	/// \brief Destructor
	~Reference(){}

	void reset( void* obj_=0)
	{
		m_ptr.reset( obj_, m_deleter);
	}

	/// \brief Assignment operator
	Reference& operator = (const Reference& o)
	{
		m_ptr = o.m_ptr;
		m_deleter = o.m_deleter;
		return *this;
	}

	/// \brief Object access as function
	const void* get() const				{return m_ptr.get();}
	/// \brief Object access as function
	void* get()					{return m_ptr.get();}

private:
	boost::shared_ptr<void> m_ptr;
	void (*m_deleter)( void* obj);
};


/// \brief Object representing a function definition with string parameters
class FunctionDef
{
public:
	FunctionDef( const FunctionDef& o)
		:m_name(o.m_name),m_arguments(o.m_arguments){}
	FunctionDef( const std::string& name_, const std::string& arg1)
		:m_name(name_)
	{
		m_arguments.push_back( arg1);
	}
	FunctionDef( const std::string& name_, const std::string& arg1, const std::string& arg2)
		:m_name(name_)
	{
		m_arguments.push_back( arg1);
		m_arguments.push_back( arg2);
	}
	FunctionDef( const std::string& name_)
		:m_name(name_),m_arguments(){}

	const std::string& name() const				{return m_name;}
	const std::vector<std::string>& arguments() const	{return m_arguments;}

private:
	std::string m_name;
	std::vector<std::string> m_arguments;
};


/// \brief Variant type for passing parameter values of arbitrary type
class Variant
{
public:
	enum Type
	{
		UNDEFINED,UINT,INT,FLOAT,TEXT
	};

	Variant();
	Variant( const Variant& o);
	Variant( unsigned int v);
	Variant( int v);
	Variant( float v);
	Variant( double v);
	Variant( const char* v);
	Variant( const std::string& v);

	bool defined() const			{return m_type != UNDEFINED;}
	Type type() const			{return m_type;}
	unsigned int getUInt() const;
	int getInt() const;
	float getFloat() const;
	const char* getText() const;

private:
	friend class Storage;
	friend class Summarizer;
	friend class WeightingFunction;
	friend class QueryEval;
	friend class DocumentAnalyzer;
	Type m_type;
	union
	{
		unsigned int UINT;
		int INT;
		float FLOAT;
		const char* TEXT;
	} m_value;
	std::string m_buf;
};


/// \brief One typed term occurrence in a document or a query
class Term
{
public:
	/// \brief Constructor
	Term( const std::string& type_, const std::string& value_, const Index& position_)
		:m_type(type_),m_value(value_),m_position(position_){}
	/// \brief Copy constructor
	Term( const Term& o)
		:m_type(o.m_type),m_value(o.m_value),m_position(o.m_position){}
	/// \brief Default constructor
	Term()
		:m_position(0){}

	/// \brief Get the term type name
	const std::string& name() const		{return m_type;}
	/// \brief Get the term value
	const std::string& value() const	{return m_value;}
	/// \brief Get the term position
	unsigned int position() const		{return m_position;}

private:
	std::string m_type;
	std::string m_value;
	Index m_position;
};

/// \brief Data object that represents single numeric property of a document that
///	can be subject of retrieval or act as search restriction.
class MetaData
{
public:
	/// \brief Constructor
	MetaData( const std::string& name_, Variant value_)
		:m_name(name_),m_value(value_){}
	/// \brief Copy constructor
	MetaData( const MetaData& o)
		:m_name(o.m_name),m_value(o.m_value){}
	/// \brief Default constructor
	MetaData()
		:m_name(0),m_value(){}

	/// \brief Get the type name of this meta data field:
	const std::string& name() const		{return m_name;}
	/// \brief Get the value of this meta data field:
	const Variant& value() const		{return m_value;}

private:
	std::string m_name;
	Variant m_value;
};

/// \brief Data object that describes a single property of a document
///	that is not subject of retrieval. It acts as description of the
///	document that can be shown as a result of retrieval.
class Attribute
{
public:
	/// \brief Constructor
	Attribute( const std::string& name_, const std::string& value_)
		:m_name(name_),m_value(value_){}
	/// \brief Constructor
	Attribute( const Attribute& o)
		:m_name(o.m_name),m_value(o.m_value){}
	/// \brief Constructor
	Attribute()
		:m_name(0){}

	/// \brief Get the unique type name of this attribute
	const std::string& name() const		{return m_name;}
	/// \brief Get the type value of this attribute
	const std::string& value() const	{return m_value;}

private:
	std::string m_name;
	std::string m_value;
};

/// \brief Document object representing one item of retrieval. A document can be
///	manually composed of its sub parts or it can be the result of an analyzer run.
class Document
{
public:
	/// \brief Default constructor
	Document(){}
	/// \brief Copy constructor
	Document( const Document& o);

	/// \brief Add a single term occurrence to the document for retrieval
	void addSearchIndexTerm( const std::string& type_, const std::string& value_, const Index& position_);
	/// \brief Add a single term occurrence to the document for use in the summary of a retrieval result
	void addForwardIndexTerm( const std::string& type_, const std::string& value_, const Index& position_);
	/// \brief Define a meta data value of the document
	void setMetaData( const std::string& name_, Variant value_);
	/// \brief Define an attribute of the document
	void setAttribute( const std::string& name_, const std::string& value_);
	/// \brief Allow a user to access the document
	/// \remark This function is only implemented if ACL is enabled in the storage
	void setUserAccessRight( const std::string& username_);
	/// \brief Set the document identifier (docid) of the document
	void setDocid( const std::string& docid_);

	/// \brief Get the list of search terms of the document
	const std::vector<Term>& searchIndexTerms() const		{return m_searchIndexTerms;}
	/// \brief Get the list of forward terms of the document
	const std::vector<Term>& forwardIndexTerms() const		{return m_forwardIndexTerms;}
	/// \brief Get the list of meta data of the document
	const std::vector<MetaData>& metaData() const			{return m_metaData;}
	/// \brief Get the list of attributes of the document
	const std::vector<Attribute>& attributes() const		{return m_attributes;}
	/// \brief Get the list of users that are allowed to access the document
	const std::vector<std::string>& users() const			{return m_users;}
	/// \brief Get the document identifier (docid) of the document
	/// \return the document identifier
	const std::string& docid()					{return m_docid;}

private:
	std::vector<Term> m_searchIndexTerms;
	std::vector<Term> m_forwardIndexTerms;
	std::vector<MetaData> m_metaData;
	std::vector<Attribute> m_attributes;
	std::vector<std::string> m_users;
	std::string m_docid;
};


/// \class DocumentAnalyzer
/// \brief Analyzer object representing a program for segmenting, 
///	tokenizing and normalizing a document into atomic parts, that 
///	can be inserted into a storage and be retrieved from there.
class DocumentAnalyzer
{
public:
	/// \brief Constructor
	DocumentAnalyzer();
	/// \brief Copy constructor
	DocumentAnalyzer( const DocumentAnalyzer& o);
	/// \brief Destructor
	~DocumentAnalyzer(){}

	/// \brief Define how a feature to insert into the inverted index (search index) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] tokenizer normalizer function description to use for this feature
	void addSearchIndexFeature(
		const std::string& type,
		const std::string& selectexpr,
		const FunctionDef& tokenizer,
		const FunctionDef& normalizer);

	/// \brief Define how a feature to insert into the forward index (summarization) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] tokenizer normalizer function description to use for this feature
	void addForwardIndexFeature(
		const std::string& type,
		const std::string& selectexpr,
		const FunctionDef& tokenizer,
		const FunctionDef& normalizer);

	/// \brief Define how a feature to insert as meta data (summarization) is selected, tokenized and normalized
	/// \param[in] fieldname name of the addressed meta data field.
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] tokenizer normalizer function description to use for this feature
	void defineMetaData(
		const std::string& fieldname,
		const std::string& selectexpr,
		const FunctionDef& tokenizer,
		const FunctionDef& normalizer);

	/// \brief Define how a feature to insert as document attribute (summarization) is selected, tokenized and normalized
	void defineAttribute(
		const std::string& attribname,
		const std::string& selectexpr,
		const FunctionDef& tokenizer,
		const FunctionDef& normalizer);
	/// \param[in] attribname name of the addressed attribute.
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] tokenizer normalizer function description to use for this feature

	/// \brief Analye the content and return the set of features to insert
	Document analyze( const std::string& content);

private:
	Reference m_textproc_impl;
	Reference m_analyzer_impl;
};


/// \class QueryAnalyzer
/// \brief Analyzer object representing a set of function for transforming a phrase,
///	the smallest unit in any query language, to a set of terms that can be used
///	to build a query.
class QueryAnalyzer
{
public:
	/// \brief Constructor
	QueryAnalyzer();
	/// \brief Copy constructor
	QueryAnalyzer( const QueryAnalyzer& o);
	/// \brief Destructor
	~QueryAnalyzer(){}

	/// \brief Defines a phrase type by name. Phrases can be passed together with this name
	///		to the query analyzer to get the terms for building query.
	void definePhraseType(
			const std::string& phraseType,
			const std::string& featureType,
			const FunctionDef& tokenizer,
			const FunctionDef& normalizer);

	/// \brief Tokenizes and normalizes a phrase and creates some typed terms out of it according the definition of the phrase type given.
	std::vector<Term> analyzePhrase(
			const std::string& phraseType,
			const std::string& phraseContent) const;

private:
	Reference m_textproc_impl;
	Reference m_analyzer_impl;
};


/// \brief Singleton object representing the client to the storage 
///	of the information retrieval engine.
class Storage
{
public:
	/// \brief Constructor from a configuration description string
	/// \remark config is not a file name, but a string of semicolon separated key value assignments
	explicit Storage( const std::string& config);
	Storage( const Storage& o);

	~Storage(){}

	/// \brief Get the number of documents inserted into the storage
	GlobalCounter nofDocumentsInserted() const;

	/// \brief Prepare inserting a document into the storage
	/// \remark The document is physically inserted with the next implicit or explicit call of 'flush()'
	void insertDocument( const std::string& docid, const Document& doc);

	/// \brief Prepare deletion of a document from the storage
	/// \remark The document is physically deleted with the next implicit or explicit call of 'flush()'
	void deleteDocument( const std::string& docid);

	/// \brief Delete all document access rights of a user
	void deleteUserAccessRights( const std::string& username);

	/// \brief Commit all insert or delete or user access right change statements open.
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
	friend class QueryEval;
	Reference m_database_impl;
	Reference m_storage_impl;
	Reference m_queryproc_impl;
	Reference m_transaction_impl;
};


class Summarizer
{
public:
	/// \brief Constructor
	explicit Summarizer( const std::string& name_)
		:m_name(name_){}
	/// \brief Copy constructor
	Summarizer( const Summarizer& o)
		:m_name(o.m_name),m_parameters(o.m_parameters),m_features(o.m_features){}

	/// \brief Define a summarizer feature
	void defineParameter( const std::string& name_, const Variant& value_)
	{
		m_parameters[ name_] = value_;
	}

	/// \brief Define a summarizer feature
	void defineFeature( const std::string& class_, const std::string& set_)
	{
		m_features[ class_] = set_;
	}

private:
	friend class QueryEval;
	std::string m_name;
	std::map<std::string,Variant> m_parameters;
	std::map<std::string,std::string> m_features;
};

class WeightingFunction
{
public:
	explicit WeightingFunction( const std::string& name_)
		:m_name(name_){}
	WeightingFunction( const WeightingFunction& o)
		:m_name(o.m_name),m_parameters(o.m_parameters){}

	void defineParameter( const std::string& name_, const Variant& value_)
	{
		m_parameters[ name_] = value_;
	}

private:
	friend class QueryEval;
	std::string m_name;
	std::map<std::string,Variant> m_parameters;
};

/// \brief Query program object representing a retrieval method for documents in a storage.
class QueryEval
{
public:
	/// \brief Constructor
	explicit QueryEval( const Storage& storage);
	/// \brief Copy constructor
	QueryEval( const QueryEval& o);
	/// \brief Destructor
	~QueryEval(){}

	/// \brief Declare a term that is used in the query evaluation as structural element without beeing part of the query (for example punctuation used for match phrases summarization)
	void addTerm(
			const std::string& set_,
			const std::string& type_,
			const std::string& value_);

	/// \brief Declare a feature set to be used as selecting feature
	void addSelectionFeature( const std::string& set_);

	/// \brief Declare a feature set to be used as restriction
	void addRestrictionFeature( const std::string& set_);

	/// \brief Declare a feature set to be used for weighting
	void addWeightingFeature( const std::string& set_);

	/// \brief Declare a summarizer
	void addSummarizer(
			const std::string& resultAttribute,
			const Summarizer& summarizer);

	/// \brief Declare the weighting function used
	void defineWeightingFunction(
			const WeightingFunction& weightingFunction);

private:
	friend class Query;
	Reference m_database_impl;
	Reference m_storage_impl;
	Reference m_queryproc_impl;
	Reference m_queryeval_impl;
};


/// \brief Attribute of a query evaluation result element
class RankAttribute
{
public:
	/// \brief Constructor
	RankAttribute(){}
	/// \brief Constructor
	RankAttribute( const std::string& name_, const std::string& value_)
		:m_name(name_),m_value(value_){}
	/// \brief Copy connstructor
	RankAttribute( const RankAttribute& o)
		:m_name(o.m_name),m_value(o.m_value){}

	/// \brief Get the name of this attribute
	const std::string& name() const		{return m_name;}
	/// \brief Get the value of this attribute
	const std::string& value() const	{return m_value;}

private:
	friend class Query;
	std::string m_name;
	std::string m_value;
};

/// \brief Weighted document reference with attributes (result of a query evaluation)
class Rank
{
public:
	/// \brief Constructor
	Rank()
		:m_docno(0),m_weight(0.0){}
	/// \brief Constructor
	Rank( Index docno_, float weight_, const std::vector<RankAttribute>& attributes_)
		:m_docno(docno_),m_weight(weight_),m_attributes(attributes_){}
	/// \brief Copy constructor
	Rank( const Rank& o)
		:m_docno(o.m_docno),m_weight(o.m_weight),m_attributes(o.m_attributes){}

	/// \brief Get the internal document nuber used
	Index docno() const					{return m_docno;}
	/// \brief Get the weight of the rank
	float weight() const					{return m_weight;}
	/// \brief Get the attributes
	const std::vector<RankAttribute>& attributes() const	{return m_attributes;}

private:
	friend class Query;
	Index m_docno;
	float m_weight;
	std::vector<RankAttribute> m_attributes;
};

/// \brief Query program object representing a retrieval method for documents in a storage.
class Query
{
public:
	/// \brief Constructor
	explicit Query( const QueryEval& queryeval);
	/// \brief Copy constructor
	Query( const Query& o);
	/// \brief Destructor
	~Query(){}

	/// \brief Push a single term feature on the stack
	void pushTerm( const std::string& type_, const std::string& value_);

	/// \brief Create an expression from the topmost 'argc' elements of the stack, pop them from the stack and push the expression as single unit on the stack
	void pushExpression( const std::string& opname_, unsigned int argc, int range_);

	/// \brief Attaches a variable to the top expression or term on the query stack.
	/// \note The positions of the query matches of the referenced term or expression can be accessed through this variable in summarization.
	/// \param[in] name_ name of the variable attached
	/// \remark The stack is not changed
	void attachVariable( const std::string& name_);

	/// \brief Create a feature from the top element on the stack (and pop the element from the stack)
	void defineFeature( const std::string& set_, float weight_=1.0);

	/// \brief Define a meta data restrictions
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] operand numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void defineMetaDataRestriction(
			const char* compareOp, const std::string& name,
			const Variant& operand, bool newGroup=true);

	/// \brief Set maximum number of ranks to evaluate (not the maximum size of the result rank list. This is maxNofRanks - minRank)
	void setMaxNofRanks( unsigned int maxNofRanks_);

	/// \brief Set the index of the first rank to be returned
	void setMinRank( unsigned int minRank_);

	/// \brief Set the user of the query (overwrites the current user set silently)
	void setUserName( const std::string& username_);

	/// \brief Evaluate the query
	/// \return the result
	std::vector<Rank> evaluate() const;

private:
	Reference m_database_impl;
	Reference m_storage_impl;
	Reference m_queryeval_impl;
	Reference m_queryproc_impl;
	Reference m_query_impl;
};

#endif


