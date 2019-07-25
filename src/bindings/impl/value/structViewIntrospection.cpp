/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the introspection of a static structure defined as StructView
#include "structViewIntrospection.hpp"
#include "strus/structView.hpp"
#include "strus/errorBufferInterface.hpp"

using namespace strus;
using namespace strus::bindings;

void StructViewIntrospection::serialize( papuga_Serialization& serialization, const std::string& path, bool substructure)
{
	switch (m_impl.type())
	{
		case StructView::Null:
			if (!papuga_Serialization_pushValue_void( &serialization)) throw std::bad_alloc();
			break;
		case StructView::Numeric:
		{
			NumericVariant::String strval = m_impl.asnumeric().tostring();
			if (!papuga_Serialization_pushValue_charp( &serialization, strval.c_str())) throw std::bad_alloc();
			break;
		}
		case StructView::String:
			papuga_Serialization_pushValue_string( &serialization, m_impl.asstring().c_str(), m_impl.asstring().size());
			break;
		case StructView::Structure:
			if (m_impl.isArray())
			{
				bool rt = true;
				if (substructure) rt &= papuga_Serialization_pushOpen( &serialization);
				std::size_t ai = 0, ae = m_impl.arraySize();
				for (; ai != ae; ++ai)
				{
					StructViewIntrospection elem( m_errorhnd, *m_impl.get( ai));
					elem.serialize( serialization, path, true/*substructure*/);
				}
				if (substructure) rt &= papuga_Serialization_pushClose( &serialization);
				if (!rt) throw std::bad_alloc();
			}
			else
			{
				serializeMembers( serialization, path, substructure);
			}
			break;
	}
}

IntrospectionBase* StructViewIntrospection::open( const std::string& name)
{
	if (m_impl.isAtomic())
	{
		return NULL;
	}
	else if (m_impl.isArray())
	{
		return NULL;
	}
	else
	{
		const StructView* chld = m_impl.get( name);
		if (!chld) return NULL;

		return new StructViewIntrospection( m_errorhnd, *chld);
	}
}

std::vector<IntrospectionLink> StructViewIntrospection::list()
{
	std::vector<IntrospectionLink> rt;
	if (m_impl.isAtomic())
	{
		throw unresolvable_exception();
	}
	else if (m_impl.isArray())
	{
		throw unresolvable_exception();
	}
	else
	{
		StructView::dict_iterator di = m_impl.dict_begin(), de = m_impl.dict_end();
		for (; di != de; ++di)
		{
			rt.push_back( IntrospectionLink( true/*autoexpand*/, di->first));
		}
	}
	return rt;
}




