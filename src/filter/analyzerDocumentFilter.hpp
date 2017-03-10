/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_ANALYZER_DOCUMENT_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_ANALYZER_DOCUMENT_FILTER_HPP_INCLUDED
/// \brief Iterator on the structure of an analyzer document created as result of a document analysis
/// \file analyzerDocumentFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/reference.hpp"
#include "strus/binding/valueVariant.hpp"

/// \brief strus toplevel namespace
namespace strus {

/// \brief Iterator on the structure of an analyzer document created as result of a document analysis
class AnalyzerDocumentFilter
	:public BindingFilterInterface
{
public:
	/// \brief Default constructor
	explicit AnalyzerDocumentFilter( const analyzer::Document& impl)
		:m_state(StateSubDocType)
		,m_tag(BindingFilterInterface::Open)
		,m_idx(0)
		,m_impl( new analyzer::Document(impl)){}

	/// \brief Copy constructor
	AnalyzerDocumentFilter( const AnalyzerDocumentFilter& o)
		:m_state(o.m_state)
		,m_tag(o.m_tag)
		,m_idx(o.m_idx)
		,m_impl(o.m_impl){}


	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy( BindingFilterMem* mem) const;

private:
	const char* getCurrentTagName();
	ValueVariant getCurrentValue();

private:
	enum State {
		StateSubDocType,
		StateMetaData,
		StateAttributes,
		StateSearchIndexTerms,
		StateForwardIndexTerms,
		StateEnd
	};
	State m_state;
	BindingFilterInterface::Tag m_tag;
	std::size_t m_idx;
	Reference<analyzer::Document> m_impl;
};

}//namespace
#endif
