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
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

#ifdef DOXYGEN_LANG
#include "swig.hpp"
// ... doxygen needs this for creating alternative interface descriptions for different languages
#if defined DOXYGEN_JAVA
namespace net {
namespace strus {
namespace api {
#endif
#elif (defined STRUS_BOOST_PYTHON)
typedef std::string String;
typedef std::vector<int> IntVector;
typedef std::vector<std::string> StringVector;
#else
#define String std::string
#define IntVector std::vector<int>
#define StringVector std::vector<std::string>
#if !defined DOXYGEN_PHP && !defined DOXYGEN_PYTHON
#define NormalizerVector std::vector<Normalizer>
#endif
#define TermVector std::vector<Term>
#define RankVector std::vector<Rank>
#define RankAttributeVector std::vector<RankAttribute>
#define AttributeVector std::vector<Attribute>
#define MetaDataVector std::vector<MetaData>
#endif
typedef unsigned int Index;
typedef unsigned long GlobalCounter;

#ifndef DOXYGEN_LANG
/// \brief Reference to an object used for making objects independent and save from garbage collecting in an interpreter context
class Reference
{
public:
#ifdef STRUS_BOOST_PYTHON
	Reference()
		:m_deleter(0){}
#endif
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

#ifdef STRUS_BOOST_PYTHON
typedef std::vector<Normalizer> NormalizerVector;
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


/// \brief Enumeration for defining internal variant representation
enum VariantType
{
	Variant_UNDEFINED,
	Variant_UINT,
	Variant_INT,
	Variant_FLOAT,
	Variant_TEXT
};

#if defined DOXYGEN_LANG
/// \brief Object representing a string or a numeric value of the binding language
class Variant
{
};
#else
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

#ifdef STRUS_BOOST_PYTHON
	bool operator==( const Variant& o) const
	{
		return isEqual( o);
	}
	bool operator!=( const Variant& o) const
	{
		return !isEqual( o);
	}
#endif
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
	Term( const String& type_, const String& value_, const Index& position_)
		:m_type(type_),m_value(value_),m_position(position_){}
	/// \brief Copy constructor
	Term( const Term& o)
		:m_type(o.m_type),m_value(o.m_value),m_position(o.m_position){}
	/// \brief Default constructor
	Term()
		:m_position(0){}

	/// \brief Get the term type name
	const String& type() const			{return m_type;}
	/// \brief Get the term value
	const String& value() const			{return m_value;}
	/// \brief Get the term position
	unsigned int position() const			{return m_position;}

#ifdef STRUS_BOOST_PYTHON
	bool operator==( const Term& o) const
	{
		return m_type == o.m_type && m_value == o.m_value && m_position == o.m_position ;
	}
	bool operator!=( const Term& o) const
	{
		return m_type != o.m_type || m_value != o.m_value || m_position != o.m_position;
	}
#endif

private:
	std::string m_type;
	std::string m_value;
	Index m_position;
};
#ifdef STRUS_BOOST_PYTHON
typedef std::vector<Term> TermVector;
#endif


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
	const Variant& value() const			{return m_value;}

#ifdef STRUS_BOOST_PYTHON
	bool operator==( const MetaData& o) const
	{
		return (m_name == o.m_name && m_value == o.m_value);
	}
	bool operator!=( const MetaData& o) const
	{
		return (m_name != o.m_name || m_value != o.m_value);
	}
#endif
private:
	std::string m_name;
	Variant m_value;
};
#ifdef STRUS_BOOST_PYTHON
typedef std::vector<MetaData> MetaDataVector;
#endif


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

#ifdef STRUS_BOOST_PYTHON
	bool operator==( const Attribute& o) const
	{
		return m_name == o.m_name && m_value == o.m_value;
	}
	bool operator!=( const Attribute& o) const
	{
		return m_name != o.m_name || m_value != o.m_value;
	}
#endif
private:
	std::string m_name;
	std::string m_value;
};
#ifdef STRUS_BOOST_PYTHON
typedef std::vector<Attribute> AttributeVector;
#endif


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
#ifdef STRUS_BOOST_PYTHON
	/// \brief Define a meta data value of the document
	/// \param[in] name name of the meta data table element
	/// \param[in] value value of the meta data table element
	void setMetaData_double( const String& name, double value)
	{
		setMetaData_double( name, value);
	}
	/// \brief Define a meta data value of the document
	/// \param[in] name name of the meta data table element
	/// \param[in] value value of the meta data table element
	void setMetaData_int( const String& name, int value)
	{
		setMetaData_double( name, value);
	}
	/// \brief Define a meta data value of the document
	/// \param[in] name name of the meta data table element
	/// \param[in] value value of the meta data table element
	void setMetaData_uint( const String& name, unsigned int value)
	{
		setMetaData_double( name, value);
	}
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
	const String& docid()					{return m_docid;}

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
/// \remark The only way to construct a document analyzer instance is to call Context::createDocumentAnalyzer()
class DocumentAnalyzer
{
public:
#ifdef STRUS_BOOST_PYTHON
	DocumentAnalyzer(){}
#endif
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
		const String& type,
		const String& selectexpr,
		const Tokenizer& tokenizer,
		const NormalizerVector& normalizers,
		const StringVector& options=StringVector());

#ifdef STRUS_BOOST_PYTHON
	void addSearchIndexFeature_4(
		const String& type,
		const String& selectexpr,
		const FunctionObject& tokenizer_,
		const FunctionObject& normalizers_);

