#include "transaction.hpp"
#include "strus/base/pseudoRandom.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/thread.hpp"
#include "strus/base/atomic.hpp"
#include "strus/base/sleep.hpp"
#include "strus/base/unique_ptr.hpp"
#include "strus/base/platform.hpp"
#include "papuga/typedefs.h"
#include "papuga/requestHandler.h"
#include "papuga/valueVariant.h"
#include "papuga/errors.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <map>

static strus::PseudoRandom g_random;
static strus::AtomicCounter<int> g_errorCounter;
static bool g_verbose = false;
static int64_t g_timecnt_start = 0;
static strus::AtomicCounter<int64_t> g_timecnt;
static strus::unique_ptr<strus::TransactionPool> g_tpool;

static bool odd( int num)
{
	return ((num & 1) == 1);
}


static void runThread( int treadidx, int nofIterations, int randomSeed)
{
	try
	{
		strus::PseudoRandom random( randomSeed);
		int ii;
		std::map<std::string,int> refmap;
		std::vector<std::string> tidlist;
		papuga_ErrorCode errcode = papuga_Ok;
	
		for (ii=0; ii<nofIterations; ++ii)
		{
			int idx = ii+1;
			papuga_RequestContext* ctx = papuga_create_RequestContext( 0/*class name*/);
			papuga_ValueVariant val;
			papuga_init_ValueVariant_int( &val, idx);
			if (!papuga_RequestContext_add_variable( ctx, "index", &val)) throw std::bad_alloc();
			int timeout = odd(idx) ? random.get( 7, 10) : random.get( 1, 3);
			std::string tid = g_tpool->createTransaction( ctx, timeout);
			if (tid.empty())
			{
				papuga_destroy_RequestContext( ctx);
				throw std::bad_alloc();
			}
			tidlist.push_back( tid);
			refmap[ tid] = idx;
			if (g_verbose) std::cerr << strus::string_format( "create transaction %s [i=%d,t=%d]\n", tid.c_str(), idx, timeout);
		}
		for (ii=0; ii<nofIterations; ++ii)
		{
			int idx = ii+1;
			strus::TransactionRef tref = g_tpool->fetchTransaction( tidlist[ ii]);
			if (!tref.get()) throw std::runtime_error("lost transaction");
			papuga_RequestContext* ctx = tref->context();
			const papuga_ValueVariant* var = papuga_RequestContext_get_variable( ctx, "index", NULL/*param[out] isArray*/);
			if (!var) throw std::runtime_error("index variable of context undefined");
			int value = papuga_ValueVariant_toint( var, &errcode);
			if (errcode != papuga_Ok) throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
			if (value != idx) throw std::runtime_error("index variable of context does not match");
			std::string tid = tref->id();
			if (g_verbose) std::cerr << strus::string_format( "check transaction %s [i=%d]\n", tid.c_str(), idx);
			g_tpool->returnTransaction( tref);
		}
		strus::sleep( 5);
		for (ii=0; ii<nofIterations; ++ii)
		{
			int idx = ii+1;
			strus::TransactionRef tref = g_tpool->fetchTransaction( tidlist[ ii]);
			if (odd( idx))
			{
				if (!tref.get()) throw std::runtime_error("lost transaction");
			}
			else
			{
				if (tref.get()) throw std::runtime_error("transaction not collected");
				continue;
			}
			papuga_RequestContext* ctx = tref->context();
			const papuga_ValueVariant* var = papuga_RequestContext_get_variable( ctx, "index", NULL/*param[out] isArray*/);
			if (!var) throw std::runtime_error("index variable of context undefined");
			int value = papuga_ValueVariant_toint( var, &errcode);
			if (errcode != papuga_Ok) throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
			if (value != idx) throw std::runtime_error("index variable of context does not match");
			std::string tid = tref->id();
			if (g_verbose) std::cerr << strus::string_format( "check transaction %s [i=%d]\n", tid.c_str(), idx);
			g_tpool->returnTransaction( tref);
		}
	}
	catch (const std::exception& err)
	{
		std::cerr << strus::string_format( "ERROR in thread %d: %s\n", treadidx, err.what());
		g_errorCounter.increment();
	}
}

