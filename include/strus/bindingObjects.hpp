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

bool DocumentAnalyzer__addSearchIndexFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__addForwardIndexFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__defineMetaData( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__defineAggregatedMetaData( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__defineAttribute( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__defineMetaDataFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__defineAttributeFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__definePatternMatcherPostProc( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__definePatternMatcherPostProcFromFile( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__defineDocument( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__analyze( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__analyze( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzer__createQueue( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzeQueue__push( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzeQueue__push( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzeQueue__hasMore( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentAnalyzeQueue__fetch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzer__addSearchIndexElement( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzer__addSearchIndexElementFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzer__addPatternLexem( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzer__definePatternMatcherPostProc( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzer__definePatternMatcherPostProcFromFile( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzer__analyzeField( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzer__createContext( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzeContext__putField( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryAnalyzeContext__analyze( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageClient__nofDocumentsInserted( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageClient__createTransaction( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageClient__createInitStatisticsIterator( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageClient__createUpdateStatisticsIterator( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageClient__createDocumentBrowser( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageClient__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageTransaction__insertDocument( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageTransaction__deleteDocument( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageTransaction__deleteUserAccessRights( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageTransaction__commit( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StorageTransaction__rollback( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StatisticsIterator__getNext( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StatisticsProcessor__decode( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool StatisticsProcessor__encode( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageSearcher__findSimilar( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageSearcher__findSimilarFromSelection( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageSearcher__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__createSearcher( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__createTransaction( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__conceptClassNames( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__conceptFeatures( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__nofConcepts( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__featureConcepts( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__featureVector( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__featureName( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__featureIndex( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__nofFeatures( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__config( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageClient__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageTransaction__addFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageTransaction__defineFeatureConceptRelation( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageTransaction__commit( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageTransaction__rollback( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool VectorStorageTransaction__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryEval__addTerm( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryEval__addSelectionFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryEval__addRestrictionFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryEval__addExclusionFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryEval__addSummarizer( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryEval__addWeightingFunction( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryEval__addWeightingFormula( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool QueryEval__createQuery( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__defineFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__defineDocFieldFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__defineTermStatistics( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__defineGlobalStatistics( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__addDocumentEvaluationSet( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__setMaxNofRanks( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__setMinRank( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__addUserName( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__setWeightingVariables( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__setDebugMode( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__evaluate( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Query__tostring( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentBrowser__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentBrowser__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentBrowser__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentBrowser__skipDoc( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool DocumentBrowser__attribute( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__checkErrors( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__loadModule( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__addModulePath( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__addResourcePath( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createStatisticsProcessor( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createStorageClient( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createStorageClient( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createVectorStorageClient( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createVectorStorageClient( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createStorage( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createVectorStorage( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__destroyStorage( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__detectDocumentClass( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createDocumentAnalyzer( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createQueryAnalyzer( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__createQueryEval( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);
bool Context__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv);

}}//namespace
#endif

