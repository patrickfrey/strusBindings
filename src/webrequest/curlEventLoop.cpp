/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Periodic timer event
#include "curlEventLoop.hpp"
#include "strus/base/atomic.hpp"
#include "strus/base/bitset.hpp"
#include "strus/base/thread.hpp"
#include "strus/reference.hpp"
#include "private/internationalization.hpp"
#include "private/errorUtils.hpp"
#include <queue>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>

using namespace strus;

typedef strus::Reference<CurlEventLoop::Job> JobRef;

typedef strus::Reference<CurlEventLoop::Ticker> TickerRef;
typedef strus::Reference<CurlEventLoop::Logger> LoggerRef;

struct CurlEventLoop::Data
{
	explicit Data( int secondsPeriod_, Logger* logger_, Ticker* ticker_, int max_total_conn, int max_host_conn)
		:m_multi_handle(0)
		,m_mutex()
		,m_thread(0)
		,m_waitingList()
		,m_activatedMap()
		,m_ticker(ticker_)
		,m_milliSecondsPeriod(secondsPeriod_*1000)
		,m_requestCount(0)
		,m_numberOfRequestsBeforeTick(secondsPeriod_*NumberOfRequestsBeforeTick)
		,m_terminate(false)
		,m_pipcurlfd(0)
		,m_pip_handle(0)
	{
		bool sc = true;
		m_multi_handle = curl_multi_init();
		if (!m_multi_handle) throw std::bad_alloc();

		sc &= (CURLM_OK == curl_multi_setopt( m_multi_handle, CURLMOPT_MAX_TOTAL_CONNECTIONS, max_total_conn));
		sc &= (CURLM_OK == curl_multi_setopt( m_multi_handle, CURLMOPT_MAX_HOST_CONNECTIONS, max_host_conn));
#ifdef CURLPIPE_MULTIPLEX
		// enables http/2 if available
		sc &= (CURLM_OK == curl_multi_setopt( m_multi_handle, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX));
#endif
		if (!sc) throw strus::runtime_error(_TXT("curl handle setting options failed"));

		openSelfSignalingPipe();
		if (!initSelfPipeTrickCurlHandle()) throw strus::runtime_error(_TXT("self pipe initialization failed"));
	}

	~Data()
	{
		::close( m_pipfd[0]);
		::close( m_pipfd[1]);
		curl_easy_cleanup( m_pip_handle);
		std::map<CURL*,JobRef>::iterator ai = m_activatedMap.begin(), ae = m_activatedMap.end();
		for (; ai != ae; ++ai)
		{
			curl_multi_remove_handle( m_multi_handle, ai->first);
		}
		curl_multi_cleanup( m_multi_handle);
	}

	void openSelfSignalingPipe()
	{
		int ec = 0;
		m_pipfd[0] = 0;
		m_pipfd[1] = 0;
		if (::pipe( m_pipfd) == -1) throw strus::runtime_error(_TXT("failed to open pipe: %s"), ::strerror(errno));

		int flags = fcntl( m_pipfd[0], F_GETFL);
		if (flags == -1) goto ERROR;
		flags |= O_NONBLOCK;
		if (fcntl( m_pipfd[0], F_SETFL, flags) == -1) goto ERROR;
		flags = fcntl( m_pipfd[1], F_GETFL);
		if (flags == -1) goto ERROR;
		flags |= O_NONBLOCK;
		if (fcntl( m_pipfd[1], F_SETFL, flags) == -1) goto ERROR;
		m_pipcurlfd = m_pipfd[0];
		return;
	ERROR:
		ec = errno;
		::close( m_pipfd[0]);
		::close( m_pipfd[1]);
		throw strus::runtime_error(_TXT("failed to open pipe: %s"), ::strerror(ec));
	}

	static curl_socket_t opensocket( void *clientp, curlsocktype purpose, struct curl_sockaddr *address)
	{
		curl_socket_t sockfd;
		sockfd = *(curl_socket_t *)clientp;
		/* the actual externally set socket is passed in via the OPENSOCKETDATA option */
		return sockfd;
	}

