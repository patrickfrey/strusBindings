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


/// \brief Interface for introspection
class IntrospectionBase
{
public:
	virtual ~IntrospectionBase(){}

	virtual void serialize( papuga_Serialization& serialization)=0;
	virtual IntrospectionBase* open( const std::string& name)=0;
	virtual std::vector<std::string> list( bool all)=0;

	void serializeStructureAs( papuga_Serialization& serialization, const char* name);

protected:
	void serializeList( papuga_Serialization& serialization, bool all=false);
	static std::vector<std::string> getList( const char** ar, bool all);
	static std::vector<std::string> getKeyList( const std::vector<std::pair<std::string,std::string> >& ar);
	static std::runtime_error unresolvable_exception();
};


class IntrospectionValueIterator
	:public IntrospectionBase
{
public:
	enum {MaxListSizeDeepExpansion=50};

	IntrospectionValueIterator( ErrorBufferInterface* errorhnd_, const strus::Reference<ValueIteratorInterface>& impl_, bool prefixBound_, const std::string& name_="");

	virtual ~IntrospectionValueIterator(){}

	virtual void serialize( papuga_Serialization& serialization);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<std::string> list( bool all);

private:
	ErrorBufferInterface* m_errorhnd;
	strus::Reference<ValueIteratorInterface> m_impl;
	std::string m_name;
	bool m_prefixBound;
};

}}//namespace
#endif



