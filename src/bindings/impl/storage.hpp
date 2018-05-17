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
#include "strus/constants.hpp"
#include "strus/index.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/iterator.hpp"
#include "impl/value/struct.hpp"
#include "impl/value/introspectionBase.hpp"
#include <vector>
#include <string>

namespace strus {

///\brief Forward declaration
class StorageTransactionInterface;

namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \brief Forward declaration
class StorageTransactionImpl;

/// \class StorageClientImpl
/// \brief Object representing a client connection to the storage 
/// \note The only way to construct a storage client instance is to call Context::createStorageClient( config)
class StorageClientImpl
{
public:
	/// \brief Destructor
	virtual ~StorageClientImpl(){}

	/// \brief Get the number of documents inserted into this storage
	/// \return the total number of documents
	/// \example 112324
	/// \example 9873247
	unsigned int nofDocumentsInserted() const;

	/// \brief Get the number of inserted documents where a specific feature occurrs in
	/// \param[in] type the term type of the feature queried
	/// \example "WORD"
	/// \example "stem"
	/// \example "Name"
	/// \param[in] term the term value of the feature queried
	/// \example "John"
	/// \example "21314"
	/// \example "Z0 ssd-qx"
	/// \return the number of documents where the argument feature occurrs in
	/// \example 12321
	/// \example 98
	Index documentFrequency( const std::string& type, const std::string& term) const;

	/// \brief Get the internal document number from the document identifier
	/// \param[in] docid document identifier
	/// \example "doc://2132093"
	/// \example "http://www.acme.com/pub/acme?D=232133"
	/// \return internal document number or 0, if no document with this id is inserted
	/// \example 892374
	/// \example 1233
	/// \example 0
	Index documentNumber( const std::string& docid) const;

	/// \brief Get an interator on the tuples (value,pos) of the forward index of a given type for a document
	/// \param[in] docno internal local document number
	/// \example 312332
	/// \param[in] termtype term type string
	/// \example "WORD"
	/// \param[in] pos ordinal start position in forward index (where to start iterating)
	/// \example 311
	/// \return iterator on tuples (value,pos)
	Iterator documentForwardIndexTerms( const Index& docno, const std::string& termtype, const Index& pos=0) const;

	/// \brief Get an interator on the tuples (value,tf,firstpos) of the search index of a given type for a document
	/// \param[in] docno internal local document number
	/// \example 123
	/// \param[in] termtype term type string
	/// \example "stem"
	/// \return iterator on tuples (value,pos)
	Iterator documentSearchIndexTerms( const Index& docno, const std::string& termtype) const;

	/// \brief Get an iterator on the set of postings inserted
	/// \param[in] expression query term expression
	/// \example  [ "sequence" 10 ["sequence" 2 ["word" "complet"]  ["word" "diff"]]  ["sequence"  3  ["word" "you"]  ["word" "expect"]] ]
	/// \example  [ "word" "hello" ]
	/// \example  [ "sequence"  2  ["word" "ch"] ["number" "13"] ]
	/// \param[in] restriction meta data restrictions
	/// \example  [ [["=" "country" 12] ["=" "country" 17]]  ["<" "year" "2007"] ]
	/// \example  ["<" "year" "2002"]
	/// \param[in] start_docno starting document number
	/// \example 973141
	/// \example 873
	/// \return iterator on a set of postings
	Iterator postings( const ValueVariant& expression, const ValueVariant& restriction=ValueVariant(), const Index& start_docno=0);

	/// \brief Get an iterator on records of selected elements for matching documents starting from a specified document number
	/// \param[in] what list of items to select: names of document attributes or meta data or "position" for matching positions or "docno" for the document number
	/// \example  [ "docno" "title" "position" ]
	/// \param[in] expression query term expression
	/// \example  [ "within" 5 ["word" "world"]  ["word" "money"]]
	/// \example  [ "word" "hello" ]
	/// \example  [ "sequence"  2  ["word" "ch"] ["number" "13"] ]
	/// \param[in] restriction meta data restrictions
	/// \example  [ [["=" "country" 12] ["=" "country" 17]]  ["<" "year" "2007"] ]
	/// \example  ["<" "year" "2002"]
	/// \param[in] start_docno starting document number
	/// \example 973141
	/// \example 873
	/// \param[in] accesslist list of access restrictions (one of them must match)
	/// \example [ "public" "devel" ]
	/// \return iterator on a set of postings
	Iterator select( const ValueVariant& what, const ValueVariant& expression, const ValueVariant& restriction=ValueVariant(), const Index& start_docno=0, const ValueVariant& accesslist=ValueVariant());

