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
#include "introspectionTemplates.hpp"
#include "strus/statisticsMapInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/stdint.h"

using namespace strus;
using namespace strus::bindings;

void StatisticsMapIntrospection::serialize( papuga_Serialization& serialization, bool substructure)
{
	switch (m_level)
	{
		case 0:
		{
			serializeMembers( serialization, substructure);
			break;
		}
		case 1:
			if (substructure) if (!papuga_Serialization_pushValue_void( &serialization)) throw std::bad_alloc();
			break;
		case 2:
		{
			GlobalCounter df = m_impl->df( m_type, m_value);
			if (!papuga_Serialization_pushValue_int( &serialization, df)) throw std::bad_alloc();
			if (!df && m_errorhnd->hasError()) throw strus::runtime_error(m_errorhnd->fetchError());
			break;
		}
	}
}

IntrospectionBase* StatisticsMapIntrospection::open( const std::string& name)
{
	switch (m_level)
	{
		case 0:
		{
			if (name == "nofdocs")
			{
				return new IntrospectionAtomic<GlobalCounter>( m_errorhnd, m_impl->nofDocuments());
			}
			else if (name == "types")
			{
				return new IntrospectionStructure<std::vector<std::string> >( m_errorhnd, m_impl->types());
			}
			else
			{
				return new StatisticsMapIntrospection( m_errorhnd, m_impl, name);
			}
		}
		case 1: return new StatisticsMapIntrospection( m_errorhnd, m_impl, m_type, name);
		case 2: return NULL;
	}
	return NULL;
}

std::vector<IntrospectionLink> StatisticsMapIntrospection::list()
{
	static const char* ar_0[] = {"types","nofdocs",NULL};
	if (m_level == 0) return getList( ar_0); 
	return std::vector<IntrospectionLink>();
}


