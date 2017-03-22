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
#ifndef _STRUS_BINDING_DOCUMENTFREQUENCYCHANGE_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_DOCUMENTFREQUENCYCHANGE_FILTER_HPP_INCLUDED
/// \file documentFrequencyChangeFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/bindings/statisticsMessage.hpp"


/// \brief strus toplevel namespace
namespace strus {

class DocumentFrequencyChangeFilter
	:public BindingFilterInterface
{
public:
	DocumentFrequencyChangeFilter();
	DocumentFrequencyChangeFilter( const DocumentFrequencyChangeFilter& o);
	explicit DocumentFrequencyChangeFilter( const bindings::DocumentFrequencyChange* impl);
	DocumentFrequencyChangeFilter( bindings::DocumentFrequencyChange* impl, bool withOwnership);

	virtual ~DocumentFrequencyChangeFilter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const bindings::DocumentFrequencyChange* m_impl;
	bindings::DocumentFrequencyChange* m_ownership;
	unsigned int m_state;
};


class DocumentFrequencyChangeVectorFilter
	:public BindingFilterInterface
{
public:
	DocumentFrequencyChangeVectorFilter();
	DocumentFrequencyChangeVectorFilter( const DocumentFrequencyChangeVectorFilter& o);
	explicit DocumentFrequencyChangeVectorFilter( const std::vector<bindings::DocumentFrequencyChange>* impl);
	DocumentFrequencyChangeVectorFilter( std::vector<bindings::DocumentFrequencyChange>* impl, bool withOwnership);
	virtual ~DocumentFrequencyChangeVectorFilter();

	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	const std::vector<bindings::DocumentFrequencyChange>* m_impl;
	std::vector<bindings::DocumentFrequencyChange>* m_ownership;
	unsigned int m_state;
	unsigned int m_index;
};

}//namespace
#endif
