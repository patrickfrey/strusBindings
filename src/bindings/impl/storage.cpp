/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/storage.hpp"
#include "impl/value/postingIterator.hpp"
#include "impl/value/valueIterator.hpp"
#include "impl/value/selectIterator.hpp"
#include "impl/value/statisticsIterator.hpp"
#include "impl/value/forwardTermsIterator.hpp"
#include "impl/value/searchTermsIterator.hpp"
#include "impl/value/storageIntrospection.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageTransactionInterface.hpp"
#include "strus/storageDocumentInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageMetaDataTableUpdateInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/metaDataRestrictionInstanceInterface.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/forwardIteratorInterface.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "papuga/serialization.h"
#include "serializer.hpp"
#include "valueVariantWrap.hpp"
#include "private/internationalization.hpp"
#include "expressionBuilder.hpp"
#include "deserializer.hpp"
#include "structDefs.hpp"
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
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

StorageClientImpl::~StorageClientImpl()
{}

void StorageClientImpl::reload( const ValueVariant& config_)
{
	std::string configstr = Deserializer::getConfigString( config_);
	StorageClientInterface* THIS = m_storage_impl.getObject<StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	if (!THIS->reload( configstr))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "error reloading configuration: %s", errorhnd->fetchError());
	}
}

long StorageClientImpl::nofDocumentsInserted() const
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

Index StorageClientImpl::documentNumber( const std::string& docid_) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->documentNumber( docid_);
}

Iterator StorageClientImpl::documentForwardIndexTerms( const Index& docno, const std::string& termtype, const Index& pos) const
{
	Reference<ForwardTermsIterator> itr( new ForwardTermsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, termtype, docno, pos));
	Iterator rt( itr.get(), &ForwardTermsIterator::Deleter, &ForwardTermsIterator::GetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::documentSearchIndexTerms( const Index& docno, const std::string& termtype) const
{
	Reference<SearchTermsIterator> itr( new SearchTermsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, termtype, docno));
	Iterator rt( itr.get(), &SearchTermsIterator::Deleter, &SearchTermsIterator::GetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::postings( const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno)
{
	Reference<PostingIterator> itr( new PostingIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, expression, restriction, start_docno));
	Iterator rt( itr.get(), &PostingIterator::Deleter, &PostingIterator::GetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::select( const ValueVariant& what, const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno, const ValueVariant& accesslist)
{
	Reference<SelectIterator> itr( new SelectIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, what, expression, restriction, start_docno, accesslist));
	Iterator rt( itr.get(), &SelectIterator::Deleter, &SelectIterator::GetNext);
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
	if (!itr.get()) throw strus::runtime_error("%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::termValues() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createTermValueIterator(), "TermTypeIterator"));
	if (!itr.get()) throw strus::runtime_error("%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::docids() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createDocIdIterator(), "DocIdIterator"));
	if (!itr.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

std::string StorageClientImpl::docid( const Index& docno) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	Reference<AttributeReaderInterface> areader( THIS->createAttributeReader());
	if (!areader.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	Index eh = areader->elementHandle( Constants::attribute_docid());
	if (!eh) throw strus::runtime_error( _TXT("attribute '%s' not defined"), Constants::attribute_docid());
	areader->skipDoc( docno);
	return areader->getValue( eh);
}

Iterator StorageClientImpl::usernames() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createUserNameIterator(), "UserNameIterator"));
	if (!itr.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

std::vector<std::string>* StorageClientImpl::attributeNames() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
	Reference<AttributeReaderInterface> reader( storage->createAttributeReader());
	if (!reader.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return new std::vector<std::string>( reader->getNames());
}

std::vector<std::string>* StorageClientImpl::metadataNames() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
	Reference<MetaDataReaderInterface> reader( storage->createMetaDataReader());
	if (!reader.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return new std::vector<std::string>( reader->getNames());
}

Iterator StorageClientImpl::getAllStatistics()
{
	Iterator rt( new StatisticsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl), &StatisticsIterator::Deleter, &StatisticsIterator::GetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::getChangeStatistics( const ValueVariant& timestamp)
{
	Iterator rt( new StatisticsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, timestamp), &StatisticsIterator::Deleter, &StatisticsIterator::GetNext);
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

void StorageClientImpl::compaction()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	storage->compaction();
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

	ConfigStruct rt( storage->config(), errorhnd);
	rt.release();
	return rt;
}

Struct StorageClientImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new StorageIntrospection( errorhnd, storage));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
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
	if (!transaction) throw strus::runtime_error( "%s", errorhnd->fetchError());
	m_transaction_impl.resetOwnership( transaction, "StorageTransaction");
}

void StorageTransactionImpl::insertDocument( const std::string& docid, const ValueVariant& doc)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (docid.empty()) throw strus::runtime_error( _TXT("empty document id passed to %s"), _TXT("storage transaction insert document"));

	Reference<StorageDocumentInterface> document( transaction->createDocument( docid));
	if (!document.get()) throw strus::runtime_error( _TXT("failed to create document with id '%s' to insert: %s"), docid.c_str(), errorhnd->fetchError());

	Deserializer::buildInsertDocument( document.get(), doc, errorhnd);
	document->done();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to insert document: %s"), errorhnd->fetchError());
	}
}

void StorageTransactionImpl::updateDocument( const std::string& docid, const ValueVariant& content, const ValueVariant& deletes)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	strus::Index docno = storage->documentNumber( docid);
	if (!docno) throw strus::runtime_error( _TXT("failed to update document with undefined id '%s'"), docid.c_str());
	Reference<StorageDocumentUpdateInterface> document( transaction->createDocumentUpdate( docno));
	if (!document.get()) throw strus::runtime_error( _TXT("failed to create document with id '%s' to insert: %s"), docid.c_str(), errorhnd->fetchError());

	Deserializer::buildUpdateDocument( document.get(), content, deletes, errorhnd);
	document->done();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to update document: %s"), errorhnd->fetchError());
	}
}

