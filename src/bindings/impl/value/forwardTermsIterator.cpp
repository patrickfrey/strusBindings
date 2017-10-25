/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/forwardTermsIterator.hpp"
#include "papuga/serialization.h"
#include "papuga/callResult.h"
#include "papuga/allocator.h"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/forwardIteratorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "internationalization.hpp"

#define ITERATOR_NAME "forward terms iterator"

using namespace strus;
using namespace strus::bindings;

ForwardTermsIterator::ForwardTermsIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const std::string& termtype, const Index& docno, const Index& pos_)
	:m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_storage_impl(storage_)
	,m_errorhnd_impl(errorhnd_)
	,m_iter(),m_pos(pos_)
{
	const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!docno) throw strus::runtime_error(_TXT("zero passed as docno to %s"), ITERATOR_NAME);
	m_iter.reset( storage->createForwardIterator( termtype));
	if (!m_iter.get()) throw strus::runtime_error(_TXT("failed to create %s: %s"), ITERATOR_NAME, errorhnd->fetchError());
	m_iter->skipDoc( docno);
	if (errorhnd->hasError()) throw strus::runtime_error(_TXT("error in %s get next: %s"), ITERATOR_NAME, errorhnd->fetchError());
}

bool ForwardTermsIterator::getNext( papuga_CallResult* result)
{
	try
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		bool ser = true;
		m_pos = m_iter->skipPos( m_pos+1);
		if (!m_pos)
		{
			if (errorhnd->hasError())
			{
				papuga_CallResult_reportError( result, _TXT("error in %s get next: %s"), ITERATOR_NAME, errorhnd->fetchError());
			}
			return false;
		}
		else
		{
			std::string value = m_iter->fetch();
			if (errorhnd->hasError())
			{
				papuga_CallResult_reportError( result, _TXT("error in %s get next: %s"), ITERATOR_NAME, errorhnd->fetchError());
				return false;
			}
			ser &= papuga_add_CallResult_string_copy( result, value.c_str(), value.size());
			ser &= papuga_add_CallResult_int( result, m_pos);
			if (!ser) throw std::bad_alloc();
			return true;
		}
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

bool ForwardTermsIterator::GetNext( void* self, papuga_CallResult* result)
{
	return ((ForwardTermsIterator*)self)->getNext( result);
}

void ForwardTermsIterator::Deleter( void* obj)
{
	delete (ForwardTermsIterator*)obj;
}


