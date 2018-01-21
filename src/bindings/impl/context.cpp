/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/context.hpp"
#include "impl/storage.hpp"
#include "impl/vector.hpp"
#include "impl/query.hpp"
#include "impl/analyzer.hpp"
#include "papuga/valueVariant.hpp"
#include "papuga/serialization.hpp"
#include "papuga/errors.hpp"
#include "strus/lib/rpc_client.hpp"
#include "strus/lib/rpc_client_socket.hpp"
#include "strus/lib/module.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/lib/error.hpp"
#include "strus/databaseInterface.hpp"
#include "strus/storageInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageTransactionInterface.hpp"
#include "strus/storageDocumentInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/vectorStorageTransactionInterface.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "strus/rpcClientInterface.hpp"
#include "strus/rpcClientMessagingInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/constants.hpp"
#include "strus/base/configParser.hpp"
#include "private/internationalization.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "structDefs.hpp"
#include "traceUtils.hpp"
#include "valueVariantWrap.hpp"
#include <iostream>

using namespace strus;
using namespace strus::bindings;

static ErrorBufferInterface* createErrorBuffer_( unsigned int maxNofThreads)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_standard( 0, maxNofThreads);
	if (!errorhnd)
	{
		throw strus::runtime_error( "%s", _TXT("failed to create error buffer object"));
	}
	return errorhnd;
}

static ModuleLoaderInterface* createModuleLoader_( ErrorBufferInterface* errorhnd)
{
	ModuleLoaderInterface* rt = createModuleLoader( errorhnd);
	if (!rt)
	{
		throw strus::runtime_error( _TXT("failed to create module loader object: %s"), errorhnd->fetchError());
	}
	return rt;
}

ContextImpl::ContextImpl( const ValueVariant& descr)
	:m_errorhnd_impl()
	,m_moduleloader_impl()
	,m_rpc_impl()
	,m_trace_impl()
	,m_storage_objbuilder_impl()
	,m_analyzer_objbuilder_impl()
	,m_textproc(0)
{
	ContextDef contextdef( descr);
	ErrorBufferInterface* errorhnd = createErrorBuffer_( contextdef.threads);
	m_errorhnd_impl.resetOwnership( errorhnd, "ErrorBuffer");

	if (contextdef.rpc.empty())
	{
		ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
		m_moduleloader_impl.resetOwnership( moduleLoader, "ModuleLoader");
		if (!contextdef.trace.empty())
		{
			m_trace_impl.resetOwnership( new TraceProxy( moduleLoader, contextdef.trace, errorhnd), "TraceProxy");
		}
	}
	else
	{
		Reference<RpcClientMessagingInterface> messaging;
		messaging.reset( createRpcClientMessaging( contextdef.rpc.c_str(), errorhnd));
		if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
		m_rpc_impl.resetOwnership( createRpcClient( messaging.get(), errorhnd), "RpcClient");
		if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
		(void)messaging.release();
	}
}

