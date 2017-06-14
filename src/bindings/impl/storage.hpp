/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STORAGE_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STORAGE_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/numericVariant.hpp"
#include "strus/index.hpp"
#include "impl/objectref.hpp"
#include "impl/iterator.hpp"
#include "impl/struct.hpp"
#include <vector>
#include <string>

namespace strus {

///\brief Forward declaration
class StorageTransactionInterface;

namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \brief Forward declaration
class StorageTransactionImpl;

/// \brief Object representing a client connection to the storage 
/// \remark The only way to construct a storage client instance is to call Context::createStorageClient(const std::string&)
class StorageClientImpl
{
public:
	/// \brief Destructor
	virtual ~StorageClientImpl(){}

	/// \brief Get the number of documents inserted into this storage
	/// return the total number of documents
	unsigned int nofDocumentsInserted() const;

	/// \brief Get the number of documents inserted in this storage instance
	/// \param[in] type the term type addressed
	/// \param[in] term the term value addressed
	/// \return the number of documents
	Index documentFrequency( const std::string& type, const std::string& term) const;

	/// \brief Get the internal document number from the document identifier
	/// return the internal document number or 0, if not defined
	Index documentNumber( const std::string& docid) const;

	/// \brief Get an iterator on the set of postings inserted
	/// \param[in] expression query term expression
	/// \param[in] restriction meta data restrictions
	/// \param[in] start_docno starting document number
	/// return an iterator on a set of postings
	Iterator postings( const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno);

	/// \brief Get an iterator on records of selected elements for matching documents starting from a specified document number
	/// \param[in] what list of items to select: names of document attributes or meta data or "position" for matching positions or "docno" for the document number
	/// \param[in] expression query term expression
	/// \param[in] restriction meta data restrictions
	/// \param[in] start_docno starting document number
	/// return an iterator on a set of postings
	Iterator select( const ValueVariant& what, const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno);

	/// \brief Get an iterator on the term types inserted
	/// return an iterator on the term types
	Iterator termTypes() const;

	/// \brief Get an iterator on the document identifiers inserted
	/// return an iterator on the docids
	Iterator docids() const;

	/// \brief Get an iterator on the user names (roles) used in document access restrictions
	/// \return an iterator on the user names (roles)
	Iterator usernames() const;

	/// \brief Get an iterator on message blobs that all statistics of the storage (e.g. feature occurrencies and number of documents inserted)
	/// \note The blobs an be decoded with Context::unpackStatisticBlob
	/// \param[in] sign true = registration, false = deregistration, if false the sign of all statistics is inverted
	/// \return an iterator on the encoded blobs of the complete statistics of the storage
	Iterator getAllStatistics( bool sign=true);

	/// \brief Get an iterator on message blobs that encode changes in statistics of the storage (e.g. feature occurrencies and number of documents inserted)
	/// \note The blobs an be decoded with Context::unpackStatisticBlob
	/// \return an iterator on the encoded blobs of the statistic changes of the storage
	Iterator getChangeStatistics();

	/// \brief Create a transaction
	/// return the transaction object created
	StorageTransactionImpl* createTransaction() const;

	/// \brief Get the configuration of this storage
	/// \return the configuration as structure
	Struct config() const;

	/// \brief Get the configuration of this storage as string
	/// \return the configuration as string
	std::string configstring() const;

	/// \brief Close of the storage client
	void close();

private:
	friend class ContextImpl;
	StorageClientImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config);

	friend class QueryImpl;
	friend class QueryEvalImpl;
	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
};


/// \brief Object representing a transaction of the storage 
/// \remark The only way to construct a storage transaction instance is to call StorageClient::createTransaction()
class StorageTransactionImpl
{
public:
	/// \brief Destructor
	virtual ~StorageTransactionImpl(){}

	/// \brief Prepare the inserting a document into the storage
	/// \param[in] docid the identifier of the document to insert
	/// \param[in] doc the structure of the document to insert (analyzer::Document)
	/// \remark The document is physically inserted with the call of 'commit()'
	void insertDocument( const std::string& docid, const ValueVariant& doc);

	/// \brief Prepare the deletion of a document from the storage
	/// \param[in] docid the identifier of the document to delete
	/// \remark The document is physically deleted with the call of 'commit()'
	void deleteDocument( const std::string& docid);

	/// \brief Prepare the deletion of all document access rights of a user
	/// \param[in] username the name of the user to delete all access rights (in the local collection)
	/// \remark The user access rights are changed accordingly with the next implicit or explicit call of 'flush'
	void deleteUserAccessRights( const std::string& username);

	/// \brief Commit all insert or delete or user access right change statements of this transaction.
	void commit();

	/// \brief Rollback all insert or delete or user access right change statements of this transaction.
	void rollback();

private:
	friend class StorageClientImpl;
	StorageTransactionImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const ObjectRef& storage_);

	friend class QueryImpl;
	friend class QueryEvalImpl;
	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_transaction_impl;
};

}}//namespace
#endif

