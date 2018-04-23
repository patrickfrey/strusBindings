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
#include "strus/base/thread.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/struct.hpp"
#include "impl/value/introspectionBase.hpp"
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
/// \brief Forward declaration
class InserterImpl;

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
	/// \note this function is not thread safe and should only be called in the initialization phase before calling endConfig when used in a multithreaded context.
	void loadModule( const std::string& name);

	/// \brief Add one or more paths from where to try to load modules from
	/// \example addModulePath( "/home/bob/modules" )
	/// \remark Only implemented in local mode with own module loader (see constructors)
	/// \param[in] paths a string or a list of module search paths
	/// \example [ "/home/bob/modules" "/home/anne/modules" ]
	/// \example "/home/bob/modules"
	/// \note this function is not thread safe and should only be called in the initialization phase before calling endConfig when used in a multithreaded context.
	void addModulePath( const ValueVariant& paths);

	/// \brief Add a path where to load analyzer resource files from
	/// \remark Only implemented in local mode with own module loader (see constructors)
	/// \param[in] paths a string or a list of resource search paths
	/// \example [ "/home/bob/resources" "/home/anne/resources" ]
	/// \example "/home/bob/resources"
	void addResourcePath( const ValueVariant& paths);

	/// \brief Define the working directory where files are written to
	/// \note All paths used for data written must be relative from the working directory, if the working directory is defined
	/// \param[in] path a string specifying the working directory
	/// \example "/srv/strus"
	void defineWorkingDirectory( const std::string& path);

	/// \brief End the configuration of the context, creates the object builders
	/// \remark If this function is not called, then the object builders are created on the first request
	void endConfig();

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
	/// \return storage client interface (class StorageClient) for accessing the storage
	StorageClientImpl* createStorageClient( const ValueVariant& config=ValueVariant());

	/// \brief Create a vector storage client instance
	/// \example createVectorStorageClient()
	/// \example createVectorStorageClient( "path=/srv/searchengine/vecstorage" )
	/// \example createVectorStorageClient( [ path: "/srv/searchengine/vecstorage" ] )
	/// \param[in] config configuration (string or structure with named elements) of the storage client or undefined, if the default remote vector storage of the RPC server is chosen
	/// \return vector storage client interface (class VectorStorageClient) for accessing the vector storage
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
	/// \example storageExists( [ path: "/srv/searchengine/storage" ] )
	/// \note Works also on vector storages, it does not distinguish between those
	/// \param[in] config storage configuration (string or structure with named elements) 
	/// \return true, if the storage with this configuration exists
	bool storageExists( const ValueVariant& config);

	/// \brief Detect the type of document from its content
	/// \example detectDocumentClass( "<?xml version='1.0' encoding='UTF-8'?><doc>...</doc>")
	/// \param[in] content the document content to classify
	/// \return the document class
	/// \example [ mimetype:"application/xml" encoding:"UTF-8" scheme:"customer" ]
	/// \example [ mimetype:"application/json" encoding:"UTF-8" ]
	analyzer::DocumentClass* detectDocumentClass( const std::string& content);

	/// \brief Create a document analyzer instance
	/// \example createDocumentAnalyzer( [ mimetype:"application/xml" encoding:"UTF-8" ] )
	/// \param[in] doctype structure describing the segmenter to use (either document class description structure or segmenter name)
	/// \example [ mimetype:"application/xml" encoding:"UTF-8" scheme:"customer" ]
	/// \example [ mimetype:"application/json" encoding:"UTF-8" ]
	/// \example [ segmenter:"textwolf" ]
	/// \example "application/json"
	/// \example "json"
	/// \return document analyzer interface (class DocumentAnalyzer)
	DocumentAnalyzerImpl* createDocumentAnalyzer( const ValueVariant& doctype);

	/// \brief Create a query analyzer instance
	/// \example createQueryAnalyzer()
	/// \return query analyzer interface (class QueryAnalyzer)
	QueryAnalyzerImpl* createQueryAnalyzer();

	/// \brief Create a query evaluation instance
	/// \example createQueryEval()
	/// \return query evaluation interface (class QueryEval)
	QueryEvalImpl* createQueryEval();

	/// \brief Create an inserter based on a storage and a document analyzer
	/// \example createInserter( storage, analyzer)
	/// \param[in] storage storage client to insert into
	/// \param[in] analyzer document analyzer to use for preparing the documents to insert
	/// \return inserter interface (class Inserter)
	InserterImpl* createInserter( StorageClientImpl* storage, DocumentAnalyzerImpl* analyzer);

	/// \brief Unpack a statistics blob retrieved from a storage
	/// \param[in] blob binary blob with statistics to decode (created by StorageClient:getAllStatistics or StorageClient:getChangeStatistics)
	/// \param[in] procname name of statistics processor to use for decoding the message (use default processor, if not defined)
	/// \example "default"
	/// \example ""
	/// \return the statistics structure encoded in the blob passed as argument
	Struct unpackStatisticBlob( const ValueVariant& blob, const std::string& procname="");

	/// \brief Force cleanup to circumvent object pooling mechanisms in an interpreter context
	void close();

	/// \brief Debug method that returns the serialization of the arguments as string
	/// \example debug_serialize( [ surname:"John" lastname:"Doe" company:[ name:"ACME" url:"acme.com" ]] )
	/// \note this function is used for verifying if the deserialization of binding language data structures work as expected
	/// \param[in] arg structure to serialize as string for visualization (debuging)
	/// \param[in] deterministic true, if output is deterministic
	/// \return the input serialization as string
	/// \example "open name 'surname' value 'John' name 'lastname' value 'Doe' name 'company' open name 'name' value 'ACME' name 'url' value 'acme.com' close close"
	std::string debug_serialize( const ValueVariant& arg, bool deterministic=false);

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \example ["queryproc","weightfunc"]
	/// \example ["weightfunc"]
	/// \example ["env"]
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path);

	/// \brief Enable the debug trace interface for a named component for the current thread
	/// \param[in] component name of component to enable debug tracing for
	void enableDebugTrace( const std::string& component);

	/// \brief Fetch all debug trace messages of the current thread
	/// \return all messages
	/// \note Clears all messages stored for the current thread
	Struct fetchDebugTrace();

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
	int m_threads;
	strus::mutex m_mutex;
};

}}//namespace
#endif

