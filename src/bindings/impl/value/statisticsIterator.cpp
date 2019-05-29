/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/statisticsIterator.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "callResultUtils.hpp"
#include "papuga/valueVariant.h"
#include "papuga/callResult.h"
#include "strus/lib/error.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/base/base64.hpp"
#include "strus/base/local_ptr.hpp"
#include "private/internationalization.hpp"

#define ITERATOR_NAME "statistics iterator"

using namespace strus;
using namespace strus::bindings;

StatisticsIterator::StatisticsIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const ValueVariant& timestamp_)
	:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_errorhnd_impl(errorhnd_)
{
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (papuga_ValueVariant_defined( &timestamp_))
	{
		TimeStamp timestamp = Deserializer::getTimeStamp( timestamp_);
		m_iter.reset( storage->createChangeStatisticsIterator( timestamp));
	}
	else
	{
		m_iter.reset( storage->createAllStatisticsIterator());
	}
	if (!m_iter.get())
	{
		throw strus::runtime_error(_TXT("failed to create %s: %s"), ITERATOR_NAME, errorhnd->fetchError());
	}
}

StatisticsIterator::StatisticsIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_)
	:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_errorhnd_impl(errorhnd_)
{
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	m_iter.reset( storage->createAllStatisticsIterator());
	if (!m_iter.get())
	{
		throw strus::runtime_error(_TXT("failed to create %s: %s"), ITERATOR_NAME, errorhnd->fetchError());
	}
}

bool StatisticsIterator::getNext( papuga_CallResult* result)
{
	try
	{
		StatisticsIteratorInterface* iter = m_iter.get();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		strus::local_ptr<StatisticsMessage> msg( new StatisticsMessage());
		*msg = iter->getNext();
		if (msg->empty())
		{
			if (errorhnd->hasError())
			{
				throw strus::runtime_error( _TXT("error fetching statistics message: %s"), errorhnd->fetchError());
			}
			return false;
		}
		initCallResultStructureOwnership( result, msg.release());
		return true;
	}
	catch (const std::bad_alloc& err)
	{
		papuga_CallResult_reportError( result, _TXT("memory allocation error in %s get next"), ITERATOR_NAME);
		return false;
	}
	catch (const std::runtime_error& err)
	{
		papuga_CallResult_reportError( result, _TXT("error in %s get next: %s"), ITERATOR_NAME, err.what());
		return false;
	}
}

bool StatisticsIterator::GetNext( void* self, papuga_CallResult* result)
{
	return ((StatisticsIterator*)self)->getNext( result);
}

void StatisticsIterator::Deleter( void* obj)
{
	delete (StatisticsIterator*)obj;
}

