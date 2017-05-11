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
#include "papugaSerialization.hpp"
#include "hostObjectReference.hpp"
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vector.hpp"
#include "impl/analyzer.hpp"
#include "impl/query.hpp"
#include "impl/statistics.hpp"
#include "impl/struct.hpp"
#include "implTraits.hpp"
#include "serializer.hpp"

namespace {

template <typename STRUCTVALUE>
void initCallResultStructureOwnership( papuga_CallResult* retval, STRUCTVALUE* st)
{
	strus::bindings::HostObjectReference::initOwnership( retval->object, st);
	strus::bindings::Serializer::serialize( &retval->serialization, *st);
	papuga_init_ValueVariant_serialization( &retval->value, &retval->serialization);
}

template <>
void initCallResultStructureOwnership<strus::bindings::Struct>( papuga_CallResult* retval, strus::bindings::Struct* st)
{
	strus::bindings::HostObjectReference::initOwnership( retval->object, st);
	papuga_init_ValueVariant_serialization( &retval->value, &st->serialization);
}

template <>
void initCallResultStructureOwnership<strus::StatisticsViewerInterface>( papuga_CallResult* retval, strus::StatisticsViewerInterface* st)
{
	strus::bindings::HostObjectReference::initOwnership( retval->object, st);
	if (!strus::bindings::Serializer::serialize_nothrow( &retval->serialization, *st)) throw std::bad_alloc();
	papuga_init_ValueVariant_serialization( &retval->value, &retval->serialization);
}

template <typename STRUCTVALUE>
static void initCallResultStructureConst( papuga_CallResult* retval, const STRUCTVALUE* st)
{
	strus::bindings::Serializer::serialize( &retval->serialization, *st);
	papuga_init_ValueVariant_serialization( &retval->value, &retval->serialization);
}

template <typename STRUCTVALUE>
static void initCallResultNumericValues( papuga_CallResult* retval, const STRUCTVALUE& st)
{
	strus::bindings::Serializer::serialize( &retval->serialization, st);
	papuga_init_ValueVariant_serialization( &retval->value, &retval->serialization);
}

template <typename OBJECT>
static void initCallResultObjectOwnership( papuga_CallResult* retval, OBJECT* st)
{
	strus::bindings::HostObjectReference::initOwnership( retval->object, st);
	papuga_init_ValueVariant_hostobj( &retval->value, st, strus::bindings::ClassIdMap::get(*st));
}

template <typename OBJECT>
static void initCallResultObjectConst( papuga_CallResult* retval, const OBJECT* st)
{
	strus::bindings::HostObjectReference::initConst( &retval->object, st);
	papuga_init_ValueVariant_hostobj( &retval->value, st, strus::bindings::ClassIdMap::get(*st));
}

}//namespace
#endif


