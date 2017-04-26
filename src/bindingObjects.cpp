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

DLL_PUBLIC bool bindings::Context__getLastError( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->getLastError();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__loadModule( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__addModulePath( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__addResourcePath( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__createStatisticsProcessor( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__createStorageClient( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createStorageClient();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createStorageClient( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__createVectorStorageClient( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createVectorStorageClient();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createVectorStorageClient( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__createStorage( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__createVectorStorage( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__destroyStorage( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__detectDocumentClass( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__createDocumentAnalyzer( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
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

DLL_PUBLIC bool bindings::Context__createQueryAnalyzer( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createQueryAnalyzer();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__createQueryEval( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createQueryEval();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Context__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		ContextImpl* THIS = self.getObject<ContextImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__nofDocumentsInserted( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->nofDocumentsInserted();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__createTransaction( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createTransaction();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__createInitStatisticsIterator( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
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

DLL_PUBLIC bool bindings::StorageClient__createUpdateStatisticsIterator( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createUpdateStatisticsIterator();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__createDocumentBrowser( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createDocumentBrowser();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__config( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->config();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageClient__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageClientImpl* THIS = self.getObject<StorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageTransaction__insertDocument( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
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

DLL_PUBLIC bool bindings::StorageTransaction__deleteDocument( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
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

DLL_PUBLIC bool bindings::StorageTransaction__deleteUserAccessRights( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
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

DLL_PUBLIC bool bindings::StorageTransaction__commit( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->commit();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StorageTransaction__rollback( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StorageTransactionImpl* THIS = self.getObject<StorageTransactionImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->rollback();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentBrowser__addMetaDataRestrictionCondition( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = self.getObject<DocumentBrowserImpl>();
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

DLL_PUBLIC bool bindings::DocumentBrowser__skipDoc( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = self.getObject<DocumentBrowserImpl>();
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

DLL_PUBLIC bool bindings::DocumentBrowser__get( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentBrowserImpl* THIS = self.getObject<DocumentBrowserImpl>();
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

DLL_PUBLIC bool bindings::StatisticsIterator__getNext( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StatisticsIteratorImpl* THIS = self.getObject<StatisticsIteratorImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->getNext();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::StatisticsProcessor__decode( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StatisticsProcessorImpl* THIS = self.getObject<StatisticsProcessorImpl>();
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

DLL_PUBLIC bool bindings::StatisticsProcessor__encode( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		StatisticsProcessorImpl* THIS = self.getObject<StatisticsProcessorImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageSearcher__findSimilar( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = self.getObject<VectorStorageSearcherImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageSearcher__findSimilarFromSelection( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = self.getObject<VectorStorageSearcherImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageSearcher__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageSearcherImpl* THIS = self.getObject<VectorStorageSearcherImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__createSearcher( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageClient__createTransaction( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->createTransaction();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__conceptClassNames( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->conceptClassNames();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__conceptFeatures( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageClient__nofConcepts( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageClient__featureConcepts( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageClient__featureVector( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageClient__featureName( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageClient__featureIndex( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageClient__nofFeatures( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->nofFeatures();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__config( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->config();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageClient__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageClientImpl* THIS = self.getObject<VectorStorageClientImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageTransaction__addFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageTransaction__defineFeatureConceptRelation( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
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

DLL_PUBLIC bool bindings::VectorStorageTransaction__commit( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->commit();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageTransaction__rollback( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->rollback();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::VectorStorageTransaction__close( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		VectorStorageTransactionImpl* THIS = self.getObject<VectorStorageTransactionImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->close();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::DocumentAnalyzer__addSearchIndexFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__addForwardIndexFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineMetaData( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAggregatedMetaData( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAttribute( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__addSearchIndexFeatureFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__addForwardIndexFeatureFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineMetaDataFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineAttributeFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__definePatternMatcherPostProc( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__definePatternMatcherPostProcFromFile( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__defineDocument( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__analyze( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::DocumentAnalyzer__analyze( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		DocumentAnalyzerImpl* THIS = self.getObject<DocumentAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::QueryAnalyzer__addSearchIndexElement( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::QueryAnalyzer__addSearchIndexElementFromPatternMatch( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::QueryAnalyzer__addPatternLexem( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::QueryAnalyzer__definePatternMatcherPostProc( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::QueryAnalyzer__definePatternMatcherPostProcFromFile( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::QueryAnalyzer__defineImplicitGroupBy( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::QueryAnalyzer__analyze( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryAnalyzerImpl* THIS = self.getObject<QueryAnalyzerImpl>();
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

DLL_PUBLIC bool bindings::QueryEval__addTerm( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
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

DLL_PUBLIC bool bindings::QueryEval__addSelectionFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
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

DLL_PUBLIC bool bindings::QueryEval__addRestrictionFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
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

DLL_PUBLIC bool bindings::QueryEval__addExclusionFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
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

DLL_PUBLIC bool bindings::QueryEval__addSummarizer( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
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

DLL_PUBLIC bool bindings::QueryEval__addWeightingFunction( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
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

DLL_PUBLIC bool bindings::QueryEval__addWeightingFormula( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
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

DLL_PUBLIC bool bindings::QueryEval__createQuery( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryEvalImpl* THIS = self.getObject<QueryEvalImpl>();
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

DLL_PUBLIC bool bindings::Query__defineFeature( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__addMetaDataRestrictionCondition( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__defineTermStatistics( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__defineGlobalStatistics( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__addDocumentEvaluationSet( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__setMaxNofRanks( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__setMinRank( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__addUserName( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__setWeightingVariables( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__setDebugMode( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
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

DLL_PUBLIC bool bindings::Query__evaluate( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->evaluate();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

DLL_PUBLIC bool bindings::Query__tostring( const HostObjectReference& self, CallResult& retval, std::size_t argc, ValueVariant const* argv)
{
	try {
		QueryImpl* THIS = self.getObject<QueryImpl>();
		if (argc > 0) throw strus::runtime_error(_TXT("too many arguments"));
		retval = THIS->tostring();
		return true;
	}
	CATCH_METHOD_CALL_ERROR( retval, ci->name().c_str(), mi->name().c_str());
}

