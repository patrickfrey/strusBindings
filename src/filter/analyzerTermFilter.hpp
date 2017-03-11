/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_ANALYZER_TERM_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_ANALYZER_TERM_FILTER_HPP_INCLUDED
/// \file analyzerTermFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/term.hpp"
#include "strus/reference.hpp"
#include "stateTable.hpp"
#include "structElementArray.hpp"

/// \brief strus toplevel namespace
namespace strus {

struct AnalyzerTermAccess
{
	static binding::ValueVariant get( unsigned int state, const analyzer::Term& st);
};

class AnalyzerTermFilter
	:public BindingFilterInterface
{
public:
	AnalyzerTermFilter();
	AnalyzerTermFilter( const AnalyzerTermFilter& o);
	explicit AnalyzerTermFilter( const analyzer::Term* impl);
	AnalyzerTermFilter( analyzer::Term* impl, bool withOwnership);

	virtual ~AnalyzerTermFilter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const analyzer::Term* m_impl;
	analyzer::Term* m_ownership;
	unsigned int m_state;
};


class AnalyzerTermVectorFilter
	:public BindingFilterInterface
{
public:
	AnalyzerTermVectorFilter();
	AnalyzerTermVectorFilter( const AnalyzerTermVectorFilter& o);
	explicit AnalyzerTermVectorFilter( const std::vector<analyzer::Term>* impl);
	AnalyzerTermVectorFilter( std::vector<analyzer::Term>* impl, bool withOwnership);
	virtual ~AnalyzerTermVectorFilter();

	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	const std::vector<analyzer::Term>* m_impl;
	std::vector<analyzer::Term>* m_ownership;
	unsigned int m_state;
	unsigned int m_index;
};

}//namespace
#endif

