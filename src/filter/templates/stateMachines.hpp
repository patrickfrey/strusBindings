/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_STATEMACHINES_HPP_INCLUDED
#define _STRUS_BINDING_STATEMACHINES_HPP_INCLUDED
/// \brief State switching helpers for binding filters
/// \file stateMachines.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/binding/valueVariant.hpp"
#include "instruction.hpp"
#include "program.hpp"
#include <vector>

/// \brief strus toplevel namespace
namespace strus {
namespace filter {

/// \brief Static array of tag names as value variants
class StructElementArray
{
public:
	StructElementArray()
	{
		m_ar.push_back( binding::ValueVariant());
	}

	StructElementArray( const char** names)
	{
		char const** nitr = names;
		for (; *nitr; ++nitr) m_ar.push_back( *nitr);
		m_ar.push_back( binding::ValueVariant());
	}

	const binding::ValueVariant& operator[]( std::size_t idx) const
	{
		return m_ar[ idx];
	}

	unsigned int size() const
	{
		return m_ar.size();
	}

private:
	std::vector<binding::ValueVariant> m_ar;
};


/// \brief Statemachine for iterating on an array with a binding filter
class ArrayStateMachine
{
public:
	/// \brief Default constructor
	ArrayStateMachine();

	/// \brief Constructor
	explicit ArrayStateMachine( unsigned int size_);

	/// \brief Copy constructor
	ArrayStateMachine( const ArrayStateMachine& o);

	unsigned int currentStateIndex() const
	{
		return m_program.reg(0);
	}

	BindingFilterInterface::Tag currentTag() const
	{
		return m_program.tag();
	}

	bool skipNext()
	{
		if (m_program.endOfProgram()) return false;
		return m_program.execStep();
	}

	bool skipScope()
	{
		return m_program.execSkip();
	}

private:
	ProgramContext m_program;
};


/// \brief Statemachine for iterating on a structure with a binding filter
class StructStateMachine
{
public:
	/// \brief Default constructor
	StructStateMachine();

	/// \brief Constructor
	explicit StructStateMachine( const StructElementArray& elements_);

	/// \brief Copy constructor
	StructStateMachine( const StructStateMachine& o);

	void reset();

	unsigned int currentStateIndex() const
	{
		return m_program.reg(0);
	}

	BindingFilterInterface::Tag currentTag() const
	{
		return m_program.tag();
	}

	const binding::ValueVariant& currentTagValue() const
	{
		return (*m_elements)[ currentStateIndex()];
	}

	bool skipNext()
	{
		if (m_program.endOfProgram()) return false;
		return m_program.execStep();
	}

	bool skipScope()
	{
		return m_program.execSkip();
	}

private:
	ProgramContext m_program;
	const StructElementArray* m_elements;
};


/// \brief Statemachine for iterating on an array of structures with a binding filter
class StructArrayStateMachine
	:public StructStateMachine
{
public:
	/// \brief Default constructor
	StructArrayStateMachine();

	/// \brief Constructor
	StructArrayStateMachine( const StructElementArray& elements_, unsigned int size_);

	/// \brief Copy constructor
	StructArrayStateMachine( const StructArrayStateMachine& o);

	void reset();

	unsigned int currentArrayIndex() const
	{
		return m_program.reg(1);
	}

	unsigned int currentStateIndex() const
	{
		return m_program.reg(0);
	}

	BindingFilterInterface::Tag currentTag() const
	{
		return m_program.tag();
	}

	const binding::ValueVariant& currentTagValue() const
	{
		return (*m_elements)[ currentStateIndex()];
	}

	bool skipNext()
	{
		if (m_program.endOfProgram()) return false;
		return m_program.execStep();
	}

	bool skipScope()
	{
		return m_program.execSkip();
	}

private:
	ProgramContext m_program;
	const StructElementArray* m_elements;
};

}}//namespace
#endif

