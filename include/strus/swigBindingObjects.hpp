/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_OBJECTS_HPP_INCLUDED
#define _STRUS_BINDING_OBJECTS_HPP_INCLUDED
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

#ifdef DOXYGEN_LANG
#include "swig.hpp"
// ... doxygen needs this for creating alternative interface descriptions for different languages
#if defined DOXYGEN_JAVA
namespace net {
namespace strus {
namespace api {
#elif defined DOXYGEN_PYTHON
namespace strus {
#endif
#define String std::string
#define WString std::wstring
#define IndexVector std::vector<int>
#define StringVector std::vector<std::string>
#define FloatVector std::vector<double>
#if !defined DOXYGEN_PHP && !defined DOXYGEN_PYTHON
#define NormalizerVector std::vector<Normalizer>
#endif
#define TermVector std::vector<Term>
#define QueryTermVector std::vector<QueryTerm>
#define RankVector std::vector<Rank>
#define VecRankVector std::vector<VecRank>
#define SummaryElementVector std::vector<SummaryElement>
#define AttributeVector std::vector<Attribute>
#define MetaDataVector std::vector<MetaData>
#define DocumentFrequencyChangeVector std::vector<DocumentFrequencyChange> 
typedef int Index;
typedef long GlobalCounter;

#ifndef DOXYGEN_LANG
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
#endif


#if defined DOXYGEN_PHP || defined DOXYGEN_PYTHON
/// \brief Object epresenting a configuration of a tokenizer as single string naming a tokenizer without arguments or a tuple of strings consisting of the tokenizer name followed by the arguments
#error DOXYGEN_PYTHON
class Tokenizer
{
};
#else
/// \brief Object representing a tokenizer function definition
class Tokenizer
{
public:
	Tokenizer(){}
	Tokenizer( const Tokenizer& o)
		:m_name(o.m_name),m_arguments(o.m_arguments){}
	Tokenizer( const String& name_, const StringVector& arg_)
		:m_name(name_),m_arguments(arg_){}
	Tokenizer( const String& name_, const String& arg_)
		:m_name(name_)
	{
		m_arguments.push_back( arg_);
	}
	Tokenizer( const String& name_, const String& arg1_, const String& arg2_)
		:m_name(name_),m_arguments()
	{
		m_arguments.push_back( arg1_);
		m_arguments.push_back( arg2_);
	}
	Tokenizer( const String& name_)
		:m_name(name_),m_arguments(){}

	const String& name() const				{return m_name;}
	const StringVector& arguments() const			{return m_arguments;}

	void setName( const String& name_)			{m_name = name_;}
	void addArgument( const String& arg_)			{m_arguments.push_back( arg_);}
	void addArgumentInt( long arg_);
	void addArgumentFloat( double arg_);

private:
	std::string m_name;
	std::vector<std::string> m_arguments;
};
#endif


#if defined DOXYGEN_PHP || defined DOXYGEN_PYTHON
/// \brief Object representing a configuration of a normalizer as single string naming a normalizer without arguments or a tuple of strings consisting of the normalizer name followed by the arguments
class Normalizer
{
};
#else
/// \brief Object representing a normalizer function definition
class Normalizer
{
public:
	Normalizer(){}
	Normalizer( const Normalizer& o)
		:m_name(o.m_name),m_arguments(o.m_arguments){}
	Normalizer( const String& name_, const StringVector& arg_)
		:m_name(name_),m_arguments(arg_){}
	Normalizer( const String& name_, const String& arg_)
		:m_name(name_)
	{
		m_arguments.push_back( arg_);
	}
	Normalizer( const String& name_, const String& arg1_, const String& arg2_)
		:m_name(name_),m_arguments()
	{
		m_arguments.push_back( arg1_);
		m_arguments.push_back( arg2_);
	}
	Normalizer( const String& name_)
		:m_name(name_),m_arguments(){}

	~Normalizer(){}

	const String& name() const			{return m_name;}
	const StringVector& arguments() const		{return m_arguments;}

	void setName( const String& name_)		{m_name = name_;}
	void addArgument( const String& arg_)		{m_arguments.push_back( arg_);}
	void addArgumentInt( long arg_);
	void addArgumentFloat( double arg_);

private:
	friend class QueryAnalyzer;
	friend class DocumentAnalyzer;

	std::string m_name;
	std::vector<std::string> m_arguments;
};
#endif

#if defined DOXYGEN_PHP || defined DOXYGEN_PYTHON
/// \brief A sequence of normalizer definitions represented as single string naming a normalizer without arguments or list of strings tuples consisting of the normalizer name followed by the arguments
class NormalizerVector
{
};
#endif

#if defined DOXYGEN_PHP || defined DOXYGEN_PYTHON
/// \brief Object representing a configuration of an aggregator function as single string naming an aggregator without arguments or a tuple of strings consisting of the aggregator name followed by the arguments
class Aggregator
{
};
#else
/// \brief Object representing a aggregator function definition
class Aggregator
{
public:
	/// \brief Default constructor
	Aggregator(){}
	/// \brief Copy constructor
	Aggregator( const Aggregator& o)
		:m_name(o.m_name),m_arguments(o.m_arguments){}
	/// \brief Constructor
	/// \param[in] name_ name of the aggregator function
	/// \param[in] arg_ arguments of the aggregator function
	Aggregator( const String& name_, const StringVector& arg_)
		:m_name(name_),m_arguments(arg_){}
	/// \brief Constructor
	/// \param[in] name_ name of the aggregator function
	/// \param[in] arg_ single argument of the aggregator function
	Aggregator( const String& name_, const String& arg_)
		:m_name(name_)
	{
		m_arguments.push_back( arg_);
	}
	/// \brief Constructor
	/// \param[in] name_ name of the aggregator function
	/// \param[in] arg1_ first argument of the aggregator function
	/// \param[in] arg2_ second argument of the aggregator function
	Aggregator( const String& name_, const String& arg1_, const String& arg2_)
		:m_name(name_),m_arguments()
	{
		m_arguments.push_back( arg1_);
		m_arguments.push_back( arg2_);
	}
	/// \brief Constructor
	/// \param[in] name_ name of the aggregator function
	Aggregator( const String& name_)
		:m_name(name_),m_arguments(){}

	/// \brief Destructor
	~Aggregator(){}

	/// \brief Get the name of the aggregator function
	/// \return the name
	const String& name() const			{return m_name;}
	/// \brief Get the arguments of the aggregator function
	/// \return the arguments
	const StringVector& arguments() const		{return m_arguments;}

	/// \brief Set the name of the aggregator function
	/// \param[in] name_ the name
	void setName( const String& name_)		{m_name = name_;}
	/// \brief Add an argument to the aggregator function
	/// \param[in] arg_ the argument to add
	void addArgument( const String& arg_)		{m_arguments.push_back( arg_);}
	/// \brief Add a numeric argument to the aggregator function
	/// \param[in] arg_ the argument to add
	void addArgumentInt( long arg_);
	/// \brief Add a numeric argument to the aggregator function
	/// \param[in] arg_ the argument to add
	void addArgumentFloat( double arg_);

private:
	friend class DocumentAnalyzer;

	std::string m_name;
	std::vector<std::string> m_arguments;
};
#endif

#if defined DOXYGEN_LANG
/// \brief Object representing a string or a numeric value of the binding language
class Variant
{
};
#else
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
	double FLOAT;
	const char* TEXT;
};

/// \brief Variant type for passing parameter values of arbitrary type
class Variant
{
public:
	/// \brief Default constructor
	Variant();
	/// \brief Copy constructor
	Variant( const Variant& o);
	/// \brief Constructor from a nonnegative integer
	Variant( unsigned int v);
	/// \brief Constructor from an integer
	Variant( int v);
	/// \brief Constructor from a floating point number
	Variant( double v);
	/// \brief Constructor from a string
	Variant( const String& v);
	/// \brief Constructor from a string pointer
	Variant( const char* v);

	/// \brief Check if the variant is defined (not NULL)
	/// \return true, if yes, false if not
	bool defined() const				{return m_type != Variant_UNDEFINED;}
	/// \brief Get the type of the variant 
	/// \return the type
	VariantType type() const			{return m_type;}
	/// \brief Get the value of the variant as nonnegative integer
	/// \return the value
	unsigned long getUInt() const;
	/// \brief Get the value of the variant as integer
	/// \return the value
	long getInt() const;
	/// \brief Get the value of the variant as floating point number
	/// \return the value
	double getFloat() const;
	/// \brief Get the value of the variant as pointer to a string
	/// \return the value
	const char* getText() const;

