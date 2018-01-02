/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/metadataExpression.hpp"

using namespace strus;
using namespace strus::bindings;

void MetaDataExpression::pushCompare( MetaDataRestrictionInterface::CompareOperator op, const std::string& fieldtype, const std::string& value)
{
	++m_fieldno_cnt;
	m_analyzer->putField( m_fieldno_cnt, fieldtype, value);
	m_fieldno_stack.push_back( m_fieldno_cnt);
	std::vector<int> fieldnoList( m_fieldno_stack.end()-1, m_fieldno_stack.end());
	int groupid = getCompareOp( op);
	m_analyzer->groupElements( groupid, fieldnoList, QueryAnalyzerContextInterface::GroupUnique, true/*groupSingle*/);
}

void MetaDataExpression::pushOperator( const BooleanOp& op, unsigned int argc)
{
	if (m_fieldno_stack.size() < argc) throw strus::runtime_error( "%s", _TXT("push metadata operator without all arguments defined"));
	int* fnstart = m_fieldno_stack.data() + m_fieldno_stack.size() - argc;
	int* fnend = fnstart + argc;
	std::vector<int> fieldnoList( fnstart, fnend);

	unsigned int groupid = getBooleanOp( op);
	QueryAnalyzerContextInterface::GroupBy groupBy = QueryAnalyzerContextInterface::GroupAll;
	m_analyzer->groupElements( groupid, fieldnoList, groupBy, true/*groupSingle*/);
	m_fieldno_stack.resize( m_fieldno_stack.size() - argc + 1);
}


