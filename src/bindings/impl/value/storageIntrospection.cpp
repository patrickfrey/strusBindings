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
#include "strus/base/configParser.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/forwardIteratorInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
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


namespace {
class AttibuteIntrospection
	:public IntrospectionBase
{
public:
	AttibuteIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_,
			strus::Index docno_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_->createAttributeReader())
		,m_docno(docno_)
	{
		if (!m_impl) throw std::runtime_error( m_errorhnd->fetchError());
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
		strus::Index elemhandle = m_impl->elementHandle( name.c_str());
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
			strus::Index elemhandle = m_impl->elementHandle( ci->c_str());
			if (!elemhandle) return std::vector<std::string>();
			std::string value = m_impl->getValue( elemhandle);
			if (!value.empty()) rt.push_back( *ci);
		}
		return rt;
	}

	static IntrospectionBase* createSingle( ErrorBufferInterface* errorhnd_, const StorageClientInterface* sto, const std::string& name)
	{
		strus::local_ptr<AttributeReaderInterface> areader( sto->createAttributeReader());
		if (!areader.get()) throw std::runtime_error( errorhnd_->fetchError());
		Index elemhandle = areader->elementHandle( name.c_str());
		if (!elemhandle) return NULL;
		std::string value = areader->getValue( elemhandle);
		if (value.empty()) return NULL;
		return createIntrospectionAtomic( errorhnd_, value);
	}

