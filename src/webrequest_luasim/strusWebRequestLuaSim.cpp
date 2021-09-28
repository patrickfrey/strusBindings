/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/lib/error.hpp"
#include "strus/lib/webrequest.hpp"
#include "strus/webRequestHandlerInterface.hpp"
#include "strus/webRequestContextInterface.hpp"
#include "strus/webRequestEventLoopInterface.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/debugTraceInterface.hpp"
#include "strus/reference.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/shared_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/numstring.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/stdint.h"
#include "strus/base/regex.hpp"
#include "strus/base/utf8.hpp"
#include "private/internationalization.hpp"
#include "blockingCurlClient.hpp"
#include "papuga/lib/lua_dev.h"
#include "papuga/serialization.h"
#include "papuga/valueVariant.h"
#include "papuga/encoding.h"
#include "papuga/requestParser.h"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "strus/lib/lua.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <limits>
#include <unistd.h>
#include <map>
#include <cstdbool>
#include <iostream>
#include <inttypes.h>
#include <sys/types.h>
#include <signal.h>

struct CString
{
	const char* value;

	CString( const char* value_) :value(value_){}
	CString( const CString& o) :value(o.value){}

	bool operator<( const CString& o) const {return std::strcmp(value,o.value) < 0;}
};

/// \brief Interface for executing XML/JSON requests based on the strus bindings API
class Logger
	:public strus::WebRequestLoggerInterface
{
public:
	Logger()
		:m_loglevel(0),m_logmask(0){}

	void init( int loglevel_)
	{
		m_loglevel = loglevel_;
		switch (m_loglevel)
		{
			case 4: m_logmask |= (LogContentEvents|LogConnectionEvents);
			case 3: m_logmask |= (LogAction);
			case 2: m_logmask |= (LogRequests|LogDelegateRequests|LogConfiguration);
			case 1: m_logmask |= (LogError|LogWarning|LogContextInfoMessages);
			case 0: m_logmask |= (LogNothing); break;
			default: m_logmask = LogAll;
		}
	}

	void initLogContextInfoMessages()
	{
		m_logmask |= LogContextInfoMessages;
	}

	void setCurrentProc( const char* name)
	{
		m_procname = name ? name : "";
	}

	virtual ~Logger(){}

	virtual int logMask() const
	{
		return m_logmask;
	}

	virtual int structDepth() const
	{
		return 100;
	}
	virtual void logRequest( const char* reqstr, std::size_t reqstrlen)
	{
		std::string reqstrbuf;
		reqstr = reduceContentSize( reqstrbuf, reqstr, reqstrlen, MaxLogContentSize);
		std::cerr << header() << strus::string_format( "REQUEST [%s]", reqstr) << std::endl;
	}

	void logRequestType( const char* title, const char* procdescr, const char* contextType, const char* contextName)
	{
		if (contextName && contextType && contextName != contextType)
		{
			std::cerr << header() << strus::string_format( "REQTYPE %s %s %s %s", title, procdescr, contextType, contextName) << std::endl;
		}
		else if (contextType)
		{
			std::cerr << header() << strus::string_format( "REQTYPE %s %s %s", title, procdescr, contextType) << std::endl;
		}
		else if (procdescr)
		{
			std::cerr << header() << strus::string_format( "REQTYPE %s %s", title, procdescr) << std::endl;
		}
		else
		{
			std::cerr << header() << strus::string_format( "REQTYPE %s", title) << std::endl;
		}
	}

	void logRequestAnswer( const char* content, std::size_t contentsize)
	{
		if (contentsize)
		{
			std::string contentbuf;
			content = reduceContentSize( contentbuf, content, contentsize, MaxLogContentSize);
			std::cerr << header() << strus::string_format( "REQANSWER [%s]", content) << std::endl;
		}
		else
		{
			std::cerr << header() << strus::string_format( "REQANSWER none") << std::endl;
		}
	}

	virtual void logDelegateRequest( const char* address, const char* method, const char* content, std::size_t contentsize)
	{
		if (contentsize == 0)
		{
			std::cerr << header() << strus::string_format( "DELEGATE %s '%s'", method, address) << std::endl;
		}
		else
		{
			std::string contentbuf;
			content = reduceContentSize( contentbuf, content, contentsize, MaxLogContentSize);

			std::cerr << header() << strus::string_format( "DELEGATE %s '%s': '%s...'", method, address, content) << std::endl;
		}
	}

	virtual void logPutConfiguration( const char* contextType, const char* contextName, const std::string& configstr)
	{
		if (contextName && contextType && contextName != contextType)
		{
			std::cerr << header() << strus::string_format( "CONFIG %s '%s':\n\t", contextType, contextName) << indentString( configstr) << std::endl;
		}
		else
		{
			std::cerr << header() << strus::string_format( "CONFIG %s:\n\t", contextType) << indentString( configstr) << std::endl;
		}
	}

	virtual void logAction( const char* contextType, const char* contextName, const char* action)
	{
		if (contextName && contextType && contextName != contextType)
		{
			std::cerr << header() << strus::string_format( "ACTION %s '%s' %s", contextType, contextName, action) << std::endl;
		}
		else
		{
			std::cerr << header() << strus::string_format( "ACTION %s %s", contextType, action) << std::endl;
		}
	}

	virtual void logContentEvent( const char* title, const char* item, const char* content, std::size_t contentsize)
	{
		if (item && item[0])
		{
			if (content && content[0])
			{
				std::string contentbuf;
				std::size_t maxsize = isReadableText( content, 64) ? MaxLogReadableItemSize : MaxLogBinaryItemSize;
				content = reduceContentSize( contentbuf, content, contentsize, maxsize);

				std::cerr << header() << strus::string_format( "PARSE %s %s: %s", title, item, content) << std::endl;
			}
			else
			{
				std::cerr << header() << strus::string_format( "PARSE %s %s", title, item) << std::endl;
			}
		}
		else
		{
			if (contentsize)
			{
				std::string contentbuf;
				std::size_t maxsize = isReadableText( content, 64) ? MaxLogReadableItemSize : MaxLogBinaryItemSize;
				content = reduceContentSize( contentbuf, content, contentsize, maxsize);

				std::cerr << header() << strus::string_format( "PARSE %s: %s", title, content) << std::endl;
			}
			else
			{
				std::cerr << header() << strus::string_format( "PARSE %s", title) << std::endl;
			}
		}
	}

	virtual void logConnectionEvent( const char* content)
	{
		std::cerr << header() << strus::string_format( "connection event %s", content) << std::endl;
	}

	virtual void logConnectionState( const char* state, int arg)
	{
		if (arg)
		{
			std::cerr << header() << strus::string_format( "delegate connection state %s [%d]", state, arg) << std::endl;
		}
		else
		{
			std::cerr << header() << strus::string_format( "delegate connection state %s", state) << std::endl;
		}
	}

	virtual void logWarning( const char* warnmsg)
	{
		std::cerr << header() << strus::string_format( "WARNING %s", warnmsg) << std::endl;
	}

	virtual void logError( const char* errmsg)
	{
		std::cerr << header() << strus::string_format( "ERROR %s", errmsg) << std::endl;
	}

	virtual void logContextInfoMessages( const char* content)
	{
		std::cerr << header() << strus::string_format( "INFO %s", content) << std::endl;
	}

	enum {
		MaxLogContentSize=2048/*2K*/,
		MaxLogBinaryItemSize=32,
		MaxLogReadableItemSize=256
	};
private:
	std::string header() const
	{
		return m_procname.empty() ? std::string() : strus::string_format( "[%s] ", m_procname.c_str());
	}
	std::string indentString( const std::string& input, int ind=1)
	{
		std::string rt;
		std::string::const_iterator ii = input.begin(), ie = input.end();
		for (; ii != ie; ++ii)
		{
			rt.push_back( *ii);
			if (*ii == '\n')
			{
				for (int ti=0; ti<ind; ++ti)
				{
					rt.push_back( '\t');
				}
			}
		}
		return rt;
	}

	static bool isReadableText( const char* sample, std::size_t samplesize)
	{
		int si = 0, se = samplesize;
		for (; si != se; ++si)
		{
			if ((unsigned char)sample[si] <= 32) return true;
		}
		return false;
	}

	static const char* reduceContentSize( std::string& contentbuf, const char* content, std::size_t contentsize, std::size_t maxsize)
	{
		if (contentsize > maxsize)
		{
			std::size_t endidx = maxsize;
			for (;endidx > 0 && strus::utf8midchr( content[ endidx-1]); --endidx){}
			contentbuf.append( content, endidx);
			contentbuf.append( " ...");
			return contentbuf.c_str();
		}
		else
		{
			return content;
		}
	}

private:
	int m_loglevel;
	int m_logmask;
	std::string m_procname;
};

