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
#elif (defined STRUS_BOOST_PYTHON)
typedef std::string String;
typedef std::wstring WString;
typedef std::vector<int> IntVector;
typedef std::vector<std::string> StringVector;
#else
#define String std::string
#define WString std::wstring
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
#define DocumentFrequencyChangeVector std::vector<DocumentFrequencyChange> 
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
#if defined STRUS_BOOST_PYTHON || defined DOXYGEN_PYTHON
	WString ucvalue() const;
#endif
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
	double value() const				{return m_value.getFloat();}

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

#if defined STRUS_BOOST_PYTHON || defined DOXYGEN_PYTHON
	WString ucvalue() const;
#endif
	
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
	void setMetaData_double( const String& name, double value)
	{
		setMetaData( name, value);
	}
	void setMetaData_int( const String& name, int value)
	{
		setMetaData( name, value);
	}
	void setMetaData_uint( const String& name, unsigned int value)
	{
		setMetaData( name, value);
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
#ifdef STRUS_BOOST_PYTHON
	void setAttribute_unicode( const String& name, const WString& value);
#endif
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

/// \class DocumentAnalyzer
/// \brief Analyzer object representing a program for segmenting, 
///	tokenizing and normalizing a document into atomic parts, that 
///	can be inserted into a storage and be retrieved from there.
/// \remark The only way to construct a document analyzer instance is to call Context::createDocumentAnalyzer()
class DocumentAnalyzer
{
public:
#ifdef STRUS_BOOST_PYTHON
	DocumentAnalyzer()
		:m_textproc(0){}
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
		const String& options);
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
		const String& options);
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

#ifdef STRUS_BOOST_PYTHON
	Document analyze_unicode_1( const WString& content);
	Document analyze_unicode_2( const WString& content, const DocumentClass& dclass);
	Document analyze_1( const String& content);
	Document analyze_2( const String& content, const DocumentClass& dclass);
#endif

	/// \brief Creates a queue for multi document analysis
	/// \return the queue
	DocumentAnalyzeQueue createQueue() const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	DocumentAnalyzer( const Reference& objbuilder, const Reference& errorhnd, const String& segmentername, const void* textproc_);

	Reference m_errorhnd_impl;
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
#ifdef STRUS_BOOST_PYTHON
	DocumentAnalyzeQueue()
		:m_result_queue_idx(0),m_analyzerctx_queue_idx(0),m_textproc(0){}
#endif
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

#ifdef STRUS_BOOST_PYTHON
	void push_unicode_1( const WString& content);
	void push_unicode_2( const WString& content, const DocumentClass& dclass);
#endif
	/// \brief Checks if there are more results to fetch
	/// \return true, if yes
	bool hasMore() const;

	/// \brief Processes the next phrase of the queue for phrases to analyzer. Does the tokenization and normalization and creates some typed terms out of it according the definition of the phrase type given.
	/// \return list of terms (query phrase analyzer result)
	Document fetch();

private:
	void analyzeNext();

private:
	/// \brief Constructor used by Context
	friend class DocumentAnalyzer;
	explicit DocumentAnalyzeQueue( const Reference& objbuilder, const Reference& errorhnd, const Reference& analyzer, const void* textproc_);

	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_analyzer_impl;
	std::vector<Document> m_result_queue;
	std::size_t m_result_queue_idx;
	std::vector<Reference> m_analyzerctx_queue;
	std::size_t m_analyzerctx_queue_idx;
	const void* m_textproc;
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

#ifdef STRUS_BOOST_PYTHON
	TermVector analyzePhrase_unicode(
			const String& phraseType,
			const WString& phraseContent) const;
#endif

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
	QueryAnalyzeQueue()
		:m_result_queue_idx(0){}
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
#ifdef STRUS_BOOST_PYTHON
	void push_unicode( const String& phraseType, const WString& phraseContent);
#endif

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

/// \brief Forward declaration
class PeerMessageQueue;
/// \brief Forward declaration
class StorageTransaction;

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

	/// \brief Create a transaction
	/// return the transaction object created
	StorageTransaction createTransaction() const;

	/// \brief Create a handler for processing distributed storage statistics
	/// return the peer message queue object created
	PeerMessageQueue createPeerMessageQueue() const;

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
};


