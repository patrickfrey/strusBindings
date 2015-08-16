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
#ifndef _STRUS_BINDING_SWIG_HPP_INCLUDED
#define _STRUS_BINDING_SWIG_HPP_INCLUDED

#if defined DOXYGEN_JAVA
namespace java {
namespace lang {
class String
	:public std::string
{
public:
	///\brief Constructor
	String( char[]);
	///\brief Copy constructor
	String( const String& o);
};
}
namespace util {

/// \brief Interface of java abstract list as used by derived classes of strus
template <Element>
class AbstractList :public std::vector<Element>
{
public:
	///\brief Get an element by index
	///\param[in] i index starting from 0
	///\return the element reference
	const_reference get(int i) const throw (std::out_of_range);
	///\brief Set an element
	///\param[in] i index starting from 0
	///\param[in] val value of the element to set
	value_type set(int i, const value_type& val) throw (std::out_of_range);
	///\brief Get the size of the array
	int32_t size() const;
	///\brief Remove a range of elements from the array
	void removeRange(int32_t from, int32_t to);
};
}}

namespace net {
namespace strus {
namespace api {
/// \brief Fixed size vector of strings
class StringVector :public java::util::AbstractList<std::string>
{
	///\brief Create a String vector with a preallocated size
	///\param[in] size number of elements to preallocate
	StringVector(int32_t size);
};

/// \brief Fixed size vector of normalizer definitions
class NormalizerVector :public java::util::AbstractList<Normalizer>
{
public:
	///\brief Create a Normalizer vector with a preallocated size
	///\param[in] size number of elements to preallocate
	NormalizerVector(int32_t size);
};

/// \brief Fixed size vector of query/document terms
class TermVector :public java::util::AbstractList<Term>
{
public:
	///\brief Create a Term vector with a preallocated size
	///\param[in] size number of elements to preallocate
	TermVector(int32_t size);
};

/// \brief Fixed size vector of a query rank. Represents the result of an information rerieval query
class RankVector :public java::util::AbstractList<Rank>
{
public:
	///\brief Create a Rank (item of a query result) vector with a preallocated size
	///\param[in] size number of elements to preallocate
	RankVector(int32_t size);
};

/// \brief Fixed size vector of a query rank. Represents the result of an information rerieval query
class RankAttributeVector :public java::util::AbstractList<RankAttribute>
{
public:
	///\brief Create a RankAttribute (attribute of a query result) vector with a preallocated size
	///\param[in] size number of elements to preallocate
	RankAttributeVector(int32_t size);
};

/// \brief Fixed size vector of document attributes
class AttributeVector :public java::util::AbstractList<Attribute>
{
public:
	///\brief Create a document attribute vector with a preallocated size
	///\param[in] size number of elements to preallocate
	AttributeVector(int32_t size);
};

/// \brief Fixed size vector of document meta data elements
class MetaDataVector :public java::util::AbstractList<MetaData>
{
public:
	///\brief Create a document meta data vector with a preallocated size
	///\param[in] size number of elements to preallocate
	MetaDataVector(int32_t size);
};
}}}

#elif defined DOXYGEN_PHP
#elif defined DOXYGEN_PYTHON
#else
#error Unknown language (swig.hpp)
#endif

