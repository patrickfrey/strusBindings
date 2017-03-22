/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief State switching helpers for binding filters
/// \file stateMachines.cpp
#include "stateMachines.hpp"

/// \brief strus toplevel namespace
using namespace strus;
using namespace strus::filter;

static StructElementArray g_emtpy;

class ArrayStateMachineProgram
	:public Program
{
public:
	ArrayStateMachineProgram()
	{
		i_nextState( 0, +3, +4);
		i_setTag( BindingFilterInterface::Index, +3);
		i_setTag( BindingFilterInterface::Value, +2);
		i_nextState( 0, -3, +1);
	}
};
static const ArrayStateMachineProgram g_arrayStateMachineProgram;

class StructStateMachineProgram
	:public Program
{
public:
	StructStateMachineProgram()
	{
		i_nextState( 0, +4, +5);
		i_setTag( BindingFilterInterface::Open, +4);
		i_setTag( BindingFilterInterface::Value, +3);
		i_setTag( BindingFilterInterface::Close, +2);
		i_nextState( 0, -4, +1);
	}
};
static const StructStateMachineProgram g_structStateMachineProgram;

class StructArrayStateMachineProgram
	:public Program
{
public:
	StructArrayStateMachineProgram()
	{
		i_nextState( 1, +7, +8);
		i_setTag( BindingFilterInterface::Index, +7);
		i_nextState( 0, -3, +6);
		i_setTag( BindingFilterInterface::Open, +5);
		i_setTag( BindingFilterInterface::Value, -1);
		i_setTag( BindingFilterInterface::Close, -2);
		i_jumprel( -5, +2);
		i_jumprel( -8, +1);
	}
};
static const StructArrayStateMachineProgram g_StructArrayStateMachineProgram;


ArrayStateMachine::ArrayStateMachine()
	:m_program(&g_arrayStateMachineProgram){}

ArrayStateMachine::ArrayStateMachine( unsigned int size_)
	:m_program(&g_arrayStateMachineProgram)
{
	m_program.setlim( 0/*reg*/, size_);
}

ArrayStateMachine::ArrayStateMachine( const ArrayStateMachine& o)
	:m_program(o.m_program){}


StructStateMachine::StructStateMachine()
	:m_program(&g_structStateMachineProgram)
	,m_elements(&g_emtpy){}

StructStateMachine::StructStateMachine( const StructElementArray& elements_)
	:m_program(&g_structStateMachineProgram)
	,m_elements(&g_emtpy)
{
	m_program.setlim( 0/*reg*/, elements_.size());
}

StructStateMachine::StructStateMachine( const StructStateMachine& o)
	:m_program(o.m_program)
	,m_elements(o.m_elements){}

void StructStateMachine::reset()
{
	m_program.reset();
}


StructArrayStateMachine::StructArrayStateMachine()
	:m_program(&g_StructArrayStateMachineProgram)
	,m_elements(&g_emtpy){}

StructArrayStateMachine::StructArrayStateMachine( const StructElementArray& elements_, unsigned int size_)
	:m_program(&g_StructArrayStateMachineProgram)
	,m_elements(&g_emtpy)
{
	m_program.setlim( 0/*reg*/, elements_.size());
	m_program.setlim( 1/*reg*/, size_);
}

StructArrayStateMachine::StructArrayStateMachine( const StructArrayStateMachine& o)
	:m_program(o.m_program)
	,m_elements(o.m_elements){}

void StructArrayStateMachine::reset()
{
	m_program.reset();
}