	static int sockopt_callback( void *clientp, curl_socket_t curlfd, curlsocktype purpose)
	{
		return CURL_SOCKOPT_ALREADY_CONNECTED;
	}

	bool initSelfPipeTrickCurlHandle()
	{
		bool rt = true;
		m_pip_handle = curl_easy_init();
		if (m_pip_handle)
		{
			rt &= (CURLE_OK == curl_easy_setopt( m_pip_handle, CURLOPT_OPENSOCKETFUNCTION, opensocket));
			rt &= (CURLE_OK == curl_easy_setopt( m_pip_handle, CURLOPT_OPENSOCKETDATA, &m_pipcurlfd));
			rt &= (CURLE_OK == curl_easy_setopt( m_pip_handle, CURLOPT_SOCKOPTFUNCTION, sockopt_callback));
			if (!rt) goto ERROR;
			rt &= (CURLM_OK == curl_multi_add_handle( m_multi_handle, m_pip_handle));
			if (!rt) goto ERROR;
			return true;
		}
	ERROR:
		curl_easy_cleanup( m_pip_handle);
		m_pip_handle = 0;
		return false;
	}

	void notify()
	{
		::write( m_pipfd[1], "x", 1);
	}

	void consumeSelfPipeWrites()
	{
		int readCnt = 0;
		for (;;++readCnt)
		{
			char ch;
			if (::read( m_pipfd[0], &ch, 1) == -1) {
				int ec = errno;
				if (ec == EAGAIN) break;
				if (ec == EINTR) {--readCnt; continue;}
				throw strus::runtime_error(_TXT("failed to read from pipe: %s"), ::strerror(ec));
			}
		}
	}

	void handleException( const char* msg)
	{
		m_logger->print( CurlEventLoop::Logger::LogFatal, _TXT("connection exception: %s"), msg);
		notify();
	}

