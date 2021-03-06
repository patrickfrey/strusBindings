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
#include "impl/value/objectref.hpp"
#include "impl/value/struct.hpp"
#include "impl/value/iterator.hpp"
#include "impl/value/introspectionBase.hpp"
#include "strus/storage/wordVector.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \brief Forward declaration
class VectorStorageTransactionImpl;
/// \brief Forward declaration
class SentenceLexerImpl;


/// \class VectorStorageClientImpl
/// \brief Object representing a client connection to a vector storage 
/// \note The only way to construct a vector storage client instance is to call Context::createVectorStorageClient( config) or Context::createVectorStorageClient()
class VectorStorageClientImpl
{
public:
	/// \brief Destructor
	virtual ~VectorStorageClientImpl();

	/// \brief Prepare datastructures for calling findSimilar
	/// \remark This method does not have to be called, because the structures for search are built implicitely on the first search. It just avoids a massive delay on the first call of findSimilar.
	/// \param[in] type type of the features to search for
	/// \example "verb"
	void prepareSearch( const std::string& type);

	/// \brief For a defined type find the most similar vectors to a vector
	/// \param[in] type type of the features to search for
	/// \example "verb"
	/// \param[in] vec vector to search for (double[])
	/// \example [ 0.0322312 0.01121243 0.0078784 0.0012344 0.0064535 0.05454322 ]
	/// \param[in] maxNofResults maximum number of results to return
	/// \example 20
	/// \example 50
	/// \param[in] minSimilarity value between 0.0 and 1.0 specifying the minimum similarity a result should have 
	/// \example 0.9
	/// \example 0.85
 	/// \param[in] speedRecallFactor factor bigger than 0.0 used to adjust performance/recall, smaller value = better performance, bigger value = better recall
	/// \example 0.9
	/// \example 0.5
	/// \remark for the standard LSH implementation a good value is 0.8. In the standard LSH implementation the value describes the accepted difference in bits of a sample, compared with the average error accepted. For example if the accepted difference in bits of the LSH values compared is 400 out of 2048, then the accepted difference for a sample of 64 bits that decides wheter a candidate should be considered further is (1.0 + speedRecallFactor) * 400 / (2048/64)) = (1.0 + speedRecallFactor) * 12.5 bits.
	/// \param[in] realVecWeights true, if the real weights of the similarities should be calculated in the final result (for the best matches), false, if the weights of the final result should be the approximated values from the LSH similarity
	/// \example true
	/// \example false
	/// \return the list of most similar vectors (double[])
	Struct findSimilar( const std::string& type, const ValueVariant& vec, unsigned int maxNofResults, double minSimilarity, double speedRecallFactor, bool realVecWeights) const;

	/// \brief Create a vector storage transaction instance
	/// \return the transaction instance
	VectorStorageTransactionImpl* createTransaction();
	
	/// \brief Get the list of feature types defined
	/// \return the list
	/// \example ["noun","verb","adjectiv"]
	Struct types() const;

	/// \brief Get the list of types assigned to a specific featureValue
	/// \param[in] featureValue feature value to get the types assigned to
	/// \example "house"
	/// \example "running"
	/// \return the list of types assigned to 'featureValue'
	/// \example ["noun","verb"]
	Struct featureTypes( const std::string& featureValue) const;

	/// \brief Get an iterator on the feature strings inserted
	/// \return iterator on the feature values
	Iterator featureValues();

	/// \brief Get the number of vectors defined for the features of a type
	/// \param[in] type name of the type
	/// \example "verb"
	/// \return the number of vectors
	int nofVectors( const std::string& type) const;

	/// \brief Get the vector assigned to a feature addressed by index
	/// \param[in] type type of the feature to get the vector assiged to
	/// \example "noun"
	/// \example "verb"
	/// \param[in] feat value of the feature to get the vector assiged to
	/// \example "house"
	/// \example "running"
	/// \return the vector
	/// \example [ 0.08721391 0.01232134 0.02342453 0.0011312 0.0012314 0.087232243 ]
	WordVector featureVector( const std::string& type, const std::string& feat) const;

	/// \brief Calculate a value between 0.0 and 1.0 representing the similarity of two vectors
	/// \param[in] v1 first input vector
	/// \example [ 0.0322312 0.01121243 0.0078784 0.0012344 0.0064535 0.05454322 ]
	/// \param[in] v2 second input vector
	/// \example [ 0.0113123 0.094324 0.01242345 0.00479234 0.00423425 0.03213434 ]
	/// \return the similarity measure
	/// \example 0.978391
	/// \example 0.768423
	double vectorSimilarity( const ValueVariant& v1, const ValueVariant& v2) const;

	/// \brief Calculate the normalized vector representation of the argument vector
	/// \param[in] vec input vector
	/// \example [ 0.0322312 0.0112124 0.0078784 0.0012344 0.0064535 0.05454322 ]
	/// \return the normalized vector
	/// \example [ 0.0123124 0.0423423 0.0065654 0.0023133 0.0076574 0.06354521 ]
	WordVector normalize( const ValueVariant& vec) const;

	/// \brief Create an class for parsing query sentences based on features inserted in a vector storage
	/// \return the parser
	SentenceLexerImpl* createSentenceLexer() const;

	/// \brief Get the configuration of this storage
	/// \return the configuration structure
	Struct config() const;

	/// \brief Get the configuration of this storage as string
	/// \return the configuration string
	std::string configstring() const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	/// \remark This method is not implicitely called with the destructor because it might be a complicated operation that cannot be afforded in panic shutdown.
	/// \note the method does not have to be called necessarily.
	void close();

	/// \brief Call compaction method of the database if provided
	void compaction();

	/// \brief Introspect the storage structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \example ["config"]
	/// \example ["types"]
	/// \example ["values"]
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path) const;

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
	/// \param[in] type type of the feature added
	/// \example "noun"
	/// \example "verb"
	/// \param[in] feat value of the feature added
	/// \example "castle"
	/// \example "conquest"
	/// \param[in] vec vector assigned to the feature
	/// \example [ 0.08721391 0.01232134 0.02342453 0.0011312 0.0012314 0.087232243 ]
	void defineVector( const std::string& type, const std::string& feat, const ValueVariant& vec);

	/// \brief Define a feature without vector
	/// \param[in] type of the feature to add
	/// \param[in] feat name of the feature to add
	void defineFeature( const std::string& type, const std::string& feat);

	/// \brief Clear all vectors,types and feature names in the storage
	void clear();

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

