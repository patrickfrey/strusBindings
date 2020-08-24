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
#include "expressionBuilder.hpp"
#include "papuga/typedefs.h"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \brief Query parts collected in the process of query analysis and preliminary query evaluation steps
class QueryExpression
	:public ExpressionBuilder
{
public:
	virtual ~QueryExpression();
	QueryExpression();

	void setMaxNofRanks( int maxNofRanks_)				{m_maxNofRanks = maxNofRanks_;}
	void setMinRank( int minRank_)					{m_minRank = minRank_;}

	void addFeature( const std::string& set, const papuga_ValueVariant& expr, double weight);
	void addRestriction( const papuga_ValueVariant& restriction);
	void addCollectSummary( const std::vector<SummaryElement>& summary);

	void serializeFeatures( papuga_Serialization* ser, bool mapTypes) const;
	void serializeCollectedTerms( papuga_Serialization* ser) const;

	const papuga_Serialization* getRestrictions() const		{return &m_restrictionSerialization;}

public:/*implement ExpressionBuilder*/
	virtual void pushTerm( const std::string& type, const std::string& value, unsigned int length);
	virtual void pushTerm( const std::string& type, const std::string& value);
	virtual void pushTerm( const std::string& type);
	virtual void pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality);
	virtual void attachVariable( const std::string& name);
	virtual void definePattern( const std::string& name, const std::string& formatstring, bool visible);
	virtual void defineFeature( const std::string& featureSet, double weight);

private:
#if __cplusplus >= 201103L
	QueryExpression( const QueryExpression& o) = delete;
#endif
private:
	struct Term
	{
		std::string type;
		std::string value;
		int length;

		Term( const std::string& type_, const std::string& value_, int length_)
			:type(type_),value(value_),length(length_){}
		Term( const Term& o)
			:type(o.type),value(o.value),length(o.length){}
	};
	struct Expression
	{
		std::string op;
		int argc;
		int range;
		int cardinality;
		int nodeidx;

		Expression( const std::string& op_, int argc_, int range_, int cardinality_, int nodeidx_)
			:op(op_),argc(argc_),range(range_),cardinality(cardinality_),nodeidx(nodeidx_){}
		Expression( const Expression& o)
			:op(o.op),argc(o.argc),range(o.range),cardinality(o.cardinality),nodeidx(o.nodeidx){}
	};
	struct Node
	{
		enum Type {TermType, ExpressionType};
		Type type;
		int idx;
		int varidx;
		int next;

		Node( Type type_, int idx_, int next_)
			:type(type_),idx(idx_),varidx(-1),next(next_){}
		Node( const Node& o)
			:type(o.type),idx(o.idx),varidx(o.varidx),next(o.next){}
	};
	struct Feature
	{
		std::string set;
		double weight;
		int nodeidx;

		Feature( const std::string& set_, double weight_, int nodeidx_)
			:set(set_),weight(weight_),nodeidx(nodeidx_){}
		Feature( const Feature& o)
			:set(o.set),weight(o.weight),nodeidx(o.nodeidx){}
	};

private:
	void fillWeightedQueryTerms( std::vector<WeightedSentenceTerm>& weightedQueryTerms, const Node& nd, double ww) const;
	void serializeFeature( papuga_Serialization* serialization, const Feature& feature, bool mapTypes) const;
	void serializeNode( papuga_Serialization* serialization, const Node& nd, bool mapTypes) const;

	typedef std::map<std::string,double> CollectedSummaryMap;
	typedef std::map<std::string,CollectedSummaryMap> NameCollectedSummaryMapMap;

private:
	int m_minRank;
	int m_maxNofRanks;
	papuga_Serialization m_restrictionSerialization;
	papuga_Allocator m_allocator;
	int m_allocatorMem[ 1024];
	NameCollectedSummaryMapMap m_nameCollectedSummaryMapMap;
	CollectedSummaryMap m_collectedSummaryMaxWeightMap;
	std::vector<Term> m_termar;
	std::vector<Expression> m_exprar;
	std::vector<Node> m_nodear;
	std::vector<int> m_freenodear;
	std::vector<Feature> m_featar;
	std::vector<std::string> m_variablear;
};

}}//namespace
#endif

