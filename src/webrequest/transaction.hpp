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
		:m_context(context_),m_ref(ref_),m_tidx(tidx_),m_timeout_after_s(timeout_after_s_){}
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

private:
	papuga_RequestContext* m_context;
	int* m_ref;
	int m_timeout_after_s;
	uint64_t m_tidx;
};
typedef strus::shared_ptr<Transaction> TransactionRef;


class TransactionPool
{
public:
	TransactionPool( int maxTransactionTimeout_, int nofTransactionPerSecond_);
	~TransactionPool();

	void tick();

	/// \brief Create a transaction holding the context object passed
	/// \param[in] context transaction context passed by ownership
	/// \param[in] timeout_after_s maximum livetime of transaction
	/// \return Transaction identifier
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
	static time_t time();
	static TransactionRef newTransaction( papuga_RequestContext* context, int timeout_after_s);
	static std::string transactionId( uint64_t tidx);
	static uint64_t transactionIndex( const std::string& tid);
	uint64_t nextRand();
	void clear();
	int aridx( int timeout_after_s);

private:
	TransactionRef* m_ar;				///< map expiration time to object reference
	int* m_refar;					///< map transaction id to transaction object reference
	std::size_t m_arsize;				///< size of refar/ar in elements
	uint64_t m_lasttick;				///< last tick call index
	int m_maxTransactionTimeout;			///< maximum timeout value for untouched transactions
	int m_nofTransactionPerSecond;			///< allocation dimension value for ar/refar
	int m_allocNofTries;				///< number of tries to get a lock
	uint64_t m_randSeed;				///< seed for pseudo random numers
	enum {NofMutex=32};				///< number of mutexes
	strus::mutex m_mutex_refar[ NofMutex];		///< mutex array for accessing refar
	strus::mutex m_mutex_ar[ NofMutex];		///< mutex array for accessing ar
	strus::AtomicFlag m_tickflag;			///< flag controlling mutual exclusion of ticker calls
};


}//namespace
#endif

