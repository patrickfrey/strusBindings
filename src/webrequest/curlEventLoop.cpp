/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Periodic timer event
#include "curlEventLoop.hpp"
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
#include "strus/errorCodes.hpp"
#include "strus/reference.hpp"
#include "private/internationalization.hpp"
#include "private/errorUtils.hpp"
#include <stdexcept>
#include <new>
#include <queue>
#include <curl/curl.h>
#include <ctime>
#include <map>
#include <unistd.h>
#include <fcntl.h>

using namespace strus;

template <typename DATA>
static void set_curl_opt( CURL *curl, CURLoption opt, const DATA& data)
{
	CURLcode res = curl_easy_setopt( curl, opt, data);
	if (res != CURLE_OK)
	{
		throw strus::runtime_error( "failed set socket option: %s", curl_easy_strerror(res));
	}
}

static bool set_http_header( struct curl_slist*& headers, const char* name, const std::string& value)
{
	char buf[1024];
	if ((int)sizeof(buf) < std::snprintf( buf, sizeof(buf), "%s: %s", name, value.c_str())) return false;
	struct curl_slist* new_headers = curl_slist_append( headers, buf);
	if (!new_headers) return false;
	headers = new_headers;
	return true;
}

static size_t std_string_append_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t nn = size*nmemb;
	std::string* output = (std::string*)userdata;
	try
	{
		output->append( (char*)ptr, nn);
	}
	catch (...)
	{
		return 0;
	}
	return nn;
}

class Logger {
public:
	enum LogType {LogFatal,LogError,LogWarning};

	explicit Logger( WebRequestLoggerInterface* logger_)
		:m_logger(logger_),m_loglevel(evalLogLevel( logger_)){}
	~Logger(){}

	void print( LogType logtype, const char* fmt, ...)
	{
		char msgbuf[ 1024];
		va_list ap;
		va_start( ap, fmt);
		if ((int)sizeof(msgbuf) <= std::vsnprintf( msgbuf, sizeof(msgbuf), fmt, ap)) msgbuf[ sizeof(msgbuf)-1] = 0;
		va_end( ap);

		switch (logtype)
		{
			case LogFatal:
				m_logger->logError( msgbuf);
				break;
			case LogError:
				m_logger->logError( msgbuf);
				break;
			case LogWarning:
				m_logger->logWarning( msgbuf);
				break;
		}
	}

	LogType loglevel() const	{return m_loglevel;}

private:
	static LogType evalLogLevel( WebRequestLoggerInterface* logger_)
	{
		if ((logger_->logMask() & WebRequestLoggerInterface::LogWarning) != 0)
		{
			return LogWarning;
		}
		else if ((logger_->logMask() & WebRequestLoggerInterface::LogError) != 0)
		{
			return LogError;
		}
		else
		{
			return LogFatal;
		}
	}
private:
	WebRequestLoggerInterface* m_logger;
	LogType m_loglevel;
};


struct WebRequestDelegateData
{
	std::string method;
	std::string content;
	strus::shared_ptr<WebRequestDelegateContextInterface> receiver;

	WebRequestDelegateData( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_)
		:method(method_),content(content_),receiver(receiver_){}
	WebRequestDelegateData( const WebRequestDelegateData& o)
		:method(o.method),content(o.content),receiver(o.receiver){}
};

typedef strus::shared_ptr<WebRequestDelegateData> WebRequestDelegateDataRef;

class WebRequestDelegateConnection
{
public:
	WebRequestDelegateConnection( const std::string& address, CurlEventLoop::Data* eventloopdata_);
	~WebRequestDelegateConnection();

	void push( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_);
	WebRequestDelegateDataRef fetchNewIfNotProcessing();

	void reconnect();
	void done();
	void dropAllPendingRequests( const strus::ErrorCode& errcode, const char* errmsg);

