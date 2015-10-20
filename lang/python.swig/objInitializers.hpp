/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
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

int initVariant( Variant& result, PyObject* obj);
int initTokenizer( Tokenizer& result, PyObject* obj);
int initNormalizer( Normalizer& result, PyObject* obj);
int initNormalizerList( std::vector<Normalizer>& result, PyObject* obj);
int initAggregator( Aggregator& result, PyObject* obj);
int initSummarizerConfig( SummarizerConfig& result, PyObject* obj);
int initWeightingConfig( WeightingConfig& result, PyObject* obj);
int initString( std::string& result, PyObject* obj);
int initStringVector( std::vector<std::string>& result, PyObject* obj);
int initIntVector( std::vector<int>& result, PyObject* obj);

PyObject* getTermVector( const std::vector<Term>& ar);
PyObject* getRankVector( const std::vector<Rank>& ar);

#endif
