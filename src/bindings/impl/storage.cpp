/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/storage.hpp"
#include "impl/struct.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageTransactionInterface.hpp"
#include "strus/storageDocumentInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/metaDataRestrictionInstanceInterface.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/base/configParser.hpp"
#include "papuga/serialization.hpp"
#include "serializer.hpp"
#include "valueVariantWrap.hpp"
#include "internationalization.hpp"
#include "metadataop.hpp"
#include "expressionBuilder.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

StorageClientImpl::StorageClientImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_storage_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();

	m_storage_impl.resetOwnership( createStorageClient( objBuilder, errorhnd, config_), "StorageClient");
	if (!m_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create storage client: %s"), errorhnd->fetchError());
	}
}

unsigned int StorageClientImpl::nofDocumentsInserted() const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->nofDocumentsInserted();
}

Index StorageClientImpl::documentFrequency( const std::string& type, const std::string& term) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->documentFrequency( type, term);
}

Index StorageClientImpl::documentNumber( const std::string& docid) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->documentNumber( docid);
}

class PostingIterator
{
public:
	PostingIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const ValueVariant& expression, const ValueVariant& restriction, const Index& docno_)
		:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_errorhnd_impl(errorhnd_),m_postings(),m_restriction(),m_docno(docno_?docno_:1)
	{
		const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
		const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
		const QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (!papuga_ValueVariant_defined( &expression))
		{
			throw strus::runtime_error(_TXT("null passed as expression to posting iterator"));
		}
		PostingsExpressionBuilder postingsBuilder( storage, queryproc, errorhnd);
		Deserializer::buildExpression( postingsBuilder, expression, errorhnd);
		m_postings = postingsBuilder.pop();
		if (papuga_ValueVariant_defined( &restriction))
		{
			Reference<MetaDataRestrictionInterface> builder;
			builder.reset( storage->createMetaDataRestriction());
			if (!builder.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for posting iterator"));
			Deserializer::buildMetaDataRestriction( builder.get(), restriction, errorhnd);
			m_restriction.reset( builder->createInstance());
			if (!m_restriction.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for posting iterator instance"));
		}
	}
	virtual ~PostingIterator(){}

	bool getNext( papuga_CallResult* result)
	{
		try
		{
			if (!m_docno) return false;
			for (; 0!=(m_docno = m_postings->skipDoc( m_docno)); ++m_docno)
			{
				if (!m_restriction.get() || m_restriction->match( m_docno)) break;
			}
			if (m_docno)
			{
				bool ser = true;
				if (!papuga_set_CallResult_serialization( result)) throw std::bad_alloc();
				papuga_Serialization* serialization = result->value.value.serialization;
				ser &= papuga_Serialization_pushOpen( serialization);
				ser &= papuga_Serialization_pushValue_int( serialization, m_docno++);
				ser &= papuga_Serialization_pushClose( serialization);
				ser &= papuga_Serialization_pushOpen( serialization);
				for (Index pos = 0; 0!=(pos=m_postings->skipPos(pos)); ++pos)
				{
					ser &= papuga_Serialization_pushValue_int( serialization, pos);
				}
				ser &= papuga_Serialization_pushClose( serialization);
				ser &= papuga_Serialization_pushClose( serialization);
				if (!ser)
				{
					papuga_CallResult_reportError( result, _TXT("memory allocation error in postings iterator get next"));
					return false;
				}
				return true;
			}
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			if (errorhnd->hasError())
			{
				papuga_CallResult_reportError( result, _TXT("error in postings iterator get next: %s"), errorhnd->fetchError());
			}
			return false;
		}
		catch (const std::bad_alloc& err)
		{
			papuga_CallResult_reportError( result, _TXT("memory allocation error in postings iterator get next"));
			return false;
		}
		catch (const std::runtime_error& err)
		{
			papuga_CallResult_reportError( result, _TXT("error in postings iterator get next: %s"), err.what());
			return false;
		}
	}

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_errorhnd_impl;
	Reference<PostingIteratorInterface> m_postings;
	Reference<MetaDataRestrictionInstanceInterface> m_restriction;
	Index m_docno;
};

static bool PostingsGetNext( void* self, papuga_CallResult* result)
{
	return ((PostingIterator*)self)->getNext( result);
}

static void PostingsDeleter( void* obj)
{
	delete (PostingIterator*)obj;
}

class SelectIterator
{
public:
	SelectIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const ValueVariant& what, const ValueVariant& expression, const ValueVariant& restriction, const Index& docno_)
		:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_attributereader_impl(),m_metadatareader_impl(),m_errorhnd_impl(errorhnd_),m_postings(),m_restriction(),m_docno(docno_?docno_:1),m_maxdocno(0),m_items()
	{
		const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
		const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
		const QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

		m_attributereader_impl.resetOwnership( storage->createAttributeReader(), "AttributeReader");
		if (!m_attributereader_impl.get()) throw strus::runtime_error( _TXT("failed to create attribute reader"));
		m_metadatareader_impl.resetOwnership( storage->createMetaDataReader(), "MetaDataReader");
		if (!m_metadatareader_impl.get()) throw strus::runtime_error( _TXT("failed to create metadata reader"));

		AttributeReaderInterface* attributereader = m_attributereader_impl.getObject<AttributeReaderInterface>();
		MetaDataReaderInterface* metadatareader = m_metadatareader_impl.getObject<MetaDataReaderInterface>();
		m_maxdocno = storage->maxDocumentNumber();

		if (papuga_ValueVariant_defined( &expression))
		{
			PostingsExpressionBuilder postingsBuilder( storage, queryproc, errorhnd);
			Deserializer::buildExpression( postingsBuilder, expression, errorhnd);
			m_postings = postingsBuilder.pop();
		}
		if (papuga_ValueVariant_defined( &restriction))
		{
			Reference<MetaDataRestrictionInterface> builder;
			builder.reset( storage->createMetaDataRestriction());
			if (!builder.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for posting iterator"));
			Deserializer::buildMetaDataRestriction( builder.get(), restriction, errorhnd);
			if (!m_postings.get())
			{
				m_postings.reset( storage->createBrowsePostingIterator( builder.get(), 1));
			}
			else
			{
				m_restriction.reset( builder->createInstance());
				if (!m_restriction.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for posting iterator instance"));
			}
		}
		std::vector<std::string> elemlist = Deserializer::getStringList( what);
		std::vector<std::string>::const_iterator ei = elemlist.begin(), ee = elemlist.end();
		for (; ei != ee; ++ei)
		{
			Index eh;
			if (utils::caseInsensitiveEquals( *ei, "position"))
			{
				m_items.push_back( ItemDef( *ei, ItemDef::Position));
			}
			else if (utils::caseInsensitiveEquals( *ei, "docno"))
			{
				m_items.push_back( ItemDef( *ei, ItemDef::Docno));
			}
			else if (0 <= (eh = metadatareader->elementHandle( ei->c_str())))
			{
				m_items.push_back( ItemDef( *ei, ItemDef::MetaData, eh));
			}
			else if (1 <= (eh = attributereader->elementHandle( ei->c_str())))
			{
				m_items.push_back( ItemDef( *ei, ItemDef::Attribute, eh));
			}
			else
			{
				m_items.push_back( ItemDef( *ei, ItemDef::None));
			}
		}
		
	}
	virtual ~SelectIterator(){}

	bool buildRow( papuga_CallResult* result)
	{
		AttributeReaderInterface* attributereader = 0;
		MetaDataReaderInterface* metadatareader = 0;

		bool ser = true;
		if (!papuga_set_CallResult_serialization( result)) throw std::bad_alloc();
		papuga_Serialization* serialization = result->value.value.serialization;
		if (m_items.empty())
		{
			ser &= papuga_Serialization_pushValue_int( serialization, m_docno);
		}
		else
		{
			ser &= papuga_Serialization_pushOpen( serialization);
			std::vector<ItemDef>::const_iterator ei = m_items.begin(), ee = m_items.end();
			for (; ei != ee; ++ei)
			{
				ser &= papuga_Serialization_pushName_string( serialization, ei->name().c_str(), ei->name().size());
				switch (ei->type())
				{
					case ItemDef::None:
						ser &= papuga_Serialization_pushValue_void( serialization);
						break;

					case ItemDef::MetaData:
						if (!metadatareader)
						{
							metadatareader = m_metadatareader_impl.getObject<MetaDataReaderInterface>();
							metadatareader->skipDoc( m_docno);
						}
						if (ei->handle() >= 0)
						{
							ser &= Serializer::serialize_nothrow( serialization, metadatareader->getValue( ei->handle()));
							attributereader = m_attributereader_impl.getObject<AttributeReaderInterface>();
						}
						else
						{
							ser &= papuga_Serialization_pushValue_void( serialization);
						}
						break;
					case ItemDef::Attribute:
						if (ei->handle() > 0)
						{
							if (!attributereader)
							{
								attributereader = m_attributereader_impl.getObject<AttributeReaderInterface>();
								attributereader->skipDoc( m_docno);
							}
							std::string attrvalstr = attributereader->getValue( ei->handle());
							const char* attrvalptr = papuga_Allocator_copy_string( &result->allocator, attrvalstr.c_str(), attrvalstr.size());
							if (!attrvalptr) throw std::bad_alloc();

							ser &= papuga_Serialization_pushValue_string( serialization, attrvalptr, attrvalstr.size());
						}
						else
						{
							ser &= papuga_Serialization_pushValue_void( serialization);
						}
						break;
					case ItemDef::Position:
						if (m_postings.get())
						{
							ser &= papuga_Serialization_pushOpen( serialization);
							for (Index pos = 0; 0!=(pos=m_postings->skipPos(pos)); ++pos)
							{
								ser &= papuga_Serialization_pushValue_int( serialization, pos);
							}
							ser &= papuga_Serialization_pushClose( serialization);
						}
						else
						{
							ser &= papuga_Serialization_pushValue_void( serialization);
						}
						break;
					case ItemDef::Docno:
						ser &= papuga_Serialization_pushValue_int( serialization, m_docno);
						break;
				}
			}
			ser &= papuga_Serialization_pushClose( serialization);
		}
		if (!ser)
		{
			papuga_CallResult_reportError( result, _TXT("memory allocation error in postings iterator get next"));
			return false;
		}
		return true;
	}

	bool getNext( papuga_CallResult* result)
	{
		try
		{
			if (m_postings.get())
			{
				if (!m_docno) return false;
				for (; 0!=(m_docno = m_postings->skipDoc( m_docno)); ++m_docno)
				{
					if (!m_restriction.get() || m_restriction->match( m_docno)) break;
				}
			}
			else
			{
				if (!m_docno || m_docno > m_maxdocno) return false;
			}
			bool rt = false;
			if (m_docno)
			{
				rt = buildRow( result);
				++m_docno;
			}
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			if (errorhnd->hasError())
			{
				papuga_CallResult_reportError( result, _TXT("error in postings iterator get next: %s"), errorhnd->fetchError());
				rt = false;
			}
			return rt;
		}
		catch (const std::bad_alloc& err)
		{
			papuga_CallResult_reportError( result, _TXT("memory allocation error in postings iterator get next"));
			return false;
		}
		catch (const std::runtime_error& err)
		{
			papuga_CallResult_reportError( result, _TXT("error in postings iterator get next: %s"), err.what());
			return false;
		}
	}

private:
	class ItemDef
	{
	public:
		enum Type {
			None,
			MetaData,
			Attribute,
			Position,
			Docno
		};

		ItemDef( const std::string& name_, const Type& type_, const Index& handle_=-1)
			:m_name(name_),m_type(type_),m_handle(handle_){}
		ItemDef( const ItemDef& o)
			:m_name(o.m_name),m_type(o.m_type),m_handle(o.m_handle){}

		const std::string& name() const		{return m_name;}
		const Type& type() const		{return m_type;}
		const Index& handle() const		{return m_handle;}

	private:
		std::string m_name;
		Type m_type;
		Index m_handle;
	};

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_attributereader_impl;
	ObjectRef m_metadatareader_impl;
	ObjectRef m_errorhnd_impl;
	Reference<PostingIteratorInterface> m_postings;
	Reference<MetaDataRestrictionInstanceInterface> m_restriction;
	Index m_docno;
	Index m_maxdocno;
	std::vector<ItemDef> m_items;
};

static bool SelectGetNext( void* self, papuga_CallResult* result)
{
	return ((SelectIterator*)self)->getNext( result);
}

static void SelectDeleter( void* obj)
{
	delete (SelectIterator*)obj;
}

class ValueIterator
{
public:
	enum {MaxNofElements=128};

	ValueIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& values_, const ObjectRef& errorhnd_, const std::string& key=std::string())
		:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_values(values_),m_errorhnd_impl(errorhnd_)
	{
		ValueIteratorInterface* THIS = m_values.getObject<ValueIteratorInterface>();
		if (!key.empty())
		{
			THIS->skip( key.c_str(), key.size());
		}
		m_block = THIS->fetchValues( MaxNofElements);
		m_blockitr = m_block.begin();
	}
	virtual ~ValueIterator()
	{}

	bool getNext( papuga_CallResult* result)
	{
		ValueIteratorInterface* THIS = m_values.getObject<ValueIteratorInterface>();
		if (m_blockitr == m_block.end())
		{
			if (m_block.empty()) return false;
			m_block = THIS->fetchValues( MaxNofElements);
			m_blockitr = m_block.begin();
			if (m_blockitr == m_block.end())
			{
				ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
				if (errorhnd->hasError()) throw strus::runtime_error(_TXT("error in posting iterator: %s"), errorhnd->fetchError());
				return false;
			}
		}
		if (!papuga_set_CallResult_string( result, m_blockitr->c_str(), m_blockitr->size())) throw std::bad_alloc();
		++m_blockitr;
		return true;
	}

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_values;
	ObjectRef m_errorhnd_impl;
	std::vector<std::string> m_block;
	std::vector<std::string>::const_iterator m_blockitr;
};


static bool ValueIteratorGetNext( void* self, papuga_CallResult* result)
{
	try
	{
		return (((ValueIterator*)self)->getNext( result));
	}
	catch (const std::bad_alloc& err)
	{
		papuga_CallResult_reportError( result, _TXT("memory allocation error in value iterator get next"));
		return false;
	}
	catch (const std::runtime_error& err)
	{
		papuga_CallResult_reportError( result, _TXT("error in value iterator get next: %s"), err.what());
		return false;
	}
}

static void ValueIteratorDeleter( void* obj)
{
	delete (ValueIterator*)obj;
}

class StatisticsIterator
{
public:
	StatisticsIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, bool differential, bool sign)
		:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_errorhnd_impl(errorhnd_)
	{
		StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (differential)
		{
			if (!sign) throw strus::runtime_error(_TXT("internal: illegal definition of statistics iterator"));
			m_iter.reset( storage->createChangeStatisticsIterator());
		}
		else
		{
			m_iter.reset( storage->createAllStatisticsIterator( sign));
		}
		if (!m_iter.get())
		{
			throw strus::runtime_error(_TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
		}
	}
	virtual ~StatisticsIterator(){}

	bool getNext( papuga_CallResult* result)
	{
		try
		{
			StatisticsIteratorInterface* iter = m_iter.get();
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			const char* outmsg;
			std::size_t outmsgsize;
			if (!iter->getNext( outmsg, outmsgsize))
			{
				if (errorhnd->hasError())
				{
					throw strus::runtime_error( _TXT("error fetching statistics message: %s"), errorhnd->fetchError());
				}
				return false;
			}
			if (!papuga_set_CallResult_string( result, outmsg, outmsgsize)) throw std::bad_alloc();
			return true;
		}
		catch (const std::bad_alloc& err)
		{
			papuga_CallResult_reportError( result, _TXT("memory allocation error in postings iterator get next"));
			return false;
		}
		catch (const std::runtime_error& err)
		{
			papuga_CallResult_reportError( result, _TXT("error in postings iterator get next: %s"), err.what());
			return false;
		}
	}

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_errorhnd_impl;
	Reference<StatisticsIteratorInterface> m_iter;
};

static bool StatisticsGetNext( void* self, papuga_CallResult* result)
{
	return ((StatisticsIterator*)self)->getNext( result);
}

static void StatisticsDeleter( void* obj)
{
	delete (StatisticsIterator*)obj;
}

Iterator StorageClientImpl::postings( const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno)
{
	Reference<PostingIterator> itr( new PostingIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, expression, restriction, start_docno));
	Iterator rt( itr.get(), &PostingsDeleter, &PostingsGetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::select( const ValueVariant& what, const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno)
{
	Reference<SelectIterator> itr( new SelectIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, what, expression, restriction, start_docno));
	Iterator rt( itr.get(), &SelectDeleter, &SelectGetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::termTypes() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createTermTypeIterator(), "TermTypeIterator"));
	if (!itr.get()) throw strus::runtime_error(_TXT("error creating term type iterator: %s"), errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIteratorDeleter, &ValueIteratorGetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::docids() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createDocIdIterator(), "DocIdIterator"));
	if (!itr.get()) throw strus::runtime_error(_TXT("error creating %s iterator: %s"), "docid", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIteratorDeleter, &ValueIteratorGetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::usernames() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createUserNameIterator(), "UserNameIterator"));
	if (!itr.get()) throw strus::runtime_error(_TXT("error creating %s iterator: %s"), "username", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIteratorDeleter, &ValueIteratorGetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::getAllStatistics( bool sign)
{
	Iterator rt( new StatisticsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, false, sign), &StatisticsDeleter, &StatisticsGetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::getChangeStatistics()
{
	Iterator rt( new StatisticsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, true, true), &StatisticsDeleter, &StatisticsGetNext);
	rt.release();
	return rt;
}

StorageTransactionImpl* StorageClientImpl::createTransaction() const
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return new StorageTransactionImpl( m_trace_impl, m_objbuilder_impl, m_errorhnd_impl, m_storage_impl);
}

