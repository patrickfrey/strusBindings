/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_QUERY_ANALYZER_STRUCT_HPP_INCLUDED
#define _STRUS_BINDING_QUERY_ANALYZER_STRUCT_HPP_INCLUDED
#include <vector>
#include <string>
#include <map>
#include "utils.hpp"

namespace strus {
namespace bindings {

class QueryAnalyzerStruct
{
public:
	QueryAnalyzerStruct()
		:m_groupmap(){}
	QueryAnalyzerStruct( const QueryAnalyzerStruct& o)
		:m_groupmap(o.m_groupmap){}

	struct Operator
	{
		std::string opname;
		unsigned int argc;
		int range;
		unsigned int cardinality;
		bool variable;

		Operator( const std::string& opname_, unsigned int argc_, int range_, unsigned int cardinality_, bool variable_)
			:opname(opname_),argc(argc_),range(range_),cardinality(cardinality_),variable(variable_){}
		Operator( const Operator& o)
			:opname(o.opname),argc(o.argc),range(o.range),cardinality(o.cardinality),variable(o.variable){}
	};
	struct GroupOperator
	{
		Operator opr;
		QueryAnalyzerContextInterface::GroupBy groupBy;
		bool groupSingle;

		GroupOperator( const Operator& opr_, QueryAnalyzerContextInterface::GroupBy groupBy_, bool groupSingle_)
			:opr(opr_),groupBy(groupBy_),groupSingle(groupSingle_){}
		GroupOperator( const GroupOperator& o)
			:opr(o.opr),groupBy(o.groupBy),groupSingle(o.groupSingle){}
	};
	typedef std::vector<GroupOperator> GroupOperatorList;

	void autoGroupBy( const std::string& fieldtype_, const std::string& opname, int range, unsigned int cardinality, QueryAnalyzerContextInterface::GroupBy groupBy, bool groupSingle)
	{
		std::string fieldtype = utils::tolower( fieldtype_);
		GroupOperator gop( Operator( opname, 0, range, cardinality, false/*variable*/), groupBy, groupSingle);
		GroupMap::iterator gi = m_groupmap.find( fieldtype);
		if (gi == m_groupmap.end())
		{
			m_groupmap[ fieldtype].push_back( gop);
		}
		else
		{
			gi->second.push_back( gop);
		}
	}

	const GroupOperatorList& autoGroupOperators( const std::string& fieldtype_) const
	{
		std::string fieldtype = utils::tolower( fieldtype_);
		static const std::vector<GroupOperator> empty;
		GroupMap::const_iterator gi = m_groupmap.find( fieldtype);
		if (gi == m_groupmap.end())
		{
			return empty;
		}
		else
		{
			return gi->second;
		}
	}

private:
	typedef std::map<std::string,GroupOperatorList> GroupMap;
	GroupMap m_groupmap;
};

}}//namespace
#endif

