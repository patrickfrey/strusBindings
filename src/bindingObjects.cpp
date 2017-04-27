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

using namespace strus;
using namespace strus::bindings;

#define CATCH_METHOD_CALL_ERROR( retval, classname, methodname)\
	catch (const std::runtime_error& err)\
	{\
		retval.reportError( _TXT("error calling method %s::%s(): %s"), classnam.c_str(), methodnam.c_str(), err.what());\
	} catch (const std::bad_alloc& err)\
	{\
		retval.reportError( _TXT("out of memory calling method %s::%s()"), classnam.c_str(), methodnam.c_str());\
	}\
	} catch (const std::exception& err)\
	{\
		retval.reportError( _TXT("uncaught exception calling method %s::%s(): %s"), classnam.c_str(), methodnam.c_str(), err.what());\
	}\
	return false;\
}

DLL_PUBLIC bool bindings::Context__getLastError( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->getLastError();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__loadModule( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->loadModule( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__addModulePath( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->addModulePath( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__addResourcePath( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->addResourcePath( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createStatisticsProcessor( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createStatisticsProcessor( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createStorageClient( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createStorageClient();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createStorageClient( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createStorageClient( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createVectorStorageClient( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createVectorStorageClient();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createVectorStorageClient( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createVectorStorageClient( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createStorage( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createStorage( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createVectorStorage( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createVectorStorage( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__destroyStorage( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->destroyStorage( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__detectDocumentClass( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->detectDocumentClass( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createDocumentAnalyzer( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
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
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createQueryAnalyzer( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createQueryAnalyzer();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createQueryEval( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createQueryEval();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = (ContextImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__nofDocumentsInserted( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->nofDocumentsInserted();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__createTransaction( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createTransaction();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__createInitStatisticsIterator( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->createInitStatisticsIterator( VariantValueConv::tobool( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__createUpdateStatisticsIterator( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createUpdateStatisticsIterator();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__createDocumentBrowser( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createDocumentBrowser();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__config( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->config();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = (StorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageTransaction__insertDocument( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->insertDocument( VariantValueConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageTransaction__deleteDocument( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->deleteDocument( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageTransaction__deleteUserAccessRights( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->deleteUserAccessRights( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageTransaction__commit( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->commit();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageTransaction__rollback( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = (StorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->rollback();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentBrowser__addMetaDataRestrictionCondition( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = (DocumentBrowserImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->addMetaDataRestrictionCondition( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], VariantValueConv::tobool( argv[3])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentBrowser__skipDoc( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = (DocumentBrowserImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->skipDoc( VariantValueConv::toint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentBrowser__get( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = (DocumentBrowserImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->get( VariantValueConv::toint( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StatisticsIterator__getNext( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StatisticsIteratorImpl* THIS = (StatisticsIteratorImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->getNext();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StatisticsProcessor__decode( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StatisticsProcessorImpl* THIS = (StatisticsProcessorImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->decode( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StatisticsProcessor__encode( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
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
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageSearcher__findSimilar( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = (VectorStorageSearcherImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->findSimilar( argv[0], VariantValueConv::touint( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageSearcher__findSimilarFromSelection( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = (VectorStorageSearcherImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->findSimilarFromSelection( argv[0], argv[1], VariantValueConv::touint( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageSearcher__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = (VectorStorageSearcherImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__createSearcher( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->createSearcher( VariantValueConv::toint( argv[0]), VariantValueConv::toint( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__createTransaction( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createTransaction();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__conceptClassNames( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->conceptClassNames();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__conceptFeatures( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->conceptFeatures( VariantValueConv::tostring( argv[0]), VariantValueConv::toint( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__nofConcepts( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->nofConcepts( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__featureConcepts( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->featureConcepts( VariantValueConv::tostring( argv[0]), VariantValueConv::toint( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__featureVector( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->featureVector( VariantValueConv::toint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__featureName( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->featureName( VariantValueConv::toint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__featureIndex( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->featureIndex( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__nofFeatures( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->nofFeatures();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__config( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->config();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = (VectorStorageClientImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageTransaction__addFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->addFeature( VariantValueConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageTransaction__defineFeatureConceptRelation( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->defineFeatureConceptRelation( VariantValueConv::tostring( argv[0]), VariantValueConv::toint( argv[1]), VariantValueConv::toint( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageTransaction__commit( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->commit();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageTransaction__rollback( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->rollback();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageTransaction__close( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = (VectorStorageTransactionImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__addSearchIndexFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 5) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 5: retval = THIS->addSearchIndexFeature( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3], argv[4]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__addForwardIndexFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 5) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 5: retval = THIS->addForwardIndexFeature( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3], argv[4]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineMetaData( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->defineMetaData( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAggregatedMetaData( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->defineAggregatedMetaData( VariantValueConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAttribute( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->defineAttribute( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->addSearchIndexFeatureFromPatternMatch( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->addForwardIndexFeatureFromPatternMatch( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineMetaDataFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->defineMetaDataFromPatternMatch( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAttributeFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->defineAttributeFromPatternMatch( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__definePatternMatcherPostProc( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->definePatternMatcherPostProc( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__definePatternMatcherPostProcFromFile( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->definePatternMatcherPostProcFromFile( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), VariantValueConv::tostring( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineDocument( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->defineDocument( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__analyze( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->analyze( VariantValueConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__analyze( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = (DocumentAnalyzerImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->analyze( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryAnalyzer__addSearchIndexElement( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->addSearchIndexElement( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryAnalyzer__addSearchIndexElementFromPatternMatch( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->addSearchIndexElementFromPatternMatch( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryAnalyzer__addPatternLexem( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->addPatternLexem( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryAnalyzer__definePatternMatcherPostProc( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->definePatternMatcherPostProc( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], argv[3]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryAnalyzer__definePatternMatcherPostProcFromFile( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->definePatternMatcherPostProcFromFile( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), VariantValueConv::tostring( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryAnalyzer__defineImplicitGroupBy( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = (QueryAnalyzerImpl*)(self);
		if (argc < 5) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 5: retval = THIS->defineImplicitGroupBy( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), VariantValueConv::toint( argv[2]), VariantValueConv::touint( argv[3]), VariantValueConv::tostring( argv[4])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryAnalyzer__analyze( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
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
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryEval__addTerm( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->addTerm( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), VariantValueConv::tostring( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryEval__addSelectionFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->addSelectionFeature( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryEval__addRestrictionFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->addRestrictionFeature( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryEval__addExclusionFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->addExclusionFeature( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryEval__addSummarizer( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->addSummarizer( VariantValueConv::tostring( argv[0]), argv[1], argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryEval__addWeightingFunction( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->addWeightingFunction( VariantValueConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryEval__addWeightingFormula( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = (QueryEvalImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->addWeightingFormula( VariantValueConv::tostring( argv[0]), argv[1]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::QueryEval__createQuery( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
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
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__defineFeature( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 2) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 2: retval = THIS->defineFeature( VariantValueConv::tostring( argv[0]), argv[1], 1.0); break;
			case 3: retval = THIS->defineFeature( VariantValueConv::tostring( argv[0]), argv[1], VariantValueConv::todouble( argv[2])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__addMetaDataRestrictionCondition( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 4) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 4: retval = THIS->addMetaDataRestrictionCondition( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2], VariantValueConv::tobool( argv[3])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__defineTermStatistics( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 3) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 3: retval = THIS->defineTermStatistics( VariantValueConv::tostring( argv[0]), VariantValueConv::tostring( argv[1]), argv[2]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__defineGlobalStatistics( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->defineGlobalStatistics( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__addDocumentEvaluationSet( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->addDocumentEvaluationSet( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__setMaxNofRanks( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->setMaxNofRanks( VariantValueConv::touint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__setMinRank( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->setMinRank( VariantValueConv::touint( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__addUserName( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->addUserName( VariantValueConv::tostring( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__setWeightingVariables( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->setWeightingVariables( argv[0]); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__setDebugMode( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc < 1) throw strus::runtime_error(_TXT("too few arguments"));
		switch (argc)
		{
			case 1: retval = THIS->setDebugMode( VariantValueConv::tobool( argv[0])); break;
			default: throw strus::runtime_error(_TXT("too many arguments"));
		}
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__evaluate( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->evaluate();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__tostring( void* self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = (QueryImpl*)(self);
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->tostring();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

