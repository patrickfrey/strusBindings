/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STORAGE_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STORAGE_HPP_INCLUDED
#include "strus/bindings/hostObjectReference.hpp"
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/callResult.hpp"
#include "strus/numericVariant.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \brief Object representing a client connection to the storage 
/// \remark The only way to construct a storage client instance is to call Context::createStorageClient(const std::string&)
class StorageClientImpl
{
public:
	/// \brief Copy constructor
	StorageClientImpl( const StorageClientImpl& o);

	/// \brief Destructor
	virtual ~StorageClientImpl(){}

	/// \brief Get the number of documents inserted into the storage
	/// return the total number of documents
	CallResult nofDocumentsInserted() const;

	/// \brief Create a transaction
	/// return the transaction object created
	CallResult createTransaction() const;

	/// \brief Create an iterator on the storage statistics (total value) to distribute for initialization/deinitialization
	/// \param[in] sign true = registration, false = deregistration
	/// return the statistics iterator object created
	CallResult createInitStatisticsIterator( bool sign) const;

	/// \brief Create an iterator on the storage statistics (relative value) to distribute after storage updates
	/// return the statistics message iterator object created
	CallResult createUpdateStatisticsIterator() const;

	/// \brief Create a document browser instance
	CallResult createDocumentBrowser();

	/// \brief Close of the storage client
	void close();

private:
	friend class ContextImpl;
	StorageClientImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config);

	friend class QueryImpl;
	friend class QueryEvalImpl;
	mutable HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_storage_impl;
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
	StorageTransactionImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const HostObjectReference& storage_);

	friend class QueryImpl;
	friend class QueryEvalImpl;
	mutable HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_storage_impl;
	HostObjectReference m_transaction_impl;
};

}}//namespace
#endif

