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
#include "strus/bindings/callResult.hpp"
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/serialization.hpp"
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vectorStorage.hpp"
#include "impl/analyzer.hpp"
#include "impl/query.hpp"
#include "impl/statistics.hpp"
#include "serializer.hpp"

namespace strus {
namespace bindings {

struct ClassIdMap
{
	static int get( const DocumentAnalyzerImpl& )	{return ClassDocumentAnalyzer;}
	static int get( const QueryAnalyzerImpl&)	{return ClassQueryAnalyzer;}
	static int get( const QueryEvalImpl&)		{return ClassQueryEval;}
	static int get( const QueryImpl&)		{return ClassQuery;}
};

template <typename STRUCTVALUE>
static CallResult callResultStructureOwnership( STRUCTVALUE* st)
{
	CallResult rt;
	rt.object.createOwnership( st);
	Serializer::serialize( rt.serialization, *st);
	rt.value.init( &rt.serialization);
	return rt;
}

template <typename STRUCTVALUE>
static CallResult callResultStructureConst( const STRUCTVALUE* st)
{
	CallResult rt;
	Serializer::serialize( rt.serialization, *st);
	rt.value.init( &rt.serialization);
	return rt;
}

template <typename OBJECT>
static CallResult callResultObject( OBJECT* st)
{
	CallResult rt;
	rt.object.createOwnership( st);
	rt.classid = ClassIdMap::get(*st);
	return rt;
}

}}//namespace
#endif


