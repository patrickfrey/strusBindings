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
#include <stdexcept>
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


/// \brief Object representing a tokenizer function definition
class Tokenizer
{
public:
	Tokenizer(){}
	Tokenizer( const Tokenizer& o)
		:m_name(o.m_name),m_arguments(o.m_arguments){}
	Tokenizer( const std::string& name_, const std::vector<std::string>& arg_)
		:m_name(name_),m_arguments(arg_){}
	Tokenizer( const std::string& name_)
		:m_name(name_),m_arguments(){}

	const std::string& name() const				{return m_name;}
	const std::vector<std::string>& arguments() const	{return m_arguments;}

	void setName( const std::string& name_)			{m_name = name_;}
	void addArgument( const std::string& arg_)		{m_arguments.push_back( arg_);}

private:
	std::string m_name;
	std::vector<std::string> m_arguments;
};


/// \brief Object representing a normalizer function definition
class Normalizer
{
public:
	Normalizer(){}
	Normalizer( const Normalizer& o)
		:m_name(o.m_name),m_arguments(o.m_arguments){}
	Normalizer( const std::string& name_, const std::vector<std::string>& arg_)
		:m_name(name_),m_arguments(arg_){}
	Normalizer( const std::string& name_)
		:m_name(name_),m_arguments(){}

	~Normalizer(){}

	const std::string& name() const				{return m_name;}
	const std::vector<std::string>& arguments() const	{return m_arguments;}

	void setName( const std::string& name_)			{m_name = name_;}
	void addArgument( const std::string& arg_)		{m_arguments.push_back( arg_);}

private:
	friend class QueryAnalyzer;
	friend class DocumentAnalyzer;

	std::string m_name;
	std::vector<std::string> m_arguments;
};


/// \brief Object representing a aggregator function definition
class Aggregator
{
public:
	Aggregator(){}
	Aggregator( const Aggregator& o)
		:m_name(o.m_name),m_arguments(o.m_arguments){}
	Aggregator( const std::string& name_, const std::vector<std::string>& arg_)
		:m_name(name_),m_arguments(arg_){}
	Aggregator( const std::string& name_)
		:m_name(name_),m_arguments(){}

	~Aggregator(){}

	const std::string& name() const				{return m_name;}
	const std::vector<std::string>& arguments() const	{return m_arguments;}

	void setName( const std::string& name_)			{m_name = name_;}
	void addArgument( const std::string& arg_)		{m_arguments.push_back( arg_);}

private:
	friend class DocumentAnalyzer;

	std::string m_name;
	std::vector<std::string> m_arguments;
};


/// \brief Enumeration for defining internal variant representation
enum VariantType
{
	Variant_UNDEFINED,
	Variant_UINT,
	Variant_INT,
	Variant_FLOAT,
	Variant_TEXT
};

/// \brief Union for internal variant value representation
union VariantValue
{
	unsigned int UINT;
	int INT;
	float FLOAT;
	const char* TEXT;
};

/// \brief Variant type for passing parameter values of arbitrary type
class Variant
{
public:
	Variant();
	Variant( const Variant& o);
	Variant( unsigned int v);
	Variant( int v);
	Variant( float v);
	Variant( double v);
	Variant( const std::string& v);

	bool defined() const			{return m_type != Variant_UNDEFINED;}
	VariantType type() const		{return m_type;}
	unsigned int getUInt() const;
	int getInt() const;
	float getFloat() const;
	const char* getText() const;

	void init();
	void assign( const Variant& o);
	void assign( unsigned int v);
	void assign( int v);
	void assign( float v);
	void assign( double v);
	void assign( const std::string& v);

private:
	friend class Storage;
	friend class Summarizer;
	friend class WeightingFunction;
	friend class QueryEval;
	friend class DocumentAnalyzer;
	VariantType m_type;
	VariantValue m_value;
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
	const std::string& type() const		{return m_type;}
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
	MetaData( const std::string& name_, const Variant& value_)
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


/// \brief Defines a description of the properties of a document content processed by the analyzer
class DocumentClass
{
public:
	/// \brief Default constructor
	DocumentClass(){}
	/// \brief Constructor
	explicit DocumentClass(
			const std::string& mimeType_)		:m_mimeType(mimeType_){}
	/// \brief Constructor
	DocumentClass(
			const std::string& mimeType_,
			const std::string& encoding_)		:m_mimeType(mimeType_),m_encoding(encoding_){}
	/// \brief Constructor
	DocumentClass(
			const std::string& mimeType_,
			const std::string& encoding_,
			const std::string& scheme_)		:m_mimeType(mimeType_),m_scheme(scheme_),m_encoding(encoding_){}
	/// \brief Copy constructor
	DocumentClass( const DocumentClass& o)			:m_mimeType(o.m_mimeType),m_scheme(o.m_scheme),m_encoding(o.m_encoding){}

