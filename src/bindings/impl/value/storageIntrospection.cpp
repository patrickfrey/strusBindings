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
#include "strus/constants.hpp"
#include "strus/base/stdint.h"
#include "strus/base/numstring.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/fileio.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/forwardIteratorInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "papuga/allocator.h"
#include "papuga/constants.h"
#include <cstring>
#include <vector>
#include <set>
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
IntrospectionBase* createIntrospectionValueIterator( ErrorBufferInterface* errorhnd, ValueIteratorInterface* val, bool prefixBound, const std::string& prefix="")
{
	if (!val) throw strus::runtime_error("%s", errorhnd->fetchError());
	return new IntrospectionValueIterator( errorhnd, val, prefixBound, prefix);
}
IntrospectionBase* createIntrospectionDirectoryIterator( ErrorBufferInterface* errorhnd, ValueIteratorInterface* val, const std::string& prefix, bool complete)
{
	if (!val) throw strus::runtime_error("%s", errorhnd->fetchError());
	return new IntrospectionDirectoryIterator( errorhnd, val, prefix, complete);
}
static strus::Index parseIndex( const char* start, std::size_t size, const char* descr)
{
	strus::NumParseError numerr = NumParseOk;
	strus::Index rt = strus::intFromString( start, size, std::numeric_limits<strus::Index>::max(), numerr);
	if (!rt)
	{
		if (numerr == NumParseErrNoMem) throw std::bad_alloc();
		if (numerr == NumParseErrOutOfRange) throw strus::runtime_error(_TXT("%s out of range"), descr);
		if (numerr == NumParseErrOutOfRange) throw strus::runtime_error(_TXT("%s out of range"), descr);
		throw strus::runtime_error(_TXT("%s is invalid"), descr);
	}
	return rt;
}
static strus::Index parseIndex( const std::string& val, const char* descr)
{
	return parseIndex( val.c_str(), val.size(), descr);
}
}//namespace

static std::string getDocidFromDocno( const StorageClientInterface* storage, const strus::Index& docno, ErrorBufferInterface* errorhnd)
{
	strus::local_ptr<AttributeReaderInterface> areader( storage->createAttributeReader());
	if (!areader.get()) throw std::runtime_error( errorhnd->fetchError());
	Index elemhandle = areader->elementHandle( Constants::attribute_docid());
	if (!elemhandle) return std::string();
	areader->skipDoc( docno);
	return areader->getValue( elemhandle);
}

static bool isValidDocumentIdPrefix( const StorageClientInterface* storage, const std::string& prefix, ErrorBufferInterface* errorhnd)
{
	strus::local_ptr<ValueIteratorInterface> vitr( storage->createDocIdIterator());
	if (!vitr.get()) throw std::runtime_error( errorhnd->fetchError());
	vitr->skipPrefix( prefix.c_str(), prefix.size());
	return !vitr->fetchValues( 1).empty();
}


namespace {
class AttibuteIntrospection
	:public IntrospectionBase
{
public:
	AttibuteIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const strus::Index& docno_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docno(docno_)
	{}
	virtual ~AttibuteIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		serializeMembers( serialization, path);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		std::string value;
		return getValue( value, name) ? createIntrospectionAtomic( m_errorhnd, value) : NULL;
	}

	virtual std::vector<IntrospectionLink> list()
	{
		std::vector<IntrospectionLink> rt;
		strus::local_ptr<AttributeReaderInterface> areader( m_impl->createAttributeReader());
		if (!areader.get()) throw std::runtime_error( m_errorhnd->fetchError());
		areader->skipDoc( m_docno);
		std::vector<std::string> candidates = areader->getNames();
		std::vector<std::string>::const_iterator ci = candidates.begin(), ce = candidates.end();
		for (; ci != ce; ++ci)
		{
			std::string value;
			if (getValue( value, areader.get(), *ci)) rt.push_back( IntrospectionLink( true, *ci));
		}
		return rt;
	}

private:
	bool getValue( std::string& result, const std::string& name) const
	{
		strus::local_ptr<AttributeReaderInterface> areader( m_impl->createAttributeReader());
		if (!areader.get()) throw std::runtime_error( m_errorhnd->fetchError());
		areader->skipDoc( m_docno);
		return getValue( result, areader.get(), name);
	}
	bool getValue( std::string& result, AttributeReaderInterface* reader, const std::string& name) const
	{
		strus::Index elemhandle = reader->elementHandle( name.c_str());
		if (!elemhandle) return false;
		result = reader->getValue( elemhandle);
		return true;
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	strus::Index m_docno;
	std::string m_name;
};


class MetaDataIntrospection
	:public IntrospectionBase
{
public:
	MetaDataIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const strus::Index& docno_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docno(docno_)
	{}
	virtual ~MetaDataIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		serializeMembers( serialization, path);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		NumericVariant value;
		return getValue( value, name) ? createIntrospectionAtomic( m_errorhnd, value) : NULL;
	}

