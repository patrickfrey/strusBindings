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
#include "transaction.hpp"
#include "private/internationalization.hpp"
#include <cstdio>
#include <limits>
#include <cstdlib>
#include <cstring>

using namespace strus;

static std::string transactionId_( int64_t tidx)
{
	char buf[ 64];
	const int64_t mod32 = ((int64_t)1<<32);
	std::snprintf( buf, sizeof(buf), "%08x%08x", (unsigned int)(tidx / mod32), (unsigned int)(tidx % mod32));
	return std::string( buf, 16);
}

static int64_t transactionIndex_( const std::string& tid)
{
	unsigned int p1,p2;
	if (2!=std::sscanf( tid.c_str(), "%08x%08x", &p1, &p2))
	{
		throw strus::runtime_error( ErrorCodeInvalidArgument, _TXT("illegal id of transaction"));
	}
	return ((int64_t)p1 << 32) + p2;
}

std::string Transaction::id() const
{
	return transactionId_( m_tidx);
}


TransactionPool::TransactionPool( int64_t timecount, int maxTransactionTimeout_, int nofTransactionPerSecond_)
		:m_ar(0),m_refar(0),m_arsize(64),m_lasttick(timecount)
		,m_maxTransactionTimeout(maxTransactionTimeout_)
		,m_nofTransactionPerSecond(nofTransactionPerSecond_)
		,m_allocNofTries(0)
		,m_randSeed(timecount)
		,m_tickflag(false)
{
	if (m_maxTransactionTimeout == 0) m_maxTransactionTimeout = 8;
	if (m_nofTransactionPerSecond == 0) m_nofTransactionPerSecond = 1;
	if (m_nofTransactionPerSecond < 0 || m_nofTransactionPerSecond >= (1<<20)) throw strus::runtime_error( ErrorCodeMaxLimitReached, _TXT("max transaction per second exceeds maximum limit"));
	if (m_maxTransactionTimeout < 0 || m_maxTransactionTimeout >= (1<<20)) throw strus::runtime_error( ErrorCodeMaxLimitReached, _TXT("max transaction duration exceeds maximum limit"));
	std::size_t min_arsize = m_maxTransactionTimeout * m_nofTransactionPerSecond;
	while (m_arsize < min_arsize) m_arsize *= 2;
	m_allocNofTries = m_nofTransactionPerSecond * 8;
	m_ar = new TransactionRef[ m_arsize];
	m_refar = (int*)std::malloc( m_arsize * sizeof(m_refar[0]));
	if (!m_refar)
	{
		clear();
		throw std::bad_alloc();
	}
	std::memset( m_refar, -1, m_arsize * sizeof(m_refar[0]));
}

void TransactionPool::clear()
{
	if (m_ar) {delete [] m_ar; m_ar=0;}
	if (m_refar) {std::free( m_refar); m_refar=0;}
}

TransactionPool::~TransactionPool()
{
	clear();
}

void TransactionPool::collectGarbage( int64_t timecount)
{
	if (!m_tickflag.set( true)) return;
	int64_t aridx = m_lasttick % (m_arsize-1);
	m_lasttick = timecount;
	int64_t arend = m_lasttick % (m_arsize-1);
	aridx *= m_nofTransactionPerSecond;
	arend *= m_nofTransactionPerSecond;
	aridx %= (m_arsize-1);
	arend %= (m_arsize-1);
	if (arend < aridx) arend += m_arsize;

	for (; aridx < arend; ++aridx)
	{
		m_ar[ aridx & (m_arsize-1)].reset();
	}
	m_tickflag.set( false);
}


int TransactionPool::transactionRefIndexCandidate( int keepAliveTimeCount)
{
	return (((m_lasttick % (m_arsize-1)) + keepAliveTimeCount) * m_nofTransactionPerSecond) % (m_arsize-1);
}