static Logger g_logger;
static const char* g_accept = "application/json, text/html, text/plain, application/xml";
static const char* g_charset = "utf-8";
static const char* g_doctype = "application/json";
static strus::BlockingCurlClient* g_blockingCurlClient = 0;
static int g_serverConnSleep = 2;
static int g_serverConnRetry = 12;
static bool g_printLuaCalls = false;
static int g_verbosity = 0;
static bool g_killChildOnExit = true;

namespace {
template <typename ValueType>
ValueType castLuaValue( lua_State* ls, int luaddr)
{
	throw std::runtime_error(_TXT("lua value cast error"));
}
template <>
int castLuaValue<int>( lua_State* ls, int luaddr)
{
	return lua_tointeger( ls, luaddr);
}
#if 0 //... not used
template <>
double castLuaValue<double>( lua_State* ls, int luaddr)
{
	return lua_tonumber( ls, luaddr);
}
#endif
template <>
std::string castLuaValue<std::string>( lua_State* ls, int luaddr)
{
	std::size_t valsize;
	lua_pushvalue( ls, luaddr);
	const char* val = lua_tolstring( ls, -1, &valsize);
	std::string rt( val, valsize);
	lua_pop( ls, 1);
	return rt;
}
}// anonymous namespace


class Configuration
{
public:
	Configuration( lua_State* ls_, int luaddr_)
		:m_ls(ls_),m_luaddr(luaddr_){}
	Configuration( const Configuration& o)
		:m_ls(o.m_ls),m_luaddr(o.m_luaddr){}

	template <typename ValueType>
	bool getValue( ValueType& val, const char* id) const
	{
		lua_pushvalue( m_ls, m_luaddr);				// STACK: TABLE

		char const* keyitr = id;
		while (hasFollowKey( keyitr))
		{
			if (!lua_istable( m_ls, -1))
			{
				throw strus::runtime_error(_TXT("try to access non table structure with key '%s'"), keyitr);
			}
			std::string key = nextKey( keyitr);
			lua_pushlstring( m_ls, key.c_str(), key.size()); //STACK: KEY TABLE
			if (lua_gettable( m_ls, -2))
			{						//STACK: SUBTABLE TABLE
				lua_remove( m_ls, -2);			//STACK: SUBTABLE
			}
			else
			{						//STACK: SUBTABLE
				lua_pop( m_ls, 1);			//STACK:
				return false;
			}
		}
		lua_pushstring( m_ls, keyitr);				// STACK: RESTKEY TABLE
		if (lua_gettable( m_ls, -2))
		{							//STACK: VALUE TABLE
			val = castLuaValue<ValueType>( m_ls, -1);
			lua_pop( m_ls, 2);				//STACK:
			return true;
		}
		else
		{							//STACK: TABLE
			lua_pop( m_ls, 1);				//STACK:
			return false;
		}
	}

private:
	static bool hasFollowKey( char const* id)
	{
		return 0!=std::strchr( id, '.');
	}

	static std::string nextKey( char const*& id)
	{
		char const* end = std::strchr( id, '.');
		const char* start = id;
		if (end)
		{
			id = end+1;
		}
		else
		{
			id = end = std::strchr( id, '\0');
		}
		return std::string( start, end-start);
	}

private:
	lua_State* m_ls;
	int m_luaddr;
};


class Processor
{
public:
	Processor( const std::string& hostname_, const Configuration& config, const std::string& configjson_)
		:m_hostname(hostname_),m_handler(createWebRequestHandler(config,configjson_)){}
	Processor( const Processor& o)
		:m_hostname(o.m_hostname),m_handler(o.m_handler){}

	strus::WebRequestAnswer call( const std::string& method, const std::string& path, const strus::WebRequestContent& content);

private:
	static strus::WebRequestHandlerInterface* createWebRequestHandler( const Configuration& config_, const std::string& configjson_);

private:
	std::string m_hostname;
	strus::shared_ptr<strus::WebRequestHandlerInterface> m_handler;
};

class GlobalContext
{
public:
	GlobalContext(){}

	void defineServer( const std::string& hostname, const Configuration& configmap, const std::string& configjson);
	void startServerProcess( const std::string& hostname, const std::string& configjson);
	strus::WebRequestAnswer call( const std::string& method, const std::string& url, const char* contentstr, std::size_t contentlen);
	void reportError( const char* fmt, ...);

private:
	typedef std::map<std::string,Processor> ProcMap;
	ProcMap m_procMap;
};

class EventLoop
	:public strus::WebRequestEventLoopInterface
{
public:
	EventLoop()
		:m_tickers(),m_time(0),m_lastTickTime(0),m_timerEventSecondsPeriod(30){}

	virtual ~EventLoop(){}

	virtual bool start(){return true;}
	virtual void stop(){}

	virtual bool send(
			char const* address,
			char const* method,
			char const* contentstr,
			size_t contentlen,
			strus::WebRequestDelegateContextInterface* receiver);

	virtual bool addTickerEvent( void* obj, TickerFunction func)
	{
		try
		{
			m_tickers.push_back( TickerDef( func, obj));
			return true;
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}
	}

	virtual void handleException( const char* msg)
	{
		g_logger.logError( msg);
	}
	virtual long time() const
	{
		return m_time;
	}

	void setTime( long time_)
	{
		m_time = time_;
		if (m_time - m_lastTickTime >= m_timerEventSecondsPeriod)
		{
			m_timerEventSecondsPeriod = m_time;
			callTickers();
		}
	}

	void initConfiguration( const Configuration& config);

public:
	void callTickers()
	{
		std::vector<TickerDef>::const_iterator ti = m_tickers.begin(), te = m_tickers.end();
		for (; ti != te; ++ti)
		{
			(*ti->first)( ti->second);
		}
	}

private:
	typedef std::pair<TickerFunction,void*> TickerDef;
	std::vector<TickerDef> m_tickers;
	long m_time;
	long m_lastTickTime;
	int m_timerEventSecondsPeriod;
};

struct ChildProcess
{
	std::string address;
	pid_t pid;
	std::string cmdline;

	ChildProcess( const std::string& address_, pid_t pid_, const std::string& cmdline_)
		:address(address_),pid(pid_),cmdline(cmdline_){}
	ChildProcess( const ChildProcess& o)
		:address(o.address),pid(o.pid),cmdline(o.cmdline){}
};

static std::string g_scriptDir;
static std::string g_outputDir;
static std::string g_serviceProgramCmdLine;
static std::vector<ChildProcess> g_childProcessList;
static strus::DebugTraceInterface* g_debugTrace = 0;
static strus::ErrorBufferInterface* g_errorhnd = 0;
static EventLoop g_eventLoop;
static GlobalContext* g_globalContext = 0;

static std::pair<std::string,std::string> splitUrl( const std::string& url)
{
	std::pair<std::string,std::string> rt;
	char const* pi = url.c_str();
	if (0==std::memcmp( pi, "http://", 7)) pi+=7;
	const char* server_start = pi;
	for (; *pi && *pi != '/'; ++pi){}
	rt.first.append( server_start, pi-server_start);
	for (; *pi == '/'; ++pi){}
	rt.second.append( pi);
	return rt;
}