	virtual std::vector<IntrospectionLink> list()
	{
		std::vector<IntrospectionLink> rt;
		strus::local_ptr<MetaDataReaderInterface> areader( m_impl->createMetaDataReader());
		if (!areader.get()) throw std::runtime_error( m_errorhnd->fetchError());
		areader->skipDoc( m_docno);
		std::vector<std::string> candidates = areader->getNames();
		std::vector<std::string>::const_iterator ci = candidates.begin(), ce = candidates.end();
		for (; ci != ce; ++ci)
		{
			NumericVariant value;
			if (getValue( value, areader.get(), *ci)) rt.push_back( IntrospectionLink( true, *ci));
		}
		return rt;
	}

private:
	bool getValue( NumericVariant& result, const std::string& name) const
	{
		strus::local_ptr<MetaDataReaderInterface> areader( m_impl->createMetaDataReader());
		if (!areader.get()) throw std::runtime_error( m_errorhnd->fetchError());
		areader->skipDoc( m_docno);
		return getValue( result, areader.get(), name);
	}
	bool getValue( NumericVariant& result, MetaDataReaderInterface* reader, const std::string& name) const
	{
		strus::Index elemhandle = reader->elementHandle( name);
		if (elemhandle < 0) return false;
		result = reader->getValue( elemhandle);
		return true;
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	strus::Index m_docno;
	std::string m_name;
};


class ForwardIndexIntrospection
	:public IntrospectionBase
{
public:
	ForwardIndexIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const strus::Index& docno_,
			const std::string& type_ = std::string(),
			const strus::Index& pos_ = 0,
			const strus::Index& size_ = 0)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docno(docno_)
		,m_type(type_)
		,m_pos(pos_)
		,m_size(size_)
		{}
	virtual ~ForwardIndexIntrospection(){}

	enum {PartialDumpSize=1024};

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		if (m_type.empty())
		{
			std::vector<std::string> typelist = types();
			std::vector<std::string>::const_iterator ti = typelist.begin(), te = typelist.end();
			for (; ti != te; ++ti)
			{
				ForwardIndexIntrospection part( m_errorhnd, m_impl, m_docno, *ti);
				part.serializeStructureAs( serialization, ti->c_str(), path);
			}
		}
		else if (m_pos == 0)
		{
			Serializer::serialize( &serialization, dumpContent( m_type), true/*deep*/);
		}
		else if (m_size == 0)
		{
			Serializer::serialize( &serialization, dumpContent( m_type, m_pos, PartialDumpSize), true/*deep*/);
		}
		else
		{
			Serializer::serialize( &serialization, dumpContent( m_type, m_pos, m_size), true/*deep*/);
		}
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (m_type.empty())
		{
			return new ForwardIndexIntrospection( m_errorhnd, m_impl, m_docno, name);
		}
		else if (!m_pos)
		{
			strus::Index pos_ = parseIndex( name, _TXT("forward index position"));
			return new ForwardIndexIntrospection( m_errorhnd, m_impl, m_docno, m_type, pos_);
		}
		else
		{
			strus::Index len_ = parseIndex( name, _TXT("forward index length"));
			return new ForwardIndexIntrospection( m_errorhnd, m_impl, m_docno, m_type, m_pos, len_);
		}
		return NULL;
	}

	virtual std::vector<IntrospectionLink> list()
	{
		if (m_type.empty())
		{
			return IntrospectionLink::getList( false, types());
		}
		else
		{
			throw unresolvable_exception();
		}
	}

