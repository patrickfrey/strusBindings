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
#include "strus/base/stdint.h"
#include "strus/base/numstring.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/configParser.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "papuga/allocator.h"
#include "papuga/constants.h"
#include <cstring>
#include <vector>
#include <utility>
#include <cstdint>
#include <limits>

using namespace strus;
using namespace strus::bindings;

namespace {
template <class TypeName>
IntrospectionBase* createIntrospectionAtomic( ErrorBufferInterface* errorhnd, const TypeName& val)
{
	return new IntrospectionAtomic<TypeName>( errorhnd, val);
}
IntrospectionBase* createIntrospectionValueIterator( ErrorBufferInterface* errorhnd, ValueIteratorInterface* val, const std::string& prefix="")
{
	return val ? new IntrospectionValueIterator( errorhnd, val, prefix) : NULL;
}
}//namespace


namespace {
class AttibuteIntrospection
	:public IntrospectionBase
{
public:
	AttibuteIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			Index docno_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_->createAttributeReader())
		,m_docno(docno_)
	{
		if (!m_impl) throw strus::runtime_error("%s", m_errorhnd->fetchError());
		m_impl->skipDoc( m_docno);
	}
	~AttibuteIntrospection()
	{
		delete m_impl;
	}

	virtual void serialize( papuga_Serialization& serialization) const
	{
		serializeList( serialization);
	}

	virtual IntrospectionBase* open( const std::string& name) const
	{
		Index elemhandle = m_impl->elementHandle( name.c_str());
		if (!elemhandle) return NULL;
		std::string value = m_impl->getValue( elemhandle);
		if (value.empty()) return NULL;
		return createIntrospectionAtomic( m_errorhnd, value);
	}

	virtual std::vector<std::string> list( bool all) const
	{
		std::vector<std::string> rt;
		std::vector<std::string> candidates = m_impl->getNames();
		std::vector<std::string>::const_iterator ci = candidates.begin(), ce = candidates.end();
		for (; ci != ce; ++ci)
		{
			Index elemhandle = m_impl->elementHandle( ci->c_str());
			if (!elemhandle) return std::vector<std::string>();
			std::string value = m_impl->getValue( elemhandle);
			if (!value.empty()) rt.push_back( *ci);
		}
		return rt;
	}

	static IntrospectionBase* createSingle( ErrorBufferInterface* errorhnd_, const StorageClientInterface* sto, Index docno_, const std::string& name)
	{
		strus::local_ptr<AttributeReaderInterface> areader( sto->createAttributeReader());
		if (!areader.get()) return NULL;
		Index elemhandle = areader->elementHandle( name.c_str());
		if (!elemhandle) return NULL;
		std::string value = areader->getValue( elemhandle);
		if (value.empty()) return NULL;
		return createIntrospectionAtomic( errorhnd_, value);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	AttributeReaderInterface* m_impl;
	Index m_docno;
	std::string m_name;
};


class TermIntrospection
	:public IntrospectionBase
{
public:
	enum Function {Df};

public:
	TermIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			Function function_,
			const std::string& type_="",
			const std::string& value_="")
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_function(function_)
		,m_type(type_)
		,m_value(value_)
		{}

	virtual void serialize( papuga_Serialization& serialization) const
	{
		throw unresolvable_exception();
	}

	virtual IntrospectionBase* open( const std::string& name) const
	{
		if (m_type.empty())
		{
			return new TermIntrospection( m_errorhnd, m_impl, m_function, name);
		}
		else if (m_value.empty())
		{
			return new TermIntrospection( m_errorhnd, m_impl, m_function, m_type, name);
		}
		else
		{
			switch (m_function)
			{
				case Df: return createIntrospectionAtomic( m_errorhnd, (int64_t)m_impl->documentFrequency( m_type, name));
				default: return NULL;
			}
		}
	}

	virtual std::vector<std::string> list( bool all) const
	{
		throw unresolvable_exception();
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	Function m_function;
	std::string m_type;
	std::string m_value;
};


class DocumentIntrospection
	:public IntrospectionBase
{
public:
	DocumentIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const Index& docno_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docno(docno_)
		{}

	virtual void serialize( papuga_Serialization& serialization) const
	{
		throw unresolvable_exception();
	}

	virtual IntrospectionBase* open( const std::string& name) const
	{
		return NULL;
	}

	virtual std::vector<std::string> list( bool all) const
	{
		throw unresolvable_exception();
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	strus::Index m_docno;
};


class DocidIntrospection
	:public IntrospectionBase
{
public:
	DocidIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const std::string& docid_="")
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docid(docid_)
		{}

	virtual void serialize( papuga_Serialization& serialization) const
	{
		throw unresolvable_exception();
	}

	virtual IntrospectionBase* open( const std::string& name) const
	{
		strus::Index docno = 0;
		if (m_docid.empty() && name[0] == '_')
		{
			strus::NumParseError numerr = NumParseOk;
			docno = strus::intFromString( name.c_str()+1, name.size()-1, std::numeric_limits<strus::Index>::max(), numerr);
			if (!docno)
			{
				if (numerr == NumParseErrNoMem) throw std::bad_alloc();
				if (numerr == NumParseErrOutOfRange) throw std::runtime_error(_TXT("document number (prefix '_') out of range"));
				if (numerr == NumParseErrOutOfRange) throw std::runtime_error(_TXT("document number (prefix '_') out of range"));
				throw std::runtime_error(_TXT("document number (prefix '_') is invalid"));
			}
			return new DocumentIntrospection( m_errorhnd, m_impl, docno);
		}
		std::string docidprefix = m_docid.empty() ? name : (m_docid + '/' + name);
		docno = m_impl->documentNumber( docidprefix);
		if (docno)
		{
			return new DocumentIntrospection( m_errorhnd, m_impl, docno);
		}
		else
		{
			return new DocidIntrospection( m_errorhnd, m_impl, docidprefix);
		}
	}

	virtual std::vector<std::string> list( bool all) const
	{
		strus::local_ptr<IntrospectionBase> values( createIntrospectionValueIterator( m_errorhnd, m_impl->createDocIdIterator(), m_docid));
		if (!values.get()) throw std::bad_alloc();
		return values->list( all);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	std::string m_docid;
};
}//namespace


void StorageIntrospection::serialize( papuga_Serialization& serialization) const
{
	serializeList( serialization);
}

IntrospectionBase* StorageIntrospection::open( const std::string& name) const
{
	if (name == "config") return new IntrospectionConfig<StorageClientInterface>( m_errorhnd, m_impl);
	else if (name == "nofdocs") return createIntrospectionAtomic( m_errorhnd, (int64_t) m_impl->nofDocumentsInserted());
	else if (name == "maxdocno") return createIntrospectionAtomic( m_errorhnd, (int64_t) m_impl->maxDocumentNumber());
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
	else if (name == "df")
	{
		return new TermIntrospection( m_errorhnd, m_impl, TermIntrospection::Df);
	}
	else if (name == "doc")
	{
		return new DocidIntrospection( m_errorhnd, m_impl);
	}
	return NULL;
}

std::vector<std::string> StorageIntrospection::list( bool all) const
{
	static const char* ar[] = {"config","nofdocs","maxdocno","user","termtype","metadataname","attributename",".df",".doc",NULL};
	return getList( ar, all);
}