	/// \brief Check if this document class is valid
	/// \return true, if yes
	bool valid() const					{return !m_mimeType.empty();}

	/// \brief Set the MIME type of the document class
	/// \param[in] the document MIME type string
	void setMimeType( const std::string& mimeType_)		{m_mimeType = mimeType_;}
	/// \brief Set the scheme identifier of the document class
	/// \param[in] the document scheme identifier
	void setScheme( const std::string& scheme_)		{m_scheme = scheme_;}
	/// \brief Set the character set encoding of the document class
	/// \param[in] the character set encoding string
	void setEncoding( const std::string& encoding_)		{m_encoding = encoding_;}

	/// \brief Get the MIME type of the document class
	/// \return the document MIME type string
	const std::string& mimeType() const			{return m_mimeType;}
	/// \brief Get the scheme identifier of the document class
	/// \return the document scheme identifier
	const std::string& scheme() const			{return m_scheme;}
	/// \brief Get the character set encoding of the document class
	/// \return the character set encoding string
	const std::string& encoding() const			{return m_encoding;}

private:
	std::string m_mimeType;
	std::string m_scheme;
	std::string m_encoding;
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
	/// \param[in] type_ term type name of the search index term
	/// \param[in] value_ term value of the search index term
	/// \param[in] position_ word count position of the search index term
	void addSearchIndexTerm( const std::string& type_, const std::string& value_, const Index& position_);
	/// \brief Add a single term occurrence to the document for use in the summary of a retrieval result
	/// \param[in] type_ term type name of the forward index term
	/// \param[in] value_ term value of the forward index term
	/// \param[in] position_ word count position of the forward index term
	void addForwardIndexTerm( const std::string& type_, const std::string& value_, const Index& position_);
	/// \brief Define a meta data value of the document
	/// \param[in] name_ name of the meta data table element
	/// \param[in] value_ value of the meta data table element
	void setMetaData( const std::string& name_, const Variant& value_);
	/// \brief Define an attribute of the document
	/// \param[in] name_ name of the document attribute
	/// \param[in] value_ value of the document attribute
	void setAttribute( const std::string& name_, const std::string& value_);
	/// \brief Allow a user to access the document
	/// \param[in] username_ name of the user to be allowed to access this document
	/// \remark This function is only implemented if ACL is enabled in the storage
	void setUserAccessRight( const std::string& username_);
	/// \brief Set the document identifier (docid) of the document
	/// \param[in] docid_ document identifier of this document
	void setDocid( const std::string& docid_);

	/// \brief Get the list of search terms of the document
	/// \return the list of terms
	const std::vector<Term>& searchIndexTerms() const		{return m_searchIndexTerms;}
	/// \brief Get the list of forward terms of the document
	/// \return the list of terms
	const std::vector<Term>& forwardIndexTerms() const		{return m_forwardIndexTerms;}
	/// \brief Get the list of meta data of the document
	/// \return the list of meta data definitions
	const std::vector<MetaData>& metaData() const			{return m_metaData;}
	/// \brief Get the list of attributes of the document
	/// \return the list of attributes
	const std::vector<Attribute>& attributes() const		{return m_attributes;}
	/// \brief Get the list of users that are allowed to access the document
	/// \return the list of users allowed to access this document
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
/// \remark The only way to construct a document analyzer instance is to call StrusContext::createDocumentAnalyzer()
class DocumentAnalyzer
{
public:
	/// \brief Copy constructor
	DocumentAnalyzer( const DocumentAnalyzer& o);
	/// \brief Destructor
	~DocumentAnalyzer(){}

	/// \brief Define how a feature to insert into the inverted index (search index) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	/// \param[in] options a list of options as strings, one of {"BindPosPred" => the position is bound to the preceeding feature, "BindPosSucc" => the position is bound to the succeeding feature}
	void addSearchIndexFeature(
		const std::string& type,
		const std::string& selectexpr,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers,
		const std::vector<std::string>& options=std::vector<std::string>());