void StorageClientImpl::close()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	storage->close();
	m_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage client close: %s"), errorhnd->fetchError());
	}
}

std::string StorageClientImpl::configstring() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return storage->config();
}

Struct StorageClientImpl::config() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	typedef std::vector<std::pair<std::string,std::string> > Configuration;
	Reference<Configuration> cfg( new Configuration( getConfigStringItems( storage->config(), errorhnd)));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the storage configuration: %s"), errorhnd->fetchError());
	}
	Struct rt;
	strus::bindings::Serializer::serialize( &rt.serialization, *cfg);
	if (!papuga_Allocator_alloc_HostObject( &rt.allocator, 0, cfg.get(), strus::bindings::BindingClassTemplate<std::vector<std::string> >::getDestructor())) throw std::bad_alloc();
	cfg.release();
	rt.release();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the storage configuration: %s"), errorhnd->fetchError());
	}
	return rt;
}

StorageTransactionImpl::StorageTransactionImpl( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& errorhnd_, const ObjectRef& storage_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_storage_impl(storage_)
	,m_transaction_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	StorageTransactionInterface* transaction = storage->createTransaction();
	if (!transaction) throw strus::runtime_error( _TXT("failed to create transaction: %s"), errorhnd->fetchError());
	m_transaction_impl.resetOwnership( transaction, "StorageTransaction");
}

void StorageTransactionImpl::insertDocument( const std::string& docid, const ValueVariant& doc)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("try to insert document in a closed transaction"));
	Reference<StorageDocumentInterface> document( transaction->createDocument( docid));
	if (!document.get()) throw strus::runtime_error( _TXT("failed to create document with id '%s' to insert: %s"), docid.c_str(), errorhnd->fetchError());

	Deserializer::buildInsertDocument( document.get(), doc, errorhnd);
	document->done();
}

void StorageTransactionImpl::deleteDocument( const std::string& docId)
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("try to delete document in a closed transaction"));
	transaction->deleteDocument( docId);
}

void StorageTransactionImpl::deleteUserAccessRights( const std::string& username)
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("try to delete user access rights in a closed transaction"));
	transaction->deleteUserAccessRights( username);
}

void StorageTransactionImpl::commit()
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (transaction)
	{
		if (!transaction->commit())
		{
			m_transaction_impl.reset();
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			throw strus::runtime_error( _TXT("error flushing storage operations: %s"), errorhnd->fetchError());
		}
		m_transaction_impl.reset();
	}
}

void StorageTransactionImpl::rollback()
{
	m_transaction_impl.reset();
}



