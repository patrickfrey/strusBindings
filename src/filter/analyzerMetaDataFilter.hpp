/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_ANALYZER_METADATA_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_ANALYZER_METADATA_FILTER_HPP_INCLUDED
/// \brief Iterator on the structure of an analyzer document metadata element created as result of a document analysis
/// \file analyzerMetaDataFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/metaData.hpp"
#include "strus/reference.hpp"
#include "templates/structFilter.hpp"
#include "templates/structVectorFilter.hpp"

/// \brief strus toplevel namespace
namespace strus {

struct AnalyzerMetaDataAccess
{
	static binding::ValueVariant get( unsigned int state, const analyzer::MetaData& st);
};

/// \brief Iterator on the structure of an analyzer document metadata element created as result of a document analysis
class AnalyzerMetaDataFilter
	:public StructFilter<analyzer::MetaData, AnalyzerMetaDataAccess>
{
public:
	typedef StructFilter<analyzer::MetaData, AnalyzerMetaDataAccess> Parent;

public:
	AnalyzerMetaDataFilter(){}
	AnalyzerMetaDataFilter( const AnalyzerMetaDataFilter& o) :Parent(o){}

	explicit AnalyzerMetaDataFilter( const analyzer::MetaData* impl);
};

/// \brief Iterator on a vector of structures of analyzer document metadata elements created as result of a document analysis
class AnalyzerMetaDataVectorFilter
	:public StructVectorFilter<analyzer::MetaData, AnalyzerMetaDataAccess>
{
public:
	typedef StructVectorFilter<analyzer::MetaData, AnalyzerMetaDataAccess> Parent;

public:
	AnalyzerMetaDataVectorFilter(){}
	AnalyzerMetaDataVectorFilter( const AnalyzerMetaDataVectorFilter& o);

	explicit AnalyzerMetaDataVectorFilter( const std::vector<analyzer::MetaData>* impl);
};


}//namespace
#endif
