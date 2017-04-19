/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_OBJECTS_HPP_INCLUDED
#define _STRUS_BINDING_OBJECTS_HPP_INCLUDED
#include "strus/bindings/hostObjectReference.hpp"
#include "strus/bindings/valueVariant.hpp"
#include "strus/numericVariant.hpp"
#include <vector>
#include <string>

namespace strus {

/// \brief Forward declaration
class StatisticsProcessorInterface;
/// \brief Forward declaration
class TextProcessorInterface;
/// \brief Forward declaration
class QueryProcessorInterface;

namespace bindings {

/// \brief Forward declaration
class StatisticsIteratorImpl;
/// \brief Forward declaration
class StorageTransactionImpl;
/// \brief Forward declaration
class DocumentBrowserImpl;

class VectorStorageSearcherImpl
{
public:
	/// \brief Copy constructor
	VectorStorageSearcherImpl( const VectorStorageSearcherImpl& o);

	/// \brief Destructor
	virtual ~VectorStorageSearcherImpl(){}

	/// \brief Find the most similar vectors to vector
	/// \param[in] vec vector to search for (double[])
	/// \param[in] maxNofResults maximum number of results to return
	/// return the list of most similar vectors (double[])
	ValueVariant findSimilar( const ValueVariant& vec, unsigned int maxNofResults) const;

	/// \brief Find the most similar vectors to vector in a selection of features addressed by index
	/// \param[in] featidxlist list of candidate indices (int[])
	/// \param[in] vec vector to search for (double[])
	/// \param[in] maxNofResults maximum number of results to return
	/// return the list of most similar vectors (double[])
	ValueVariant findSimilarFromSelection( const ValueVariant& featidxlist, const ValueVariant& vec, unsigned int maxNofResults) const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClientImpl;
	VectorStorageSearcherImpl( const HostObjectReference& storage, const HostObjectReference& trace, int range_from, int range_to, const HostObjectReference& errorhnd_);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_searcher_impl;
	HostObjectReference m_trace_impl;
};

/// \brief Forward declaration
class VectorStorageTransactionImpl;

/// \brief Object representing a client connection to a vector storage 
/// \remark The only way to construct a vector storage client instance is to call Context::createVectorStorageClient(const std::string&)
class VectorStorageClientImpl
{
public:
	/// \brief Copy constructor
	VectorStorageClientImpl( const VectorStorageClientImpl& o);

	/// \brief Destructor
	virtual ~VectorStorageClientImpl(){}

	/// \brief Create a searcher object for scanning the vectors for similarity
	/// \param[in] range_from start range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \param[in] range_to end of range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \return the vector search interface (with ownership)
	VectorStorageSearcherImpl createSearcher( int range_from, int range_to) const;

	/// \brief Create a vector storage transaction instance
	VectorStorageTransactionImpl createTransaction();
	
	/// \brief Get the list of concept class names defined
	/// \return the list
	ValueVariant conceptClassNames() const;

	/// \brief Get the list of indices of features represented by a learnt concept feature
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \param[in] conceptid index (indices of learnt concepts starting from 1) 
	/// \return the resulting vector indices (index is order of insertion starting from 0)
	ValueVariant conceptFeatures( const std::string& conceptClass, int conceptid) const;

	/// \brief Get the number of concept features learned for a class
	/// \param[in] conceptClass name identifying a class of concepts learnt.
	/// \return the number of concept features and also the maximum number assigned to a feature (starting with 1)
	unsigned int nofConcepts( const std::string& conceptClass) const;

	/// \brief Get the set of learnt concepts of a class for a feature defined
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \param[in] index index of vector in the order of insertion starting from 0
	/// \return the resulting concept feature indices (indices of learnt concepts starting from 1) (std::vector<int>)
	ValueVariant featureConcepts( const std::string& conceptClass, int index) const;

	/// \brief Get the vector assigned to a feature addressed by index
	/// \param[in] index index of the feature (starting from 0)
	/// return the vector (std::vector<double>)
	ValueVariant featureVector( int index) const;

	/// \brief Get the name of a feature by its index starting from 0
	/// \param[in] index index of the feature (starting from 0)
	/// \return the name of the feature defined 
	std::string featureName( int index) const;

	/// \brief Get the index starting from 0 of a feature by its name
	/// \param[in] name name of the feature
	/// \return index -1, if not found, else index of the feature to get the name of (index is order of insertion starting with 0)
	int featureIndex( const std::string& name) const;

	/// \brief Get the number of feature vectors defined
	/// \return the number of features
	unsigned int nofFeatures() const;