static std::string getPostString( const std::string& hostaddr)
{
	char const* hptr = hostaddr.c_str();
	if (strus::caseInsensitiveStartsWith( hostaddr, "http:") || strus::caseInsensitiveStartsWith( hostaddr, "https:"))
	{
		hptr = std::strchr( hptr, ':')+1;
	}
	char const* portsep = std::strchr( hptr, ':');
	char const* portstr = portsep ? (portsep+1) : "80";
	char const* portend = std::strchr( portstr, '/');
	return portend ? std::string( portstr, portend-portstr) : std::string( portstr);
}

static std::string substStringVariable( const std::string& src, const char* name, const std::string& value)
{
	std::size_t namelen = std::strlen(name);
	char const* si = std::strchr( src.c_str(), '{');
	for (; si; si = std::strchr( si+1, '{'))
	{
		char const* ni = si+1;
		for (; (unsigned char)*ni <= 32; ++ni){}
		if (0!=std::memcmp( ni, name, namelen)) continue;
		ni += namelen;
		for (; (unsigned char)*ni <= 32; ++ni){}
		if (*ni == '}')
		{
			std::string rt;
			rt.append( src.c_str(), si-src.c_str());
			rt.append( value);
			rt.append( ni+1);
			return rt;
		}
	}
	return src;
}

static std::vector<std::string> splitCmdLine( const std::string& cmdline)
{
	std::vector<std::string> rt;
	char const* ci = cmdline.c_str();
	while (*ci)
	{
		if (*ci == '{')
		{
			char const* start = ci;
			int brkcnt = 1;
			for (++ci; *ci && brkcnt > 0; ++ci)
			{
				if (*ci == '{') ++brkcnt;
				if (*ci == '}') --brkcnt;
			}
			if (brkcnt) throw std::runtime_error(_TXT("syntax error, brackets not balanced in JSON passed by command line"));
			rt.push_back( std::string( start, ci-start));
		}
		else if (*ci == ' ')
		{
			++ci;
		}
		else if (*ci == '"' || *ci == '\'')
		{
			char const* start = ci+1;
			char eb = *ci;
			for (++ci; *ci && *ci != eb; ++ci){}
			if (*ci != eb) throw std::runtime_error(_TXT("syntax error, string passed by command line not terminated"));
			rt.push_back( std::string( start, ci-start));
			++ci;
		}
		else
		{
			char const* start = ci;
			char eb = ' ';
			for (++ci; *ci && *ci != eb; ++ci){}
			rt.push_back( std::string( start, ci-start));
			if (*ci == eb) ++ci;
		}
	}
	return rt;
}

static void killBackGroundProcesses()
{
	std::vector<ChildProcess>::const_iterator pi = g_childProcessList.begin(), pe = g_childProcessList.end();
	for (; pi != pe; ++pi)
	{
		if (0>::kill( pi->pid, SIGTERM))
		{
			::fprintf( stderr, _TXT("failed to kill process for %s with pid %d: %s\n"), pi->address.c_str(), (int)pi->pid, ::strerror(errno));
		}
		else if (g_verbosity >= 2)
		{
			::fprintf( stderr, _TXT("KILL pid %d (%s): %s\n"), (int)pi->pid, pi->address.c_str(), pi->cmdline.c_str());
		}
	}
	g_childProcessList.clear();
	::fflush( stderr);
}

static void listBackGroundProcesses()
{
	std::vector<ChildProcess>::const_iterator pi = g_childProcessList.begin(), pe = g_childProcessList.end();
	for (; pi != pe; ++pi)
	{
		if (g_verbosity >= 2)
		{
			::fprintf( stderr, _TXT("PID %d address %s: %s\n"), (int)pi->pid, pi->address.c_str(), pi->cmdline.c_str());
		}
	}
	::fflush( stderr);
}

void EventLoop::initConfiguration( const Configuration& config)
{
	int max_idle_time = 600;
	(void)config.getValue( max_idle_time, "transactions.max_idle_time");
	m_timerEventSecondsPeriod = std::max( 10, max_idle_time/20);
}

bool EventLoop::send(
		char const* address,
		char const* method,
		char const* contentstr,
		size_t contentlen,
		strus::WebRequestDelegateContextInterface* receiver)
{
	try
	{
		strus::WebRequestAnswer result = g_globalContext->call( method, address, contentstr, contentlen);
		receiver->putAnswer( result);
		return true;
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}
}

strus::WebRequestHandlerInterface* Processor::createWebRequestHandler( const Configuration& config, const std::string& configjson_)
{
	std::string configdir = "./";
	std::string schemadir;
	std::string scriptdir;
	std::string servicename = "test";
	int transaction_max_idle_time = 600;
	int transaction_nof_per_sec = 60;

	(void)config.getValue( configdir, "data.configdir");
	schemadir = strus::joinFilePath( configdir, "schema");
	scriptdir = strus::joinFilePath( configdir, "script");
	(void)config.getValue( schemadir, "data.schemadir");
	(void)config.getValue( scriptdir, "data.scriptdir");
	(void)config.getValue( servicename, "service.name");
	(void)config.getValue( transaction_max_idle_time, "transactions.max_idle_time");
	(void)config.getValue( transaction_nof_per_sec, "transactions.nof_per_sec");

	strus::WebRequestHandlerInterface* rt =
		strus::createWebRequestHandler(
			&g_eventLoop, &g_logger, ""/*html head*/,
			configdir, scriptdir, schemadir, servicename, 0/*port*/, true/*beautifiedOutput*/,
			transaction_max_idle_time, transaction_nof_per_sec,
			g_errorhnd);
	if (!rt) throw strus::runtime_error( _TXT("error creating request handler: %s"), g_errorhnd->fetchError());
	strus::WebRequestAnswer answer;
	if (!rt->init( configjson_.c_str(), configjson_.size(), answer))
	{
		const char* msg = answer.errorStr() ? answer.errorStr() : strus::errorCodeToString( answer.appErrorCode());
		throw strus::runtime_error( _TXT("failed to initialize request handler: %s"), msg);
	}
	return rt;
}

strus::WebRequestAnswer Processor::call( const std::string& method, const std::string& path, const strus::WebRequestContent& content)
{
	strus::WebRequestAnswer rt;
	std::string html_base_href =
		path.empty()
			? strus::string_format( "http://%s/", m_hostname.c_str())
			: strus::string_format( "http://%s/%s/", m_hostname.c_str(), path.c_str());

	strus::Reference<strus::WebRequestContextInterface>
		ctx( m_handler->createContext( g_accept, html_base_href.c_str(), method.c_str(), path.c_str(), content.str(), content.len(), rt));
	if (!ctx.get()) return rt;
	for (;;)
	{
		if (ctx->execute())
		{
			return ctx->getAnswer();
	}	}
	if (!rt.content().empty())
	{
		rt.copyContent();
	}
	return rt;
}

void GlobalContext::defineServer( const std::string& hostname, const Configuration& configmap, const std::string& configjson)
{
	std::pair<std::string,std::string> serverPathPair = splitUrl( hostname);
	if (!serverPathPair.second.empty())
	{
		reportError( _TXT("not accepted name for a host '%s'"), hostname.c_str());
	}
	if (m_procMap.insert( ProcMap::value_type( serverPathPair.first, Processor( serverPathPair.first, configmap, configjson))).second == false)
	{
		reportError( _TXT("duplicate definition of server '%s'"), serverPathPair.first.c_str());
	}
}

