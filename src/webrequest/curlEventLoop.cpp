/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Periodic timer event
#include "curlEventLoop.hpp"
#include "curlMessage.hpp"
#include "curlLogger.hpp"
#include "webRequestUtils.hpp"
#include "strus/lib/error.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/webRequestAnswer.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/base/shared_ptr.hpp"
#include "strus/base/thread.hpp"
#include "strus/base/atomic.hpp"
#include "strus/base/bitset.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/filehandle.hpp"
#include "strus/errorCodes.hpp"
#include "strus/reference.hpp"
#include "private/internationalization.hpp"
#include <stdexcept>
#include <new>
#include <queue>
#include <curl/curl.h>
#include <ctime>
#include <map>
#include <unistd.h>
#include <fcntl.h>

using namespace strus;

class WebRequestDelegateJob
{
public:
	WebRequestDelegateJob( const std::string& address_, const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_, CurlLogger* logger_, CurlEventLoop::Data* eventloopdata_)
		:m_message(address_,method_,content_,logger_),m_receiver(receiver_)
		,m_logger(logger_),m_eventloopdata(eventloopdata_){}

	void resume( CURLcode ec);
	void dropRequest( ErrorCode errcode, const char* errmsg);

	CURL* handle() const	{return m_message.handle();}
	void flushLogs()	{m_message.flushLogs();}

private:
	WebRequestDelegateJob( const WebRequestDelegateJob&){}	//... non copyable
	void operator=( const WebRequestDelegateJob&){}		//... non copyable

private:
	CurlMessage m_message;
	strus::shared_ptr<WebRequestDelegateContextInterface> m_receiver;
	CurlLogger* m_logger;
	CurlEventLoop::Data* m_eventloopdata;
};


void WebRequestDelegateJob::dropRequest( ErrorCode errcode, const char* errmsg)
{
	int httpStatus = errorCodeToHttpStatus( errcode);
	if (!errmsg) errmsg = strus::errorCodeToString(errcode);
	WebRequestAnswer answer( httpStatus, errcode, errmsg);
	m_receiver->putAnswer( answer);
	if (CurlLogger::LogError <= m_logger->loglevel())
	{
		if (errmsg)
		{
			m_logger->print( CurlLogger::LogError, _TXT("connection exception: %s, %s"), strus::errorCodeToString(errcode), errmsg);
		}
		else
		{
			m_logger->print( CurlLogger::LogError, _TXT("connection exception: %s"), strus::errorCodeToString(errcode));
		}
	}
}

void WebRequestDelegateJob::resume( CURLcode ec)
{
	try
	{
		if (CurlLogger::LogInfo <= m_logger->loglevel())
		{
			flushLogs();
			m_logger->logState( "resume", ec);
		}
		CURL* curl = handle();
		if (ec == CURLE_OK)
		{
			long http_code = 0;
			curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code);
			WebRequestContent answerContent( "UTF-8", "application/json", m_message.response_content().c_str(), m_message.response_content().size());
			WebRequestAnswer answer( http_code, answerContent);
			m_receiver->putAnswer( answer);
		}
		else
		{
			long http_code = 500;
			curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code);
			WebRequestAnswer answer( http_code, ErrorCodeDelegateRequestFailed, m_message.response_error());
			m_receiver->putAnswer( answer);
		}
	}
	catch (const std::runtime_error& err)
	{
		dropRequest( ErrorCodeRuntimeError, err.what());
	}
	catch (const std::bad_alloc& )
	{
		dropRequest( ErrorCodeOutOfMem, 0);
	}
	catch (...)
	{
		dropRequest( ErrorCodeDelegateRequestFailed, _TXT("unexpected exception"));
	}
}

typedef strus::Reference<WebRequestDelegateJob> WebRequestDelegateJobRef;


struct CurlEventLoop::Data
{
	explicit Data( WebRequestLoggerInterface* logger_, int secondsPeriod_, int max_total_conn, int max_host_conn)
		:m_multi_handle(0)
		,m_thread(0)
		,m_waitingList(),m_waitingList_mutex()
		,m_activatedMap()
		,m_tickerar()
		,m_logger(logger_)
		,m_logState(LogStateInit)
		,m_milliSecondsPeriod(secondsPeriod_*1000)
		,m_requestCount(0)
		,m_numberOfRequestsBeforeTick(secondsPeriod_*NumberOfRequestsBeforeTick)
		,m_terminate(false)
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

