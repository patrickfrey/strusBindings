/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/storage.hpp"
#include "impl/statistics.hpp"
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
#include "strus/base/configParser.hpp"
#include "papuga/serialization.hpp"
#include "serializer.hpp"
#include "valueVariantWrap.hpp"
#include "internationalization.hpp"
#include "metadataop.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

StorageClientImpl::StorageClientImpl( const ObjectRef& objbuilder, const ObjectRef& trace, const ObjectRef& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_storage_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();

	m_storage_impl.resetOwnership( createStorageClient( objBuilder, errorhnd, config_));
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

Index StorageClientImpl::documentNumber( const std::string& docid) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->documentNumber( docid);
}

StorageTransactionImpl* StorageClientImpl::createTransaction() const
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return new StorageTransactionImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl);
}

StatisticsIteratorImpl* StorageClientImpl::createStatisticsIterator( bool sign)
{
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef iter;
	iter.resetOwnership( storage->createStatisticsIterator( sign));
	if (!iter.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return new StatisticsIteratorImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter);
}

StatisticsIteratorImpl* StorageClientImpl::createUpdateStatisticsIterator()
{
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef iter;
	iter.resetOwnership( storage->createUpdateStatisticsIterator());
	if (!iter.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return new StatisticsIteratorImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter);
}

DocumentBrowserImpl* StorageClientImpl::createDocumentBrowser() const
{
	if (!m_objbuilder_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return new DocumentBrowserImpl( m_objbuilder_impl, m_trace_impl, m_storage_impl, m_errorhnd_impl);
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

std::vector<std::pair<std::string,std::string> >* StorageClientImpl::config() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	typedef std::vector<std::pair<std::string,std::string> > Configuration;
	Configuration* rt( new Configuration( getConfigStringItems( storage->config(), errorhnd)));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the storage configuration: %s"), errorhnd->fetchError());
	}
	return rt;
}

StorageTransactionImpl::StorageTransactionImpl( const ObjectRef& objbuilder_, const ObjectRef& trace_, const ObjectRef& errorhnd_, const ObjectRef& storage_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_storage_impl(storage_)
	,m_transaction_impl()
{}

StorageTransactionInterface* StorageTransactionImpl::getTransaction()
{
	StorageTransactionInterface* rt;
	if (!m_transaction_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
		m_transaction_impl.resetOwnership( rt = storage->createTransaction());
		if (!rt) throw strus::runtime_error( _TXT("failed to create transaction: %s"), errorhnd->fetchError());
	}
	else
	{
		rt = m_transaction_impl.getObject<StorageTransactionInterface>();
	}
	return rt;
}

void StorageTransactionImpl::insertDocument( const std::string& docid, const ValueVariant& doc)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageTransactionInterface* transaction = getTransaction();
	Reference<StorageDocumentInterface> document( transaction->createDocument( docid));
	if (!document.get()) throw strus::runtime_error( _TXT("failed to create document with id '%s' to insert: %s"), docid.c_str(), errorhnd->fetchError());

	Deserializer::buildInsertDocument( document.get(), doc, errorhnd);
	document->done();
}

void StorageTransactionImpl::deleteDocument( const std::string& docId)
{
	StorageTransactionInterface* transaction = getTransaction();
	transaction->deleteDocument( docId);
}

void StorageTransactionImpl::deleteUserAccessRights( const std::string& username)
{
	StorageTransactionInterface* transaction = getTransaction();
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

DocumentBrowserImpl::DocumentBrowserImpl( const ObjectRef& objbuilder_impl_, const ObjectRef& trace_impl_, const ObjectRef& storage_impl_, const ObjectRef& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_impl_)
	,m_objbuilder_impl(objbuilder_impl_)
	,m_storage_impl(storage_impl_)
	,m_restriction_impl()
	,m_postingitr_impl()
	,m_attributereader_impl()
	,m_metadatareader_impl()
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	m_restriction_impl.resetOwnership( storage->createMetaDataRestriction());
	if (!m_restriction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create meta data restriction interface for browsing documents"));
	}
	m_attributereader_impl.resetOwnership( storage->createAttributeReader());
	m_metadatareader_impl.resetOwnership( storage->createMetaDataReader());
}