	/// \brief Initialize this value as undefined (NULL)
	void init();
	/// \brief Assign a value to this variant
	/// \param[in] o the value to assign
	void assign( const Variant& o);
	/// \brief Assign a value to this variant
	/// \param[in] v the value to assign
	void assignUint( unsigned long v);
	/// \brief Assign a value to this variant
	/// \param[in] v the value to assign
	void assignInt( long v);
	/// \brief Assign a value to this variant
	/// \param[in] v the value to assign
	void assignFloat( double v);
	/// \brief Assign a value to this variant
	/// \param[in] v the value to assign
	void assignText( const String& v);

	Variant& operator=( const Variant& o)
	{
		assign( o);
		return *this;
	}

private:
	bool isEqual( const Variant& o) const;

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
#endif


/// \brief One typed term occurrence in a document or a query
class Term
{
public:
	/// \brief Constructor
	Term( const String& type_, const String& value_, const Index& position_, const Index& length_)
		:m_type(type_),m_value(value_),m_position(position_),m_length(length_){}
	/// \brief Copy constructor
	Term( const Term& o)
		:m_type(o.m_type),m_value(o.m_value),m_position(o.m_position),m_length(o.m_length){}
	/// \brief Default constructor
	Term()
		:m_position(0),m_length(0){}

	/// \brief Get the term type name
	const String& type() const			{return m_type;}
	/// \brief Get the term value
	const String& value() const			{return m_value;}
	/// \brief Get the term position
	Index position() const				{return m_position;}
	/// \brief Get the term length
	Index length() const				{return m_length;}

#ifndef SWIG
	bool operator < (const Term& o) const;
#endif
private:
	std::string m_type;
	std::string m_value;
	Index m_position;
	Index m_length;
};


/// \brief Data object that represents single numeric property of a document that
///	can be subject of retrieval or act as search restriction.
class MetaData
{
public:
	/// \brief Constructor
	MetaData( const String& name_, const Variant& value_)
		:m_name(name_),m_value(value_){}
	/// \brief Copy constructor
	MetaData( const MetaData& o)
		:m_name(o.m_name),m_value(o.m_value){}
	/// \brief Default constructor
	MetaData()
		:m_name(0),m_value(){}

	/// \brief Get the type name of this meta data field:
	const String& name() const			{return m_name;}
	/// \brief Get the value of this meta data field:
	double value() const				{return m_value.getFloat();}

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
	Attribute( const String& name_, const String& value_)
		:m_name(name_),m_value(value_){}
	/// \brief Constructor
	Attribute( const Attribute& o)
		:m_name(o.m_name),m_value(o.m_value){}
	/// \brief Constructor
	Attribute()
		:m_name(0){}

	/// \brief Get the unique type name of this attribute
	const String& name() const		{return m_name;}
	/// \brief Get the type value of this attribute
	const String& value() const		{return m_value;}
	
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
			const String& mimeType_)	:m_mimeType(mimeType_){}
	/// \brief Constructor
	DocumentClass(
			const String& mimeType_,
			const String& encoding_)	:m_mimeType(mimeType_),m_encoding(encoding_){}
	/// \brief Constructor
	DocumentClass(
			const String& mimeType_,
			const String& encoding_,
			const String& scheme_)		:m_mimeType(mimeType_),m_scheme(scheme_),m_encoding(encoding_){}
	/// \brief Copy constructor
	DocumentClass( const DocumentClass& o)		:m_mimeType(o.m_mimeType),m_scheme(o.m_scheme),m_encoding(o.m_encoding){}

	/// \brief Check if this document class is valid
	/// \return true, if yes
	bool valid() const				{return !m_mimeType.empty();}

	/// \brief Set the MIME type of the document class
	/// \param[in] mimeType_ the document MIME type string
	void setMimeType( const String& mimeType_)	{m_mimeType = mimeType_;}
	/// \brief Set the scheme identifier of the document class
	/// \param[in] scheme_ the document scheme identifier
	void setScheme( const String& scheme_)		{m_scheme = scheme_;}
	/// \brief Set the character set encoding of the document class
	/// \param[in] encoding_ the character set encoding as string
	void setEncoding( const String& encoding_)	{m_encoding = encoding_;}

	/// \brief Get the MIME type of the document class
	/// \return the document MIME type string
	const String& mimeType() const			{return m_mimeType;}
	/// \brief Get the scheme identifier of the document class
	/// \return the document scheme identifier
	const String& scheme() const			{return m_scheme;}
	/// \brief Get the character set encoding of the document class
	/// \return the character set encoding string
	const String& encoding() const			{return m_encoding;}

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
	/// \param[in] type term type name of the search index term
	/// \param[in] value term value of the search index term
	/// \param[in] position word count position of the search index term
	void addSearchIndexTerm( const String& type, const String& value, const Index& position);
	/// \brief Add a single term occurrence to the document for use in the summary of a retrieval result
	/// \param[in] type term type name of the forward index term
	/// \param[in] value term value of the forward index term
	/// \param[in] position word count position of the forward index term
	void addForwardIndexTerm( const String& type, const String& value, const Index& position);
#ifndef DOXYGEN_JAVA
	/// \brief Define a meta data value of the document
	/// \param[in] name name of the meta data table element
	/// \param[in] value value of the meta data table element
	void setMetaData( const String& name, const Variant& value);
#endif
	/// \brief Define a meta data value of the document
	/// \param[in] name name of the meta data table element
	/// \param[in] value value of the meta data table element
	void setMetaData( const String& name, double value);
	/// \brief Define a meta data value of the document
	/// \param[in] name name of the meta data table element
	/// \param[in] value value of the meta data table element
	void setMetaData( const String& name, int value);
	/// \brief Define a meta data value of the document
	/// \param[in] name name of the meta data table element
	/// \param[in] value value of the meta data table element
	void setMetaData( const String& name, unsigned int value);
	/// \brief Define an attribute of the document
	/// \param[in] name name of the document attribute
	/// \param[in] value value of the document attribute
	void setAttribute( const String& name, const String& value);
	/// \brief Allow a user to access the document
	/// \param[in] username name of the user to be allowed to access this document
	/// \remark This function is only implemented if ACL is enabled in the storage
	void setUserAccessRight( const String& username);
	/// \brief Set the document identifier (docid) of the document
	/// \param[in] docid_ document identifier of this document
	void setDocid( const String& docid_);

	/// \brief Get the list of search terms of the document
	/// \return the list of terms
	const TermVector& searchIndexTerms() const		{return m_searchIndexTerms;}
	/// \brief Get the list of forward terms of the document
	/// \return the list of terms
	const TermVector& forwardIndexTerms() const		{return m_forwardIndexTerms;}
	/// \brief Get the list of meta data of the document
	/// \return the list of meta data definitions
	const MetaDataVector& metaData() const			{return m_metaData;}
	/// \brief Get the list of attributes of the document
	/// \return the list of attributes
	const AttributeVector& attributes() const		{return m_attributes;}
	/// \brief Get the list of users that are allowed to access the document
	/// \return the list of users allowed to access this document
	const StringVector& users() const			{return m_users;}
	/// \brief Get the document identifier (docid) of the document
	/// \return the document identifier
	const String& docid() const				{return m_docid;}

private:
	std::vector<Term> m_searchIndexTerms;
	std::vector<Term> m_forwardIndexTerms;
	std::vector<MetaData> m_metaData;
	std::vector<Attribute> m_attributes;
	std::vector<std::string> m_users;
	std::string m_docid;
};


/// \brief Forward declaration
class DocumentAnalyzeQueue;
/// \brief Forward declaration
class PatternMatcher;

/// \class DocumentAnalyzer
/// \brief Analyzer object representing a program for segmenting, 
///	tokenizing and normalizing a document into atomic parts, that 
///	can be inserted into a storage and be retrieved from there.
/// \remark The only way to construct a document analyzer instance is to call Context::createDocumentAnalyzer()
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
	/// \param[in] options a list of options as string, elements separated by ',', one of {"BindPosPred" => the position is bound to the preceeding feature, "BindPosSucc" => the position is bound to the succeeding feature}
#if defined SWIGPHP
	// ... SWIG PHP implementation cannot handle signatures with typemaps and default parameters (!)
	void addSearchIndexFeature(
		const String& type,
		const String& selectexpr,
		const Tokenizer& tokenizer,
		const NormalizerVector& normalizers,
		const String& options);
#else
	void addSearchIndexFeature(
		const String& type,
		const String& selectexpr,
		const Tokenizer& tokenizer,
		const NormalizerVector& normalizers,
		const String& options=String());
#endif