private:
	std::vector<std::string> types() const
	{
		std::vector<std::string> rt;
		strus::local_ptr<ValueIteratorInterface> valitr( m_impl->createTermTypeIterator());
		if (!valitr.get()) throw std::runtime_error( m_errorhnd->fetchError());
		std::vector<std::string> values = valitr->fetchValues( std::numeric_limits<short>::max());
		std::vector<std::string>::const_iterator vi = values.begin(), ve = values.end();
		for (; vi != ve; ++vi)
		{
			if (m_impl->isForwardIndexTerm( *vi))
			{
				rt.push_back( *vi);
			}
		}
		return rt;
	}
	std::string dumpContent( const std::string& type_, const strus::Index& startpos=0, const strus::Index& dumpsize=0) const
	{
		strus::local_ptr<ForwardIteratorInterface> itr( m_impl->createForwardIterator( type_));
		if (!itr.get()) throw std::runtime_error( m_errorhnd->fetchError());
		itr->skipDoc( m_docno);
		strus::Index pos = startpos, prevpos = startpos+1, endpos = dumpsize ? (startpos + dumpsize) : std::numeric_limits<strus::Index>::max();
		std::string content;
		while (!!(pos = itr->skipPos( pos+1)) && pos < endpos)
		{
			if (!content.empty())
			{
				content.push_back(' ');
			}
			else if (prevpos + 3 < pos)
			{
				content.append(" -- ");
			}
			else if (prevpos + 1 < pos)
			{
				content.push_back(' ');
			}
			content.append( itr->fetch());
			prevpos = pos;
		}
		return content;
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	strus::Index m_docno;
	std::string m_type;
	strus::Index m_pos;
	strus::Index m_size;
};


class DocumentPostingsIntrospection
	:public IntrospectionBase
{
public:
	DocumentPostingsIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const strus::Index& docno_,
			const std::string& type_,
			const std::string& value_,
			const strus::Index& pos_=0)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docno(docno_)
		,m_type(type_)
		,m_value(value_)
		,m_pos(pos_)
		{}
	virtual ~DocumentPostingsIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		strus::local_ptr<PostingIteratorInterface> pitr( m_impl->createTermPostingIterator( m_type, m_value, 1));
		if (!pitr.get()) throw std::runtime_error( m_errorhnd->fetchError());
		if (m_docno != pitr->skipDoc( m_docno)) return;
		if (m_pos)
		{
			strus::Index pos = pitr->skipPos( m_pos);
			if (pos)
			{
				Serializer::serialize( &serialization, (int64_t)pos, true/*deep*/);
			}
		}
		else
		{
			std::vector<strus::Index> posar;
			strus::Index pos = 0;
			while (!!(pos = pitr->skipPos( pos+1)))
			{
				posar.push_back( pos);
			}
			Serializer::serialize( &serialization, posar, true/*deep*/);
		}
	}
	virtual IntrospectionBase* open( const std::string& name)
	{
		if (!m_pos)
		{
			strus::Index pos_ = parseIndex( name, _TXT("search index position"));
			return new DocumentPostingsIntrospection( m_errorhnd, m_impl, m_docno, m_type, m_value, pos_);
		}
		else
		{
			throw unresolvable_exception();
		}
	}
	virtual std::vector<IntrospectionLink> list()
	{
		throw unresolvable_exception();
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	strus::Index m_docno;
	std::string m_type;
	std::string m_value;
	strus::Index m_pos;
};


