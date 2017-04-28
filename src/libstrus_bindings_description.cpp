/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Strus interface description used for generating language bindings
/// \note PROGRAM (strusBindingsGen) GENERATED FILE! DO NOT MODIFY!!!
/// \file bindings_description.cpp
#include "strus/lib/bindings_description.hpp"
#include "strus/bindingClassId.hpp"
#include "strus/base/dll_tags.hpp"
#include "internationaliation.hpp"
#include <cstddef>

using namespace strus;


static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_Context[16] = 
{
	{"getLastError", "Context__getLastError", 0},
	{"loadModule", "Context__loadModule", 1},
	{"addModulePath", "Context__addModulePath", 1},
	{"addResourcePath", "Context__addResourcePath", 1},
	{"createStatisticsProcessor", "Context__createStatisticsProcessor", 1},
	{"createStorageClient", "Context__createStorageClient", 1},
	{"createVectorStorageClient", "Context__createVectorStorageClient", 1},
	{"createStorage", "Context__createStorage", 1},
	{"createVectorStorage", "Context__createVectorStorage", 1},
	{"destroyStorage", "Context__destroyStorage", 1},
	{"detectDocumentClass", "Context__detectDocumentClass", 1},
	{"createDocumentAnalyzer", "Context__createDocumentAnalyzer", 1},
	{"createQueryAnalyzer", "Context__createQueryAnalyzer", 0},
	{"createQueryEval", "Context__createQueryEval", 0},
	{"close", "Context__close", 0},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_StorageClient[8] = 
{
	{"nofDocumentsInserted", "StorageClient__nofDocumentsInserted", 0},
	{"createTransaction", "StorageClient__createTransaction", 0},
	{"createInitStatisticsIterator", "StorageClient__createInitStatisticsIterator", 1},
	{"createUpdateStatisticsIterator", "StorageClient__createUpdateStatisticsIterator", 0},
	{"createDocumentBrowser", "StorageClient__createDocumentBrowser", 0},
	{"config", "StorageClient__config", 0},
	{"close", "StorageClient__close", 0},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_StorageTransaction[6] = 
{
	{"insertDocument", "StorageTransaction__insertDocument", 2},
	{"deleteDocument", "StorageTransaction__deleteDocument", 1},
	{"deleteUserAccessRights", "StorageTransaction__deleteUserAccessRights", 1},
	{"commit", "StorageTransaction__commit", 0},
	{"rollback", "StorageTransaction__rollback", 0},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_DocumentBrowser[4] = 
{
	{"addMetaDataRestrictionCondition", "DocumentBrowser__addMetaDataRestrictionCondition", 4},
	{"skipDoc", "DocumentBrowser__skipDoc", 1},
	{"get", "DocumentBrowser__get", 2},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_StatisticsIterator[2] = 
{
	{"getNext", "StatisticsIterator__getNext", 0},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_StatisticsProcessor[3] = 
{
	{"decode", "StatisticsProcessor__decode", 1},
	{"encode", "StatisticsProcessor__encode", 1},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_VectorStorageSearcher[4] = 
{
	{"findSimilar", "VectorStorageSearcher__findSimilar", 2},
	{"findSimilarFromSelection", "VectorStorageSearcher__findSimilarFromSelection", 3},
	{"close", "VectorStorageSearcher__close", 0},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_VectorStorageClient[13] = 
{
	{"createSearcher", "VectorStorageClient__createSearcher", 2},
	{"createTransaction", "VectorStorageClient__createTransaction", 0},
	{"conceptClassNames", "VectorStorageClient__conceptClassNames", 0},
	{"conceptFeatures", "VectorStorageClient__conceptFeatures", 2},
	{"nofConcepts", "VectorStorageClient__nofConcepts", 1},
	{"featureConcepts", "VectorStorageClient__featureConcepts", 2},
	{"featureVector", "VectorStorageClient__featureVector", 1},
	{"featureName", "VectorStorageClient__featureName", 1},
	{"featureIndex", "VectorStorageClient__featureIndex", 1},
	{"nofFeatures", "VectorStorageClient__nofFeatures", 0},
	{"config", "VectorStorageClient__config", 0},
	{"close", "VectorStorageClient__close", 0},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_VectorStorageTransaction[6] = 
{
	{"addFeature", "VectorStorageTransaction__addFeature", 2},
	{"defineFeatureConceptRelation", "VectorStorageTransaction__defineFeatureConceptRelation", 3},
	{"commit", "VectorStorageTransaction__commit", 0},
	{"rollback", "VectorStorageTransaction__rollback", 0},
	{"close", "VectorStorageTransaction__close", 0},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_DocumentAnalyzer[14] = 
{
	{"addSearchIndexFeature", "DocumentAnalyzer__addSearchIndexFeature", 5},
	{"addForwardIndexFeature", "DocumentAnalyzer__addForwardIndexFeature", 5},
	{"defineMetaData", "DocumentAnalyzer__defineMetaData", 4},
	{"defineAggregatedMetaData", "DocumentAnalyzer__defineAggregatedMetaData", 2},
	{"defineAttribute", "DocumentAnalyzer__defineAttribute", 4},
	{"addSearchIndexFeatureFromPatternMatch", "DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch", 4},
	{"addForwardIndexFeatureFromPatternMatch", "DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch", 4},
	{"defineMetaDataFromPatternMatch", "DocumentAnalyzer__defineMetaDataFromPatternMatch", 3},
	{"defineAttributeFromPatternMatch", "DocumentAnalyzer__defineAttributeFromPatternMatch", 3},
	{"definePatternMatcherPostProc", "DocumentAnalyzer__definePatternMatcherPostProc", 4},
	{"definePatternMatcherPostProcFromFile", "DocumentAnalyzer__definePatternMatcherPostProcFromFile", 3},
	{"defineDocument", "DocumentAnalyzer__defineDocument", 2},
	{"analyze", "DocumentAnalyzer__analyze", 2},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_QueryAnalyzer[8] = 
{
	{"addSearchIndexElement", "QueryAnalyzer__addSearchIndexElement", 4},
	{"addSearchIndexElementFromPatternMatch", "QueryAnalyzer__addSearchIndexElementFromPatternMatch", 3},
	{"addPatternLexem", "QueryAnalyzer__addPatternLexem", 4},
	{"definePatternMatcherPostProc", "QueryAnalyzer__definePatternMatcherPostProc", 4},
	{"definePatternMatcherPostProcFromFile", "QueryAnalyzer__definePatternMatcherPostProcFromFile", 3},
	{"defineImplicitGroupBy", "QueryAnalyzer__defineImplicitGroupBy", 5},
	{"analyze", "QueryAnalyzer__analyze", 1},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_QueryEval[9] = 
{
	{"addTerm", "QueryEval__addTerm", 3},
	{"addSelectionFeature", "QueryEval__addSelectionFeature", 1},
	{"addRestrictionFeature", "QueryEval__addRestrictionFeature", 1},
	{"addExclusionFeature", "QueryEval__addExclusionFeature", 1},
	{"addSummarizer", "QueryEval__addSummarizer", 3},
	{"addWeightingFunction", "QueryEval__addWeightingFunction", 2},
	{"addWeightingFormula", "QueryEval__addWeightingFormula", 2},
	{"createQuery", "QueryEval__createQuery", 1},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Method g_methods_Query[13] = 
{
	{"defineFeature", "Query__defineFeature", 3},
	{"addMetaDataRestrictionCondition", "Query__addMetaDataRestrictionCondition", 4},
	{"defineTermStatistics", "Query__defineTermStatistics", 3},
	{"defineGlobalStatistics", "Query__defineGlobalStatistics", 1},
	{"addDocumentEvaluationSet", "Query__addDocumentEvaluationSet", 1},
	{"setMaxNofRanks", "Query__setMaxNofRanks", 1},
	{"setMinRank", "Query__setMinRank", 1},
	{"addUserName", "Query__addUserName", 1},
	{"setWeightingVariables", "Query__setWeightingVariables", 1},
	{"setDebugMode", "Query__setDebugMode", 1},
	{"evaluate", "Query__evaluate", 0},
	{"tostring", "Query__tostring", 0},
	{0,0,0}
};
static const papuga::LanguageInterface::InterfaceDescription::Class g_classes[14] = 
{
	{ ClassContext, "Context", g_methods_Context},
	{ ClassStorageClient, "StorageClient", g_methods_StorageClient},
	{ ClassStorageTransaction, "StorageTransaction", g_methods_StorageTransaction},
	{ ClassDocumentBrowser, "DocumentBrowser", g_methods_DocumentBrowser},
	{ ClassStatisticsIterator, "StatisticsIterator", g_methods_StatisticsIterator},
	{ ClassStatisticsProcessor, "StatisticsProcessor", g_methods_StatisticsProcessor},
	{ ClassVectorStorageSearcher, "VectorStorageSearcher", g_methods_VectorStorageSearcher},
	{ ClassVectorStorageClient, "VectorStorageClient", g_methods_VectorStorageClient},
	{ ClassVectorStorageTransaction, "VectorStorageTransaction", g_methods_VectorStorageTransaction},
	{ ClassDocumentAnalyzer, "DocumentAnalyzer", g_methods_DocumentAnalyzer},
	{ ClassQueryAnalyzer, "QueryAnalyzer", g_methods_QueryAnalyzer},
	{ ClassQueryEval, "QueryEval", g_methods_QueryEval},
	{ ClassQuery, "Query", g_methods_Query},
	{0,0}
};

static const papuga::LanguageInterface::InterfaceDescription g_descr = { "strus", g_classes };

DLL_PUBLIC const papuga::LanguageInterface::InterfaceDescription* strus::getBindingsInterfaceDescription()
{
	return &g_descr;
}
