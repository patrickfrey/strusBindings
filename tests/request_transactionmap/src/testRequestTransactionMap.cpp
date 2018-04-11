#include "transaction.hpp"
#include "strus/base/pseudoRandom.hpp"
#include "strus/base/string_format.hpp"
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

static bool odd( int num)
{
	return ((num & 1) == 1);
}

int main( int argc, const char* argv[])
{
	bool verbose = false;
	int argi = 1;

	for (; argv[argi][0] == '-'; ++argi)
	{
		if (0==std::strcmp( argv[argi], "--"))
		{
			++argi;
			break;
		}
		else if (0==std::strcmp( argv[argi], "-h") || 0==std::strcmp( argv[argi], "--help"))
		{
			std::cerr << "Usage: testRequestTransactionMap [-h,-v] <nofIterations>" << std::endl;
			return 0;
		}
		else if (0==std::strcmp( argv[argi], "-v") || 0==std::strcmp( argv[argi], "--verbose"))
		{
			verbose = true;
		}
		else
		{
			std::cerr << "unknown option " << argv[argi] << " (only --help|-h or --verbose|-v known)" << std::endl;
			return -1;
		}
	}
	int nofIterations = (argc == argi) ? 100 : atoi( argv[argi]);
	if (nofIterations <= 0)
	{
		std::cerr << "Usage: testRequestTransactionMap [-h,-v] <nofIterations>" << std::endl;
		return 1;
	}
	try
	{
		int64_t timecount = g_random.get( 1, std::numeric_limits<int>::max());
		timecount *= g_random.get( 1, std::numeric_limits<int>::max());
		enum {MaxTransactionTimeout=60, MinNofTransactionPerSecond=10};
		strus::TransactionPool tpool( timecount, MaxTransactionTimeout, nofIterations*2 + MinNofTransactionPerSecond);

		int ii;
		std::map<std::string,int> refmap;
		std::vector<std::string> tidlist;
		papuga_ErrorCode errcode = papuga_Ok;

		for (ii=0; ii<nofIterations; ++ii)
		{
			int idx = ii+1;
			papuga_RequestContext* ctx = papuga_create_RequestContext();
			papuga_ValueVariant val;
			papuga_init_ValueVariant_int( &val, idx);
			if (!papuga_RequestContext_add_variable( ctx, "index", &val)) throw std::bad_alloc();
			int timeout = odd(idx) ? g_random.get( 5, 9) : g_random.get( 1, 4);
			std::string tid = tpool.createTransaction( ctx, timeout);
			if (tid.empty())
			{
				papuga_destroy_RequestContext( ctx);
				throw std::bad_alloc();
			}
			tidlist.push_back( tid);
			refmap[ tid] = idx;
			if (verbose) std::cerr << strus::string_format( "create transaction %s [i=%d,t=%d]", tid.c_str(), idx, timeout) << std::endl;
		}
		for (ii=0; ii<nofIterations; ++ii)
		{
			int idx = ii+1;
			strus::TransactionRef tref = tpool.fetchTransaction( tidlist[ ii]);
			if (!tref.get()) throw std::runtime_error("lost transaction");
			papuga_RequestContext* ctx = tref->context();
			const papuga_ValueVariant* var = papuga_RequestContext_get_variable( ctx, "index");
			if (!var) throw std::runtime_error("index variable of context undefined");
			int value = papuga_ValueVariant_toint( var, &errcode);
			if (errcode != papuga_Ok) throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
			if (value != idx) throw std::runtime_error("index variable of context does not match");
			std::string tid = tref->id();
			if (verbose) std::cerr << strus::string_format( "check transaction %s [i=%d]", tid.c_str(), idx) << std::endl;
			if (!tpool.returnTransaction( tref)) throw std::bad_alloc();
		}
#if DISABLED_COLLECTION_IN_4_STEPS
		if (g_random.get( 0, 2) == 1)
		{
			for (int kk=0; kk<4; ++kk)
			{
				if (verbose) std::cerr << "sleeping 1 second ..." << std::endl;
				timecount += 1;
				if (verbose) std::cerr << "collecting transactions run into timeout ..." << std::endl;
				tpool.collectGarbage();
			}
		}
		else
#endif
		{
			if (verbose) std::cerr << "sleeping 4 seconds ..." << std::endl;
			timecount += 4;
			if (verbose) std::cerr << "collecting transactions run into timeout ..." << std::endl;
			tpool.collectGarbage( timecount);
		}
		for (ii=0; ii<nofIterations; ++ii)
		{
			int idx = ii+1;
			strus::TransactionRef tref = tpool.fetchTransaction( tidlist[ ii]);
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
			const papuga_ValueVariant* var = papuga_RequestContext_get_variable( ctx, "index");
			if (!var) throw std::runtime_error("index variable of context undefined");
			int value = papuga_ValueVariant_toint( var, &errcode);
			if (errcode != papuga_Ok) throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
			if (value != idx) throw std::runtime_error("index variable of context does not match");
			std::string tid = tref->id();
			if (verbose) std::cerr << strus::string_format( "check transaction %s [i=%d]", tid.c_str(), idx) << std::endl;
			if (!tpool.returnTransaction( tref)) throw std::bad_alloc();
		}
		std::cerr << "OK" << std::endl;
		return 0;
	}
	catch (const std::exception& err)
	{
		std::cerr << "ERROR " << err.what() << std::endl;
		return -1;
	}
}



