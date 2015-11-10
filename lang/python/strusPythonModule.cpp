/*

! THIS IS A GENERATED FILE. DO NOT MODYFY IT. CALL genInterface.sh TO RECREATE IT !

---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#define FunctionObject boost::python::api::object
#include "strus/bindingObjects.hpp"
#include <string>

namespace bp = boost::python;

BOOST_PYTHON_MODULE(strus)
{
bp::class_<TermVector>("TermVector") .def( bp::vector_indexing_suite<TermVector>());
bp::class_<RankVector>("RankVector") .def( bp::vector_indexing_suite<RankVector>());
bp::class_<RankAttributeVector>("RankAttributeVector") .def( bp::vector_indexing_suite<RankAttributeVector>());
bp::class_<StringVector>("StringVector") .def( bp::vector_indexing_suite<StringVector>());
bp::class_<AttributeVector>("AttributeVector") .def( bp::vector_indexing_suite<AttributeVector>());
bp::class_<MetaDataVector>("MetaDataVector") .def( bp::vector_indexing_suite<MetaDataVector>());
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
	.def("setAttribute", &Document::setAttribute)
	.def("setAttribute", &Document::setAttribute_unicode)
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
	.def("defineMetaData", &DocumentAnalyzer::defineMetaData)
	.def("defineMetaData", &DocumentAnalyzer::defineMetaData_obj)
	.def("defineAggregatedMetaData", &DocumentAnalyzer::defineAggregatedMetaData)
	.def("defineAggregatedMetaData", &DocumentAnalyzer::defineAggregatedMetaData_obj)
	.def("defineAttribute", &DocumentAnalyzer::defineAttribute)
	.def("defineAttribute", &DocumentAnalyzer::defineAttribute_obj)
	.def("analyze", &DocumentAnalyzer::analyze_unicode_1)
	.def("analyze", &DocumentAnalyzer::analyze_unicode_2)
	.def("analyze", &DocumentAnalyzer::analyze_1)
	.def("analyze", &DocumentAnalyzer::analyze_2)
;
bp::class_<QueryAnalyzer>("QueryAnalyzer")
	.def("definePhraseType", &QueryAnalyzer::definePhraseType)
	.def("definePhraseType", &QueryAnalyzer::definePhraseType_obj)
	.def("analyzePhrase", &QueryAnalyzer::analyzePhrase)
	.def("analyzePhrase", &QueryAnalyzer::analyzePhrase_unicode)
	.def("createQueue", &QueryAnalyzer::createQueue)
;
bp::class_<QueryAnalyzeQueue>("QueryAnalyzeQueue")
	.def("push", &QueryAnalyzeQueue::push)
	.def("push", &QueryAnalyzeQueue::push_unicode)
	.def("fetch", &QueryAnalyzeQueue::fetch)
;
bp::class_<StorageClient>("StorageClient")
	.def("nofDocumentsInserted", &StorageClient::nofDocumentsInserted)
	.def("insertDocument", &StorageClient::insertDocument)
	.def("deleteDocument", &StorageClient::deleteDocument)
	.def("deleteDocument", &StorageClient::deleteDocument_unicode)
	.def("deleteUserAccessRights", &StorageClient::deleteUserAccessRights)
	.def("deleteUserAccessRights", &StorageClient::deleteUserAccessRights_unicode)
	.def("flush", &StorageClient::flush)
	.def("close", &StorageClient::close)
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
	.def("createQuery", &QueryEval::createQuery)
;
bp::class_<RankAttribute>("RankAttribute")
	.def("name", &RankAttribute::name, bp::return_value_policy<bp::copy_const_reference>())
	.def("value", &RankAttribute::value, bp::return_value_policy<bp::copy_const_reference>())
	.def("weight", &RankAttribute::weight)
	.def("ucvalue", &RankAttribute::ucvalue)
;
bp::class_<Rank>("Rank")
	.def("docno", &Rank::docno)
	.def("weight", &Rank::weight)
	.def("attributes", &Rank::attributes, bp::return_value_policy<bp::copy_const_reference>())
;
bp::class_<QueryExpression>("QueryExpression")
	.def("pushTerm", &QueryExpression::pushTerm)
	.def("pushTerm", &QueryExpression::pushTerm_unicode)
	.def("pushExpression", &QueryExpression::pushExpression_2)
	.def("pushExpression", &QueryExpression::pushExpression_3)
	.def("pushExpression", &QueryExpression::pushExpression_4)
	.def("attachVariable", &QueryExpression::attachVariable)
	.def("attachVariable", &QueryExpression::attachVariable_unicode)
	.def("add", &QueryExpression::add)
	.def("size", &QueryExpression::size)
;
bp::class_<Query>("Query")
	.def("defineFeature", &Query::defineFeature_2)
	.def("defineFeature", &Query::defineFeature_3)
	.def("defineMetaDataRestriction", &Query::defineMetaDataRestriction_double_3)
	.def("defineMetaDataRestriction", &Query::defineMetaDataRestriction_double_4)
	.def("defineMetaDataRestriction", &Query::defineMetaDataRestriction_uint_3)
	.def("defineMetaDataRestriction", &Query::defineMetaDataRestriction_uint_4)
	.def("defineMetaDataRestriction", &Query::defineMetaDataRestriction_int_3)
	.def("defineMetaDataRestriction", &Query::defineMetaDataRestriction_int_4)
	.def("addDocumentEvaluationSet", &Query::addDocumentEvaluationSet)
	.def("setMaxNofRanks", &Query::setMaxNofRanks)
	.def("setMinRank", &Query::setMinRank)
	.def("addUserName", &Query::addUserName)
	.def("addUserName", &Query::addUserName_unicode)
	.def("evaluate", &Query::evaluate)
;
bp::class_<Context>("Context",bp::init<>())
	.def(bp::init<const std::string&>())
	.def(bp::init<const unsigned int>())
	.def(bp::init<const std::string&, unsigned int>())
	.def("loadModule", &Context::loadModule)
	.def("addModulePath", &Context::addModulePath)
	.def("addModulePath", &Context::addModulePath_unicode)
	.def("addResourcePath", &Context::addResourcePath)
	.def("addResourcePath", &Context::addResourcePath_unicode)
	.def("createStorageClient", &Context::createStorageClient_0)
	.def("createStorageClient", &Context::createStorageClient_1)
	.def("createStorageClient", &Context::createStorageClient_unicode)
	.def("createStorage", &Context::createStorage)
	.def("createStorage", &Context::createStorage_unicode)
	.def("destroyStorage", &Context::destroyStorage)
	.def("destroyStorage", &Context::destroyStorage_unicode)
	.def("detectDocumentClass", &Context::detectDocumentClass)
	.def("detectDocumentClass", &Context::detectDocumentClass_unicode)
	.def("createDocumentAnalyzer", &Context::createDocumentAnalyzer_unicode)
	.def("createDocumentAnalyzer", &Context::createDocumentAnalyzer_0)
	.def("createDocumentAnalyzer", &Context::createDocumentAnalyzer_1)
	.def("createQueryAnalyzer", &Context::createQueryAnalyzer)
	.def("createQueryEval", &Context::createQueryEval)
	.def("close", &Context::close)
;
};
