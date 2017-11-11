/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_METADATA_COMPARISON_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_METADATA_COMPARISON_HPP_INCLUDED
#include "papuga/serialization.h"
#include "strus/analyzer/queryTerm.hpp"
#include <string>

namespace strus {
namespace bindings {

/// \brief Analyzed term expression structure
class MetaDataComparison
{
public:
	/// \brief Constructor
	MetaDataComparison( const char* cmpop_, const analyzer::QueryTerm* term_, bool newGroup_)
		:m_cmpop(cmpop_),m_term(term_),m_newGroup(newGroup_){}
	MetaDataComparison( const MetaDataComparison& o)
		:m_cmpop(o.m_cmpop),m_term(o.m_term),m_newGroup(o.m_newGroup){}
	~MetaDataComparison()
	{}

	const char* cmpop() const			{return m_cmpop;}
	const analyzer::QueryTerm* term() const		{return m_term;}
	bool newGroup() const				{return m_newGroup;}

	void setNewGroup( bool val)			{m_newGroup = val;}

private:
	const char* m_cmpop;
	const analyzer::QueryTerm* m_term;
	bool m_newGroup;
};

}}//namespace
#endif

