/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Iterator on the structure of an analyzer document metadata element created as result of a document analysis
/// \file analyzerMetaDataFilter.cpp
#include "analyzerMetaDataFilter.hpp"

using namespace strus;
static const char* g_element_names[] = {"name", "value", 0 };
static const filter::StructElementArray g_elements( g_element_names);

enum State {
	StateName,
	StateValue,
	StateEnd
};

binding::ValueVariant AnalyzerMetaDataAccess::get( unsigned int state, const analyzer::MetaData& data)
{
	switch ((State)state)
	{
		case StateName:
			return binding::ValueVariant( data.name().c_str(), data.name().size());
		case StateValue:
			return binding::ValueVariant( data.value());
		case StateEnd:
			break;
	}
	return binding::ValueVariant();
}

AnalyzerMetaDataFilter::AnalyzerMetaDataFilter( const analyzer::MetaData* impl)
	:Parent( impl, g_elements){}

AnalyzerMetaDataVectorFilter::AnalyzerMetaDataVectorFilter( const std::vector<analyzer::MetaData>* impl)
	:Parent( impl, g_elements){}


