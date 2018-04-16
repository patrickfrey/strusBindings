/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Abstract interface for introspection
#ifndef _STRUS_BINDING_IMPL_VALUE_INTROSPECTION_BASE_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_INTROSPECTION_BASE_HPP_INCLUDED
#include "papuga/serialization.h"
#include "strus/valueIteratorInterface.hpp"
#include "strus/reference.hpp"
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>

namespace strus {

/// \brief Forward declaration
class ErrorBufferInterface;
/// \brief Forward declaration
class ValueIteratorInterface;

namespace bindings {

/// \brief Maximum size of an auto expanded list
enum {MaxListSizeDeepExpansion=20};


/// \brief Introspection link element
class IntrospectionLink
{
public:
	IntrospectionLink( bool autoexpand_, const std::string& value_)
		:m_autoexpand(autoexpand_),m_value(value_){}
	IntrospectionLink( const IntrospectionLink& o)
		:m_autoexpand(o.m_autoexpand),m_value(o.m_value){}

	bool autoexpand() const			{return m_autoexpand;}
	const std::string& value() const	{return m_value;}

	static std::vector<IntrospectionLink> getList( bool autoexpand_, const std::vector<std::string>& values);

	bool m_autoexpand;
	std::string m_value;
};

/// \brief Interface for introspection
class IntrospectionBase
{
public:
	virtual ~IntrospectionBase(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)=0;
	virtual IntrospectionBase* open( const std::string& name)=0;
	virtual std::vector<IntrospectionLink> list()=0;

	void serializeStructureAs( papuga_Serialization& serialization, const char* name, const std::string& path);
	void getPathContent( papuga_Serialization& serialization, const std::vector<std::string>& path);

protected:
	void serializeMembers( papuga_Serialization& serialization, const std::string& path);
	static std::vector<IntrospectionLink> getList( const char** ar);
	static std::vector<std::string> getKeyList( const std::vector<std::pair<std::string,std::string> >& ar);
	static std::runtime_error unresolvable_exception();
};


class IntrospectionValueIterator
	:public IntrospectionBase
{
public:
	IntrospectionValueIterator( ErrorBufferInterface* errorhnd_, const strus::Reference<ValueIteratorInterface>& impl_, bool prefixBound_, const std::string& name_="");

	virtual ~IntrospectionValueIterator(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	strus::Reference<ValueIteratorInterface> m_impl;
	std::string m_name;
	bool m_prefixBound;
};

}}//namespace
#endif



