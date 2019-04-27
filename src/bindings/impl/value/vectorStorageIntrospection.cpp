/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a storage client
#include "vectorStorageIntrospection.hpp"
#include "introspectionTemplates.hpp"
#include "serializer.hpp"
#include "private/internationalization.hpp"
#include "strus/constants.hpp"
#include "strus/base/stdint.h"
#include "strus/base/numstring.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/fileio.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "papuga/allocator.h"
#include "papuga/constants.h"
#include <cstring>
#include <vector>
#include <set>
#include <utility>
#include <limits>
#include <algorithm>

using namespace strus;
using namespace strus::bindings;

template <class TypeName>
static IntrospectionBase* createIntrospectionAtomic( ErrorBufferInterface* errorhnd, const TypeName& val)
{
	return new IntrospectionAtomic<TypeName>( errorhnd, val);
}
template <class TypeName>
static IntrospectionBase* createIntrospectionStructure( ErrorBufferInterface* errorhnd, const TypeName& val)
{
	return new IntrospectionStructure<TypeName>( errorhnd, val);
}

static bool isValidFeatureValuePrefix( const VectorStorageClientInterface* vstorage, const std::string& prefix, ErrorBufferInterface* errorhnd)
{
	strus::local_ptr<ValueIteratorInterface> vitr( vstorage->createFeatureValueIterator());
	if (!vitr.get()) throw std::runtime_error( errorhnd->fetchError());
	vitr->skipPrefix( prefix.c_str(), prefix.size());
	return !vitr->fetchValues( 1).empty();
}


namespace {

class FeatureTypeListIntrospection
	:public IntrospectionBase
{
public:
	FeatureTypeListIntrospection(
			ErrorBufferInterface* errorhnd_,
			const VectorStorageClientInterface* impl_,
			const std::string& feat_,
			const std::vector<std::string>& types_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_feat(feat_)
		,m_types(types_)
		{}
	virtual ~FeatureTypeListIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path, bool substructure)
	{
		serializeMembers( serialization, path, substructure);
	}

	virtual std::vector<IntrospectionLink> list()
	{
		return IntrospectionLink::getList( false/*autoexpand*/, m_types);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		std::vector<std::string>::const_iterator ti = std::find( m_types.begin(), m_types.end(), name);
		if (ti == m_types.end())
		{
			throw unresolvable_exception();
		}
		else
		{
			WordVector vec = m_impl->featureVector( *ti, m_feat);
			return strus::bindings::IntrospectionValueListConstructor<WordVector>::func( m_errorhnd, vec);
		}
	}
	
private:
	ErrorBufferInterface* m_errorhnd;
	const VectorStorageClientInterface* m_impl;
	std::string m_feat;
	std::vector<std::string> m_types;
};


class FeatureValueIntrospection
	:public IntrospectionBase
{
public:
	FeatureValueIntrospection(
			ErrorBufferInterface* errorhnd_,
			const VectorStorageClientInterface* impl_,
			const std::string& prefix_="")
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_prefix(prefix_)
		{}
	virtual ~FeatureValueIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path, bool substructure)
	{
		serializeMembers( serialization, path, substructure);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (!m_prefix.empty()) return NULL;
		if (!name.empty() && name[ name.size()-1] == '*')
		{
			std::string prefix_( name.c_str(), name.size()-1);
			if (isValidFeatureValuePrefix( m_impl, prefix_, m_errorhnd))
			{
				return new FeatureValueIntrospection( m_errorhnd, m_impl, prefix_);
			}
			else
			{
				throw unresolvable_exception();
			}
		}
		else
		{
			std::vector<std::string> types = m_impl->featureTypes( name);
			if (types.empty())
			{
				throw unresolvable_exception();
			}
			else
			{
				return new FeatureTypeListIntrospection( m_errorhnd, m_impl, name, types);
			}
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		if (m_prefix.empty())
		{
			IntrospectionValueIterator vitr( m_errorhnd, m_impl->createFeatureValueIterator(), false/*prefixBound*/);
			return vitr.list();
		}
		else
		{
			strus::local_ptr<ValueIteratorInterface> itr( m_impl->createFeatureValueIterator());
			if (!itr.get()) throw std::runtime_error( m_errorhnd->fetchError());
			itr->skipPrefix( m_prefix.c_str(), m_prefix.size());

			std::vector<IntrospectionLink> rt;
			std::vector<std::string> values = itr->fetchValues( MaxListSizeDeepExpansion);
			std::vector<std::string>::const_iterator vi = values.begin(), ve = values.end();
			for (; vi != ve; ++vi)
			{
				rt.push_back( IntrospectionLink( false/*autoexpand*/, *vi));
			}
			return rt;
		}
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const VectorStorageClientInterface* m_impl;
	std::string m_prefix;
};
}//anonymous namespace


void VectorStorageIntrospection::serialize( papuga_Serialization& serialization, const std::string& path, bool substructure)
{
	serializeMembers( serialization, path, substructure);
}

IntrospectionBase* VectorStorageIntrospection::open( const std::string& name)
{
	if (name == "config") return new IntrospectionConfig<VectorStorageClientInterface>( m_errorhnd, m_impl);
	else if (name == "noftypes") return createIntrospectionAtomic( m_errorhnd, (int64_t) m_impl->nofTypes());
	else if (name == "noffeats") return createIntrospectionAtomic( m_errorhnd, (int64_t) m_impl->nofFeatures());
	else if (name == "feature") return new FeatureValueIntrospection( m_errorhnd, m_impl);
	return NULL;
}

std::vector<IntrospectionLink> VectorStorageIntrospection::list()
{
	static const char* ar[] = {"config","noftypes","noffeats",".feature",NULL};
	return getList( ar);
}




