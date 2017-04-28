/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_VECTOR_STORAGE_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VECTOR_STORAGE_HPP_INCLUDED
#include "papuga/hostObjectReference.hpp"
#include "papuga/valueVariant.hpp"
#include "papuga/callResult.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga::ValueVariant ValueVariant;
typedef papuga::CallResult CallResult;
typedef papuga::HostObjectReference HostObjectReference;

/// \brief Object used to search for similar vectors in the collection
class VectorStorageSearcherImpl
{
public:
	/// \brief Destructor
	virtual ~VectorStorageSearcherImpl(){}

	/// \brief Find the most similar vectors to vector
	/// \param[in] vec vector to search for (double[])
	/// \param[in] maxNofResults maximum number of results to return
	/// return the list of most similar vectors (double[])
	CallResult findSimilar( const ValueVariant& vec, unsigned int maxNofResults) const;

	/// \brief Find the most similar vectors to vector in a selection of features addressed by index
	/// \param[in] featidxlist list of candidate indices (int[])
	/// \param[in] vec vector to search for (double[])
	/// \param[in] maxNofResults maximum number of results to return
	/// return the list of most similar vectors (double[])
	CallResult findSimilarFromSelection( const ValueVariant& featidxlist, const ValueVariant& vec, unsigned int maxNofResults) const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClientImpl;
	VectorStorageSearcherImpl( const HostObjectReference& storage, const HostObjectReference& trace, int range_from, int range_to, const HostObjectReference& errorhnd_);

	mutable HostObjectReference m_errorhnd_impl;
	HostObjectReference m_searcher_impl;
	HostObjectReference m_trace_impl;
};

/// \brief Object representing a client connection to a vector storage 
/// \remark The only way to construct a vector storage client instance is to call Context::createVectorStorageClient(const std::string&)
class VectorStorageClientImpl
{
public:
	/// \brief Destructor
	virtual ~VectorStorageClientImpl(){}

	/// \brief Create a searcher object for scanning the vectors for similarity
	/// \param[in] range_from start range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \param[in] range_to end of range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \return the vector search interface (with ownership)
	CallResult createSearcher( int range_from, int range_to) const;

	/// \brief Create a vector storage transaction instance
	CallResult createTransaction();
	
	/// \brief Get the list of concept class names defined
	/// \return the list
	CallResult conceptClassNames() const;

	/// \brief Get the list of indices of features represented by a learnt concept feature
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \param[in] conceptid index (indices of learnt concepts starting from 1) 
	/// \return the resulting vector indices (index is order of insertion starting from 0)
	CallResult conceptFeatures( const std::string& conceptClass, int conceptid) const;

	/// \brief Get the number of concept features learnt for a class
	/// \param[in] conceptClass name identifying a class of concepts learnt.
	/// \return the number of concept features and also the maximum number assigned to a feature (starting with 1)
	CallResult nofConcepts( const std::string& conceptClass) const;

	/// \brief Get the set of learnt concepts of a class for a feature defined
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \param[in] index index of vector in the order of insertion starting from 0
	/// \return the resulting concept feature indices (indices of learnt concepts starting from 1) (std::vector<int>)
	CallResult featureConcepts( const std::string& conceptClass, int index) const;

	/// \brief Get the vector assigned to a feature addressed by index
	/// \param[in] index index of the feature (starting from 0)
	/// return the vector (std::vector<double>)
	CallResult featureVector( int index) const;

	/// \brief Get the name of a feature by its index starting from 0
	/// \param[in] index index of the feature (starting from 0)
	/// \return the name of the feature defined 
	CallResult featureName( int index) const;

	/// \brief Get the index starting from 0 of a feature by its name
	/// \param[in] name name of the feature
	/// \return index -1, if not found, else index of the feature to get the name of (index is order of insertion starting with 0)
	CallResult featureIndex( const std::string& name) const;

	/// \brief Get the number of feature vectors defined
	/// \return the number of features
	CallResult nofFeatures() const;

	/// \brief Get the configuration of this storage
	/// \return the configuration as structure
	CallResult config() const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class ContextImpl;
	VectorStorageClientImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config);

	mutable HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_vector_storage_impl;
	std::string m_config;
};


/// \brief Object representing a vector storage transaction
class VectorStorageTransactionImpl
{
public:
	/// \brief Destructor
	virtual ~VectorStorageTransactionImpl(){}

	/// \brief Add named feature to vector storage
	/// \param[in] name unique name of the feature added
	/// \param[in] vec vector assigned to the feature
	void addFeature( 
			const std::string& name,
			const ValueVariant& vec);

	/// \brief Assign a concept (index) to a feature referenced by index
	/// \param[in] conceptClass name of the relation
	/// \param[in] featidx index of the feature
	/// \param[in] conidx index of the concept
	void defineFeatureConceptRelation(
			const std::string& conceptClass,
			int featidx,
			int conidx);

	/// \brief Commit of the transaction
	CallResult commit();

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

}}//namespace
#endif

