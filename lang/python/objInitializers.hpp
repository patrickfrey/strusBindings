/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2015 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#ifndef _STRUS_BINDING_PYHON_OBJECT_INITIALIZERS_HPP_INCLUDED
#define _STRUS_BINDING_PYHON_OBJECT_INITIALIZERS_HPP_INCLUDED
#include <Python.h>
#include "strus/bindingObjects.hpp"
#include <vector>

void initVariant( Variant& result, PyObject* obj);
void initTokenizer( Tokenizer& result, PyObject* obj);
void initNormalizer( Normalizer& result, PyObject* obj);
void initNormalizerList( std::vector<Normalizer>& result, PyObject* obj);
void initAggregator( Aggregator& result, PyObject* obj);
void initSummarizerConfig( SummarizerConfig& result, PyObject* obj);
void initWeightingConfig( WeightingConfig& result, PyObject* obj);
void initQueryExpression( QueryExpression& result, PyObject* obj);
void initString( std::string& result, PyObject* obj);
void initStringVector( std::vector<std::string>& result, PyObject* obj);
void initIntVector( std::vector<int>& result, PyObject* obj);
void initTermStatistics( TermStatistics& result, PyObject* obj);
void initGlobalStatistics( GlobalStatistics& result, PyObject* obj);
void initDataBlob( std::string& result, PyObject* obj);

#endif