void GlobalContext::startServerProcess( const std::string& hostname, const std::string& configjson)
{
	std::string portstr = getPostString( hostname);
	std::string configfile = strus::string_format( "tmp.config.%s.js", portstr.c_str());
	std::string configpath = strus::joinFilePath( g_outputDir, configfile);
	int ec = strus::writeFile( configpath, configjson);
	if (ec) throw strus::runtime_error( _TXT("failed to write configuration for server to start into text file %s (errno %d): %s"), configpath.c_str(), ec, ::strerror(ec));

	std::string cmdline = g_serviceProgramCmdLine;
	cmdline = substStringVariable( cmdline, "port", portstr);
	cmdline = substStringVariable( cmdline, "config", configpath);
	cmdline = substStringVariable( cmdline, "verbosity", strus::string_format( "%d", g_verbosity));

	std::vector<std::string> cmd = splitCmdLine( cmdline);
	enum {MaxArgNum=64};
	const char* argv[ MaxArgNum+2];
	int argc = 0;
	if (cmd.empty()) throw std::runtime_error(_TXT("server program specified with option --program is invalid"));
	if (cmd.size() >= MaxArgNum) throw std::runtime_error(_TXT("too many arguments for starting server"));
	std::vector<std::string>::const_iterator ci = cmd.begin(), ce = cmd.end();
	for (; ci != ce; ++ci,++argc)
	{
		argv[ argc] = ci->c_str();
	}
	argv[ argc] = 0;

	std::string show_cmdline;
	ec = strus::getFileName( argv[0], show_cmdline);
	if (ec) throw strus::runtime_error(_TXT("failed to get filename of server program specified with option --program: %s"), ::strerror(ec));
	show_cmdline.append( cmdline.c_str() + cmd[0].size());

	pid_t pid = ::fork();
	if ( pid == 0 )
	{
		pid_t pid_chld = ::getpid();
		if (g_verbosity >= 2) fprintf( stderr, _TXT("FORK child pid %d EXEC %s\n"), (int)pid_chld, cmdline.c_str());
		ec = ::execvp( argv[0], (char* const*)(argv));
		if (ec)
		{
			std::fprintf( stderr, _TXT("failed to run server %s: return %d"), hostname.c_str(), ec);
		}
		exit( ec);
	}
	else
	{
		if (g_verbosity >= 2) fprintf( stderr, _TXT("FORK parent pid %d, sleeping for %d seconds\n"), (int)getpid(), g_serverConnSleep);
		g_childProcessList.push_back( ChildProcess( hostname, pid, show_cmdline));
		::sleep( g_serverConnSleep);
	}
}

strus::WebRequestAnswer GlobalContext::call( const std::string& method, const std::string& url, const char* contentstr, std::size_t contentlen)
{
	if (g_blockingCurlClient)
	{
		strus::BlockingCurlClient::Response response = g_blockingCurlClient->sendJsonUtf8( method, url, g_charset, g_doctype, contentstr, contentlen, g_serverConnRetry);
		strus::WebRequestContent content( g_charset, g_doctype, response.content.c_str(), response.content.size());
		strus::WebRequestAnswer rt( response.httpstatus, content);
		rt.copyContent();
		return rt;
	}
	else
	{
		std::pair<std::string,std::string> serverPathPair = splitUrl( url);
		const std::string& proc = serverPathPair.first;
		const std::string& path = serverPathPair.second;
		strus::WebRequestContent content( g_charset, g_doctype, contentstr, contentlen);

		ProcMap::iterator pi = m_procMap.find( proc);
		if (pi == m_procMap.end())
		{
			const char* errmsg = strus::errorCodeToString( strus::ErrorCodeNotFound);
			return strus::WebRequestAnswer( 404, strus::ErrorCodeNotFound, errmsg);
		}
		else
		{
			return pi->second.call( method, path, content);
		}
	}
}

void GlobalContext::reportError( const char* fmt, ...)
{
	va_list ap;
	va_start( ap, fmt);

	std::string msg = strus::string_format_va( fmt, ap);
	std::cerr << "ERROR " << msg << std::endl;
}

enum TableType {TableTypeEmpty,TableTypeArray,TableTypeDictionary};
static TableType getTableType( lua_State* L, int luaddr)
{
	lua_pushvalue( L, luaddr);
	lua_pushnil( L );
	if (lua_next( L, -2) != 0)
	{
		TableType rt = (lua_isnumber( L, -2) && lua_tointeger( L, -2) == 1) ? TableTypeArray : TableTypeDictionary;
		lua_pop( L, 3 );
		return rt;
	}
	lua_pop( L, 1 );
	return TableTypeEmpty;
}

static void convertLuaArrayToJson_( lua_State* L, int luaddr, std::string& dest, int indent);
static void convertLuaDictionaryToJson_( lua_State* L, int luaddr, std::string& dest, int indent);

static void appendIndent( std::string& dest, int indent)
{
	if (indent >= 0)
	{
		dest.append( "\n");
		dest.resize( dest.size()+(indent*2), ' ');
	}
}

static int nextIndent( int indent)
{
	return indent >= 0 ? (indent+1) : indent;
}

static bool convertLuaValueToJson_( lua_State* L, int luaddr, std::string& dest, int indent)
{
	if (lua_isnil( L, luaddr))
	{
		return false;
	}
	else if (lua_isboolean( L, luaddr))
	{
		dest.append( lua_toboolean ( L, luaddr) ? "true" : "false");
	}
	else if (lua_isnumber( L, luaddr))
	{
		double fval = lua_tonumber( L, luaddr);
		double frac = fval - ::floor( fval);
		if (fval < 0.0 || frac > std::numeric_limits<double>::epsilon() * 10)
		{
			dest.push_back( '"');
			dest.append( lua_tostring( L, luaddr));
			dest.push_back( '"');
		}
		else
		{
			int value = lua_tointeger( L, luaddr);
			dest.append( strus::string_format( "%d", value));
		}
	}
	else if (lua_isstring( L, luaddr))
	{
		dest.push_back( '"');
		dest.append( lua_tostring( L, luaddr));
		dest.push_back( '"');
	}
	else if (lua_istable( L, luaddr))
	{
		switch (getTableType( L, luaddr))
		{
			case TableTypeEmpty:
				return false;
			case TableTypeArray:
				dest.append( "[");
				convertLuaArrayToJson_( L, luaddr, dest, indent);
				dest.append( "]");
				break;
			case TableTypeDictionary:
				dest.append( "{");
				convertLuaDictionaryToJson_( L, luaddr, dest, indent);
				dest.append( "}");
				break;
		}
	}
	else
	{
		const char* str = lua_tostring( L, luaddr);
		dest.push_back( '"');
		if (!str) return false;
		dest.push_back( '"');
		dest.append( str);
	}
	return true;
}

static void convertLuaArrayToJson_( lua_State* L, int luaddr, std::string& dest, int indent)
{
	lua_pushvalue( L, luaddr);
	lua_pushnil( L );
	int itercnt = -1;

	while (lua_next( L, -2) != 0)
	{
		++itercnt;

		if (lua_isnumber( L, -2))
		{
			if (lua_tointeger( L, -2) != itercnt+1)
			{
				luaL_error( L, _TXT("array with non strict ascending indices or not starting with 1"));
			}
		}
		else
		{
			luaL_error( L, _TXT("mixing array and table"));
		}
		if (itercnt)
		{
			dest.append( ",");
		}
		appendIndent( dest, indent);
		if (!convertLuaValueToJson_( L, -1, dest, nextIndent(indent)))
		{
			dest.append( "null");
		}
		lua_pop( L, 1 );
	}
	lua_pop( L, 1); // Get luaddr-value from stack
}

static void convertLuaDictionaryToJson_( lua_State* L, int luaddr, std::string& dest, int indent)
{
	std::map<CString,std::string> dictmap;
	lua_pushvalue( L, luaddr);
	lua_pushnil( L );

	while (lua_next( L, -2) != 0)
	{
		const char* key = 0;

		if (lua_isnumber( L, -2))
		{
			luaL_error( L, _TXT("mixing array and table"));
		}
		else if (lua_isstring( L, -2))
		{
			key = lua_tostring( L, -2);
		}
		else {
			luaL_error( L, _TXT("non string type keys are not supported in table conversion to JSON"));
		}
		std::string value;
		if (convertLuaValueToJson_( L, -1, value, nextIndent(indent)))
		{
			dictmap[ key] = value;
		}
		lua_pop( L, 1 );
	}
	lua_pop( L, 1); // Get luaddr-value from stack

	std::map<CString,std::string>::const_iterator di = dictmap.begin(), de = dictmap.end();
	for (int didx=0; di != de; ++di,++didx)
	{
		if (didx) dest.append( ",");
		appendIndent( dest, indent);
		dest.append( "\"");
		dest.append( di->first.value);
		dest.append( "\": ");
		dest.append( di->second);
	}
}

