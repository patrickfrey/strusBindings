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
#include "strus/documentAnalyzerInterface.hpp"
#include "strus/queryAnalyzerInterface.hpp"
#include "strus/analyzer/documentAnalyzerView.hpp"
#include "strus/analyzer/queryAnalyzerView.hpp"
#include "strus/errorBufferInterface.hpp"

namespace strus {
namespace bindings {

class DocumentAnalyzerIntrospection
	:public IntrospectionBase
{
public:
	DocumentAnalyzerIntrospection(
			ErrorBufferInterface* errorhnd_,
			const DocumentAnalyzerInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_view(impl_->view())
		{}
	virtual ~DocumentAnalyzerIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const DocumentAnalyzerInterface* m_impl;
	analyzer::DocumentAnalyzerView m_view;
};

class QueryAnalyzerIntrospection
	:public IntrospectionBase
{
public:
	QueryAnalyzerIntrospection(
			ErrorBufferInterface* errorhnd_,
			const QueryAnalyzerInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_view(impl_->view())
		{}
	virtual ~QueryAnalyzerIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const QueryAnalyzerInterface* m_impl;
	analyzer::QueryAnalyzerView m_view;
};

}}//namespace
#endif

