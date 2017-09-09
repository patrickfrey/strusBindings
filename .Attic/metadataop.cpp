/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "metadataop.hpp"
#include "internationalization.hpp"
#include <set>

using namespace strus;
using namespace strus::bindings;

MetaDataOp::CompareOperator MetaDataOp::getCompareOp( const char* compareOp)
{
	CompareOperator cmpop;
	if (compareOp[0] == '<')
	{
		if (compareOp[1] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareLess;
		}
		else if (compareOp[1] == '=' && compareOp[2] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareLessEqual;
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
		}
	}
	else if (compareOp[0] == '>')
	{
		if (compareOp[1] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareGreater;
		}
		else if (compareOp[1] == '=' && compareOp[2] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareGreaterEqual;
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
		}
	}
	else if (compareOp[0] == '=' && compareOp[1] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareEqual;
	}
	else if (compareOp[0] == '=' && compareOp[1] == '=' && compareOp[2] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareEqual;
	}
	else if (compareOp[0] == '!' && compareOp[1] == '=' && compareOp[2] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareNotEqual;
	}
	else
	{
		throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
	}
	return cmpop;
}

bool MetaDataOp::operator < (const MetaDataOp& o) const
{
	if (m_name < o.m_name) return true;
	if (m_name > o.m_name) return false;
	if (m_opr < o.opr()) return true;
	if (m_opr > o.opr()) return false;
	return m_operand < o.m_operand;
}



