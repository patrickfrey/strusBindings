/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/*
 * DO NOT MODIFY THIS FILE !!!
 * 
 * This file has been generated with the script scripts/genFilters.py
 * Modifications on this file will be lost!
 */
#ifndef _STRUS_BINDING_TERM_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_TERM_FILTER_HPP_INCLUDED
/// \file termFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/term.hpp"


/// \brief strus toplevel namespace
namespace strus {

class TermFilter
	:public BindingFilterInterface
{
public:
	TermFilter();
	TermFilter( const TermFilter& o);
	explicit TermFilter( const analyzer::Term* impl);
	TermFilter( analyzer::Term* impl, bool withOwnership);

	virtual ~TermFilter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const analyzer::Term* m_impl;
	analyzer::Term* m_ownership;
	unsigned int m_state;
};


class TermVectorFilter
	:public BindingFilterInterface
{
public:
	TermVectorFilter();
	TermVectorFilter( const TermVectorFilter& o);
	explicit TermVectorFilter( const std::vector<analyzer::Term>* impl);
	TermVectorFilter( std::vector<analyzer::Term>* impl, bool withOwnership);
	virtual ~TermVectorFilter();

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
