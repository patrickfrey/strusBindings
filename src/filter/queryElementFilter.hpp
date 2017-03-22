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
#ifndef _STRUS_BINDING_QUERYELEMENT_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_QUERYELEMENT_FILTER_HPP_INCLUDED
/// \file queryElementFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/query.hpp"


/// \brief strus toplevel namespace
namespace strus {

class QueryElementFilter
	:public BindingFilterInterface
{
public:
	QueryElementFilter();
	QueryElementFilter( const QueryElementFilter& o);
	explicit QueryElementFilter( const analyzer::Query::Element* impl);
	QueryElementFilter( analyzer::Query::Element* impl, bool withOwnership);

	virtual ~QueryElementFilter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const analyzer::Query::Element* m_impl;
	analyzer::Query::Element* m_ownership;
	unsigned int m_state;
};


class QueryElementVectorFilter
	:public BindingFilterInterface
{
public:
	QueryElementVectorFilter();
	QueryElementVectorFilter( const QueryElementVectorFilter& o);
	explicit QueryElementVectorFilter( const std::vector<analyzer::Query::Element>* impl);
	QueryElementVectorFilter( std::vector<analyzer::Query::Element>* impl, bool withOwnership);
	virtual ~QueryElementVectorFilter();

	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	const std::vector<analyzer::Query::Element>* m_impl;
	std::vector<analyzer::Query::Element>* m_ownership;
	unsigned int m_state;
	unsigned int m_index;
};

}//namespace
#endif
