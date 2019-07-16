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
#include "strus/base/string_format.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/shared_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/numstring.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/stdint.h"
#include "private/internationalization.hpp"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "strus/lib/lua.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <limits>
#include <map>
#include <cstdbool>
#include <iostream>

#undef STRUS_LOWLEVEL_DEBUG

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
			case 3: m_logmask |= (LogMethodCalls|LogAction);
			case 2: m_logmask |= (LogRequests|LogConfiguration);
			case 1: m_logmask |= (LogError|LogWarning);
			case 0: m_logmask |= (LogNothing); break;
			default: m_logmask = LogAll;
		}
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
	virtual void logRequest( const char* reqstr)
	{
		std::cerr << header() << strus::string_format( "REQUEST %s", reqstr) << std::endl;
	}

	virtual void logPutConfiguration( const char* type, const char* name, const std::string& configstr)
	{
		std::cerr << header() << strus::string_format( "CONFIG %s '%s':\n\t", type, name) << indentString( configstr) << std::endl;
	}

	virtual void logAction( const char* type, const char* name, const char* action)
	{
		std::cerr << header() << strus::string_format( "ACTION %s '%s' %s", type, name, action) << std::endl;
	}

	virtual void logMethodCall(
			const std::string& classname,
			const std::string& methodname,
			const std::string& arguments,
			const std::string& result)
	{
		std::cerr << header() << strus::string_format( "CALL %s %s (%s) -> %s", classname.c_str(), methodname.c_str(), arguments.c_str(), result.c_str()) << std::endl;
	}

	virtual void logWarning( const char* warnmsg)
	{
		std::cerr << header() << strus::string_format( "WARNING %s", warnmsg) << std::endl;
	}

	virtual void logError( const char* errmsg)
	{
		std::cerr << header() << strus::string_format( "ERROR %s", errmsg) << std::endl;
	}

	
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

private:
	int m_loglevel;
	int m_logmask;
	std::string m_procname;
};

static bool g_verbose = false;
static Logger g_logger;
static const char* g_charset = "UTF-8";
static const char* g_doctype = "application/json";

typedef std::map<std::string,std::string> Configuration;


class Processor
{
public:
	Processor( const std::string& hostname_, const Configuration& config_, const std::string& configjson_)
		:m_hostname(hostname_),m_config(config_),m_handler(createWebRequestHandler(config_,configjson_)){}
	Processor( const Processor& o)
		:m_hostname(o.m_hostname),m_handler(o.m_handler){}

	std::pair<strus::WebRequestAnswer,std::string> call( const std::string& method, const std::string& path, const strus::WebRequestContent& content);

private:
	static strus::WebRequestHandlerInterface* createWebRequestHandler( const Configuration& config_, const std::string& configjson_);

private:
	std::string m_hostname;
	Configuration m_config;
	strus::shared_ptr<strus::WebRequestHandlerInterface> m_handler;
};

class GlobalContext
{
public:
	GlobalContext(){}

	void defineServer( const std::string& hostname, const Configuration& configmap, const std::string& configjson);
	std::pair<strus::WebRequestAnswer,std::string> call( const std::string& method, const std::string& url, const std::string& content);
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