	void addSearchIndexFeature_5(
		const String& type,
		const String& selectexpr,
		const FunctionObject& tokenizer_,
		const FunctionObject& normalizers_,
		const StringVector& options);
#endif

	/// \brief Define how a feature to insert into the forward index (for summarization) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	/// \param[in] options a list of options as strings, one of {"BindPosPred" => the position is bound to the preceeding feature, "BindPosSucc" => the position is bound to the succeeding feature}
	void addForwardIndexFeature(
		const String& type,
		const String& selectexpr,
		const Tokenizer& tokenizer,
		const NormalizerVector& normalizers,
		const StringVector& options=StringVector());

#ifdef STRUS_BOOST_PYTHON
	void addForwardIndexFeature_4(
		const String& type,
		const String& selectexpr,
		const FunctionObject& tokenizer_,
		const FunctionObject& normalizers_);

	void addForwardIndexFeature_5(
		const String& type,
		const String& selectexpr,
		const FunctionObject& tokenizer_,
		const FunctionObject& normalizers_,
		const StringVector& options);
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

#ifdef STRUS_BOOST_PYTHON
	void defineMetaData_obj(
		const String& fieldname,
		const String& selectexpr,
		const FunctionObject& tokenizer_,
		const FunctionObject& normalizers_);
#endif

	/// \brief Declare some aggregated value of the document to be put into the meta data table used for restrictions, weighting and summarization.
 	/// \param[in] fieldname name of the addressed meta data field.
	/// \param[in] function defining how and from what the value is aggregated
	void defineAggregatedMetaData(
		const String& fieldname,
		const Aggregator& function);

#ifdef STRUS_BOOST_PYTHON
	void defineAggregatedMetaData_obj(
		const String& fieldname,
		const FunctionObject& function_);
#endif

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

#ifdef STRUS_BOOST_PYTHON
	void defineAttribute_obj(
		const String& fieldname,
		const String& selectexpr,
		const FunctionObject& tokenizer_,
		const FunctionObject& normalizers_);
#endif

	/// \brief Analye the content and return the set of features to insert
	/// \param[in] content string (NOT a file name !) of the document to analyze
	Document analyze( const String& content);

	/// \brief Analye the content and return the set of features to insert
	/// \param[in] content string (NOT a file name !) of the document to analyze
	/// \param[in] dclass document class of the document to analyze
	Document analyze( const String& content, const DocumentClass& dclass);

#ifdef STRUS_BOOST_PYTHON
	Document analyze_1( const String& content)
	{
		return analyze( content);
	}