	bool pushJob( const JobRef& job)
	{
		try
		{
			strus::unique_lock lock( m_mutex);
			m_waitingList.push_back( job);
			notify();
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void tick()
	{
		m_ticker->tick();
	}

	void activateIdleJobs()
	{
		strus::unique_lock lock( m_mutex);
		std::vector<JobRef>::iterator wi = m_waitingList.begin(), we = m_waitingList.end();
		for (; wi != we; ++wi)
		{
			CURLMcode ec = curl_multi_add_handle( m_multi_handle, (*wi)->handle());
			if (ec != CURLM_OK)
			{
				m_waitingList.erase( m_waitingList.begin(), wi);
				throw strus::runtime_error(_TXT("failed to add handle: %s"), curl_multi_strerror( ec));
			}
			m_activatedMap[ (*wi)->handle()] = *wi;
		}
		m_waitingList.clear();
	}

	JobRef fetchJob( CURL* handle)
	{
		strus::unique_lock lock( m_mutex);
		std::map<CURL*,JobRef>::iterator ai = m_activatedMap.find( handle);
		if (ai == m_activatedMap.end()) return JobRef();
		JobRef rt = ai->second;
		m_activatedMap.erase( ai);
		return rt;
	}

	bool forcedTick()
	{
		if (++m_requestCount == m_numberOfRequestsBeforeTick)
		{
			m_requestCount = 0;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool terminated() const
	{
		return m_terminate.test();
	}


	bool start( strus::thread* thread_)
	{
		if (m_thread) return false;
		m_terminate.set( false);
		m_thread = thread_;
		return true;
	}

	bool stopped()
	{
		return m_thread == NULL;
	}

	void stop()
	{
		if (m_thread)
		{
			m_terminate.set( true);
			notify();
			m_thread->join();
			delete m_thread;
			m_thread = 0;
		}
	}

	void LOG( const char* context, CURLMcode ec)
	{
		if (ec != CURLM_OK)
		{
			if (CurlEventLoop::Logger::LogError <= m_logger->loglevel())
			{
				m_logger->print( CurlEventLoop::Logger::LogError, _TXT("error in %s: %s"), context, curl_multi_strerror(ec));
			}
		}
	}

	void run()
	{
		int still_running = 1;
		while (still_running && !m_terminate.test())
		{
			try
			{
				int tickEvent = 0;
				int numfds = 0;
				activateIdleJobs();
				LOG( _TXT("wait event"), curl_multi_wait( m_multi_handle, NULL, 0, m_milliSecondsPeriod, &numfds));
				if (numfds == 0)
				{
					m_requestCount = 0;
					tick();
					continue;
				}
				else if (forcedTick())
				{
					tick();
				}
				if (m_terminate.test()) break;
				LOG( _TXT("perform event"), curl_multi_perform( m_multi_handle, &still_running));
	
				CURLMsg* msg = NULL;
				int msgs_left = 1;
				while (msgs_left && NULL != (msg = curl_multi_info_read( m_multi_handle, &msgs_left)))
				{
					if (msg->easy_handle == m_pip_handle)
					{
						consumeSelfPipeWrites();
						++tickEvent;
						tick();
					}
					else if (msg->msg == CURLMSG_DONE)
					{
						JobRef job = fetchJob( msg->easy_handle);
						if (!job.get())
						{
							if (CurlEventLoop::Logger::LogError <= m_logger->loglevel())
							{
								m_logger->print( CurlEventLoop::Logger::LogError, _TXT("logic error: lost handle"));
							}
						}
						else
						{
							curl_multi_remove_handle( m_multi_handle, msg->easy_handle);
							job->resume( msg->data.result);
						}
					}
				}
			}
			catch (const std::runtime_error& err)
			{
				if (CurlEventLoop::Logger::LogFatal <= m_logger->loglevel())
				{
					m_logger->print( CurlEventLoop::Logger::LogFatal, _TXT("runtime error in CURL event loop: %s"), err.what());
				}
			}
			catch (const std::bad_alloc& err)
			{
				if (CurlEventLoop::Logger::LogFatal <= m_logger->loglevel())
				{
					m_logger->print( CurlEventLoop::Logger::LogFatal, _TXT("out of memory in CURL event loop"));
				}
			}
		}
	}

private:
	CURLM* m_multi_handle;
	strus::mutex m_mutex;
	strus::thread* m_thread;
	std::vector<JobRef> m_waitingList;
	std::map<CURL*,JobRef> m_activatedMap;
	TickerRef m_ticker;
	LoggerRef m_logger;
	int m_milliSecondsPeriod;
	int m_requestCount;
	int m_numberOfRequestsBeforeTick;
	AtomicFlag m_terminate;
	int m_pipfd[2];				//< pipe to listen for queue events in select: "self pipe trick"
	curl_socket_t m_pipcurlfd;		//< copy of read end of m_pipfd
	CURL* m_pip_handle;			//< CURL handle corresponding to pipe
};


CurlEventLoop::CurlEventLoop( int secondsPeriod_, Logger* logger_, Ticker* ticker_, int max_total_conn, int max_host_conn)
{
	m_data = new Data( secondsPeriod_, logger_, ticker_, max_total_conn, max_host_conn);
}

CurlEventLoop::~CurlEventLoop()
{
	stop();
	delete m_data;
}

void CurlEventLoop::run()
{
	m_data->run();
}

bool CurlEventLoop::start()
{
	try
	{
		if (!m_data->stopped()) return false;
		return m_data->start( new strus::thread( &CurlEventLoop::run, this));
	}
	catch (...)
	{
		return false;
	}
}

void CurlEventLoop::stop()
{
	m_data->stop();
}

bool CurlEventLoop::pushJob( Job* job)
{
	return m_data->pushJob( job);
}

void CurlEventLoop::handleException( const char* msg)
{
	m_data->handleException( msg);
}


