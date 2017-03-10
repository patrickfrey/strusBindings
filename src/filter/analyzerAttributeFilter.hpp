/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_ANALYZER_ATTRIBUTE_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_ANALYZER_ATTRIBUTE_FILTER_HPP_INCLUDED
/// \brief Iterator on the structure of an analyzer document attribute created as result of a document analysis
/// \file analyzerAttributeFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/attribute.hpp"
#include "strus/reference.hpp"
#include "templates/structFilter.hpp"
#include "templates/structVectorFilter.hpp"

/// \brief strus toplevel namespace
namespace strus {

struct AnalyzerAttributeAccess
{
	static binding::ValueVariant get( unsigned int state, const analyzer::Attribute& st);
};

/// \brief Iterator on the structure of an analyzer document attribute created as result of a document analysis
class AnalyzerAttributeFilter
	:public StructFilter<analyzer::Attribute, AnalyzerAttributeAccess>
{
public:
	typedef StructFilter<analyzer::Attribute, AnalyzerAttributeAccess> Parent;

public:
	AnalyzerAttributeFilter(){}
	AnalyzerAttributeFilter( const AnalyzerAttributeFilter& o) :Parent(o){}

	explicit AnalyzerAttributeFilter( const analyzer::Attribute* impl);
};

/// \brief Iterator on a vector of structures of analyzer document attributes created as result of a document analysis
class AnalyzerAttributeVectorFilter
	:public StructVectorFilter<analyzer::Attribute, AnalyzerAttributeAccess>
{
public:
	typedef StructVectorFilter<analyzer::Attribute, AnalyzerAttributeAccess> Parent;

public:
	AnalyzerAttributeVectorFilter(){}
	AnalyzerAttributeVectorFilter( const AnalyzerAttributeVectorFilter& o);

	explicit AnalyzerAttributeVectorFilter( const std::vector<analyzer::Attribute>* impl);
};


}//namespace
#endif
