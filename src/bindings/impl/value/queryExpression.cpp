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
#include "strus/base/localErrorBuffer.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "papuga/serialization.h"
#include "papuga/allocator.h"
#include "papuga/valueVariant.h"
#include "papuga/errors.h"
#include <limits>

using namespace strus;
using namespace strus::bindings;

QueryExpression::QueryExpression( const papuga_ValueVariant& config_)
	:m_minRank(0),m_maxNofRanks(QueryInterface::DefaultMaxNofRanks),m_config(config_)
	,m_nearDocids(),m_weightedTerms(),m_termar(),m_exprar(),m_nodear()
	,m_freenodear(),m_featar(),m_variablear()
{
	papuga_init_Allocator( &m_allocator, &m_allocatorMem, sizeof(m_allocatorMem));
	papuga_init_Serialization( &m_restrictionSerialization, &m_allocator);
}

QueryExpression::~QueryExpression()
{
	papuga_destroy_Allocator( &m_allocator);
}

void QueryExpression::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	m_freenodear.push_back( m_nodear.size());
	m_nodear.push_back( Node( Node::TermType, m_termar.size(), -1/*next*/));
	m_termar.push_back( Term( type, value, length));
}

void QueryExpression::pushTerm( const std::string& type, const std::string& value)
{
	m_freenodear.push_back( m_nodear.size());
	m_nodear.push_back( Node( Node::TermType, m_termar.size(), -1/*next*/));
	m_termar.push_back( Term( type, value, 1/*length*/));
}

void QueryExpression::pushTerm( const std::string& type)
{
	m_freenodear.push_back( m_nodear.size());
	m_nodear.push_back( Node( Node::TermType, m_termar.size(), -1/*next*/));
	m_termar.push_back( Term( type, ""/*value*/, 1/*length*/));
}

void QueryExpression::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	if (argc > m_freenodear.size()) throw std::runtime_error(_TXT("logic error: push expression number of arguments exceeds number of elements on the stack"));
	int ni = m_freenodear.size() - argc;
	for (int ai=1,ae=argc; ai < ae; ++ai)
	{
		m_nodear[ m_freenodear[ ni+ai-1]].next = m_freenodear[ ni+ai];
	}
	int nodeidx = argc == 0 ? -1 : m_freenodear[ ni];
	m_freenodear.resize( ni);
	m_freenodear.push_back( m_nodear.size());
	m_nodear.push_back( Node( Node::ExpressionType, m_exprar.size(), -1/*next*/));
	m_exprar.push_back( Expression( op, argc, range, cardinality, nodeidx));
}

void QueryExpression::attachVariable( const std::string& name)
{
	if (m_freenodear.empty())
	{
		throw std::runtime_error(_TXT("logic error: attach variable without node defined"));
	}
	std::vector<std::string>::const_iterator vi = std::find( m_variablear.begin(), m_variablear.end(), name);
	if (vi == m_variablear.end())
	{
		m_nodear[ m_freenodear.back()].varidx = m_variablear.size();
		m_variablear.push_back( name);
	}
	else
	{
		m_nodear[ m_freenodear.back()].varidx = vi - m_variablear.begin();
	}
}

void QueryExpression::definePattern( const std::string& name, const std::string& formatstring, bool visible)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "define pattern", "query");
}

void QueryExpression::defineFeature( const std::string& featureSet, double weight)
{
	if (m_freenodear.empty())
	{
		throw std::runtime_error(_TXT("logic error: define feature without node defined"));
	}
	int nidx = m_freenodear.back();
	m_freenodear.pop_back();
	m_featar.push_back( Feature( featureSet, weight, nidx));
}

