/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Iterator on the structure of an analyzer attribute created as result of a document analysis
/// \file analyzerAttributeFilter.cpp
#include "analyzerAttributeFilter.hpp"

using namespace strus;
static const char* g_element_names[] = {"name", "value", 0 };
static const filter::StructElementArray g_elements( g_element_names);

enum State {
	StateName,
	StateValue,
	StateEnd
};

binding::ValueVariant AnalyzerAttributeAccess::get( unsigned int state, const analyzer::Attribute& attribute)
{
	switch ((State)state)
	{
		case StateName:
			return binding::ValueVariant( attribute.name().c_str(), attribute.name().size());
		case StateValue:
			return binding::ValueVariant( attribute.value().c_str(), attribute.value().size());
		case StateEnd:
			break;
	}
	return binding::ValueVariant();
}

AnalyzerAttributeFilter::AnalyzerAttributeFilter( const analyzer::Attribute* impl)
	:Parent( impl, g_elements){}

AnalyzerAttributeVectorFilter::AnalyzerAttributeVectorFilter( const std::vector<analyzer::Attribute>* impl)
	:Parent( impl, g_elements){}