TransactionRef TransactionPool::newTransaction( papuga_RequestContext* context, int keepAliveTimeCount)
{
	if (keepAliveTimeCount <= 0 || keepAliveTimeCount > m_maxTransactionTimeout)
	{
		keepAliveTimeCount = m_maxTransactionTimeout;
	}
	int64_t tidx;
	int* tidxref = 0;
	{
		int cnt = m_allocNofTries;
		while (cnt >= 0)
		{
			tidx = nextRand();
			strus::scoped_lock lock( m_mutex_refar[ tidx % NofMutex]);
			tidxref = &m_refar[ tidx & (m_arsize-1)];
			if (*tidxref == -1)
			{
				*tidxref = std::numeric_limits<int>::max(); 
				break;
			}
			--cnt;
		}
		if (!cnt) return TransactionRef();
	}{
		int64_t eidx = transactionRefIndexCandidate( keepAliveTimeCount) + (tidx % m_nofTransactionPerSecond);
		int cnt = m_allocNofTries;
		while (cnt >= 0)
		{
			try
			{
				strus::scoped_lock lock( m_mutex_ar[ eidx % NofMutex]);
				TransactionRef& tref = m_ar[ eidx & (m_arsize-1)];
				if (!tref.get())
				{
					TransactionRef rt( new Transaction( context, tidx, tidxref, keepAliveTimeCount));
					*tidxref = eidx & (m_arsize-1);
					return m_ar[ eidx & (m_arsize-1)] = rt;
				}
			}
			catch (...)
			{
				*tidxref = -1; //... roll back
				throw std::bad_alloc();
			}
			--cnt;
			++eidx;
		}
		*tidxref = -1; //... roll back
		return TransactionRef();
	}
}

std::string TransactionPool::createTransaction( papuga_RequestContext* context, int keepAliveTimeCount)
{
	try
	{
		TransactionRef tr = newTransaction( context, keepAliveTimeCount);
		if (!tr.get()) return std::string();
		return transactionId( tr->idx());
	}
	catch (...)
	{
		return std::string();
	}
}

TransactionRef TransactionPool::fetchTransaction( const std::string& tid)
{
	int64_t tidx = transactionIndex( tid);
	int eidx = m_refar[ tidx & (m_arsize-1)];
	TransactionRef rt = m_ar[ eidx & (m_arsize-1)];
	if (rt.get() && tidx == rt->idx())
	{
		m_ar[ eidx & (m_arsize-1)].reset();
		return rt;
	}
	return TransactionRef();
}

bool TransactionPool::returnTransaction( const TransactionRef& tr)
{
	int64_t eidx = transactionRefIndexCandidate( tr->keepAliveTimeCount()) + (tr->idx() % m_nofTransactionPerSecond);
	int cnt = m_allocNofTries;
	while (cnt >= 0)
	{
		try
		{
			strus::scoped_lock lock( m_mutex_ar[ eidx % NofMutex]);
			TransactionRef& tref = m_ar[ eidx & (m_arsize-1)];
			if (!tref.get())
			{
				m_ar[ eidx & (m_arsize-1)] = tr;
				tr->setRef( eidx & (m_arsize-1));
				return true;
			}
		}
		catch (...)
		{
			return false;
		}
		--cnt;
		++eidx;
	}
	return false;
}

std::string TransactionPool::transactionId( int64_t tidx)
{
	return transactionId_( tidx);
}

int64_t TransactionPool::transactionIndex( const std::string& tid)
{
	return transactionIndex_( tid);
}

int64_t TransactionPool::nextRand()
{
	/* Written in 2015 by Sebastiano Vigna (vigna@acm.org), see http://xorshift.di.unimi.it/splitmix64.c */
	int64_t rnd = m_lasttick + m_randSeed + 0x9e3779b97f4a7c15;
	rnd = (rnd ^ (rnd >> 30)) * 0xbf58476d1ce4e5b9;
	rnd = (rnd ^ (rnd >> 27)) * 0x94d049bb133111eb;
	rnd = rnd ^ (rnd >> 31);
	m_randSeed = rnd;
	return m_randSeed ^ (uintptr_t)&rnd;
}


