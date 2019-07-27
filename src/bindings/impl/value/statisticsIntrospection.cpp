/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a global term statistics map
#include "introspectionBase.hpp"
#include "statisticsIntrospection.hpp"
#include "strus/statisticsMapInterface.hpp"
#include "strus/errorBufferInterface.hpp"

using namespace strus;
using namespace strus::bindings;

void StatisticsMapIntrospection::serialize( papuga_Serialization& serialization, bool substructure)
{
	if (m_level == 0)
	{
		GlobalCounter nofDocuments = m_impl->nofDocuments();
		if (!papuga_Serialization_pushValue_int( &serialization, nofDocuments)) throw std::bad_alloc();
	}
	else
	{
		GlobalCounter df = m_impl->df( m_type, m_value);
		if (!papuga_Serialization_pushValue_int( &serialization, df)) throw std::bad_alloc();
	}
}

IntrospectionBase* StatisticsMapIntrospection::open( const std::string& name)
{
	switch (m_level)
	{
		case 0: return new StatisticsMapIntrospection( m_errorhnd, m_impl, name);
		case 1: return new StatisticsMapIntrospection( m_errorhnd, m_impl, m_type, name);
		case 2: return NULL;
	}
	return NULL;
}

std::vector<IntrospectionLink> StatisticsMapIntrospection::list()
{
	return std::vector<IntrospectionLink>();
}