class TermPostingsIntrospection
	:public IntrospectionBase
{
public:
	TermPostingsIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const std::string& type_,
			const std::string& value_,
			const std::string& docid_="",
			const strus::Index& docno_=0)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_areader(impl_->createAttributeReader())
		,m_postings(impl_->createTermPostingIterator( type_, value_, 1))
		,m_docid_handle(0)
		,m_type(type_)
		,m_value(value_)
		,m_docid(docid_)
		,m_docno(docno_)
	{
		if (!m_areader.get() || !m_postings.get()) throw std::runtime_error( m_errorhnd->fetchError());
		m_docid_handle = m_areader->elementHandle( Constants::attribute_docid());
	}
	virtual ~TermPostingsIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		Index dn;
		if (m_docno)
		{
			dn = m_docno -1;
		}
		else
		{
			if (!m_docid.empty())
			{
				strus::local_ptr<ValueIteratorInterface> vitr( m_impl->createDocIdIterator());
				if (!vitr.get()) throw std::runtime_error( m_errorhnd->fetchError());
				vitr->skipPrefix( m_docid.c_str(), m_docid.size());
				std::vector<std::string> values = vitr->fetchValues( 1);
				if (values.empty()) return;
				dn = m_impl->documentNumber( values[0]);
				if (!dn) return;
				--dn;
			}
			else
			{
				dn = 0;
			}
		}
		int cnt = MaxListSizeDeepExpansion;
		while (cnt-- && !!(dn=m_postings->skipDoc( dn+1)))
		{
			if (!papuga_Serialization_pushOpen( &serialization)) throw std::bad_alloc();
			if (m_docid_handle)
			{
				m_areader->skipDoc( dn);
				std::string docid = m_areader->getValue( m_docid_handle);
				if (docid.empty() && m_errorhnd->hasError()) throw std::runtime_error( m_errorhnd->fetchError());
				Serializer::serializeWithName( &serialization, "docid", docid, true/*deep*/);
			}
			Serializer::serializeWithName( &serialization, "docno", (int64_t)dn, true/*deep*/);
			Serializer::serializeWithName( &serialization, "frequency", (int64_t)m_postings->frequency(), true/*deep*/);
			Serializer::serializeWithName( &serialization, "pos", positions(), true/*deep*/);
			if (!papuga_Serialization_pushClose( &serialization)) throw std::bad_alloc();
		}
	}
	virtual IntrospectionBase* open( const std::string& name)
	{
		if (!m_docno)
		{
			strus::Index docno = 0;
			if (m_docid.empty() && name[0] == '_')
			{
				docno = parseIndex( name.c_str()+1, name.size()-1, _TXT("document number"));
				std::string docid = getDocidFromDocno( m_impl, docno, m_errorhnd);
				return new TermPostingsIntrospection( m_errorhnd, m_impl, m_type, m_value, docid, docno);
			}
			std::string docidprefix = m_docid.empty() ? name : (m_docid + '/' + name);
			docno = m_impl->documentNumber( docidprefix);
			if (docno)
			{
				return new TermPostingsIntrospection( m_errorhnd, m_impl, m_type, m_value, docidprefix, docno);
			}
			else if (isValidDocumentIdPrefix( m_impl, docidprefix, m_errorhnd))
			{
				return new TermPostingsIntrospection( m_errorhnd, m_impl, m_type, m_value, docidprefix);
			}
			else
			{
				throw unresolvable_exception();
			}
		}
		else if (name == "pos")
		{
			m_postings->skipDoc( m_docno);
			return createIntrospectionAtomic( m_errorhnd, positions());
		}
		else if (name == "frequency")
		{
			m_postings->skipDoc( m_docno);
			return createIntrospectionAtomic( m_errorhnd, (int64_t)m_postings->frequency());
		}
		else if (name == "docid")
		{
			m_areader->skipDoc( m_docno);
			return createIntrospectionAtomic( m_errorhnd, m_areader->getValue( m_docid_handle));
		}
		else if (name == "docno")
		{
			return createIntrospectionAtomic( m_errorhnd, (int64_t)m_docno);
		}
		else
		{
			throw unresolvable_exception();
		}
	}
	virtual std::vector<IntrospectionLink> list()
	{
		if (m_docno)
		{
			static const char* ar[] = {"docid","docno","frequency","pos",NULL};
			return getList( ar);
		}
		else
		{
			throw unresolvable_exception();
		}
	}

private:
	std::vector<strus::Index> positions( strus::Index pos_=0)
	{
		std::vector<strus::Index> rt;
		while (!!(pos_=m_postings->skipPos(pos_+1)))
		{
			rt.push_back( pos_);
		}
		return rt;
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	strus::Reference<AttributeReaderInterface> m_areader;
	strus::Reference<PostingIteratorInterface> m_postings;
	strus::Index m_docid_handle;
	std::string m_type;
	std::string m_value;
	std::string m_docid;
	strus::Index m_docno;
	strus::Index m_pos;
};


class SearchIndexIntrospection
	:public IntrospectionBase
{
public:
	SearchIndexIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const strus::Index& docno_,
			const std::string& type_ = std::string())
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docno(docno_)
		,m_type(type_)
		{}
	virtual ~SearchIndexIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		if (m_type.empty())
		{
			std::vector<std::string> typelist = types();
			std::vector<std::string>::const_iterator ti = typelist.begin(), te = typelist.end();
			for (; ti != te; ++ti)
			{
				SearchIndexIntrospection part( m_errorhnd, m_impl, m_docno, *ti);
				part.serializeStructureAs( serialization, ti->c_str(), path);
			}
		}
		else
		{
			strus::local_ptr<DocumentTermIteratorInterface> itr( m_impl->createDocumentTermIterator( m_type));
			if (!itr.get()) throw std::runtime_error( m_errorhnd->fetchError());
			if (m_docno != itr->skipDoc( m_docno)) return;
			Serializer::serialize( &serialization, itr.get(), true/*deep*/);
		}
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (m_type.empty())
		{
			return new SearchIndexIntrospection( m_errorhnd, m_impl, m_docno, name);
		}
		else
		{
			return new DocumentPostingsIntrospection( m_errorhnd, m_impl, m_docno, m_type, name);
		}
		return NULL;
	}

	virtual std::vector<IntrospectionLink> list()
	{
		if (m_type.empty())
		{
			return IntrospectionLink::getList( false, types());
		}
		else
		{
			throw unresolvable_exception();
		}
	}