	/// \brief Define how a feature to insert into the forward index (for summarization) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	/// \param[in] options a list of options as string, elements separated by ',', one of {"BindPosPred" => the position is bound to the preceeding feature, "BindPosSucc" => the position is bound to the succeeding feature}
#if defined SWIGPHP
	// ... SWIG PHP implementation cannot handle signatures with typemaps and default parameters (!)
	void addForwardIndexFeature(
		const String& type,
		const String& selectexpr,
		const Tokenizer& tokenizer,
		const NormalizerVector& normalizers,
		const String& options);
#else
	void addForwardIndexFeature(
		const String& type,
		const String& selectexpr,
		const Tokenizer& tokenizer,
		const NormalizerVector& normalizers,
		const String& options=String());
#endif

	/// \brief Define how a feature to insert as meta data is selected, tokenized and normalized
	/// \param[in] fieldname name of the addressed meta data field.
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	void defineMetaData(
		const String& fieldname,
		const String& selectexpr,
		const Tokenizer& tokenizer,
		const NormalizerVector& normalizers);

	/// \brief Declare some aggregated value of the document to be put into the meta data table used for restrictions, weighting and summarization.
 	/// \param[in] fieldname name of the addressed meta data field.
	/// \param[in] function defining how and from what the value is aggregated
	void defineAggregatedMetaData(
		const String& fieldname,
		const Aggregator& function);

	/// \brief Define how a feature to insert as document attribute (for summarization) is selected, tokenized and normalized
	/// \param[in] attribname name of the addressed attribute.
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	void defineAttribute(
		const String& attribname,
		const String& selectexpr,
		const Tokenizer& tokenizer,
		const NormalizerVector& normalizers);

#if defined SWIGPHP
	// ... SWIG PHP implementation cannot handle signatures with typemaps and default parameters (!)
	void addSearchIndexFeatureFromPatternMatch(
		const String& type,
		const String& patternTypeName,
		const NormalizerVector& normalizers,
		const String& options);
#else
	void addSearchIndexFeatureFromPatternMatch(
		const String& type,
		const String& patternTypeName,
		const NormalizerVector& normalizers,
		const String& options=String());
#endif
#if defined SWIGPHP
	// ... SWIG PHP implementation cannot handle signatures with typemaps and default parameters (!)
	void addForwardIndexFeatureFromPatternMatch(
		const String& type,
		const String& patternTypeName,
		const NormalizerVector& normalizers,
		const String& options);
#else
	void addForwardIndexFeatureFromPatternMatch(
		const String& type,
		const String& patternTypeName,
		const NormalizerVector& normalizers,
		const String& options=String());
#endif
	void defineMetaDataFromPatternMatch(
		const String& fieldname,
		const String& patternTypeName,
		const NormalizerVector& normalizers);

	void defineAttributeFromPatternMatch(
		const String& attribname,
		const String& patternTypeName,
		const NormalizerVector& normalizers);

	/// \brief Declare a pattern matcher on the document features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \param[in] patterns structure with all patterns
	void definePatternMatcherPostProc(
			const String& patternTypeName,
			const String& patternMatcherModule,
			const PatternMatcher& patterns);

	/// \brief Declare a pattern matcher on the document features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \param[in] serializedPatternFile path to file with serialized (binary) patterns
	void definePatternMatcherPostProcFromFile(
			const String& patternTypeName,
			const String& patternMatcherModule,
			const String& serializedPatternFile);

	/// \brief Declare a sub document for the handling of multi part documents in an analyzed content
	/// \param[in] selectexpr an expression that defines the content of the sub document declared
	/// \param[in] subDocumentTypeName type name assinged to this sub document
	/// \remark Sub documents are defined as the sections selected by the expression plus some data selected not belonging to any sub document.
	void defineDocument(
		const String& subDocumentTypeName,
		const String& selectexpr);

	/// \brief Analye the content and return the set of features to insert
	/// \param[in] content string (NOT a file name !) of the document to analyze
	Document analyze( const String& content);

	/// \brief Analye the content and return the set of features to insert
	/// \param[in] content string (NOT a file name !) of the document to analyze
	/// \param[in] dclass document class of the document to analyze
	Document analyze( const String& content, const DocumentClass& dclass);

	/// \brief Creates a queue for multi document analysis
	/// \return the queue
	DocumentAnalyzeQueue createQueue() const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	DocumentAnalyzer( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd, const String& segmentername, const void* textproc_);

	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
	const void* m_textproc;
};


/// \class DocumentAnalyzeQueue
/// \brief Analyzer object implementing a queue of analyze document tasks.
/// \remark Document analysis with this class reduces network roundtrips when using it as proxy
class DocumentAnalyzeQueue
{
public:
	/// \brief Copy constructor
	DocumentAnalyzeQueue( const DocumentAnalyzeQueue& o);
	/// \brief Destructor
	~DocumentAnalyzeQueue(){}

	/// \brief Push a document into the queue to analyze
	/// \param[in] content content string of the document to analyze
	void push( const String& content);
	/// \brief Push a document into the queue to analyze
	/// \param[in] content content string of the document to analyze
	/// \param[in] dclass document class of the document to analyze
	void push( const String& content, const DocumentClass& dclass);

	/// \brief Checks if there are more results to fetch
	/// \return true, if yes
	bool hasMore() const;

	/// \brief Processes the next field of the queue for fields to analyzer. Does the tokenization and normalization and creates some typed terms out of it according the definition of the field type given.
	/// \return list of terms (query field analyzer result)
	Document fetch();

private:
	void analyzeNext();

private:
	/// \brief Constructor used by Context
	friend class DocumentAnalyzer;
	explicit DocumentAnalyzeQueue( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd, const Reference& analyzer, const void* textproc_);

	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
	std::vector<Document> m_result_queue;
	std::size_t m_result_queue_idx;
	std::vector<Reference> m_analyzerctx_queue;
	std::size_t m_analyzerctx_queue_idx;
	const void* m_textproc;
};


/// \brief Forward declaration
class QueryAnalyzeContext;

/// \brief Object representing a pattern match program
class PatternMatcher
{
public:
	PatternMatcher( const PatternMatcher& o)
		:m_ops(o.m_ops),m_strings(o.m_strings),m_size(o.m_size){}
	PatternMatcher()
		:m_size(0){}

	/// \brief Add operation push term
	/// \param[in] type_ query term type name
	/// \param[in] value_ query term value
	void pushTerm( const String& type_, const String& value_);

	/// \brief Add operation push term
	/// \param[in] name_ name of the referenced pattern
	void pushPattern( const String& name_);

	/// \brief Add operation push expression
	/// \param[in] opname_ name of the expression operator
	/// \param[in] argc_ number of operands (topmost elements from stack) of the expression
	/// \param[in] range_ range number for the expression span in the document
	/// \param[in] cardinality_ number that specifies the minimum size of a subset or subset permutation to match
	void pushExpression( const String& opname_, unsigned int argc_, int range_=0, unsigned int cardinality_=0);

	/// \brief Add operation push pattern
	/// \param[in] name_ name of the pattern created
	void definePattern( const String& name_, bool visible);

	/// \brief Add operation attach variable
	/// \note The positions of the query matches of the referenced term or expression can be accessed through this variable in summarization.
	/// \param[in] name_ name of the variable attached
	/// \remark The stack is not changed
	void attachVariable( const String& name_);

	/// \brief Appends the operations of 'o' to this
	/// \param[in] o expression to copy
	void add( const PatternMatcher& o);

	/// \brief Get the number of items (sub expressions) on the stack as result of this expression
	/// \return the number of items (sub expressions)
	std::size_t size() const
	{
		return m_size;
	}

private:
	friend class Query;

	struct StackOp
	{
		enum Type
		{
			PushTerm,
			PushPattern,
			PushExpression,
			DefinePattern,
			AttachVariable
		};
		enum ArgIndex
		{
			Term_type=0x0,
			Term_value=0x1,
			Expression_opname=0x0,
			Expression_argc=0x1,
			Expression_range=0x2,
			Expression_cardinality=0x3,
			Variable_name=0x0,
			Pattern_name=0x0,
			Pattern_visible=0x1
		};
		Type type;
		int arg[4];

		StackOp( Type type_, int arg0_, int arg1_=0, int arg2_=0, int arg3_=0)
			:type(type_)
		{
			arg[0] = arg0_;
			arg[1] = arg1_;
			arg[2] = arg2_;
			arg[3] = arg3_;
		}

