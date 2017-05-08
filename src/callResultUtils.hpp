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
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vector.hpp"
#include "impl/analyzer.hpp"
#include "impl/query.hpp"
#include "impl/statistics.hpp"
#include "impl/struct.hpp"
#include "implTraits.hpp"
#include "serializer.hpp"

namespace strus {
namespace bindings {

template <typename STRUCTVALUE>
static void initCallResultStructureOwnership( papuga::CallResult& retval, STRUCTVALUE* st)
{
	retval.object.createOwnership( st);
	Serializer::serialize( retval.serialization, *st);
	retval.value.init( &retval.serialization);
}

template <>
static void initCallResultStructureOwnership<Struct>( papuga::CallResult& retval, Struct* st)
{
	retval.object.createOwnership( st);
	retval.value.init( &st->serialization);
}

template <typename STRUCTVALUE>
static void initCallResultStructureConst( papuga::CallResult& retval, const STRUCTVALUE* st)
{
	Serializer::serialize( retval.serialization, *st);
	retval.value.init( &retval.serialization);
}

template <typename STRUCTVALUE>
static void initCallResultNumericValues( papuga::CallResult& retval, const STRUCTVALUE& st)
{
	Serializer::serialize( retval.serialization, st);
	retval.value.init( &retval.serialization);
}

template <typename OBJECT>
static void initCallResultObjectOwnership( papuga::CallResult& retval, OBJECT* st)
{
	HostObjectReference::initOwnership( &retval.object, st);
	retval.value.init( (const void*)st, ClassIdMap::get(*st));
}

template <typename OBJECT>
static void initCallResultObjectConst( papuga::CallResult& retval, const OBJECT* st)
{
	HostObjectReference::initConst( &retval.object, st);
	retval.value.init( (const void*)st, ClassIdMap::get(*st));
}


}}//namespace
#endif