		// open self signaling pipe ("self pipe trick"):
		int ec = 0;
		m_pipfd[0] = 0;
		m_pipfd[1] = 0;
		if (::pipe( m_pipfd) == -1) throw strus::runtime_error(_TXT("failed to open pipe: %s"), ::strerror(errno));

		int flags = ::fcntl( m_pipfd[0], F_GETFL);
		if (flags == -1) goto ERROR;
		flags |= O_NONBLOCK;
		if (::fcntl( m_pipfd[0], F_SETFL, flags) == -1) goto ERROR;
		flags = ::fcntl( m_pipfd[1], F_GETFL);
		if (flags == -1) goto ERROR;
		flags |= O_NONBLOCK;
		if (::fcntl( m_pipfd[1], F_SETFL, flags) == -1) goto ERROR;

		// PF:NOTE For the following 3 lines thanks to the author of 'http://eao197.blogspot.com/2018/03/progc-async-processing-of-incoming-and.html':
		m_pipcurl_notify_fd.fd = m_pipfd[0]; //... read end of the pipe
		m_pipcurl_notify_fd.events = CURL_WAIT_POLLIN;
		m_pipcurl_notify_fd.revents = 0;     //... here the result is signalled
		return;
	ERROR:
		ec = errno;
		::close( m_pipfd[0]);
		::close( m_pipfd[1]);
		throw strus::runtime_error(_TXT("failed to open pipe: %s"), ::strerror(ec));
	}

	~Data()
	{
		::close( m_pipfd[0]);
		::close( m_pipfd[1]);
		std::map<CURL*,WebRequestDelegateJobRef>::iterator ai = m_activatedMap.begin(), ae = m_activatedMap.end();
		for (; ai != ae; ++ai)
		{
			curl_multi_remove_handle( m_multi_handle, ai->first);
		}
		curl_multi_cleanup( m_multi_handle);
	}

	bool notify()
	{
		int ww;
	AGAIN:
		ww = ::write( m_pipfd[1], "x", 1);
		if (ww == 0)
		{
			int ec = errno;
			if (ec == EAGAIN || ec == EINTR) goto AGAIN;
			m_logger.print( CurlLogger::LogFatal, _TXT("internal: delegate request event loop signalling failed: %s"), ::strerror(ec));
		}
		return true;
	}

	void consumeSelfPipeWrites()
	{
		for (;;)
		{
			char ch[32];
			ssize_t nn = ::read( m_pipfd[0], ch, sizeof(ch));
			if (nn == -1) {
				int ec = errno;
				if (ec == EAGAIN) break;
				if (ec == EINTR) continue;
				throw strus::runtime_error(_TXT("failed to read from pipe: %s"), ::strerror(ec));
			}
			else if (nn == 0)
			{
				break;
			}
		}
	}

	bool send(
		char const* address,
		char const* method,
		char const* contentstr,
		size_t contentlen,
		WebRequestDelegateContextInterface* receiver)
	{
		try
		{
			strus::shared_ptr<WebRequestDelegateContextInterface> receiverRef( receiver);
			if (!m_thread) throw std::runtime_error( _TXT("send failed because eventloop thread not started yet"));
			WebRequestDelegateJobRef job( new WebRequestDelegateJob( address, method, std::string( contentstr, contentlen), receiverRef, &m_logger, this));
			{
				strus::unique_lock lock( m_waitingList_mutex);
				m_waitingList.push_back( job);
			}
			notify();
			return true;
		}
		catch (const std::runtime_error& err)
		{
			WebRequestAnswer answer( 500, ErrorCodeDelegateRequestFailed, err.what());
			receiver->putAnswer( answer);
			m_logger.print( CurlLogger::LogFatal, err.what());
			delete receiver;
			return false;
		}
		catch (const std::bad_alloc& )
		{
			WebRequestAnswer answer( 500, ErrorCodeOutOfMem, _TXT("memory allocation error"));
			receiver->putAnswer( answer);
			m_logger.print( CurlLogger::LogFatal, _TXT("memory allocation error"));
			delete receiver;
			return false;
		}
		catch (...)
		{
			WebRequestAnswer answer( 500, ErrorCodeDelegateRequestFailed, _TXT("unexpected exception"));
			receiver->putAnswer( answer);
			m_logger.print( CurlLogger::LogFatal, _TXT("unexpected exception"));
			delete receiver;
			return false;
		}
	}

	typedef std::pair<WebRequestEventLoopInterface::TickerFunction,void*> TickerFunctionDef;

	bool addTickerEvent( void* obj, WebRequestEventLoopInterface::TickerFunction func)
	{
		try
		{
			strus::unique_lock lock( m_tickerar_mutex);
			m_tickerar.push_back( TickerFunctionDef( func, obj));
			return true;
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}
	}
	
	void tick()
	{
		strus::unique_lock lock( m_tickerar_mutex);
		std::vector<TickerFunctionDef>::const_iterator ti = m_tickerar.begin(), te = m_tickerar.end();
		for (; ti != te; ++ti)
		{
			ti->first( ti->second);
		}
		m_requestCount = 0;
	}

	void checkTick()
	{
		if (++m_requestCount == m_numberOfRequestsBeforeTick)
		{
			m_requestCount = 0;
			tick();
		}
	}

	void activateIdleJobs()
	{
		int nofRequests = 0;
		strus::unique_lock lock( m_waitingList_mutex);
		std::vector<WebRequestDelegateJobRef>::iterator wi = m_waitingList.begin(), we = m_waitingList.end();
		for (; wi != we; ++wi)
		{
			CURLMcode ec = curl_multi_add_handle( m_multi_handle, (*wi)->handle());
			if (ec != CURLM_OK)
			{
				(*wi)->dropRequest( ErrorCodeDelegateRequestFailed, curl_multi_strerror( ec));
			}
			else
			{
				m_activatedMap[ (*wi)->handle()] = *wi;
				++nofRequests;
			}
		}
		if (CurlLogger::LogInfo <= m_logger.loglevel() && nofRequests > 0)
		{
			m_logger.logState( "new requests", nofRequests);
		}
		m_waitingList.clear();
	}

	WebRequestDelegateJobRef fetchJob( CURL* handle)
	{
		std::map<CURL*,WebRequestDelegateJobRef>::iterator ai = m_activatedMap.find( handle);
		if (ai == m_activatedMap.end()) return WebRequestDelegateJobRef();
		WebRequestDelegateJobRef rt = ai->second;
		m_activatedMap.erase( ai);
		return rt;
	}

	void terminatePendingRequests()
	{
		std::map<CURL*,WebRequestDelegateJobRef>::iterator ai = m_activatedMap.begin(), ae = m_activatedMap.end();
		for (; ai != ae; ++ai)
		{
			ai->second->dropRequest( ErrorCodeServiceShutdown, 0);
		}
		m_activatedMap.clear();
	}

	void flushLogsOfPendingRequests()
	{
		std::map<CURL*,WebRequestDelegateJobRef>::iterator ai = m_activatedMap.begin(), ae = m_activatedMap.end();
		for (; ai != ae; ++ai)
		{
			ai->second->flushLogs();
		}
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
			if (CurlLogger::LogError <= m_logger.loglevel())
			{
				m_logger.print( CurlLogger::LogError, _TXT("error in %s: %s"), context, curl_multi_strerror(ec));
			}
		}
	}

	void processEvents()
	{
		// Handle message results:
		CURLMsg* msg = NULL;
		int msgLeft = 1;
		int msgCount = 0;
		int runningHandles = 0;

		do
		{
			msgCount = 0;
			LOG( _TXT("complete event"), curl_multi_perform( m_multi_handle, &runningHandles));
			if (CurlLogger::LogInfo <= m_logger.loglevel() && m_logState != LogStateConnections && runningHandles)
			{
				m_logger.logState( "connections", runningHandles);
				m_logState = LogStateConnections;
			}
			while (msgLeft && NULL != (msg = curl_multi_info_read( m_multi_handle, &msgLeft)))
			{
				++msgCount;
				if (CurlLogger::LogInfo <= m_logger.loglevel())
				{
					m_logger.logState( "connection event", 0/*arg*/);
					m_logState = LogStateConnEvent;
				}
				if (msg->msg == CURLMSG_DONE)
				{
					WebRequestDelegateJobRef job = fetchJob( msg->easy_handle);
					if (!job.get())
					{
						if (CurlLogger::LogError <= m_logger.loglevel())
						{
							m_logger.print( CurlLogger::LogError, _TXT("logic error: lost handle"));
						}
					}
					else
					{
						curl_multi_remove_handle( m_multi_handle, msg->easy_handle);
						job->resume( msg->data.result);
					}
				}
				else
				{
					m_logger.print( CurlLogger::LogError, _TXT("unknown curl message type"));
				}
			}
			// Log the status of the connections if required:
			if (CurlLogger::LogInfo <= m_logger.loglevel())
			{
				flushLogsOfPendingRequests();
			}
		}
		while (msgCount);
	}

	bool listen()
	{
		int numfds = 0;

		// Blocking listen for events (results available or new jobs in the queue) or a timeout:
		if (CurlLogger::LogInfo <= m_logger.loglevel() && !m_activatedMap.empty() && (m_logState != LogStateListen && m_logState != LogStateConnections))
		{
			m_logger.logState( "listen", 0);
			m_logState = LogStateListen;
		}
		LOG( _TXT("wait event"), curl_multi_wait( m_multi_handle, &m_pipcurl_notify_fd, 1, m_milliSecondsPeriod, &numfds));
		if (m_pipcurl_notify_fd.revents)
		{
			//... interrupted by new request in the queue or a terminate is signaled
			m_pipcurl_notify_fd.revents = 0;
			consumeSelfPipeWrites();
			if (CurlLogger::LogInfo <= m_logger.loglevel())
			{
				m_logger.logState( "queue event", 0/*arg*/);
				m_logState = LogStateQueueEvent;
			}
			return true;
		}
		else if (numfds == 0)
		{
			//... interrupted by timeout
			return false;
		}
		else
		{
			// ... interrupted by connection event
			return true;
		}
	}

	void run()
	{
		m_logState = LogStateInit;

		while (!m_terminate.test())
		{
			try
			{
				// Get new messages jobs from the queue into the listener context:
				activateIdleJobs();
				processEvents();

				if (listen())
				{
					// ... have events, do a tick after a defined amount of subsequent interruptions by an event and not by a timeout
					checkTick();
				}
				else
				{
					//... interrupted by timeout
					tick();
				}
				if (m_terminate.test()) break;
			}
			catch (const std::runtime_error& err)
			{
				if (CurlLogger::LogFatal <= m_logger.loglevel())
				{
					m_logger.print( CurlLogger::LogFatal, _TXT("runtime error in CURL event loop: %s"), err.what());
				}
			}
			catch (const std::bad_alloc& err)
			{
				if (CurlLogger::LogFatal <= m_logger.loglevel())
				{
					m_logger.print( CurlLogger::LogFatal, _TXT("out of memory in CURL event loop"));
				}
			}
		}
		terminatePendingRequests();
	}