		StackOp()
			:type(PushTerm)
		{
			std::memset( arg, 0, sizeof(arg));
		}
		StackOp( const StackOp& o)
		{
			std::memcpy( this, &o, sizeof(*this));
		}
	};
	const std::vector<StackOp>& ops() const
	{
		return m_ops;
	}
	const std::string& strings() const
	{
		return m_strings;
	}

private:
	std::size_t allocid( const String& str);

private:
	friend class QueryAnalyzer;
	friend class DocumentAnalyzer;
	friend class PatternMatchLoader;
	std::vector<StackOp> m_ops;
	std::string m_strings;
	std::size_t m_size;
};


/// \class QueryAnalyzer
/// \brief Analyzer object representing a set of function for transforming a field,
///	the smallest unit in any query language, to a set of terms that can be used
///	to build a query.
/// \remark The only way to construct a query analyzer instance is to call Context::createQueryAnalyzer()
class QueryAnalyzer
{
public:
	/// \brief Copy constructor
	QueryAnalyzer( const QueryAnalyzer& o);
	/// \brief Destructor
	~QueryAnalyzer(){}

	/// \brief Defines a search index element.
	/// \param[in] featureType element feature type created from this field type
	/// \param[in] fieldType name of the field type defined
	/// \param[in] tokenizer tokenizer function description to use for the features of this field type
	/// \param[in] normalizers list of normalizer function description to use for the features of this field type in the ascending order of appearance
	void addSearchIndexElement(
			const String& featureType,
			const String& fieldType,
			const Tokenizer& tokenizer,
			const NormalizerVector& normalizers);

	/// \brief Defines a search index element from a pattern matching result.
	/// \param[in] type element type created from this pattern match result type
	/// \param[in] patternTypeName name of the pattern match result item
	/// \param[in] normalizers list of normalizer functions
	void addSearchIndexElementFromPatternMatch(
			const std::string& type,
			const std::string& patternTypeName,
			const NormalizerVector& normalizers);

	/// \brief Declare an element to be used as lexem by post processing pattern matching but not put into the result of query analysis
	/// \param[in] termtype term type name of the lexem to be feed to the pattern matching
	/// \param[in] fieldtype type of the field of this element in the query
	/// \param[in] tokenizer tokenizer function description to use for the features of this field type
	/// \param[in] normalizers list of normalizer function description to use for the features of this field type in the ascending order of appearance
	void addPatternLexem(
			const std::string& termtype,
			const std::string& fieldtype,
			const Tokenizer& tokenizer,
			const NormalizerVector& normalizers);

	/// \brief Declare a pattern matcher on the query features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \param[in] patterns structure with all patterns
	void definePatternMatcherPostProc(
			const String& patternTypeName,
			const String& patternMatcherModule,
			const PatternMatcher& patterns);

	/// \brief Declare a pattern matcher on the query features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \param[in] serializedPatternFile path to file with serialized (binary) patterns
	void definePatternMatcherPostProcFromFile(
			const String& patternTypeName,
			const String& patternMatcherModule,
			const String& serializedPatternFile);

	/// \brief Tokenizes and normalizes a query field and creates some typed terms out of it according the definition of the field type given.
	/// \param[in] fieldType name of the field type to use for analysis
	/// \param[in] fieldContent content string of the query field to analyze
	/// \note This is a very simplistic method to analyze a query. For multi field queries the object QueryAnalyzeContext is more appropriate
	TermVector analyzeField(
			const String& fieldType,
			const String& fieldContent);

	/// \brief Creates a context for analyzing a multipart query
	/// \return the queue
	QueryAnalyzeContext createContext() const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	QueryAnalyzer( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd);

	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
};


/// \class QueryTerm
/// \brief Query analyzer term with info about the field it originated from.
class QueryTerm :public Term
{
public:
	/// \brief Constructor
	QueryTerm( const Index& field_, const String& type_, const String& value_, const Index& position_, const Index& length_)
		:Term(type_,value_,position_, length_),m_field(field_){}
	/// \brief Copy constructor
	QueryTerm( const QueryTerm& o)
		:Term(o),m_field(o.m_field){}
	/// \brief Default constructor
	QueryTerm()
		:Term(),m_field(0){}

	/// \brief Get the term position
	unsigned int field() const			{return m_field;}
	/// \brief Get the term type name
	const String& type() const			{return Term::type();}
	/// \brief Get the term value
	const String& value() const			{return Term::value();}
	/// \brief Get the term position
	Index position() const				{return Term::position();}

private:
	Index m_field;
};

/// \class QueryAnalyzeContext
/// \brief Query analyzer context for analysing a multipart query.
class QueryAnalyzeContext
{
public:
	/// \brief Copy constructor
	QueryAnalyzeContext( const QueryAnalyzeContext& o);
	/// \brief Destructor
	~QueryAnalyzeContext(){}

	/// \brief Define a query field
	/// \param[in] fieldNo index given to the field by the caller, to identify its results
	/// \param[in] fieldType name of the field type to use for analysis
	/// \param[in] fieldContent content string of the query field to analyze
	void putField(
			unsigned int fieldNo, 
			const String& fieldType,
			const String& fieldContent);

	/// \brief Processes the next field of the queue for fields to analyzer. Does the tokenization and normalization and creates some typed terms out of it according the definition of the field type given.
	/// \return list of terms (query field analyzer result)
	QueryTermVector analyze();

private:
	/// \brief Constructor used by Context
	friend class QueryAnalyzer;
	explicit QueryAnalyzeContext( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd, const Reference& analyzer);

	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
	Reference m_analyzer_ctx_impl;
};

/// \brief Forward declaration
class StatisticsIterator;
/// \brief Forward declaration
class StorageTransaction;
/// \brief Forward declaration
class DocumentBrowser;

/// \brief Object representing a client connection to the storage 
/// \remark The only way to construct a storage client instance is to call Context::createStorageClient(const std::string&)
class StorageClient
{
public:
	/// \brief Copy constructor
	StorageClient( const StorageClient& o);

	/// \brief Destructor
	~StorageClient(){}

	/// \brief Get the number of documents inserted into the storage
	/// return the total number of documents
	Index nofDocumentsInserted() const;

	/// \brief Create a transaction
	/// return the transaction object created
	StorageTransaction createTransaction() const;

	/// \brief Create an iterator on the storage statistics (total value) to distribute for initialization/deinitialization
	/// \param[in] sign true = registration, false = deregistration
	/// return the statistics iterator object created
	StatisticsIterator createInitStatisticsIterator( bool sign) const;

	/// \brief Create an iterator on the storage statistics (relative value) to distribute after storage updates
	/// return the statistics message iterator object created
	StatisticsIterator createUpdateStatisticsIterator() const;

	/// \brief Create a document browser instance
	DocumentBrowser createDocumentBrowser();

	/// \brief Close of the storage client
	void close();

private:
	friend class Context;
	StorageClient( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd_, const String& config);

	friend class Query;
	friend class QueryEval;
	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
};


/// \brief Object representing a transaction of the storage 
/// \remark The only way to construct a storage transaction instance is to call StorageClient::createTransaction()
class StorageTransaction
{
public:
	/// \brief Destructor
	~StorageTransaction(){}

	/// \brief Prepare the inserting a document into the storage
	/// \param[in] docid the identifier of the document to insert
	/// \param[in] doc the structure of the document to insert
	/// \remark The document is physically inserted with the call of 'commit()'
	void insertDocument( const String& docid, const Document& doc);

	/// \brief Prepare the deletion of a document from the storage
	/// \param[in] docid the identifier of the document to delete
	/// \remark The document is physically deleted with the call of 'commit()'
	void deleteDocument( const String& docid);

	/// \brief Prepare the deletion of all document access rights of a user
	/// \param[in] username the name of the user to delete all access rights (in the local collection)
	/// \remark The user access rights are changed accordingly with the next implicit or explicit call of 'flush'
	void deleteUserAccessRights( const String& username);

	/// \brief Commit all insert or delete or user access right change statements of this transaction.
	void commit();

	/// \brief Rollback all insert or delete or user access right change statements of this transaction.
	void rollback();

private:
	friend class StorageClient;
	StorageTransaction( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd_, const Reference& storage_);