/// \brief Object representing a transaction of the storage 
/// \remark The only way to construct a storage transaction instance is to call StorageClient::createTransaction()
class StorageTransaction
{
public:
#ifdef STRUS_BOOST_PYTHON
	StorageTransaction(){}
#endif
	/// \brief Destructor
	~StorageTransaction(){}

	/// \brief Preallocate a range of document numbers for new documents to insert
	void allocateDocnoRange( unsigned int nofDocuments);

	/// \brief Prepare the inserting a document into the storage
	/// \param[in] docid the identifier of the document to insert
	/// \param[in] doc the structure of the document to insert
	/// \remark The document is physically inserted with the call of 'commit()'
	void insertDocument( const String& docid, const Document& doc, bool isnew);

	/// \brief Prepare the deletion of a document from the storage
	/// \param[in] docid the identifier of the document to delete
	/// \remark The document is physically deleted with the call of 'commit()'
	void deleteDocument( const String& docid);
#ifdef STRUS_BOOST_PYTHON
	void deleteDocument_unicode( const WString& docid);
#endif

	/// \brief Prepare the deletion of all document access rights of a user
	/// \param[in] username the name of the user to delete all access rights (in the local collection)
	/// \remark The user access rights are changed accordingly with the next implicit or explicit call of 'flush'
	void deleteUserAccessRights( const String& username);
#ifdef STRUS_BOOST_PYTHON
	void deleteUserAccessRights_unicode( const WString& username);
#endif

	/// \brief Commit all insert or delete or user access right change statements of this transaction.
	void commit();

	/// \brief Rollback all insert or delete or user access right change statements of this transaction.
	void rollback();

private:
	friend class StorageClient;
	StorageTransaction( const Reference& objbuilder, const Reference& errorhnd_, const Reference& storage_);

	friend class Query;
	friend class QueryEval;
	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_transaction_impl;
	Reference m_docnoalloc_impl;

	struct DocnoRange
	{
		DocnoRange( const DocnoRange& o)
			:first(o.first),size(o.size){}
		DocnoRange( unsigned int first_, unsigned int size_)
			:first(first_),size(size_){}

		unsigned int first;
		unsigned int size;
	};
	std::vector<DocnoRange> m_docnorangear;
};


/// \brief Structure describing the document frequency change of one term in the collection
class DocumentFrequencyChange
{
public:
	/// \brief Constructor
	DocumentFrequencyChange( const String& type_, const String& value_, int increment_, bool isnew_)
		:m_type(type_),m_value(value_),m_increment(increment_),m_isnew(isnew_){}
	/// \brief Copy constructor
	DocumentFrequencyChange( const DocumentFrequencyChange& o)
		:m_type(o.m_type),m_value(o.m_value),m_increment(o.m_increment),m_isnew(o.m_isnew){}
	/// \brief Default constructor
	DocumentFrequencyChange()
		:m_increment(0),m_isnew(false){}

	/// \brief Get the term type name
	const String& type() const			{return m_type;}
	/// \brief Get the term value
	const String& value() const			{return m_value;}
#if defined STRUS_BOOST_PYTHON || defined DOXYGEN_PYTHON
	WString ucvalue() const;
#endif
	/// \brief Get the term increment
	unsigned int increment() const			{return m_increment;}

	/// \brief Eval if the document frequency change if for an unknown term
	unsigned int isnew() const			{return m_isnew;}

#ifdef STRUS_BOOST_PYTHON
	bool operator==( const DocumentFrequencyChange& o) const
	{
		return m_type == o.m_type && m_value == o.m_value && m_increment == o.m_increment && m_isnew == o.m_isnew;
	}
	bool operator!=( const DocumentFrequencyChange& o) const
	{
		return m_type != o.m_type || m_value != o.m_value || m_increment != o.m_increment || m_isnew != o.m_isnew;
	}
#endif

private:
	std::string m_type;
	std::string m_value;
	int m_increment;
	bool m_isnew;
};
#ifdef STRUS_BOOST_PYTHON
typedef std::vector<DocumentFrequencyChange> DocumentFrequencyChangeVector;
#endif


