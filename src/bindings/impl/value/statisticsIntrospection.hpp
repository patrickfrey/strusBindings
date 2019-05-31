/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a global term statistics map
#ifndef _STRUS_BINDING_IMPL_VALUE_STATISTICS_MAP_INTROSPECTION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_STATISTICS_MAP_INTROSPECTION_HPP_INCLUDED
#include "introspectionBase.hpp"
#include <string>

namespace strus {

/// \brief Forward declaration
class ErrorBufferInterface;
/// \brief Forward declaration
class StatisticsMapInterface;

namespace bindings {

class StatisticsMapIntrospection
	:public IntrospectionBase
{
public:
	StatisticsMapIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StatisticsMapInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_type()
		,m_value()
		,m_level(0)
		{}
	StatisticsMapIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StatisticsMapInterface* impl_,
			const std::string& type_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_type(type_)
		,m_level(1)
		{}
	StatisticsMapIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StatisticsMapInterface* impl_,
			const std::string& type_,
			const std::string& value_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_type(type_)
		,m_value(value_)
		,m_level(2)
		{}
	virtual ~StatisticsMapIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path, bool substructure);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const StatisticsMapInterface* m_impl;
	std::string m_type;
	std::string m_value;
	int m_level;
};

}}//namespace
#endif

