/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a storage client
#include "storageIntrospection.hpp"
#include "introspectionTemplates.hpp"
#include "serializer.hpp"
#include "private/internationalization.hpp"
#include "strus/reference.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/configParser.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "papuga/allocator.h"
#include <cstring>
#include <vector>
#include <utility>
#include <cstdint>

using namespace strus;
using namespace strus::bindings;

namespace {
template <class TypeName>
IntrospectionBase* createIntrospectionAtomic( ErrorBufferInterface* errorhnd, const TypeName& val)
{
	return new IntrospectionAtomic<TypeName>( errorhnd, val);
}
IntrospectionBase* createIntrospectionValueIterator( ErrorBufferInterface* errorhnd, ValueIteratorInterface* val)
{
	return val ? new IntrospectionValueIterator( errorhnd, val) : NULL;
}
}//namespace

void StorageIntrospection::serialize( papuga_Serialization& serialization) const
{
	serializeList( serialization);
}

IntrospectionBase* StorageIntrospection::open( const std::string& name) const
{
	if (name == "config") return new IntrospectionConfig<StorageClientInterface>( m_errorhnd, m_impl);
	else if (name == "nofdocs") return createIntrospectionAtomic( m_errorhnd, (std::int64_t) m_impl->nofDocumentsInserted());
	else if (name == "maxdocno") return createIntrospectionAtomic( m_errorhnd, (std::int64_t) m_impl->maxDocumentNumber());
	else if (name == "user") return createIntrospectionValueIterator( m_errorhnd, m_impl->createUserNameIterator());
	else if (name == "termtype") return createIntrospectionValueIterator( m_errorhnd, m_impl->createTermTypeIterator());
	else if (name == "metadataname")
	{
		Reference<MetaDataReaderInterface> reader( m_impl->createMetaDataReader());
		if (!reader.get()) throw strus::runtime_error( "%s", m_errorhnd->fetchError());
		return createIntrospectionAtomic( m_errorhnd, reader->getNames());
	}
	else if (name == "attributename")
	{
		Reference<AttributeReaderInterface> reader( m_impl->createAttributeReader());
		if (!reader.get()) throw strus::runtime_error( "%s", m_errorhnd->fetchError());
		return createIntrospectionAtomic( m_errorhnd, reader->getNames());
	}
	return NULL;
}

std::vector<std::string> StorageIntrospection::list() const
{
	static const char* ar[] = {"config","nofdocs","maxdocno","user","termtype","metadataname","attributename",NULL};
	return getList( ar);
}




