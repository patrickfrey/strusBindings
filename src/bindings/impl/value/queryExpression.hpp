/*
 * Copyright (c) 2020 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_QUERY_EXPRESSION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_QUERY_EXPRESSION_HPP_INCLUDED
/// \brief Query parts collected in the process of query analysis and preliminary query evaluation steps
#include "strus/sentenceLexerInstanceInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storage/summaryElement.hpp"
#include "impl/value/termExpression.hpp"
#include "structDefs.hpp"
#include "papuga/typedefs.h"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \brief Query parts collected in the process of query analysis and preliminary query evaluation steps
class QueryExpression
{
public:
	explicit QueryExpression( const papuga_ValueVariant& config_);

	void setMaxNofRanks( int maxNofRanks_)				{m_maxNofRanks = maxNofRanks_;}
	void setMinRank( int minRank_)					{m_minRank = minRank_;}

	void addFeature( const papuga_ValueVariant& feature);
	void addRestriction( const papuga_ValueVariant& restriction);
	void addCollectSummary( const std::vector<SummaryElement>& summary);

	const papuga_Serialization* getFeatures() const			{return &m_featureSerialization;}
	const papuga_Serialization* getRestrictions() const		{return &m_restrictionSerialization;}

private:
#if __cplusplus >= 201103L
	QueryExpression( const QueryExpression& o) = delete;
#endif

private:
	int m_minRank;
	int m_maxNofRanks;
	QueryBuilderConfig m_config;
	papuga_Serialization m_featureSerialization;
	papuga_Serialization m_restrictionSerialization;
	papuga_Allocator m_allocator;
	int m_allocatorMem[ 1024];
	std::vector<std::string> m_nearDocids;
	std::vector<WeightedSentenceTerm> m_weightedTerms;
};

}}//namespace
#endif

