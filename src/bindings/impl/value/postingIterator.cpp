/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/postingIterator.hpp"
#include "papuga/serialization.h"
#include "papuga/callResult.h"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "expressionBuilder.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "internationalization.hpp"

#define ITERATOR_NAME "postings iterator"

using namespace strus;
using namespace strus::bindings;

PostingIterator::PostingIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const papuga_ValueVariant& expression, const papuga_ValueVariant& restriction, const Index& start_docno_)
	:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_errorhnd_impl(errorhnd_),m_postings(),m_restriction(),m_docno(start_docno_?start_docno_:1)
{
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
	const QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!papuga_ValueVariant_defined( &expression))
	{
		throw strus::runtime_error(_TXT("null passed as expression to %s"), ITERATOR_NAME);
	}
	PostingsExpressionBuilder postingsBuilder( storage, queryproc, errorhnd);
	Deserializer::buildExpression( postingsBuilder, expression, errorhnd, false);
	m_postings = postingsBuilder.pop();
	if (papuga_ValueVariant_defined( &restriction))
	{
		Reference<MetaDataRestrictionInterface> builder;
		builder.reset( storage->createMetaDataRestriction());
		if (!builder.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for %s"), ITERATOR_NAME);
		Deserializer::buildMetaDataRestriction( builder.get(), restriction, errorhnd);
		m_restriction.reset( builder->createInstance());
		if (!m_restriction.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for %s instance"), ITERATOR_NAME);
	}
}

bool PostingIterator::getNext( papuga_CallResult* result)
{
	try
	{
		if (!m_docno) return false;
		for (; 0!=(m_docno = m_postings->skipDoc( m_docno)); ++m_docno)
		{
			if (!m_restriction.get() || m_restriction->match( m_docno)) break;
		}
		if (m_docno)
		{
			bool ser = true;
			if (!papuga_set_CallResult_serialization( result)) throw std::bad_alloc();
			papuga_Serialization* serialization = result->value.value.serialization;
			ser &= papuga_Serialization_pushValue_int( serialization, m_docno++);
			ser &= papuga_Serialization_pushOpen( serialization);
			for (Index pos = 0; 0!=(pos=m_postings->skipPos(pos)); ++pos)
			{
				ser &= papuga_Serialization_pushValue_int( serialization, pos);
			}
			ser &= papuga_Serialization_pushClose( serialization);
			if (!ser)
			{
				papuga_CallResult_reportError( result, _TXT("memory allocation error in %s get next"), ITERATOR_NAME);
				return false;
			}
			return true;
		}
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			papuga_CallResult_reportError( result, _TXT("error in %s get next: %s"), ITERATOR_NAME, errorhnd->fetchError());
		}
		return false;
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

bool PostingIterator::GetNext( void* self, papuga_CallResult* result)
{
	return ((PostingIterator*)self)->getNext( result);
}

void PostingIterator::Deleter( void* obj)
{
	delete (PostingIterator*)obj;
}


