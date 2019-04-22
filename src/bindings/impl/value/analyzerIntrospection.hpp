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
#include "strus/documentAnalyzerInstanceInterface.hpp"
#include "strus/queryAnalyzerInstanceInterface.hpp"
#include "strus/contentStatisticsInterface.hpp"
#include "strus/analyzer/contentStatisticsView.hpp"
#include "strus/analyzer/documentAnalyzerView.hpp"
#include "strus/analyzer/queryAnalyzerView.hpp"
#include "strus/analyzer/documentAnalyzerView.hpp"
#include "strus/errorBufferInterface.hpp"

namespace strus {
namespace bindings {

class DocumentAnalyzerIntrospection
	:public IntrospectionBase
{
public:
	DocumentAnalyzerIntrospection(
			ErrorBufferInterface* errorhnd_,
			const DocumentAnalyzerInstanceInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_view(impl_->view())
		{}
	virtual ~DocumentAnalyzerIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path, bool substructure);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const DocumentAnalyzerInstanceInterface* m_impl;
	analyzer::DocumentAnalyzerView m_view;
};

class QueryAnalyzerIntrospection
	:public IntrospectionBase
{
public:
	QueryAnalyzerIntrospection(
			ErrorBufferInterface* errorhnd_,
			const QueryAnalyzerInstanceInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_view(impl_->view())
		{}
	virtual ~QueryAnalyzerIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path, bool substructure);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const QueryAnalyzerInstanceInterface* m_impl;
	analyzer::QueryAnalyzerView m_view;
};

class ContentStatisticsIntrospection
	:public IntrospectionBase
{
public:
	ContentStatisticsIntrospection(
			ErrorBufferInterface* errorhnd_,
			const ContentStatisticsInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		,m_view(impl_->view())
		{}
	virtual ~ContentStatisticsIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path, bool substructure);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const ContentStatisticsInterface* m_impl;
	analyzer::ContentStatisticsView m_view;
};

}}//namespace
#endif

