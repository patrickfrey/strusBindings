/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/storage.hpp"
#include "impl/statistics.hpp"
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
#include "internationalization.hpp"
#include "valueVariantConv.hpp"
#include "internationalization.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "callResultUtils.hpp"
#include "structDefs.hpp"
#include "traceUtils.hpp"

using namespace strus;
using namespace strus::bindings;

typedef papuga::Serialization Serialization;

static ErrorBufferInterface* createErrorBuffer_( unsigned int maxNofThreads)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_standard( 0, maxNofThreads);
	if (!errorhnd)
	{
		throw strus::runtime_error( _TXT("failed to create error buffer object: %s"), errorhnd->fetchError());
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

static ContextDef parseContext( const ValueVariant& ctx)
{
	if (ctx.isStringType())
	{
		return ContextDef( ValueVariantConv::tostring( ctx));
	}
	else if (ctx.type == ValueVariant::Serialization)
	{
		Serialization::const_iterator si = ctx.value.serialization->begin();
		return ContextDef( si, ctx.value.serialization->end());
	}
	else
	{
		throw strus::runtime_error(_TXT("expected string or structure for context configuration"));
	}
}

ContextImpl::ContextImpl()
	:m_errorhnd_impl()
	,m_moduleloader_impl()
	,m_rpc_impl()
	,m_trace_impl()
	,m_storage_objbuilder_impl()
	,m_analyzer_objbuilder_impl()
	,m_textproc(0)
	,m_queryproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.resetOwnership( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.resetOwnership( moduleLoader);
}

ContextImpl::ContextImpl( const ValueVariant& descr)
	:m_errorhnd_impl()
	,m_moduleloader_impl()
	,m_rpc_impl()
	,m_trace_impl()
	,m_storage_objbuilder_impl()
	,m_analyzer_objbuilder_impl()
	,m_textproc(0)
	,m_queryproc(0)
{
	ContextDef contextdef = parseContext( descr);
	ErrorBufferInterface* errorhnd = createErrorBuffer_( contextdef.threads+1);
	m_errorhnd_impl.resetOwnership( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.resetOwnership( moduleLoader);
	if (!contextdef.rpc.empty())
	{
		Reference<RpcClientMessagingInterface> messaging;
		messaging.reset( createRpcClientMessaging( contextdef.rpc.c_str(), errorhnd));
		if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
		m_rpc_impl.resetOwnership( createRpcClient( messaging.get(), errorhnd));
		if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
		(void)messaging.release();
	}
	if (!contextdef.trace.empty())
	{
		m_trace_impl.resetOwnership( new TraceProxy( moduleLoader, contextdef.trace, errorhnd));
	}
}

CallResult ContextImpl::getLastError() const
{
	CallResult rt;
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		rt.reportError( "%s", errorhnd->fetchError());
	}
	return rt;
}

void ContextImpl::loadModule( const std::string& name_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot load modules in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = m_moduleloader_impl.getObject<ModuleLoaderInterface>();
	if (!moduleLoader->loadModule( name_))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error(_TXT("failed to load module: %s"), errorhnd->fetchError());
	}
}

void ContextImpl::addModulePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a module path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = m_moduleloader_impl.getObject<ModuleLoaderInterface>();
	moduleLoader->addModulePath( paths_);
}

void ContextImpl::addResourcePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a resource path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = m_moduleloader_impl.getObject<ModuleLoaderInterface>();
	moduleLoader->addResourcePath( paths_);
}

CallResult ContextImpl::createStatisticsProcessor( const std::string& name)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return callResultObject( new StatisticsProcessorImpl( m_storage_objbuilder_impl, m_trace_impl, name, m_errorhnd_impl));
}

void ContextImpl::initStorageObjBuilder()
{
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
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
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
	m_storage_objbuilder_impl.resetOwnership( storageObjectBuilder);
}

void ContextImpl::initAnalyzerObjBuilder()
{
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
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
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
	m_analyzer_objbuilder_impl.resetOwnership( analyzerObjectBuilder);
}

CallResult ContextImpl::detectDocumentClass( const std::string& content)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_analyzer_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	if (!m_textproc)
	{
		m_textproc = objBuilder->getTextProcessor();
		if (!m_textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
	}
	Reference<analyzer::DocumentClass> dclass( new analyzer::DocumentClass());
	if (m_textproc->detectDocumentClass( *dclass, content.c_str(), content.size()))
	{
		return callResultStructureOwnership( dclass.release());
	}
	else
	{
		if (errorhnd->hasError()) throw strus::runtime_error( _TXT("failed to detect document class: %s"), errorhnd->fetchError());
		return CallResult();
	}
}

CallResult ContextImpl::createStorageClient( const ValueVariant& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return callResultObject( new StorageClientImpl( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, Deserializer::getStorageConfigString( config_, errorhnd)));
}

CallResult ContextImpl::createVectorStorageClient( const ValueVariant& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return callResultObject( new VectorStorageClientImpl( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, Deserializer::getStorageConfigString( config_, errorhnd)));
}

CallResult ContextImpl::createDocumentAnalyzer( const ValueVariant& doctype)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	if (!m_textproc)
	{
		AnalyzerObjectBuilderInterface* objBuilder = m_analyzer_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
		m_textproc = objBuilder->getTextProcessor();
		if (!m_textproc)
		{
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
		}
	}
	return callResultObject( new DocumentAnalyzerImpl( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl, doctype, m_textproc));
}

CallResult ContextImpl::createQueryAnalyzer()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return callResultObject( new QueryAnalyzerImpl( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl));
}

CallResult ContextImpl::createQueryEval()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return callResultObject( new QueryEvalImpl( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl));
}

void ContextImpl::createStorage( const ValueVariant& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( Deserializer::getStorageConfigString( config_, errorhnd));
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
	std::string storagecfg( Deserializer::getStorageConfigString( config_, errorhnd));
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

void ContextImpl::destroyStorage( const ValueVariant& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( Deserializer::getStorageConfigString( config_, errorhnd));
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = m_storage_objbuilder_impl.getObject<StorageObjectBuilderInterface>();
	if (!objBuilder) throw strus::runtime_error( _TXT("failed to get object builder: %s"), errorhnd->fetchError());
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	if (!dbi->destroyDatabase( storagecfg)) throw strus::runtime_error( _TXT("failed to destroy database: %s"), errorhnd->fetchError());
}

void ContextImpl::close()
{
	m_analyzer_objbuilder_impl.reset();
	m_storage_objbuilder_impl.reset();
	if (m_rpc_impl.get()) (m_rpc_impl.getObject<RpcClientInterface>())->close();
	m_moduleloader_impl.reset();
}



