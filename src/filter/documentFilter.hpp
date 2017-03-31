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
#ifndef _STRUS_BINDING_DOCUMENT_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_DOCUMENT_FILTER_HPP_INCLUDED
/// \file documentFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/analyzer/metaData.hpp"
#include "strus/analyzer/attribute.hpp"
#include "strus/analyzer/term.hpp"

#include <vector>

/// \brief strus toplevel namespace
namespace strus {

class DocumentFilter
	:public BindingFilterInterface
{
public:
	DocumentFilter();
	DocumentFilter( const DocumentFilter& o);
	explicit DocumentFilter( const analyzer::Document* impl);
	DocumentFilter( analyzer::Document* impl, bool withOwnership);

	virtual ~DocumentFilter();
	virtual Tag getNext( bindings::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	enum {MaxDepth=8};

private:
	const analyzer::Document* m_impl;
	analyzer::Document* m_ownership;
	unsigned int m_state;
	unsigned int m_index[ MaxDepth];
};

}//namespace
#endif
