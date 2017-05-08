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
#include "internationalization.hpp"
#include "papuga/typedefs.h"
#include <cstddef>

using namespace strus;


static const papuga_MethodDescription g_methods_Context[16] = 
{
	{"getLastError", "_strus_binding_Context__getLastError", true, 0},
	{"loadModule", "_strus_binding_Context__loadModule", true, 1},
	{"addModulePath", "_strus_binding_Context__addModulePath", true, 1},
	{"addResourcePath", "_strus_binding_Context__addResourcePath", true, 1},
	{"createStatisticsProcessor", "_strus_binding_Context__createStatisticsProcessor", true, 1},
	{"createStorageClient", "_strus_binding_Context__createStorageClient", true, 1},
	{"createVectorStorageClient", "_strus_binding_Context__createVectorStorageClient", true, 1},
	{"createStorage", "_strus_binding_Context__createStorage", true, 1},
	{"createVectorStorage", "_strus_binding_Context__createVectorStorage", true, 1},
	{"destroyStorage", "_strus_binding_Context__destroyStorage", true, 1},
	{"detectDocumentClass", "_strus_binding_Context__detectDocumentClass", true, 1},
	{"createDocumentAnalyzer", "_strus_binding_Context__createDocumentAnalyzer", true, 1},
	{"createQueryAnalyzer", "_strus_binding_Context__createQueryAnalyzer", true, 0},
	{"createQueryEval", "_strus_binding_Context__createQueryEval", true, 0},
	{"close", "_strus_binding_Context__close", true, 0},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_StorageClient[8] = 
{
	{"nofDocumentsInserted", "_strus_binding_StorageClient__nofDocumentsInserted", true, 0},
	{"createTransaction", "_strus_binding_StorageClient__createTransaction", true, 0},
	{"createInitStatisticsIterator", "_strus_binding_StorageClient__createInitStatisticsIterator", true, 1},
	{"createUpdateStatisticsIterator", "_strus_binding_StorageClient__createUpdateStatisticsIterator", true, 0},
	{"createDocumentBrowser", "_strus_binding_StorageClient__createDocumentBrowser", true, 0},
	{"config", "_strus_binding_StorageClient__config", true, 0},
	{"close", "_strus_binding_StorageClient__close", true, 0},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_StorageTransaction[6] = 
{
	{"insertDocument", "_strus_binding_StorageTransaction__insertDocument", true, 2},
	{"deleteDocument", "_strus_binding_StorageTransaction__deleteDocument", true, 1},
	{"deleteUserAccessRights", "_strus_binding_StorageTransaction__deleteUserAccessRights", true, 1},
	{"commit", "_strus_binding_StorageTransaction__commit", true, 0},
	{"rollback", "_strus_binding_StorageTransaction__rollback", true, 0},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_DocumentBrowser[4] = 
{
	{"addMetaDataRestrictionCondition", "_strus_binding_DocumentBrowser__addMetaDataRestrictionCondition", true, 4},
	{"skipDoc", "_strus_binding_DocumentBrowser__skipDoc", true, 1},
	{"get", "_strus_binding_DocumentBrowser__get", true, 2},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_StatisticsIterator[2] = 
{
	{"getNext", "_strus_binding_StatisticsIterator__getNext", true, 0},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_StatisticsProcessor[3] = 
{
	{"decode", "_strus_binding_StatisticsProcessor__decode", true, 1},
	{"encode", "_strus_binding_StatisticsProcessor__encode", true, 1},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_VectorStorageSearcher[4] = 
{
	{"findSimilar", "_strus_binding_VectorStorageSearcher__findSimilar", true, 2},
	{"findSimilarFromSelection", "_strus_binding_VectorStorageSearcher__findSimilarFromSelection", true, 3},
	{"close", "_strus_binding_VectorStorageSearcher__close", true, 0},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_VectorStorageClient[13] = 
{
	{"createSearcher", "_strus_binding_VectorStorageClient__createSearcher", true, 2},
	{"createTransaction", "_strus_binding_VectorStorageClient__createTransaction", true, 0},
	{"conceptClassNames", "_strus_binding_VectorStorageClient__conceptClassNames", true, 0},
	{"conceptFeatures", "_strus_binding_VectorStorageClient__conceptFeatures", true, 2},
	{"nofConcepts", "_strus_binding_VectorStorageClient__nofConcepts", true, 1},
	{"featureConcepts", "_strus_binding_VectorStorageClient__featureConcepts", true, 2},
	{"featureVector", "_strus_binding_VectorStorageClient__featureVector", true, 1},
	{"featureName", "_strus_binding_VectorStorageClient__featureName", true, 1},
	{"featureIndex", "_strus_binding_VectorStorageClient__featureIndex", true, 1},
	{"nofFeatures", "_strus_binding_VectorStorageClient__nofFeatures", true, 0},
	{"config", "_strus_binding_VectorStorageClient__config", true, 0},
	{"close", "_strus_binding_VectorStorageClient__close", true, 0},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_VectorStorageTransaction[6] = 
{
	{"addFeature", "_strus_binding_VectorStorageTransaction__addFeature", true, 2},
	{"defineFeatureConceptRelation", "_strus_binding_VectorStorageTransaction__defineFeatureConceptRelation", true, 3},
	{"commit", "_strus_binding_VectorStorageTransaction__commit", true, 0},
	{"rollback", "_strus_binding_VectorStorageTransaction__rollback", true, 0},
	{"close", "_strus_binding_VectorStorageTransaction__close", true, 0},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_DocumentAnalyzer[14] = 
{
	{"addSearchIndexFeature", "_strus_binding_DocumentAnalyzer__addSearchIndexFeature", true, 5},
	{"addForwardIndexFeature", "_strus_binding_DocumentAnalyzer__addForwardIndexFeature", true, 5},
	{"defineMetaData", "_strus_binding_DocumentAnalyzer__defineMetaData", true, 4},
	{"defineAggregatedMetaData", "_strus_binding_DocumentAnalyzer__defineAggregatedMetaData", true, 2},
	{"defineAttribute", "_strus_binding_DocumentAnalyzer__defineAttribute", true, 4},
	{"addSearchIndexFeatureFromPatternMatch", "_strus_binding_DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch", true, 4},
	{"addForwardIndexFeatureFromPatternMatch", "_strus_binding_DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch", true, 4},
	{"defineMetaDataFromPatternMatch", "_strus_binding_DocumentAnalyzer__defineMetaDataFromPatternMatch", true, 3},
	{"defineAttributeFromPatternMatch", "_strus_binding_DocumentAnalyzer__defineAttributeFromPatternMatch", true, 3},
	{"definePatternMatcherPostProc", "_strus_binding_DocumentAnalyzer__definePatternMatcherPostProc", true, 4},
	{"definePatternMatcherPostProcFromFile", "_strus_binding_DocumentAnalyzer__definePatternMatcherPostProcFromFile", true, 3},
	{"defineDocument", "_strus_binding_DocumentAnalyzer__defineDocument", true, 2},
	{"analyze", "_strus_binding_DocumentAnalyzer__analyze", true, 2},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_QueryAnalyzer[8] = 
{
	{"addSearchIndexElement", "_strus_binding_QueryAnalyzer__addSearchIndexElement", true, 4},
	{"addSearchIndexElementFromPatternMatch", "_strus_binding_QueryAnalyzer__addSearchIndexElementFromPatternMatch", true, 3},
	{"addPatternLexem", "_strus_binding_QueryAnalyzer__addPatternLexem", true, 4},
	{"definePatternMatcherPostProc", "_strus_binding_QueryAnalyzer__definePatternMatcherPostProc", true, 4},
	{"definePatternMatcherPostProcFromFile", "_strus_binding_QueryAnalyzer__definePatternMatcherPostProcFromFile", true, 3},
	{"defineImplicitGroupBy", "_strus_binding_QueryAnalyzer__defineImplicitGroupBy", true, 5},
	{"analyze", "_strus_binding_QueryAnalyzer__analyze", true, 1},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_QueryEval[9] = 
{
	{"addTerm", "_strus_binding_QueryEval__addTerm", true, 3},
	{"addSelectionFeature", "_strus_binding_QueryEval__addSelectionFeature", true, 1},
	{"addRestrictionFeature", "_strus_binding_QueryEval__addRestrictionFeature", true, 1},
	{"addExclusionFeature", "_strus_binding_QueryEval__addExclusionFeature", true, 1},
	{"addSummarizer", "_strus_binding_QueryEval__addSummarizer", true, 3},
	{"addWeightingFunction", "_strus_binding_QueryEval__addWeightingFunction", true, 2},
	{"addWeightingFormula", "_strus_binding_QueryEval__addWeightingFormula", true, 2},
	{"createQuery", "_strus_binding_QueryEval__createQuery", true, 1},
	{0,0,0}
};
static const papuga_MethodDescription g_methods_Query[13] = 
{
	{"defineFeature", "_strus_binding_Query__defineFeature", true, 3},
	{"addMetaDataRestrictionCondition", "_strus_binding_Query__addMetaDataRestrictionCondition", true, 4},
	{"defineTermStatistics", "_strus_binding_Query__defineTermStatistics", true, 3},
	{"defineGlobalStatistics", "_strus_binding_Query__defineGlobalStatistics", true, 1},
	{"addDocumentEvaluationSet", "_strus_binding_Query__addDocumentEvaluationSet", true, 1},
	{"setMaxNofRanks", "_strus_binding_Query__setMaxNofRanks", true, 1},
	{"setMinRank", "_strus_binding_Query__setMinRank", true, 1},
	{"addUserName", "_strus_binding_Query__addUserName", true, 1},
	{"setWeightingVariables", "_strus_binding_Query__setWeightingVariables", true, 1},
	{"setDebugMode", "_strus_binding_Query__setDebugMode", true, 1},
	{"evaluate", "_strus_binding_Query__evaluate", true, 0},
	{"tostring", "_strus_binding_Query__tostring", true, 0},
	{0,0,0}
};
static const papuga_ClassDescription g_classes[14] = 
{
	{ ClassContext, "Context", "_strus_binding_constructor__Context", "_strus_binding_destructor__Context", g_methods_Context},
	{ ClassStorageClient, "StorageClient", NULL, "_strus_binding_destructor__StorageClient", g_methods_StorageClient},
	{ ClassStorageTransaction, "StorageTransaction", NULL, "_strus_binding_destructor__StorageTransaction", g_methods_StorageTransaction},
	{ ClassDocumentBrowser, "DocumentBrowser", NULL, "_strus_binding_destructor__DocumentBrowser", g_methods_DocumentBrowser},
	{ ClassStatisticsIterator, "StatisticsIterator", NULL, "_strus_binding_destructor__StatisticsIterator", g_methods_StatisticsIterator},
	{ ClassStatisticsProcessor, "StatisticsProcessor", NULL, "_strus_binding_destructor__StatisticsProcessor", g_methods_StatisticsProcessor},
	{ ClassVectorStorageSearcher, "VectorStorageSearcher", NULL, "_strus_binding_destructor__VectorStorageSearcher", g_methods_VectorStorageSearcher},
	{ ClassVectorStorageClient, "VectorStorageClient", NULL, "_strus_binding_destructor__VectorStorageClient", g_methods_VectorStorageClient},
	{ ClassVectorStorageTransaction, "VectorStorageTransaction", NULL, "_strus_binding_destructor__VectorStorageTransaction", g_methods_VectorStorageTransaction},
	{ ClassDocumentAnalyzer, "DocumentAnalyzer", NULL, "_strus_binding_destructor__DocumentAnalyzer", g_methods_DocumentAnalyzer},
	{ ClassQueryAnalyzer, "QueryAnalyzer", NULL, "_strus_binding_destructor__QueryAnalyzer", g_methods_QueryAnalyzer},
	{ ClassQueryEval, "QueryEval", NULL, "_strus_binding_destructor__QueryEval", g_methods_QueryEval},
	{ ClassQuery, "Query", NULL, "_strus_binding_destructor__Query", g_methods_Query},
	{0,0}
};

static const papuga_ErrorDescription g_errors[] = {
	{papuga_Ok,_TXT("Ok")},
	{papuga_LogicError,_TXT("logic error")},
	{papuga_NoMemError,_TXT("out of memory")},
	{papuga_TypeError,_TXT("type mismatch")},
	{papuga_EncodingError,_TXT("string character encoding error")},
	{papuga_BufferOverflowError,_TXT("internal buffer not big enough")},
	{papuga_OutOfRangeError,_TXT("value out of range")},
	{papuga_NofArgsError,_TXT("number of arguments does not match")},
	{papuga_MissingSelf,_TXT("self argument is missing")},
	{0,NULL}
};

static const papuga_InterfaceDescription g_descr = { "strus", "strus/bindingObjects.h", g_classes, g_errors };

DLL_PUBLIC const papuga_InterfaceDescription* strus::getBindingsInterfaceDescription()
{
	return &g_descr;
}