void ContextImpl::loadModule( const std::string& name_)
{
	strus::unique_lock lck( m_mutex);
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( "%s", _TXT("cannot load modules in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( "%s", _TXT("tried to load modules after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( "%s", _TXT("tried to load modules after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = m_moduleloader_impl.getObject<ModuleLoaderInterface>();
	if (!moduleLoader->loadModule( name_))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error(_TXT("failed to load module: %s"), errorhnd->fetchError());
	}
}

void ContextImpl::addModulePath( const ValueVariant& paths_)
{
	strus::unique_lock lck( m_mutex);
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( "%s", _TXT("cannot add a module path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( "%s", _TXT("tried to set the module search path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( "%s", _TXT("tried to set the module search path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = m_moduleloader_impl.getObject<ModuleLoaderInterface>();
	std::vector<std::string> pathlist = Deserializer::getStringList( paths_);
	std::vector<std::string>::const_iterator pi = pathlist.begin(), pe = pathlist.end();
	for (; pi != pe; ++pi)
	{
		moduleLoader->addModulePath( *pi);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add module path: %s"), errorhnd->fetchError());
	}
}

void ContextImpl::addResourcePath( const ValueVariant& paths_)
{
	strus::unique_lock lck( m_mutex);
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( "%s", _TXT("cannot add a resource path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( "%s", _TXT("tried to add a resource path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( "%s", _TXT("tried to add a resource path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = m_moduleloader_impl.getObject<ModuleLoaderInterface>();
	std::vector<std::string> pathlist = Deserializer::getStringList( paths_);
	std::vector<std::string>::const_iterator pi = pathlist.begin(), pe = pathlist.end();
	for (; pi != pe; ++pi)
	{
		moduleLoader->addResourcePath( *pi);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add module path: %s"), errorhnd->fetchError());
	}
}

void ContextImpl::initStorageObjBuilder()
{
	strus::unique_lock lck( m_mutex);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageObjectBuilderInterface* storageObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		RpcClientInterface* client = m_rpc_impl.getObject<RpcClientInterface>();
		storageObjectBuilder = client->createStorageObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		ModuleLoaderInterface* moduleLoader = m_moduleloader_impl.getObject<ModuleLoaderInterface>();
		storageObjectBuilder = moduleLoader->createStorageObjectBuilder();
	}
	else
	{
		throw strus::runtime_error( "%s", _TXT("bad state, no context initialized"));
	}
	if (!storageObjectBuilder)
	{
		throw strus::runtime_error(_TXT("failed to create storage object builder: %s"), errorhnd->fetchError());
	}
	TraceProxy* tp = m_trace_impl.getObject<TraceProxy>();
	if (tp)
	{
		StorageObjectBuilderInterface* storageObjectBuilder_proxy = tp->createProxy( storageObjectBuilder);
		if (!storageObjectBuilder_proxy)
		{
			delete storageObjectBuilder;
			throw strus::runtime_error(_TXT("failed to create storage object builder trace proxy: %s"), errorhnd->fetchError());
		}
		storageObjectBuilder = storageObjectBuilder_proxy;
	}
	m_storage_objbuilder_impl.resetOwnership( storageObjectBuilder, "StorageObjectBuilder");
}

void ContextImpl::initAnalyzerObjBuilder()
{
	strus::unique_lock lck( m_mutex);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	AnalyzerObjectBuilderInterface* analyzerObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		RpcClientInterface* client = m_rpc_impl.getObject<RpcClientInterface>();
		analyzerObjectBuilder = client->createAnalyzerObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		ModuleLoaderInterface* moduleLoader = m_moduleloader_impl.getObject<ModuleLoaderInterface>();
		analyzerObjectBuilder = moduleLoader->createAnalyzerObjectBuilder();
	}
	else
	{
		throw strus::runtime_error( "%s", _TXT("bad state, no context initialized"));
	}
	if (!analyzerObjectBuilder)
	{
		throw strus::runtime_error( _TXT("failed to create analyzer object builder: %s"), errorhnd->fetchError());
	}
	TraceProxy* tp = m_trace_impl.getObject<TraceProxy>();
	if (tp)
	{
		AnalyzerObjectBuilderInterface* analyzerObjectBuilder_proxy = tp->createProxy( analyzerObjectBuilder);
		if (!analyzerObjectBuilder_proxy)
		{
			delete analyzerObjectBuilder;
			throw strus::runtime_error(_TXT("failed to create storage object builder trace proxy: %s"), errorhnd->fetchError());
		}
		analyzerObjectBuilder = analyzerObjectBuilder_proxy;
	}
	m_analyzer_objbuilder_impl.resetOwnership( analyzerObjectBuilder, "AnalyzerObjectBuilder");
	const AnalyzerObjectBuilderInterface* objBuilder = m_analyzer_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	m_textproc = objBuilder->getTextProcessor();
	if (!m_textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
}

void ContextImpl::endConfig()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
}

analyzer::DocumentClass* ContextImpl::detectDocumentClass( const std::string& content)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	Reference<analyzer::DocumentClass> dclass( new analyzer::DocumentClass());
	enum {MaxHdrSize = 8092};
	std::size_t hdrsize = content.size() > MaxHdrSize ? MaxHdrSize : content.size();

	if (m_textproc->detectDocumentClass( *dclass, content.c_str(), hdrsize, MaxHdrSize < content.size()))
	{
		return dclass.release();
	}
	else
	{
		if (errorhnd->hasError()) throw strus::runtime_error( _TXT("failed to detect document class: %s"), errorhnd->fetchError());
		return 0;
	}
}

StorageClientImpl* ContextImpl::createStorageClient( const ValueVariant& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return new StorageClientImpl( m_trace_impl, m_storage_objbuilder_impl, m_errorhnd_impl, Deserializer::getConfigString( config_, errorhnd));
}

VectorStorageClientImpl* ContextImpl::createVectorStorageClient( const ValueVariant& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return new VectorStorageClientImpl( m_trace_impl, m_storage_objbuilder_impl, m_errorhnd_impl, Deserializer::getConfigString( config_, errorhnd));
}

DocumentAnalyzerImpl* ContextImpl::createDocumentAnalyzer( const ValueVariant& doctype)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return new DocumentAnalyzerImpl( m_trace_impl, m_analyzer_objbuilder_impl, m_errorhnd_impl, doctype, m_textproc);
}

QueryAnalyzerImpl* ContextImpl::createQueryAnalyzer()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return new QueryAnalyzerImpl( m_trace_impl, m_analyzer_objbuilder_impl, m_errorhnd_impl);
}

QueryEvalImpl* ContextImpl::createQueryEval()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return new QueryEvalImpl( m_trace_impl, m_storage_objbuilder_impl, m_errorhnd_impl);
}

void ContextImpl::createStorage( const ValueVariant& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( Deserializer::getConfigString( config_, errorhnd));
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = m_storage_objbuilder_impl.getObject<StorageObjectBuilderInterface>();
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const StorageInterface* sti = objBuilder->getStorage();
	if (!sti) throw strus::runtime_error( _TXT("failed to get storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());
}

void ContextImpl::createVectorStorage( const ValueVariant& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagename;
	std::string storagecfg( Deserializer::getConfigString( config_, errorhnd));
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);
	if (!extractStringFromConfigString( dbname, storagename, "storage", errorhnd))
	{
		storagename = Constants::standard_vector_storage();
	}
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = m_storage_objbuilder_impl.getObject<StorageObjectBuilderInterface>();
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const VectorStorageInterface* sti = objBuilder->getVectorStorage( storagename);
	if (!sti) throw strus::runtime_error( _TXT("failed to get vector storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create vector storage: %s"), errorhnd->fetchError());
}

void ContextImpl::destroyStorage( const ValueVariant& config)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!papuga_ValueVariant_defined( &config)) throw strus::runtime_error( _TXT("called context method destroyStorage with undefined config"));
	std::string dbname;
	std::string storagecfg( Deserializer::getConfigString( config, errorhnd));
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = m_storage_objbuilder_impl.getObject<StorageObjectBuilderInterface>();
	if (!objBuilder) throw strus::runtime_error( _TXT("failed to get object builder: %s"), errorhnd->fetchError());
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	if (!dbi->destroyDatabase( storagecfg)) throw strus::runtime_error( _TXT("failed to destroy database: %s"), errorhnd->fetchError());
}

bool ContextImpl::storageExists( const ValueVariant& config)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError()) throw strus::runtime_error( _TXT("called context method storageExists with an unhandled error: %s"), errorhnd->fetchError());
	if (!papuga_ValueVariant_defined( &config)) throw strus::runtime_error( _TXT("called storageExists with undefined config"));
	std::string dbname;
	std::string storagecfg( Deserializer::getConfigString( config, errorhnd));
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = m_storage_objbuilder_impl.getObject<StorageObjectBuilderInterface>();
	if (!objBuilder) throw strus::runtime_error( _TXT("failed to get object builder: %s"), errorhnd->fetchError());
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	bool rt = dbi->exists( storagecfg);
	if (!rt && errorhnd->hasError()) throw strus::runtime_error( _TXT("failed to test if database exists: %s"), errorhnd->fetchError());
	return rt;
}

Struct ContextImpl::unpackStatisticBlob( const ValueVariant& blob_, const std::string& procname)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	const StorageObjectBuilderInterface* objBuilder = m_storage_objbuilder_impl.getObject<StorageObjectBuilderInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsProcessorInterface* statsproc = objBuilder->getStatisticsProcessor( procname);
	if (!statsproc)
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT( "error getting statistics message processor: %s"), errorhnd->fetchError());
		}
		throw strus::runtime_error(_TXT( "error statistics message processor '%s' not defined"), procname.c_str());
	}
	Struct rt;
	papuga_ErrorCode errcode = papuga_Ok;
	std::size_t bloblen;
	const void* blob = papuga_ValueVariant_toblob( &blob_, &rt.allocator, &bloblen, &errcode);
	if (errcode != papuga_Ok)
	{
		throw strus::runtime_error(_TXT( "cannot convert 1st argument to binary blob: %s"), papuga_ErrorCode_tostring( errcode));
	}
	Reference<StatisticsViewerInterface> viewer( statsproc->createViewer( blob, bloblen));
	if (!viewer.get()) throw strus::runtime_error(_TXT( "error decoding statistics from blob: %s"), errorhnd->fetchError());
	strus::bindings::Serializer::serialize( &rt.serialization, *viewer);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to deserialize statistics blob: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

void ContextImpl::close()
{
	m_analyzer_objbuilder_impl.reset();
	m_storage_objbuilder_impl.reset();
	if (m_rpc_impl.get()) (m_rpc_impl.getObject<RpcClientInterface>())->close();
	m_moduleloader_impl.reset();
}

std::string ContextImpl::debug_serialize( const ValueVariant& arg, bool deterministic)
{
	if (!papuga_ValueVariant_defined( &arg))
	{
		return std::string();
	}
	if (arg.valuetype != papuga_TypeSerialization)
	{
		return ValueVariantWrap::tostring( arg);
	}
	papuga_ErrorCode errcode = papuga_Ok;
	std::string rt;
	if (deterministic)
	{
		rt = papuga::Serialization_tostring_deterministic( *arg.value.serialization, "", errcode);
	}
	else
	{
		rt = papuga::Serialization_tostring( *arg.value.serialization, "", errcode);
	}
	if (errcode != papuga_Ok)
	{
		throw papuga::error_exception( errcode, "method debug_serialize");
	}
	return rt;
}


