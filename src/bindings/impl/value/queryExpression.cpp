/*
 * Copyright (c) 2020 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Query parts collected in the process of query analysis and preliminary query evaluation steps
#include "impl/value/queryExpression.hpp"
#include "private/internationalization.hpp"
#include "strus/constants.hpp"
#include "strus/queryInterface.hpp"
#include "papuga/serialization.h"
#include "papuga/allocator.h"
#include "papuga/valueVariant.h"
#include "papuga/errors.h"

using namespace strus;
using namespace strus::bindings;

QueryExpression::QueryExpression( const papuga_ValueVariant& config_)
	:m_minRank(0),m_maxNofRanks(QueryInterface::DefaultMaxNofRanks),m_config(config_)
{
	papuga_init_Allocator( &m_allocator, &m_allocatorMem, sizeof(m_allocatorMem));
	papuga_init_Serialization( &m_featureSerialization, &m_allocator);
	papuga_init_Serialization( &m_restrictionSerialization, &m_allocator);
}

void QueryExpression::addFeature( const papuga_ValueVariant& feature)
{
	if (!papuga_ValueVariant_defined( &feature)) return;
	if (feature.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("structure expected as argument of '%s'"), "QueryExpression::addExpression");
	}
	papuga_ValueVariant feature_copy;
	papuga_ErrorCode errcode = papuga_Ok;
	if (!papuga_Allocator_deepcopy_value( &m_allocator, &feature_copy, const_cast<papuga_ValueVariant*>(&feature)/*unchanged*/, false/*no host objects expected*/, &errcode))
	{
		throw strus::runtime_error(_TXT("failed '%s': %s"), "QueryExpression::addExpression", papuga_ErrorCode_tostring( errcode));
	}
	papuga_Serialization_pushValue( &m_featureSerialization, &feature_copy);
}

void QueryExpression::addRestriction( const papuga_ValueVariant& restriction)
{
	if (!papuga_ValueVariant_defined( &restriction)) return;
	if (restriction.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("structure expected as argument of '%s'"), "QueryExpression::addExpression");
	}
	papuga_ValueVariant restriction_copy;
	papuga_ErrorCode errcode = papuga_Ok;
	if (!papuga_Allocator_deepcopy_value( &m_allocator, &restriction_copy, const_cast<papuga_ValueVariant*>(&restriction)/*unchanged*/, false/*no host objects expected*/, &errcode))
	{
		throw strus::runtime_error(_TXT("failed '%s': %s"), "QueryExpression::addExpression", papuga_ErrorCode_tostring( errcode));
	}
	papuga_Serialization_pushValue( &m_restrictionSerialization, &restriction_copy);
}

void QueryExpression::addCollectSummary( const std::vector<SummaryElement>& summary)
{
	std::vector<SummaryElement>::const_iterator si = summary.begin(), se = summary.end();
	for (; si != se; ++si)
	{
		const char* text = si->value().c_str();
		if (si->name() == m_config.expandSummaryName)
		{
			if (m_config.extractFeatureType.empty())
			{
				if (m_config.extractFeatureTypeValueSeparator)
				{
					char const* zi = std::strchr( text, m_config.extractFeatureTypeValueSeparator);
					if (zi)
					{
						std::string type( text, zi-text);
						std::string value( zi+1);
						m_weightedTerms.push_back( WeightedSentenceTerm( type, value, si->weight()));
					}
				}
			}
			else if (m_config.extractFeatureTypeValueSeparator)
			{
				char const* zi = std::strchr( text, m_config.extractFeatureTypeValueSeparator);
				if (zi)
				{
					m_weightedTerms.push_back( WeightedSentenceTerm( m_config.extractFeatureType, zi+1, si->weight()));
				}
			}
			else
			{
				m_weightedTerms.push_back( WeightedSentenceTerm( m_config.extractFeatureType, text, si->weight()));
			}
		}
		else if (si->name() == m_config.docidSummaryName)
		{
			m_nearDocids.push_back( si->value());
		}
	}
}