static std::string convertLuaValueToJson( lua_State* L, int luaddr, bool withIndentiation)
{
	std::string rt;
	if (withIndentiation)
	{
		convertLuaValueToJson_( L, luaddr, rt, 0);
		rt.push_back('\n');
	}
	else
	{
		convertLuaValueToJson_( L, luaddr, rt, -1);
	}
	return rt;
}

static std::string getContentArgumentValue( const char* arg)
{
	if (!arg)
	{
		return std::string();
	}
	else if (arg[0] == '@')
	{
		std::string content;
		std::string fullpath = strus::joinFilePath( g_scriptDir, arg+1);
		int ec = strus::readFile( fullpath, content);
		if (ec) throw strus::runtime_error( _TXT("error (%d) reading file %s with content to process by server: %s"), ec, fullpath.c_str(), ::strerror(ec));
		return content;
	}
	else
	{
		return arg;
	}
}

static void pushConvertedJsonAsLuaTable( lua_State* L, int luaddr)
{
	if (!lua_isstring( L, luaddr))
	{
		throw strus::runtime_error( _TXT("error converting JSON to lua table: %s"), _TXT("argument is not a string"));
	}
	std::string content;
	std::size_t srclen;
	const char* src = lua_tolstring( L, luaddr, &srclen);
	if (src[0] == '@')
	{
		content = getContentArgumentValue( src);
		src = content.c_str();
		srclen = content.size();
	}
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_Serialization jsonser;
	int allocatormem[ 1024];
	papuga_Allocator allocator;
	papuga_init_Allocator( &allocator, allocatormem, sizeof(allocatormem));
	papuga_init_Serialization( &jsonser, &allocator);
	if (!papuga_Serialization_append_json( &jsonser, src, srclen, papuga_UTF8, true/*withRoot*/, &errcode))
	{
		papuga_destroy_Allocator( &allocator);
		throw strus::runtime_error( _TXT("error converting JSON to lua table: %s"), papuga_ErrorCode_tostring( errcode));
	}
	papuga_ValueVariant jsonval;
	papuga_init_ValueVariant_serialization( &jsonval, &jsonser);
	if (!papuga_lua_push_value( L, &jsonval, &errcode))
	{
		papuga_destroy_Allocator( &allocator);
		throw strus::runtime_error( _TXT("error converting JSON to lua table: %s"), papuga_ErrorCode_tostring( errcode));
	}
	papuga_destroy_Allocator( &allocator);
}

static std::string convertSourceReformatFloat( const char* src, unsigned int precision)
{
	std::string rt;
	enum {MaxFloatStringSize=48};
	if (precision >= MaxFloatStringSize-3) return src;
	char const* si = src;
	while (*si)
	{
		if (*si == '-' || (*si >= '0' && *si <= '9'))
		{
			bool isFloat = false;
			char const* se = si;
			if (*se == '-') ++se;
			for (; *se >= '0' && *se <= '9'; ++se){}
			if (*se == '.')
			{
				++se;
				for (; *se >= '0' && *se <= '9'; ++se){}
				isFloat = true;
			}
			if (*se == 'e' || *se == 'E')
			{
				++se;
				if (*se == '-') ++se;
				for (; *se >= '0' && *se <= '9'; ++se){}
				isFloat = true;
			}
			if (isFloat && se - si < MaxFloatStringSize)
			{
				char numbuf[ MaxFloatStringSize+1];
				std::memcpy( numbuf, si, se-si);
				numbuf[ se-si] = 0;
				double number;
				if (1==std::sscanf( numbuf, "%lf", &number))
				{
					char formatstr[ 16];
					std::snprintf( formatstr, sizeof(formatstr), "%%.%uf", precision);
					std::snprintf( numbuf, sizeof(numbuf), formatstr, number);
					rt.append( numbuf);
					si = se;
					continue;
				}

			}
		}
		rt.push_back( *si);
		++si;
	}
	return rt;
}

static std::string convertSourceRegexReplace( const char* src, const char* expr, const char* substfmt)
{
	std::string rt;
	std::size_t srcsize = std::strlen(src);
	strus::RegexSearch regex( expr, 0, g_errorhnd);
	strus::RegexSubst subst( expr, substfmt, g_errorhnd);
	if (g_errorhnd->hasError()) throw strus::runtime_error( _TXT("error in init replace regex: %s"), g_errorhnd->fetchError());
	std::vector<strus::RegexSearch::Match> matches = regex.find_all( src, srcsize);
	std::size_t ofs = 0;
	std::vector<strus::RegexSearch::Match>::const_iterator mi = matches.begin(), me = matches.end();
	for (; mi != me; ++mi)
	{
		rt.append( src + ofs, mi->pos - ofs);
		if (mi->pos + mi->len > (int)srcsize) throw strus::runtime_error(_TXT("array bound read in %s"), "convertSourceRegexReplace");
		subst.exec( rt, src + mi->pos, mi->len);
		ofs = mi->pos + mi->len;
	}
	rt.append( src + ofs, srcsize - ofs);
	if (g_errorhnd->hasError()) throw strus::runtime_error( _TXT("error in run replace regex: %s"), g_errorhnd->fetchError());
	return rt;
}

static int g_nofLuaCalls = 0;
static void logLuaCall( std::ostream& out, lua_State* L, const char* fname, int nofArgs)
{
	int ni = 0;
	++g_nofLuaCalls;
	std::string cmdline;
	cmdline.append( fname);
	for (; ni < nofArgs; ++ni)
	{
		cmdline.push_back( ' ');
		cmdline.append( convertLuaValueToJson( L, 1+ni, false/*no indent*/));
	}
	out << "LUA " << strus::string_format( "%d: ", g_nofLuaCalls) << cmdline << std::endl;
}

static void logLuaReturn( std::ostream& out, lua_State* L, const char* fname, int nofRetvals)
{
	int ni = nofRetvals;
	std::string retline( fname);
	retline.append( strus::string_format( "[%d]", nofRetvals));
	for (; ni > 0; --ni)
	{
		retline.push_back( ' ');
		retline.append( convertLuaValueToJson( L, -ni, false/*no indent*/));
	}
	out << "RET " << strus::string_format( "%d: ", g_nofLuaCalls) << retline << std::endl;
}

static void LUA_FUNCTION_HEADER( lua_State* L, const char* funcname, int minNofArgs, int maxNofArgs, const char* expectedArgs)
{
	int nofArgs = lua_gettop(L);
	if (g_verbosity >= 2 || g_printLuaCalls) logLuaCall( std::cerr, L, funcname, nofArgs);

	if (nofArgs > maxNofArgs) luaL_error(L, _TXT("too many (%d, max %d) arguments for '%s', expected %s"), nofArgs, maxNofArgs, funcname, expectedArgs);
	if (nofArgs < minNofArgs) luaL_error(L, _TXT("too few (%d, min %d) arguments for '%s', expected %s"), nofArgs, minNofArgs, funcname, expectedArgs);
}

static void LUA_FUNCTION_TAIL( lua_State* L, const char* funcname, int nofRetvals)
{
	if (g_verbosity >= 2 || g_printLuaCalls) logLuaReturn( std::cerr, L, funcname, nofRetvals);
}

static int l_set_time( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "set_time", 1, 1, "<seconds since baseline as int>");

		long newtime = lua_tointeger( L, 1);
		g_eventLoop.setTime( newtime);

		LUA_FUNCTION_TAIL( L, "set_time", 0);
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
	}
	return 0;
}

