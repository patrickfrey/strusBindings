/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef _STRUS_BINDING_METADATA_OP_HPP_INCLUDED
#define _STRUS_BINDING_METADATA_OP_HPP_INCLUDED
#include "strus/metaDataRestrictionInterface.hpp"
#include <vector>

namespace strus {
namespace bindings {

class MetaDataOp
{
public:
	typedef MetaDataRestrictionInterface::CompareOperator CompareOperator;
	static CompareOperator getCompareOp( const char* compareOp);

	MetaDataOp( const CompareOperator& opr_, const std::string& name_, const NumericVariant& operand_)
		:m_opr(opr_),m_name(name_),m_operand(operand_){}
	MetaDataOp( const MetaDataOp& o)
		:m_opr(o.m_opr),m_name(o.m_name),m_operand(o.m_operand){}

	const CompareOperator& opr() const	{return m_opr;}
	const std::string& name() const		{return m_name;}
	const NumericVariant& operand() const	{return m_operand;}

	bool operator < (const MetaDataOp& o) const;

private:
	CompareOperator m_opr;
	std::string m_name;
	NumericVariant m_operand;
};

}}
#endif