/// \brief Pair of queues for messages to and from peer storages for distributing statistics
class PeerMessage
{
public:
#ifdef STRUS_BOOST_PYTHON
	PeerMessage(){}
#endif
	/// \brief Copy constructor
	PeerMessage( const PeerMessage& o)
		:m_nofDocumentsInsertedChange(o.m_nofDocumentsInsertedChange)
		,m_documentFrequencyChangeList(o.m_documentFrequencyChangeList){}

	/// \brief Constructor from elements
	PeerMessage( const std::vector<DocumentFrequencyChange>& dfchglist, int nofdocs)
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


/// \brief Pair of queues for messages to and from peer storages for distributing statistics
class PeerMessageQueue
{
public:
#ifdef STRUS_BOOST_PYTHON
	PeerMessageQueue(){}
#endif
	/// \brief Copy constructor
	PeerMessageQueue( const PeerMessageQueue& o);

	/// \brief Notify initialization/deinitialization, fetching local statistics to populate to other peers
	/// \param[in] sign of the statistics to populate, true = positive (on initialization), false = negative (on deinitialization)
	void start( bool sign);

	/// \brief Push a message from another peer storage
	/// \param[in] msg message from peer
	/// \return message to reply to sender or empty blob if there is nothing to reply
	String push( const String& msg);

	/// \brief Fetches the next message to distribute to all other peers
	/// \return message blob or empty string if there is no message left
	String fetch();

	/// \brief Decode a peer message blob for introspection
	/// \param[in] blob peer message blob
	/// \return the peer message
	PeerMessage decode( const String& blob) const;

	/// \brief Create binary blob to push from peer message structure
	/// \param[in] msg peer message structure
	/// \return the peer message blob
	String encode( const PeerMessage& msg) const;

private:
	friend class StorageClient;
	PeerMessageQueue( const Reference& objbuilder, const Reference& errorhnd_, const Reference& storage_);

private:
	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_msgqueue_impl;
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
	const void* m_queryproc;
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

#if defined STRUS_BOOST_PYTHON || defined DOXYGEN_PYTHON
	WString ucvalue() const;

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
	void pushTerm( const String& type_, const String& value_);

#ifdef STRUS_BOOST_PYTHON
	void pushTerm_unicode( const String& type_, const WString& value_);
#endif
	/// \brief Add operation "Create an expression from the topmost 'argc' elements of the stack, pop them from the stack and push the expression as single unit on the stack"
	/// \param[in] opname_ name of the expression operator
	/// \param[in] argc_ number of operands (topmost elements from stack) of the expression
	/// \param[in] range_ range number for the expression span in the document
	/// \param[in] cardinality_ number that specifies the minimum size of a subset or subset permutation to match
	void pushExpression( const String& opname_, unsigned int argc_, int range_=0, unsigned int cardinality_=0);

#ifdef STRUS_BOOST_PYTHON
	void pushExpression_2( const String& opname_, unsigned int argc)
	{
		pushExpression( opname_, argc, 0, 0);
	}
	void pushExpression_3( const String& opname_, unsigned int argc, int range_)
	{
		pushExpression( opname_, argc, range_, 0);
	}
	void pushExpression_4( const String& opname_, unsigned int argc, int range_, unsigned int cardinality_)
	{
		pushExpression( opname_, argc, range_, cardinality_);
	}
#endif

	/// \brief Add operation "Attach a variable to the top expression or term on the query stack".
	/// \note The positions of the query matches of the referenced term or expression can be accessed through this variable in summarization.
	/// \param[in] name_ name of the variable attached
	/// \remark The stack is not changed
	void attachVariable( const String& name_);
#ifdef STRUS_BOOST_PYTHON
	void attachVariable_unicode( const WString& name_);
#endif

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
			PushExpression,
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
#ifdef STRUS_BOOST_PYTHON
	std::size_t allocid( const WString& str);
#endif

private:
	std::vector<StackOp> m_ops;
	std::string m_strings;
	std::size_t m_size;
};


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
#ifdef STRUS_BOOST_PYTHON
	void defineFeature_2( const String& set_, const QueryExpression& expr_)
	{
		defineFeature( set_, expr_, 1.0);
	}

