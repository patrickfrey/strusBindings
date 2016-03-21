/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_PHP_OBJECT_INITIALIZERS_HPP_INCLUDED
#define _STRUS_BINDING_PHP_OBJECT_INITIALIZERS_HPP_INCLUDED
#include <php.h>
#include "strus/bindingObjects.hpp"
#include <vector>

int initVariant( Variant& result, zval* obj);
int initTokenizer( Tokenizer& result, zval* obj);
int initNormalizer( Normalizer& result, zval* obj);
int initNormalizerList( std::vector<Normalizer>& result, zval* obj);
int initAggregator( Aggregator& result, zval* obj);
int initSummarizerConfig( SummarizerConfig& result, zval* obj);
int initWeightingConfig( WeightingConfig& result, zval* obj);
int initQueryExpression( QueryExpression& result, zval* obj);
int initStringVector( std::vector<std::string>& result, zval* obj);
int initIntVector( std::vector<int>& result, zval* obj);
int initGlobalStatistics( GlobalStatistics& result, zval* obj);
int initTermStatistics( TermStatistics& result, zval* obj);

int getTermVector( zval* result, const std::vector<Term>& ar);
int getRankVector( zval* result, const std::vector<Rank>& ar);
int getQueryResult( zval* result, const QueryResult& res);

#endif
