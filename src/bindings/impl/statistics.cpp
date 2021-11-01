/*
 * Copyright (c) 2021 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/statistics.hpp"
#include "impl/value/statisticsIntrospection.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "deserializer.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/statisticsStorageInterface.hpp"
#include "strus/statisticsStorageClientInterface.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/configParser.hpp"
#include "private/internationalization.hpp"
#include "papuga/serialization.h"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "valueVariantWrap.hpp"
#include "structDefs.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

StatisticsStorageClientImpl::StatisticsStorageClientImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_storage_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();

	m_storage_impl.resetOwnership( createStatisticsStorageClient( objBuilder, errorhnd, config_), "StatisticsStorageClient");
	if (!m_storage_impl.get())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

StatisticsStorageClientImpl::~StatisticsStorageClientImpl()
{}

void StatisticsStorageClientImpl::reload( const ValueVariant& config_)
{
	std::string configstr = Deserializer::getConfigString( config_);
	StatisticsStorageClientInterface* THIS = m_storage_impl.getObject<StatisticsStorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	if (!THIS->reload( configstr))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "error reloading configuration: %s", errorhnd->fetchError());
	}
}

GlobalCounter StatisticsStorageClientImpl::nofDocuments() const
{
	const StatisticsStorageClientInterface* THIS = m_storage_impl.getObject<const StatisticsStorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	return THIS->nofDocuments();
}

GlobalCounter StatisticsStorageClientImpl::documentFrequency( const std::string& type, const std::string& term) const
{
	const StatisticsStorageClientInterface* THIS = m_storage_impl.getObject<const StatisticsStorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	return THIS->documentFrequency( type, term);
}

TimeStamp StatisticsStorageClientImpl::storageTimeStamp( const std::string& storageid) const
{
	const StatisticsStorageClientInterface* THIS = m_storage_impl.getObject<const StatisticsStorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	return THIS->storageTimeStamp( storageid);
}

void StatisticsStorageClientImpl::putStatisticsMessage( const ValueVariant& msg_, const std::string& storageid)
{
	StatisticsStorageClientInterface* THIS = m_storage_impl.getObject<StatisticsStorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	StatisticsMessage msg = Deserializer::getStatisticsMessage( msg_);
	if (!THIS->putStatisticsMessage( msg, storageid))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("put statistics message failed: %s"), errorhnd->fetchError());
	}
}

void StatisticsStorageClientImpl::close()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	StatisticsStorageClientInterface* storage = m_storage_impl.getObject<StatisticsStorageClientInterface>();
	storage->close();
	m_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling statistics storage client close: %s"), errorhnd->fetchError());
	}
}

void StatisticsStorageClientImpl::compaction()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	StatisticsStorageClientInterface* storage = m_storage_impl.getObject<StatisticsStorageClientInterface>();
	storage->compaction();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling statistics storage client close: %s"), errorhnd->fetchError());
	}
}

std::string StatisticsStorageClientImpl::configstring() const
{
	const StatisticsStorageClientInterface* storage = m_storage_impl.getObject<StatisticsStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	return storage->config();
}

Struct StatisticsStorageClientImpl::config() const
{
	const StatisticsStorageClientInterface* storage = m_storage_impl.getObject<StatisticsStorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling statistics storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	ConfigStruct rt( storage->config(), errorhnd);
	rt.release();
	return rt;
}

Struct StatisticsStorageClientImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsStorageClientInterface* storage = m_storage_impl.getObject<StatisticsStorageClientInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new StatisticsStorageIntrospection( errorhnd, storage));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

