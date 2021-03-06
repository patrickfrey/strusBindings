/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/vector.hpp"
#include "impl/sentence.hpp"
#include "impl/value/vectorStorageIntrospection.hpp"
#include "impl/value/valueIterator.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/vectorStorageDumpInterface.hpp"
#include "strus/vectorStorageTransactionInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/constants.hpp"
#include "private/internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

VectorStorageClientImpl::~VectorStorageClientImpl()
{}

void VectorStorageClientImpl::prepareSearch( const std::string& type)
{
	VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	storage->prepareSearch( type);
}

Struct VectorStorageClientImpl::findSimilar( const std::string& type, const ValueVariant& vec, unsigned int maxNofResults, double minSimilarity, double speedRecallFactor, bool realVecWeights) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::vector<VectorQueryResult> res = storage->findSimilar( type, Deserializer::getFloatList( vec), maxNofResults, minSimilarity, speedRecallFactor, realVecWeights);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	Struct rt;
	strus::bindings::Serializer::serialize( &rt.serialization, res, true/*deep*/);
	rt.release();
	return rt;
}

VectorStorageTransactionImpl* VectorStorageClientImpl::createTransaction()
{
	return new VectorStorageTransactionImpl( m_trace_impl, m_objbuilder_impl, m_vector_storage_impl, m_errorhnd_impl, m_config);
}

Struct VectorStorageClientImpl::types() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	typedef std::vector<std::string> NameList;
	Reference<NameList> cfg( new NameList( storage->types()));
	if (cfg->empty())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError()) throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	Struct rt;
	if (!papuga_Allocator_alloc_HostObject( &rt.allocator, 0, cfg.get(), strus::bindings::BindingClassTemplate<NameList>::getDestructor())) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( &rt.serialization, *cfg,false/*deep*/);
	cfg.release();
	rt.release();
	return rt;
}

Struct VectorStorageClientImpl::featureTypes( const std::string& featureValue) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	typedef std::vector<std::string> NameList;
	Reference<NameList> cfg( new NameList( storage->featureTypes( featureValue)));
	if (cfg->empty())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError()) throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	Struct rt;
	if (!papuga_Allocator_alloc_HostObject( &rt.allocator, 0, cfg.get(), strus::bindings::BindingClassTemplate<NameList>::getDestructor())) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( &rt.serialization, *cfg,false/*deep*/);
	cfg.release();
	rt.release();
	return rt;
}

Iterator VectorStorageClientImpl::featureValues()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const VectorStorageClientInterface* vstorage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!vstorage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( vstorage->createFeatureValueIterator(), "FeatureValueIterator"));
	if (!itr.get()) throw strus::runtime_error("%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_vector_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

int VectorStorageClientImpl::nofVectors( const std::string& type) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	int rt = storage->nofVectors( type);
	if (!rt)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( "%s", errorhnd->fetchError());
		}
	}
	return rt;
}

WordVector VectorStorageClientImpl::featureVector( const std::string& type, const std::string& feat) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	WordVector rt( storage->featureVector( type, feat));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return rt;
}

double VectorStorageClientImpl::vectorSimilarity( const ValueVariant& v1, const ValueVariant& v2) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	double rt = storage->vectorSimilarity( Deserializer::getFloatList( v1), Deserializer::getFloatList( v2));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return rt;
}

WordVector VectorStorageClientImpl::normalize( const ValueVariant& vec) const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	WordVector rt = storage->normalize( Deserializer::getFloatList( vec));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClientImpl::configstring() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	std::string rt( storage->config());
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return rt;
}

Struct VectorStorageClientImpl::config() const
{
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	ConfigStruct rt( storage->config(), errorhnd);
	rt.release();
	return rt;
}

void VectorStorageClientImpl::close()
{
	VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	storage->close();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

void VectorStorageClientImpl::compaction()
{
	VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	storage->compaction();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

Struct VectorStorageClientImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const VectorStorageClientInterface* vstorage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new VectorStorageIntrospection( errorhnd, vstorage));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

SentenceLexerImpl* VectorStorageClientImpl::createSentenceLexer() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ObjectRef lexer_impl;
	SentenceLexerInstanceInterface* lexer = storage->createSentenceLexer();
	if (!lexer) throw strus::runtime_error( _TXT("failed to create sentence lexer: %s"), errorhnd->fetchError());
	lexer_impl.resetOwnership( lexer, "SentenceLexer");
	return new SentenceLexerImpl( m_trace_impl, m_objbuilder_impl, lexer_impl, m_errorhnd_impl);
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
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

void VectorStorageTransactionImpl::defineVector( const std::string& type, const std::string& feat, const ValueVariant& vec)
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage transaction method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	WordVector vv = Deserializer::getFloatList( vec);
	if (vv.empty())
	{
		transaction->defineFeature( type, feat);
	}
	else
	{
		transaction->defineVector( type, feat, vv);
	}
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

void VectorStorageTransactionImpl::defineFeature( const std::string& type, const std::string& feat)
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage transaction method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	transaction->defineFeature( type, feat);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

void VectorStorageTransactionImpl::clear()
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage transaction method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	transaction->clear();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

void VectorStorageTransactionImpl::commit()
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage transaction method after close"));

	bool rt = transaction->commit();
	if (!rt)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( "%s", errorhnd->fetchError());
		}
		else
		{
			throw std::runtime_error( _TXT( "commit failed"));
		}
	}
}

void VectorStorageTransactionImpl::rollback()
{
	VectorStorageTransactionInterface* transaction = m_vector_transaction_impl.getObject<VectorStorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage transaction method after close"));
	transaction->rollback();
}

void VectorStorageTransactionImpl::close()
{
	if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("calling vector storage transaction method after close"));
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
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}




