/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/vector.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/vectorStorageDumpInterface.hpp"
#include "strus/vectorStorageTransactionInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

VectorStorageSearcherImpl::VectorStorageSearcherImpl( const HostObjectReference& storageref, const HostObjectReference& trace, int range_from, int range_to, const HostObjectReference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_searcher_impl()
	,m_trace_impl( trace)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const VectorStorageClientInterface* storage = storageref.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	m_searcher_impl.resetOwnership( storage->createSearcher( range_from, range_to));
	if (!m_searcher_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage searcher: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcherImpl::VectorStorageSearcherImpl( const VectorStorageSearcherImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_searcher_impl(o.m_searcher_impl)
	,m_trace_impl(o.m_trace_impl)
{}

CallResult VectorStorageSearcherImpl::findSimilar( const ValueVariant& vec, unsigned int maxNofResults) const
{
	const VectorStorageSearchInterface* searcher = m_searcher_impl.getObject<VectorStorageSearchInterface>();
	if (!searcher) throw strus::runtime_error( _TXT("calling vector storage searcher method after close"));

	std::vector<VectorStorageSearchInterface::Result>
		res = searcher->findSimilar( Deserializer::getDoubleList( vec), maxNofResults);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	return callResultStructure( res);
}

CallResult VectorStorageSearcherImpl::findSimilarFromSelection( const ValueVariant& featidxlist, const ValueVariant& vec, unsigned int maxNofResults) const
{
	const VectorStorageSearchInterface* searcher = m_searcher_impl.getObject<VectorStorageSearchInterface>();
	if (!searcher) throw strus::runtime_error( _TXT("calling vector storage searcher method after close"));

	std::vector<VectorStorageSearchInterface::Result>
		res = searcher->findSimilarFromSelection(
			Deserializer::getIndexList( featidxlist),
			Deserializer::getDoubleList( vec),
			maxNofResults);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	return callResultStructure( res);
}

void VectorStorageSearcherImpl::close()
{
	if (!m_searcher_impl.get()) throw strus::runtime_error( _TXT("calling storage searcher method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_searcher_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage searcher close: %s"), errorhnd->fetchError());
	}
}

VectorStorageClientImpl::VectorStorageClientImpl( const VectorStorageClientImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_vector_storage_impl(o.m_vector_storage_impl)
	,m_config(o.m_config)
{}

void VectorStorageClientImpl::close()
{
	if (!m_vector_storage_impl.get()) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_vector_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

CallResult VectorStorageClientImpl::createSearcher( int range_from, int range_to) const
{
	return callResultObject( new VectorStorageSearcherImpl( m_vector_storage_impl, m_trace_impl, range_from, range_to, m_errorhnd_impl));
}

CallResult VectorStorageClientImpl::createTransaction()
{
	return callResultObject( new VectorStorageTransactionImpl( m_objbuilder_impl, m_vector_storage_impl, m_trace_impl, m_errorhnd_impl, m_config));
}

CallResult VectorStorageClientImpl::conceptClassNames() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	CallResult rt( callResultStructureOwnership( new std::vector<std::string>( storage->conceptClassNames())));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept class names: %s"), errorhnd->fetchError());
	}
	return rt;
}

CallResult VectorStorageClientImpl::conceptFeatures( const std::string& conceptClass, int conceptid) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	CallResult rt( callResultStructureOwnership( new std::vector<Index>( storage->conceptFeatures( conceptClass, conceptid))));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept features: %s"), errorhnd->fetchError());
	}
	return rt;
}

CallResult VectorStorageClientImpl::nofConcepts( const std::string& conceptClass) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofConcepts( conceptClass);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of concepts: %s"), errorhnd->fetchError());
	}
	return rt;
}

CallResult VectorStorageClientImpl::featureConcepts( const std::string& conceptClass, int index) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	CallResult rt( callResultStructureOwnership( new std::vector<Index>( storage->featureConcepts( conceptClass, index))));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature concepts: %s"), errorhnd->fetchError());
	}
	return rt;
}

CallResult VectorStorageClientImpl::featureVector( int index) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	CallResult rt( callResultStructure( new std::vector<double>( storage->featureVector( index))));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature vector of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

CallResult VectorStorageClientImpl::featureName( int index) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	CallResult rt( storage->featureName( index));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

CallResult VectorStorageClientImpl::featureIndex( const std::string& name) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	Index rt = storage->featureIndex( name);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %s: %s"), name.c_str(), errorhnd->fetchError());
	}
	return rt;
}

CallResult VectorStorageClientImpl::nofFeatures() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofFeatures();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of features defined: %s"), errorhnd->fetchError());
	}
	return rt;
}

CallResult VectorStorageClientImpl::config() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::string rt = storage->config();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the storage configuration string: %s"), errorhnd->fetchError());
	}
	return rt;
}

VectorStorageClientImpl::VectorStorageClientImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_vector_storage_impl()
	,m_config(config)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();

	m_vector_storage_impl.resetOwnership( createVectorStorageClient( objBuilder, errorhnd, config));
	if (!m_vector_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage client: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransactionImpl::VectorStorageTransactionImpl( const VectorStorageTransactionImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_vector_storage_impl(o.m_vector_storage_impl)
	,m_vector_transaction_impl(o.m_vector_transaction_impl)
{}

void VectorStorageTransactionImpl::addFeature( const std::string& name, const ValueVariant& vec)
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!transaction)
	{
		VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
		m_vector_transaction_impl.resetOwnership( transaction = storage->createTransaction());
		if (!transaction) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
	}
	transaction->addFeature( name, Deserializer::getDoubleList( vec));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

void VectorStorageTransactionImpl::defineFeatureConceptRelation( const std::string& relationTypeName, int featidx, int conidx)
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!transaction)
	{
		VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
		m_vector_transaction_impl.resetOwnership( transaction = storage->createTransaction());
		if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
	}
	transaction->defineFeatureConceptRelation( relationTypeName, featidx, conidx);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

CallResult VectorStorageTransactionImpl::commit()
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));

	bool rt = transaction->commit();
	if (!rt)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT("failed to complete vector storage building (done): %s"), errorhnd->fetchError());
		}
	}
	return rt;
}

void VectorStorageTransactionImpl::rollback()
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));
	transaction->rollback();
}

void VectorStorageTransactionImpl::close()
{
	if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_vector_transaction_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransactionImpl::VectorStorageTransactionImpl( const HostObjectReference& objbuilder, const HostObjectReference& storageref, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_vector_storage_impl(storageref)
	,m_vector_transaction_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();

	m_vector_transaction_impl.resetOwnership( storage->createTransaction());
	if (!m_vector_transaction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage transaction: %s"), errorhnd->fetchError());
	}
}



