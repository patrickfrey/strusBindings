/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Structure to build and map the Result of an XML/JSON request
 * @file requestResult.h
 */
#ifndef _STRUS_WEBREQUEST_TRANSACTION_HPP_INCLUDED
#define _STRUS_WEBREQUEST_TRANSACTION_HPP_INCLUDED
#include "papuga/requestHandler.h"
#include "strus/base/shared_ptr.hpp"
#include "strus/base/thread.hpp"
#include "strus/base/atomic.hpp"
#include "strus/base/stdint.h"
#include "strus/errorCodes.hpp"
#include <ctime>

namespace strus {

class Transaction
{
public:
	Transaction( papuga_RequestContext* context_, uint64_t tidx_, int* ref_, int timeout_after_s_)
		:m_context(context_),m_ref(ref_),m_timeout_after_s(timeout_after_s_),m_tidx(tidx_){}
	~Transaction()
	{
		if (m_context) papuga_destroy_RequestContext( m_context);
		*m_ref = -1;
	}
	uint64_t idx() const
	{
		return m_tidx;
	}
	int timeout_after_s() const
	{
		return m_timeout_after_s;
	}
	void setRef( int refidx)
	{
		*m_ref = refidx;
	}

private:
	papuga_RequestContext* m_context;
	int* m_ref;
	int m_timeout_after_s;
	uint64_t m_tidx;
};
typedef strus::shared_ptr<Transaction> TransactionRef;

/// \brief Pool for managing transaction objects
class TransactionPool
{
public:
	/// \brief Constructor
	/// \param[in] maxTransactionTimeout_ maximum timeout value for untouched transactions
	/// \param[in] nofTransactionPerSecond_ 2nd allocation dimension value for ar/refar besides maxTransactionTimeout
	TransactionPool( int maxTransactionTimeout_, int nofTransactionPerSecond_);

	/// \brief Destructor
	~TransactionPool();

	/// \brief Signal the garbagge collector to do its job
	void tick();

	/// \brief Create a transaction holding the context object passed
	/// \param[in] context transaction context (passed with ownership)
	/// \param[in] timeout_after_s maximum time intervall a transaction lives without beeing touched by the client, timeout counter is renewed with every touch
	/// \return transaction identifier of the transaction created
	std::string createTransaction( papuga_RequestContext* context, int timeout_after_s);

	/// \brief Get a transaction object addressed by its identifier
	/// \param[in] tid transaction identifier
	/// \return Transaction reference
	/// \note A transaction is processed by fetching it and returning it after processing. If the time between the last return and the succeding fetch exceeds the timeout set on create transaction, then the transaction is destroyed
	TransactionRef fetchTransaction( const std::string& tid);

	/// \brief Return a transaction object after operation
	/// \param[in] tr transaction reference
	bool returnTransaction( const TransactionRef& tr);

	/// \brief Release a transaction object addressed by its identifier
	/// \param[in] tid transaction identifier
	void releaseTransaction( const std::string& tid);

private:
	/// \brief Get the current time as pseudo random generator seed
	static time_t time();

	/// \brief Create a new transaction
	/// \param[in] context transaction context (passed with ownership)
	/// \param[in] timeout_after_s maximum time intervall a transaction lives without beeing touched by the client, timeout counter is renewed with every touch
	TransactionRef newTransaction( papuga_RequestContext* context, int timeout_after_s);

	/// \brief Get a transaction identifier as string
	/// \param[in] tidx internal transaction index
	/// \return transaction id string
	static std::string transactionId( uint64_t tidx);

	/// \brief Get the internal transaction identifier from its string representation
	/// \note inverse of transactionId
	/// \param[in] tid transaction id string
	/// \return internal transaction index
	static uint64_t transactionIndex( const std::string& tid);

	/// \brief Get a psuedo random number
	/// \return an insecure psuedo random number
	uint64_t nextRand();

	/// \brief Clear all allocated structures
	void clear();

	/// \brief Get a candidate for a transaction reference slot
	int transactionRefIndexCandidate( int timeout_after_s);

private:
	TransactionRef* m_ar;				///< map expiration time to object reference
	int* m_refar;					///< map transaction id to transaction object reference
	std::size_t m_arsize;				///< size of refar/ar in elements
	uint64_t m_lasttick;				///< last tick call index
	int m_maxTransactionTimeout;			///< maximum timeout value for untouched transactions
	int m_nofTransactionPerSecond;			///< 2nd allocation dimension value for ar/refar besides maxTransactionTimeout
	int m_allocNofTries;				///< number of tries to get a lock
	uint64_t m_randSeed;				///< seed for pseudo random numers
	enum {NofMutex=32};				///< number of mutexes
	strus::mutex m_mutex_refar[ NofMutex];		///< mutex array for accessing refar
	strus::mutex m_mutex_ar[ NofMutex];		///< mutex array for accessing ar
	strus::AtomicFlag m_tickflag;			///< flag controlling mutual exclusion of ticker calls
};


}//namespace
#endif

