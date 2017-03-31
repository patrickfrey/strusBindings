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
#ifndef _STRUS_BINDING_TERMARRAY_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_TERMARRAY_FILTER_HPP_INCLUDED
/// \file termArrayFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/term.hpp"

#include <vector>

/// \brief strus toplevel namespace
namespace strus {

class TermArrayFilter
	:public BindingFilterInterface
{
public:
	TermArrayFilter();
	TermArrayFilter( const TermArrayFilter& o);
	explicit TermArrayFilter( const std::vector<analyzer::Term>* impl);
	TermArrayFilter( std::vector<analyzer::Term>* impl, bool withOwnership);

	virtual ~TermArrayFilter();
	virtual Tag getNext( bindings::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	enum {MaxDepth=8};

private:
	const std::vector<analyzer::Term>* m_impl;
	std::vector<analyzer::Term>* m_ownership;
	unsigned int m_state;
	unsigned int m_index[ MaxDepth];
};

}//namespace
#endif