	CURL* handle() const {return m_curl;}

private:
	enum State {Init,Connected,Process};

private:
	CURL* m_curl;
	std::string m_url;
	int m_port;
	State m_state;
	strus::mutex m_mutex;
	std::queue<WebRequestDelegateDataRef> m_requestQueue;
	CurlEventLoop::Data* m_eventloopdata;

private:
	void connect();
};

typedef strus::shared_ptr<WebRequestDelegateConnection> WebRequestDelegateConnectionRef;
typedef strus::shared_ptr<WebRequestDelegateContextInterface> WebRequestDelegateContextRef;


class WebRequestDelegateJob
{
public:
	WebRequestDelegateJob( const WebRequestDelegateConnectionRef& conn_, const WebRequestDelegateDataRef& data_, CurlEventLoop::Data* eventloopdata_);

	~WebRequestDelegateJob(){}
	void resume( CURLcode ec);
	void dropRequest( const char* errmsg);
	void feedNextJobForThisConnection();

public:
	CURL* handle() const {return m_handle;}

private:
	CURL* m_handle;
	WebRequestDelegateConnectionRef m_conn;
	WebRequestDelegateDataRef m_data;
	CurlEventLoop::Data* m_eventloopdata;

	std::string m_response_content;
	char m_response_errbuf[ CURL_ERROR_SIZE];
};

struct WebRequestDelegateConnectionGlobals
{
	std::string user_agent;
	struct curl_slist* headers;
	bool valid;

	WebRequestDelegateConnectionGlobals()
		:user_agent(strus::string_format( "libcurl/%s",curl_version_info(CURLVERSION_NOW)->version)),headers(0)
	{
		curl_global_init( CURL_GLOBAL_ALL);
		valid = true;

		valid &= set_http_header( headers, "Expect", "");
		valid &= set_http_header( headers, "Content-Type", "application/json; charset=utf-8");
		valid &= set_http_header( headers, "Accept", "application/json");
		valid &= set_http_header( headers, "Accept-Charset", "UTF-8");
	}
};
static WebRequestDelegateConnectionGlobals g_delegateRequestGlobals;

static void parseAddress( const std::string& address, std::string& url, int& port)
{
	url.clear();

	char const* pi = address.c_str() + address.size();
	while (pi > address.c_str() && *(pi-1) >= '0' && *(pi-1) <= '9') {--pi;}
	if (pi > address.c_str() && *(pi-1) == ':')
	{
		port = atoi( pi);
		--pi;
		url.append( address.c_str(), pi - address.c_str());
	}
	else
	{
		url = address;
	}
}