	/// \brief Get the configuration of this model
	/// \return the configuration string
	std::string config() const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class ContextImpl;
	VectorStorageClientImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_vector_storage_impl;
	std::string m_config;
};

class VectorStorageTransactionImpl
{
public:
	/// \brief Copy constructor
	VectorStorageTransactionImpl( const VectorStorageTransactionImpl& o);

	/// \brief Destructor
	virtual ~VectorStorageTransactionImpl(){}

	/// \brief Add named feature to vector storage
	/// \param[in] name unique name of the feature added
	/// \param[in] vec vector assigned to the feature
	void addFeature( 
			const std::string& name,
			const ValueVariant& vec);

	/// \brief Assign a concept (index) to a feature referenced by index
	/// \param[in] relationTypeName name of the relation
	/// \param[in] featidx index of the feature
	/// \param[in] conidx index of the concept
	void defineFeatureConceptRelation(
			const std::string& relationTypeName,
			int featidx,
			int conidx);
	/// \brief Commit of the transaction
	bool commit();

	/// \brief Rollback of the transaction
	void rollback();

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClientImpl;
	VectorStorageTransactionImpl( const HostObjectReference& objbuilder, const HostObjectReference& storage, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_vector_storage_impl;	
	HostObjectReference m_vector_transaction_impl;
};

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
	/// \brief Constructor for local mode with own module loader
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	explicit ContextImpl( unsigned int maxNofThreads);
	/// \brief Constructor for local mode with own module loader and logging of all method call traces
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	/// \param[in] tracecfg configuration for generating method call traces
	explicit ContextImpl( unsigned int maxNofThreads, const std::string& tracecfg);
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \param[in] connectionstring RPC server connection string
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	explicit ContextImpl( const std::string& connectionstring);
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \param[in] connectionstring RPC server connection string
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	ContextImpl( const std::string& connectionstring, unsigned int maxNofThreads);
	/// \brief Copy constructor
	ContextImpl( const ContextImpl& o);
	/// \brief Destructor
	~ContextImpl(){}

	/// \brief Check if there has an error occurred and throw if yes
	/// \remark Some bindings have coroutines and with a coroutine switch an error message might get lost, because error context is per thread. So if a coroutine switch is done without the last error fetched it might happen that the second coroutine gets the error of the first one. Call this function after calling some method without return value before a state where a context switch is possible.
	void checkErrors() const;

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
	StatisticsProcessorImpl createStatisticsProcessor( const std::string& name);

	/// \brief Create a storage client instance of the the default storage
	StorageClientImpl createStorageClient();

	/// \brief Create a storage client instance
	/// \param[in] config_ configuration string of the storage client or empty, if the default remote storage of the RPC server is chosen,
	StorageClientImpl createStorageClient( const std::string& config_);

	/// \brief Create a storage client instance of the the default remote storage of the RPC server
	VectorStorageClientImpl createVectorStorageClient();

	/// \brief Create a vector storage client instance
	/// \param[in] config_ configuration string of the storage client or empty for the default storage
	VectorStorageClientImpl createVectorStorageClient( const std::string& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration
 	/// \remark Fails if the storage already exists
	void createStorage( const std::string& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration
 	/// \remark Fails if the storage already exists
	void createVectorStorage( const std::string& config_);

	/// \brief Delete the storage (physically) described by config
	/// \note works also on vector storages
	/// \param[in] config_ storage description
	/// \note Handle this function carefully
	void destroyStorage( const std::string& config_);

	/// \brief Detect the type of document from its content
	/// \param[in] content the document content to classify
	/// \return the document class (analyzer::DocumentClass)
	ValueVariant detectDocumentClass( const std::string& content);

	/// \brief Create a document analyzer instance
	/// \param[in] segmentername_ name of the segmenter to use (if empty then the default segmenter is used)
	DocumentAnalyzerImpl createDocumentAnalyzer( const std::string& segmentername_="");

	/// \brief Create a query analyzer instance
	QueryAnalyzerImpl createQueryAnalyzer();

	/// \brief Create a query evaluation instance
	QueryEvalImpl createQueryEval();

	/// \brief Force cleanup to circumvent object pooling mechanisms in an interpreter context
	void close();

private:
	void initStorageObjBuilder();
	void initAnalyzerObjBuilder();

private:
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_moduleloader_impl;
	HostObjectReference m_rpc_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_storage_objbuilder_impl;
	HostObjectReference m_analyzer_objbuilder_impl;
	const TextProcessorInterface* m_textproc;
};

}}//namespace
#endif


