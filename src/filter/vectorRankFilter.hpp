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
#ifndef _STRUS_BINDING_VECTORRANK_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_VECTORRANK_FILTER_HPP_INCLUDED
/// \file vectorRankFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/vectorStorageSearchInterface.hpp"


/// \brief strus toplevel namespace
namespace strus {

class VectorRankFilter
	:public BindingFilterInterface
{
public:
	VectorRankFilter();
	VectorRankFilter( const VectorRankFilter& o);
	explicit VectorRankFilter( const VectorStorageSearchInterface::Result* impl);
	VectorRankFilter( VectorStorageSearchInterface::Result* impl, bool withOwnership);

	virtual ~VectorRankFilter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const VectorStorageSearchInterface::Result* m_impl;
	VectorStorageSearchInterface::Result* m_ownership;
	unsigned int m_state;
};


class VectorRankVectorFilter
	:public BindingFilterInterface
{
public:
	VectorRankVectorFilter();
	VectorRankVectorFilter( const VectorRankVectorFilter& o);
	explicit VectorRankVectorFilter( const std::vector<VectorStorageSearchInterface::Result>* impl);
	VectorRankVectorFilter( std::vector<VectorStorageSearchInterface::Result>* impl, bool withOwnership);
	virtual ~VectorRankVectorFilter();

	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	const std::vector<VectorStorageSearchInterface::Result>* m_impl;
	std::vector<VectorStorageSearchInterface::Result>* m_ownership;
	unsigned int m_state;
	unsigned int m_index;
};

}//namespace
#endif