static void runThreads( int nofThreads, int nofIterations)
{
	std::vector<strus::shared_ptr<strus::thread> > threadGroup;
	for (int ti=0; ti < nofThreads; ++ti)
	{
		if (g_verbose) std::cerr << strus::string_format( "running thread %d ...\n", ti);
		int randomSeed = g_random.get( 0, std::numeric_limits<int>::max());
		strus::shared_ptr<strus::thread> th( new strus::thread( &runThread, ti, nofIterations, randomSeed));
		threadGroup.push_back( th);
	}
	std::vector<strus::shared_ptr<strus::thread> >::iterator gi = threadGroup.begin(), ge = threadGroup.end();
	for (int ti=0; gi != ge; ++gi,++ti)
	{
		(*gi)->join();
		if (g_verbose) std::cerr << strus::string_format( "done thread %d\n", ti);
	}
}

static void runTimerThread()
{
	for (int ii=0; ii<6; ++ii)
	{
		strus::sleep(1);
		g_timecnt.increment();
		if (g_verbose) std::cerr << "timer tick " << (g_timecnt.value() - g_timecnt_start) << std::endl;
		g_tpool->collectGarbage( g_timecnt.value());
	}
}

int main( int argc, const char* argv[])
{
	int argi = 1;
	int nofThreads = -1;
	for (; argi < argc && argv[argi][0] == '-'; ++argi)
	{
		if (0==std::strcmp( argv[argi], "--"))
		{
			++argi;
			break;
		}
		else if (0==std::strcmp( argv[argi], "-h") || 0==std::strcmp( argv[argi], "--help"))
		{
			std::cerr << "Usage: testRequestTransactionMap [-h,-V] <nofIterations>" << std::endl;
			return 0;
		}
		else if (0==std::strcmp( argv[argi], "-t") || 0==std::strcmp( argv[argi], "--threads"))
		{
			const char* optarg_ = argv[++argi];
			nofThreads = optarg_ ? atoi( optarg_) : -1;
			if (nofThreads <= 0)
			{
				std::cerr << "argument of option --threads|-t has to be a positive integer: " << (optarg_?optarg_:"NULL") << std::endl;
				return -1;
			}
		}
		else if (0==std::strcmp( argv[argi], "-V") || 0==std::strcmp( argv[argi], "--verbose"))
		{
			g_verbose = true;
		}
		else
		{
			std::cerr << "unknown option " << argv[argi] << " (only --help|-h or --verbose|-V known)" << std::endl;
			return -1;
		}
	}
	int nofIterations = (argc == argi) ? 100 : atoi( argv[argi++]);
	if (nofThreads <= 0)
	{
		nofThreads = strus::platform::cores();
		if (nofThreads <= 0)
		{
			std::cerr << "failed to determine numer of cores, default to 2" << std::endl;
			nofThreads = 2;
		}
		else
		{
			std::cerr << "found " << nofThreads << " cores" << std::endl;
		}
	}
	else
	{
		std::cerr << "number of threads " << nofThreads << " specified as program option." << std::endl;
	}
	if (nofIterations <= 0 || argi < argc)
	{
		std::cerr << "Usage: testRequestTransactionMap [-h,-V,-t <threads>] <nofIterations>" << std::endl;
		return 1;
	}
	try
	{
		g_timecnt_start = g_random.get( 1, std::numeric_limits<int>::max());
		g_timecnt.set( g_timecnt_start);

		enum {MaxTransactionTimeout=60, MinNofTransactionPerSecond=10};
		g_tpool.reset( new strus::TransactionPool( g_timecnt.value(), MaxTransactionTimeout, nofIterations*2*nofThreads + MinNofTransactionPerSecond, NULL/*logger interface*/));
		strus::thread timerThread( &runTimerThread);

		runThreads( nofThreads, nofIterations);
		timerThread.join();
		if (g_errorCounter.value() == 0)
		{
			std::cerr << "OK" << std::endl;
			return 0;
		}
		else
		{
			std::cerr << "ERR " << g_errorCounter.value() << " errors" << std::endl;
			return -1;
		}
	}
	catch (const std::exception& err)
	{
		std::cerr << "ERROR " << err.what() << std::endl;
		return -1;
	}
}



