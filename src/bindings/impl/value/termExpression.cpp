/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/termExpression.hpp"

using namespace strus;
using namespace strus::bindings;

void TermExpression::pushField( const std::string& fieldtype, const std::string& value)
{
	typedef QueryAnalyzerStruct::GroupOperatorList GroupOperatorList;

	++m_fieldno_cnt;
	m_analyzer->putField( m_fieldno_cnt, fieldtype, value);
	m_fieldno_stack.push_back( m_fieldno_cnt);
	const GroupOperatorList& gop = m_analyzerStruct->autoGroupOperators( fieldtype);
	if (!gop.empty())
	{
		std::vector<unsigned int> fieldnoList( m_fieldno_stack.end()-1, m_fieldno_stack.end());
		GroupOperatorList::const_iterator gi = gop.begin(), ge = gop.end();
		for (; gi != ge; ++gi)
		{
			unsigned int groupid = newOperator( gi->opr.name, gi->opr.range, gi->opr.cardinality);
			m_analyzer->groupElements( groupid, fieldnoList, gi->groupBy, gi->groupSingle);
		}
	}
}

void TermExpression::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	if (m_fieldno_stack.size() < argc) throw strus::runtime_error( "%s", _TXT("push expression without all arguments defined"));
	unsigned int* fnstart = m_fieldno_stack.data() + m_fieldno_stack.size() - argc;
	unsigned int* fnend = fnstart + argc;
	std::vector<unsigned int> fieldnoList( fnstart, fnend);

	unsigned int groupid = newOperator( op, range, cardinality);
	QueryAnalyzerContextInterface::GroupBy groupBy = QueryAnalyzerContextInterface::GroupAll;
	m_analyzer->groupElements( groupid, fieldnoList, groupBy, true/*groupSingle*/);
	m_fieldno_stack.resize( m_fieldno_stack.size() - argc + 1);
}

void TermExpression::attachVariable( const std::string& name)
{
	if (m_fieldno_stack.empty()) throw strus::runtime_error( "%s", _TXT("attach variable not allowed without query fields defined"));
	std::vector<unsigned int> fieldnoList( m_fieldno_stack.end()-1, m_fieldno_stack.end());

	unsigned int groupid = newVariable( name);
	QueryAnalyzerContextInterface::GroupBy groupBy = QueryAnalyzerContextInterface::GroupEvery;
	m_analyzer->groupElements( groupid, fieldnoList, groupBy, true/*groupSingle*/);
}




