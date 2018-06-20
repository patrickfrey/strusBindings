/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_VECTOR_STORAGE_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VECTOR_STORAGE_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/vectorStorageSearchInterface.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/struct.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \brief Forward declaration
class VectorStorageTransactionImpl;

/// \class VectorStorageSearcherImpl
/// \brief Object used to search for similar vectors in the collection
/// \note The only way to construct a vector storage searcher instance is to call VectorStorageClient::createSearcher( from, to)
class VectorStorageSearcherImpl
{
public:
	/// \brief Destructor
	virtual ~VectorStorageSearcherImpl(){}

	/// \brief Find the most similar vectors to vector
	/// \param[in] vec vector to search for (double[])
	/// \param[in] maxNofResults maximum number of results to return
	/// \return the list of most similar vectors (double[])
	std::vector<VectorQueryResult> findSimilar( const ValueVariant& vec, unsigned int maxNofResults) const;

	/// \brief Find the most similar vectors to vector in a selection of features addressed by index
	/// \param[in] featidxlist list of candidate indices (int[])
	/// \param[in] vec vector to search for (double[])
	/// \param[in] maxNofResults maximum number of results to return
	/// \return the list of most similar vectors (double[])
	std::vector<VectorQueryResult> findSimilarFromSelection( const ValueVariant& featidxlist, const ValueVariant& vec, unsigned int maxNofResults) const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClientImpl;
	VectorStorageSearcherImpl( const ObjectRef& trace, const ObjectRef& storage, int range_from, int range_to, const ObjectRef& errorhnd_);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_searcher_impl;
	ObjectRef m_trace_impl;
};

/// \class VectorStorageClientImpl
/// \brief Object representing a client connection to a vector storage 
/// \note The only way to construct a vector storage client instance is to call Context::createVectorStorageClient( config) or Context::createVectorStorageClient()
class VectorStorageClientImpl
{
public:
	/// \brief Destructor
	virtual ~VectorStorageClientImpl(){}

	/// \brief Create a searcher object for scanning the vectors for similarity
	/// \param[in] range_from start range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \example 0
	/// \example 1000000
	/// \param[in] range_to end of range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \example 1000000
	/// \example 2000000
	/// \return the vector search interface (with ownership)
	VectorStorageSearcherImpl* createSearcher( int range_from, int range_to) const;

	/// \brief Create a vector storage transaction instance
	/// \return the transaction instance
	VectorStorageTransactionImpl* createTransaction();
	
	/// \brief Get the list of concept class names defined
	/// \return the list
	/// \example ["flections" "entityrel"]
	Struct conceptClassNames() const;

	/// \brief Get the list of indices of features represented by a learnt concept feature
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \example "flections"
	/// \example "entityrel"
	/// \example ""
	/// \param[in] conceptid index (indices of learnt concepts starting from 1) 
	/// \example 1
	/// \example 121
	/// \example 3249432
	/// \return the resulting vector indices (index is order of insertion starting from 0)
	/// \example [ 2121 5355 35356 214242 8309732 32432424 ]
	std::vector<Index> conceptFeatures( const std::string& conceptClass, int conceptid) const;

	/// \brief Get the number of concept features learnt for a class
	/// \param[in] conceptClass name identifying a class of concepts learnt.
	/// \example "entityrel"
	/// \example ""
	/// \return the number of concept features and also the maximum number assigned to a feature (starting with 1)
	/// \example 0
	/// \example 3535
	/// \example 324325
	/// \example 2343246
	unsigned int nofConcepts( const std::string& conceptClass) const;

	/// \brief Get the set of learnt concepts of a class for a feature defined
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \example "flections"
	/// \example ""
	/// \param[in] index index of vector in the order of insertion starting from 0
	/// \example 0
	/// \example 3785
	/// \example 123325
	/// \example 8793246
	/// \return the resulting concept feature indices (indices of learnt concepts starting from 1)
	/// \example [ 2121 5355 35356 214242 8309732 32432424 ]
	std::vector<Index> featureConcepts( const std::string& conceptClass, int index) const;

	/// \brief Get the vector assigned to a feature addressed by index
	/// \param[in] index index of the feature (starting from 0)
	/// \example 0
	/// \example 3785
	/// \return the vector
	/// \example [ 0.08721391 0.01232134 0.02342453 0.0011312 0.0012314 0.087232243 ]
	std::vector<float> featureVector( int index) const;

	/// \brief Get the name of a feature by its index starting from 0
	/// \param[in] index index of the feature (starting from 0)
	/// \example 0
	/// \example 71243
	/// \return the name of the feature defined
	/// \example "castle"
	std::string featureName( int index) const;

	/// \brief Get the index starting from 0 of a feature by its name
	/// \param[in] name name of the feature
	/// \example "castle"
	/// \return index -1, if not found, else index of the feature to get the name of (index is order of insertion starting with 0)
	/// \example -1
	/// \example 52636
	Index featureIndex( const std::string& name) const;

	/// \brief Get the number of feature vectors defined
	/// \return the number of features
	/// \example 0
	/// \example 15612336
	unsigned int nofFeatures() const;

	/// \brief Get the configuration of this vector storage
	/// \return the configuration as structure
	/// \example [ path:'storage' commit:10 dim:300 bit:64 var:32 simdist:340 maxdist:640 realvecweights:1 ]
	Struct config() const;

	/// \brief Get the configuration of this vector storage as string
	/// \return the configuration as string
	/// \example "path=storage;commit=10;dim=300;bit=64;var=32;simdist=340;maxdist=640;realvecweights=1"
	std::string configstring() const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class ContextImpl;
	VectorStorageClientImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_vector_storage_impl;
	std::string m_config;
};


/// \class VectorStorageTransactionImpl
/// \brief Object representing a vector storage transaction
/// \note The only way to construct a vector storage transaction instance is to call VectorStorageClient::createTransaction()
class VectorStorageTransactionImpl
{
public:
	/// \brief Destructor
	virtual ~VectorStorageTransactionImpl(){}

	/// \brief Add named feature to vector storage
	/// \param[in] name unique name of the feature added
	/// \example "castle"
	/// \example "conquest"
	/// \param[in] vec vector assigned to the feature
	/// \example [ 0.08721391 0.01232134 0.02342453 0.0011312 0.0012314 0.087232243 ]
	void addFeature( 
			const std::string& name,
			const ValueVariant& vec);

	/// \brief Assign a concept (index) to a feature referenced by index
	/// \param[in] conceptClass name of the relation
	/// \example "entityrel"
	/// \param[in] featidx index of the feature
	/// \example 1242321
	/// \param[in] conidx index of the concept
	/// \example 32874
	void defineFeatureConceptRelation(
			const std::string& conceptClass,
			int featidx,
			int conidx);

	/// \brief Commit of the transaction
	/// \remark throws an error on failure
	void commit();

	/// \brief Rollback of the transaction
	void rollback();

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClientImpl;
	VectorStorageTransactionImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& storage, const ObjectRef& errorhnd_, const std::string& config);

	ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_vector_storage_impl;	
	ObjectRef m_vector_transaction_impl;
};

}}//namespace
#endif

