/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/searchTermsIterator.hpp"
#include "papuga/serialization.h"
#include "papuga/callResult.h"
#include "papuga/allocator.h"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/documentTermIteratorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "internationalization.hpp"

#define ITERATOR_NAME "search terms iterator"

using namespace strus;
using namespace strus::bindings;

SearchTermsIterator::SearchTermsIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const std::string& termtype, const Index& docno)
	:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_errorhnd_impl(errorhnd_),m_iter(),m_docno(0)
{
	const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!docno) throw strus::runtime_error(_TXT("zero passed as docno to %s"), ITERATOR_NAME);
	m_iter.reset( storage->createDocumentTermIterator( termtype));
	if (!m_iter.get()) throw strus::runtime_error(_TXT("failed to create %s: %s"), ITERATOR_NAME, errorhnd->fetchError());
	if (docno == m_iter->skipDoc( docno))
	{
		m_docno = docno;
	}
	if (errorhnd->hasError()) throw strus::runtime_error(_TXT("error in %s get next: %s"), ITERATOR_NAME, errorhnd->fetchError());
}

bool SearchTermsIterator::getNext( papuga_CallResult* result)
{
	try
	{
		if (!m_docno) return false;
		bool ser = true;
		DocumentTermIteratorInterface::Term term;
		if (m_iter->nextTerm( term))
		{
			std::string value = m_iter->termValue( term.termno);
			ser &= papuga_add_CallResult_string_copy( result, value.c_str(), value.size());
			ser &= papuga_add_CallResult_int( result, term.tf);
			ser &= papuga_add_CallResult_int( result, term.firstpos);
			if (!ser) throw std::bad_alloc();
			return true;
		}
		else
		{
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			if (errorhnd->hasError())
			{
				papuga_CallResult_reportError( result, _TXT("error in %s get next: %s"), ITERATOR_NAME, errorhnd->fetchError());
			}
			return false;
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

bool SearchTermsIterator::GetNext( void* self, papuga_CallResult* result)
{
	return ((SearchTermsIterator*)self)->getNext( result);
}

void SearchTermsIterator::Deleter( void* obj)
{
	delete (SearchTermsIterator*)obj;
}