	Document analyze_2( const String& content, const DocumentClass& dclass)
	{
		return analyze( content, dclass);
	}
#endif
private:
	/// \brief Constructor used by Context
	friend class Context;
	DocumentAnalyzer( const Reference& objbuilder, const Reference& errorhnd, const String& segmentername);

	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
};


/// \brief Forward declaration
class QueryAnalyzeQueue;

/// \class QueryAnalyzer
/// \brief Analyzer object representing a set of function for transforming a phrase,
///	the smallest unit in any query language, to a set of terms that can be used
///	to build a query.
/// \remark The only way to construct a query analyzer instance is to call Context::createQueryAnalyzer()
class QueryAnalyzer
{
public:
#ifdef STRUS_BOOST_PYTHON
	QueryAnalyzer(){}
#endif
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
			const String& phraseType,
			const String& featureType,
			const Tokenizer& tokenizer,
			const NormalizerVector& normalizers);

#ifdef STRUS_BOOST_PYTHON
	void definePhraseType_obj(
		const String& phrasetype,
		const String& selectexpr,
		const FunctionObject& tokenizer_,
		const FunctionObject& normalizers_);
#endif

	/// \brief Tokenizes and normalizes a phrase and creates some typed terms out of it according the definition of the phrase type given.
	/// \param[in] phraseType name of the phrase type to use for analysis
	/// \param[in] phraseContent content string of the query phrase to analyze
	/// \deprecated
	TermVector analyzePhrase(
			const String& phraseType,
			const String& phraseContent) const;

	/// \brief Creates a queue for phrase bulk analysis
	/// \return the queue
	QueryAnalyzeQueue createQueue() const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	QueryAnalyzer( const Reference& objbuilder, const Reference& errorhnd);

	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
};


/// \class QueryAnalyzeQueue
/// \brief Analyzer object implementing a queue of analyze phrase tasks.
/// \remark Query analysis with this class reduces network roundtrips when using it as proxy
class QueryAnalyzeQueue
{
public:
#ifdef STRUS_BOOST_PYTHON
	QueryAnalyzeQueue(){}
#endif
	/// \brief Copy constructor
	QueryAnalyzeQueue( const QueryAnalyzeQueue& o);
	/// \brief Destructor
	~QueryAnalyzeQueue(){}

	/// \brief Push a phrase into the queue for phrases to tokenize and normalize
	/// \param[in] phraseType name of the phrase type to use for analysis
	/// \param[in] phraseContent content string of the query phrase to analyze
	void push(
			const String& phraseType,
			const String& phraseContent);

	/// \brief Processes the next phrase of the queue for phrases to analyzer. Does the tokenization and normalization and creates some typed terms out of it according the definition of the phrase type given.
	/// \return list of terms (query phrase analyzer result)
	TermVector fetch();

private:
	/// \brief Constructor used by Context
	friend class QueryAnalyzer;
	explicit QueryAnalyzeQueue( const Reference& objbuilder, const Reference& errorhnd, const Reference& analyzer);

	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
	std::vector<Term> m_phrase_queue;
	std::vector<std::vector<Term> > m_result_queue;
	std::size_t m_result_queue_idx;
};


/// \brief Object representing a client connection to the storage 
/// \remark The only way to construct a storage client instance is to call Context::createStorageClient(const std::string&)
class StorageClient
{
public:
#ifdef STRUS_BOOST_PYTHON
	StorageClient(){}
#endif
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
	void insertDocument( const String& docid, const Document& doc);

	/// \brief Prepare the deletion of a document from the storage
	/// \param[in] docid the identifier of the document to delete
	/// \remark The document is physically deleted with the next implicit or explicit call of 'flush()'
	void deleteDocument( const String& docid);

	/// \brief Prepare the deletion of all document access rights of a user
	/// \param[in] username the name of the user to delete all access rights (in the local collection)
	/// \remark The user access rights are changed accordingly with the next implicit or explicit call of 'flush'
	void deleteUserAccessRights( const String& username);