static int l_def_server( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "def_server", 2, 2, "<hostname> <configuration>");

		const char* hostname = lua_tostring( L, 1);
		Configuration configmap( L, 2);
		std::string configjson = convertLuaValueToJson( L, 2, true/*indentiation*/);

		if (g_serviceProgramCmdLine.empty())
		{
			g_globalContext->defineServer( hostname, configmap, configjson);
		}
		else
		{
			g_globalContext->startServerProcess( hostname, configjson);
		}
		LUA_FUNCTION_TAIL( L, "def_server", 0);
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
	}
	return 0;
}

static int l_call_server( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "call_server", 2, 3, "<method> <url> [<content to send>]");
		int nofArgs = lua_gettop(L);

		const char* method = lua_tostring( L, 1);
		const char* url = lua_tostring( L, 2);
		const char* arg = "";
		std::size_t arglen = 0;
		std::string content;
		if (nofArgs == 3 && !lua_isnil( L, 3))
		{
			if (lua_isstring( L, 3))
			{
				arg = lua_tostring( L, 3);
				content = getContentArgumentValue( arg);
				arg = content.c_str();
				arglen = content.size();
			}
			else if (lua_istable( L, 3))
			{
				content = convertLuaValueToJson( L, 3, true/*indentiation*/);
				arg = content.c_str();
				arglen = content.size();
			}
			else
			{
				luaL_error(L, _TXT("table or string expected as 3rd argument 'call_server'"));
			}
		}
		strus::WebRequestAnswer result = g_globalContext->call( method, url, arg, arglen);
		if (result.content().empty())
		{
			lua_pushnil(L);
			lua_pushinteger(L, result.httpStatus()); /* second return value */
			lua_pushstring(L, result.errorStr());
		}
		else
		{
			lua_pushlstring(L, result.content().str(), result.content().len());
			lua_pushinteger(L, result.httpStatus()); /* second return value */
			lua_pushnil(L);
		}
		LUA_FUNCTION_TAIL( L, "call_server", 3);
		return 3;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static void push_linestring( lua_State* L, char const* ln)
{
	char const* end = std::strchr( ln, '\n');
	if (!end) end = std::strchr( ln, '\0');
	lua_pushlstring( L, ln, end-ln);
}

static const char* skipEoln( const char* si)
{
	if (si[0] == '\r')
	{
		return (si[1] == '\n') ? (si + 2) /*CRLF - Windows*/: (si + 1) /*CR - older Mac OS*/;
	}
	else if (si[0] == '\n')
	{
		return si+1;
	}
	else
	{
		return NULL;
	}
}

