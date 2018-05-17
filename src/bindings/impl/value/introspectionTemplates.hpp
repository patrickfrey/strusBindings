/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Abstract interface for introspection
#ifndef _STRUS_BINDING_IMPL_VALUE_INTROSPECTION_TEMPLATES_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_INTROSPECTION_TEMPLATES_HPP_INCLUDED
#include "strus/errorBufferInterface.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/numstring.hpp"
#include "strus/base/stdint.h"
#include "introspectionBase.hpp"
#include "serializer.hpp"

namespace strus {
namespace bindings {

template <class TypeName>
class IntrospectionAtomic
	:public IntrospectionBase
{
public:
	IntrospectionAtomic(
			ErrorBufferInterface* errorhnd_,
			const TypeName& value_)
		:m_errorhnd(errorhnd_),m_value(value_){}
	virtual ~IntrospectionAtomic(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		Serializer::serialize( &serialization, m_value, true/*deep*/);
	}
	virtual IntrospectionBase* open( const std::string& name)
	{
		return NULL;
	}
	virtual std::vector<IntrospectionLink> list()
	{
		throw unresolvable_exception();
	}
private:
	ErrorBufferInterface* m_errorhnd;
	TypeName m_value;
};


template <class TypeName>
class IntrospectionKeyValueList
	:public IntrospectionBase
{
public:
	IntrospectionKeyValueList(
			ErrorBufferInterface* errorhnd_,
			const TypeName& value_)
		:m_errorhnd(errorhnd_),m_value(value_){}
	virtual ~IntrospectionKeyValueList(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		serializeMembers( serialization, path);
	}
	virtual IntrospectionBase* open( const std::string& name)
	{
		std::vector<std::string> rt;
		typename TypeName::const_iterator li = m_value.begin(), le = m_value.end();
		for (; li != le && name != li->first; ++li){}
		return (li == le) ? NULL : new IntrospectionAtomic<typename TypeName::value_type::second_type>( m_errorhnd, li->second);
	}
	virtual std::vector<IntrospectionLink> list()
	{
		std::vector<IntrospectionLink> rt;
		typename TypeName::const_iterator vi = m_value.begin(), ve = m_value.end();
		for (; vi != ve; ++vi)
		{
			rt.push_back( IntrospectionLink( true/*autoexpand*/, vi->first));
		}
		return rt;
	}

private:
	ErrorBufferInterface* m_errorhnd;
	TypeName m_value;
};


template <class TypeName>
class IntrospectionObjectList
	:public IntrospectionBase
{
public:
	typedef IntrospectionBase* (*ElementConstructor)( strus::ErrorBufferInterface*, const typename TypeName::value_type&);

	IntrospectionObjectList(
			ErrorBufferInterface* errorhnd_,
			const TypeName& value_,
			ElementConstructor elementConstructor_)
		:m_errorhnd(errorhnd_),m_value(value_),m_elementConstructor(elementConstructor_){}
	virtual ~IntrospectionObjectList(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		serializeMembers( serialization, path);
	}
	virtual IntrospectionBase* open( const std::string& name)
	{
		NumParseError err = strus::NumParseOk;
		int64_t idx = strus::intFromString( name.c_str(), name.size(), m_value.size(), err);
		if (err != strus::NumParseOk) return NULL;
		return (*m_elementConstructor)( m_errorhnd, m_value[ idx]);
	}
	virtual std::vector<IntrospectionLink> list()
	{
		std::vector<IntrospectionLink> rt;
		int ii=0, ie=m_value.size();
		for (; ii != ie; ++ii)
		{
			rt.push_back( IntrospectionLink( true, strus::string_format( "%d", ii)));
		}
		return rt;
	}

private:
	ErrorBufferInterface* m_errorhnd;
	TypeName m_value;
	ElementConstructor m_elementConstructor;
};

template <class TypeName>
struct IntrospectionValueListConstructor
{
	static IntrospectionBase* func( strus::ErrorBufferInterface* errhnd, const TypeName& val)
	{
		return new IntrospectionAtomic<TypeName>( errhnd, val);
	}
};

template <class TypeName>
class IntrospectionValueList
	:public IntrospectionObjectList<TypeName>
{
public:
	IntrospectionValueList(
			ErrorBufferInterface* errorhnd_,
			const TypeName& value_)
		:IntrospectionObjectList<TypeName>( errorhnd_, value_, &IntrospectionValueListConstructor<TypeName>::func){}
};


template<class InterfaceType>
class IntrospectionConfig
	:public IntrospectionBase
{
public:
	IntrospectionConfig( ErrorBufferInterface* errorhnd_, const InterfaceType* impl_)
		:m_errorhnd(errorhnd_),m_impl(impl_){}
	virtual ~IntrospectionConfig(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		Serializer::serialize( &serialization, strus::getConfigStringItems( m_impl->config(), m_errorhnd), true/*deep*/);
	}
	virtual IntrospectionBase* open( const std::string& name)
	{
		typedef std::vector<std::pair<std::string,std::string> > CfgItems;
		CfgItems items = strus::getConfigStringItems( m_impl->config(), m_errorhnd);
		typename CfgItems::const_iterator ci = items.begin(), ce = items.end();
		for (; ci != ce; ++ci)
		{
			if (name == ci->first)
			{
				if (name == "metadata")
				{
					items = strus::getSubConfigStringItems( ci->second, m_errorhnd);
					return new IntrospectionKeyValueList<CfgItems>( m_errorhnd, items);
				}
				else
				{
					return new IntrospectionAtomic<std::string>( m_errorhnd, ci->second);
				}
			}
		}
		return NULL;
	}
	virtual std::vector<IntrospectionLink> list()
	{
		return IntrospectionLink::getList( true, IntrospectionBase::getKeyList( strus::getConfigStringItems( m_impl->config(), m_errorhnd)));
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const InterfaceType* m_impl;
};

}}//namespace
#endif



