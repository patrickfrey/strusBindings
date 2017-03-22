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
#ifndef _STRUS_BINDING_METADATA_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_METADATA_FILTER_HPP_INCLUDED
/// \file metaDataFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/metaData.hpp"


/// \brief strus toplevel namespace
namespace strus {

class MetaDataFilter
	:public BindingFilterInterface
{
public:
	MetaDataFilter();
	MetaDataFilter( const MetaDataFilter& o);
	explicit MetaDataFilter( const analyzer::MetaData* impl);
	MetaDataFilter( analyzer::MetaData* impl, bool withOwnership);

	virtual ~MetaDataFilter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const analyzer::MetaData* m_impl;
	analyzer::MetaData* m_ownership;
	unsigned int m_state;
};


class MetaDataVectorFilter
	:public BindingFilterInterface
{
public:
	MetaDataVectorFilter();
	MetaDataVectorFilter( const MetaDataVectorFilter& o);
	explicit MetaDataVectorFilter( const std::vector<analyzer::MetaData>* impl);
	MetaDataVectorFilter( std::vector<analyzer::MetaData>* impl, bool withOwnership);
	virtual ~MetaDataVectorFilter();

	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	const std::vector<analyzer::MetaData>* m_impl;
	std::vector<analyzer::MetaData>* m_ownership;
	unsigned int m_state;
	unsigned int m_index;
};

}//namespace
#endif