	/// \brief Commit all insert or delete or user access right change statements open.
	void flush();

	/// \brief Close of the storage client
	void close();

private:
	friend class Context;
	StorageClient( const Reference& objbuilder, const Reference& errorhnd_, const String& config);

	friend class Query;
	friend class QueryEval;
	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_transaction_impl;
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

#ifdef STRUS_BOOST_PYTHON
	void defineParameter_string( const String& name, const char* value)
	{
		defineParameter( name, value);
	}
	void defineParameter_charp( const String& name, const char* value)
	{
		defineParameter( name, value);
	}
	void defineParameter_int( const String& name, int value)
	{
		defineParameter( name, value);
	}
	void defineParameter_uint( const String& name, unsigned int value)
	{
		defineParameter( name, value);
	}
	void defineParameter_double( const String& name, double value)
	{
		defineParameter( name, value);
	}
#endif

	/// \brief Define a summarizer feature
	/// \param[in] sumtype type name of the feature as defined in the summarizer implementation
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
#ifdef STRUS_BOOST_PYTHON
	QueryEval(){}
#endif
	/// \brief Copy constructor
	QueryEval( const QueryEval& o);
	/// \brief Destructor
	~QueryEval(){}

	/// \brief Declare a term that is used in the query evaluation as structural element without beeing part of the query (for example punctuation used for match phrases summarization)
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
	/// \param[in] resultAttribute name of the result attribute this summarization result is assigned to
	/// \param[in] name the name of the summarizer to add
	/// \param[in] config the configuration of the summarizer to add
	void addSummarizer(
			const String& resultAttribute,
			const String& name,
			const SummarizerConfig& config);

#ifdef STRUS_BOOST_PYTHON
	void addSummarizer_obj(
		const String& resultAttribute,
		const String& name,
		const FunctionObject& config_);
#endif

	/// \brief Add a weighting function to use as summand of the total document weight
	/// \param[in] weight additive weight of the feature (compared with other weighting functions added)
	/// \param[in] name the name of the weighting function to add
	/// \param[in] config the configuration of the function to add
	void addWeightingFunction(
			double weight,
			const String& name,
			const WeightingConfig& config);

#ifdef STRUS_BOOST_PYTHON
	void addWeightingFunction_obj(
		double weight,
		const String& name,
		const FunctionObject& config_);
#endif

	/// \brief Create a query to instantiate based on this query evaluation scheme
	/// \param[in] storage storage to execute the query on
	Query createQuery( const StorageClient& storage) const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	QueryEval( const Reference& objbuilder, const Reference& errorhnd);

	friend class Query;
	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_queryeval_impl;
};


/// \brief Attribute of a query evaluation result element
class RankAttribute
{
public:
	/// \brief Constructor
	RankAttribute()
		:m_weight(0.0){}
	/// \brief Constructor
	RankAttribute( const String& name_, const String& value_, double weight_)
		:m_name(name_),m_value(value_),m_weight(weight_){}
	/// \brief Copy connstructor
	RankAttribute( const RankAttribute& o)
		:m_name(o.m_name),m_value(o.m_value),m_weight(o.m_weight){}

	/// \brief Get the name of this attribute
	const String& name() const		{return m_name;}
	/// \brief Get the value of this attribute
	const String& value() const		{return m_value;}
	/// \brief Get the weight of the attribute
	double weight() const			{return m_weight;}

#ifdef STRUS_BOOST_PYTHON
	bool operator==( const RankAttribute& o) const
	{
		if (m_name != o.m_name || m_value != o.m_value) return false;
		double ww = m_weight - o.m_weight;
		return (ww < 0.0)?(-ww<std::numeric_limits<double>::epsilon()):(ww<std::numeric_limits<double>::epsilon());
	}
	bool operator!=( const RankAttribute& o) const
	{
		return !operator==(o);
	}
#endif
private:
	friend class Query;
	std::string m_name;
	std::string m_value;
	double m_weight;
};
#ifdef STRUS_BOOST_PYTHON
typedef std::vector<RankAttribute> RankAttributeVector;
#endif


