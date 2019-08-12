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
#include "strus/base/regex.hpp"
#include "private/internationalization.hpp"
#include "papuga/lib/lua_dev.h"
#include "papuga/serialization.h"
#include "papuga/valueVariant.h"
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
#include <string>
#include <vector>
#include <limits>
#include <map>
#include <cstdbool>
#include <iostream>

#undef STRUS_LOWLEVEL_DEBUG

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
			case 4: m_logmask |= (LogContentEvents);
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

	virtual void logContentEvent( const char* title, const char* item, const char* value)
	{
		if (item && item[0])
		{
			if (value && value[0])
			{
				std::cerr << header() << strus::string_format( "PARSE %s %s '%s'", title, item, value) << std::endl;
			}
			else
			{
				std::cerr << header() << strus::string_format( "PARSE %s %s", title, item) << std::endl;
			}
		}
		else
		{
			if (value && value[0])
			{
				std::cerr << header() << strus::string_format( "PARSE %s '%s'", title, value) << std::endl;
			}
			else
			{
				std::cerr << header() << strus::string_format( "PARSE %s", title) << std::endl;
			}
		}
	}

	virtual void logMethodCall(
			const char* classname,
			const char* methodname,
			const char* arguments,
			const char* result,
			const char* resultvar)
	{
		std::string assignment;
		const char* resultStatement = result;
		if (resultvar && resultvar[0])
		{
			assignment = strus::string_format( "%s := %s", resultvar, result?result:"NULL");
			resultStatement = assignment.c_str();
		}
		if (!classname || !classname[0])
		{
			// Variable assignment
			std::cerr << header() << strus::string_format( "ASSIGN %s", resultStatement) << std::endl;
		}
		else if (!methodname || !methodname[0])
		{
			// Constructor call
			if (!resultStatement || !resultStatement[0])
			{
				std::cerr << header() << strus::string_format( "NEW %s (%s)", classname, arguments?arguments:"") << std::endl;
			}
			else
			{
				std::cerr << header() << strus::string_format( "NEW %s (%s) -> %s", classname, arguments?arguments:"", resultStatement) << std::endl;
			}
		}
		else
		{
			// Method call
			if (!resultStatement || !resultStatement[0])
			{
				std::cerr << header() << strus::string_format( "CALL %s::%s (%s)", classname, methodname, arguments?arguments:"") << std::endl;
			}
			else
			{
				std::cerr << header() << strus::string_format( "CALL %s::%s (%s) -> %s", classname, methodname, arguments?arguments:"", resultStatement) << std::endl;
			}
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
template <>
double castLuaValue<double>( lua_State* ls, int luaddr)
{
	return lua_tonumber( ls, luaddr);
}
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

#if 0
static void logValue( lua_State* ls, int luaddr, const char* title)
{
	lua_pushvalue( ls, luaddr);
	const char* val = 0;
	const char* typestr = "UNKNOWN";

	switch (lua_type( ls, -1))
	{
		case LUA_TNIL: typestr = "NIL"; break;
		case LUA_TNUMBER: typestr = "NUMBER"; val = lua_tostring( ls, -1); break;
		case LUA_TBOOLEAN: typestr = "BOOLEAN"; val = lua_tostring( ls, -1); break;
		case LUA_TSTRING: typestr = "STRING"; val = lua_tostring( ls, -1); break;
		case LUA_TTABLE: typestr = "TABLE"; break;
		case LUA_TFUNCTION: typestr = "FUNCTION"; break;
		case LUA_TUSERDATA: typestr = "USERDATA"; break;
		case LUA_TTHREAD: typestr = "THREAD"; break;
		case LUA_TLIGHTUSERDATA: typestr = "LIGHTUSERDATA"; break;
	}
	if (val)
	{
		fprintf( stderr, "LOG [%d] %s type %s value '%s'\n", luaddr, title, typestr, val);
	}
	else
	{
		fprintf( stderr, "LOG [%d] %s type %s\n", luaddr, title, typestr);
	}
	lua_pop( ls, 1);
}
#endif

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

	std::pair<strus::WebRequestAnswer,std::string> call( const std::string& method, const std::string& path, const strus::WebRequestContent& content);

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
static std::string g_outputDir;
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

void EventLoop::initConfiguration( const Configuration& config)
{
	int max_idle_time = 600;
	(void)config.getValue( max_idle_time, "transactions.max_idle_time");
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

strus::WebRequestHandlerInterface* Processor::createWebRequestHandler( const Configuration& config, const std::string& configjson_)
{
	std::string configdir = "./";
	std::string servicename = "test";
	int transaction_max_idle_time = 600;
	int transaction_nof_per_sec = 60;

	(void)config.getValue( configdir, "data.configdir");
	(void)config.getValue( servicename, "service.name");
	(void)config.getValue( transaction_max_idle_time, "transactions.max_idle_time");
	(void)config.getValue( transaction_nof_per_sec, "transactions.nof_per_sec");

	strus::WebRequestHandlerInterface* rt =
		strus::createWebRequestHandler(
			&g_eventLoop, &g_logger, ""/*html head*/,
			configdir, servicename, configjson_, transaction_max_idle_time, transaction_nof_per_sec,
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

static void convertLuaArrayToJson_( lua_State* L, int luaddr, std::string& configstr, int indent);
static void convertLuaDictionaryToJson_( lua_State* L, int luaddr, std::string& configstr, int indent);

static bool convertLuaValueToJson_( lua_State* L, int luaddr, std::string& configstr, int indent=0)
{
	if (lua_isnil( L, luaddr))
	{
		return false;
	}
	else if (lua_isstring( L, luaddr))
	{
		configstr.push_back( '"');
		configstr.append( lua_tostring( L, luaddr));
		configstr.push_back( '"');
	}
	else if (lua_istable( L, luaddr))
	{
		switch (getTableType( L, luaddr))
		{
			case TableTypeEmpty:
				return false;
			case TableTypeArray:
				configstr.append( "[ ");
				convertLuaArrayToJson_( L, luaddr, configstr, indent);
				configstr.append( " ]");
				break;
			case TableTypeDictionary:
				configstr.append( "{ ");
				convertLuaDictionaryToJson_( L, luaddr, configstr, indent);
				configstr.append( " }");
				break;
		}
	}
	else
	{
		configstr.append( lua_tostring( L, luaddr));
	}
	return true;
}

static void convertLuaArrayToJson_( lua_State* L, int luaddr, std::string& configstr, int indent)
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
			configstr.append( ",");
		}
		configstr.append( "\n");
		configstr.resize( configstr.size()+(indent*2), ' ');

		if (!convertLuaValueToJson_( L, -1, configstr, indent+1))
		{
			configstr.append( "null");
		}
		lua_pop( L, 1 );
	}
	lua_pop( L, 1); // Get luaddr-value from stack
}

static void convertLuaDictionaryToJson_( lua_State* L, int luaddr, std::string& configstr, int indent)
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
		if (convertLuaValueToJson_( L, -1, value, indent+1))
		{
			dictmap[ key] = value;
		}
		lua_pop( L, 1 );
	}
	lua_pop( L, 1); // Get luaddr-value from stack

	std::map<CString,std::string>::const_iterator di = dictmap.begin(), de = dictmap.end();
	for (int didx=0; di != de; ++di,++didx)
	{
		if (didx) configstr.append( ",");
		configstr.append( "\n");
		configstr.resize( configstr.size()+(indent*2), ' ');
		configstr.append( "\"");
		configstr.append( di->first.value);
		configstr.append( "\": ");
		configstr.append( di->second);
	}
}

