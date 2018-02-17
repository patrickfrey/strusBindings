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
#include "papuga/serialization.h"
#include "bindingClassTemplate.hpp"
#include "hostObject.hpp"
#include "bindingClassTemplate.hpp"
#include "serializer.hpp"

namespace strus {

void initCallResultStruct( papuga_CallResult* retval, const strus::bindings::Struct& st);

void initCallResultIterator( papuga_CallResult* retval, const strus::bindings::Iterator& st);

template <typename STRUCTVALUE>
void initCallResultStructureOwnership( papuga_CallResult* retval, STRUCTVALUE* st)
{
	papuga_HostObject* obj = papuga_Allocator_alloc_HostObject( retval->allocator, strus::bindings::BindingClassTemplate<STRUCTVALUE>::classid(), st, strus::bindings::BindingClassTemplate<STRUCTVALUE>::getDestructor());
	if (!obj)
	{
		strus::bindings::BindingClassTemplate<STRUCTVALUE>::getDestructor()( st);
		throw std::bad_alloc();
	}
	if (!papuga_add_CallResult_serialization( retval)) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( retval->valuear[0].value.serialization, *st, false/*deep*/);
}

template <typename STRUCTVALUE>
static void initCallResultStructureConst( papuga_CallResult* retval, const STRUCTVALUE* st)
{
	if (!papuga_add_CallResult_serialization( retval)) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( retval->valuear[0].value.serialization, *st, false/*deep*/);
}

template <typename STRUCTVALUE>
static void initCallResultAtomic( papuga_CallResult* retval, const STRUCTVALUE& st)
{
	if (!papuga_add_CallResult_serialization( retval)) throw std::bad_alloc();
	strus::bindings::Serializer::serialize( retval->valuear[0].value.serialization, st, false/*deep*/);
}

template <typename OBJECT>
static void initCallResultObjectOwnership( papuga_CallResult* retval, OBJECT* st)
{
	if (!papuga_add_CallResult_hostobject( retval, strus::bindings::BindingClassTemplate<OBJECT>::classid(), st, strus::bindings::BindingClassTemplate<OBJECT>::getDestructor()))
	{
		strus::bindings::BindingClassTemplate<OBJECT>::getDestructor()( st);
		throw std::bad_alloc();
	}
}

template <typename OBJECT>
static void initCallResultObjectConst( papuga_CallResult* retval, const OBJECT* st)
{
	if (!papuga_add_CallResult_hostobject( retval, strus::bindings::BindingClassTemplate<OBJECT>::classid(), st, 0)) throw std::bad_alloc();
}

}//namespace
#endif


