/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/webRequestHandlerInterface.hpp"
#include "strus/webRequestContextInterface.hpp"
#include "strus/webRequestEventLoopInterface.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/shared_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "strus/lib/error.hpp"
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

static Logger g_logger;
static const char* g_charset = "UTF-8";
static const char* g_doctype = "application/json";


class Processor
{
public:
	explicit Processor( const std::string& hostname_)
		:m_hostname(hostname_),m_handler(){}
	Processor( const Processor& o)
		:m_hostname(o.m_hostname),m_handler(o.m_handler){}

	std::pair<strus::WebRequestAnswer,std::string> call( const std::string& method, const std::string& path, const strus::WebRequestContent& content);

private:
	std::string m_hostname;
	strus::shared_ptr<strus::WebRequestHandlerInterface> m_handler;
};

class GlobalContext
{
public:
	GlobalContext(){}

	std::pair<strus::WebRequestAnswer,std::string> call( const std::string& method, const std::string& url, const strus::WebRequestContent& content);

private:
	typedef std::map<std::string,Processor> ProcMap;
	ProcMap m_procMap;
};

class EventLoop
	:public strus::WebRequestEventLoopInterface
{
public:
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
};



static GlobalContext g_globalContext;
static EventLoop g_eventLoop;

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

bool EventLoop::send( const std::string& address,
		const std::string& method,
		const std::string& contentstr,
		strus::WebRequestDelegateContextInterface* receiver)
{
	try
	{
		strus::WebRequestContent content( g_charset, g_doctype, contentstr.c_str(), contentstr.size());
		std::pair<strus::WebRequestAnswer,std::string> result = g_globalContext.call( method, address, content);
		receiver->putAnswer( result.first);
		return true;
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}
}

std::pair<strus::WebRequestAnswer,std::string> Processor::call( const std::string& method, const std::string& path, const strus::WebRequestContent& content)
{
	std::pair<strus::WebRequestAnswer,std::string> rt;
	std::string html_base_href = strus::string_format( "http://%s/", m_hostname.c_str());

	strus::local_ptr<strus::WebRequestContextInterface> ctx( m_handler->createContext( 
				g_charset, g_doctype, html_base_href.c_str(), rt.first));
	if (!ctx.get()) return rt;

	std::vector<strus::WebRequestDelegateRequest> delegateRequests;
	if (!ctx->executeRequest( method.c_str(), path.c_str(), content, rt.first, delegateRequests)) return rt;

	if (delegateRequests.empty())
	{
		if (!rt.first.content().empty())
		{
			rt.second.append( rt.first.content().str(), rt.first.content().len());
			rt.first.content().setContent( rt.second.c_str(), rt.second.size());
		}
	}
	else
	{
		std::vector<strus::WebRequestDelegateRequest>::const_iterator di = delegateRequests.begin(), de = delegateRequests.end();
		for (; di != de; ++di)
		{
			std::pair<strus::WebRequestAnswer,std::string> delegateResult = g_globalContext.call( di->method(), di->url(), di->content());
			strus::WebRequestAnswer delegatePutAnswer;
			if (!ctx->returnDelegateRequestAnswer( di->schema(), delegateResult.first.content(), delegatePutAnswer))
			{
				std::string errexplanation = strus::string_format( _TXT("error delegating sub request to server '%s'"), m_hostname.c_str());
				delegatePutAnswer.explain( errexplanation.c_str());
				return std::pair<strus::WebRequestAnswer,std::string>( delegatePutAnswer, std::string());
			}
		}
	}
	return rt;
}

std::pair<strus::WebRequestAnswer,std::string> GlobalContext::call( const std::string& method, const std::string& url, const strus::WebRequestContent& content)
{
	std::pair<std::string,std::string> serverPathPair = splitUrl( url);
	const std::string& proc = serverPathPair.first;
	const std::string& path = serverPathPair.second;

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

static int l_call( lua_State *L) {
	const char* method = 0;
	const char* url = 0;
	const char* arg = "";
	std::size_t arglen = 0;
	int nofArgs = lua_gettop(L);
	if (nofArgs > 3) return luaL_error(L, _TXT("too many arguments for 'call': expected <method> <addr> [<arg>]"));
	if (nofArgs < 2) return luaL_error(L, _TXT("too few arguments for 'call': expected <method> <addr> [<arg>]"));
	switch (nofArgs)
	{
		case 3: arg = lua_tolstring( L, 3, &arglen);
		case 2: url = lua_tostring( L, 2);
		case 1: method = lua_tostring( L, 1);
	}
	strus::WebRequestContent content( g_charset, g_doctype, arg, arglen);
	std::pair<strus::WebRequestAnswer,std::string> result = g_globalContext.call( method, url, content);
	if (result.second.empty())
	{
		lua_pushinteger(L, result.first.httpstatus()); /* first return value */
		return 1;
	}
	else
	{
		lua_pushinteger(L, result.first.httpstatus()); /* first return value */
		lua_pushlstring(L, result.second.c_str(), result.second.size()); 
		return 2;
	}
}

static void declareFunctions( lua_State *L)
{
	lua_pushcfunction( L, l_call);
	lua_setglobal( L, "call");
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
		switch (errcode)
		{
			case LUA_ERRSYNTAX:
			{
				fprintf( stderr, _TXT("LUA_ERRSYNTAX %s loading script '%s'\n"), lua_tostring( ls, -1), inputfile);
				break;
			}
			case LUA_ERRMEM:
			{
				fprintf( stderr, _TXT("out of memory loading script '%s'\n"), inputfile);
				break;
			}
			case LUA_ERRFILE:
			{
				fprintf( stderr, _TXT("LUA_ERRFILE loading script '%s'\n"), inputfile);
				break;
			}
			default:
				fprintf( stderr, _TXT("unknown error loading script '%s'\n"), inputfile);
				break;
		}
		lua_close( ls);
		return -2;
	}
#ifdef STRUS_LOWLEVEL_DEBUG
	fprintf( stderr, "starting ...\n");
#endif
	/* Run the script: */
	errcode = lua_pcall( ls, 0, LUA_MULTRET, 0);
	if (errcode)
	{
		switch (errcode)
		{
			case LUA_OK:
				lua_close( ls);
				return 0;
			case LUA_ERRRUN:
				fprintf( stderr, _TXT("error in script '%s': '%s'\n"), inputfile, lua_tostring( ls, -1));
				break;
			case LUA_ERRMEM:
				fprintf( stderr, _TXT("out of memory in script '%s'\n"), inputfile);
				break;
#ifdef LUA_ERRGCMM
			case LUA_ERRGCMM:
				fprintf( stderr, _TXT("internal error in destructor call (call of __gc metamethod) in script '%s'\n"), inputfile);
				break;
#endif
			case LUA_ERRERR:
				fprintf( stderr, _TXT("error in lua error handler executing script '%s'\n"), inputfile);
				break;
			default:
				fprintf( stderr, _TXT("unknown error in script '%s'\n"), inputfile);
				break;
		}
		lua_close( ls);
		return -3;
	}
	lua_close( ls);
	return 0;
}

