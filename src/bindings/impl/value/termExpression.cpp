/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/termExpression.hpp"
#include <cstring>

using namespace strus;
using namespace strus::bindings;

void TermExpression::pushField( const std::string& fieldtype, const std::string& value)
{
	typedef QueryAnalyzerStruct::GroupOperatorList GroupOperatorList;

	m_fieldar.push_back( fieldtype);
	m_analyzer->putField( m_fieldar.size(), fieldtype, value);
	m_fieldno_stack.push_back( m_fieldar.size());
	const GroupOperatorList& gop = m_analyzerStruct->autoGroupOperators( fieldtype);
	if (!gop.empty())
	{
		std::vector<int> fieldnoList( m_fieldno_stack.end()-1, m_fieldno_stack.end());
		GroupOperatorList::const_iterator gi = gop.begin(), ge = gop.end();
		for (; gi != ge; ++gi)
		{
			int groupid = newOperator( gi->opr.name, gi->opr.range, gi->opr.cardinality);
			m_analyzer->groupElements( groupid, fieldnoList, gi->groupBy, gi->groupSingle);
		}
	}
}

void TermExpression::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	if (m_fieldno_stack.size() < argc) throw std::runtime_error( _TXT("push expression without all arguments defined"));
	int* fnstart = m_fieldno_stack.data() + m_fieldno_stack.size() - argc;
	int* fnend = fnstart + argc;
	std::vector<int> fieldnoList( fnstart, fnend);

	int groupid = newOperator( op, range, cardinality);
	QueryAnalyzerContextInterface::GroupBy how = QueryAnalyzerContextInterface::GroupAll;
	m_analyzer->groupElements( groupid, fieldnoList, how, true/*groupSingle*/);
	m_fieldno_stack.resize( m_fieldno_stack.size() - argc + 1);
}

static QueryAnalyzerContextInterface::GroupBy groupByOpFromName( const char* name)
{
	QueryAnalyzerContextInterface::GroupBy rt = QueryAnalyzerContextInterface::GroupAll;
	if (!name)
	{}
	else if (strus::caseInsensitiveEquals( name, "pos"))
	{
		rt = QueryAnalyzerContextInterface::GroupByPosition;
	}
	else if (strus::caseInsensitiveEquals( name, "every"))
	{
		rt = QueryAnalyzerContextInterface::GroupEvery;
	}
	else if (strus::caseInsensitiveEquals( name, "all"))
	{
		rt = QueryAnalyzerContextInterface::GroupAll;
	}
	else if (strus::caseInsensitiveEquals( name, "unique"))
	{
		rt = QueryAnalyzerContextInterface::GroupUnique;
	}
	else
	{
		throw strus::runtime_error(_TXT("unknown group by operator '%s'"), name);
	}
	return rt;
}

void TermExpression::groupBy( const std::vector<std::string>& fieldtypes, const std::string& op, const char* howstr)
{
	QueryAnalyzerContextInterface::GroupBy how = groupByOpFromName( howstr);
	std::vector<int> fieldnoList;
	std::vector<std::string>::const_iterator fi = m_fieldar.begin(), fe = m_fieldar.end();
	for (int fidx=1; fi != fe; ++fi,++fidx)
	{
		std::vector<std::string>::const_iterator ti = fieldtypes.begin(), te = fieldtypes.end();
		for (; ti != te; ++ti)
		{
			if (*ti == *fi)
			{
				fieldnoList.push_back( fidx);
				break;
			}
		}
	}
	int groupid = newOperator( op, 0/*range*/, 0/*cardinality*/);
	m_analyzer->groupElements( groupid, fieldnoList, how, true/*groupSingle*/);
}

void TermExpression::attachVariable( const std::string& name)
{
	if (m_fieldno_stack.empty()) throw std::runtime_error( _TXT("attach variable not allowed without query fields defined"));
	std::vector<int> fieldnoList( m_fieldno_stack.end()-1, m_fieldno_stack.end());

	int groupid = newVariable( name);
	QueryAnalyzerContextInterface::GroupBy how = QueryAnalyzerContextInterface::GroupEvery;
	m_analyzer->groupElements( groupid, fieldnoList, how, true/*groupSingle*/);
}