private:
	enum LogState {LogStateInit,LogStateListen,LogStateQueueEvent,LogStateConnEvent,LogStateConnections};

private:
	CURLM* m_multi_handle;					//< handle to listen for multiple connections simultaneously
	strus::thread* m_thread;				//< background thread of the eventloop 
	std::vector<WebRequestDelegateJobRef> m_waitingList;	//< list of jobs in the queue to process
	strus::mutex m_waitingList_mutex;			//< mutex for job queue monitor
	std::map<CURL*,WebRequestDelegateJobRef> m_activatedMap;//< map of jobs bound to a cURL handle
	std::vector<TickerFunctionDef> m_tickerar;		//< tickers to call after timeout or a number of requests
	strus::mutex m_tickerar_mutex;				//< mutex for tickers monitor
	CurlLogger m_logger;					//< logger for logging
	LogState m_logState;					//< state for logging, suppressing repetitive log messages
	int m_milliSecondsPeriod;				//< milliseconds to wait for a request until a timeout is signalled
	int m_requestCount;					//< counter of subsequent requests signalled without timeout
	int m_numberOfRequestsBeforeTick;			//< number of times a wait on a request or a timeout is interrupted by a request before a tick is inserted that normally occurrs on a timeout
	AtomicFlag m_terminate;					//< flag that determines the termination of the eventloop
	int m_pipfd[2];						//< pipe to listen for queue events in select: "self pipe trick"
	curl_waitfd m_pipcurl_notify_fd;			//< curl additional handle to wait for listening on the read part of the "self pipe trick" pipe
};




CurlEventLoop::CurlEventLoop( WebRequestLoggerInterface* logger_, int secondsPeriod_, int max_total_conn, int max_host_conn)
{
	m_data = new Data( logger_, secondsPeriod_, max_total_conn, max_host_conn);
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

long CurlEventLoop::time() const
{
	return ::time(NULL);
}

bool CurlEventLoop::send(
		char const* address,
		char const* method,
		char const* contentstr,
		size_t contentlen,
		WebRequestDelegateContextInterface* receiver)
{
	return m_data->send( address, method, contentstr, contentlen, receiver);
}

bool CurlEventLoop::addTickerEvent( void* obj, TickerFunction func)
{
	return m_data->addTickerEvent( obj, func);
}