void QueryExpression::addFeature( const std::string& set, const papuga_ValueVariant& expr, double weight)
{
	LocalErrorBuffer errorhnd;
	Deserializer::buildQueryFeatures( *this, set, weight, expr, &errorhnd);
	if (errorhnd.hasError()) throw std::runtime_error( errorhnd.fetchError());
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

void QueryExpression::fillWeightedQueryTerms( std::vector<WeightedSentenceTerm>& weightedQueryTerms, const Node& nd, double ww) const
{
	switch (nd.type)
	{
		case Node::TermType:
		{
			const Term& term = m_termar[ nd.idx];
			weightedQueryTerms.push_back( WeightedSentenceTerm( term.type, term.value, ww));
			break;
		}
		case Node::ExpressionType:
		{
			const Expression& expr = m_exprar[ nd.idx];
			int ai=0, ae=expr.argc;
			int ni = expr.nodeidx;
			for (; ai != ae; ++ai)
			{
				const Node& chldnd = m_nodear[ ni];
				fillWeightedQueryTerms( weightedQueryTerms, chldnd, ww / expr.argc);
				ni = chldnd.next;
			}
			if (ni != -1) throw std::runtime_error(_TXT("internal: corrupt query expression"));
			break;
		}
	}
}

void QueryExpression::serializeNode( papuga_Serialization* serialization, const Node& nd, bool mapTypes) const
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( serialization);
	switch (nd.type)
	{
		case Node::TermType:
		{
			rt &= papuga_Serialization_pushName_charp( serialization, "term");
			rt &= papuga_Serialization_pushOpen( serialization);
			const Term& term = m_termar[ nd.idx];
			if (nd.varidx >= 0)
			{
				rt &= papuga_Serialization_pushName_charp( serialization, "variable");
				Serializer::serialize( serialization, m_variablear[ nd.varidx], true/*deep*/);
			}
			rt &= papuga_Serialization_pushName_charp( serialization, "type");
			if (mapTypes)
			{
				const std::string& type = m_config.queryFeatureRewriteType( term.type);
				Serializer::serialize( serialization, type, true/*deep*/);
			}
			else
			{
				Serializer::serialize( serialization, term.type, true/*deep*/);
			}
			rt &= papuga_Serialization_pushName_charp( serialization, "value");
			Serializer::serialize( serialization, term.value, true/*deep*/);
			if (term.length > 1)
			{
				rt &= papuga_Serialization_pushName_charp( serialization, "len");
				rt &= papuga_Serialization_pushValue_int( serialization, term.length);
			}
			rt &= papuga_Serialization_pushClose( serialization);
			break;
		}
		case Node::ExpressionType:
		{
			rt &= papuga_Serialization_pushName_charp( serialization, "expression");
			rt &= papuga_Serialization_pushOpen( serialization);

			const Expression& expr = m_exprar[ nd.idx];
			if (nd.varidx >= 0)
			{
				rt &= papuga_Serialization_pushName_charp( serialization, "variable");
				Serializer::serialize( serialization, m_variablear[ nd.varidx], true/*deep*/);
			}
			rt &= papuga_Serialization_pushName_charp( serialization, "op");
			Serializer::serialize( serialization, expr.op, true/*deep*/);
			if (expr.range)
			{
				rt &= papuga_Serialization_pushName_charp( serialization, "range");
				rt &= papuga_Serialization_pushValue_int( serialization, expr.range);
			}
			if (expr.cardinality)
			{
				rt &= papuga_Serialization_pushName_charp( serialization, "cardinality");
				rt &= papuga_Serialization_pushValue_int( serialization, expr.cardinality);
			}
			rt &= papuga_Serialization_pushName_charp( serialization, "arg");
			rt &= papuga_Serialization_pushOpen( serialization);
			int ai=0, ae=expr.argc;
			int ni = expr.nodeidx;
			for (; ai != ae; ++ai)
			{
				const Node& chld = m_nodear[ ni];
				serializeNode( serialization, chld, mapTypes);
				ni = chld.next;
			}
			rt &= papuga_Serialization_pushClose( serialization);

			if (ni != -1) throw std::runtime_error(_TXT("internal: corrupt query expression"));
			rt &= papuga_Serialization_pushClose( serialization);
			break;
		}
	}
	rt &= papuga_Serialization_pushClose( serialization);
	if (!rt) throw std::bad_alloc();
}

void QueryExpression::serializeFeature( papuga_Serialization* serialization, const Feature& feature, bool mapTypes) const
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( serialization);
	rt &= papuga_Serialization_pushName_charp( serialization, "set");
	Serializer::serialize( serialization, feature.set, true/*deep*/);
	rt &= papuga_Serialization_pushName_charp( serialization, "weight");
	rt &= papuga_Serialization_pushValue_double( serialization, feature.weight);
	rt &= papuga_Serialization_pushName_charp( serialization, "analyzed");
	serializeNode( serialization, m_nodear[ feature.nodeidx], mapTypes);
	rt &= papuga_Serialization_pushClose( serialization);
	if (!rt) throw std::bad_alloc();
}

void QueryExpression::serializeFeatures( papuga_Serialization* ser, bool mapTypes) const
{
	std::vector<Feature>::const_iterator fi = m_featar.begin(), fe = m_featar.end();
	for (; fi != fe; ++fi)
	{
		serializeFeature( ser, *fi, mapTypes);
	}
}

void QueryExpression::serializeWeightedTerms( papuga_Serialization* ser) const
{
	std::vector<WeightedSentenceTerm> weightedQueryTerms;
	std::vector<Feature>::const_iterator fi = m_featar.begin(), fe = m_featar.end();
	for (; fi != fe; ++fi)
	{
		fillWeightedQueryTerms( weightedQueryTerms, m_nodear[ fi->nodeidx], 1.0);
	}
	Serializer::serialize( ser, m_weightedTerms, true/*deep*/);
	Serializer::serialize( ser, weightedQueryTerms, true/*deep*/);
}



