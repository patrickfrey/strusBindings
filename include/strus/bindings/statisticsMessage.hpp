/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_STATISTICS_MESSAGE_HPP_INCLUDED
#define _STRUS_BINDINGS_STATISTICS_MESSAGE_HPP_INCLUDED
/// \brief Decoded storage statistics message blob
/// \file statisticsMessage.hpp
#include "strus/statisticsViewerInterface.hpp"
#include <boost/shared_ptr.hpp>

namespace strus {
namespace bindings {

typedef StatisticsViewerInterface::DocumentFrequencyChange DocumentFrequencyChange;

class StatisticsMessage
{
public:
	typedef std::vector<DocumentFrequencyChange> DfList;

	explicit StatisticsMessage( int nofdocs_=0)
		:m_dflist(),m_nofdocs(nofdocs_){}
	StatisticsMessage( const DfList& dflist_, int nofdocs_)
		:m_dflist(dflist_),m_nofdocs(nofdocs_){}
	StatisticsMessage( const StatisticsMessage& o)
		:m_dflist(o.m_dflist),m_nofdocs(o.m_nofdocs){}

	const DfList& dflist() const	{return m_dflist;}
	int nofdocs() const		{return m_nofdocs;}

private:
	DfList m_dflist;		///< list of document frequency changes
	int m_nofdocs;			///< number of document changes
};

}}//namespace
#endif


}
