/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Structure to build and map the Result of an XML/JSON request
 * @file transaction.hpp
 */
#ifndef _STRUS_WEBREQUEST_TRANSACTION_HPP_INCLUDED
#define _STRUS_WEBREQUEST_TRANSACTION_HPP_INCLUDED
#include "papuga/requestContext.h"
#include "strus/base/shared_ptr.hpp"
#include "strus/base/thread.hpp"
#include "strus/base/atomic.hpp"
#include "strus/base/stdint.h"
#include "strus/errorCodes.hpp"
#include "papugaContextRef.hpp"
#include <string>

namespace strus {

/// \brief Forward declaration
class WebRequestLoggerInterface;

class Transaction
{
public:
	Transaction( const std::string& contextType_, const PapugaContextRef& context_, int64_t tidx_, int* ref_, int maxIdleTime_)
		:m_contextType(contextType_),m_context(context_),m_ref(ref_),m_maxIdleTime(maxIdleTime_),m_tidx(tidx_){}
	~Transaction()
	{
		*m_ref = -1;
	}
	int64_t idx() const
	{
		return m_tidx;
	}
	int maxIdleTime() const
	{
		return m_maxIdleTime;
	}
	void setRef( int refidx)
	{
		*m_ref = refidx;
	}
	const char* contextType() const
	{
		return m_contextType.c_str();
	}
	const PapugaContextRef& context() const
	{
		return m_context;
	}
	std::string id() const;

private:
	std::string m_contextType;
	PapugaContextRef m_context;
	int* m_ref;
	int m_maxIdleTime;
	int64_t m_tidx;
};
typedef strus::shared_ptr<Transaction> TransactionRef;

/// \brief Pool for managing transaction objects
class TransactionPool
{
public:
	/// \brief Constructor
	/// \param[in] timecount current time counter value
	/// \note the time unit or granularity is defined by the caller
	/// \param[in] maxIdleTime_ maximum timeout value for untouched transactions in the unit provided by timecount
	/// \param[in] nofTransactionsPerSlot_ 2nd allocation dimension value for the sliding window used internally for open transactions besides maxIdleTime
	/// \param[in] logger_ logger interface (NULL if not defined)
	TransactionPool( int64_t timecount, int maxIdleTime_, int nofTransactionsPerSlot_, WebRequestLoggerInterface* logger_);

	/// \brief Destructor
	~TransactionPool();

	/// \brief Signal the garbagge collector to do its job
	/// \param[in] timecount current time counter value
	/// \note the time unit or granularity is defined by the caller and must much the granularity used in the constructor
	void collectGarbage( int64_t timecount);

	/// \brief Create a transaction holding the context object passed
	/// \param[in] contextType type name used to address schemas and methods of this transaction object
	/// \param[in] context transaction context reference
	/// \param[in] maxIdleTime maximum time intervall a transaction lives without beeing touched by the client, timeout counter is renewed with every touch
	/// \return transaction identifier of the transaction created
	std::string createTransaction( const std::string& contextType, const PapugaContextRef& context, int maxIdleTime);

	/// \brief Get a transaction object addressed by its identifier
	/// \param[in] tid transaction identifier
	/// \return Transaction reference
	/// \note A transaction is processed by fetching it and returning it after processing. If the time between the last return and the succeding fetch exceeds the timeout set on create transaction, then the transaction is destroyed
	TransactionRef fetchTransaction( const std::string& tid);

	/// \brief Return a transaction object after operation
	/// \param[in] tr transaction reference
	void returnTransaction( const TransactionRef& tr);

private:
	/// \brief Create a new transaction
	/// \param[in] context transaction context reference
	/// \param[in] maxIdleTime maximum time intervall a transaction lives without beeing touched by the client, timeout counter is renewed with every touch
	TransactionRef newTransaction( const std::string& contextType, const PapugaContextRef& context, int maxIdleTime);

	/// \brief Get a transaction identifier as string
	/// \param[in] tidx internal transaction index
	/// \return transaction id string
	static std::string transactionId( int64_t tidx);

	/// \brief Get the internal transaction identifier from its string representation
	/// \note inverse of transactionId
	/// \param[in] tid transaction id string
	/// \return internal transaction index
	static int64_t transactionIndex( const std::string& tid);

	/// \brief Get a psuedo random number
	/// \return an insecure psuedo random number
	int64_t nextRand();

	/// \brief Clear all allocated structures
	void clear();

	/// \brief Get a candidate for a transaction reference slot
	int transactionRefIndexCandidate( int maxIdleTime);

private:
#if __cplusplus >= 201103L
	TransactionPool( const TransactionPool&)=delete;
#else
	TransactionPool( const TransactionPool&){}	///< noncopyable
	void operator=( const TransactionPool&){}	///< noncopyable
#endif

private:
	WebRequestLoggerInterface* m_logger;		///< logger interface
	TransactionRef* m_ar;				///< map expiration time to object reference
	int* m_refar;					///< map transaction id to transaction object reference
	std::size_t m_arsize;				///< size of refar/ar in elements
	int64_t m_lasttick;				///< last tick call index
	int m_maxIdleTime;				///< maximum timeout value for untouched transactions
	int m_nofTransactionPerSlot;			///< 2nd allocation dimension value for ar/refar besides maxIdleTime
	int m_allocNofTries;				///< number of tries to get a lock
	int64_t m_randSeed;				///< seed for pseudo random numers
	enum {NofMutex=64};				///< number of mutexes
	strus::mutex m_mutex_refar[ NofMutex];		///< mutex array for accessing refar
	strus::mutex m_mutex_ar[ NofMutex];		///< mutex array for accessing ar
	strus::AtomicFlag m_tickflag;			///< flag controlling mutual exclusion of ticker calls
};

}//namespace
#endif

