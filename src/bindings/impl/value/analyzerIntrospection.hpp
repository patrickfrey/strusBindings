/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a analyzers
#ifndef _STRUS_BINDING_IMPL_VALUE_ANALYZER_INTROSPECTION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_ANALYZER_INTROSPECTION_HPP_INCLUDED
#include "introspectionBase.hpp"
#include "structViewIntrospection.hpp"
#include "strus/documentAnalyzerInstanceInterface.hpp"
#include "strus/queryAnalyzerInstanceInterface.hpp"
#include "strus/contentStatisticsInterface.hpp"
#include "strus/errorBufferInterface.hpp"

namespace strus {
namespace bindings {

class DocumentAnalyzerIntrospection
	:public StructViewIntrospection
{
public:
	DocumentAnalyzerIntrospection(
			ErrorBufferInterface* errorhnd_,
			const DocumentAnalyzerInstanceInterface* impl_)
		:StructViewIntrospection(errorhnd_,impl_->view())
		{}
};

class QueryAnalyzerIntrospection
	:public StructViewIntrospection
{
public:
	QueryAnalyzerIntrospection(
			ErrorBufferInterface* errorhnd_,
			const QueryAnalyzerInstanceInterface* impl_)
		:StructViewIntrospection(errorhnd_,impl_->view())
		{}
};

class ContentStatisticsIntrospection
	:public StructViewIntrospection
{
public:
	ContentStatisticsIntrospection(
			ErrorBufferInterface* errorhnd_,
			const ContentStatisticsInterface* impl_)
		:StructViewIntrospection(errorhnd_,impl_->view())
		{}
};

}}//namespace
#endif

