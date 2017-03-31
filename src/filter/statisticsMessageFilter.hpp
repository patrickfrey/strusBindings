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
#ifndef _STRUS_BINDING_STATISTICSMESSAGE_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_STATISTICSMESSAGE_FILTER_HPP_INCLUDED
/// \file statisticsMessageFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/bindings/statisticsMessage.hpp"

#include <vector>

/// \brief strus toplevel namespace
namespace strus {

class StatisticsMessageFilter
	:public BindingFilterInterface
{
public:
	StatisticsMessageFilter();
	StatisticsMessageFilter( const StatisticsMessageFilter& o);
	explicit StatisticsMessageFilter( const bindings::StatisticsMessage* impl);
	StatisticsMessageFilter( bindings::StatisticsMessage* impl, bool withOwnership);

	virtual ~StatisticsMessageFilter();
	virtual Tag getNext( bindings::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	enum {MaxDepth=8};

private:
	const bindings::StatisticsMessage* m_impl;
	bindings::StatisticsMessage* m_ownership;
	unsigned int m_state;
	unsigned int m_index[ MaxDepth];
};

}//namespace
#endif
