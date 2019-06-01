/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/statistics.hpp"
#include "impl/value/statisticsIntrospection.hpp"
#include "deserializer.hpp"
#include "strus/statisticsMapInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/statisticsMapInterface.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/configParser.hpp"
#include "private/internationalization.hpp"

using namespace strus;
using namespace strus::bindings;

StatisticsMapImpl::StatisticsMapImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_statmap_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	std::string configstr = config;
	std::string statsprocname;
	(void)extractStringFromConfigString( statsprocname, configstr, "proc", errorhnd);

	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
	m_statsproc = objBuilder->getStatisticsProcessor( statsprocname);
	if (!m_statsproc) throw strus::runtime_error( _TXT("unknown statistics processor '%s'"), statsprocname.c_str());

	m_statmap_impl.resetOwnership( m_statsproc->createMap( configstr), "statistics map");
	if (!m_statmap_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create statistics map: %s"), errorhnd->fetchError());
	}
}

void StatisticsMapImpl::addNofDocumentsInsertedChange( int increment)
{
	StatisticsMapInterface* THIS = m_statmap_impl.getObject<StatisticsMapInterface>();
	THIS->addNofDocumentsInsertedChange( increment);
}

void StatisticsMapImpl::addDfChange( const std::string& type, const std::string& term, int increment)
{
	StatisticsMapInterface* THIS = m_statmap_impl.getObject<StatisticsMapInterface>();
	THIS->addDfChange( type.c_str(), term.c_str(), increment);
}

void StatisticsMapImpl::processStatisticsMessage( const ValueVariant& blob)
{
	StatisticsMapInterface* THIS = m_statmap_impl.getObject<StatisticsMapInterface>();
	StatisticsMessage msg = Deserializer::getStatisticsMessage( blob);
	if (!THIS->processStatisticsMessage( msg.ptr(), msg.size()))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error(_TXT( "failed to feed statistics message blob: %s"), errorhnd->fetchError());
	}
}

GlobalCounter StatisticsMapImpl::nofDocuments()
{
	StatisticsMapInterface* THIS = m_statmap_impl.getObject<StatisticsMapInterface>();
	return THIS->nofDocuments();
}

GlobalCounter StatisticsMapImpl::df( const std::string& termtype, const std::string& termvalue)
{
	StatisticsMapInterface* THIS = m_statmap_impl.getObject<StatisticsMapInterface>();
	return THIS->df( termtype, termvalue);
}

Struct StatisticsMapImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsMapInterface* THIS = m_statmap_impl.getObject<const StatisticsMapInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new StatisticsMapIntrospection( errorhnd, THIS));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}