	/// \brief Define how a feature to insert into the forward index (for summarization) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	/// \param[in] options a list of options as strings, one of {"BindPosPred" => the position is bound to the preceeding feature, "BindPosSucc" => the position is bound to the succeeding feature}
	void addForwardIndexFeature(
		const std::string& type,
		const std::string& selectexpr,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers,
		const std::vector<std::string>& options=std::vector<std::string>());

	/// \brief Define how a feature to insert as meta data is selected, tokenized and normalized
	/// \param[in] fieldname name of the addressed meta data field.
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	void defineMetaData(
		const std::string& fieldname,
		const std::string& selectexpr,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers);

	/// \brief Declare some aggregated value of the document to be put into the meta data table used for restrictions, weighting and summarization.
 	/// \param[in] fieldname name of the addressed meta data field.
	/// \param[in] function defining how and from what the value is aggregated
	void defineAggregatedMetaData(
		const std::string& fieldname,
		const Aggregator& function);

	/// \brief Define how a feature to insert as document attribute (for summarization) is selected, tokenized and normalized
	/// \param[in] attribname name of the addressed attribute.
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	void defineAttribute(
		const std::string& attribname,
		const std::string& selectexpr,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers);

	/// \brief Analye the content and return the set of features to insert
	/// \param[in] content string (NOT a file name !) of the document to analyze
	Document analyze( const std::string& content);

	/// \brief Analye the content and return the set of features to insert
	/// \param[in] content string (NOT a file name !) of the document to analyze
	/// \param[in] dclass document class of the document to analyze
	Document analyze( const std::string& content, const DocumentClass& dclass);

private:
	/// \brief Constructor used by StrusContext
	friend class StrusContext;
	DocumentAnalyzer( const Reference& objbuilder, const std::string& segmentername);

	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
};


/// \class QueryAnalyzer
/// \brief Analyzer object representing a set of function for transforming a phrase,
///	the smallest unit in any query language, to a set of terms that can be used
///	to build a query.
/// \remark The only way to construct a query analyzer instance is to call StrusContext::createQueryAnalyzer()
class QueryAnalyzer
{
public:
	/// \brief Copy constructor
	QueryAnalyzer( const QueryAnalyzer& o);
	/// \brief Destructor
	~QueryAnalyzer(){}

	/// \brief Defines a phrase type by name. Phrases can be passed together with this name
	///		to the query analyzer to get the terms for building query.
	/// \param[in] phraseType name of the phrase type defined
	/// \param[in] featureType feature type name assigned to the features created by this phrase type
	/// \param[in] tokenizer tokenizer function description to use for the features of this phrase type
	/// \param[in] normalizers list of normalizer function description to use for the features of this phrase type in the ascending order of appearance
	void definePhraseType(
			const std::string& phraseType,
			const std::string& featureType,
			const Tokenizer& tokenizer,
			const std::vector<Normalizer>& normalizers);

	/// \brief Tokenizes and normalizes a phrase and creates some typed terms out of it according the definition of the phrase type given.
	/// \param[in] phraseType name of the phrase type to use for analysis
	/// \param[in] phraseContent content string of the query phrase to analyze
	std::vector<Term> analyzePhrase(
			const std::string& phraseType,
			const std::string& phraseContent) const;

private:
	/// \brief Constructor used by StrusContext
	friend class StrusContext;
	explicit QueryAnalyzer( const Reference& objbuilder);

	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
};


/// \brief Object representing a client connection to the storage 
/// \remark The only way to construct a storage client instance is to call StrusContext::createStorageClient(const std::string&)
class StorageClient
{
public:
	/// \brief Copy constructor
	StorageClient( const StorageClient& o);

	/// \brief Destructor
	~StorageClient(){}

	/// \brief Get the number of documents inserted into the storage
	/// return the total number of documents
	GlobalCounter nofDocumentsInserted() const;

	/// \brief Prepare the inserting a document into the storage
	/// \param[in] docid the identifier of the document to insert
	/// \param[in] doc the structure of the document to insert
	/// \remark The document is physically inserted with the next implicit or explicit call of 'flush()'
	void insertDocument( const std::string& docid, const Document& doc);

	/// \brief Prepare the deletion of a document from the storage
	/// \param[in] docid the identifier of the document to delete
	/// \remark The document is physically deleted with the next implicit or explicit call of 'flush()'
	void deleteDocument( const std::string& docid);

