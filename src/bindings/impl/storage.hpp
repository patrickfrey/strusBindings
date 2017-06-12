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
#include <vector>
#include <string>

namespace strus {

///\brief Forward declaration
class StorageTransactionInterface;

namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \brief Forward declaration
class StorageTransactionImpl;
/// \brief Forward declaration
class StatisticsIteratorImpl;
/// \brief Forward declaration
class DocumentBrowserImpl;
/// \brief Forward declaration
class Struct;

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

	/// \brief Create a transaction
	/// return the transaction object created
	StorageTransactionImpl* createTransaction() const;

	/// \brief Create an iterator on the storage statistics (total value) to distribute for initialization/deinitialization
	/// \param[in] sign true = registration, false = deregistration
	/// return the statistics iterator object created
	StatisticsIteratorImpl* createStatisticsIterator( bool sign);

	/// \brief Create an iterator on the storage statistics (relative value) to distribute after storage updates
	/// return the statistics message iterator object created
	StatisticsIteratorImpl* createUpdateStatisticsIterator();

	/// \brief Create a document browser instance
	DocumentBrowserImpl* createDocumentBrowser() const;

	/// \brief Get the configuration of this storage
	/// \return the configuration as structure
	std::vector<std::pair<std::string,std::string> >* config() const;

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

///\brief Implements browsing the documents of a storage without weighting query, just with a restriction on metadata
class DocumentBrowserImpl
{
public:
	/// \brief Destructor
	~DocumentBrowserImpl(){}

	/// \brief Define a meta data restriction condition on the documents visited
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	/// \remark Metadata restrictions can only be defined before the first call of this DocumentBrowser::next()
	void addMetaDataRestrictionCondition(
			const char* compareOp, const std::string& name,
			const ValueVariant& value, bool newGroup);

	///\brief Get the internal document number of the next document bigger or equal the document number passed
	///\param[in] docno document number to get the matching least upperbound from
	///\return the internal document number or 0 if no more documents defined
	Index skipDoc( int docno);

	///\brief Get a list of attributes, resp. metadata elements addressed by name
	///\param[in] docno document number to get the selected elements from
	///\param[in] elementsSelected array of names of elements to select
	///\return the array of values of the elements selected
	Struct get( int docno, const ValueVariant& elementsSelected);

private:
	friend class StorageClientImpl;
	DocumentBrowserImpl(
		const ObjectRef& trace_impl_,
		const ObjectRef& objbuilder_impl_,
		const ObjectRef& storage_impl_,
		const ObjectRef& errorhnd_);

private:
	ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_restriction_impl;
	ObjectRef m_postingitr_impl;
	ObjectRef m_attributereader_impl;
	ObjectRef m_metadatareader_impl;
};

}}//namespace
#endif

