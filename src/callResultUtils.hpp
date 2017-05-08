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
#include "papuga/callResult.hpp"
#include "papuga/valueVariant.hpp"
#include "papugaSerialization.hpp"
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vector.hpp"
#include "impl/analyzer.hpp"
#include "impl/query.hpp"
#include "impl/statistics.hpp"
#include "implTraits.hpp"
#include "serializer.hpp"

namespace strus {
namespace bindings {

template <typename STRUCTVALUE>
static papuga::CallResult callResultStructureOwnership( STRUCTVALUE* st)
{
	papuga::CallResult rt;
	rt.object.createOwnership( st);
	Serializer::serialize( rt.serialization, *st);
	rt.value.init( &rt.serialization);
	return rt;
}

template <typename STRUCTVALUE>
static papuga::CallResult callResultStructureConst( const STRUCTVALUE* st)
{
	papuga::CallResult rt;
	Serializer::serialize( rt.serialization, *st);
	rt.value.init( &rt.serialization);
	return rt;
}

template <typename STRUCTVALUE>
static papuga::CallResult callResultStructure( const STRUCTVALUE& st)
{
	papuga::CallResult rt;
	Serializer::serialize( rt.serialization, st);
	rt.value.init( &rt.serialization);
	return rt;
}

template <typename OBJECT>
static papuga::CallResult callResultObject( OBJECT* st)
{
	papuga::CallResult rt;
	rt.object.createOwnership( st);
	rt.value.init( (const void*)st, ClassIdMap::get(*st));
	return rt;
}

}}//namespace
#endif


