/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_CONTEXT_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_CONTEXT_HPP_INCLUDED
#include "strus/bindings/hostObjectReference.hpp"
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/callResult.hpp"
#include "strus/numericVariant.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \brief Object holding the global context of the strus information retrieval engine
/// \note There a two modes of this context object operating on a different base.
///	If you create this object without parameter, then the context is local.
///	In a local context you can load modules, define resources, etc. If you create
///	this object with a connection string as parameter, then all object created by
///	this context reside on the server (strusRpcServer) addressed with the connection string.
///	In this case loaded modules and resources are ignored. What modules to use is then
///	specified on server startup.
class ContextImpl
{
public:
	/// \brief Constructor for local mode with own module loader
	ContextImpl();
	/// \brief Constructor
	/// \param[in] config context configuration
	explicit ContextImpl( const ValueVariant& config);
	/// \brief Destructor
	~ContextImpl(){}

	/// \brief Check if there has an error occurred and return it if yes
	/// \remark Some bindings have coroutines and with a coroutine switch an error message might get lost, because error context is per thread. So if a coroutine switch is done without the last error fetched it might happen that the second coroutine gets the error of the first one. Call this function after calling some method without return value before a state where a context switch is possible.
	CallResult getLastError() const;

	/// \brief Load a module
	/// \param[in] name_ name of the module to load
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void loadModule( const std::string& name_);

	/// \brief Add a path from where to try to load modules from
	/// \param[in] paths_ semicolon separated list of module search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addModulePath( const std::string& paths_);

	/// \brief Define where to load analyzer resource files from
	/// \param[in] paths_ semicolon separated list of resource search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addResourcePath( const std::string& paths_);

	/// \brief Create a statistics message processor instance
	/// \return the processor
	CallResult createStatisticsProcessor( const std::string& name);

	/// \brief Create a storage client instance of the the default storage
	CallResult createStorageClient();

	/// \brief Create a storage client instance
	/// \param[in] config_ configuration (string or structure with named elements) of the storage client or empty, if the default remote storage of the RPC server is chosen,
	CallResult createStorageClient( const ValueVariant& config_);

	/// \brief Create a storage client instance of the the default remote storage of the RPC server
	CallResult createVectorStorageClient();

	/// \brief Create a vector storage client instance
	/// \param[in] config_ configuration (string or structure with named elements) of the storage client or empty for the default storage
	CallResult createVectorStorageClient( const ValueVariant& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration (string or structure with named elements) 
 	/// \remark Fails if the storage already exists
	void createStorage( const ValueVariant& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration (string or structure with named elements) 
 	/// \remark Fails if the storage already exists
	void createVectorStorage( const ValueVariant& config_);

	/// \brief Delete the storage (physically) described by config
	/// \note works also on vector storages
	/// \param[in] config_ storage configuration (string or structure with named elements) 
	/// \note Handle this function carefully
	void destroyStorage( const ValueVariant& config_);

	/// \brief Detect the type of document from its content
	/// \param[in] content the document content to classify
	/// \return the document class (analyzer::DocumentClass)
	CallResult detectDocumentClass( const std::string& content);

	/// \brief Create a document analyzer instance
	/// \param[in] doctype structure describing the segmenter to use (either document class description structure or segmenter name)
	CallResult createDocumentAnalyzer( const ValueVariant& doctype);

	/// \brief Create a query analyzer instance
	CallResult createQueryAnalyzer();

	/// \brief Create a query evaluation instance
	CallResult createQueryEval();

	/// \brief Force cleanup to circumvent object pooling mechanisms in an interpreter context
	void close();

private:
	void initStorageObjBuilder();
	void initAnalyzerObjBuilder();

private:
	mutable HostObjectReference m_errorhnd_impl;
	HostObjectReference m_moduleloader_impl;
	HostObjectReference m_rpc_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_storage_objbuilder_impl;
	HostObjectReference m_analyzer_objbuilder_impl;
	const TextProcessorInterface* m_textproc;
	const QueryProcessorInterface* m_queryproc;
};

}}//namespace
#endif