static int l_cmp_content( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "cmp_content", 2, 2, "<content string> <other>");

		int linecnt = 1;
		const char* arg_1 = lua_tostring( L, 1);
		const char* arg_2 = lua_tostring( L, 2);
		std::string content_a = getContentArgumentValue( arg_1);
		std::string content_b = getContentArgumentValue( arg_2);
		char const* ai = content_a.c_str();
		char const* bi = content_b.c_str();
		const char* a_ln = ai;
		const char* b_ln = bi;

		while (*ai && *bi)
		{
			if ((*ai == '\r' || *ai == '\n') && (*bi == '\r' || *bi == '\n'))
			{
				++linecnt;
				ai = skipEoln( ai);
				bi = skipEoln( bi);
				if (ai && bi)
				{
					a_ln = ai;
					b_ln = bi;
					continue;
				}
				else
				{
					return luaL_error(L, _TXT("bad state in 'cmp_content'"));
				}
			}
			else if (*ai == *bi)
			{
				++ai;
				++bi;
				continue;
			}
			lua_pushboolean( L, false);
			lua_pushinteger( L, linecnt);
			push_linestring( L, a_ln);
			push_linestring( L, b_ln);
			return 4;
		}
		while (*ai == '\r' || *ai == '\n') ai = skipEoln( ai);
		while (*bi == '\r' || *bi == '\n') bi = skipEoln( bi);
		if (!*ai && !*bi)
		{
			lua_pushboolean( L, true);
			LUA_FUNCTION_TAIL( L, "cmp_content", 1);
			return 1;
		}
		else
		{
			lua_pushboolean( L, false);
			lua_pushinteger( L, linecnt);
			push_linestring( L, a_ln);
			push_linestring( L, b_ln);
			LUA_FUNCTION_TAIL( L, "cmp_content", 4);
			return 4;
		}
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_load_file( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "load_file", 1, 1, "<filename>");

		const char* filename = lua_tostring( L, 1);
		std::string content;
		std::string fullpath = strus::joinFilePath( g_scriptDir, filename);
		int ec = strus::readFile( fullpath, content);
		if (ec) throw strus::runtime_error( _TXT("error (%d) reading file %s with content to process by server: %s"), ec, fullpath.c_str(), ::strerror(ec));
		lua_pushlstring(L, content.c_str(), content.size());

		LUA_FUNCTION_TAIL( L, "load_file", 1);
		return 1;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_write_file( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "write_file", 2, 2, "<filename> <content>");

		const char* filename = lua_tostring( L, 1);
		const char* content = lua_tostring( L, 2);

		std::string fullpath = strus::joinFilePath( g_outputDir, filename);
		std::string normcontent;
		char const* ci = content;
		while (*ci)
		{
			if (*ci == '\n' || *ci == '\r')
			{
				normcontent.push_back( '\n');
				ci = skipEoln( ci);
			}
			else
			{
				normcontent.push_back( *ci);
				++ci;
			}
		}
		int ec = strus::writeFile( fullpath, normcontent);
		if (ec)
		{
			luaL_error( L, _TXT("failed to write text file (errno %d): %s"), ec, ::strerror(ec));
		}
		LUA_FUNCTION_TAIL( L, "write_file", 0);
		return 0;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_remove_file( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "remove_file", 1, 1, "<filename>");

		const char* filename = lua_tostring( L, 1);
		std::string fullpath = strus::joinFilePath( g_outputDir, filename);
		int ec = strus::removeFile( fullpath, false/*fail_ifnofexist*/);
		if (ec)
		{
			luaL_error( L, _TXT("failed to remove file (errno %d): %s"), ec, ::strerror(ec));
		}
		LUA_FUNCTION_TAIL( L, "remove_file", 0);
		return 0;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_create_dir( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "create_dir", 1, 1, "<dirname>");

		const char* nam = lua_tostring( L, 1);
		std::string fullpath = strus::joinFilePath( g_outputDir, nam);
		int ec = strus::removeDirRecursive( fullpath, false/*fail_ifnofexist*/);
		if (ec)
		{
			luaL_error( L, _TXT("failed to remove directory (errno %d): %s"), ec, ::strerror(ec));
		}
		ec = strus::mkdirp( fullpath);
		if (ec)
		{
			luaL_error( L, _TXT("failed to create directory (errno %d): %s"), ec, ::strerror(ec));
		}
		LUA_FUNCTION_TAIL( L, "create_dir", 0);
		return 0;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_reformat_float( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "reformat_float", 2, 2, "<string to map> <precision of floats>");

		const char* content = lua_tostring( L, 1);
		int precision = lua_tointeger( L, 2);
		if (precision <= 0) return luaL_error(L, _TXT("expected positive number for precision argument of 'reformat_float'"));

		std::string result = convertSourceReformatFloat( content, precision);
		lua_pushlstring( L, result.c_str(), result.size());

		LUA_FUNCTION_TAIL( L, "reformat_float", 1);
		return 1;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_reformat_regex( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "reformat_regex", 2, 3, "<string to map> <expression to match> [<format string to replace with>]");
		int nofArgs = lua_gettop(L);

		const char* content = lua_tostring( L, 1);
		const char* expr = lua_tostring( L, 2);
		const char* substfmt = (nofArgs > 2) ? lua_tostring( L, 3) : "";

		std::string result = convertSourceRegexReplace( content, expr, substfmt);
		lua_pushlstring( L, result.c_str(), result.size());

		LUA_FUNCTION_TAIL( L, "reformat_regex", 1);
		return 1;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_to_json( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "to_json", 1, 2, "<value to map>");
		int nofArgs = lua_gettop(L);

		bool indentiation = (nofArgs > 1) ? lua_toboolean( L, 2) : true;
		std::string result = convertLuaValueToJson( L, 1, indentiation);
		lua_pushlstring( L, result.c_str(), result.size());

		LUA_FUNCTION_TAIL( L, "to_json", 1);
		return 1;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_from_json( lua_State* L)
{
	try
	{
		LUA_FUNCTION_HEADER( L, "from_json", 1, 1, "<json content to parse>");

		pushConvertedJsonAsLuaTable( L, 1);

		LUA_FUNCTION_TAIL( L, "from_json", 1);
		return 1;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static void declareFunctions( lua_State* L)
{
#define DEFINE_FUNCTION( NAME)	lua_pushcfunction( L, l_ ##NAME); lua_setglobal( L, #NAME);

	DEFINE_FUNCTION( set_time );
	DEFINE_FUNCTION( def_server );
	DEFINE_FUNCTION( call_server );
	DEFINE_FUNCTION( cmp_content );
	DEFINE_FUNCTION( write_file );
	DEFINE_FUNCTION( remove_file );
	DEFINE_FUNCTION( load_file );
	DEFINE_FUNCTION( create_dir );
	DEFINE_FUNCTION( reformat_float );
	DEFINE_FUNCTION( reformat_regex );
	DEFINE_FUNCTION( to_json );
	DEFINE_FUNCTION( from_json );
}

static void declareArguments( lua_State* L, const char** arguments)
{
	int ai = 0;
	lua_newtable( L);
	for (; arguments[ ai]; ++ai)
	{
		lua_pushinteger( L, ai+1);
		lua_pushstring( L, arguments[ ai]);
		lua_settable( L, -3);
	}
	lua_setglobal( L, "argv");

	lua_pushboolean( L, g_verbosity > 0);
	lua_setglobal( L, "verbose");
}

static void printUsage()
{
	fprintf( stderr, "%s",
		"strusWebRequestLuaSim [<options>] <luascript> [<outputdir>]\n"
		"Program Options <options>:\n"
		 "   -h,--help          :Print this usage\n"
		 "   -m,--mod <PATH>    :Set <PATH> as addidional module path\n"
		 "   -G,--debug <ID>    :Enable debug trace of items with id <ID>\n"
		 "   -V                 :Raise verbosity level (may be repeated)\n"
		 "   --calltrace        :Print trace of lua procedures defined here\n"
		 "                       This option has no effect with a verbosity >= 2,\n"
		 "                       because then the call trace is printed anyway.\n"
		 "   --nokill           :Do not kill child processes after exit\n"
		 "   -p,--program <PRG> :Use the command line <PRG> for starting a\n"
		 "                       web service instance instead of running the\n"
		 "                       commands directly. The strings {port} and {config}\n"
		 "                       in the command line are substituted by the\n"
		 "                       corresponding values\n");
	fprintf( stderr,
		 "Program arguments:\n"
		 "<luascript>           :Lua script with the commands to execute\n"
		 "<outputdir>           :Working directory (default execution dir)\n");
	fprintf( stderr, "The environment variable STRUS_CONN_SLEEP can be set as \n"
		"the number of seconds the fork parent sleeps while starting the\n"
		"web service program specified with --program. The default is %d\n",
		 g_serverConnSleep);
	fprintf( stderr, "The environment variable STRUS_CONN_RETRY can be set as \n"
		"the number of connection retries on client server connections\n"
		"when running with the option --program. The default is %d\n",
		 g_serverConnRetry);
}

static void setLuaPath( lua_State* ls, const char* path)
{
	const char* cur_path;
	if (g_verbosity) fprintf( stderr, _TXT("set lua module path to: '%s'\n"), path);
	char pathbuf[ 2048];
	lua_getglobal( ls, "package");
	lua_getfield( ls, -1, "path");
	cur_path = lua_tostring( ls, -1);
	if (cur_path[0])
	{
		if ((int)sizeof(pathbuf) <= std::snprintf( pathbuf, sizeof(pathbuf), "%s;%s/?.lua", cur_path, path))
		{
			luaL_error( ls, _TXT("internal buffer is too small for path"));
		}
	}
	else
	{
		if ((int)sizeof(pathbuf) <= std::snprintf( pathbuf, sizeof(pathbuf), "%s/?.lua", path))
		{
			luaL_error( ls, _TXT("internal buffer is too small for path"));
		}
	}
	if (g_verbosity) fprintf( stderr, _TXT("set lua module pattern to: '%s'\n"), pathbuf);
	lua_pop( ls, 1);
	lua_pushstring( ls, pathbuf);
	lua_setfield( ls, -2, "path");
	lua_pop( ls, 1);
}

static void setLuaPathParentDir( lua_State* ls, const char* path)
{
	char pathbuf[ 2048];
	char const* pp = strchr( path, '\0')-1;
	for (; pp>=path && (*pp == '/' || *pp == '\\'); --pp){}
	for (; pp>=path && *pp != '/' && *pp != '\\'; --pp){}
	for (; pp>=path && (*pp == '/' || *pp == '\\'); --pp){}
	++pp;
	if (pp - path >= (int)sizeof(pathbuf))
	{
		luaL_error( ls, _TXT("internal buffer is too small for path"));
	}
	memcpy( pathbuf, path, pp-path);
	pathbuf[ pp-path] = '\0';
	setLuaPath( ls, pathbuf);
}

static void handleLuaScriptError( lua_State* L, int errcode, const char* inputfile, const char* context)
{
	switch (errcode)
	{
		case LUA_OK:
			break;
		case LUA_ERRRUN:
			fprintf( stderr, _TXT("runtime error %s '%s': '%s'\n"), context, inputfile, lua_tostring( L, -1));
			break;
		case LUA_ERRMEM:
			fprintf( stderr, _TXT("out of memory %s '%s'\n"), context, inputfile);
			break;
		case LUA_ERRSYNTAX:
			fprintf( stderr, _TXT("syntax error error %s '%s': '%s'\n"), context, inputfile, lua_tostring( L, -1));
			break;
#ifdef LUA_ERRGCMM
		case LUA_ERRGCMM:
			fprintf( stderr, _TXT("internal error in destructor call (call of __gc metamethod) %s '%s'\n"), context, inputfile);
			break;
#endif
		case LUA_ERRERR:
			fprintf( stderr, _TXT("error in lua error handler %s '%s'\n"), context, inputfile);
			break;
		default:
			fprintf( stderr, _TXT("unknown error %s '%s'\n"), context, inputfile);
			break;
	}
}

static void printDebugTraceMessages()
{
	if (g_debugTrace)
	{
		std::vector<strus::DebugTraceMessage> messages = g_debugTrace->fetchMessages();
		std::vector<strus::DebugTraceMessage>::const_iterator mi = messages.begin(), me = messages.end();
		if (mi != me)
		{
			fprintf( stderr, "DEBUG:\n");
			for (; mi != me; ++mi)
			{
				fprintf( stderr, "%s %s %s: %s\n", mi->typeName(), mi->component(), mi->id(), mi->content().c_str());
			}
			fprintf( stderr, "\n");
		}
	}
}

#define MaxModPaths 64
int main( int argc, const char* argv[])
{
	int rt = 0;
	const char* inputfile = 0;
	enum {MaxNofArguments=7};
	const char* arguments[ MaxNofArguments+1];
	lua_State* ls = 0;
	int argi = 1;
	int errcode = 0;
	int modi = 0;
	int mi,me;
	const char* modpath[ MaxModPaths];

	try
	{
		g_debugTrace = strus::createDebugTrace_standard( 2);
		g_errorhnd = strus::createErrorBuffer_standard( NULL, 2, g_debugTrace);
		std::string thisCommandLine( "src/webrequest_luasim/strusWebRequestLuaSim");

		if (!g_debugTrace || !g_errorhnd)
		{
			throw std::runtime_error( _TXT( "out of memory creating error handler"));
		}
		strus::declareErrorBuffer_singleton( g_errorhnd);
		g_globalContext = new GlobalContext();

		/* Parse command line arguments: */
		for (; argi < argc; ++argi)
		{
			if (argv[argi][0] != '-')
			{
				break;
			}
			else if (0==strcmp( argv[argi], "--help") || 0==strcmp( argv[argi], "-h"))
			{
				printUsage();
				rt = 0;
				goto EXIT;
			}
			else if (0==strcmp( argv[argi], "-V") || 0==strcmp( argv[argi], "-VV") || 0==strcmp( argv[argi], "-VVV") || 0==strcmp( argv[argi], "-VVVV") || 0==strcmp( argv[argi], "-VVVVV"))
			{
				g_verbosity += std::strlen(argv[argi])-1;
				thisCommandLine.append( " -");
				thisCommandLine.append( std::string( g_verbosity, 'V'));
			}
			else if (0==strcmp( argv[argi], "--calltrace"))
			{
				g_printLuaCalls = true;
			}
			else if (0==strcmp( argv[argi], "--nokill"))
			{
				g_killChildOnExit = false;
			}
			else if (0==strcmp( argv[argi], "--debug") || 0==strcmp( argv[argi], "-G"))
			{
				++argi;
				if (argi == argc || argv[argi][0] == '-')
				{
					throw std::runtime_error( _TXT("option -G (--debug) needs argument"));
				}
				g_debugTrace->enable( argv[argi]);
				g_logger.initLogContextInfoMessages();
				thisCommandLine.append( strus::string_format( " -G \"%s\"", argv[argi]));
			}
			else if (0==strcmp( argv[argi], "--mod") || 0==strcmp( argv[argi], "-m"))
			{
				++argi;
				if (argi == argc || argv[argi][0] == '-')
				{
					throw std::runtime_error( _TXT("option -m (--mod) needs argument"));
				}
				if (modi >= MaxModPaths)
				{
					throw std::runtime_error( _TXT("too many options -m (--mod) specified in argument list"));
				}
				modpath[ modi++] = argv[argi];
				thisCommandLine.append( strus::string_format( " -m \"%s\"", argv[argi]));
			}
			else if (0==strcmp( argv[argi], "--program") || 0==strcmp( argv[argi], "-p"))
			{
				++argi;
				if (argi == argc || argv[argi][0] == '-')
				{
					throw std::runtime_error( _TXT("option -p (--program) needs argument"));
				}
				if (!g_serviceProgramCmdLine.empty())
				{
					throw std::runtime_error( _TXT("option -p(--program) specified twice"));
				}
				g_serviceProgramCmdLine = argv[argi];
				thisCommandLine.append( strus::string_format( " --program \"%s\"", argv[argi]));
			}
			else if (0==strcmp( argv[argi], "--"))
			{
				thisCommandLine.append( " --");
				++argi;
				break;
			}
		}
		if (argi == argc)
		{
			throw std::runtime_error( _TXT("too few arguments (less than one)"));
		}
		if (!g_serviceProgramCmdLine.empty())
		{
			char const* serverConnSleepEnv = ::getenv( "STRUS_CONN_SLEEP");
			if (serverConnSleepEnv)
			{
				int wsleep = ::atoi( serverConnSleepEnv);
				if (wsleep > 0)
				{
					g_serverConnSleep = wsleep;
				}
				else
				{
					std::cerr << _TXT("ignoring value of environment variable STRUS_WS_SLEEP expected to be an integer greater than 0") << std::endl;
				}
			}
			char const* serverConnRetryEnv = ::getenv( "STRUS_CONN_RETRY");
			if (serverConnRetryEnv)
			{
				int wretry = ::atoi( serverConnRetryEnv);
				if (wretry > 0)
				{
					g_serverConnRetry = wretry;
				}
				else
				{
					std::cerr << _TXT("ignoring value of environment variable STRUS_WS_RETRY expected to be an integer greater than 0") << std::endl;
				}
			}
		}
		inputfile = argv[ argi++];
		thisCommandLine.append( strus::string_format( " \"%s\"", inputfile));

		int ec = strus::getParentPath( inputfile, g_scriptDir);
		if (ec) throw std::runtime_error( _TXT("failed to get directory of the script to execute"));

		if (argi == argc)
		{
			g_outputDir = ".";
		}
		else
		{
			g_outputDir = argv[ argi++];
			thisCommandLine.append( strus::string_format( " \"%s\"", g_outputDir.c_str()));
		}
		int ai = 0;
		while (ai != MaxNofArguments && argi != argc)
		{
			arguments[ ai++] = argv[ argi++];
		}
		arguments[ ai] = NULL;

		if (argi != argc)
		{
			throw std::runtime_error( _TXT("too many arguments"));
		}
		if (!g_serviceProgramCmdLine.empty())
		{
			mi = 0, me = modi;
			for (; mi < me; ++mi)
			{
				g_serviceProgramCmdLine.append( " -m \"");
				g_serviceProgramCmdLine.append( modpath[ mi]);
				g_serviceProgramCmdLine.append( "\"");
			}
			if (g_verbosity > 0)
			{
				std::cerr << "START " << thisCommandLine << std::endl;

			}
		}
		g_logger.init( g_verbosity);

		if (g_verbosity) fprintf( stderr, _TXT("create lua state\n"));
		if (!g_serviceProgramCmdLine.empty())
		{
			g_blockingCurlClient = new strus::BlockingCurlClient();
		}
		/* Define the lua state: */
		ls = luaL_newstate();
		luaL_openlibs( ls);
		luaopen_strus( ls);

		/* Set module directories: */
		setLuaPathParentDir( ls, inputfile);
		for (mi=0,me=modi; mi != me; ++mi)
		{
			setLuaPath( ls, modpath[ mi]);
		}
		setLuaPath( ls, g_outputDir.c_str());
		declareFunctions( ls);
		declareArguments( ls, arguments);

		/* Load the script: */
		if (g_verbosity) fprintf( stderr, _TXT("load script file: '%s'\n"), inputfile);

		errcode = luaL_loadfile( ls, inputfile);
		if (errcode != LUA_OK)
		{
			handleLuaScriptError( ls, errcode, inputfile, _TXT("loading script"));
			lua_close( ls);
			rt = -2;
			goto EXIT;
		}
		if (g_verbosity) fprintf( stderr, _TXT("starting script ...\n"));

		/* Run the script: */
		errcode = lua_pcall( ls, 0, LUA_MULTRET, 0);
		if (errcode != LUA_OK)
		{
			handleLuaScriptError( ls, errcode, inputfile, _TXT("executing script"));
			lua_close( ls);
			rt = -3;
			goto EXIT;
		}
		lua_close( ls);

		if (g_errorhnd)
		{
			if (g_errorhnd->hasError())
			{
				throw strus::runtime_error( _TXT("error in strus context: %s"), g_errorhnd->fetchError());
			}
		}
		goto EXIT;
	}
	catch (const std::bad_alloc&)
	{
		fprintf( stderr, _TXT( "out of memory\n"));
		rt = -1;
		goto EXIT;
	}
	catch (const std::runtime_error& err)
	{
		fprintf( stderr, _TXT( "runtime error: %s\n"), err.what());
		rt = -1;
		goto EXIT;
	}
	catch (const std::exception& err)
	{
		fprintf( stderr, _TXT( "uncaught exception: %s\n"), err.what());
		rt = -1;
		goto EXIT;
	}
EXIT:
	printDebugTraceMessages();
	if (g_killChildOnExit)
	{
		killBackGroundProcesses();
	}
	else
	{
		listBackGroundProcesses();
	}
	if (g_blockingCurlClient) delete g_blockingCurlClient;
	if (g_globalContext) delete g_globalContext;
	if (g_errorhnd) delete g_errorhnd;
	return rt;
}

