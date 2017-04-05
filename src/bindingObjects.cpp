/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Identifiers for objects and methods for serialization
/// \note PROGRAM (strusBindingsGen) GENERATED FILE! DO NOT MODIFY!!!
/// \file bindingObjects.cpp
#include "strus/bindings/bindingObjects.hpp"
#include "strus/base/dll_tags.hpp"

using namespace strus;

DLL_PUBLIC bool bindings::DocumentAnalyzer__addSearchIndexFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->addSearchIndexFeature( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__addForwardIndexFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->addForwardIndexFeature( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__defineMetaData( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->defineMetaData( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAggregatedMetaData( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->defineAggregatedMetaData( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAttribute( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->defineAttribute( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->addSearchIndexFeatureFromPatternMatch( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->addForwardIndexFeatureFromPatternMatch( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__defineMetaDataFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->defineMetaDataFromPatternMatch( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAttributeFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->defineAttributeFromPatternMatch( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__definePatternMatcherPostProc( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->definePatternMatcherPostProc( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__definePatternMatcherPostProcFromFile( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->definePatternMatcherPostProcFromFile( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__defineDocument( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->defineDocument( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__analyze( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->analyze( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__analyze( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->analyze( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzer__createQueue( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
	THIS->createQueue( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzeQueue__push( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzeQueueImpl* THIS = self.getObject<DocumentAnalyzeQueueImpl>();
	THIS->push( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzeQueue__push( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzeQueueImpl* THIS = self.getObject<DocumentAnalyzeQueueImpl>();
	THIS->push( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzeQueue__hasMore( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzeQueueImpl* THIS = self.getObject<DocumentAnalyzeQueueImpl>();
	THIS->hasMore( );
}
DLL_PUBLIC bool bindings::DocumentAnalyzeQueue__fetch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentAnalyzeQueueImpl* THIS = self.getObject<DocumentAnalyzeQueueImpl>();
	THIS->fetch( );
}
DLL_PUBLIC bool bindings::QueryAnalyzer__addSearchIndexElement( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
	THIS->addSearchIndexElement( );
}
DLL_PUBLIC bool bindings::QueryAnalyzer__addSearchIndexElementFromPatternMatch( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
	THIS->addSearchIndexElementFromPatternMatch( );
}
DLL_PUBLIC bool bindings::QueryAnalyzer__addPatternLexem( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
	THIS->addPatternLexem( );
}
DLL_PUBLIC bool bindings::QueryAnalyzer__definePatternMatcherPostProc( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
	THIS->definePatternMatcherPostProc( );
}
DLL_PUBLIC bool bindings::QueryAnalyzer__definePatternMatcherPostProcFromFile( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
	THIS->definePatternMatcherPostProcFromFile( );
}
DLL_PUBLIC bool bindings::QueryAnalyzer__analyzeField( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
	THIS->analyzeField( );
}
DLL_PUBLIC bool bindings::QueryAnalyzer__createContext( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
	THIS->createContext( );
}
DLL_PUBLIC bool bindings::QueryAnalyzeContext__putField( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzeContextImpl* THIS = self.getObject<QueryAnalyzeContextImpl>();
	THIS->putField( );
}
DLL_PUBLIC bool bindings::QueryAnalyzeContext__analyze( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryAnalyzeContextImpl* THIS = self.getObject<QueryAnalyzeContextImpl>();
	THIS->analyze( );
}
DLL_PUBLIC bool bindings::StorageClient__nofDocumentsInserted( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
	THIS->nofDocumentsInserted( );
}
DLL_PUBLIC bool bindings::StorageClient__createTransaction( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
	THIS->createTransaction( );
}
DLL_PUBLIC bool bindings::StorageClient__createInitStatisticsIterator( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
	THIS->createInitStatisticsIterator( );
}
DLL_PUBLIC bool bindings::StorageClient__createUpdateStatisticsIterator( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
	THIS->createUpdateStatisticsIterator( );
}
DLL_PUBLIC bool bindings::StorageClient__createDocumentBrowser( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
	THIS->createDocumentBrowser( );
}
DLL_PUBLIC bool bindings::StorageClient__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
	THIS->close( );
}
DLL_PUBLIC bool bindings::StorageTransaction__insertDocument( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
	THIS->insertDocument( );
}
DLL_PUBLIC bool bindings::StorageTransaction__deleteDocument( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
	THIS->deleteDocument( );
}
DLL_PUBLIC bool bindings::StorageTransaction__deleteUserAccessRights( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
	THIS->deleteUserAccessRights( );
}
DLL_PUBLIC bool bindings::StorageTransaction__commit( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
	THIS->commit( );
}
DLL_PUBLIC bool bindings::StorageTransaction__rollback( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
	THIS->rollback( );
}
DLL_PUBLIC bool bindings::StatisticsIterator__getNext( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StatisticsIteratorImpl* THIS = self.getObject<StatisticsIteratorImpl>();
	THIS->getNext( );
}
DLL_PUBLIC bool bindings::StatisticsProcessor__decode( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StatisticsProcessorImpl* THIS = self.getObject<StatisticsProcessorImpl>();
	THIS->decode( );
}
DLL_PUBLIC bool bindings::StatisticsProcessor__encode( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	StatisticsProcessorImpl* THIS = self.getObject<StatisticsProcessorImpl>();
	THIS->encode( );
}
DLL_PUBLIC bool bindings::VectorStorageSearcher__findSimilar( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageSearcherImpl* THIS = self.getObject<VectorStorageSearcherImpl>();
	THIS->findSimilar( );
}
DLL_PUBLIC bool bindings::VectorStorageSearcher__findSimilarFromSelection( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageSearcherImpl* THIS = self.getObject<VectorStorageSearcherImpl>();
	THIS->findSimilarFromSelection( );
}
DLL_PUBLIC bool bindings::VectorStorageSearcher__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageSearcherImpl* THIS = self.getObject<VectorStorageSearcherImpl>();
	THIS->close( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__createSearcher( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->createSearcher( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__createTransaction( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->createTransaction( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__conceptClassNames( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->conceptClassNames( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__conceptFeatures( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->conceptFeatures( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__nofConcepts( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->nofConcepts( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__featureConcepts( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->featureConcepts( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__featureVector( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->featureVector( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__featureName( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->featureName( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__featureIndex( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->featureIndex( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__nofFeatures( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->nofFeatures( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__config( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->config( );
}
DLL_PUBLIC bool bindings::VectorStorageClient__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
	THIS->close( );
}
DLL_PUBLIC bool bindings::VectorStorageTransaction__addFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
	THIS->addFeature( );
}
DLL_PUBLIC bool bindings::VectorStorageTransaction__defineFeatureConceptRelation( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
	THIS->defineFeatureConceptRelation( );
}
DLL_PUBLIC bool bindings::VectorStorageTransaction__commit( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
	THIS->commit( );
}
DLL_PUBLIC bool bindings::VectorStorageTransaction__rollback( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
	THIS->rollback( );
}
DLL_PUBLIC bool bindings::VectorStorageTransaction__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
	THIS->close( );
}
DLL_PUBLIC bool bindings::QueryEval__addTerm( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
	THIS->addTerm( );
}
DLL_PUBLIC bool bindings::QueryEval__addSelectionFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
	THIS->addSelectionFeature( );
}
DLL_PUBLIC bool bindings::QueryEval__addRestrictionFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
	THIS->addRestrictionFeature( );
}
DLL_PUBLIC bool bindings::QueryEval__addExclusionFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
	THIS->addExclusionFeature( );
}
DLL_PUBLIC bool bindings::QueryEval__addSummarizer( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
	THIS->addSummarizer( );
}
DLL_PUBLIC bool bindings::QueryEval__addWeightingFunction( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
	THIS->addWeightingFunction( );
}
DLL_PUBLIC bool bindings::QueryEval__addWeightingFormula( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
	THIS->addWeightingFormula( );
}
DLL_PUBLIC bool bindings::QueryEval__createQuery( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
	THIS->createQuery( );
}
DLL_PUBLIC bool bindings::Query__defineFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->defineFeature( );
}
DLL_PUBLIC bool bindings::Query__defineDocFieldFeature( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->defineDocFieldFeature( );
}
DLL_PUBLIC bool bindings::Query__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->addMetaDataRestrictionCondition( );
}
DLL_PUBLIC bool bindings::Query__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->addMetaDataRestrictionCondition( );
}
DLL_PUBLIC bool bindings::Query__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->addMetaDataRestrictionCondition( );
}
DLL_PUBLIC bool bindings::Query__defineTermStatistics( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->defineTermStatistics( );
}
DLL_PUBLIC bool bindings::Query__defineGlobalStatistics( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->defineGlobalStatistics( );
}
DLL_PUBLIC bool bindings::Query__addDocumentEvaluationSet( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->addDocumentEvaluationSet( );
}
DLL_PUBLIC bool bindings::Query__setMaxNofRanks( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->setMaxNofRanks( );
}
DLL_PUBLIC bool bindings::Query__setMinRank( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->setMinRank( );
}
DLL_PUBLIC bool bindings::Query__addUserName( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->addUserName( );
}
DLL_PUBLIC bool bindings::Query__setWeightingVariables( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->setWeightingVariables( );
}
DLL_PUBLIC bool bindings::Query__setDebugMode( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->setDebugMode( );
}
DLL_PUBLIC bool bindings::Query__evaluate( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->evaluate( );
}
DLL_PUBLIC bool bindings::Query__tostring( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	QueryImpl* THIS = self.getObject<QueryImpl>();
	THIS->tostring( );
}
DLL_PUBLIC bool bindings::DocumentBrowser__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentBrowserImpl* THIS = self.getObject<DocumentBrowserImpl>();
	THIS->addMetaDataRestrictionCondition( );
}
DLL_PUBLIC bool bindings::DocumentBrowser__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentBrowserImpl* THIS = self.getObject<DocumentBrowserImpl>();
	THIS->addMetaDataRestrictionCondition( );
}
DLL_PUBLIC bool bindings::DocumentBrowser__addMetaDataRestrictionCondition( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentBrowserImpl* THIS = self.getObject<DocumentBrowserImpl>();
	THIS->addMetaDataRestrictionCondition( );
}
DLL_PUBLIC bool bindings::DocumentBrowser__skipDoc( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentBrowserImpl* THIS = self.getObject<DocumentBrowserImpl>();
	THIS->skipDoc( );
}
DLL_PUBLIC bool bindings::DocumentBrowser__attribute( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	DocumentBrowserImpl* THIS = self.getObject<DocumentBrowserImpl>();
	THIS->attribute( );
}
DLL_PUBLIC bool bindings::Context__checkErrors( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->checkErrors( );
}
DLL_PUBLIC bool bindings::Context__loadModule( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->loadModule( );
}
DLL_PUBLIC bool bindings::Context__addModulePath( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->addModulePath( );
}
DLL_PUBLIC bool bindings::Context__addResourcePath( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->addResourcePath( );
}
DLL_PUBLIC bool bindings::Context__createStatisticsProcessor( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createStatisticsProcessor( );
}
DLL_PUBLIC bool bindings::Context__createStorageClient( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createStorageClient( );
}
DLL_PUBLIC bool bindings::Context__createStorageClient( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createStorageClient( );
}
DLL_PUBLIC bool bindings::Context__createVectorStorageClient( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createVectorStorageClient( );
}
DLL_PUBLIC bool bindings::Context__createVectorStorageClient( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createVectorStorageClient( );
}
DLL_PUBLIC bool bindings::Context__createStorage( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createStorage( );
}
DLL_PUBLIC bool bindings::Context__createVectorStorage( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createVectorStorage( );
}
DLL_PUBLIC bool bindings::Context__destroyStorage( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->destroyStorage( );
}
DLL_PUBLIC bool bindings::Context__detectDocumentClass( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->detectDocumentClass( );
}
DLL_PUBLIC bool bindings::Context__createDocumentAnalyzer( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createDocumentAnalyzer( );
}
DLL_PUBLIC bool bindings::Context__createQueryAnalyzer( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createQueryAnalyzer( );
}
DLL_PUBLIC bool bindings::Context__createQueryEval( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->createQueryEval( );
}
DLL_PUBLIC bool bindings::Context__close( const HostObjectReference& self, callResult& retval, std::size_t argc, ValueVariant* argv)
{
	ContextImpl* THIS = self.getObject<ContextImpl>();
	THIS->close( );
}

}}//namespace
#endif