	void defineFeature_3( const String& set_, const QueryExpression& expr_, double weight_)
	{
		defineFeature( set_, expr_, weight_);
	}

	void defineFeature_expr_2( const String& set_, const FunctionObject& expr_);
	void defineFeature_expr_3( const String& set_, const FunctionObject& expr_, double weight_);
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

#ifdef STRUS_BOOST_PYTHON
	void addUserName_unicode( const WString& username_);
#endif

	/// \brief Evaluate this query and return the result
	/// \return the result
	RankVector evaluate() const;

private:
	friend class QueryEval;
	Query( const Reference& objbuilder_impl_, const Reference& errorhnd_, const Reference& storage_impl_, const Reference& queryeval_impl_, const Reference& query_impl_, const void* queryproc_)
		:m_errorhnd_impl(errorhnd_),m_objbuilder_impl(objbuilder_impl_),m_storage_impl(storage_impl_),m_queryeval_impl(queryeval_impl_),m_query_impl(query_impl_),m_queryproc(queryproc_)
	{}

	Reference m_errorhnd_impl;
	Reference m_objbuilder_impl;
	Reference m_storage_impl;
	Reference m_queryeval_impl;
	Reference m_query_impl;
	const void* m_queryproc;
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
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \param[in] connectionstring RPC server connection string
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	explicit Context( const std::string& connectionstring);
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \param[in] connectionstring RPC server connection string
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	Context( const std::string& connectionstring, unsigned int maxNofThreads);
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

#ifdef STRUS_BOOST_PYTHON
	void addModulePath_unicode( const WString& paths_);
#endif

	/// \brief Define where to load analyzer resource files from
	/// \param[in] paths_ semicolon separated list of resource search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addResourcePath( const String& paths_);

	/// \brief Define the peer message processor
	/// \param[in] name_ the name of the peer message processor
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void definePeerMessageProcessor( const String& name_);

#ifdef STRUS_BOOST_PYTHON
	void addResourcePath_unicode( const WString& paths_);
#endif

	/// \brief Create a storage client instance of the the default remote storage of the RPC server
	StorageClient createStorageClient();

	/// \brief Create a storage client instance
	/// \param[in] config_ configuration string of the storage client or empty, if the default remote storage of the RPC server is chosen,
	StorageClient createStorageClient( const String& config_);

#ifdef STRUS_BOOST_PYTHON
	StorageClient createStorageClient_0();
	StorageClient createStorageClient_1( const String& config_);
	StorageClient createStorageClient_unicode( const WString& config_);
#endif

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration
 	/// \remark Fails if the storage already exists
	void createStorage( const String& config_);

#ifdef STRUS_BOOST_PYTHON
	void createStorage_unicode( const WString& config_);
#endif
	/// \brief Delete the storage (physically) described by config
	/// \param[in] config_ storage description
	/// \note Handle this function carefully
	void destroyStorage( const String& config_);

#ifdef STRUS_BOOST_PYTHON
	void destroyStorage_unicode( const WString& config_);
#endif
	/// \brief Detect the type of document from its content
	/// \param[in] content the document content to classify
	/// \return the document class
	DocumentClass detectDocumentClass( const String& content);

#ifdef STRUS_BOOST_PYTHON
	DocumentClass detectDocumentClass_unicode( const WString& content);
#endif
	/// \brief Create a document analyzer instance
	/// \param[in] segmentername_ name of the segmenter to use (if empty then the default segmenter is used)
	DocumentAnalyzer createDocumentAnalyzer( const String& segmentername_="");

#ifdef STRUS_BOOST_PYTHON
	DocumentAnalyzer createDocumentAnalyzer_unicode( const WString& segmentername_);

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


