/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_INSERTER_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_INSERTER_HPP_INCLUDED
#include "storage.hpp"
#include "analyzer.hpp"

namespace strus {
namespace bindings {

/// \brief Forward declaration
class InserterTransactionImpl;

/// \class InserterImpl
/// \brief Object representing a client connection to the storage and an analyzer providing inserts of content to be analyzed
/// \note The only way to construct an inserter instance is to call Context::createInserter( storage, analyzer)
class InserterImpl
{
public:
	/// \brief Destructor
	virtual ~InserterImpl(){}

	/// \brief Create a transaction
	/// \return the transaction object (class InserterTransaction) created
	InserterTransactionImpl* createTransaction() const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;

	InserterImpl( const StorageClientImpl* storage, const DocumentAnalyzerImpl* analyzer)
		:m_storage( storage->m_trace_impl, storage->m_objbuilder_impl, storage->m_errorhnd_impl, storage->m_storage_impl)
		,m_analyzer( analyzer->m_trace_impl, analyzer->m_objbuilder_impl, analyzer->m_errorhnd_impl, analyzer->m_analyzer_impl, analyzer->m_textproc){}

	StorageClientImpl m_storage;
	DocumentAnalyzerImpl m_analyzer;

};

/// \class InserterTransactionImpl
/// \brief Object representing a transaction of the storage for insering content to be analyzed
/// \note The only way to construct an inserter transaction instance is to call Inserter::createTransaction()
class InserterTransactionImpl
{
public:
	/// \brief Destructor
	virtual ~InserterTransactionImpl(){}

	/// \brief Prepare the inserting a document into the storage
	/// \note The document is physically inserted with the call of 'commit()'
	/// \param[in] docid the identifier of the document to insert or empty if document id is extracted by analyzer
	/// \param[in] doc plain content of the document to analyze and insert
	/// \param[in] documentClass (optional) document class of the document to insert (autodetection if undefined)
	void insertDocument( const std::string& docid, const std::string& doc, const ValueVariant& documentClass=ValueVariant());

	/// \brief Prepare the deletion of a document from the storage
	/// \note The document is physically deleted with the call of 'commit()'
	/// \param[in] docid the identifier of the document to delete
	void deleteDocument( const std::string& docid)
	{
		m_transaction.deleteDocument( docid);
	}

	/// \brief Prepare the deletion of all document access rights of a user
	/// \note The user access rights are changed accordingly with the next implicit or explicit call of 'flush'
	/// \param[in] username the name of the user to delete all access rights (in the local collection)
	void deleteUserAccessRights( const std::string& username)
	{
		m_transaction.deleteUserAccessRights( username);
	}

	/// \brief Commit all insert or delete or user access right change statements of this transaction.
	/// \remark throws an error on failure
	void commit()
	{
		m_transaction.commit();
	}

	/// \brief Rollback all insert or delete or user access right change statements of this transaction.
	void rollback()
	{
		m_transaction.rollback();
	}

private:
	friend class InserterImpl;

	InserterTransactionImpl( const StorageTransactionImpl* transaction, const DocumentAnalyzerImpl* analyzer)
		:m_transaction( transaction->m_trace_impl, transaction->m_objbuilder_impl, transaction->m_errorhnd_impl, transaction->m_storage_impl, transaction->m_transaction_impl)
		,m_analyzer( analyzer->m_trace_impl, analyzer->m_objbuilder_impl, analyzer->m_errorhnd_impl, analyzer->m_analyzer_impl, analyzer->m_textproc){}
	
	StorageTransactionImpl m_transaction;
	DocumentAnalyzerImpl m_analyzer;
};

}}//namespace
#endif

