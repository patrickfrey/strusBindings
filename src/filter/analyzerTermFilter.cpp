/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Iterator on the structure of an analyzer term created as result of a document analysis
/// \file analyzerTermFilter.cpp
#include "analyzerTermFilter.hpp"

using namespace strus;
static const char* g_element_names[] = {"type", "value", "pos", "len", 0 };
static const filter::StructElementArray g_elements( g_element_names);

enum State {
	StateType,
	StateValue,
	StatePos,
	StateLen,
	StateEnd
};

binding::ValueVariant AnalyzerTermAccess::get( unsigned int state, const analyzer::Term& term)
{
	switch ((State)state)
	{
		case StateType:
			return binding::ValueVariant( term.type().c_str(), term.type().size());
		case StateValue:
			return binding::ValueVariant( term.value().c_str(), term.type().size());
		case StatePos:
			return binding::ValueVariant( (binding::ValueVariant::UIntType)term.pos());
		case StateLen:
			return binding::ValueVariant( (binding::ValueVariant::UIntType)term.len());
		case StateEnd:
			break;
	}
	return binding::ValueVariant();
}

AnalyzerTermFilter::AnalyzerTermFilter( const analyzer::Term* impl)
	:Parent( impl, g_elements){}

AnalyzerTermVectorFilter::AnalyzerTermVectorFilter( const std::vector<analyzer::Term>* impl)
	:Parent( impl, g_elements){}




