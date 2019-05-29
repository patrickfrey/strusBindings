/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STORAGE_STATISTICS_ITERATOR_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STORAGE_STATISTICS_ITERATOR_HPP_INCLUDED
#include "papuga/typedefs.h"
#include "strus/reference.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "impl/value/objectref.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;

class StatisticsIterator
{
public:
	StatisticsIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const ValueVariant& timestamp_);
	StatisticsIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_);
	virtual ~StatisticsIterator(){}

	bool getNext( papuga_CallResult* result);

	static bool GetNext( void* self, papuga_CallResult* result);
	static void Deleter( void* obj);

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_errorhnd_impl;
	Reference<StatisticsIteratorInterface> m_iter;
};

}}//namespace
#endif