void StorageTransactionImpl::deleteDocument( const std::string& docId)
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	transaction->deleteDocument( docId);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to insert document: %s"), errorhnd->fetchError());
	}
}

void StorageTransactionImpl::deleteUserAccessRights( const std::string& username)
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	transaction->deleteUserAccessRights( username);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to insert document: %s"), errorhnd->fetchError());
	}
}

static void fillUpdateMetaDataTable( StorageMetaDataTableUpdateInterface* update, const std::vector<MetaDataTableCommand>& cmdlist, ErrorBufferInterface* errorhnd)
{
	std::vector<MetaDataTableCommand>::const_iterator ci = cmdlist.begin(), ce = cmdlist.end();
	for (; ci != ce; ++ci)
	{
		switch (ci->id)
		{
			case MetaDataTableCommand::Add:
				update->addElement( ci->name, ci->type);
				break;
			case MetaDataTableCommand::Replace:
				if (ci->type.empty())
				{
					update->alterElement( ci->oldname, ci->name, ci->type);
				}
				else
				{
					update->renameElement( ci->oldname, ci->name);
				}
				break;
			case MetaDataTableCommand::Remove:
				if (ci->name.empty())
				{
					update->deleteElements();
				}
				else
				{
					update->deleteElement( ci->name);
				}
				break;
			case MetaDataTableCommand::Clear:
				update->clearElement( ci->name);
				break;
		}
	}
	update->done();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to update meta data table: %s"), errorhnd->fetchError());
	}
}

void StorageTransactionImpl::updateMetaDataTable( const ValueVariant& commandlist)
{
	std::vector<MetaDataTableCommand> cmdlist = MetaDataTableCommand::getList( commandlist);
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	strus::Reference<StorageMetaDataTableUpdateInterface> update( transaction->createMetaDataTableUpdate());
	if (!update.get()) throw strus::runtime_error( _TXT("failed to create meta data table update structure"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	fillUpdateMetaDataTable( update.get(), cmdlist, errorhnd);
}

void StorageTransactionImpl::defineMetaDataTable( const ValueVariant& deflist)
{
	std::vector<MetaDataTableCommand> cmdlist = MetaDataTableCommand::getListFromNameTypePairs( deflist);
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	strus::Reference<StorageMetaDataTableUpdateInterface> update( transaction->createMetaDataTableUpdate());
	if (!update.get()) throw strus::runtime_error( _TXT("failed to create meta data table update structure"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	fillUpdateMetaDataTable( update.get(), cmdlist, errorhnd);
}

void StorageTransactionImpl::commit()
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	strus::StorageCommitResult cmres = transaction->commit();
	if (!cmres.success())
	{
		m_transaction_impl.reset();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("error in commit transaction: %s"), errorhnd->fetchError());
	}
}

void StorageTransactionImpl::rollback()
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	transaction->rollback();
}