	friend class Query;
	friend class QueryEval;
	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_transaction_impl;
};


/// \brief Structure describing the document frequency change of one term in the collection
class DocumentFrequencyChange
{
public:
	/// \brief Constructor
	/// \param[in] type_ term type
	/// \param[in] value_ term value
	/// \param[in] increment_ df change increment/decrement
	DocumentFrequencyChange( const String& type_, const String& value_, int increment_)
		:m_type(type_),m_value(value_),m_increment(increment_){}
	/// \brief Copy constructor
	DocumentFrequencyChange( const DocumentFrequencyChange& o)
		:m_type(o.m_type),m_value(o.m_value),m_increment(o.m_increment){}
	/// \brief Default constructor
	DocumentFrequencyChange()
		:m_increment(0){}

	/// \brief Get the term type name
	const String& type() const			{return m_type;}
	/// \brief Get the term value
	const String& value() const			{return m_value;}
	/// \brief Get the term increment
	int increment() const				{return m_increment;}

private:
	std::string m_type;
	std::string m_value;
	int m_increment;
};


/// \brief Message with storage statistics
class StatisticsMessage
{
public:
	/// \brief Copy constructor
	StatisticsMessage( const StatisticsMessage& o)
		:m_nofDocumentsInsertedChange(o.m_nofDocumentsInsertedChange)
		,m_documentFrequencyChangeList(o.m_documentFrequencyChangeList){}

	/// \brief Constructor from elements
	StatisticsMessage( const std::vector<DocumentFrequencyChange>& dfchglist, int nofdocs)
		:m_nofDocumentsInsertedChange(nofdocs)
		,m_documentFrequencyChangeList(dfchglist){}

	/// \brief Return the change of number of documents inserted
	/// \return the change of number of documents
	int nofDocumentsInsertedChange() const
	{
		return m_nofDocumentsInsertedChange;
	}

	/// \brief Return the list of document frequency changes
	/// \return the list of df changes
	const std::vector<DocumentFrequencyChange>& documentFrequencyChangeList() const
	{
		return m_documentFrequencyChangeList;
	}

private:
	int m_nofDocumentsInsertedChange;
	std::vector<DocumentFrequencyChange> m_documentFrequencyChangeList;
};


/// \brief Iterator on messages with storage statistics
class StatisticsIterator
{
public:
	/// \brief Copy constructor
	StatisticsIterator( const StatisticsIterator& o);

	/// \brief Fetches the next statistics message
	/// \return message blob or empty string if there is no message left
	String getNext();

private:
	friend class StorageClient;
	StatisticsIterator( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd_, const Reference& storage_, const Reference& iter_);

private:
	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_iter_impl;
};


/// \brief Translation to update a storage with statistics messages
class StatisticsProcessor
{
public:
	/// \brief Decode a statistics message blob for introspection
	/// \param[in] blob statistics message blob
	/// \return the statistics message
	StatisticsMessage decode( const String& blob) const;
	/// \brief Create binary blob from statistics message
	/// \param[in] msg statistics message structure
	/// \return the statistics message blob
	String encode( const StatisticsMessage& msg) const;

private:
	friend class Context;
	StatisticsProcessor( const Reference& objbuilder_, const Reference& trace_, const std::string& name_, const Reference& errorhnd_);

private:
	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	const void* m_statsproc;
};


/// \brief Weighted vector (result of a query vector storage search)
class VecRank
{
public:
	/// \brief Constructor
	VecRank()
		:m_index(0),m_weight(0.0){}
	/// \brief Constructor
	VecRank( Index index_, double weight_)
		:m_index(index_),m_weight(weight_){}
	/// \brief Copy constructor
	VecRank( const VecRank& o)
		:m_index(o.m_index),m_weight(o.m_weight){}

	/// \brief Get the feature number ( >= 0) from this
	Index index() const					{return m_index;}
	/// \brief Get the weight of the vector rank
	double weight() const					{return m_weight;}
	/// \brief Get the summary elements

private:
	Index m_index;
	double m_weight;
};

class VectorStorageSearcher
{
public:
	/// \brief Copy constructor
	VectorStorageSearcher( const VectorStorageSearcher& o);

	/// \brief Destructor
	~VectorStorageSearcher(){}

	/// \brief Find the most similar vectors to vector
	/// \param[in] vec vector to search for
	/// \param[in] maxNofResults maximum number of results to return
	/// return the list of most similar vectors
	VecRankVector findSimilar( const FloatVector& vec, unsigned int maxNofResults) const;

	/// \brief Find the most similar vectors to vector in a selection of features addressed by index
	/// \param[in] featidxlist list of candidate indices
	/// \param[in] vec vector to search for
	/// \param[in] maxNofResults maximum number of results to return
	/// return the list of most similar vectors
	VecRankVector findSimilarFromSelection( const IndexVector& featidxlist, const FloatVector& vec, unsigned int maxNofResults) const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClient;
	VectorStorageSearcher( const Reference& storage, const Reference& trace, const Index& range_from, const Index& range_to, const Reference& errorhnd_);

	Reference m_errorhnd_impl;
	Reference m_searcher_impl;
	Reference m_trace_impl;
};

/// \brief Forward declaration
class VectorStorageTransaction;

/// \brief Object representing a client connection to a vector storage 
/// \remark The only way to construct a vector storage client instance is to call Context::createVectorStorageClient(const std::string&)
class VectorStorageClient
{
public:
	/// \brief Copy constructor
	VectorStorageClient( const VectorStorageClient& o);

	/// \brief Destructor
	~VectorStorageClient(){}

	/// \brief Create a searcher object for scanning the vectors for similarity
	/// \param[in] range_from start range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \param[in] range_to end of range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \return the vector search interface (with ownership)
	VectorStorageSearcher createSearcher( const Index& range_from, const Index& range_to) const;

	/// \brief Create a vector storage transaction instance
	VectorStorageTransaction createTransaction();
	
	/// \brief Get the list of concept class names defined
	/// \return the list
	StringVector conceptClassNames() const;

	/// \brief Get the list of indices of features represented by a learnt concept feature
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \param[in] conceptid index (indices of learnt concepts starting from 1) 
	/// \return the resulting vector indices (index is order of insertion starting from 0)
	IndexVector conceptFeatures( const String& conceptClass, const Index& conceptid) const;

	/// \brief Get the number of concept features learned for a class
	/// \param[in] conceptClass name identifying a class of concepts learnt.
	/// \return the number of concept features and also the maximum number assigned to a feature (starting with 1)
	unsigned int nofConcepts( const String& conceptClass) const;

	/// \brief Get the set of learnt concepts of a class for a feature defined
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \param[in] index index of vector in the order of insertion starting from 0
	/// \return the resulting concept feature indices (indices of learnt concepts starting from 1)
	IndexVector featureConcepts( const String& conceptClass, const Index& index) const;

	/// \brief Get the vector assigned to a feature addressed by index
	/// \param[in] index index of the feature (starting from 0)
	/// return the vector
	FloatVector featureVector( const Index& index) const;

	/// \brief Get the name of a feature by its index starting from 0
	/// \param[in] index index of the feature (starting from 0)
	/// \return the name of the feature defined 
	String featureName( const Index& index) const;

	/// \brief Get the index starting from 0 of a feature by its name
	/// \param[in] name name of the feature
	/// \return index -1, if not found, else index of the feature to get the name of (index is order of insertion starting with 0)
	Index featureIndex( const String& name) const;

	/// \brief Get the number of feature vectors defined
	/// \return the number of features
	unsigned int nofFeatures() const;

	/// \brief Get the configuration of this model
	/// \return the configuration string
	String config() const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class Context;
	VectorStorageClient( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd_, const String& config);

	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_vector_storage_impl;
	std::string m_config;
};

class VectorStorageTransaction
{
public:
	/// \brief Copy constructor
	VectorStorageTransaction( const VectorStorageTransaction& o);

	~VectorStorageTransaction(){}

	void addFeature( const String& name, const FloatVector& vec);
	void defineFeatureConceptRelation( const String& relationTypeName, const Index& featidx, const Index& conidx);

	bool commit();

	void rollback();

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClient;
	VectorStorageTransaction( const Reference& objbuilder, const Reference& storage, const Reference& trace, const Reference& errorhnd_, const String& config);

	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_vector_storage_impl;	
	Reference m_vector_transaction_impl;
};


/// \brief Configuration describing a scalar function variable value
class FunctionVariableConfig
{
public:
	/// \brief Default constructor
	FunctionVariableConfig(){}
	/// \brief Copy constructor
	FunctionVariableConfig( const FunctionVariableConfig& o)
		:m_variables(o.m_variables){}

