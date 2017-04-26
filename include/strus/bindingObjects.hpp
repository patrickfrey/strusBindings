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

bool Context__getLastError( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__loadModule( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__addModulePath( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__addResourcePath( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createStatisticsProcessor( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createStorageClient( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createStorageClient( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createVectorStorageClient( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createVectorStorageClient( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createStorage( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createVectorStorage( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__destroyStorage( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__detectDocumentClass( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createDocumentAnalyzer( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createQueryAnalyzer( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__createQueryEval( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Context__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__nofDocumentsInserted( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__createTransaction( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__createInitStatisticsIterator( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__createUpdateStatisticsIterator( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__createDocumentBrowser( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__config( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageClient__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__insertDocument( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__deleteDocument( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__deleteUserAccessRights( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__commit( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StorageTransaction__rollback( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentBrowser__addMetaDataRestrictionCondition( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentBrowser__skipDoc( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentBrowser__get( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StatisticsIterator__getNext( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StatisticsProcessor__decode( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool StatisticsProcessor__encode( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageSearcher__findSimilar( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageSearcher__findSimilarFromSelection( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageSearcher__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__createSearcher( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__createTransaction( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__conceptClassNames( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__conceptFeatures( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__nofConcepts( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__featureConcepts( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__featureVector( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__featureName( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__featureIndex( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__nofFeatures( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__config( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageClient__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__addFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__defineFeatureConceptRelation( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__commit( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__rollback( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool VectorStorageTransaction__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__addSearchIndexFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__addForwardIndexFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineMetaData( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineAggregatedMetaData( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineAttribute( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineMetaDataFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineAttributeFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__definePatternMatcherPostProc( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__definePatternMatcherPostProcFromFile( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__defineDocument( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__analyze( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool DocumentAnalyzer__analyze( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__addSearchIndexElement( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__addSearchIndexElementFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__addPatternLexem( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__definePatternMatcherPostProc( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__definePatternMatcherPostProcFromFile( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__defineImplicitGroupBy( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryAnalyzer__analyze( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addTerm( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addSelectionFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addRestrictionFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addExclusionFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addSummarizer( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addWeightingFunction( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__addWeightingFormula( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool QueryEval__createQuery( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__defineFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__addMetaDataRestrictionCondition( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__defineTermStatistics( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__defineGlobalStatistics( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__addDocumentEvaluationSet( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__setMaxNofRanks( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__setMinRank( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__addUserName( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__setWeightingVariables( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__setDebugMode( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__evaluate( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);
bool Query__tostring( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv);

}}//namespace
#endif

