/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Identifiers for objects and methods for serialization
/// \note PROGRAM (strusBindingsGen) GENERATED FILE! DO NOT MODIFY!!!
/// \file bindingObjects.h
#ifndef _STRUS_BINDINGS_bindingObjects_H_INCLUDED
#define _STRUS_BINDINGS_bindingObjects_H_INCLUDED
#include "papuga/valueVariant.h"
#include "papuga/callResult.h"
#include "papuga/serialization.h"
#include "papuga/hostObjectReference.h"
#include <stddef.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
void _strus_bindings_destructor__Context( void* self);

bool _strus_bindings_constructor__Context( papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__getLastError( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__loadModule( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__addModulePath( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__addResourcePath( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__createStatisticsProcessor( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__createStorageClient( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__createVectorStorageClient( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__createStorage( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__createVectorStorage( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__destroyStorage( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__detectDocumentClass( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__createDocumentAnalyzer( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__createQueryAnalyzer( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__createQueryEval( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Context__close( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__StorageClient( void* self);

bool _strus_bindings_StorageClient__nofDocumentsInserted( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageClient__createTransaction( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageClient__createInitStatisticsIterator( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageClient__createUpdateStatisticsIterator( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageClient__createDocumentBrowser( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageClient__config( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageClient__configstring( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageClient__close( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__StorageTransaction( void* self);

bool _strus_bindings_StorageTransaction__insertDocument( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageTransaction__deleteDocument( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageTransaction__deleteUserAccessRights( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageTransaction__commit( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StorageTransaction__rollback( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__DocumentBrowser( void* self);

bool _strus_bindings_DocumentBrowser__addMetaDataRestrictionCondition( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentBrowser__skipDoc( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentBrowser__get( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__StatisticsIterator( void* self);

bool _strus_bindings_StatisticsIterator__getNext( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__StatisticsProcessor( void* self);

bool _strus_bindings_StatisticsProcessor__decode( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_StatisticsProcessor__encode( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__VectorStorageSearcher( void* self);

bool _strus_bindings_VectorStorageSearcher__findSimilar( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageSearcher__findSimilarFromSelection( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageSearcher__close( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__VectorStorageClient( void* self);

bool _strus_bindings_VectorStorageClient__createSearcher( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__createTransaction( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__conceptClassNames( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__conceptFeatures( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__nofConcepts( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__featureConcepts( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__featureVector( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__featureName( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__featureIndex( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__nofFeatures( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__config( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__configstring( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageClient__close( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__VectorStorageTransaction( void* self);

bool _strus_bindings_VectorStorageTransaction__addFeature( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageTransaction__defineFeatureConceptRelation( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageTransaction__commit( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageTransaction__rollback( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_VectorStorageTransaction__close( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__DocumentAnalyzer( void* self);

bool _strus_bindings_DocumentAnalyzer__addSearchIndexFeature( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__addForwardIndexFeature( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__defineMetaData( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__defineAggregatedMetaData( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__defineAttribute( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__defineMetaDataFromPatternMatch( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__defineAttributeFromPatternMatch( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__definePatternMatcherPostProc( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__definePatternMatcherPostProcFromFile( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__defineDocument( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_DocumentAnalyzer__analyze( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__QueryAnalyzer( void* self);

bool _strus_bindings_QueryAnalyzer__addSearchIndexElement( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryAnalyzer__addSearchIndexElementFromPatternMatch( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryAnalyzer__addPatternLexem( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryAnalyzer__definePatternMatcherPostProc( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryAnalyzer__definePatternMatcherPostProcFromFile( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryAnalyzer__defineImplicitGroupBy( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryAnalyzer__analyze( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__QueryEval( void* self);

bool _strus_bindings_QueryEval__addTerm( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryEval__addSelectionFeature( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryEval__addRestrictionFeature( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryEval__addExclusionFeature( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryEval__addSummarizer( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryEval__addWeightingFunction( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryEval__addWeightingFormula( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_QueryEval__createQuery( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
void _strus_bindings_destructor__Query( void* self);

bool _strus_bindings_Query__defineFeature( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__addMetaDataRestrictionCondition( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__defineTermStatistics( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__defineGlobalStatistics( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__addDocumentEvaluationSet( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__setMaxNofRanks( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__setMinRank( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__addUserName( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__setWeightingVariables( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__setDebugMode( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__evaluate( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
bool _strus_bindings_Query__tostring( void* self, papuga_CallResult* retval, size_t argc, const papuga_ValueVariant* argv);
#ifdef __cplusplus
}
#endif
#endif