	/// \brief Define a variable value
	/// \param[in] name name of the variable
	/// \param[in] value value of the variable
	void defineVariable( const String& name, double value)
	{
		m_variables[ name] = value;
	}

private:
	friend class Query;
	friend class QueryEval;
	std::map<std::string,double> m_variables;
};

/// \brief Configuration describing the values passed to a summarizer function
class SummarizerConfig
{
public:
	/// \brief Constructor
	SummarizerConfig(){}
	/// \brief Copy constructor
	SummarizerConfig( const SummarizerConfig& o)
		:m_parameters(o.m_parameters),m_features(o.m_features){}

#ifndef DOXYGEN_JAVA
	/// \brief Define a summarizer parameter
	/// \param[in] name name of the parameter as defined in the summarizer implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, const Variant& value)
	{
		m_parameters[ name] = value;
	}
#endif
	/// \brief Define a summarizer parameter
	/// \param[in] name name of the parameter as defined in the summarizer implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, const char* value)
	{
		m_parameters[ name] = Variant(value);
	}

	/// \brief Define a summarizer parameter
	/// \param[in] name name of the parameter as defined in the summarizer implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, int value)
	{
		m_parameters[ name] = value;
	}

	/// \brief Define a summarizer parameter
	/// \param[in] name name of the parameter as defined in the summarizer implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, unsigned int value)
	{
		m_parameters[ name] = value;
	}

	/// \brief Define a summarizer parameter
	/// \param[in] name name of the parameter as defined in the summarizer implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, double value)
	{
		m_parameters[ name] = value;
	}

	/// \brief Define a summarizer feature
	/// \param[in] sumtype type name of the feature as defined in the summarizer implementation
	/// \param[in] set feature set of the feature used to address the features
	void defineFeature( const String& sumtype, const String& set)
	{
		m_features[ sumtype] = set;
	}

	/// \brief Rename a result (each summarizer result has a default name that can be changed with this method)
	/// \param[in] origname original name of the result
	/// \param[in] newname defined name of the result
	void defineResultName( const String& origname, const String& newname)
	{
		m_resultnamemap[ origname] = newname;
	}

private:
	friend class QueryEval;
	std::map<std::string,Variant> m_parameters;
	std::map<std::string,std::string> m_features;
	std::map<std::string,std::string> m_resultnamemap;
};

/// \brief Configuration describing the values passed to a weighting function
class WeightingConfig
{
public:
	/// \brief Default constructor
	WeightingConfig(){}
	/// \brief Copy constructor
	WeightingConfig( const WeightingConfig& o)
		:m_parameters(o.m_parameters){}

#ifndef DOXYGEN_JAVA
	/// \brief Define a parameter used for weighting
	/// \param[in] name name of the parameter as defined in the weighting function implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, const Variant& value)
	{
		m_parameters[ name] = value;
	}
#endif
	/// \brief Define a parameter used for weighting
	/// \param[in] name name of the parameter as defined in the weighting function implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, int value)
	{
		m_parameters[ name] = value;
	}

	/// \brief Define a parameter used for weighting
	/// \param[in] name name of the parameter as defined in the weighting function implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, unsigned int value)
	{
		m_parameters[ name] = value;
	}

	/// \brief Define a parameter used for weighting
	/// \param[in] name name of the parameter as defined in the weighting function implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, double value)
	{
		m_parameters[ name] = value;
	}

	/// \brief Define a parameter used for weighting
	/// \param[in] name name of the parameter as defined in the weighting function implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, const char* value)
	{
		m_parameters[ name] = Variant(value);
	}
#if !defined SWIGJAVA && !defined SWIGPHP
	/// \brief Define a parameter used for weighting
	/// \param[in] name name of the parameter as defined in the weighting function implementation
	/// \param[in] value value of the parameter
	void defineParameter( const String& name, const String& value)
	{
		m_parameters[ name] = Variant(value);
	}
#endif

	/// \brief Define a weighting feature
	/// \param[in] sumtype type name of the feature as defined in the weighting function implementation
	/// \param[in] set feature set of the feature used to address the features
	void defineFeature( const String& sumtype, const String& set)
	{
		m_features[ sumtype] = set;
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

	/// \brief Declare a term that is used in the query evaluation as structural element without beeing part of the query (for example punctuation used for match fields summarization)
	/// \param[in] set_ identifier of the term set that is used to address the terms
	/// \param[in] type_ feature type of the of the term
	/// \param[in] value_ feature value of the of the term
	void addTerm(
			const String& set_,
			const String& type_,
			const String& value_);

	/// \brief Declare a feature set to be used as selecting feature
	/// \param[in] set_ identifier of the term set addressing the terms to use for selection
	void addSelectionFeature( const String& set_);

	/// \brief Declare a feature set to be used as restriction
	/// \param[in] set_ identifier of the term set addressing the terms to use as restriction
	void addRestrictionFeature( const String& set_);

	/// \brief Declare a feature set to be used as exclusion
	/// \param[in] set_ identifier of the term set addressing the terms to use as exclusion
	void addExclusionFeature( const String& set_);

	/// \brief Declare a summarizer
	/// \param[in] name the name of the summarizer to add
	/// \param[in] config the configuration of the summarizer to add
	void addSummarizer(
			const String& name,
			const SummarizerConfig& config,
			const String& debugAttributeName);

	/// \brief Add a weighting function to use as summand of the total document weight
	/// \param[in] name the name of the weighting function to add
	/// \param[in] config the configuration of the function to add
	void addWeightingFunction(
			const String& name,
			const WeightingConfig& config,
			const String& debugAttributeName);

	/// \brief Add a weighting formula to use for calculating the total weight from the weighting function results
	/// \param[in] defaultParameter default parameter values
	void addWeightingFormula(
			const String& source,
			const FunctionVariableConfig& defaultParameter);

	/// \brief Create a query to instantiate based on this query evaluation scheme
	/// \param[in] storage storage to execute the query on
	Query createQuery( const StorageClient& storage) const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	QueryEval( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd);

	friend class Query;
	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_queryeval_impl;
	const void* m_queryproc;
};


/// \brief Attribute of a query evaluation result element
class SummaryElement
{
public:
	/// \brief Constructor
	SummaryElement()
		:m_weight(0.0),m_index(-1){}
	/// \brief Constructor
	SummaryElement( const String& name_, const String& value_, double weight_, int index_)
		:m_name(name_),m_value(value_),m_weight(weight_),m_index(index_){}
	/// \brief Copy connstructor
	SummaryElement( const SummaryElement& o)
		:m_name(o.m_name),m_value(o.m_value),m_weight(o.m_weight),m_index(o.m_index){}

	/// \brief Get the name of the summary element
	const String& name() const		{return m_name;}
	/// \brief Get the value of the summary element
	const String& value() const		{return m_value;}
	/// \brief Get the weight of the summary element
	double weight() const			{return m_weight;}
	/// \brief Get the index (grouping) of the summary element if defined or -1
	int index() const			{return m_index;}

private:
	friend class Query;
	std::string m_name;
	std::string m_value;
	double m_weight;
	int m_index;
};


/// \brief Weighted document reference with attributes (result of a query evaluation)
class Rank
{
public:
	/// \brief Constructor
	Rank()
		:m_docno(0),m_weight(0.0){}
	/// \brief Constructor
	Rank( Index docno_, double weight_, const SummaryElementVector& summaryElements_)
		:m_docno(docno_),m_weight(weight_),m_summaryElements(summaryElements_){}
	/// \brief Copy constructor
	Rank( const Rank& o)
		:m_docno(o.m_docno),m_weight(o.m_weight),m_summaryElements(o.m_summaryElements){}

	/// \brief Get the internal document nuber used
	Index docno() const					{return m_docno;}
	/// \brief Get the weight of the rank
	double weight() const					{return m_weight;}
	/// \brief Get the summary elements

	/// \brief Get the summary elements
	const SummaryElementVector& summaryElements() const	{return m_summaryElements;}

private:
	friend class Query;
	Index m_docno;
	double m_weight;
	std::vector<SummaryElement> m_summaryElements;
};


/// \brief Object representing a sequence of query operations to get the set of postings (d,p) of an atomic term or an expression
class QueryExpression
{
public:
	QueryExpression( const QueryExpression& o)
		:m_ops(o.m_ops),m_strings(o.m_strings),m_size(o.m_size){}
	QueryExpression()
		:m_size(0){}

	/// \brief Add operation "Push a single term on the stack"
	/// \param[in] type_ query term type name
	/// \param[in] value_ query term value
	/// \param[in] length_ length of the feature (ordinal position count)
	void pushTerm( const String& type_, const String& value_, const Index& length_);

