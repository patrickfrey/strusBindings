/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_FILTER_PROGRAM_HPP_INCLUDED
#define _STRUS_BINDING_FILTER_PROGRAM_HPP_INCLUDED
/// \brief Instruction of a filter statemachine
/// \file instruction.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/base/string_format.hpp"
#include "instruction.hpp"
#include "private/internationalization.hpp"
#include <vector>
#include <limits>
#include <iostream>
#include <limits>

/// \brief strus toplevel namespace
namespace strus {
namespace filter {

#define STRUS_LOWLEVEL_DEBUG

class Program
{
public:
	void i_nextState( unsigned int regidx, int incr_ip, int incr_endofscope)
	{
		m_ar.push_back( Instruction( false/*no stop*/, regidx,
					(BindingFilterInterface::Tag)(-1),
					-1/*mask*/, 1/*incr*/, incr_ip,
					incr_endofscope));
	}
	void i_jumprel( int incr_ip, int incr_endofscope)
	{
		m_ar.push_back( Instruction( false/*no stop*/, ZeroReg,
					(BindingFilterInterface::Tag)(-1),
					-1/*mask*/, 0/*incr*/, incr_ip,
					incr_endofscope));
	}
	void i_setTag( BindingFilterInterface::Tag tag, int incr_endofscope)
	{
		m_ar.push_back( Instruction( true/*stop*/, 0/*regidx*/,
					tag, -1/*mask*/, 0/*incr*/, 0/*branchadr*/,
					incr_endofscope));
	}

	void addProgram( const Program& o)
	{
		m_ar.insert( m_ar.end(), o.m_ar.begin(), o.m_ar.end());
	}

	bool endOfProgram( int ip) const			{return ip > (int)m_ar.size();}
	const Instruction& operator[]( int ip) const		{return m_ar[ip];}

	std::string tostring() const
	{
		std::ostringstream rt;
		std::vector<Instruction>::const_iterator ai = m_ar.begin(), ae = m_ar.end();
		for (int ip=0; ai != ae; ++ai,++ip)
		{
			rt << "[" << ip << "] " << ai->tostring() << std::endl;
		}
		return rt.str();
	}

public:
	enum {ZeroReg=3};

private:
	std::vector<Instruction> m_ar;
};


class ProgramContext
{
public:
	explicit ProgramContext( const Program* prg_)
		:m_prg(prg_),m_ip(0)
	{
		std::memset( m_reg, 0, sizeof(m_reg));
		std::memset( m_lim, 0, sizeof(m_lim));
	}
	ProgramContext( const ProgramContext& o)
		:m_prg(o.m_prg),m_ip(o.m_ip)
	{
		std::memcpy( &m_reg, &o.m_reg, sizeof(m_reg));
		std::memcpy( &m_lim, &o.m_lim, sizeof(m_lim));
	}

	void reset()
	{
		m_ip = 0;
		std::memset( m_reg, 0, sizeof(m_reg));
		std::memset( m_lim, 0, sizeof(m_lim));
	}

	bool endOfProgram() const
	{
		return m_prg->endOfProgram( m_ip);
	}

	bool execSkip()
	{
		const Instruction& instr = (*m_prg)[ m_ip];
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << string_format( "skip [%d] %d", m_ip, instr.skipadr()) << std::endl;
#endif
		m_ip = instr.skipadr();
		return endOfProgram();
	}

	bool execStep()
	{
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << string_format( "exec [%u] (%u,%u,%u,%u)", (unsigned int)m_ip, m_reg[0], m_reg[1], m_reg[2], m_reg[3]) << std::endl;
#endif
		for(;;)
		{
			const Instruction& instr = (*m_prg)[ m_ip];
#ifdef STRUS_LOWLEVEL_DEBUG
			std::cerr << "  do [" << m_ip << "] " << instr.tostring() << std::endl;
#endif
			++m_ip;
			if (instr.stop()) break;
			int ax = m_reg[ instr.regidx()];
			ax = (ax & instr.mask()) + instr.incr();
			if (ax >= m_lim[ instr.regidx()])
			{
#ifdef STRUS_LOWLEVEL_DEBUG
				std::cerr << string_format("  got %d >= %d (lim), reset r%u jmp_rel %d", ax, m_lim[ instr.regidx()], instr.regidx(), instr.branchadr()) << std::endl;
#endif
				m_reg[ instr.regidx()] = 0;
				m_ip += instr.branchadr();
			}
			else
			{
#ifdef STRUS_LOWLEVEL_DEBUG
				std::cerr << string_format("  got r%u = %d", instr.regidx(), ax) << std::endl;
#endif
				m_reg[ instr.regidx()] = ax;
			}
			if (endOfProgram()) return false;
		}
		return true;
	}

	void setlim( int regidx, int maxval)
	{
		if (regidx >= MaxReg) throw strus::runtime_error(_TXT("internal: array bound write (register limit)"));
		m_lim[ regidx] = maxval;
	}

	BindingFilterInterface::Tag tag() const
	{
		if (endOfProgram()) return BindingFilterInterface::Close;
		return (*m_prg)[ m_ip].tag();
	}

	int reg( int regidx) const
	{
		if (regidx >= MaxReg) throw strus::runtime_error(_TXT("internal: array bound read (register)"));
		return m_reg[ regidx];
	}

	int ip() const
	{
		return m_ip;
	}

private:
	const Program* m_prg;
	enum {MaxReg=Program::ZeroReg};
	int m_reg[ MaxReg+1];
	int m_lim[ MaxReg+1];
	int m_ip;
};

}}//namespace
#endif