void DocumentBrowserImpl::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		const ValueVariant& value, bool newGroup)
{
	MetaDataRestrictionInterface* restriction = m_restriction_impl.getObject<MetaDataRestrictionInterface>();
	if (!restriction)
	{
		throw strus::runtime_error( _TXT("it is not allowed to add more restrictions to a document browser after the first call of next()"));
	}
	MetaDataRestrictionInterface::CompareOperator cmpop = getCompareOp( compareOp);
	restriction->addCondition( cmpop, name, ValueVariantWrap::tonumeric(value), newGroup);
}

Index DocumentBrowserImpl::skipDoc( int docno)
{
	if (!m_postingitr_impl.get())
	{
		const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
		MetaDataRestrictionInterface* restriction = m_restriction_impl.getObject<MetaDataRestrictionInterface>();
		m_postingitr_impl.resetOwnership( storage->createBrowsePostingIterator( restriction, 1));
		if (!m_postingitr_impl.get())
		{
			throw strus::runtime_error( _TXT("failed to create posting iterator for document browser"));
		}
	}
	PostingIteratorInterface* itr = m_postingitr_impl.getObject<PostingIteratorInterface>();
	return itr->skipDoc( docno);
}

Struct* DocumentBrowserImpl::get( int docno, const ValueVariant& elementsSelected)
{
	if (docno <= 0) throw strus::runtime_error(_TXT("document browser get called without valid document selected"));

	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::vector<std::string> elemlist = Deserializer::getStringList( elementsSelected);
	AttributeReaderInterface* attributereader = m_attributereader_impl.getObject<AttributeReaderInterface>();
	MetaDataReaderInterface* metadatareader = m_metadatareader_impl.getObject<MetaDataReaderInterface>();
	bool attributereader_called = false;
	bool metadatareader_called = false;

	Reference<Struct> rt( new Struct());
	papuga_Serialization* res = &rt->serialization;
	bool serflag = true;
	serflag &= papuga_Serialization_pushOpen( res);
	std::vector<int> miar( elemlist.size(), -1);

	std::vector<std::string>::const_iterator ei = elemlist.begin(), ee = elemlist.end();
	for (int eidx=0; ei != ee; ++ei,++eidx)
	{
		Index eh = attributereader->elementHandle( ei->c_str());
		if (eh)
		{
			if (!attributereader_called)
			{
				attributereader->skipDoc( docno);
				attributereader_called = true;
			}
			miar[ eidx] = rt->strings.size();
			rt->strings.append( attributereader->getValue( eh));
			rt->strings.push_back( '\0');
		}
	}
	ei = elemlist.begin(), ee = elemlist.end();
	for (int eidx=0; ei != ee; ++ei,++eidx)
	{
		if (miar[ eidx] >= 0)
		{
			serflag &= papuga_Serialization_pushValue_charp( res, rt->strings.c_str() + miar[ eidx]);
		}
		else
		{
			Index eh = metadatareader->elementHandle( ei->c_str());
			if (eh >= 0)
			{
				if (!metadatareader_called)
				{
					metadatareader->skipDoc( docno);
					metadatareader_called = true;
				}
				serflag &= Serializer::serialize_nothrow( res, metadatareader->getValue( eh));
			}
			else
			{
				serflag &= papuga_Serialization_pushValue_void( res);
			}
		}
	}
	serflag &= papuga_Serialization_pushClose( res);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error getting document attributes and metadata with document browser: %s"), errorhnd->fetchError());
	}
	if (!serflag) throw std::bad_alloc();
	return rt.release();
}


