/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STORAGE_POSTING_ITERATOR_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STORAGE_POSTING_ITERATOR_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/index.hpp"
#include "strus/reference.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/metaDataRestrictionInstanceInterface.hpp"
#include "impl/value/objectref.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

class PostingIterator
{
public:
	PostingIterator(
		const ObjectRef& trace_,
		const ObjectRef& objbuilder_,
		const ObjectRef& storage_,
		const ObjectRef& errorhnd_,
		const papuga_ValueVariant& expression,
		const papuga_ValueVariant& restriction,
		const Index& start_docno_);
	virtual ~PostingIterator(){}

	bool getNext( papuga_CallResult* result);

	static bool GetNext( void* self, papuga_CallResult* result);
	static void Deleter( void* obj);

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_errorhnd_impl;
	Reference<PostingIteratorInterface> m_postings;
	Reference<MetaDataRestrictionInstanceInterface> m_restriction;
	Index m_docno;
};

}}//namespace
#endif



