/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/valueIterator.hpp"
#include "papuga/valueVariant.h"
#include "papuga/callResult.h"
#include "strus/index.hpp"
#include "strus/reference.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "internationalization.hpp"
#include <vector>
#include <string>

#define ITERATOR_NAME "value iterator"

using namespace strus;
using namespace strus::bindings;

ValueIterator::ValueIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& values_, const ObjectRef& errorhnd_, const std::string& key)
	:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_values(values_),m_errorhnd_impl(errorhnd_)
{
	ValueIteratorInterface* THIS = m_values.getObject<ValueIteratorInterface>();
	if (!key.empty())
	{
		THIS->skip( key.c_str(), key.size());
	}
	m_block = THIS->fetchValues( MaxNofElements);
	m_blockitr = m_block.begin();
}

bool ValueIterator::getNext( papuga_CallResult* result)
{
	ValueIteratorInterface* THIS = m_values.getObject<ValueIteratorInterface>();
	if (m_blockitr == m_block.end())
	{
		if (m_block.empty()) return false;
		m_block = THIS->fetchValues( MaxNofElements);
		m_blockitr = m_block.begin();
		if (m_blockitr == m_block.end())
		{
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			if (errorhnd->hasError()) throw strus::runtime_error(_TXT("error in %s: %s"), ITERATOR_NAME, errorhnd->fetchError());
			return false;
		}
	}
	if (!papuga_set_CallResult_string( result, m_blockitr->c_str(), m_blockitr->size())) throw std::bad_alloc();
	++m_blockitr;
	return true;
}

bool ValueIterator::GetNext( void* self, papuga_CallResult* result)
{
	try
	{
		return (((ValueIterator*)self)->getNext( result));
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

void ValueIterator::Deleter( void* obj)
{
	delete (ValueIterator*)obj;
}