private:
	std::vector<std::string> types() const
	{
		std::vector<std::string> rt;
		strus::local_ptr<ValueIteratorInterface> valitr( m_impl->createTermTypeIterator());
		if (!valitr.get()) throw std::runtime_error( m_errorhnd->fetchError());
		std::vector<std::string> valar = valitr->fetchValues( std::numeric_limits<short>::max());
		std::vector<std::string>::const_iterator vi = valar.begin(), ve = valar.end();
		for (; vi != ve; ++vi)
		{
			strus::local_ptr<DocumentTermIteratorInterface> itr( m_impl->createDocumentTermIterator( *vi));
			if (!itr.get()) throw std::runtime_error( m_errorhnd->fetchError());
			if (m_docno == itr->skipDoc( m_docno))
			{
				rt.push_back( *vi);
			}
		}
		return rt;
	}
	std::vector<std::string> values( const std::string& type_) const
	{
		std::vector<std::string> rt;
		strus::local_ptr<DocumentTermIteratorInterface> itr( m_impl->createDocumentTermIterator( type_));
		if (!itr.get()) throw std::runtime_error( m_errorhnd->fetchError());
		if (m_docno != itr->skipDoc( m_docno)) return std::vector<std::string>();
		DocumentTermIteratorInterface::Term term;
		bool sc = true;
		while (sc && itr->nextTerm( term))
		{
			rt.push_back( itr->termValue( term.termno));
		}
		if (!sc) throw std::bad_alloc();
		return rt;
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	strus::Index m_docno;
	std::string m_type;
};


class TermIntrospection
	:public IntrospectionBase
{
public:
	TermIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const std::string& type_="",
			const std::string& value_="")
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_type(type_)
		,m_value(value_)
		{}
	virtual ~TermIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		serializeMembers( serialization, path);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (m_type.empty())
		{
			return new TermIntrospection( m_errorhnd, m_impl, name);
		}
		else if (m_value.empty())
		{
			return new TermIntrospection( m_errorhnd, m_impl, m_type, name);
		}
		else if (name == "df")
		{
			return createIntrospectionAtomic( m_errorhnd, (int64_t)m_impl->documentFrequency( m_type, m_value));
		}
		else if (name == "postings")
		{
			return new TermPostingsIntrospection( m_errorhnd, m_impl, m_type, m_value);
		}
		else
		{
			throw unresolvable_exception();
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		if (m_type.empty())
		{
			IntrospectionValueIterator vitr( m_errorhnd, m_impl->createTermTypeIterator(), false/*prefixBound*/);
			return vitr.list();
		}
		else if (m_value.empty())
		{
			strus::local_ptr<DocumentTermIteratorInterface> itr( m_impl->createDocumentTermIterator( m_type));
			if (!itr.get()) throw std::runtime_error( m_errorhnd->fetchError());
			strus::Index docno = 0;
			std::vector<IntrospectionLink> rt;
			std::set<std::string> valueset;
			while (!!(docno=itr->skipDoc(docno+1)) && rt.size() < MaxListSizeDeepExpansion)
			{
				DocumentTermIteratorInterface::Term term;
				while (itr->nextTerm( term) && rt.size() < MaxListSizeDeepExpansion)
				{
					std::string value = itr->termValue( term.termno);
					if (valueset.insert( value).second)
					{
						rt.push_back( IntrospectionLink( false/*autoexpand*/, value));
					}
				}
			}
			return rt;
		}
		else
		{
			static const char* ar[] = {"df","postings",0};
			return getList( ar);
		}
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
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
			const std::string& docid_,
			const strus::Index& docno_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docid(docid_)
		,m_docno(docno_)
		{}
	virtual ~DocumentIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		serializeMembers( serialization, path);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		if (name == "docno")
		{
			return createIntrospectionAtomic( m_errorhnd, (int64_t)m_docno);
		}
		else if (name == "attribute")
		{
			return new AttibuteIntrospection( m_errorhnd, m_impl, m_docno);
		}
		else if (name == "metadata")
		{
			return new MetaDataIntrospection( m_errorhnd, m_impl, m_docno);
		}
		else if (name == "sindex")
		{
			return new SearchIndexIntrospection( m_errorhnd, m_impl, m_docno);
		}
		else if (name == "findex")
		{
			return new ForwardIndexIntrospection( m_errorhnd, m_impl, m_docno);
		}
		return NULL;
	}

	virtual std::vector<IntrospectionLink> list()
	{
		static const char* ar[] = {"docno","attribute","metadata","findex","sindex", NULL};
		return getList( ar);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	std::string m_docid;
	strus::Index m_docno;
};


