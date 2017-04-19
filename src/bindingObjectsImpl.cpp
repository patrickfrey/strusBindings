/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "bindingObjectsImpl.hpp"
#include "deserializer.hpp"
#include "expressionBuilder.hpp"
#include "strus/strus.hpp"
#include "strus/lib/rpc_client.hpp"
#include "strus/lib/rpc_client_socket.hpp"
#include "strus/lib/module.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/lib/error.hpp"
#include "strus/rpcClientInterface.hpp"
#include "strus/rpcClientMessagingInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "strus/statisticsBuilderInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/scalarFunctionInterface.hpp"
#include "strus/scalarFunctionParserInterface.hpp"
#include "strus/documentAnalyzerInterface.hpp"
#include "strus/documentAnalyzerContextInterface.hpp"
#include "strus/queryAnalyzerInterface.hpp"
#include "strus/queryAnalyzerContextInterface.hpp"
#include "strus/patternMatcherInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/patternTermFeederInterface.hpp"
#include "strus/patternTermFeederInstanceInterface.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/vectorStorageTransactionInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/utf8.hpp"
#include "strus/base/symbolTable.hpp"
#include "strus/base/fileio.hpp"
#include "internationalization.hpp"
#include "utils.hpp"
#include "traceUtils.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <limits>

using namespace strus;
using namespace strus::bindings;


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

ContextImpl::ContextImpl()
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
}

ContextImpl::ContextImpl( unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
}

ContextImpl::ContextImpl( unsigned int maxNofThreads, const std::string& tracecfg)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
	if (!tracecfg.empty())
	{
		m_trace_impl.reset( new TraceProxy( moduleLoader, tracecfg, errorhnd));
	}
}

ContextImpl::ContextImpl( const std::string& connectionstring)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.reset( errorhnd);

	std::auto_ptr<RpcClientMessagingInterface> messaging;
	messaging.reset( createRpcClientMessaging( connectionstring.c_str(), errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

ContextImpl::ContextImpl( const std::string& connectionstring, unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);

	std::auto_ptr<RpcClientMessagingInterface> messaging;
	messaging.reset( createRpcClientMessaging( connectionstring.c_str(), errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

ContextImpl::ContextImpl( const Context& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_moduleloader_impl(o.m_moduleloader_impl)
	,m_rpc_impl(o.m_rpc_impl)
	,m_trace_impl( o.m_trace_impl)
	,m_storage_objbuilder_impl(o.m_storage_objbuilder_impl)
	,m_analyzer_objbuilder_impl(o.m_analyzer_objbuilder_impl)
	,m_textproc(o.m_textproc)
{}

void ContextImpl::checkErrors() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("unhandled error: %s"), errorhnd->fetchError());
	}
}

void ContextImpl::loadModule( const std::string& name_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot load modules in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
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
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addModulePath( paths_);
}

void ContextImpl::addResourcePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a resource path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addResourcePath( paths_);
}

StatisticsProcessorImpl ContextImpl::createStatisticsProcessor( const std::string& name)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StatisticsProcessor( m_storage_objbuilder_impl, m_trace_impl, name, m_errorhnd_impl);
}

void ContextImpl::initStorageObjBuilder()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageObjectBuilderInterface* storageObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		RpcClientInterface* client = (RpcClientInterface*)m_rpc_impl.get();
		storageObjectBuilder = client->createStorageObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
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
	TraceProxy* tp = (TraceProxy*)m_trace_impl.get();
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
	m_storage_objbuilder_impl.reset( storageObjectBuilder);
}

void ContextImpl::initAnalyzerObjBuilder()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	AnalyzerObjectBuilderInterface* analyzerObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		RpcClientInterface* client = (RpcClientInterface*)m_rpc_impl.get();
		analyzerObjectBuilder = client->createAnalyzerObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
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
	TraceProxy* tp = (TraceProxy*)m_trace_impl.get();
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
	m_analyzer_objbuilder_impl.reset( analyzerObjectBuilder);
}

ValueVariant ContextImpl::detectDocumentClass( const std::string& content)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = (const AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
	const TextProcessorInterface* textproc;
	if (m_textproc)
	{
		textproc = (const TextProcessorInterface*)m_textproc;
	}
	else
	{
		textproc = objBuilder->getTextProcessor();
		m_textproc = textproc;
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
	}
	analyzer::DocumentClass dclass;
	if (textproc->detectDocumentClass( dclass, content.c_str(), content.size()))
	{
		return DocumentClass( dclass.mimeType(), dclass.encoding(), dclass.scheme());
	}
	else
	{
		if (errorhnd->hasError()) throw strus::runtime_error( _TXT("failed to detect document class: %s"), errorhnd->fetchError());
		return DocumentClass();
	}
}

StorageClientImpl ContextImpl::createStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, config_);
}

StorageClientImpl ContextImpl::createStorageClient()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, std::string());
}

VectorStorageClientImpl ContextImpl::createVectorStorageClient()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return VectorStorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, std::string());
}

VectorStorageClientImpl ContextImpl::createVectorStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return VectorStorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, config_);
}

DocumentAnalyzerImpl ContextImpl::createDocumentAnalyzer( const std::string& segmentername_)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	if (!m_textproc)
	{
		AnalyzerObjectBuilderInterface* objBuilder = (AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
		m_textproc = objBuilder->getTextProcessor();
		if (!m_textproc)
		{
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
		}
	}
	return DocumentAnalyzer( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl, segmentername_, m_textproc);
}

QueryAnalyzerImpl ContextImpl::createQueryAnalyzer()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return QueryAnalyzer( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl);
}

QueryEvalImpl ContextImpl::createQueryEval()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return QueryEval( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl);
}

void ContextImpl::createStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const StorageInterface* sti = objBuilder->getStorage();
	if (!sti) throw strus::runtime_error( _TXT("failed to get storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());
}

void ContextImpl::createVectorStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagename;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);
	if (!extractStringFromConfigString( dbname, storagename, "storage", errorhnd))
	{
		storagename = Constants::standard_vector_storage();
	}
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const VectorStorageInterface* sti = objBuilder->getVectorStorage( storagename);
	if (!sti) throw strus::runtime_error( _TXT("failed to get vector storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create vector storage: %s"), errorhnd->fetchError());
}

void ContextImpl::destroyStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	if (!objBuilder) throw strus::runtime_error( _TXT("failed to get object builder: %s"), errorhnd->fetchError());
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	if (!dbi->destroyDatabase( storagecfg)) throw strus::runtime_error( _TXT("failed to destroy database: %s"), errorhnd->fetchError());
}

void ContextImpl::close()
{
	m_analyzer_objbuilder_impl.reset();
	m_storage_objbuilder_impl.reset();
	if (m_rpc_impl.get()) ((RpcClientInterface*)m_rpc_impl.get())->close();
	m_moduleloader_impl.reset();
}


