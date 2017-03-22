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
#ifndef _STRUS_BINDING_SUMMARYELEMENT_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_SUMMARYELEMENT_FILTER_HPP_INCLUDED
/// \file summaryElementFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/summaryElement.hpp"


/// \brief strus toplevel namespace
namespace strus {

class SummaryElementFilter
	:public BindingFilterInterface
{
public:
	SummaryElementFilter();
	SummaryElementFilter( const SummaryElementFilter& o);
	explicit SummaryElementFilter( const SummaryElement* impl);
	SummaryElementFilter( SummaryElement* impl, bool withOwnership);

	virtual ~SummaryElementFilter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const SummaryElement* m_impl;
	SummaryElement* m_ownership;
	unsigned int m_state;
};


class SummaryElementVectorFilter
	:public BindingFilterInterface
{
public:
	SummaryElementVectorFilter();
	SummaryElementVectorFilter( const SummaryElementVectorFilter& o);
	explicit SummaryElementVectorFilter( const std::vector<SummaryElement>* impl);
	SummaryElementVectorFilter( std::vector<SummaryElement>* impl, bool withOwnership);
	virtual ~SummaryElementVectorFilter();

	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	const std::vector<SummaryElement>* m_impl;
	std::vector<SummaryElement>* m_ownership;
	unsigned int m_state;
	unsigned int m_index;
};

}//namespace
#endif