	/// \brief Prepare the deletion of all document access rights of a user
	/// \param[in] username the name of the user to delete all access rights (in the local collection)
	/// \remark The user access rights are changed accordingly with the next implicit or explicit call of 'flush'
	void deleteUserAccessRights( const std::string& username);

	/// \brief Commit all insert or delete or user access right change statements open.
	void flush();

	/// \brief Close of the storage client
	void close();

private:
	friend class StrusContext;
	StorageClient( const Reference& objbuilder, const std::string& config);

	friend class Query;
	friend class QueryEval;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_transaction_impl;
};


class SummarizerConfig
{
public:
	/// \brief Constructor
	SummarizerConfig(){}
	/// \brief Copy constructor
	SummarizerConfig( const SummarizerConfig& o)
		:m_parameters(o.m_parameters),m_features(o.m_features){}

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
	std::map<std::string,Variant> m_parameters;
	std::map<std::string,std::string> m_features;
};

class WeightingConfig
{
public:
	WeightingConfig(){}
	WeightingConfig( const WeightingConfig& o)
		:m_parameters(o.m_parameters){}

	void defineParameter( const std::string& name_, const Variant& value_)
	{
		m_parameters[ name_] = value_;
	}

	/// \brief Define a weighting feature
	void defineFeature( const std::string& class_, const std::string& set_)
	{
		m_features[ class_] = set_;
	}

private:
	friend class QueryEval;
	std::map<std::string,Variant> m_parameters;
	std::map<std::string,std::string> m_features;
};

/// \brief Forward declaration
class Query;

/// \brief Query evaluation program object representing an information retrieval scheme for documents in a storage.
class QueryEval
{
public:
	/// \brief Copy constructor
	QueryEval( const QueryEval& o);
	/// \brief Destructor
	~QueryEval(){}

	/// \brief Declare a term that is used in the query evaluation as structural element without beeing part of the query (for example punctuation used for match phrases summarization)
	/// \param[in] set_ identifier of the term set that is used to address the terms
	/// \param[in] type_ feature type of the of the term
	/// \param[in] value_ feature value of the of the term
	void addTerm(
			const std::string& set_,
			const std::string& type_,
			const std::string& value_);

	/// \brief Declare a feature set to be used as selecting feature
	/// \param[in] set_ identifier of the term set addressing the terms to use for selection
	void addSelectionFeature( const std::string& set_);

	/// \brief Declare a feature set to be used as restriction
	/// \param[in] set_ identifier of the term set addressing the terms to use as restriction
	void addRestrictionFeature( const std::string& set_);

	/// \brief Declare a feature set to be used as exclusion
	/// \param[in] set_ identifier of the term set addressing the terms to use as exclusion
	void addExclusionFeature( const std::string& set_);

	/// \brief Declare a summarizer
	/// \param[in] resultAttribute name of the result attribute this summarization result is assigned to
	/// \param[in] name the name of the summarizer to add
	/// \param[in] config the configuration of the summarizer to add
	void addSummarizer(
			const std::string& resultAttribute,
			const std::string& name,
			const SummarizerConfig& config);

	/// \brief Add a weighting function to use as summand of the total document weight
	/// \param[in] weight additive weight of the feature (compared with other weighting functions added)
	/// \param[in] name the name of the weighting function to add
	/// \param[in] config the configuration of the function to add
	void addWeightingFunction(
			float weight,
			const std::string& name,
			const WeightingConfig& config);

	/// \brief Create a query builder based on this query evaluation scheme
	/// \param[in] storage storage to execute the query on
	Query* createQuery( const StorageClient& storage) const;

private:
	/// \brief Constructor used by strusContext
	friend class StrusContext;
	explicit QueryEval( const Reference& objbuilder);

	friend class Query;
	Reference m_objbuilder_impl;
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
	/// \brief Copy constructor
	Query( const Query& o);
	/// \brief Destructor
	~Query(){}

	/// \brief Push a single term on the stack
	/// \param[in] type_ query term type name
	/// \param[in] value_ query term value
	void pushTerm( const std::string& type_, const std::string& value_);

	/// \brief Create an expression from the topmost 'argc' elements of the stack, pop them from the stack and push the expression as single unit on the stack
	/// \param[in] opname_ name of the expression operator
	/// \param[in] argc number of operands (topmost elements from stack) of the expression
	/// \param[in] range_ range number for the expression span in the document
	void pushExpression( const std::string& opname_, unsigned int argc, int range_=0);

	/// \brief Push a duplicate of the topmost element of the query stack
	/// \note This function makes it possible to reference terms or expressions more than once as features or as subexpressions.
	void pushDuplicate();

