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
#include "strus/bindingObjects.hpp"
#include "strus/base/dll_tags.hpp"
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vector.hpp"
#include "impl/analyzer.hpp"
#include "impl/query.hpp"
#include "impl/statistics.hpp"
#include "internationalization.hpp"
#include "valueVariantConv.hpp"

using namespace strus;
using namespace strus::bindings;

#define CATCH_METHOD_CALL_ERROR( retval, classnam, methodnam)\
	catch (const std::runtime_error& err)\
	{\
		retval.reportError( _TXT("error calling method %s::%s(): %s"), classnam, methodnam, err.what());\
	}\
	catch (const std::bad_alloc& err)\
	{\
		retval.reportError( _TXT("out of memory calling method %s::%s()"), classnam, methodnam);\
	}\
	catch (const std::exception& err)\
	{\
		retval.reportError( _TXT("uncaught exception calling method %s::%s(): %s"), classnam, methodnam, err.what());\
	}\
	return false;
DLL_PUBLIC bool strus::Context__getLastError( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->getLastError();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "getLastError")
}

DLL_PUBLIC bool strus::Context__loadModule( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->loadModule( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "loadModule")
}

DLL_PUBLIC bool strus::Context__addModulePath( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->addModulePath( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "addModulePath")
}

DLL_PUBLIC bool strus::Context__addResourcePath( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->addResourcePath( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "addResourcePath")
}

DLL_PUBLIC bool strus::Context__createStatisticsProcessor( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createStatisticsProcessor( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "createStatisticsProcessor")
}

DLL_PUBLIC bool strus::Context__createStorageClient( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		switch (argc)
		{
			case 0: retval = THIS->createStorageClient( ValueVariant()); break;
			case 1: retval = THIS->createStorageClient( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "createStorageClient")
}

DLL_PUBLIC bool strus::Context__createVectorStorageClient( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		switch (argc)
		{
			case 0: retval = THIS->createVectorStorageClient( ValueVariant()); break;
			case 1: retval = THIS->createVectorStorageClient( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "createVectorStorageClient")
}

DLL_PUBLIC bool strus::Context__createStorage( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->createStorage( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "createStorage")
}

DLL_PUBLIC bool strus::Context__createVectorStorage( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->createVectorStorage( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "createVectorStorage")
}

DLL_PUBLIC bool strus::Context__destroyStorage( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->destroyStorage( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "destroyStorage")
}

DLL_PUBLIC bool strus::Context__detectDocumentClass( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->detectDocumentClass( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "detectDocumentClass")
}

DLL_PUBLIC bool strus::Context__createDocumentAnalyzer( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createDocumentAnalyzer( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "createDocumentAnalyzer")
}

DLL_PUBLIC bool strus::Context__createQueryAnalyzer( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createQueryAnalyzer();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "createQueryAnalyzer")
}

DLL_PUBLIC bool strus::Context__createQueryEval( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createQueryEval();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "createQueryEval")
}

DLL_PUBLIC bool strus::Context__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Context", "close")
}

DLL_PUBLIC bool strus::StorageClient__nofDocumentsInserted( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->nofDocumentsInserted();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageClient", "nofDocumentsInserted")
}

DLL_PUBLIC bool strus::StorageClient__createTransaction( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createTransaction();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageClient", "createTransaction")
}

