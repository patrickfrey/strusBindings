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

#if 0/*DEPRECATED UNFINISHED*/
MetaDataOpCNF bindings::metaDataOp( const MetaDataOp::CompareOperator& opr, const std::string& name, const NumericVariant& operand)
{
	MetaDataOpCNF rt;
	MetaDataOpList elem;
	elem.push_back( MetaDataOp( opr, name, operand));
	rt.push_back( elem);
	return rt;
}

/// Build new CNF out of a list of CNF, by selecting one leaf element from each member to form a new member
static MetaDataOpCNF select_one_of_each( const std::vector<MetaDataOpCNF>::const_iterator& start, std::vector<MetaDataOpCNF>::const_iterator end)
{
	MetaDataOpCNF rt;
	if (end == start) return MetaDataOpCNF();
	--end;
	if (end == start)
	{
		MetaDataOpCNF::const_iterator ei = start->begin(), ee = start->end();
		for (; ei != ee; ++ei)
		{
			MetaDataOpList elem;
			elem.push_back( MetaDataOp( ei->opr(), ei->name(), ei->operand()));
			rt.push_back( elem);
		}
	}
	else
	{
		MetaDataOpCNF rec = select_one_of_each( start, end);
		MetaDataOpCNF::const_iterator ri = rec.begin(), re = rec.end();
		for (; ri != re; ++ri)
		{
			MetaDataOpList::const_iterator ei = start->begin(), ee = start->end();
			for (; ei != ee; ++ei)
			{
				MetaDataOpList elem( *ri);
				elem.push_back( MetaDataOp( ei->opr(), ei->name(), ei->operand()));
				rt.push_back( elem);
			}
		}
	}
	return rt;
}

static void eliminate_duplicates( MetaDataOpCNF& cnf)
{
	MetaDataOpCNF::iterator ci = cnf.begin(), ce = cnf.end();
	for (; ci != ce; ++ci)
	{
		std::set<MetaDataOp> elemset;
		elemset.insert( elemset.begin(), ci->begin(), ce->end());
		if (elemset.size() < ci->size())
		{
			ci->clear();
			ci->insert( ci->begin(), elemset.begin(), elemset.end());
		}
	}
	std::set<MetaDataOpList> opset;
	opset.insert( opset.begin(), cnf.begin(), cnf.end());
	if (opset.size() < cnf.size())
	{
		cnf.clear();
		cnf.insert( cnf.begin(), opset.begin(), opset.end());
	}
}

MetaDataOpCNF bindings::joinMetaDataOp( MetaDataOp::Join joinop, const std::vector<MetaDataOpCNF>& arglist)
{
	MetaDataOpCNF rt;
	switch (joinop)
	{
		case MetaDataOp::JoinAnd:
			for (std::vector<MetaDataOpCNF>::const_iterator ai = arglist.begin(); ai != arglist.end(); ++ai)
			{
				rt.insert( rt.end(), ai->begin(), ai->end());
			}
			break;
		case MetaDataOp::JoinOr:
			rt = select_one_of_each( arglist.begin(), arglist.end());
			eliminate_duplicates( rt);
			break;
	}
	return rt;
}
#endif