	/// \brief Add operation "Push a single document field specification on the stack"
	/// \param[in] metadata_start_ name of meta data element that defines the start of the document field, assumed as 1 if name is empty
	/// \param[in] metadata_end_ name of meta data element that defines the end of the document field, assumed as max value if name is empty
	void pushDocField( const String& metadata_start_, const String& metadata_end_);

	/// \brief Add operation "Create an expression from the topmost 'argc' elements of the stack, pop them from the stack and push the expression as single unit on the stack"
	/// \param[in] opname_ name of the expression operator
	/// \param[in] argc_ number of operands (topmost elements from stack) of the expression
	/// \param[in] range_ range number for the expression span in the document
	/// \param[in] cardinality_ number that specifies the minimum size of a subset or subset permutation to match
	void pushExpression( const String& opname_, unsigned int argc_, int range_=0, unsigned int cardinality_=0);

	/// \brief Add operation "Attach a variable to the top expression or term on the query stack".
	/// \note The positions of the query matches of the referenced term or expression can be accessed through this variable in summarization.
	/// \param[in] name_ name of the variable attached
	/// \remark The stack is not changed
	void attachVariable( const String& name_);

	/// \brief Appends the operations of 'o' to this
	/// \param[in] o expression to copy
	void add( const QueryExpression& o);

	/// \brief Get the number of items (sub expressions) on the stack as result of this expression
	/// \return the number of items (sub expressions)
	std::size_t size() const
	{
		return m_size;
	}

private:
	friend class Query;

	struct StackOp
	{
		enum Type
		{
			PushTerm,
			PushDocField,
			PushExpression,
			AttachVariable
		};
		enum ArgIndex
		{
			Term_type=0x0,
			Term_value=0x1,
			Term_length=0x2,
			Term_metastart=0x0,
			Term_metaend=0x1,
			Expression_opname=0x0,
			Expression_argc=0x1,
			Expression_range=0x2,
			Expression_cardinality=0x3,
			Variable_name=0x0
		};
		Type type;
		int arg[4];

		StackOp( Type type_, int arg0_, int arg1_=0, int arg2_=0, int arg3_=0)
			:type(type_)
		{
			arg[0] = arg0_;
			arg[1] = arg1_;
			arg[2] = arg2_;
			arg[3] = arg3_;
		}

		StackOp()
			:type(PushTerm)
		{
			std::memset( arg, 0, sizeof(arg));
		}
		StackOp( const StackOp& o)
		{
			std::memcpy( this, &o, sizeof(*this));
		}
	};
	std::size_t allocid( const String& str);

private:
	std::vector<StackOp> m_ops;
	std::string m_strings;
	std::size_t m_size;
};


/// \brief Structure holding the statistics of a term to be used in a query
/// \note If values of this structure are undefined, then the storage values are used
class TermStatistics
{
public:
	/// \brief Constructor
	TermStatistics()
		:m_df(-1){}
	/// \brief Constructor
	TermStatistics( GlobalCounter df_)
		:m_df(df_){}
	/// \brief Copy constructor
	TermStatistics( const TermStatistics& o)
		:m_df(o.m_df){}

	/// \brief Get the global document frequency
	/// \return the global document frequency or -1 for undefined, if undefined then the value cache for the global dfs in the document frequency or what is stored in the local storage)
	GlobalCounter df() const		{return m_df;}
	/// \brief Set the global document frequency for a term in a query evaluation
	/// \param[in] df_ the document frequency value to use
	void set_df( GlobalCounter df_)		{m_df = df_;}

private:
	GlobalCounter m_df;		///< global document frequency (-1 for undefined, if undefined then the value cache for the global dfs in the document frequency or what is stored in the local storage)
};

/// \brief Global document statistics, if passed down with the query
/// \note If values of this structure are undefined, then the storage values are used
struct GlobalStatistics
{
	/// \brief Default constructor
	GlobalStatistics()
		:m_nofDocumentsInserted(-1){}
	/// \brief Constructor
	explicit GlobalStatistics( GlobalCounter nofDocumentsInserted_)
		:m_nofDocumentsInserted(nofDocumentsInserted_){}
	/// \brief Copy constructor
	GlobalStatistics( const GlobalStatistics& o)
		:m_nofDocumentsInserted(o.m_nofDocumentsInserted){}

	/// \brief Get the global total number of documents in the collection
	GlobalCounter nofdocs() const				{return m_nofDocumentsInserted;}
	/// \brief Set the global total number of documents in the collection for a query evaluation
	/// \param[in] nofdocs_ the total number of documents value to use
	void set_nofdocs( GlobalCounter nofdocs_)		{m_nofDocumentsInserted = nofdocs_;}

private:
	GlobalCounter m_nofDocumentsInserted;	///< global number of documents inserted (-1 for undefined, if undefined then the storage value of the global number of documents is used)
};

/// \brief Structure representing the result of a query
class QueryResult
{
public:
	/// \brief Default constructor
	QueryResult()
		:m_evaluationPass(0),m_nofDocumentsRanked(0),m_nofDocumentsVisited(0){}

	/// \brief Get the last query evaluation pass used (level of selection features used)
	unsigned int evaluationPass() const			{return m_evaluationPass;}
	/// \brief Get the total number of matches that were ranked (after applying all query restrictions)
	unsigned int nofDocumentsRanked() const			{return m_nofDocumentsRanked;}
	/// \brief Get the total number of matches that were visited (after applying ACL restrictions, but before applying other restrictions)
	unsigned int nofDocumentsVisited() const		{return m_nofDocumentsVisited;}

	/// \brief Get the list of result elements
	const RankVector& ranks() const				{return m_ranks;}

private:
	friend class Query;
	QueryResult( unsigned int evaluationPass_, unsigned int nofDocumentsRanked_, unsigned int nofDocumentsVisited_)
		:m_evaluationPass(evaluationPass_),m_nofDocumentsRanked(nofDocumentsRanked_),m_nofDocumentsVisited(nofDocumentsVisited_){}

private:
	unsigned int m_evaluationPass;			///< query evaluation passes used (level of selection features used)
	unsigned int m_nofDocumentsRanked;		///< total number of matches for a query with applying restrictions (might be an estimate)
	unsigned int m_nofDocumentsVisited;		///< total number of matches for a query without applying restrictions but ACL restrictions (might be an estimate)
	RankVector m_ranks;				///< list of result documents (part of the total result)
};

/// \brief Query program object representing a retrieval method for documents in a storage.
class Query
{
public:
	/// \brief Copy constructor
	Query( const Query& o);
	/// \brief Destructor
	~Query(){}

	/// \brief Create a feature from the query expression passed
	/// \param[in] set_ name of the feature set, this feature is addressed with
	/// \param[in] expr_ query expression that defines the postings of the feature and the variables attached
	/// \param[in] weight_ individual weight of the feature in the query
	/// \remark the query expression passed as parameter is refused if it does not contain exactly one element
#if defined SWIGPHP
	// ... SWIG PHP implementation cannot handle signatures with typemaps and default parameters (!)
	void defineFeature( const String& set_, const QueryExpression& expr_, double weight_);
#else
	void defineFeature( const String& set_, const QueryExpression& expr_, double weight_=1.0);
#endif
	/// \brief Define a posting iterator describing a document field addressable as feature
	/// \param[in] set_ name of the feature set, this feature is addressed with
	/// \param[in] metadataStart name of meta data element that defines the start of the document field, assumed as 1 if name is empty
	/// \param[in] metadataEnd name of meta data element that defines the end of the document field, assumed as max value if name is empty
	void defineDocFieldFeature( const String& set_, const String& metadataStart, const String& metadataEnd);

#ifndef DOXYGEN_JAVA
	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void addMetaDataRestrictionCondition(
			const char* compareOp, const String& name,
			const Variant& value, bool newGroup);
#endif
	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void addMetaDataRestrictionCondition(
			const char* compareOp, const String& name,
			double value, bool newGroup);

	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void addMetaDataRestrictionCondition(
			const char* compareOp, const String& name,
			unsigned int value, bool newGroup);

	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void addMetaDataRestrictionCondition(
			const char* compareOp, const String& name,
			int value, bool newGroup);

	void addMetaDataRestrictionCondition_uint(
			const char* compareOp, const String& name,
			unsigned int value, bool newGroup)
	{
		addMetaDataRestrictionCondition( compareOp, name, value, newGroup);
	}

	void addMetaDataRestrictionCondition_int(
			const char* compareOp, const String& name,
			int value, bool newGroup)
	{
		addMetaDataRestrictionCondition( compareOp, name, value, newGroup);
	}
#endif
	/// \brief Define term statistics to use for a term for weighting it in this query
	/// \param[in] type_ query term type name
	/// \param[in] value_ query term value
	/// \param[in] stats_ the structure with the statistics to set
	void defineTermStatistics( const String& type_, const String& value_, const TermStatistics& stats_);