static std::string convertLuaValueToJson( lua_State* L, int luaddr)
{
	std::string rt;
	convertLuaValueToJson_( L, luaddr, rt);
	return rt;
}

static void pushConvertedJsonAsLuaTable( lua_State* L, int luaddr)
{
	if (!lua_isstring( L, luaddr))
	{
		throw strus::runtime_error( _TXT("error converting JSON to lua table: %s"), _TXT("argument is not a string"));
	}
	std::size_t srclen;
	const char* src = lua_tolstring( L, luaddr, &srclen);
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
	papuga_lua_push_value_plain( L, &jsonval);
	papuga_destroy_Allocator( &allocator);
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

static int l_set_time( lua_State* L)
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

static int l_def_server( lua_State* L)
{
	try
	{
		int nofArgs = lua_gettop(L);
		if (nofArgs > 2) return luaL_error(L, _TXT("too many arguments for 'def_server': expected <host> <config>"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'def_server': expected <host> <config>"));
		const char* hostname = lua_tostring( L, 1);
		Configuration configmap( L, 2);
		std::string configjson = convertLuaValueToJson( L, 2);

		g_globalContext.defineServer( hostname, configmap, configjson);
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
		int nofArgs = lua_gettop(L);
		if (nofArgs > 3) return luaL_error(L, _TXT("too many arguments for 'call_server': expected <method> <addr> [<arg>]"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'call_server': expected <method> <addr> [<arg>]"));
		const char* method = lua_tostring( L, 1);
		const char* url = lua_tostring( L, 2);
		const char* arg = "";
		std::string content;
		if (nofArgs == 3 && !lua_isnil( L, 3))
		{
			if (lua_isstring( L, 3))
			{
				arg = lua_tostring( L, 3);
				content = getContentArgumentValue( arg);
				arg = content.c_str();
			}
			else if (lua_istable( L, 3))
			{
				content = convertLuaValueToJson( L, 3);
				arg = content.c_str();
			}
			else
			{
				luaL_error(L, _TXT("table or string expected as 3rd argument 'call_server'"));
			}
		}
		std::pair<strus::WebRequestAnswer,std::string> result = g_globalContext.call( method, url, arg);
		if (result.second.empty())
		{
			lua_pushnil(L);
			lua_pushinteger(L, result.first.httpstatus()); /* second return value */
			lua_pushstring(L, result.first.errorstr()); 
		}
		else
		{
			lua_pushlstring(L, result.second.c_str(), result.second.size()); 
			lua_pushinteger(L, result.first.httpstatus()); /* second return value */
			lua_pushnil(L);
		}
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

static int l_load_file( lua_State* L)
{
	try
	{
		int nofArgs = lua_gettop(L);
		if (nofArgs > 1) return luaL_error(L, _TXT("too many arguments for 'load_file': expected <filename>"));
		if (nofArgs < 1) return luaL_error(L, _TXT("too few arguments for 'load_file': expected <filename>"));
		const char* filename = lua_tostring( L, 1);
		std::string content;
		std::string fullpath = strus::joinFilePath( g_scriptDir, filename);
		int ec = strus::readFile( fullpath, content);
		if (ec) throw strus::runtime_error( _TXT("error (%d) reading file %s with content to process by server: %s"), ec, fullpath.c_str(), ::strerror(ec));
		lua_pushlstring(L, content.c_str(), content.size());
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
		int nofArgs = lua_gettop(L);
		if (nofArgs > 2) return luaL_error(L, _TXT("too many arguments for 'write_file': expected <filename> <content>"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'write_file': expected <filename> <content>"));
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
		int nofArgs = lua_gettop(L);
		if (nofArgs > 1) return luaL_error(L, _TXT("too many arguments for 'remove_file': expected <filename>"));
		if (nofArgs < 1) return luaL_error(L, _TXT("too few arguments for 'remove_file': expected <filename>"));
		const char* filename = lua_tostring( L, 1);

		std::string fullpath = strus::joinFilePath( g_outputDir, filename);
		int ec = strus::removeFile( fullpath, false/*fail_ifnofexist*/);
		if (ec)
		{
			luaL_error( L, _TXT("failed to remove file (errno %d): %s"), ec, ::strerror(ec));
		}
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
		int nofArgs = lua_gettop(L);
		if (nofArgs > 1) return luaL_error(L, _TXT("too many arguments for 'create_dir': expected <dirname>"));
		if (nofArgs < 1) return luaL_error(L, _TXT("too few arguments for 'create_dir': expected <dirname>"));
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
		int nofArgs = lua_gettop(L);
		if (nofArgs > 2) return luaL_error(L, _TXT("too many arguments for 'reformat_float': expected <content> <precision>"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'reformat_float': expected <content> <precision>"));
		const char* content = lua_tostring( L, 1);
		int precision = lua_tointeger( L, 2);
		if (precision <= 0) return luaL_error(L, _TXT("expected positive number for precision argument of 'reformat_float'"));

		std::string result = convertSourceReformatFloat( content, precision);
		lua_pushlstring( L, result.c_str(), result.size()); 
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
		int nofArgs = lua_gettop(L);
		if (nofArgs > 3) return luaL_error(L, _TXT("too many arguments for 'reformat_regex': expected <content> <regex> [<subst>]"));
		if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'reformat_regex': expected <content> <regex> [<subst>]"));
		const char* content = lua_tostring( L, 1);
		const char* expr = lua_tostring( L, 2);
		const char* substfmt = (nofArgs > 2) ? lua_tostring( L, 3) : "";

		std::string result = convertSourceRegexReplace( content, expr, substfmt);
		lua_pushlstring( L, result.c_str(), result.size()); 
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
		int nofArgs = lua_gettop(L);
		if (nofArgs > 1) return luaL_error(L, _TXT("too many arguments for 'tojson': expected <table>"));
		if (nofArgs < 1) return luaL_error(L, _TXT("too few arguments for 'tojson': expected <table>"));
		std::string result = convertLuaValueToJson( L, 1);
		lua_pushlstring( L, result.c_str(), result.size()); 
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
		int nofArgs = lua_gettop(L);
		if (nofArgs > 1) return luaL_error(L, _TXT("too many arguments for 'tojson': expected <table>"));
		if (nofArgs < 1) return luaL_error(L, _TXT("too few arguments for 'tojson': expected <table>"));
		pushConvertedJsonAsLuaTable( L, 1);
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
	lua_pushboolean( L, g_verbose);
	lua_setglobal( L, "verbose");
}

static void printUsage()
{
	fprintf( stderr, "%s",
		 "strusWebRequestLuaSim [<options>] <luascript> [<outputdir>]\n"
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
			else if (0==strcmp( argv[argi], "-V") || 0==strcmp( argv[argi], "-VV") || 0==strcmp( argv[argi], "-VVV") || 0==strcmp( argv[argi], "-VVVV") || 0==strcmp( argv[argi], "-VVVVV"))
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
		inputfile = argv[ argi++];
		int ec = strus::getParentPath( inputfile, g_scriptDir);
		if (ec)
		{
			fprintf( stderr, _TXT("failed to get directory of the script to execute\n"));
			return -1;
		}
		if (argi == argc)
		{
			g_outputDir = ".";
		}
		else
		{
			g_outputDir = argv[ argi++];
		}
		if (argi != argc)
		{
			fprintf( stderr, _TXT("too many arguments\n"));
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
		setLuaPath( ls, g_outputDir.c_str());
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

