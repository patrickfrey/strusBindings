/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a query analyzer
#ifndef _STRUS_BINDING_IMPL_VALUE_QUERY_ANALYZER_INTROSPECTION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_QUERY_ANALYZER_INTROSPECTION_HPP_INCLUDED
#include "introspectionBase.hpp"
#include "strus/queryAnalyzerInterface.hpp"
#include "strus/errorBufferInterface.hpp"

namespace strus {
namespace bindings {

class QueryAnalyzerIntrospection
	:public IntrospectionBase
{
public:
	QueryAnalyzerIntrospection(
			ErrorBufferInterface* errorhnd_,
			const QueryAnalyzerInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		{}
	virtual ~QueryAnalyzerIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const QueryAnalyzerInterface* m_impl;
};

}}//namespace
#endif