/// \brief Weighted document reference with attributes (result of a query evaluation)
class Rank
{
public:
	/// \brief Constructor
	Rank()
		:m_docno(0),m_weight(0.0){}
	/// \brief Constructor
	Rank( Index docno_, double weight_, const RankAttributeVector& attributes_)
		:m_docno(docno_),m_weight(weight_),m_attributes(attributes_){}
	/// \brief Copy constructor
	Rank( const Rank& o)
		:m_docno(o.m_docno),m_weight(o.m_weight),m_attributes(o.m_attributes){}

	/// \brief Get the internal document nuber used
	Index docno() const					{return m_docno;}
	/// \brief Get the weight of the rank
	double weight() const					{return m_weight;}
	/// \brief Get the attributes
	const RankAttributeVector& attributes() const		{return m_attributes;}

#ifdef STRUS_BOOST_PYTHON
	bool operator==( const Rank& o) const
	{
		if (m_docno != o.m_docno) return false;
		double ww = (m_weight - o.m_weight);
		return (ww < 0.0)?(-ww < std::numeric_limits<double>::epsilon()):(ww < std::numeric_limits<double>::epsilon());
	}
	bool operator!=( const Rank& o) const
	{
		return !operator==( o);
	}
#endif

private:
	friend class Query;
	Index m_docno;
	double m_weight;
	std::vector<RankAttribute> m_attributes;
};
#ifdef STRUS_BOOST_PYTHON
typedef std::vector<Rank> RankVector;
#endif


/// \brief Query program object representing a retrieval method for documents in a storage.
class Query
{
public:
#ifdef STRUS_BOOST_PYTHON
	Query(){}
#endif
	/// \brief Copy constructor
	Query( const Query& o);
	/// \brief Destructor
	~Query(){}

	/// \brief Push a single term on the stack
	/// \param[in] type_ query term type name
	/// \param[in] value_ query term value
	void pushTerm( const String& type_, const String& value_);

	/// \brief Create an expression from the topmost 'argc' elements of the stack, pop them from the stack and push the expression as single unit on the stack
	/// \param[in] opname_ name of the expression operator
	/// \param[in] argc number of operands (topmost elements from stack) of the expression
	/// \param[in] range_ range number for the expression span in the document
	/// \param[in] cardinality_ number that specifies the minimum size of a subset or subset permutation to match
	void pushExpression( const String& opname_, unsigned int argc, int range_=0, unsigned int cardinality_=0);

#ifdef STRUS_BOOST_PYTHON
	void pushExpression2( const String& opname_, unsigned int argc)
	{
		pushExpression( opname_, argc, 0, 0);
	}
	void pushExpression3( const String& opname_, unsigned int argc, int range_)
	{
		pushExpression( opname_, argc, range_, 0);
	}
	void pushExpression4( const String& opname_, unsigned int argc, int range_, unsigned int cardinality_)
	{
		pushExpression( opname_, argc, range_, cardinality_);
	}
#endif

	/// \brief Push a duplicate of the topmost element of the query stack
	/// \note This function makes it possible to reference terms or expressions more than once as features or as subexpressions.
	void pushDuplicate();

	/// \brief Attaches a variable to the top expression or term on the query stack.
	/// \note The positions of the query matches of the referenced term or expression can be accessed through this variable in summarization.
	/// \param[in] name_ name of the variable attached
	/// \remark The stack is not changed
	void attachVariable( const String& name_);

	/// \brief Create a feature from the top element on the stack (and pop the element from the stack)
	/// \param[in] set_ name of the feature set, this feature is addressed with
	/// \param[in] weight_ individual weight of the feature in the query
	void defineFeature( const String& set_, double weight_=1.0);

#ifdef STRUS_BOOST_PYTHON
	void defineFeature1( const String& set_)
	{
		defineFeature( set_, 1.0);
	}

