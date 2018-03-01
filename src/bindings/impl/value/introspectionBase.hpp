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


/// \brief Interface for introspection
class IntrospectionBase
{
public:
	virtual ~IntrospectionBase(){}

	virtual void serialize( papuga_Serialization& serialization) const=0;
	virtual IntrospectionBase* open( const std::string& name) const=0;
	virtual std::vector<std::string> list( bool all) const=0;

protected:
	void serializeList( papuga_Serialization& serialization) const;
	void serializeStructureAs( papuga_Serialization& serialization, const char* name) const;
	static std::vector<std::string> getList( const char** ar, bool all);
	static std::vector<std::string> getKeyList( const std::vector<std::pair<std::string,std::string> >& ar);
	static std::runtime_error unresolvable_exception();
};


std::runtime_error introspection_error( const char* msg, ErrorBufferInterface* errorhnd);



class IntrospectionValueIterator
	:public IntrospectionBase
{
public:
	IntrospectionValueIterator( ErrorBufferInterface* errorhnd_, ValueIteratorInterface* impl_, const std::string& name_="");

	~IntrospectionValueIterator();

	virtual void serialize( papuga_Serialization& serialization) const;
	virtual IntrospectionBase* open( const std::string& name) const;
	virtual std::vector<std::string> list( bool all) const;

private:
	ErrorBufferInterface* m_errorhnd;
	ValueIteratorInterface* m_impl;
	std::string m_name;
};

}}//namespace
#endif