	/// \brief Attaches a variable to the top expression or term on the query stack.
	/// \note The positions of the query matches of the referenced term or expression can be accessed through this variable in summarization.
	/// \param[in] name_ name of the variable attached
	/// \remark The stack is not changed
	void attachVariable( const std::string& name_);

	/// \brief Create a feature from the top element on the stack (and pop the element from the stack)
	/// \param[in] set_ name of the feature set, this feature is addressed with
	/// \param[in] weight_ individual weight of the feature in the query
	void defineFeature( const std::string& set_, float weight_=1.0);

	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] operand numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void defineMetaDataRestriction(
			const char* compareOp, const std::string& name,
			const Variant& operand, bool newGroup=true);

	/// \brief Set number of ranks to evaluate starting with the first rank (the maximum size of the result rank list)
	/// \param[in] maxNofRanks_ maximum number of results to return by this query
	void setMaxNofRanks( unsigned int maxNofRanks_);

	/// \brief Set the index of the first rank to be returned
	/// \param[in] minRank_ index of the first rank to be returned by this query
	void setMinRank( unsigned int minRank_);

	/// \brief Add a user for this query (as alternative role)
	/// \param[in] username_ an alternative name of a user for the evaluation of this query
	/// \note the user restriction applies if no user role specified in the query is allowed to see the document.
	void addUserName( const std::string& username_);

	/// \brief Evaluate this query and return the result
	/// \return the result
	std::vector<Rank> evaluate() const;

private:
	friend class QueryEval;
	Query( const Reference& objbuilder_impl_, const Reference& storage_impl_, const Reference& queryeval_impl_, const Reference& query_impl_)
		:m_objbuilder_impl(objbuilder_impl_),m_storage_impl(storage_impl_),m_queryeval_impl(queryeval_impl_),m_query_impl(query_impl_){}

	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_queryeval_impl;
	Reference m_query_impl;
};


/// \brief Object holding the global context of the strus information retrieval engine
/// \note There a two modes of this context object operating on a different base.
///	If you create this object without parameter, then the context is local.
///	In a local context you can load modules, define resources, etc. If you create
///	this object with a connection string as parameter, then all object created by
///	this context reside on the server (strusRpcServer) addressed with the connection string.
///	In this case loaded modules and resources are ignored. What modules to use is then
///	specified on server startup.
class StrusContext
{
public:
	/// \brief Constructor for local mode with own module loader
	StrusContext();
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	StrusContext( const char* connectionstring);
	/// \brief Copy constructor
	StrusContext( const StrusContext& o);
	/// \brief Destructor
	~StrusContext(){}

	/// \brief Load a module
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void loadModule( const std::string& name_);

	/// \brief Add a path from where to try to load modules from
	/// \param[in] paths semicolon separated list of module search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addModulePath( const std::string& paths_);

	/// \brief Define where to load analyzer resource files from
	/// \param[in] paths semicolon separated list of resource search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addResourcePath( const std::string& paths_);

	/// \brief Create a storage client instance
	/// \param[in] config_ configuration string of the storage client or empty, if the default remote storage of the RPC server is chosen,
	StorageClient createStorageClient( const std::string& config_);

	/// \brief Create a new storage (physically) described by config
	/// \remark Fails if the storage already exists
	void createStorage( const std::string& config_);

	/// \brief Delete the storage (physically) described by config
	/// \note Handle this function carefully
	void destroyStorage( const std::string& config_);

	/// \brief Detect the type of document from its content
	/// \param[in] the document content to classify
	/// \return the document class
	DocumentClass detectDocumentClass( const std::string& content);

	/// \brief Create a document analyzer instance
	/// \param[in] segmentername_ name of the segmenter to use (if empty then the default segmenter is used)
	DocumentAnalyzer createDocumentAnalyzer( const std::string& segmentername_="");

	/// \brief Create a query analyzer instance
	QueryAnalyzer createQueryAnalyzer();

	/// \brief Create a query evaluation instance
	QueryEval createQueryEval();

	/// \brief Force cleanup to circumvent object pooling mechanisms in an interpreter context
	void close();

private:
	void initStorageObjBuilder();
	void initAnalyzerObjBuilder();

private:
	Reference m_moduleloader_impl;
	Reference m_rpc_impl;
	Reference m_storage_objbuilder_impl;
	Reference m_analyzer_objbuilder_impl;
};

#endif