void WebRequestDelegateConnection::connect()
{
	if (!g_delegateRequestGlobals.valid) throw std::runtime_error(_TXT("fatal: delegate connection globals initialization failed"));

	if (m_curl) curl_easy_cleanup( m_curl);
	m_curl = curl_easy_init();
	if (m_curl == NULL) throw std::bad_alloc();

	set_curl_opt( m_curl, CURLOPT_USERAGENT, g_delegateRequestGlobals.user_agent.c_str());
	set_curl_opt( m_curl, CURLOPT_POST, 1);
	set_curl_opt( m_curl, CURLOPT_HTTPHEADER, g_delegateRequestGlobals.headers);
	set_curl_opt( m_curl, CURLOPT_FAILONERROR, 0);
	set_curl_opt( m_curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_1_1);
	set_curl_opt( m_curl, CURLOPT_URL, m_url.c_str());
	set_curl_opt( m_curl, CURLOPT_PORT, m_port);
	set_curl_opt( m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
	set_curl_opt( m_curl, CURLOPT_TCP_KEEPIDLE, 120L);
	set_curl_opt( m_curl, CURLOPT_TCP_KEEPINTVL, 60L);
	set_curl_opt( m_curl, CURLOPT_CONNECT_ONLY, 1L);
	CURLcode res = curl_easy_perform( m_curl);
	if (res != CURLE_OK)
	{
		throw strus::runtime_error( _TXT("failed to connect to '%s:%d': %s"), m_url.c_str(), m_port, curl_easy_strerror(res));
	}
	m_state = Connected;
}

WebRequestDelegateConnection::WebRequestDelegateConnection( const std::string& address, CurlEventLoop::Data* eventloopdata_)
	:m_curl(NULL)
	,m_url()
	,m_port(80)
	,m_state(Init)
	,m_mutex()
	,m_requestQueue()
	,m_eventloopdata(eventloopdata_)
{
	parseAddress( address, m_url, m_port);
	connect();
}

void WebRequestDelegateConnection::push( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_)
{
	strus::unique_lock lock( m_mutex);
	if (m_state == Init)
	{
		connect();
	}
	m_requestQueue.push( WebRequestDelegateDataRef( new WebRequestDelegateData( method_, content_, receiver_)));
}

WebRequestDelegateDataRef WebRequestDelegateConnection::fetchNewIfNotProcessing()
{
	strus::unique_lock lock( m_mutex);
	if (m_state == Connected && !m_requestQueue.empty())
	{
		WebRequestDelegateDataRef rt = m_requestQueue.front();
		m_requestQueue.pop();
		m_state = Process;
		return rt;
	}
	else
	{
		return WebRequestDelegateDataRef();
	}
}

void WebRequestDelegateConnection::done()
{
	strus::unique_lock lock( m_mutex);
	if (m_state == Process)
	{
		m_state = Connected;
	}
	else if (m_state == Init)
	{
		connect();
	}
}

void WebRequestDelegateConnection::dropAllPendingRequests( const strus::ErrorCode& errcode, const char* errmsg)
{
	strus::unique_lock lock( m_mutex);
	for (; !m_requestQueue.empty(); m_requestQueue.pop())
	{
		WebRequestDelegateDataRef data = m_requestQueue.front();
		WebRequestAnswer answer( errmsg, strus::errorCodeToHttpStatus(errcode), errcode);
		data->receiver->putAnswer( answer);
	}
	m_state = Init;
}

void WebRequestDelegateConnection::reconnect()
{
	try
	{
		strus::unique_lock lock( m_mutex);
		connect();
	}
	catch (const std::runtime_error& err)
	{
		dropAllPendingRequests( ErrorCodeRuntimeError, err.what());
	}
	catch (const std::bad_alloc&)
	{
		dropAllPendingRequests( ErrorCodeOutOfMem, strus::errorCodeToString( ErrorCodeOutOfMem));
	}
	catch (...)
	{
		dropAllPendingRequests( ErrorCodeUncaughtException, strus::errorCodeToString( ErrorCodeUncaughtException));
	}
}

WebRequestDelegateConnection::~WebRequestDelegateConnection()
{
	if (m_curl) curl_easy_cleanup( m_curl);
}

typedef strus::Reference<WebRequestDelegateJob> WebRequestDelegateJobRef;
typedef std::map<std::string,WebRequestDelegateConnectionRef> ConnectionMap;

struct CurlEventLoop::Data
{
	explicit Data( WebRequestLoggerInterface* logger_, int secondsPeriod_, int max_total_conn, int max_host_conn)
		:m_multi_handle(0)
		,m_thread(0)
		,m_waitingList(),m_waitingList_mutex()
		,m_activatedMap()
		,m_tickerar()
		,m_logger(logger_)
		,m_connectionMap(),m_connectionMap_mutex()
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

	
	WebRequestDelegateConnectionRef getConnection( const std::string& address)
	{
		strus::unique_lock lock( m_connectionMap_mutex);
		ConnectionMap::const_iterator ci = m_connectionMap.find( address);
		return ci == m_connectionMap.end() ? WebRequestDelegateConnectionRef() : ci->second;
	}

	WebRequestDelegateConnectionRef getOrCreateConnection( const std::string& address)
	{
		strus::unique_lock lock( m_connectionMap_mutex);
		ConnectionMap::const_iterator ci = m_connectionMap.find( address);
		if (ci == m_connectionMap.end())
		{
			WebRequestDelegateConnectionRef rt( new WebRequestDelegateConnection( address, this));
			m_connectionMap[ address] = rt;
			return rt;
		}
		else
		{
			return ci->second;
		}
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

	void notify()
	{
		::write( m_pipfd[1], "x", 1);
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

	void handleException( const char* msg)
	{
		m_logger.print( Logger::LogFatal, _TXT("connection exception: %s"), msg);
		notify();
	}

	void pushJob( const WebRequestDelegateJobRef& job)
	{
		{
			strus::unique_lock lock( m_waitingList_mutex);
			m_waitingList.push_back( job);
		}
		notify();
	}

	bool send(
		const std::string& address,
		const std::string& method,
		const std::string& content,
		WebRequestDelegateContextInterface* receiver)
	{
		WebRequestDelegateConnectionRef conn;
		try
		{
			strus::shared_ptr<WebRequestDelegateContextInterface> receiverRef( receiver);
			conn = getOrCreateConnection( address);
			conn->push( method, content, receiverRef);
			WebRequestDelegateDataRef data( conn->fetchNewIfNotProcessing());
			if (data.get())
			{
				WebRequestDelegateJobRef job( new WebRequestDelegateJob( conn, data, this));
				pushJob( job);
			}
			return true;
		}
		catch (const std::runtime_error& err)
		{
			WebRequestAnswer answer( err.what(), 500, ErrorCodeDelegateRequestFailed);
			receiver->putAnswer( answer);
			handleException( err.what());
			if (conn.get()) conn->dropAllPendingRequests( ErrorCodeRuntimeError, err.what());
			return false;
		}
		catch (const std::bad_alloc& )
		{
			WebRequestAnswer answer( _TXT("memory allocation error"), 500, ErrorCodeOutOfMem);
			receiver->putAnswer( answer);
			handleException( _TXT("memory allocation error"));
			if (conn.get()) conn->dropAllPendingRequests( ErrorCodeOutOfMem, strus::errorCodeToString( ErrorCodeOutOfMem));
			return false;
		}
		catch (...)
		{
			WebRequestAnswer answer( _TXT("unexpected exception"), 500, ErrorCodeDelegateRequestFailed);
			receiver->putAnswer( answer);
			handleException( _TXT("unexpected exception"));
			if (conn.get()) conn->dropAllPendingRequests( ErrorCodeUncaughtException, strus::errorCodeToString( ErrorCodeUncaughtException));
			return false;
		}
	}

	typedef std::pair<WebRequestEventLoopInterface::TickerFunction,void*> TickerFunctionDef;

	bool addTickerEvent( void* obj, WebRequestEventLoopInterface::TickerFunction func)
	{
		try
		{
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
		std::vector<TickerFunctionDef>::const_iterator ti = m_tickerar.begin(), te = m_tickerar.end();
		for (; ti != te; ++ti)
		{
			ti->first( ti->second);
		}
	}

	void activateIdleJobs()
	{
		strus::unique_lock lock( m_waitingList_mutex);
		std::vector<WebRequestDelegateJobRef>::iterator wi = m_waitingList.begin(), we = m_waitingList.end();
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

	WebRequestDelegateJobRef fetchJob( CURL* handle)
	{
		std::map<CURL*,WebRequestDelegateJobRef>::iterator ai = m_activatedMap.find( handle);
		if (ai == m_activatedMap.end()) return WebRequestDelegateJobRef();
		WebRequestDelegateJobRef rt = ai->second;
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
			if (Logger::LogError <= m_logger.loglevel())
			{
				m_logger.print( Logger::LogError, _TXT("error in %s: %s"), context, curl_multi_strerror(ec));
			}
		}
	}

	void dropAllPendingRequests( const strus::ErrorCode& errcode, const char* errmsg)
	{
		{
			strus::unique_lock lock( m_connectionMap_mutex);
			ConnectionMap::iterator ci = m_connectionMap.begin(), ce = m_connectionMap.end();
			for (; ci != ce; ++ci)
			{
				WebRequestDelegateConnectionRef conn = ci->second;
				conn->dropAllPendingRequests( errcode, errmsg);
			}
		}{
			std::map<CURL*,WebRequestDelegateJobRef>::iterator ai = m_activatedMap.begin(), ae = m_activatedMap.end();
			for (; ai != ae; ++ai)
			{
				WebRequestDelegateJobRef job = ai->second;
				job->dropRequest( errmsg);
			}
		}
	}

	void run()
	{
		while (!m_terminate.test())
		{
			try
			{
				int numfds = 0;
				int running_handles = 1;

				activateIdleJobs();
				LOG( _TXT("perform event"), curl_multi_perform( m_multi_handle, &running_handles));
				LOG( _TXT("wait event"), curl_multi_wait( m_multi_handle, &m_pipcurl_notify_fd, 1, m_milliSecondsPeriod, &numfds));
				if (m_pipcurl_notify_fd.revents)
				{
					//... interrupted by new request in the queue or a terminate is signaled
					m_pipcurl_notify_fd.revents = 0;
					consumeSelfPipeWrites();
					if (forcedTick()) tick();
				}
				else if (numfds == 0)
				{
					//... interrupted by timeout
					m_requestCount = 0;
					tick();
					continue;
				}
				else
				{
					//... interrupted by event on a request processed
					if (forcedTick()) tick();
				}
				if (m_terminate.test()) break;

				CURLMsg* msg = NULL;
				int msgs_left = 1;
				while (msgs_left && NULL != (msg = curl_multi_info_read( m_multi_handle, &msgs_left)))
				{
					if (msg->msg == CURLMSG_DONE)
					{
						WebRequestDelegateJobRef job = fetchJob( msg->easy_handle);
						if (!job.get())
						{
							if (Logger::LogError <= m_logger.loglevel())
							{
								m_logger.print( Logger::LogError, _TXT("logic error: lost handle"));
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
				if (Logger::LogFatal <= m_logger.loglevel())
				{
					m_logger.print( Logger::LogFatal, _TXT("runtime error in CURL event loop: %s"), err.what());
				}
			}
			catch (const std::bad_alloc& err)
			{
				if (Logger::LogFatal <= m_logger.loglevel())
				{
					m_logger.print( Logger::LogFatal, _TXT("out of memory in CURL event loop"));
				}
			}
		}
	}

private:
	CURLM* m_multi_handle;					//< handle to listen for multiple connections simultaneously
	strus::thread* m_thread;				//< background thread of the eventloop 
	std::vector<WebRequestDelegateJobRef> m_waitingList;	//< list of jobs in the queue to process
	strus::mutex m_waitingList_mutex;			//< mutex for job queue monitor
	std::map<CURL*,WebRequestDelegateJobRef> m_activatedMap;//< map of jobs bound to a cURL handle
	std::vector<TickerFunctionDef> m_tickerar;		//< tickers to call after timeout or a number of requests
	Logger m_logger;					//< logger for logging
	ConnectionMap m_connectionMap;				//< map of connections
	strus::mutex m_connectionMap_mutex;			//< mutex for connection map monitor
	int m_milliSecondsPeriod;				//< milliseconds to wait for a request until a timeout is signalled
	int m_requestCount;					//< counter of subsequent requests signalled without timeout
	int m_numberOfRequestsBeforeTick;			//< number of times a wait on a request or a timeout is interrupted by a request before a tick is inserted that normally occurrs on a timeout
	AtomicFlag m_terminate;					//< flag that determines the termination of the eventloop
	int m_pipfd[2];						//< pipe to listen for queue events in select: "self pipe trick"
	curl_waitfd m_pipcurl_notify_fd;			//< curl additional handle to wait for listening on the read part of the "self pipe trick" pipe
};


WebRequestDelegateJob::WebRequestDelegateJob( const WebRequestDelegateConnectionRef& conn_, const WebRequestDelegateDataRef& data_, CurlEventLoop::Data* eventloopdata_)
	:m_handle(conn_->handle())
	,m_conn(conn_),m_data(data_),m_eventloopdata(eventloopdata_)
	,m_response_content()
{
	m_response_errbuf[0] = '\0';
	CURL* curl = conn_->handle();

	set_curl_opt( curl, CURLOPT_POSTFIELDSIZE, m_data->content.size());
	set_curl_opt( curl, CURLOPT_POSTFIELDS, m_data->content.c_str());
	set_curl_opt( curl, CURLOPT_WRITEDATA, &m_response_content);
	set_curl_opt( curl, CURLOPT_WRITEFUNCTION, std_string_append_callback); 
	set_curl_opt( curl, CURLOPT_FAILONERROR, 0);
	set_curl_opt( curl, CURLOPT_ERRORBUFFER, m_response_errbuf);
	set_curl_opt( curl, CURLOPT_CUSTOMREQUEST, m_data->method.c_str());
}

void WebRequestDelegateJob::dropRequest( const char* errmsg)
{
	WebRequestAnswer answer( errmsg, 500, ErrorCodeDelegateRequestFailed);
	m_data->receiver->putAnswer( answer);
}

void WebRequestDelegateJob::resume( CURLcode ec)
{
	try
	{
		CURL* curl = m_conn->handle();
		if (ec == CURLE_OK)
		{
			long http_code = 0;
			curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code);
			WebRequestContent answerContent( "UTF-8", "application/json", m_response_content.c_str(), m_response_content.size());
			WebRequestAnswer answer( "", http_code, 0/*apperrorcode*/, answerContent);
			m_data->receiver->putAnswer( answer);
			m_conn->done();
		}
		else
		{
			long http_code = 500;
			curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code);
			WebRequestAnswer answer( m_response_errbuf, http_code, ErrorCodeDelegateRequestFailed);
			m_data->receiver->putAnswer( answer);
			m_conn->reconnect();
			m_data->receiver.reset();
		}
		feedNextJobForThisConnection();
	}
	catch (const std::runtime_error& err)
	{
		m_eventloopdata->handleException( err.what());
		m_conn->dropAllPendingRequests( ErrorCodeRuntimeError, err.what());
	}
	catch (const std::bad_alloc& )
	{
		m_eventloopdata->handleException( _TXT("memory allocation error"));
		m_conn->dropAllPendingRequests( ErrorCodeOutOfMem, strus::errorCodeToString( ErrorCodeOutOfMem));
	}
	catch (...)
	{
		m_eventloopdata->handleException( _TXT("unexpected exception"));
		m_conn->dropAllPendingRequests( ErrorCodeUncaughtException, strus::errorCodeToString( ErrorCodeUncaughtException));
	}
}

void WebRequestDelegateJob::feedNextJobForThisConnection()
{
	WebRequestDelegateDataRef next = m_conn->fetchNewIfNotProcessing();
	if (next.get())
	{
		WebRequestDelegateJobRef job( new WebRequestDelegateJob( m_conn, next, m_eventloopdata));
		m_eventloopdata->pushJob( job);
	}
}


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
	m_data->dropAllPendingRequests( ErrorCodeServiceShutdown, strus::errorCodeToString( ErrorCodeServiceShutdown));
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
		const std::string& address,
		const std::string& method,
		const std::string& content,
		WebRequestDelegateContextInterface* receiver)
{
	return m_data->send( address, method, content, receiver);
}

bool CurlEventLoop::addTickerEvent( void* obj, TickerFunction func)
{
	return m_data->addTickerEvent( obj, func);
}

void CurlEventLoop::handleException( const char* msg)
{
	m_data->handleException( msg);
}