DLL_PUBLIC bool strus::StorageClient__createInitStatisticsIterator( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createInitStatisticsIterator( ValueVariantConv::tobool( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageClient", "createInitStatisticsIterator")
}

DLL_PUBLIC bool strus::StorageClient__createUpdateStatisticsIterator( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createUpdateStatisticsIterator();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageClient", "createUpdateStatisticsIterator")
}

DLL_PUBLIC bool strus::StorageClient__createDocumentBrowser( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createDocumentBrowser();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageClient", "createDocumentBrowser")
}

DLL_PUBLIC bool strus::StorageClient__config( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->config();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageClient", "config")
}

DLL_PUBLIC bool strus::StorageClient__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageClient", "close")
}

DLL_PUBLIC bool strus::StorageTransaction__insertDocument( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: THIS->insertDocument( ValueVariantConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageTransaction", "insertDocument")
}

DLL_PUBLIC bool strus::StorageTransaction__deleteDocument( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->deleteDocument( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageTransaction", "deleteDocument")
}

DLL_PUBLIC bool strus::StorageTransaction__deleteUserAccessRights( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->deleteUserAccessRights( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageTransaction", "deleteUserAccessRights")
}

DLL_PUBLIC bool strus::StorageTransaction__commit( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		THIS->commit();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageTransaction", "commit")
}

DLL_PUBLIC bool strus::StorageTransaction__rollback( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		THIS->rollback();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StorageTransaction", "rollback")
}

DLL_PUBLIC bool strus::DocumentBrowser__addMetaDataRestrictionCondition( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = (DocumentBrowserImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		std::string conv_argv0;
		switch (argc)
		{
			case 4: THIS->addMetaDataRestrictionCondition( ValueVariantConv::tocharp( conv_argv0, argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], ValueVariantConv::tobool( argv[3])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentBrowser", "addMetaDataRestrictionCondition")
}

DLL_PUBLIC bool strus::DocumentBrowser__skipDoc( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = (DocumentBrowserImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->skipDoc( ValueVariantConv::toint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentBrowser", "skipDoc")
}

DLL_PUBLIC bool strus::DocumentBrowser__get( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = (DocumentBrowserImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->get( ValueVariantConv::toint( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentBrowser", "get")
}

DLL_PUBLIC bool strus::StatisticsIterator__getNext( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StatisticsIteratorImpl* THIS = (StatisticsIteratorImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->getNext();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StatisticsIterator", "getNext")
}

DLL_PUBLIC bool strus::StatisticsProcessor__decode( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StatisticsProcessorImpl* THIS = (StatisticsProcessorImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->decode( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StatisticsProcessor", "decode")
}

DLL_PUBLIC bool strus::StatisticsProcessor__encode( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		StatisticsProcessorImpl* THIS = (StatisticsProcessorImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->encode( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "StatisticsProcessor", "encode")
}

DLL_PUBLIC bool strus::VectorStorageSearcher__findSimilar( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = (VectorStorageSearcherImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->findSimilar( argv[0], ValueVariantConv::touint( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageSearcher", "findSimilar")
}

DLL_PUBLIC bool strus::VectorStorageSearcher__findSimilarFromSelection( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = (VectorStorageSearcherImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->findSimilarFromSelection( argv[0], argv[1], ValueVariantConv::touint( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageSearcher", "findSimilarFromSelection")
}

DLL_PUBLIC bool strus::VectorStorageSearcher__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = (VectorStorageSearcherImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageSearcher", "close")
}

DLL_PUBLIC bool strus::VectorStorageClient__createSearcher( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->createSearcher( ValueVariantConv::toint( argv[0]), ValueVariantConv::toint( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "createSearcher")
}

DLL_PUBLIC bool strus::VectorStorageClient__createTransaction( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createTransaction();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "createTransaction")
}

DLL_PUBLIC bool strus::VectorStorageClient__conceptClassNames( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->conceptClassNames();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "conceptClassNames")
}

DLL_PUBLIC bool strus::VectorStorageClient__conceptFeatures( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->conceptFeatures( ValueVariantConv::tostring( argv[0]), ValueVariantConv::toint( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "conceptFeatures")
}

DLL_PUBLIC bool strus::VectorStorageClient__nofConcepts( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->nofConcepts( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "nofConcepts")
}

DLL_PUBLIC bool strus::VectorStorageClient__featureConcepts( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->featureConcepts( ValueVariantConv::tostring( argv[0]), ValueVariantConv::toint( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "featureConcepts")
}

DLL_PUBLIC bool strus::VectorStorageClient__featureVector( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->featureVector( ValueVariantConv::toint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "featureVector")
}

DLL_PUBLIC bool strus::VectorStorageClient__featureName( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->featureName( ValueVariantConv::toint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "featureName")
}

DLL_PUBLIC bool strus::VectorStorageClient__featureIndex( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->featureIndex( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "featureIndex")
}

DLL_PUBLIC bool strus::VectorStorageClient__nofFeatures( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->nofFeatures();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "nofFeatures")
}

DLL_PUBLIC bool strus::VectorStorageClient__config( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->config();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "config")
}

DLL_PUBLIC bool strus::VectorStorageClient__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageClient", "close")
}

DLL_PUBLIC bool strus::VectorStorageTransaction__addFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: THIS->addFeature( ValueVariantConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageTransaction", "addFeature")
}

DLL_PUBLIC bool strus::VectorStorageTransaction__defineFeatureConceptRelation( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->defineFeatureConceptRelation( ValueVariantConv::tostring( argv[0]), ValueVariantConv::toint( argv[1]), ValueVariantConv::toint( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageTransaction", "defineFeatureConceptRelation")
}

DLL_PUBLIC bool strus::VectorStorageTransaction__commit( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->commit();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageTransaction", "commit")
}

DLL_PUBLIC bool strus::VectorStorageTransaction__rollback( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		THIS->rollback();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageTransaction", "rollback")
}

DLL_PUBLIC bool strus::VectorStorageTransaction__close( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "VectorStorageTransaction", "close")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__addSearchIndexFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 5) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 5: THIS->addSearchIndexFeature( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3], argv[4]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "addSearchIndexFeature")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__addForwardIndexFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 5) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 5: THIS->addForwardIndexFeature( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3], argv[4]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "addForwardIndexFeature")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__defineMetaData( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: THIS->defineMetaData( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "defineMetaData")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__defineAggregatedMetaData( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: THIS->defineAggregatedMetaData( ValueVariantConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "defineAggregatedMetaData")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__defineAttribute( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: THIS->defineAttribute( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "defineAttribute")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: THIS->addSearchIndexFeatureFromPatternMatch( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "addSearchIndexFeatureFromPatternMatch")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: THIS->addForwardIndexFeatureFromPatternMatch( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "addForwardIndexFeatureFromPatternMatch")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__defineMetaDataFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->defineMetaDataFromPatternMatch( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "defineMetaDataFromPatternMatch")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__defineAttributeFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->defineAttributeFromPatternMatch( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "defineAttributeFromPatternMatch")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__definePatternMatcherPostProc( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: THIS->definePatternMatcherPostProc( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "definePatternMatcherPostProc")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__definePatternMatcherPostProcFromFile( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->definePatternMatcherPostProcFromFile( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), ValueVariantConv::tostring( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "definePatternMatcherPostProcFromFile")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__defineDocument( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: THIS->defineDocument( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "defineDocument")
}

DLL_PUBLIC bool strus::DocumentAnalyzer__analyze( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->analyze( ValueVariantConv::tostring( argv[0]), ValueVariant()); break;
			case 2: retval = THIS->analyze( ValueVariantConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "DocumentAnalyzer", "analyze")
}

DLL_PUBLIC bool strus::QueryAnalyzer__addSearchIndexElement( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: THIS->addSearchIndexElement( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryAnalyzer", "addSearchIndexElement")
}

DLL_PUBLIC bool strus::QueryAnalyzer__addSearchIndexElementFromPatternMatch( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->addSearchIndexElementFromPatternMatch( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryAnalyzer", "addSearchIndexElementFromPatternMatch")
}

DLL_PUBLIC bool strus::QueryAnalyzer__addPatternLexem( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: THIS->addPatternLexem( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryAnalyzer", "addPatternLexem")
}

DLL_PUBLIC bool strus::QueryAnalyzer__definePatternMatcherPostProc( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: THIS->definePatternMatcherPostProc( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryAnalyzer", "definePatternMatcherPostProc")
}

DLL_PUBLIC bool strus::QueryAnalyzer__definePatternMatcherPostProcFromFile( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->definePatternMatcherPostProcFromFile( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), ValueVariantConv::tostring( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryAnalyzer", "definePatternMatcherPostProcFromFile")
}

DLL_PUBLIC bool strus::QueryAnalyzer__defineImplicitGroupBy( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 5) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 5: THIS->defineImplicitGroupBy( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), ValueVariantConv::toint( argv[2]), ValueVariantConv::touint( argv[3]), ValueVariantConv::tostring( argv[4])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryAnalyzer", "defineImplicitGroupBy")
}

DLL_PUBLIC bool strus::QueryAnalyzer__analyze( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->analyze( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryAnalyzer", "analyze")
}

DLL_PUBLIC bool strus::QueryEval__addTerm( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->addTerm( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), ValueVariantConv::tostring( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryEval", "addTerm")
}

DLL_PUBLIC bool strus::QueryEval__addSelectionFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->addSelectionFeature( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryEval", "addSelectionFeature")
}

DLL_PUBLIC bool strus::QueryEval__addRestrictionFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->addRestrictionFeature( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryEval", "addRestrictionFeature")
}

DLL_PUBLIC bool strus::QueryEval__addExclusionFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->addExclusionFeature( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryEval", "addExclusionFeature")
}

DLL_PUBLIC bool strus::QueryEval__addSummarizer( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->addSummarizer( ValueVariantConv::tostring( argv[0]), argv[1], argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryEval", "addSummarizer")
}

DLL_PUBLIC bool strus::QueryEval__addWeightingFunction( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: THIS->addWeightingFunction( ValueVariantConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryEval", "addWeightingFunction")
}

DLL_PUBLIC bool strus::QueryEval__addWeightingFormula( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: THIS->addWeightingFormula( ValueVariantConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryEval", "addWeightingFormula")
}

DLL_PUBLIC bool strus::QueryEval__createQuery( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createQuery( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "QueryEval", "createQuery")
}

DLL_PUBLIC bool strus::Query__defineFeature( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: THIS->defineFeature( ValueVariantConv::tostring( argv[0]), argv[1], 1.0); break;
			case 3: THIS->defineFeature( ValueVariantConv::tostring( argv[0]), argv[1], ValueVariantConv::todouble( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "defineFeature")
}

DLL_PUBLIC bool strus::Query__addMetaDataRestrictionCondition( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		std::string conv_argv0;
		switch (argc)
		{
			case 4: THIS->addMetaDataRestrictionCondition( ValueVariantConv::tocharp( conv_argv0, argv[0]), ValueVariantConv::tostring( argv[1]), argv[2], ValueVariantConv::tobool( argv[3])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "addMetaDataRestrictionCondition")
}

DLL_PUBLIC bool strus::Query__defineTermStatistics( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: THIS->defineTermStatistics( ValueVariantConv::tostring( argv[0]), ValueVariantConv::tostring( argv[1]), argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "defineTermStatistics")
}

DLL_PUBLIC bool strus::Query__defineGlobalStatistics( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->defineGlobalStatistics( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "defineGlobalStatistics")
}

DLL_PUBLIC bool strus::Query__addDocumentEvaluationSet( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->addDocumentEvaluationSet( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "addDocumentEvaluationSet")
}

DLL_PUBLIC bool strus::Query__setMaxNofRanks( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->setMaxNofRanks( ValueVariantConv::touint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "setMaxNofRanks")
}

DLL_PUBLIC bool strus::Query__setMinRank( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->setMinRank( ValueVariantConv::touint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "setMinRank")
}

DLL_PUBLIC bool strus::Query__addUserName( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->addUserName( ValueVariantConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "addUserName")
}

DLL_PUBLIC bool strus::Query__setWeightingVariables( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->setWeightingVariables( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "setWeightingVariables")
}

DLL_PUBLIC bool strus::Query__setDebugMode( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: THIS->setDebugMode( ValueVariantConv::tobool( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "setDebugMode")
}

DLL_PUBLIC bool strus::Query__evaluate( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->evaluate();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "evaluate")
}

DLL_PUBLIC bool strus::Query__tostring( void* self, papuga::CallResult& retval, std::size_t argc, papuga::ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->tostring();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, "Query", "tostring")
}