	void defineFeature2( const String& set_, double weight_)
	{
		defineFeature( set_, weight_);
	}
#endif

#ifndef DOXYGEN_JAVA
	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void defineMetaDataRestriction(
			const char* compareOp, const String& name,
			const Variant& value, bool newGroup=true);
#endif
	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void defineMetaDataRestriction(
			const char* compareOp, const String& name,
			double value, bool newGroup=true);

	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void defineMetaDataRestriction(
			const char* compareOp, const String& name,
			unsigned int value, bool newGroup=true);

	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void defineMetaDataRestriction(
			const char* compareOp, const String& name,
			int value, bool newGroup=true);

#ifdef STRUS_BOOST_PYTHON
	void defineMetaDataRestriction_double_3(
			const char* compareOp, const String& name,
			double value)
	{
		defineMetaDataRestriction( compareOp, name, value, true);
	}

	void defineMetaDataRestriction_double_4(
			const char* compareOp, const String& name,
			double value, bool newGroup)
	{
		defineMetaDataRestriction( compareOp, name, value, newGroup);
	}

	void defineMetaDataRestriction_uint_3(
			const char* compareOp, const String& name,
			unsigned int value)
	{
		defineMetaDataRestriction( compareOp, name, value, true);
	}

	void defineMetaDataRestriction_uint_4(
			const char* compareOp, const String& name,
			unsigned int value, bool newGroup)
	{
		defineMetaDataRestriction( compareOp, name, value, newGroup);
	}

	void defineMetaDataRestriction_int_3(
			const char* compareOp, const String& name,
			int value)
	{
		defineMetaDataRestriction( compareOp, name, value, true);
	}

	void defineMetaDataRestriction_int_4(
			const char* compareOp, const String& name,
			int value, bool newGroup)
	{
		defineMetaDataRestriction( compareOp, name, value, newGroup);
	}
#endif

	/// \brief Define a set of documents the query is evaluated on. By default the query is evaluated on all documents in the storage
	/// \param[in] docnolist_ list of documents to evaluate the query on
	void addDocumentEvaluationSet(
			const IntVector& docnolist_);

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

	/// \brief Evaluate this query and return the result
	/// \return the result
	RankVector evaluate() const;

private:
	friend class QueryEval;
	Query( const Reference& objbuilder_impl_, const Reference& errorhnd_, const Reference& storage_impl_, const Reference& queryeval_impl_, const Reference& query_impl_)
		:m_errorhnd_impl(errorhnd_),m_objbuilder_impl(objbuilder_impl_),m_storage_impl(storage_impl_),m_queryeval_impl(queryeval_impl_),m_query_impl(query_impl_){}

	Reference m_errorhnd_impl;
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
class Context
{
public:
	/// \brief Constructor for local mode with own module loader
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	explicit Context( unsigned int maxNofThreads=0);
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	Context( const char* connectionstring, unsigned int maxNofThreads=0);
	/// \brief Copy constructor
	Context( const Context& o);
	/// \brief Destructor
	~Context(){}

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

	/// \brief Create a storage client instance
	/// \param[in] config_ configuration string of the storage client or empty, if the default remote storage of the RPC server is chosen,
	StorageClient createStorageClient( const String& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration
 	/// \remark Fails if the storage already exists
	void createStorage( const String& config_);

	/// \brief Delete the storage (physically) described by config
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

#ifdef STRUS_BOOST_PYTHON
	DocumentAnalyzer createDocumentAnalyzer_0()
	{
		return createDocumentAnalyzer();
	}

	DocumentAnalyzer createDocumentAnalyzer_1( const String& segmentername_)
	{
		return createDocumentAnalyzer( segmentername_);
	}
#endif

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
	Reference m_storage_objbuilder_impl;
	Reference m_analyzer_objbuilder_impl;
};

#ifdef DOXYGEN_LANG
#if defined DOXYGEN_JAVA
}}}
#endif
#endif
#endif


