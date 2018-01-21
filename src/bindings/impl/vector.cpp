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
#include "strus/base/configParser.hpp"
#include "private/internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

VectorStorageSearcherImpl::VectorStorageSearcherImpl( const ObjectRef& trace, const ObjectRef& storageref, int range_from, int range_to, const ObjectRef& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_searcher_impl()
	,m_trace_impl( trace)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const VectorStorageClientInterface* storage = storageref.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));

	m_searcher_impl.resetOwnership( storage->createSearcher( range_from, range_to), "VectorStorageSearcher");
	if (!m_searcher_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage searcher: %s"), errorhnd->fetchError());
	}
}

std::vector<VectorQueryResult> VectorStorageSearcherImpl::findSimilar( const ValueVariant& vec, unsigned int maxNofResults) const
{
	const VectorStorageSearchInterface* searcher = m_searcher_impl.getObject<VectorStorageSearchInterface>();
	if (!searcher) throw strus::runtime_error( "%s",  _TXT("calling vector storage searcher method after close"));

	std::vector<VectorQueryResult>
		res = searcher->findSimilar( Deserializer::getDoubleList( vec), maxNofResults);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	return res;
}

std::vector<VectorQueryResult> VectorStorageSearcherImpl::findSimilarFromSelection( const ValueVariant& featidxlist, const ValueVariant& vec, unsigned int maxNofResults) const
{
	const VectorStorageSearchInterface* searcher = m_searcher_impl.getObject<VectorStorageSearchInterface>();
	if (!searcher) throw strus::runtime_error( "%s",  _TXT("calling vector storage searcher method after close"));

	std::vector<VectorQueryResult>
		res = searcher->findSimilarFromSelection(
			Deserializer::getIndexList( featidxlist),
			Deserializer::getDoubleList( vec),
			maxNofResults);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	return res;
}

void VectorStorageSearcherImpl::close()
{
	if (!m_searcher_impl.get()) throw strus::runtime_error( "%s",  _TXT("calling storage searcher method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_searcher_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage searcher close: %s"), errorhnd->fetchError());
	}
}

void VectorStorageClientImpl::close()
{
	if (!m_vector_storage_impl.get()) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_vector_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcherImpl* VectorStorageClientImpl::createSearcher( int range_from, int range_to) const
{
	return new VectorStorageSearcherImpl( m_trace_impl, m_vector_storage_impl, range_from, range_to, m_errorhnd_impl);
}

VectorStorageTransactionImpl* VectorStorageClientImpl::createTransaction()
{
	return new VectorStorageTransactionImpl( m_trace_impl, m_objbuilder_impl, m_vector_storage_impl, m_errorhnd_impl, m_config);
}

Struct VectorStorageClientImpl::conceptClassNames() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));
	Reference<std::vector<std::string> > cfg( new std::vector<std::string>( storage->conceptClassNames()));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept class names: %s"), errorhnd->fetchError());
	}
	Struct rt;
	if (!papuga_Allocator_alloc_HostObject( &rt.allocator, 0, cfg.get(), strus::bindings::BindingClassTemplate<std::vector<std::string> >::getDestructor())) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( &rt.serialization, *cfg);
	cfg.release();
	rt.release();
	return rt;
}

std::vector<Index> VectorStorageClientImpl::conceptFeatures( const std::string& conceptClass, int conceptid) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));
	std::vector<Index> rt( storage->conceptFeatures( conceptClass, conceptid));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept features: %s"), errorhnd->fetchError());
	}
	return rt;
}

unsigned int VectorStorageClientImpl::nofConcepts( const std::string& conceptClass) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofConcepts( conceptClass);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of concepts: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::vector<Index> VectorStorageClientImpl::featureConcepts( const std::string& conceptClass, int index) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));

	std::vector<Index> rt( storage->featureConcepts( conceptClass, index));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature concepts: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::vector<double> VectorStorageClientImpl::featureVector( int index) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));

	std::vector<double> rt( storage->featureVector( index));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature vector of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClientImpl::featureName( int index) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));

	std::string rt( storage->featureName( index));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

Index VectorStorageClientImpl::featureIndex( const std::string& name) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));

	Index rt = storage->featureIndex( name);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %s: %s"), name.c_str(), errorhnd->fetchError());
	}
	return rt;
}

unsigned int VectorStorageClientImpl::nofFeatures() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofFeatures();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of features defined: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClientImpl::configstring() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	std::string rt( storage->config());
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the vector storage configuration: %s"), errorhnd->fetchError());
	}
	return rt;
}

Struct VectorStorageClientImpl::config() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( "%s",  _TXT("calling vector storage client method after close"));
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
		throw strus::runtime_error(_TXT("failed to get the vector storage configuration: %s"), errorhnd->fetchError());
	}
	return rt;
}

VectorStorageClientImpl::VectorStorageClientImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_vector_storage_impl()
	,m_config( config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();

	m_vector_storage_impl.resetOwnership( createVectorStorageClient( objBuilder, errorhnd, m_config), "VectorStorageClient");
	if (!m_vector_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage client: %s"), errorhnd->fetchError());
	}
}

void VectorStorageTransactionImpl::addFeature( const std::string& name, const ValueVariant& vec)
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!transaction)
	{
		VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
		m_vector_transaction_impl.resetOwnership( transaction = storage->createTransaction(), "VectorStorageTransaction");
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
		m_vector_transaction_impl.resetOwnership( transaction = storage->createTransaction(), "VectorStorageTransaction");
		if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
	}
	transaction->defineFeatureConceptRelation( relationTypeName, featidx, conidx);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

void VectorStorageTransactionImpl::commit()
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( "%s",  _TXT("calling vector storage builder method after close"));

	bool rt = transaction->commit();
	if (!rt)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT("failed to complete vector storage building (done): %s"), errorhnd->fetchError());
		}
	}
}

void VectorStorageTransactionImpl::rollback()
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( "%s",  _TXT("calling vector storage builder method after close"));
	transaction->rollback();
}

void VectorStorageTransactionImpl::close()
{
	if (!m_vector_transaction_impl.get()) throw strus::runtime_error( "%s",  _TXT("calling vector storage builder method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_vector_transaction_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransactionImpl::VectorStorageTransactionImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& storageref, const ObjectRef& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_vector_storage_impl(storageref)
	,m_vector_transaction_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();

	m_vector_transaction_impl.resetOwnership( storage->createTransaction(), "VectorStorageTransaction");
	if (!m_vector_transaction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage transaction: %s"), errorhnd->fetchError());
	}
}