private:
	ErrorBufferInterface* m_errorhnd;
	AttributeReaderInterface* m_impl;
	strus::Index m_docno;
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

	enum {PartialDumpSize=1024};

	virtual void serialize( papuga_Serialization& serialization) const
	{
		if (m_type.empty())
		{
			std::vector<std::string> typelist = types();
			std::vector<std::string>::const_iterator ti = typelist.begin(), te = typelist.end();
			for (; ti != te; ++ti)
			{
				ForwardIndexIntrospection part( m_errorhnd, m_impl, m_docno, *ti);
				part.serializeStructureAs( serialization, ti->c_str());
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

	virtual IntrospectionBase* open( const std::string& name) const
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

	virtual std::vector<std::string> list( bool all) const
	{
		if (m_type.empty())
		{
			return types();
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


class PostingsIntrospection
	:public IntrospectionBase
{
public:
	PostingsIntrospection(
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

	virtual void serialize( papuga_Serialization& serialization) const
	{
		strus::local_ptr<PostingIteratorInterface> pitr( m_impl->createTermPostingIterator( m_type, m_value, 1));
		if (!pitr.get()) throw std::runtime_error( m_errorhnd->fetchError());
		if (m_docno != pitr->skipDoc( m_docno)) return;
		if (m_pos)
		{
			strus::Index pos = 0;
			pos = pitr->skipDoc( pos+1);
			if (pos)
			{
				Serializer::serialize( &serialization, (int64_t)pos, true/*deep*/);
			}
		}
		else
		{
			std::vector<strus::Index> posar;
			strus::Index pos = 0;
			while (!!(pos = pitr->skipDoc( pos+1)))
			{
				posar.push_back( pos);
			}
			Serializer::serialize( &serialization, posar, true/*deep*/);
		}
	}
	virtual IntrospectionBase* open( const std::string& name) const
	{
		if (!m_pos)
		{
			strus::Index pos_ = parseIndex( name, _TXT("search index position"));
			return new PostingsIntrospection( m_errorhnd, m_impl, m_docno, m_type, m_value, pos_);
		}
		else
		{
			throw unresolvable_exception();
		}
	}
	virtual std::vector<std::string> list( bool all) const
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

	virtual void serialize( papuga_Serialization& serialization) const
	{
		if (m_type.empty())
		{
			std::vector<std::string> typelist = types();
			std::vector<std::string>::const_iterator ti = typelist.begin(), te = typelist.end();
			for (; ti != te; ++ti)
			{
				SearchIndexIntrospection part( m_errorhnd, m_impl, m_docno, *ti);
				part.serializeStructureAs( serialization, ti->c_str());
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

	virtual IntrospectionBase* open( const std::string& name) const
	{
		if (m_type.empty())
		{
			return new SearchIndexIntrospection( m_errorhnd, m_impl, m_docno, name);
		}
		else
		{
			return new PostingsIntrospection( m_errorhnd, m_impl, m_docno, m_type, name);
		}
		return NULL;
	}

	virtual std::vector<std::string> list( bool all) const
	{
		if (m_type.empty())
		{
			return types();
		}
		else
		{
			return values( m_type);
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

	virtual void serialize( papuga_Serialization& serialization) const
	{
		serializeList( serialization);
	}

	virtual IntrospectionBase* open( const std::string& name) const
	{
		if (name == "docid")
		{
			return createIntrospectionAtomic( m_errorhnd, m_docid);
		}
		else if (name == "docno")
		{
			return createIntrospectionAtomic( m_errorhnd, (int64_t)m_docno);
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

	virtual std::vector<std::string> list( bool all) const
	{
		static const char* ar[] = {"docid","findex","sindex"};
		return getList( ar, all);
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
			const std::string& docid_="")
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_docid(docid_)
		{}

	virtual void serialize( papuga_Serialization& serialization) const
	{
		std::vector<std::string> elems = list( true/*all*/);
		std::vector<std::string>::const_iterator ei = elems.begin(), ee = elems.end();
		for (; ei != ee; ++ei)
		{
			docno = m_impl->documentNumber( *ei);
			if (!docno) throw unresolvable_exception();
			DocumentIntrospection part( m_errorhnd, m_impl, *ei, docno);
			part.serializeStructureAs( serialization, li->c_str());
		}
	}

	virtual IntrospectionBase* open( const std::string& name) const
	{
		strus::Index docno = 0;
		if (m_docid.empty() && name[0] == '_')
		{
			docno = parseIndex( name.c_str()+1, name.size()-1, _TXT("document number"));
			std::string docid_ = getDocidFromDocno( docno);
			return new DocumentIntrospection( m_errorhnd, m_impl, docid_, docno);
		}
		std::string docidprefix = m_docid.empty() ? name : (m_docid + '/' + name);
		docno = m_impl->documentNumber( docidprefix);
		if (docno)
		{
			return new DocumentIntrospection( m_errorhnd, m_impl, docidprefix, docno);
		}
		else if (!isEmptyList( docidprefix))
		{
			return new DocidIntrospection( m_errorhnd, m_impl, docidprefix);
		}
		else
		{
			throw unresolvable_exception();
		}
	}

	virtual std::vector<std::string> list( bool all) const
	{
		strus::local_ptr<IntrospectionBase> values( createIntrospectionValueIterator( m_errorhnd, m_impl->createDocIdIterator(), m_docid));
		if (!values.get()) throw std::runtime_error( m_errorhnd->fetchError());
		return values->list( all);
	}

private:
	std::string getDocidFromDocno( const strus::Index& docno) const
	{
		strus::local_ptr<AttributeReaderInterface> areader( m_impl->createAttributeReader());
		if (!areader.get()) throw std::runtime_error( m_errorhnd->fetchError());
		Index elemhandle = areader->elementHandle( Constants::attribute_docid());
		if (!elemhandle) return std::string();
		return areader->getValue( elemhandle);
	}
	bool isEmptyList( const std::string& prefix) const
	{
		strus::local_ptr<ValueIteratorInterface> vitr( m_impl->createDocIdIterator());
		vitr->skip( prefix.c_str(), prefix.size());
		return vitr->fetchValues( 1).empty();
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
		strus::local_ptr<MetaDataReaderInterface> reader( m_impl->createMetaDataReader());
		if (!reader.get()) throw std::runtime_error( m_errorhnd->fetchError());
		return createIntrospectionAtomic( m_errorhnd, reader->getNames());
	}
	else if (name == "attributename")
	{
		strus::local_ptr<AttributeReaderInterface> reader( m_impl->createAttributeReader());
		if (!reader.get()) throw std::runtime_error( m_errorhnd->fetchError());
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




