/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a statistics storage
#ifndef _STRUS_BINDING_IMPL_VALUE_STATISTICS_STORAGE_INTROSPECTION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_STATISTICS_STORAGE_INTROSPECTION_HPP_INCLUDED
#include "introspectionBase.hpp"
#include "strus/statisticsStorageClientInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include <string>

namespace strus {

/// \brief Forward declaration
class ErrorBufferInterface;

namespace bindings {

/// \brief Forward declaration
class StatisticsStorageClientImpl;

class StatisticsStorageIntrospection
	:public IntrospectionBase
{
public:
	StatisticsStorageIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StatisticsStorageClientImpl* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		{}
	virtual ~StatisticsStorageIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const StatisticsStorageClientImpl* m_impl;
};

}}//namespace
#endif

