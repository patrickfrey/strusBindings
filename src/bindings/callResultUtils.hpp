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
#include "strus/bindingClassId.hpp"
#include "papuga/callResult.h"
#include "papuga/valueVariant.h"
#include "papuga/serialization.hpp"
#include "hostObject.hpp"
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vector.hpp"
#include "impl/analyzer.hpp"
#include "impl/query.hpp"
#include "impl/statistics.hpp"
#include "impl/struct.hpp"
#include "impl/iterator.hpp"
#include "implTraits.hpp"
#include "serializer.hpp"

namespace {

void initCallResultStruct( papuga_CallResult* retval, const strus::bindings::Struct& st)
{
	papuga_set_CallResult_serialization( retval);
	// PF:HACK: Dangerous intrusiveness (do use move semantics instead C++11)
	std::memcpy( &retval->serialization, &st.serialization, sizeof(retval->serialization));
	std::memcpy( &retval->allocator, &st.allocator, sizeof(retval->allocator));
}

void initCallResultIterator( papuga_CallResult* retval, const strus::bindings::Iterator& st)
{
	papuga_set_CallResult_iterator( retval);
	// PF:HACK: Dangerous intrusiveness (do use move semantics instead C++11)
	std::memcpy( &retval->iterator, &st.iterator, sizeof(retval->iterator));
}

template <typename STRUCTVALUE>
void initCallResultStructureOwnership( papuga_CallResult* retval, STRUCTVALUE* st)
{
	strus::bindings::HostObject::initOwnership( retval->object, st);
	strus::bindings::Serializer::serialize( &retval->serialization, *st);
	papuga_init_ValueVariant_serialization( &retval->value, &retval->serialization);
}

template <typename STRUCTVALUE>
static void initCallResultStructureConst( papuga_CallResult* retval, const STRUCTVALUE* st)
{
	strus::bindings::Serializer::serialize( &retval->serialization, *st);
	papuga_init_ValueVariant_serialization( &retval->value, &retval->serialization);
}

template <typename STRUCTVALUE>
static void initCallResultAtomic( papuga_CallResult* retval, const STRUCTVALUE& st)
{
	strus::bindings::Serializer::serialize( &retval->serialization, st);
	papuga_init_ValueVariant_serialization( &retval->value, &retval->serialization);
}

template <typename OBJECT>
static void initCallResultObjectOwnership( papuga_CallResult* retval, OBJECT* st)
{
	strus::bindings::HostObject::initOwnership( retval->object, st);
	papuga_init_ValueVariant_hostobj( &retval->value, st, strus::bindings::ClassIdMap::get(*st));
}

template <typename OBJECT>
static void initCallResultObjectConst( papuga_CallResult* retval, const OBJECT* st)
{
	strus::bindings::HostObject::initConst( &retval->object, st);
	papuga_init_ValueVariant_hostobj( &retval->value, st, strus::bindings::ClassIdMap::get(*st));
}

}//namespace
#endif


