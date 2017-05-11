/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/statistics.hpp"
#include "impl/struct.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/statisticsBuilderInterface.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/reference.hpp"
#include "papugaSerialization.hpp"
#include "serializer.hpp"
#include "valueVariantWrap.hpp"
#include "internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"

using namespace strus;
using namespace strus::bindings;

StatisticsIteratorImpl::StatisticsIteratorImpl( const ObjectRef& objbuilder, const ObjectRef& trace, const ObjectRef& errorhnd_, const ObjectRef& storage_, const ObjectRef& iter_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_storage_impl(storage_)
	,m_iter_impl(iter_)
{}

std::string* StatisticsIteratorImpl::getNext()
{
	StatisticsIteratorInterface* iter = m_iter_impl.getObject<StatisticsIteratorInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const char* outmsg;
	std::size_t outmsgsize;
	if (!iter->getNext( outmsg, outmsgsize))
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( _TXT("error fetching statistics message: %s"), errorhnd->fetchError());
		}
	}
	return new std::string( outmsg, outmsgsize);
}

StatisticsProcessorImpl::StatisticsProcessorImpl( const ObjectRef& objbuilder_, const ObjectRef& trace_, const std::string& name_, const ObjectRef& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_statsproc(0)
{
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<StorageObjectBuilderInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	m_statsproc = objBuilder->getStatisticsProcessor( name_);
	if (!m_statsproc)
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT( "error getting statistics message processor: %s"), errorhnd->fetchError());
		}
		throw strus::runtime_error(_TXT( "error statistics message processor not defined"));
	}
}

typedef StatisticsViewerInterface::DocumentFrequencyChange DocumentFrequencyChange;

StatisticsViewerInterface* StatisticsProcessorImpl::decode( const std::string& blob) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsProcessorInterface* proc = m_statsproc;
	StatisticsViewerInterface* viewer;
	viewer = proc->createViewer( blob.c_str(), blob.size());
	if (!viewer) throw strus::runtime_error(_TXT( "error decoding statistics from blob: %s"), errorhnd->fetchError());
	return viewer;
}

std::string* StatisticsProcessorImpl::encode( const ValueVariant& msg) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsProcessorInterface* proc = m_statsproc;
	StatisticsProcessorInterface::BuilderOptions options;
	Reference<StatisticsBuilderInterface> builder( proc->createBuilder( options));
	if (!builder.get()) throw strus::runtime_error(_TXT("failed to create builder for statistics: %s"), errorhnd->fetchError());
	Deserializer::buildStatistics( builder.get(), msg, errorhnd);

	Reference<std::string> rt( new std::string());
	const char* blk;
	std::size_t blksize;
	if (builder->fetchMessage( blk, blksize))
	{
		rt->append( blk, blksize);
	}
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "error creating blob from statistics message structure: %s"), errorhnd->fetchError());
	}
	return rt.release();
}