class DocidIntrospection
	:public IntrospectionBase
{
public:
	DocidIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			const std::string& path_=std::string(),
			bool complete_=true)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_path(path_)
		,m_complete(complete_)
		{}
	virtual ~DocidIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path)
	{
		serializeMembers( serialization, path);
	}

	virtual IntrospectionBase* open( const std::string& name)
	{
		strus::Index docno = 0;
		if (name.empty()) return NULL;

		std::string docidprefix = m_path.empty() ? name : (m_path + '/' + name);
		if (m_path.empty() && name[0] == '_')
		{
			docno = parseIndex( name.c_str()+1, name.size()-1, _TXT("document number"));
			std::string docid = getDocidFromDocno( m_impl, docno, m_errorhnd);
			return new DocumentIntrospection( m_errorhnd, m_impl, docid, docno);
		}
		else if (!docidprefix.empty() && docidprefix[ docidprefix.size()-1] == '*')
		{
			docidprefix.resize( docidprefix.size()-1);
			if (isValidDocumentIdPrefix( m_impl, docidprefix, m_errorhnd))
			{
				return new DocidIntrospection( m_errorhnd, m_impl, docidprefix, false/*complete*/);
			}
			else
			{
				throw unresolvable_exception();
			}
		}
		else
		{
			docno = m_impl->documentNumber( docidprefix);
			if (docno)
			{
				return new DocumentIntrospection( m_errorhnd, m_impl, docidprefix, docno);
			}
			else if (isValidDocumentIdPrefix( m_impl, docidprefix+"/", m_errorhnd))
			{
				return new DocidIntrospection( m_errorhnd, m_impl, docidprefix, true/*complete*/);
			}
			else
			{
				throw unresolvable_exception();
			}
		}
	}

	virtual std::vector<IntrospectionLink> list()
	{
		strus::local_ptr<IntrospectionBase> values( createIntrospectionDirectoryIterator( m_errorhnd, m_impl->createDocIdIterator(), m_path, m_complete));
		return values->list();
	}

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
	std::string m_path;
	bool m_complete;
};
}//namespace


void StorageIntrospection::serialize( papuga_Serialization& serialization, const std::string& path)
{
	serializeMembers( serialization, path);
}

IntrospectionBase* StorageIntrospection::open( const std::string& name)
{
	if (name == "config") return new IntrospectionConfig<StorageClientInterface>( m_errorhnd, m_impl);
	else if (name == "nofdocs") return createIntrospectionAtomic( m_errorhnd, (int64_t) m_impl->nofDocumentsInserted());
	else if (name == "maxdocno") return createIntrospectionAtomic( m_errorhnd, (int64_t) m_impl->maxDocumentNumber());
	else if (name == "user") return createIntrospectionValueIterator( m_errorhnd, m_impl->createUserNameIterator(), false/*prefixBound*/);
	else if (name == "termtype") return createIntrospectionValueIterator( m_errorhnd, m_impl->createTermTypeIterator(), false/*prefixBound*/);
	else if (name == "termvalue") return createIntrospectionValueIterator( m_errorhnd, m_impl->createTermValueIterator(), true/*prefixBound*/);
	else if (name == "attribute")
	{
		strus::local_ptr<AttributeReaderInterface> reader( m_impl->createAttributeReader());
		if (!reader.get()) throw std::runtime_error( m_errorhnd->fetchError());
		return createIntrospectionAtomic( m_errorhnd, reader->getNames());
	}
	else if (name == "term") return new TermIntrospection( m_errorhnd, m_impl);
	else if (name == "doc")
	{
		return new DocidIntrospection( m_errorhnd, m_impl);
	}
	return NULL;
}

std::vector<IntrospectionLink> StorageIntrospection::list()
{
	static const char* ar[] = {"config","nofdocs","maxdocno","user","termtype",".termvalue",".term",".doc",NULL};
	return getList( ar);
}