	virtual bool send( const std::string& address,
			const std::string& method,
			const std::string& content,
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

class WebRequestDelegateContext
	:public strus::WebRequestDelegateContextInterface
{
public:
	WebRequestDelegateContext(
			strus::WebRequestAnswer* answer_,
			const strus::Reference<strus::WebRequestContextInterface>& requestContext_,
			const std::string& url_,
			const std::string& schema_)
		:m_requestContext(requestContext_)
		,m_url(url_),m_schema(schema_)
		,m_answer(answer_){}

	virtual ~WebRequestDelegateContext(){}
	virtual void putAnswer( const strus::WebRequestAnswer& status);

private:
	strus::Reference<strus::WebRequestContextInterface> m_requestContext;
	std::string m_url;
	std::string m_schema;
	strus::WebRequestAnswer* m_answer;
};


static EventLoop g_eventLoop;
static GlobalContext g_globalContext;
static std::string g_scriptDir;
static strus::DebugTraceInterface* g_debugTrace = 0;
static strus::ErrorBufferInterface* g_errorhnd = 0;

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

static int64_t getConfigurationValue( const Configuration& config, const std::string& key, const int& defaultValue)
{
	Configuration::const_iterator ci = config.find( key);
	if (ci == config.end()) return defaultValue;
	return strus::numstring_conv::toint( ci->second, std::numeric_limits<int64_t>::max());
}

static std::string getConfigurationValue( const Configuration& config, const std::string& key, const char* defaultValue)
{
	Configuration::const_iterator ci = config.find( key);
	if (ci == config.end()) return defaultValue;
	return ci->second;
}


void EventLoop::initConfiguration( const Configuration& config)
{
	int max_idle_time = getConfigurationValue( config, "transactions.max_idle_time", 600);
	m_timerEventSecondsPeriod = std::max( 10, max_idle_time/20);
}
		
bool EventLoop::send( const std::string& address,
		const std::string& method,
		const std::string& contentstr,
		strus::WebRequestDelegateContextInterface* receiver)
{
	try
	{
		std::pair<strus::WebRequestAnswer,std::string> result = g_globalContext.call( method, address, contentstr);
		receiver->putAnswer( result.first);
		return true;
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}
}

void WebRequestDelegateContext::putAnswer( const strus::WebRequestAnswer& status)
{
	unsigned int rc = m_requestContext.refcnt();
	bool success = true;
	if (!m_answer->ok()) return;

	if (status.ok() && status.httpstatus() >= 200 && status.httpstatus() < 300)
	{
		if (!m_requestContext->returnDelegateRequestAnswer( m_schema.c_str(), status.content(), *m_answer))
		{
			success = false;
		}
	}
	else
	{
		*m_answer = status;
		success = false;
	}
	if (success)
	{
		if (rc <= 1)
		{
			//... last delegate request reply
			*m_answer = m_requestContext->getRequestAnswer();
		}
	}
	else
	{
		std::string msg( strus::string_format( _TXT( "delegate request to %s failed"), m_url.c_str()));
		m_answer->explain( msg.c_str());
	}
	m_requestContext.reset();
}

strus::WebRequestHandlerInterface* Processor::createWebRequestHandler( const Configuration& config_, const std::string& configjson_)
{
	strus::WebRequestHandlerInterface* rt =
		strus::createWebRequestHandler(
			&g_eventLoop, &g_logger, ""/*html head*/,
			getConfigurationValue( config_, "data.configdir", "./"),
			configjson_,
			getConfigurationValue( config_, "transactions.max_idle_time", 600),
			getConfigurationValue( config_, "transactions.nof_per_sec", 60),
			g_errorhnd);
	if (!rt) throw strus::runtime_error( _TXT("error creating web request handler: %s"), g_errorhnd->fetchError());
	return rt;
}

std::pair<strus::WebRequestAnswer,std::string> Processor::call( const std::string& method, const std::string& path, const strus::WebRequestContent& content)
{
	std::pair<strus::WebRequestAnswer,std::string> rt;
	std::string html_base_href =
		path.empty()
			? strus::string_format( "http://%s/", m_hostname.c_str())
			: strus::string_format( "http://%s/%s/", m_hostname.c_str(), path.c_str());

	strus::Reference<strus::WebRequestContextInterface> ctx( m_handler->createContext( 
				g_charset, g_doctype, html_base_href.c_str(), rt.first));
	if (!ctx.get()) return rt;

	std::vector<strus::WebRequestDelegateRequest> delegateRequests;
	if (!ctx->executeRequest( method.c_str(), path.c_str(), content, delegateRequests))
	{
		rt.first = ctx->getRequestAnswer();
	}
	else if (delegateRequests.empty())
	{
		rt.first = ctx->getRequestAnswer();
	}
	else
	{
		std::vector<strus::Reference<strus::WebRequestDelegateContextInterface> > receivers;
		std::vector<strus::WebRequestDelegateRequest>::const_iterator di = delegateRequests.begin(), de = delegateRequests.end();
		for (; di != de; ++di)
		{
			strus::Reference<strus::WebRequestDelegateContextInterface> receiver(
				new WebRequestDelegateContext( &rt.first, ctx, di->url(), di->schema()));
			receivers.push_back( receiver);
		}
		di = delegateRequests.begin();
		for (int didx=0; di != de; ++di,++didx)
		{
			std::string delegateContent( di->contentstr(), di->contentlen());

			strus::Reference<strus::WebRequestDelegateContextInterface>& receiver = receivers[ didx];
			if (!m_handler->delegateRequest( di->url(), di->method(), delegateContent, receiver.release()))
			{
				g_globalContext.reportError( _TXT("delegate request failed, out of memory"));
				break;
			}
		}
	}
	if (!rt.first.content().empty())
	{
		rt.second.append( rt.first.content().str(), rt.first.content().len());
		rt.first.content().setContent( rt.second.c_str(), rt.second.size());
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

std::pair<strus::WebRequestAnswer,std::string> GlobalContext::call( const std::string& method, const std::string& url, const std::string& contentstr)
{
	std::pair<std::string,std::string> serverPathPair = splitUrl( url);
	const std::string& proc = serverPathPair.first;
	const std::string& path = serverPathPair.second;
	strus::WebRequestContent content( g_charset, g_doctype, contentstr.c_str(), contentstr.size());

	ProcMap::iterator pi = m_procMap.find( proc);
	if (pi == m_procMap.end())
	{
		const char* errmsg = strus::errorCodeToString( strus::ErrorCodeNotFound);
		strus::WebRequestAnswer answer( errmsg, 404, strus::ErrorCodeNotFound, strus::WebRequestContent());
		return std::pair<strus::WebRequestAnswer,std::string>( answer, std::string());
	}
	else
	{
		return pi->second.call( method, path, content);
	}
}

void GlobalContext::reportError( const char* fmt, ...)
{
	va_list ap;
	va_start( ap, fmt);

	std::string msg = strus::string_format_va( fmt, ap);
	std::cerr << "ERROR " << msg << std::endl;
}

static void convertConfig_( lua_State *L, int luaddr, Configuration& config, const std::string& prefix)
{
	lua_pushvalue( L, luaddr);
	lua_pushnil( L );

	while (lua_next( L, -2) != 0)
	{
		std::string key;
		if (lua_isnumber( L, -2))
		{
			lua_pushvalue( L, -2);
			key = lua_tostring( L, -1);
			lua_pop( L, 1);
		}
		else if (lua_isstring( L, -2))
		{
			key = lua_tostring( L, -2);
		}
		else {
			luaL_error( L, _TXT("non string type keys are not supported in configuration definition"));
		}
		std::string fullkey = prefix.empty() ? key : (prefix + "." + key);

		if (lua_isstring( L, -1))
		{
			config[ fullkey] = lua_tostring( L, -1);
		}
		else if (lua_istable( L, -1))
		{
			convertConfig_( L, -1, config, fullkey);
		}
		else
		{
			lua_pushvalue( L, -1);
			config[ fullkey] = lua_tostring( L, -1);
			lua_pop( L, 1);
		}
		lua_pop( L, 1 );
	}
	lua_pop( L, 1); // Get luaddr-value from stack
}

enum TableType {TableTypeEmpty,TableTypeArray,TableTypeDictionary};
static TableType getTableType( lua_State *L, int luaddr)
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

static void convertConfigToJson_( lua_State *L, int luaddr, std::string& configstr, bool array)
{
	lua_pushvalue( L, luaddr);
	lua_pushnil( L );
	int itercnt = -1;

	while (lua_next( L, -2) != 0)
	{
		const char* key = 0;
		++itercnt;

		if (lua_isnumber( L, -2))
		{
			if (!array) luaL_error( L, _TXT("mixing array and table"));
		}
		else if (lua_isstring( L, -2))
		{
			if (array) luaL_error( L, _TXT("mixing array and table"));
			key = lua_tostring( L, -2);
		}
		else {
			luaL_error( L, _TXT("non string type keys are not supported in configuration definition"));
		}
		std::size_t elementStartPos = configstr.size();
		if (itercnt)
		{
			configstr.append( ", ");
		}
		if (key)
		{
			configstr.append( "\"");
			configstr.append( key);
			configstr.append( "\": ");
		}
		if (lua_isnil( L, -1))
		{
			configstr.resize( elementStartPos);
			--itercnt;
		}
		else if (lua_isstring( L, -1))
		{
			configstr.push_back( '"');
			configstr.append( lua_tostring( L, -1));
			configstr.push_back( '"');
		}
		else if (lua_istable( L, -1))
		{
			switch (getTableType( L, -1))
			{
				case TableTypeEmpty:
					configstr.resize( elementStartPos);
					--itercnt;
					break;
				case TableTypeArray:
					configstr.append( "[ ");
					convertConfigToJson_( L, -1, configstr, true/*array*/);
					configstr.append( " ]");
					break;
				case TableTypeDictionary:
					configstr.append( "{ ");
					convertConfigToJson_( L, -1, configstr, false/*!array*/);
					configstr.append( " }");
					break;
			}
		}
		else
		{
			lua_pushvalue( L, -1);
			configstr.append( lua_tostring( L, -1));
			lua_pop( L, 1);
		}
		lua_pop( L, 1 );
	}
	lua_pop( L, 1); // Get luaddr-value from stack
}

static Configuration convertConfig( lua_State *L, int luaddr)
{
	Configuration rt;
	convertConfig_( L, luaddr, rt, std::string());
	return rt;
}

static std::string convertConfigToJson( lua_State *L, int luaddr)
{
	std::string rt( "{");
	convertConfigToJson_( L, luaddr, rt, false/*!array*/);
	rt.append( "}");
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

static int l_set_time( lua_State *L)
{
	try
	{
		int nofArgs = lua_gettop(L);
		if (nofArgs > 1) return luaL_error(L, _TXT("too many arguments for 'set_time': expected <seconds>"));
		if (nofArgs < 1) return luaL_error(L, _TXT("too few arguments for 'set_time': expected <seconds>"));
		long newtime = lua_tointeger( L, 1);
		g_eventLoop.setTime( newtime);
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
	}
	return 0;
}

static int l_def_server( lua_State *L)
{
	try
	{
		int nofArgs = lua_gettop(L);
		if (nofArgs > 2) return luaL_error(L, _TXT("too many arguments for 'def_server': expected <host> <config>"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'def_server': expected <host> <config>"));
		const char* hostname = lua_tostring( L, 1);
		Configuration configmap = convertConfig( L, 2);
		std::string configjson = convertConfigToJson( L, 2);

		g_globalContext.defineServer( hostname, configmap, configjson);
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
	}
	return 0;
}

static int l_call_server( lua_State *L)
{
	try
	{
		int nofArgs = lua_gettop(L);
		if (nofArgs > 3) return luaL_error(L, _TXT("too many arguments for 'call_server': expected <method> <addr> [<arg>]"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'call_server': expected <method> <addr> [<arg>]"));
		const char* method = lua_tostring( L, 1);
		const char* url = lua_tostring( L, 2);
		std::size_t arglen = 0;
		const char* arg = nofArgs < 3 || lua_isnil( L, 3) ? 0 : lua_tolstring( L, 3, &arglen);

		std::string content = getContentArgumentValue( arg);
		std::pair<strus::WebRequestAnswer,std::string> result = g_globalContext.call( method, url, content.c_str());
		if (result.second.empty())
		{
			lua_pushnil(L);
		}
		else
		{
			lua_pushlstring(L, result.second.c_str(), result.second.size()); 
		}
		lua_pushinteger(L, result.first.httpstatus()); /* second return value */
		return 2;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static void push_linestring( lua_State *L, char const* ln)
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

static int l_cmp_content( lua_State *L)
{
	try
	{
		int linecnt = 1;
		int nofArgs = lua_gettop(L);
		if (nofArgs > 2) return luaL_error(L, _TXT("too many arguments for 'cmp_content': expected <result> <expected>"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'cmp_content': expected <result> <expected>"));
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
			return 1;
		}
		else
		{
			lua_pushboolean( L, false);
			lua_pushinteger( L, linecnt);
			push_linestring( L, a_ln);
			push_linestring( L, b_ln);
			return 4;
		}
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static int l_write_textfile( lua_State *L)
{
	try
	{
		int nofArgs = lua_gettop(L);
		if (nofArgs > 2) return luaL_error(L, _TXT("too many arguments for 'write_textfile': expected <result> <expected>"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'write_textfile': expected <result> <expected>"));
		const char* filename = lua_tostring( L, 1);
		const char* content = lua_tostring( L, 2);

		std::string fullpath = strus::joinFilePath( g_scriptDir, filename);
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
				normcontent.push_back( '\n');
				++ci;
			}
		}
		int ec = strus::writeFile( fullpath, normcontent);
		if (ec)
		{
			luaL_error( L, _TXT("failed to write text file (errno %d): %s"), ec, ::strerror(ec));
		}
		return 0;
	}
	catch (const std::exception& err)
	{
		luaL_error( L, "%s", err.what());
		return 0;
	}
}

static void declareFunctions( lua_State *L)
{
#define DEFINE_FUNCTION( NAME)	lua_pushcfunction( L, l_ ##NAME); lua_setglobal( L, #NAME);

	DEFINE_FUNCTION( set_time );
	DEFINE_FUNCTION( def_server );
	DEFINE_FUNCTION( call_server );
	DEFINE_FUNCTION( cmp_content );
	DEFINE_FUNCTION( write_textfile );

	lua_pushboolean( L, g_verbose);
	lua_setglobal( L, "verbose");
}

static void printUsage()
{
	fprintf( stderr, "%s",
		 "strusWebRequestLuaSim [<options>] <luascript>\n"
		"<options>:\n" 
		 "   -h,--help          :Print this usage\n"
		 "   -m,--mod <PATH>    :Set <PATH> as addidional module path\n"
		 "   -V                 :Raise verbosity level (may be repeated\n"
		"<luascript>            :Lua script to execute\n");
}

static void setLuaPath( lua_State* ls, const char* path)
{
	const char* cur_path;
#ifdef STRUS_LOWLEVEL_DEBUG
	fprintf( stderr, "set lua module path to: '%s'\n", path);
#endif
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
#ifdef STRUS_LOWLEVEL_DEBUG
	fprintf( stderr, "set lua module pattern to: '%s'\n", pathbuf);
#endif
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
	const char* inputfile = 0;
	lua_State* ls = 0;
	int argi = 1;
	int errcode = 0;
	int modi = 0;
	int mi,me;
	int ai,ae;
	int verbosity = 0;
	const char* modpath[ MaxModPaths];

	try
	{
		g_debugTrace = strus::createDebugTrace_standard( 2);
		g_errorhnd = strus::createErrorBuffer_standard( NULL, 2, g_debugTrace);
		if (!g_debugTrace || !g_errorhnd)
		{
			fprintf( stderr, _TXT( "out of memory creating error handler\n"));
			return -1;
		}
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
				return 0;
			}
			else if (0==strcmp( argv[argi], "-V") || 0==strcmp( argv[argi], "-VV") || 0==strcmp( argv[argi], "-VVV") || 0==strcmp( argv[argi], "-VVVV"))
			{
				verbosity += std::strlen(argv[argi])-1;
				g_verbose = true;
			}
			else if (0==strcmp( argv[argi], "--debug") || 0==strcmp( argv[argi], "-G"))
			{
				++argi;
				if (argi == argc || argv[argi][0] == '-')
				{
					fprintf( stderr, _TXT("option -G (--debug) needs argument\n"));
					return 1;
				}
				g_debugTrace->enable( argv[argi]);
			}
			else if (0==strcmp( argv[argi], "--mod") || 0==strcmp( argv[argi], "-m"))
			{
				++argi;
				if (argi == argc || argv[argi][0] == '-')
				{
					fprintf( stderr, _TXT("option -m (--mod) needs argument\n"));
					return 1;
				}
				if (modi >= MaxModPaths)
				{
					fprintf( stderr, _TXT("too many options -m (--mod) specified in argument list\n"));
				}
				modpath[ modi++] = argv[argi];
			}
			else if (0==strcmp( argv[argi], "--"))
			{
				++argi;
				break;
			}
		}
		if (argi == argc)
		{
			fprintf( stderr, _TXT("too few arguments (less than one)\n"));
			return -1;
		}
		inputfile = argv[ argi];
		int ec = strus::getParentPath( inputfile, g_scriptDir);
		if (ec)
		{
			fprintf( stderr, _TXT("failed to get directory of the script to execute\n"));
			return -1;
		}
		g_logger.init( verbosity);
	
#ifdef STRUS_LOWLEVEL_DEBUG
		fprintf( stderr, "create lua state\n");
#endif
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
		/* Define program arguments for lua script: */
		lua_newtable( ls);
		for (ai=0,ae=argc-argi; ai != ae; ++ai)
		{
			lua_pushinteger( ls, ai);
			lua_pushstring( ls, argv[argi+ai]);
			lua_rawset( ls, -3);
		}
		lua_setglobal( ls, "arg");
		declareFunctions( ls);
	
		/* Load the script: */
#ifdef STRUS_LOWLEVEL_DEBUG
		fprintf( stderr, "load script file: '%s'\n", inputfile);
#endif
		errcode = luaL_loadfile( ls, inputfile);
		if (errcode != LUA_OK)
		{
			handleLuaScriptError( ls, errcode, inputfile, _TXT("loading script"));
			lua_close( ls);
			if (g_errorhnd) delete g_errorhnd;
			return -2;
		}
#ifdef STRUS_LOWLEVEL_DEBUG
		fprintf( stderr, "starting ...\n");
#endif
		/* Run the script: */
		errcode = lua_pcall( ls, 0, LUA_MULTRET, 0);
		if (errcode != LUA_OK)
		{
			handleLuaScriptError( ls, errcode, inputfile, _TXT("executing script"));
			lua_close( ls);
			if (g_errorhnd) delete g_errorhnd;
			return -3;
		}
		lua_close( ls);
		printDebugTraceMessages();

		if (g_errorhnd)
		{
			if (g_errorhnd->hasError())
			{
				fprintf( stderr, _TXT("error in strus context: %s\n"), g_errorhnd->fetchError());
				return -4;
			}
			delete g_errorhnd;
		}
		return 0;
	}
	catch (const std::bad_alloc&)
	{
		if (g_errorhnd) delete g_errorhnd;
		fprintf( stderr, _TXT( "out of memory\n"));
		return -1;
	}
	catch (const std::runtime_error& err)
	{
		if (g_errorhnd) delete g_errorhnd;
		fprintf( stderr, _TXT( "runtime error: %s\n"), err.what());
		return -1;
	}
	catch (const std::exception& err)
	{
		if (g_errorhnd) delete g_errorhnd;
		fprintf( stderr, _TXT( "uncaught exception: %s\n"), err.what());
		return -1;
	}
}

