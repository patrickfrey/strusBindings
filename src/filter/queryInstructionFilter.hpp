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
#ifndef _STRUS_BINDING_QUERYINSTRUCTION_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_QUERYINSTRUCTION_FILTER_HPP_INCLUDED
/// \file queryInstructionFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/query.hpp"

#include <vector>

/// \brief strus toplevel namespace
namespace strus {

class QueryInstructionFilter
	:public BindingFilterInterface
{
public:
	QueryInstructionFilter();
	QueryInstructionFilter( const QueryInstructionFilter& o);
	explicit QueryInstructionFilter( const analyzer::Query::Instruction* impl);
	QueryInstructionFilter( analyzer::Query::Instruction* impl, bool withOwnership);

	virtual ~QueryInstructionFilter();
	virtual Tag getNext( bindings::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	enum {MaxDepth=8};

private:
	const analyzer::Query::Instruction* m_impl;
	analyzer::Query::Instruction* m_ownership;
	unsigned int m_state;
	unsigned int m_index[ MaxDepth];
};

}//namespace
#endif