	/// \brief Define the global statistics to use for weighting in this query
	/// \param[in] stats_ the structure with the statistics to set
	void defineGlobalStatistics( const GlobalStatistics& stats_);

	/// \brief Define a set of documents the query is evaluated on. By default the query is evaluated on all documents in the storage
	/// \param[in] docnolist_ list of documents to evaluate the query on
	void addDocumentEvaluationSet( const IndexVector& docnolist_);

	/// \brief Set number of ranks to evaluate starting with the first rank (the maximum size of the result rank list)
	/// \param[in] maxNofRanks_ maximum number of results to return by this query
	void setMaxNofRanks( unsigned int maxNofRanks_);

	/// \brief Set the index of the first rank to be returned
	/// \param[in] minRank_ index of the first rank to be returned by this query
	void setMinRank( unsigned int minRank_);

	/// \brief Add a user for this query (as alternative role)
	/// \param[in] username_ an alternative name of a user for the evaluation of this query
	/// \note the user restriction applies if no user role specified in the query is allowed to see the document.
	void addUserName( const String& username_);

	/// \brief Assign values to variables of the weighting formula
	/// \param[in] parameter parameter values
	void setWeightingVariables(
			const FunctionVariableConfig& parameter);
	/// \brief Switch on debug mode that creates debug info of query evaluation methods and summarization as attributes of the query result
	/// \param[in] debug true if switched on, false if switched off (default off)
	/// \note Debug attributes are specified in the declaration of summarizers and weighting functions (3rd parameter of QueryEval::addSummarizer and QueryEval::addWeightingFunction)
	void setDebugMode( bool debug);

	/// \brief Evaluate this query and return the result
	/// \return the result
	QueryResult evaluate() const;

	/// \brief Map the contents of the query to a readable string
	/// \return the string
	String tostring() const;

private:
	friend class QueryEval;
	Query( const Reference& objbuilder_impl_, const Reference& trace_impl_, const Reference& errorhnd_, const Reference& storage_impl_, const Reference& queryeval_impl_, const Reference& query_impl_, const void* queryproc_)
		:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_impl_),m_objbuilder_impl(objbuilder_impl_),m_storage_impl(storage_impl_),m_queryeval_impl(queryeval_impl_),m_query_impl(query_impl_),m_queryproc(queryproc_)
	{}

	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_queryeval_impl;
	Reference m_query_impl;
	const void* m_queryproc;
};

///\brief Implements browsing the documents of a storage without weighting query, just with a restriction on metadata
class DocumentBrowser
{
public:
	/// \brief Copy constructor
	DocumentBrowser( const DocumentBrowser& o);
	/// \brief Destructor
	~DocumentBrowser(){}

#ifndef DOXYGEN_JAVA
	/// \brief Define a meta data restriction condition on the documents visited
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	/// \remark Metadata restrictions can only be defined before the first call of this DocumentBrowser::next()
	void addMetaDataRestrictionCondition(
			const char* compareOp, const String& name,
			const Variant& value, bool newGroup);
#endif
	/// \brief Define a meta data restriction condition on the documents visited
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	/// \remark Metadata restrictions can only be defined before the first call of this DocumentBrowser::next()
	void addMetaDataRestrictionCondition(
			const char* compareOp, const String& name,
			double value, bool newGroup);

	/// \brief Define a meta data restriction condition on the documents visited
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	/// \remark Metadata restrictions can only be defined before the first call of this DocumentBrowser::next()
	void addMetaDataRestrictionCondition(
			const char* compareOp, const String& name,
			unsigned int value, bool newGroup);

	/// \brief Define a meta data restriction condition on the documents visited
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	/// \remark Metadata restrictions can only be defined before the first call of this DocumentBrowser::next()
	void addMetaDataRestrictionCondition(
			const char* compareOp, const String& name,
			int value, bool newGroup);

	///\brief Get the internal document number of the next document bigger or equal the document number passed
	///\param[in] docno_ document number to get the matching least upperbound from
	///\return the internal document number
	Index skipDoc( const Index& docno_);

	///\brief Get an attribute of the current document visited
	///\return the internal document number or 0, if there is no one left
	String attribute( const String& name);

private:
	friend class StorageClient;
	DocumentBrowser(
		const Reference& objbuilder_impl_,
		const Reference& trace_impl_,
		const Reference& storage_impl_,
		const Reference& errorhnd_);

private:
	Reference m_errorhnd_impl;
	Reference m_trace_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_restriction_impl;
	Reference m_postingitr_impl;
	Reference m_attributereader_impl;
	Index m_docno;
};

/// \brief Object holding the global context of the strus information retrieval engine
/// \note There a two modes of this context object operating on a different base.
///	If you create this object without parameter, then the context is local.
///	In a local context you can load modules, define resources, etc. If you create
///	this object with a connection string as parameter, then all object created by
///	this context reside on the server (strusRpcServer) addressed with the connection string.
///	In this case loaded modules and resources are ignored. What modules to use is then
///	specified on server startup.
class Context
{
public:
	/// \brief Constructor for local mode with own module loader
	Context();
	/// \brief Constructor for local mode with own module loader
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	explicit Context( unsigned int maxNofThreads);
	/// \brief Constructor for local mode with own module loader and logging of all method call traces
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	/// \param[in] tracecfg configuration for generating method call traces
	explicit Context( unsigned int maxNofThreads, const String& tracecfg);
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \param[in] connectionstring RPC server connection string
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	explicit Context( const String& connectionstring);
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \param[in] connectionstring RPC server connection string
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	Context( const String& connectionstring, unsigned int maxNofThreads);
	/// \brief Copy constructor
	Context( const Context& o);
	/// \brief Destructor
	~Context(){}

	/// \brief Check if there has an error occurred and throw if yes
	/// \remark Some bindings have coroutines and with a coroutine switch an error message might get lost, because error context is per thread. So if a coroutine switch is done without the last error fetched it might happen that the second coroutine gets the error of the first one. Call this function after calling some method without return value before a state where a context switch is possible.
	void checkErrors() const;

	/// \brief Load a module
	/// \param[in] name_ name of the module to load
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void loadModule( const String& name_);

	/// \brief Add a path from where to try to load modules from
	/// \param[in] paths_ semicolon separated list of module search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addModulePath( const String& paths_);

	/// \brief Define where to load analyzer resource files from
	/// \param[in] paths_ semicolon separated list of resource search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addResourcePath( const String& paths_);

	/// \brief Create a statistics message processor instance
	/// \return the processor
	StatisticsProcessor createStatisticsProcessor( const String& name);

	/// \brief Create a storage client instance of the the default storage
	StorageClient createStorageClient();

	/// \brief Create a storage client instance
	/// \param[in] config_ configuration string of the storage client or empty, if the default remote storage of the RPC server is chosen,
	StorageClient createStorageClient( const String& config_);

	/// \brief Create a storage client instance of the the default remote storage of the RPC server
	VectorStorageClient createVectorStorageClient();

	/// \brief Create a vector storage client instance
	/// \param[in] config_ configuration string of the storage client or empty for the default storage
	VectorStorageClient createVectorStorageClient( const String& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration
 	/// \remark Fails if the storage already exists
	void createStorage( const String& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration
 	/// \remark Fails if the storage already exists
	void createVectorStorage( const String& config_);

	/// \brief Delete the storage (physically) described by config
	/// \note works also on vector storages
	/// \param[in] config_ storage description
	/// \note Handle this function carefully
	void destroyStorage( const String& config_);

	/// \brief Detect the type of document from its content
	/// \param[in] content the document content to classify
	/// \return the document class
	DocumentClass detectDocumentClass( const String& content);

	/// \brief Create a document analyzer instance
	/// \param[in] segmentername_ name of the segmenter to use (if empty then the default segmenter is used)
	DocumentAnalyzer createDocumentAnalyzer( const String& segmentername_="");

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
	Reference m_errorhnd_impl;
	Reference m_moduleloader_impl;
	Reference m_rpc_impl;
	Reference m_trace_impl;
	Reference m_storage_objbuilder_impl;
	Reference m_analyzer_objbuilder_impl;
	const void* m_textproc;
};

#ifdef DOXYGEN_LANG
#if defined DOXYGEN_JAVA
}}}
#elif defined DOXYGEN_PYTHON
}
#endif
#endif
#endif


