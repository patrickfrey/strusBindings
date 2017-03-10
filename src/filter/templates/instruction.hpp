/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_FILTER_INSTRUCTION_HPP_INCLUDED
#define _STRUS_BINDING_FILTER_INSTRUCTION_HPP_INCLUDED
/// \brief Instruction of a filter statemachine
/// \file instruction.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/base/string_format.hpp"
#include <vector>
#include <limits>
#include <iostream>
#include <sstream>

/// \brief strus toplevel namespace
namespace strus {
namespace filter {

class Instruction
{
public:
	Instruction( const Instruction& o)
		:m_stop(o.m_stop),m_regidx(o.m_regidx),m_tag(o.m_tag),m_elemtableidx(o.m_elemtableidx),m_elemnameidx(o.m_elemnameidx),m_mask(o.m_mask),m_incr(o.m_incr),m_branchadr(o.m_branchadr),m_skipadr(o.m_skipadr){}
	Instruction( bool stop_, unsigned char regidx_, BindingFilterInterface::Tag tag_, unsigned int elemtableidx_, unsigned int elemnameidx_, int mask_, int incr_, int branchadr_, int skipadr_)
		:m_stop(stop_),m_regidx(regidx_),m_tag(tag_),m_elemtableidx(elemtableidx_),m_elemnameidx(elemnameidx_),m_mask(mask_),m_incr(incr_),m_branchadr(branchadr_),m_skipadr(skipadr_){}

	bool stop() const			{return (bool)m_stop;}
	unsigned int regidx() const		{return (unsigned int)m_regidx;}
	BindingFilterInterface::Tag tag() const	{return (BindingFilterInterface::Tag)m_tag;}
	unsigned int elemtableidx() const	{return m_elemtableidx;}
	unsigned int elemnameidx() const	{return m_elemnameidx;}
	int mask() const			{return (bool)m_mask;}
	int incr() const			{return (bool)m_incr;}
	int branchadr() const			{return (bool)m_branchadr;}
	int skipadr() const			{return (bool)m_skipadr;}

	std::string tostring() const
	{
		std::ostringstream rt;
		rt << string_format( "reg=%u stop=%s tag=%s mask=%d incr=%d branch=%d skip=%d",
				(unsigned int)regidx(), (stop() ? "stop":"go"), tagname(tag()),
				mask(), incr(), branchadr(), skipadr());
		return rt.str();
	}

private:
	static const char* tagname( BindingFilterInterface::Tag tag)
	{
		static const char* ar[] = {"open","index","value","close",0};
		return ar[(unsigned int)tag];
	}


private:
	unsigned char m_stop;
	unsigned char m_regidx;
	unsigned char m_tag;
	unsigned char m_elemtableidx;
	unsigned char m_elemnameidx;
	int m_mask;
	int m_incr;
	int m_branchadr;
	int m_skipadr;
};

}}//namespace
#endif