	/// \brief Get an iterator on the term types inserted
	/// \return iterator on the term types
	Iterator termTypes() const;

	/// \brief Get an iterator on the document identifiers inserted
	/// \return iterator on the docids
	Iterator docids() const;

	/// \brief Get the document identifier associated with a local document number
	/// \param[in] docno local document number queried
	/// \example 79213
	/// \example 1
	/// \return the document identifier
	std::string docid( const Index& docno) const;

	/// \brief Get an iterator on the user names (roles) used in document access restrictions
	/// \return iterator on the user names (roles)
	Iterator usernames() const;

	/// \brief Get the list of inserted document attribute names
	/// \return list of names
	/// \example  ["name" "title" "docid"]
	std::vector<std::string>* attributeNames() const;

	/// \brief Get the list of inserted document metadata names
	/// \return list of names
	/// \example  ["date" "ccode" "category"]
	std::vector<std::string>* metadataNames() const;

	/// \brief Get an iterator on message blobs that all statistics of the storage (e.g. feature occurrencies and number of documents inserted)
	/// \note The blobs an be decoded with Context::unpackStatisticBlob
	/// \param[in] sign true = registration, false = deregistration, if false the sign of all statistics is inverted
	/// \return iterator on the encoded blobs of the complete statistics of the storage
	Iterator getAllStatistics( bool sign=true);

	/// \brief Get an iterator on message blobs that encode changes in statistics of the storage (e.g. feature occurrencies and number of documents inserted)
	/// \note The blobs an be decoded with Context::unpackStatisticBlob
	/// \return iterator on the encoded blobs of the statistic changes of the storage
	Iterator getChangeStatistics();

	/// \brief Create a transaction
	/// \return the transaction object (class StorageTransaction) created
	StorageTransactionImpl* createTransaction() const;

	/// \brief Get the configuration of this storage
	/// \return the configuration as structure
	/// \example [ path: "/srv/searchengine/storage" metadata:"doclen UINT32, date UINT32, docweight FLOAT" ]
	Struct config() const;

	/// \brief Get the configuration of this storage as string
	/// \return the configuration as string
	/// \example "path=/srv/searchengine/storage; metadata=doclen UINT32, date UINT32, docweight FLOAT"
	std::string configstring() const;

	/// \brief Close of the storage client
	void close();

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \example ["config"]
	/// \example ["termtypes"]
	/// \example ["attributenames"]
	/// \example ["metadatanames"]
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	StorageClientImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config);

	/// \brief Constructor used by Inserter
	friend class InserterImpl;
	StorageClientImpl( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& errorhnd_, const ObjectRef& storage_)
		:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_){}

	friend class QueryImpl;
	friend class QueryEvalImpl;
	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
};


/// \class StorageTransactionImpl
/// \brief Object representing a transaction of the storage 
/// \note The only way to construct a storage transaction instance is to call StorageClient::createTransaction()
class StorageTransactionImpl
{
public:
	/// \brief Destructor
	virtual ~StorageTransactionImpl(){}

	/// \brief Prepare the inserting a document into the storage
	/// \note The document is physically inserted with the call of 'commit()'
	/// \param[in] docid the identifier of the document to insert
	/// \param[in] doc the structure of the document to insert (analyzer::Document)
	void insertDocument( const std::string& docid, const ValueVariant& doc);

	/// \brief Prepare the deletion of a document from the storage
	/// \note The document is physically deleted with the call of 'commit()'
	/// \param[in] docid the identifier of the document to delete
	void deleteDocument( const std::string& docid);

	/// \brief Prepare the deletion of all document access rights of a user
	/// \note The user access rights are changed accordingly with the next implicit or explicit call of 'flush'
	/// \param[in] username the name of the user to delete all access rights (in the local collection)
	void deleteUserAccessRights( const std::string& username);

	/// \brief Commit all insert or delete or user access right change statements of this transaction.
	/// \remark throws an error on failure
	void commit();

	/// \brief Rollback all insert or delete or user access right change statements of this transaction.
	void rollback();

private:
	friend class StorageClientImpl;
	StorageTransactionImpl( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& errorhnd_, const ObjectRef& storage_);

	friend class InserterTransactionImpl;
	StorageTransactionImpl( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& errorhnd_, const ObjectRef& storage_, const ObjectRef& transaction_)
		:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_transaction_impl(transaction_){}

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

