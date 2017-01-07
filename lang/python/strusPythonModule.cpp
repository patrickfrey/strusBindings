// ! THIS IS A GENERATED FILE. DO NOT MODYFY IT. CALL genInterface.sh TO RECREATE IT !
/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "strus/bindingObjects.hpp"
#include <string>
#include <stdexcept>

namespace bp = boost::python;

namespace {
void translate_runtime_error( std::runtime_error const& err)
{
	PyErr_SetString( PyExc_Exception, err.what());
}
void translate_bad_alloc( std::bad_alloc const& err)
{
	PyErr_SetString( PyExc_MemoryError, "out of memory");
}
void translate_logic_error( std::logic_error const& err)
{
	PyErr_SetString( PyExc_AssertionError, err.what());
}
void translate_exception( std::exception const& err)
{
	PyErr_SetString( PyExc_Exception, err.what());
}
}

BOOST_PYTHON_MODULE(strus)
{
bp::register_exception_translator<std::runtime_error>( translate_runtime_error);
bp::register_exception_translator<std::bad_alloc>( translate_bad_alloc);
bp::register_exception_translator<std::logic_error>( translate_logic_error);
bp::register_exception_translator<std::exception>( translate_exception);

bp::class_<TermVector>("TermVector") .def( bp::vector_indexing_suite<TermVector>());
bp::class_<RankVector>("RankVector") .def( bp::vector_indexing_suite<RankVector>());
bp::class_<SummaryElementVector>("SummaryElementVector") .def( bp::vector_indexing_suite<SummaryElementVector>());
bp::class_<StringVector>("StringVector") .def( bp::vector_indexing_suite<StringVector>());
bp::class_<AttributeVector>("AttributeVector") .def( bp::vector_indexing_suite<AttributeVector>());
bp::class_<MetaDataVector>("MetaDataVector") .def( bp::vector_indexing_suite<MetaDataVector>());
bp::class_<DocumentFrequencyChangeVector>("DocumentFrequencyChangeVector") .def( bp::vector_indexing_suite<DocumentFrequencyChangeVector>());
bp::class_<Tokenizer>("Tokenizer")
	.def("name", &Tokenizer::name, bp::return_value_policy<bp::copy_const_reference>())
	.def("arguments", &Tokenizer::arguments, bp::return_value_policy<bp::copy_const_reference>())
	.def("setName", &Tokenizer::setName)
	.def("addArgument", &Tokenizer::addArgument)
	.def("addArgumentInt", &Tokenizer::addArgumentInt)
	.def("addArgumentFloat", &Tokenizer::addArgumentFloat)
;
bp::class_<Normalizer>("Normalizer")
	.def("name", &Normalizer::name, bp::return_value_policy<bp::copy_const_reference>())
	.def("arguments", &Normalizer::arguments, bp::return_value_policy<bp::copy_const_reference>())
	.def("setName", &Normalizer::setName)
	.def("addArgument", &Normalizer::addArgument)
	.def("addArgumentInt", &Normalizer::addArgumentInt)
	.def("addArgumentFloat", &Normalizer::addArgumentFloat)
;
bp::class_<Aggregator>("Aggregator")
	.def("name", &Aggregator::name, bp::return_value_policy<bp::copy_const_reference>())
	.def("arguments", &Aggregator::arguments, bp::return_value_policy<bp::copy_const_reference>())
	.def("setName", &Aggregator::setName)
	.def("addArgument", &Aggregator::addArgument)
	.def("addArgumentInt", &Aggregator::addArgumentInt)
	.def("addArgumentFloat", &Aggregator::addArgumentFloat)
;
bp::class_<Variant>("Variant")
	.def("defined", &Variant::defined)
	.def("type", &Variant::type)
	.def("getUInt", &Variant::getUInt)
	.def("getInt", &Variant::getInt)
	.def("getFloat", &Variant::getFloat)
	.def("getText", &Variant::getText)
	.def("init", &Variant::init)
	.def("assign", &Variant::assign)
	.def("assignUint", &Variant::assignUint)
	.def("assignInt", &Variant::assignInt)
	.def("assignFloat", &Variant::assignFloat)
	.def("assignText", &Variant::assignText)
;
bp::class_<Term>("Term")
	.def("type", &Term::type, bp::return_value_policy<bp::copy_const_reference>())
	.def("value", &Term::value, bp::return_value_policy<bp::copy_const_reference>())
	.def("ucvalue", &Term::ucvalue)
	.def("position", &Term::position)
;
bp::class_<MetaData>("MetaData")
	.def("name", &MetaData::name, bp::return_value_policy<bp::copy_const_reference>())
	.def("value", &MetaData::value)
;
bp::class_<Attribute>("Attribute")
	.def("name", &Attribute::name, bp::return_value_policy<bp::copy_const_reference>())
	.def("value", &Attribute::value, bp::return_value_policy<bp::copy_const_reference>())
	.def("ucvalue", &Attribute::ucvalue)
;
bp::class_<DocumentClass>("DocumentClass")
	.def("valid", &DocumentClass::valid)
	.def("setMimeType", &DocumentClass::setMimeType)
	.def("setScheme", &DocumentClass::setScheme)
	.def("setEncoding", &DocumentClass::setEncoding)
	.def("mimeType", &DocumentClass::mimeType, bp::return_value_policy<bp::copy_const_reference>())
	.def("scheme", &DocumentClass::scheme, bp::return_value_policy<bp::copy_const_reference>())
	.def("encoding", &DocumentClass::encoding, bp::return_value_policy<bp::copy_const_reference>())
;
bp::class_<Document>("Document")
	.def("addSearchIndexTerm", &Document::addSearchIndexTerm)
	.def("addForwardIndexTerm", &Document::addForwardIndexTerm)
	.def("setMetaData", &Document::setMetaData_double)
	.def("setMetaData", &Document::setMetaData_int)
	.def("setMetaData", &Document::setMetaData_uint)
	.def("setAttribute", &Document::setAttribute_obj)
	.def("setUserAccessRight", &Document::setUserAccessRight)
	.def("setDocid", &Document::setDocid)
	.def("searchIndexTerms", &Document::searchIndexTerms, bp::return_value_policy<bp::copy_const_reference>())
	.def("forwardIndexTerms", &Document::forwardIndexTerms, bp::return_value_policy<bp::copy_const_reference>())
	.def("metaData", &Document::metaData, bp::return_value_policy<bp::copy_const_reference>())
	.def("attributes", &Document::attributes, bp::return_value_policy<bp::copy_const_reference>())
	.def("users", &Document::users, bp::return_value_policy<bp::copy_const_reference>())
	.def("docid", &Document::docid, bp::return_value_policy<bp::copy_const_reference>())
;
bp::class_<DocumentAnalyzer>("DocumentAnalyzer")
	.def("addSearchIndexFeature", &DocumentAnalyzer::addSearchIndexFeature_4)
	.def("addSearchIndexFeature", &DocumentAnalyzer::addSearchIndexFeature_5)
	.def("addForwardIndexFeature", &DocumentAnalyzer::addForwardIndexFeature_4)
	.def("addForwardIndexFeature", &DocumentAnalyzer::addForwardIndexFeature_5)
	.def("defineMetaData", &DocumentAnalyzer::defineMetaData_obj)
	.def("defineAggregatedMetaData", &DocumentAnalyzer::defineAggregatedMetaData_obj)
	.def("defineAttribute", &DocumentAnalyzer::defineAttribute_obj)
	.def("addSearchIndexFeatureFromPatternMatch", &DocumentAnalyzer::addSearchIndexFeatureFromPatternMatch_3)
	.def("addSearchIndexFeatureFromPatternMatch", &DocumentAnalyzer::addSearchIndexFeatureFromPatternMatch_4)
	.def("addForwardIndexFeatureFromPatternMatch", &DocumentAnalyzer::addForwardIndexFeatureFromPatternMatch_3)
	.def("addForwardIndexFeatureFromPatternMatch", &DocumentAnalyzer::addForwardIndexFeatureFromPatternMatch_4)
	.def("defineMetaDataFromPatternMatch", &DocumentAnalyzer::defineMetaDataFromPatternMatch_obj)
	.def("defineAttributeFromPatternMatch", &DocumentAnalyzer::defineAttributeFromPatternMatch_obj)
	.def("definePatternMatcherPostProc", &DocumentAnalyzer::definePatternMatcherPostProc_expr)
	.def("defineDocument", &DocumentAnalyzer::defineDocument)
	.def("analyze", &DocumentAnalyzer::analyze_obj_1)
	.def("analyze", &DocumentAnalyzer::analyze_obj_2)
	.def("createQueue", &DocumentAnalyzer::createQueue)
;
bp::class_<DocumentAnalyzeQueue>("DocumentAnalyzeQueue")
	.def("push", &DocumentAnalyzeQueue::push_obj_1)
	.def("push", &DocumentAnalyzeQueue::push_obj_2)
	.def("hasMore", &DocumentAnalyzeQueue::hasMore)
	.def("fetch", &DocumentAnalyzeQueue::fetch)
;
bp::class_<PatternMatcher>("PatternMatcher")
	.def("pushTerm", &PatternMatcher::pushTerm_obj)
	.def("pushPattern", &PatternMatcher::pushPattern_obj)
	.def("pushExpression", &PatternMatcher::pushExpression_2)
	.def("pushExpression", &PatternMatcher::pushExpression_3)
	.def("pushExpression", &PatternMatcher::pushExpression_4)
	.def("definePattern", &PatternMatcher::definePattern_obj)
	.def("attachVariable", &PatternMatcher::attachVariable_obj)
	.def("add", &PatternMatcher::add)
	.def("size", &PatternMatcher::size)
;
bp::class_<QueryAnalyzer>("QueryAnalyzer")
	.def("addSearchIndexElement", &QueryAnalyzer::addSearchIndexElement_obj)
	.def("addSearchIndexElementFromPatternMatch", &QueryAnalyzer::addSearchIndexElementFromPatternMatch_obj)
	.def("addPatternLexem", &QueryAnalyzer::addPatternLexem_obj)
	.def("definePatternMatcherPostProc", &QueryAnalyzer::definePatternMatcherPostProc_expr)
	.def("analyzeField", &QueryAnalyzer::analyzeField_obj)
	.def("createContext", &QueryAnalyzer::createContext)
;
bp::class_<QueryAnalyzeContext>("QueryAnalyzeContext")
	.def("putField", &QueryAnalyzeContext::putField)
	.def("putField_obj", &QueryAnalyzeContext::putField_obj)
;
bp::class_<StorageClient>("StorageClient")
	.def("nofDocumentsInserted", &StorageClient::nofDocumentsInserted)
	.def("createTransaction", &StorageClient::createTransaction)
	.def("createInitStatisticsIterator", &StorageClient::createInitStatisticsIterator)
	.def("createUpdateStatisticsIterator", &StorageClient::createUpdateStatisticsIterator)
	.def("createDocumentBrowser", &StorageClient::createDocumentBrowser)
	.def("close", &StorageClient::close)
;
bp::class_<StorageTransaction>("StorageTransaction")
	.def("insertDocument", &StorageTransaction::insertDocument)
	.def("deleteDocument", &StorageTransaction::deleteDocument_obj)
	.def("deleteUserAccessRights", &StorageTransaction::deleteUserAccessRights_obj)
	.def("commit", &StorageTransaction::commit)
	.def("rollback", &StorageTransaction::rollback)
;
bp::class_<DocumentFrequencyChange>("DocumentFrequencyChange")
	.def("type", &DocumentFrequencyChange::type, bp::return_value_policy<bp::copy_const_reference>())
	.def("value", &DocumentFrequencyChange::value, bp::return_value_policy<bp::copy_const_reference>())
	.def("ucvalue", &DocumentFrequencyChange::ucvalue)
	.def("increment", &DocumentFrequencyChange::increment)
;
bp::class_<StatisticsMessage>("StatisticsMessage")
	.def("nofDocumentsInsertedChange", &StatisticsMessage::nofDocumentsInsertedChange)
	.def("documentFrequencyChangeList", &StatisticsMessage::documentFrequencyChangeList, bp::return_value_policy<bp::copy_const_reference>())
;
bp::class_<StatisticsIterator>("StatisticsIterator")
	.def("getNext", &StatisticsIterator::getNext)
;
bp::class_<StatisticsProcessor>("StatisticsProcessor")
	.def("decode", &StatisticsProcessor::decode_datablob)
	.def("encode", &StatisticsProcessor::encode)
;
bp::class_<VecRank>("VecRank")
	.def("index", &VecRank::index)
	.def("weight", &VecRank::weight)
;
bp::class_<VectorStorageSearcher>("VectorStorageSearcher")
	.def("findSimilar", &VectorStorageSearcher::findSimilar)
	.def("close", &VectorStorageSearcher::close)
;
bp::class_<VectorStorageClient>("VectorStorageClient")
	.def("createSearcher", &VectorStorageClient::createSearcher)
	.def("conceptClassNames", &VectorStorageClient::conceptClassNames)
	.def("conceptFeatures", &VectorStorageClient::conceptFeatures)
	.def("nofConcepts", &VectorStorageClient::nofConcepts)
	.def("featureConcepts", &VectorStorageClient::featureConcepts)
	.def("featureVector", &VectorStorageClient::featureVector)
	.def("featureName", &VectorStorageClient::featureName)
	.def("featureIndex", &VectorStorageClient::featureIndex)
	.def("nofFeatures", &VectorStorageClient::nofFeatures)
	.def("config", &VectorStorageClient::config)
	.def("close", &VectorStorageClient::close)
;
bp::class_<FunctionVariableConfig>("FunctionVariableConfig")
	.def("defineVariable", &FunctionVariableConfig::defineVariable)
;
bp::class_<SummarizerConfig>("SummarizerConfig")
;
bp::class_<WeightingConfig>("WeightingConfig")
;
bp::class_<QueryEval>("QueryEval")
	.def("addTerm", &QueryEval::addTerm)
	.def("addSelectionFeature", &QueryEval::addSelectionFeature)
	.def("addRestrictionFeature", &QueryEval::addRestrictionFeature)
	.def("addExclusionFeature", &QueryEval::addExclusionFeature)
	.def("addSummarizer", &QueryEval::addSummarizer)
	.def("addSummarizer", &QueryEval::addSummarizer_obj)
	.def("addWeightingFunction", &QueryEval::addWeightingFunction)
	.def("addWeightingFunction", &QueryEval::addWeightingFunction_obj)
	.def("addWeightingFormula", &QueryEval::addWeightingFormula)
	.def("addWeightingFormula", &QueryEval::addWeightingFormula_obj)
	.def("createQuery", &QueryEval::createQuery)
;
bp::class_<SummaryElement>("SummaryElement")
	.def("name", &SummaryElement::name, bp::return_value_policy<bp::copy_const_reference>())
	.def("value", &SummaryElement::value, bp::return_value_policy<bp::copy_const_reference>())
	.def("weight", &SummaryElement::weight)
	.def("index", &SummaryElement::index)
	.def("ucvalue", &SummaryElement::ucvalue)
;
bp::class_<Rank>("Rank")
	.def("docno", &Rank::docno)
	.def("weight", &Rank::weight)
	.def("summaryElements", &Rank::summaryElements, bp::return_value_policy<bp::copy_const_reference>())
;
bp::class_<QueryExpression>("QueryExpression")
	.def("pushDocField", &QueryExpression::pushDocField)
	.def("pushTerm", &QueryExpression::pushTerm_obj)
	.def("pushExpression", &QueryExpression::pushExpression_2)
	.def("pushExpression", &QueryExpression::pushExpression_3)
	.def("pushExpression", &QueryExpression::pushExpression_4)
	.def("attachVariable", &QueryExpression::attachVariable_obj)
	.def("add", &QueryExpression::add)
	.def("size", &QueryExpression::size)
;
bp::class_<TermStatistics>("TermStatistics")
	.def("df", &TermStatistics::df)
	.def("set_df", &TermStatistics::set_df)
;
bp::class_<QueryResult>("QueryResult")
	.def("evaluationPass", &QueryResult::evaluationPass)
	.def("nofDocumentsRanked", &QueryResult::nofDocumentsRanked)
	.def("nofDocumentsVisited", &QueryResult::nofDocumentsVisited)
	.def("ranks", &QueryResult::ranks, bp::return_value_policy<bp::copy_const_reference>())
;
bp::class_<Query>("Query")
	.def("defineFeature", &Query::defineFeature_2)
	.def("defineFeature", &Query::defineFeature_3)
	.def("defineFeature", &Query::defineFeature_expr_2)
	.def("defineFeature", &Query::defineFeature_expr_3)
	.def("defineDocFieldFeature", &Query::defineDocFieldFeature)
	.def("addMetaDataRestrictionCondition", &Query::addMetaDataRestrictionCondition_double)
	.def("addMetaDataRestrictionCondition", &Query::addMetaDataRestrictionCondition_uint)
	.def("addMetaDataRestrictionCondition", &Query::addMetaDataRestrictionCondition_int)
	.def("defineTermStatistics", &Query::defineTermStatistics_obj_struct)
	.def("defineGlobalStatistics", &Query::defineGlobalStatistics_struct)
	.def("addDocumentEvaluationSet", &Query::addDocumentEvaluationSet_struct)
	.def("setMaxNofRanks", &Query::setMaxNofRanks)
	.def("setMinRank", &Query::setMinRank)
	.def("addUserName", &Query::addUserName_obj)
	.def("setWeightingVariables", &Query::setWeightingVariables)
	.def("setWeightingVariables", &Query::setWeightingVariables_obj)
	.def("evaluate", &Query::evaluate)
	.def("tostring", &Query::tostring)
;
bp::class_<DocumentBrowser>("DocumentBrowser")
	.def("addMetaDataRestrictionCondition", &DocumentBrowser::addMetaDataRestrictionCondition_double)
	.def("addMetaDataRestrictionCondition", &DocumentBrowser::addMetaDataRestrictionCondition_uint)
	.def("addMetaDataRestrictionCondition", &DocumentBrowser::addMetaDataRestrictionCondition_int)
	.def("skipDoc", &DocumentBrowser::skipDoc)
	.def("attribute", &DocumentBrowser::attribute)
;
bp::class_<Context>("Context",bp::init<>())
	.def(bp::init<const unsigned int>())
	.def(bp::init<const unsigned int, const std::string&>())
	.def(bp::init<const std::string&>())
	.def(bp::init<const std::string&, unsigned int>())
	.def("checkErrors", &Context::checkErrors)
	.def("loadModule", &Context::loadModule)
	.def("addModulePath", &Context::addModulePath_obj)
	.def("addResourcePath", &Context::addResourcePath_obj)
	.def("createStatisticsProcessor", &Context::createStatisticsProcessor)
	.def("createStorageClient", &Context::createStorageClient_0)
	.def("createStorageClient", &Context::createStorageClient_obj)
	.def("createVectorStorageClient", &Context::createVectorStorageClient_0)
	.def("createVectorStorageClient", &Context::createVectorStorageClient_obj)
	.def("createStorage", &Context::createStorage_obj)
	.def("destroyStorage", &Context::destroyStorage_obj)
	.def("detectDocumentClass", &Context::detectDocumentClass_obj)
	.def("createDocumentAnalyzer", &Context::createDocumentAnalyzer_obj)
	.def("createDocumentAnalyzer", &Context::createDocumentAnalyzer_0)
	.def("createQueryAnalyzer", &Context::createQueryAnalyzer)
	.def("createQueryEval", &Context::createQueryEval)
	.def("close", &Context::close)
;
}
