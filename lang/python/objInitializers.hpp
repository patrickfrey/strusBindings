/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
void initFunctionVariableConfig( FunctionVariableConfig& result, PyObject* obj);
void initQueryExpression( QueryExpression& result, PyObject* obj);
void initString( std::string& result, PyObject* obj);
void initStringVector( std::vector<std::string>& result, PyObject* obj);
void initIntVector( std::vector<int>& result, PyObject* obj);
void initFloatVector( std::vector<double>& result, PyObject* obj);
void initTermStatistics( TermStatistics& result, PyObject* obj);
void initGlobalStatistics( GlobalStatistics& result, PyObject* obj);
void initDataBlob( std::string& result, PyObject* obj);
void initPatternMatcher( PatternMatcher& matcher, PyObject* obj);

#endif
