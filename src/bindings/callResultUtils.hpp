/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_CALL_RESULT_UTILS_HPP_INCLUDED
#define _STRUS_BINDINGS_CALL_RESULT_UTILS_HPP_INCLUDED
/// \brief Helper templates to build the CallResult structure, the uniform return value of binding methods
/// \file callResultUtils.hpp
#include "papuga/callResult.h"
#include "papuga/valueVariant.h"
#include "papuga/serialization.hpp"
#include "bindingClassTemplate.hpp"
#include "hostObject.hpp"
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vector.hpp"
#include "impl/analyzer.hpp"
#include "impl/query.hpp"
#include "impl/statistics.hpp"
#include "impl/struct.hpp"
#include "impl/iterator.hpp"
#include "bindingClassTemplate.hpp"
#include "serializer.hpp"

namespace {

void initCallResultStruct( papuga_CallResult* retval, const strus::bindings::Struct& st)
{
	if (!papuga_set_CallResult_serialization( retval)) throw std::bad_alloc();
	// PF:HACK: Dangerous intrusiveness (do use move semantics instead C++11)
	std::memcpy( retval->value.value.serialization, &st.serialization, sizeof(papuga_Serialization));
	papuga_destroy_Allocator( &retval->allocator);
	std::memcpy( &retval->allocator, &st.allocator, sizeof(st.allocator));
}

void initCallResultIterator( papuga_CallResult* retval, const strus::bindings::Iterator& st)
{
	if (!papuga_set_CallResult_iterator( retval, st.iterator.data, st.iterator.destroy, st.iterator.getNext)) throw std::bad_alloc();
}

template <typename STRUCTVALUE>
void initCallResultStructureOwnership( papuga_CallResult* retval, STRUCTVALUE* st)
{
	papuga_HostObject* obj = papuga_Allocator_alloc_HostObject( &retval->allocator, strus::bindings::BindingClassTemplate<STRUCTVALUE>::classid(), st, strus::bindings::BindingClassTemplate<STRUCTVALUE>::getDestructor());
	if (!obj) throw std::bad_alloc();
	if (!papuga_set_CallResult_serialization( retval)) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( retval->value.value.serialization, *st);
}

template <typename STRUCTVALUE>
static void initCallResultStructureConst( papuga_CallResult* retval, const STRUCTVALUE* st)
{
	if (!papuga_set_CallResult_serialization( retval)) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( retval->value.value.serialization, *st);
}

template <typename STRUCTVALUE>
static void initCallResultAtomic( papuga_CallResult* retval, const STRUCTVALUE& st)
{
	if (!papuga_set_CallResult_serialization( retval)) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( retval->value.value.serialization, st);
}

template <typename OBJECT>
static void initCallResultObjectOwnership( papuga_CallResult* retval, OBJECT* st)
{
	if (!papuga_set_CallResult_hostobject( retval, strus::bindings::BindingClassTemplate<OBJECT>::classid(), st, strus::bindings::BindingClassTemplate<OBJECT>::getDestructor())) throw std::bad_alloc();
}

template <typename OBJECT>
static void initCallResultObjectConst( papuga_CallResult* retval, const OBJECT* st)
{
	if (!papuga_set_CallResult_hostobject( retval, strus::bindings::BindingClassTemplate<OBJECT>::classid(), st, 0)) throw std::bad_alloc();
}

}//namespace
#endif


