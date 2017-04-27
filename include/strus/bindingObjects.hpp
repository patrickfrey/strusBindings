/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Identifiers for objects and methods for serialization
/// \note PROGRAM (strusBindingsGen) GENERATED FILE! DO NOT MODIFY!!!
/// \file bindingObjects.hpp
#ifndef _STRUS_BINDINGS_PARSER_bindingObjects_HPP_INCLUDED
#define _STRUS_BINDINGS_PARSER_bindingObjects_HPP_INCLUDED
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/callResult.hpp"
#include "strus/bindings/serialization.hpp"
#include "strus/bindings/hostObjectReference.hpp"
#include <cstddef>

namespace strus {
namespace bindings {

bool Context__getLastError( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__loadModule( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__addModulePath( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__addResourcePath( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createStatisticsProcessor( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createStorageClient( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createStorageClient( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createVectorStorageClient( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createVectorStorageClient( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createStorage( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createVectorStorage( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__destroyStorage( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__detectDocumentClass( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createDocumentAnalyzer( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createQueryAnalyzer( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createQueryEval( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__nofDocumentsInserted( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__createTransaction( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__createInitStatisticsIterator( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__createUpdateStatisticsIterator( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__createDocumentBrowser( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__config( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__insertDocument( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__deleteDocument( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__deleteUserAccessRights( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__commit( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__rollback( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentBrowser__addMetaDataRestrictionCondition( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentBrowser__skipDoc( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentBrowser__get( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StatisticsIterator__getNext( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StatisticsProcessor__decode( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StatisticsProcessor__encode( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageSearcher__findSimilar( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageSearcher__findSimilarFromSelection( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageSearcher__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__createSearcher( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__createTransaction( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__conceptClassNames( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__conceptFeatures( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__nofConcepts( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__featureConcepts( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__featureVector( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__featureName( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__featureIndex( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__nofFeatures( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__config( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__addFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__defineFeatureConceptRelation( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__commit( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__rollback( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__addSearchIndexFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__addForwardIndexFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineMetaData( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineAggregatedMetaData( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineAttribute( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineMetaDataFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineAttributeFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__definePatternMatcherPostProc( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__definePatternMatcherPostProcFromFile( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineDocument( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__analyze( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__analyze( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__addSearchIndexElement( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__addSearchIndexElementFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__addPatternLexem( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__definePatternMatcherPostProc( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__definePatternMatcherPostProcFromFile( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__defineImplicitGroupBy( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__analyze( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addTerm( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addSelectionFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addRestrictionFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addExclusionFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addSummarizer( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addWeightingFunction( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addWeightingFormula( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__createQuery( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__defineFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__addMetaDataRestrictionCondition( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__defineTermStatistics( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__defineGlobalStatistics( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__addDocumentEvaluationSet( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__setMaxNofRanks( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__setMinRank( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__addUserName( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__setWeightingVariables( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__setDebugMode( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__evaluate( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__tostring( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv);

}}//namespace
#endif

