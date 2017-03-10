/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_ANALYZER_TERM_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_ANALYZER_TERM_FILTER_HPP_INCLUDED
/// \brief Iterator on the structure of an analyzer document term created as result of a document analysis
/// \file analyzerTermFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/term.hpp"
#include "strus/reference.hpp"
#include "templates/structFilter.hpp"
#include "templates/structVectorFilter.hpp"

/// \brief strus toplevel namespace
namespace strus {

struct AnalyzerTermAccess
{
	static binding::ValueVariant get( unsigned int state, const analyzer::Term& st);
};

/// \brief Iterator on the structure of an analyzer document term created as result of a document analysis
class AnalyzerTermFilter
	:public StructFilter<analyzer::Term, AnalyzerTermAccess>
{
public:
	typedef StructFilter<analyzer::Term, AnalyzerTermAccess> Parent;

public:
	AnalyzerTermFilter(){}
	AnalyzerTermFilter( const AnalyzerTermFilter& o) :Parent(o){}

	explicit AnalyzerTermFilter( const analyzer::Term* impl);
};

/// \brief Iterator on a vector of structures of analyzer document terms created as result of a document analysis
class AnalyzerTermVectorFilter
	:public StructVectorFilter<analyzer::Term, AnalyzerTermAccess>
{
public:
	typedef StructVectorFilter<analyzer::Term, AnalyzerTermAccess> Parent;

public:
	AnalyzerTermVectorFilter(){}
	AnalyzerTermVectorFilter( const AnalyzerTermVectorFilter& o);

	explicit AnalyzerTermVectorFilter( const std::vector<analyzer::Term>* impl);
};

}//namespace
#endif

