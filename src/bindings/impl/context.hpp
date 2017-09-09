/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_CONTEXT_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_CONTEXT_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/numericVariant.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/struct.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \brief Forward declaration
class StorageClientImpl;
/// \brief Forward declaration
class VectorStorageClientImpl;
/// \brief Forward declaration
class DocumentAnalyzerImpl;
/// \brief Forward declaration
class QueryAnalyzerImpl;
/// \brief Forward declaration
class QueryEvalImpl;

typedef papuga_ValueVariant ValueVariant;

/// \class ContextImpl
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
	/// \brief Constructor
	/// \param[in] config context configuration. If not defined, create context for local mode with own module loader
	/// \example [ rpc: "localhost:7181" ]
	/// \example [ trace: "log=dump;file=stdout" ]
	/// \example [ threads: 12 ]
	explicit ContextImpl( const ValueVariant& config=ValueVariant());
	/// \brief Destructor
	~ContextImpl(){}

	/// \brief Load a module
	/// \example loadModule( "storage_vector" )
	/// \remark Only implemented in local mode with own module loader (see constructors)
	/// \param[in] name name of the module to load
	/// \example "analyzer_pattern"
	/// \example "storage_vector"
	void loadModule( const std::string& name);

	/// \brief Add one or more paths from where to try to load modules from
	/// \example addModulePath( "/home/bob/modules" )
	/// \remark Only implemented in local mode with own module loader (see constructors)
	/// \param[in] paths a string or a list of module search paths
	/// \example [ "/home/bob/modules" "/home/anne/modules" ]
	/// \example "/home/bob/modules"
	void addModulePath( const ValueVariant& paths);

	/// \brief Add a path where to load analyzer resource files from
	/// \remark Only implemented in local mode with own module loader (see constructors)
	/// \param[in] paths a string or a list of resource search paths
	/// \example [ "/home/bob/resources" "/home/anne/resources" ]
	/// \example "/home/bob/resources"
	void addResourcePath( const ValueVariant& paths);

	/// \brief Create a storage client instance
	/// \example createStorageClient()
	/// \example createStorageClient( "path=/srv/searchengine/storage; metadata=doclen UINT32, date UINT32, docweight FLOAT" )
	/// \example createStorageClient( [ path: "/srv/searchengine/storage" metadata: "doclen UINT32, date UINT32, docweight FLOAT" ] )
	/// \example createStorageClient( [
	///	path: "/srv/searchengine/storage"
	///	metadata: "doclen UINT32, date UINT32, docweight FLOAT"
	///	max_open_files:256
	///	write_buffer_size:"4K"
	///	block_size:"4M"
	///	cache:"1G"
	///	] )
	/// \param[in] config configuration (string or structure with named elements) of the storage client or undefined, if the default remote storage of the RPC server is chosen
	/// \return storage client interface for accessing the storage
	StorageClientImpl* createStorageClient( const ValueVariant& config=ValueVariant());

	/// \brief Create a vector storage client instance
	/// \example createVectorStorageClient()
	/// \example createVectorStorageClient( "path=/srv/searchengine/vecstorage" )
	/// \example createVectorStorageClient( [ path: "/srv/searchengine/vecstorage" ] )
	/// \param[in] config configuration (string or structure with named elements) of the storage client or undefined, if the default remote vector storage of the RPC server is chosen
	/// \return vector storage client interface for accessing the vector storage
	VectorStorageClientImpl* createVectorStorageClient( const ValueVariant& config=ValueVariant());

	/// \brief Create a new storage (physically) described by config
	/// \remark Fails if the storage already exists
	/// \example createStorage( "path=/srv/searchengine/storage; metadata=doclen UINT32, date UINT32, docweight FLOAT; acl=yes" )
	/// \example createStorage( [ path: "/srv/searchengine/storage" metadata: "doclen UINT32, date UINT32, docweight FLOAT" acl:true ] )
	/// \param[in] config storage configuration (string or structure with named elements) 
	void createStorage( const ValueVariant& config);

	/// \brief Create a new storage (physically) described by config
	/// \example createVectorStorageClient( "path=/srv/searchengine/vecstorage" )
	/// \example createVectorStorageClient( [ path: "/srv/searchengine/vecstorage" ] )
	/// \remark Fails if the storage already exists
	/// \param[in] config storage configuration (string or structure with named elements) 
	void createVectorStorage( const ValueVariant& config);

	/// \brief Delete the storage (physically) described by config
	/// \example destroyStorage( "path=/srv/searchengine/storage" )
	/// \example destroyStorage( [ path: "/srv/searchengine/storage" ] )
	/// \note Works also on vector storages
	/// \remark Handle this function carefully
	/// \param[in] config storage configuration (string or structure with named elements) 
	void destroyStorage( const ValueVariant& config);

	/// \brief Tests if the storage described by config exists
	/// \example storageExists( "path=/srv/searchengine/storage" )
	/// \note Works also on vector storages, it does not distinguish between those
	/// \param[in] config storage configuration (string or structure with named elements) 
	/// \return true, if the storage with this configuration exists
	bool storageExists( const ValueVariant& config);

	/// \brief Detect the type of document from its content
	/// \example detectDocumentClass( "<?xml version='1.0' encoding='UTF-8'?><doc>...</doc>")
	/// \param[in] content the document content to classify
	/// \return the document class
	/// \example [ mimetype="application/xml" encoding="UTF-8" scheme="customer" ]
	/// \example [ mimetype="application/json" encoding="UTF-8" ]
	analyzer::DocumentClass* detectDocumentClass( const std::string& content);

	/// \brief Create a document analyzer instance
	/// \param[in] doctype structure describing the segmenter to use (either document class description structure or segmenter name)
	/// \example [ mimetype="application/xml" encoding="UTF-8" scheme="customer" ]
	/// \example [ mimetype="application/json" encoding="UTF-8" ]
	/// \example [ segmenter="textwolf" ]
	/// \example "application/json"
	/// \example "json"
	/// \return document analyzer interface
	DocumentAnalyzerImpl* createDocumentAnalyzer( const ValueVariant& doctype);

	/// \brief Create a query analyzer instance
	/// \return query analyzer interface
	QueryAnalyzerImpl* createQueryAnalyzer();

	/// \brief Create a query evaluation instance
	/// \return quer evaluation interface
	QueryEvalImpl* createQueryEval();

	/// \brief Unpack a statistics blob retrieved from a storage
	/// \param[in] blob binary blob with statistics to decode
	/// \param[in] procname name of statistics processor to use for decoding the message (use default processor, if not defined)
	/// \example "default"
	/// \example ""
	/// \return the statistics structure encoded in the blob passed as argument
	Struct unpackStatisticBlob( const ValueVariant& blob, const std::string& procname="") const;

	/// \brief Force cleanup to circumvent object pooling mechanisms in an interpreter context
	void close();

	/// \brief Debug method that returns the serialization of the arguments as string
	/// \note this function is used for verifying if the deserialization of binding language data structures work as expected
	/// \param[in] arg structure to serialize as string for visualization (debuging)
	/// \return the input serialization as string
	std::string debug_serialize( const ValueVariant& arg);

private:
	void initStorageObjBuilder();
	void initAnalyzerObjBuilder();

private:
	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_moduleloader_impl;
	ObjectRef m_rpc_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_storage_objbuilder_impl;
	ObjectRef m_analyzer_objbuilder_impl;
	const TextProcessorInterface* m_textproc;
	const QueryProcessorInterface* m_queryproc;
};

}}//namespace
#endif

