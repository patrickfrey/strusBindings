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
#ifndef _STRUS_BINDING_VECTORRANKARRAY_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_VECTORRANKARRAY_FILTER_HPP_INCLUDED
/// \file vectorRankArrayFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/vectorStorageSearchInterface.hpp"

#include <vector>

/// \brief strus toplevel namespace
namespace strus {

class VectorRankArrayFilter
	:public BindingFilterInterface
{
public:
	VectorRankArrayFilter();
	VectorRankArrayFilter( const VectorRankArrayFilter& o);
	explicit VectorRankArrayFilter( const std::vector<VectorStorageSearchInterface::Result>* impl);
	VectorRankArrayFilter( std::vector<VectorStorageSearchInterface::Result>* impl, bool withOwnership);

	virtual ~VectorRankArrayFilter();
	virtual Tag getNext( bindings::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	enum {MaxDepth=8};

private:
	const std::vector<VectorStorageSearchInterface::Result>* m_impl;
	std::vector<VectorStorageSearchInterface::Result>* m_ownership;
	unsigned int m_state;
	unsigned int m_index[ MaxDepth];
};

}//namespace
#endif
