/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/vector.hpp"
#include "impl/sentence.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/vectorStorageDumpInterface.hpp"
#include "strus/vectorStorageTransactionInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/base/configParser.hpp"
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

SentenceAnalyzerImpl* VectorStorageClientImpl::createSentenceAnalyzer( const ValueVariant& analyzerconfig) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const VectorStorageClientInterface* storage = m_vector_storage_impl.getObject<VectorStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
	SentenceAnalyzerInstanceInterface* analyzer = objBuilder->createSentenceAnalyzer( Constants::standard_sentence_analyzer());
	if (!analyzer) throw strus::runtime_error( _TXT("failed to create sentence analyzer: %s"), errorhnd->fetchError());
	ObjectRef analyzer_impl;
	ObjectRef lexer_impl;
	analyzer_impl.resetOwnership( analyzer, "SentenceAnalyzer");
	SentenceLexerInstanceInterface* lexer = storage->createSentenceLexer();
	if (!lexer) throw strus::runtime_error( _TXT("failed to create sentence lexer: %s"), errorhnd->fetchError());
	lexer_impl.resetOwnership( lexer, "SentenceLexer");
	return new SentenceAnalyzerImpl( m_trace_impl, m_objbuilder_impl, analyzer_impl, lexer_impl, m_errorhnd_impl, analyzerconfig);
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

	transaction->defineVector( type, feat, Deserializer::getFloatList( vec));
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




