/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/storage.hpp"
#include "impl/statistics.hpp"
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
#include "valueVariantConv.hpp"
#include "internationalization.hpp"
#include "metadataop.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

StorageClientImpl::StorageClientImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config_)
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

StorageClientImpl::StorageClientImpl( const StorageClientImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
{}

CallResult StorageClientImpl::nofDocumentsInserted() const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->nofDocumentsInserted();
}

CallResult StorageClientImpl::createTransaction() const
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return callResultObject( new StorageTransactionImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl));
}

CallResult StorageClientImpl::createInitStatisticsIterator( bool sign)
{
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	HostObjectReference iter;
	iter.resetOwnership( storage->createInitStatisticsIterator( sign));
	if (!iter.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return callResultObject( new StatisticsIteratorImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter));
}

CallResult StorageClientImpl::createUpdateStatisticsIterator()
{
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	HostObjectReference iter;
	iter.resetOwnership( storage->createUpdateStatisticsIterator());
	if (!iter.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return callResultObject( new StatisticsIteratorImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter));
}

CallResult StorageClientImpl::createDocumentBrowser() const
{
	if (!m_objbuilder_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return callResultObject( new DocumentBrowserImpl( m_objbuilder_impl, m_trace_impl, m_storage_impl, m_errorhnd_impl));
}

void StorageClientImpl::close()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage client close: %s"), errorhnd->fetchError());
	}
}

StorageTransactionImpl::StorageTransactionImpl( const HostObjectReference& objbuilder_, const HostObjectReference& trace_, const HostObjectReference& errorhnd_, const HostObjectReference& storage_)
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

DocumentBrowserImpl::DocumentBrowserImpl( const DocumentBrowserImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_restriction_impl(o.m_restriction_impl)
	,m_postingitr_impl(o.m_postingitr_impl)
	,m_attributereader_impl(o.m_attributereader_impl)
	,m_docno(o.m_docno)
{}

DocumentBrowserImpl::DocumentBrowserImpl( const HostObjectReference& objbuilder_impl_, const HostObjectReference& trace_impl_, const HostObjectReference& storage_impl_, const HostObjectReference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_impl_)
	,m_objbuilder_impl(objbuilder_impl_)
	,m_storage_impl(storage_impl_)
	,m_restriction_impl()
	,m_postingitr_impl()
	,m_attributereader_impl()
	,m_docno(0)
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	m_restriction_impl.resetOwnership( storage->createMetaDataRestriction());
	if (!m_restriction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create meta data restriction interface for browsing documents"));
	}
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
	restriction->addCondition( cmpop, name, ValueVariantConv::tonumeric(value), newGroup);
}

CallResult DocumentBrowserImpl::skipDoc( int docno_)
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
	return m_docno = itr->skipDoc( docno_);
}

CallResult DocumentBrowserImpl::attribute( const std::string& name)
{
	if (m_docno)
	{
		if (!m_attributereader_impl.get())
		{
			const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
			m_attributereader_impl.resetOwnership( storage->createAttributeReader());
			if (!m_attributereader_impl.get())
			{
				throw strus::runtime_error( _TXT("failed to create attribute reader for document browser"));
			}
		}
		const AttributeReaderInterface* reader = m_attributereader_impl.getObject<AttributeReaderInterface>();
		Index elemhnd = reader->elementHandle( name.c_str());
		if (!elemhnd)
		{
			throw strus::runtime_error( _TXT("document attribute name %s is not defined"), name.c_str());
		}
		return reader->getValue( elemhnd);
	}
	else
	{
		return CallResult();
	}
}


