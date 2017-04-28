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
#include "papuga/valueVariant.hpp"
#include "papuga/callResult.hpp"
#include "papuga/serialization.hpp"
#include "papuga/hostObjectReference.hpp"
#include <cstddef>

namespace strus {

bool Context__getLastError( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__loadModule( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__addModulePath( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__addResourcePath( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__createStatisticsProcessor( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__createStorageClient( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__createVectorStorageClient( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__createStorage( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__createVectorStorage( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__destroyStorage( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__detectDocumentClass( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__createDocumentAnalyzer( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__createQueryAnalyzer( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__createQueryEval( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Context__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageClient__nofDocumentsInserted( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageClient__createTransaction( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageClient__createInitStatisticsIterator( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageClient__createUpdateStatisticsIterator( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageClient__createDocumentBrowser( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageClient__config( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageClient__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageTransaction__insertDocument( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageTransaction__deleteDocument( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageTransaction__deleteUserAccessRights( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageTransaction__commit( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StorageTransaction__rollback( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentBrowser__addMetaDataRestrictionCondition( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentBrowser__skipDoc( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentBrowser__get( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StatisticsIterator__getNext( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StatisticsProcessor__decode( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool StatisticsProcessor__encode( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageSearcher__findSimilar( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageSearcher__findSimilarFromSelection( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageSearcher__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__createSearcher( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__createTransaction( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__conceptClassNames( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__conceptFeatures( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__nofConcepts( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__featureConcepts( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__featureVector( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__featureName( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__featureIndex( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__nofFeatures( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__config( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageClient__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageTransaction__addFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageTransaction__defineFeatureConceptRelation( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageTransaction__commit( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageTransaction__rollback( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool VectorStorageTransaction__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__addSearchIndexFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__addForwardIndexFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__defineMetaData( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__defineAggregatedMetaData( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__defineAttribute( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__defineMetaDataFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__defineAttributeFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__definePatternMatcherPostProc( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__definePatternMatcherPostProcFromFile( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__defineDocument( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool DocumentAnalyzer__analyze( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryAnalyzer__addSearchIndexElement( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryAnalyzer__addSearchIndexElementFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryAnalyzer__addPatternLexem( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryAnalyzer__definePatternMatcherPostProc( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryAnalyzer__definePatternMatcherPostProcFromFile( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryAnalyzer__defineImplicitGroupBy( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryAnalyzer__analyze( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryEval__addTerm( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryEval__addSelectionFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryEval__addRestrictionFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryEval__addExclusionFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryEval__addSummarizer( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryEval__addWeightingFunction( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryEval__addWeightingFormula( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool QueryEval__createQuery( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__defineFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__addMetaDataRestrictionCondition( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__defineTermStatistics( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__defineGlobalStatistics( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__addDocumentEvaluationSet( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__setMaxNofRanks( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__setMinRank( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__addUserName( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__setWeightingVariables( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__setDebugMode( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__evaluate( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);
bool Query__tostring( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv);

}//namespace
#endif

