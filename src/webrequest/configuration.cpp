/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Accessing web request configuration objects
/// \file "configuration.hpp"
#include "configuration.hpp"
#include "strus/base/string_format.hpp"
#include "strus/errorCodes.hpp"
#include "private/internationalization.hpp"
#include <cstddef>
#include <cstring>
#include <utility>
#include <map>
#include <algorithm>

using namespace strus;

struct greater
{
	template<class T>
	bool operator()(T const &a, T const &b) const { return a > b; }
};

static std::string getConfigFilenamePart( const std::string& filename, int pi)
{
	char const* si = filename.c_str();
	while (pi--)
	{
		si = std::strchr( si, '.');
		if (!si) return std::string();
		++si;
	}
	const char* se = std::strchr( si, '.');
	return se ? std::string( si, se-si) : std::string( si);
}

static strus::mutex g_timestmpmutex;
static char g_timestmpbuf[ 16] = "";
static int g_timestmpcnt = 0;

static std::string temporaryFilename( const std::string& type, const std::string& name)
{
	char timebuf[ 16];
	time_t timer;
	struct tm* tm_info;
	
	time( &timer);
	tm_info = localtime( &timer);

	std::strftime( timebuf, sizeof(timebuf), "%Y%m%d_%H%M%S", tm_info);
	{
		strus::unique_lock lock( g_timestmpmutex);
		if (0==std::strcmp( g_timestmpbuf, timebuf))
		{
			g_timestmpcnt += 1;
		}
		else
		{
			std::memcpy( g_timestmpbuf, timebuf, sizeof(g_timestmpbuf));
			g_timestmpcnt = 0;
		}
	}
	return strus::string_format( "%s_%03d.%s.%s.temp", timebuf, g_timestmpcnt, type.c_str(), name.c_str());
}

static std::string temporaryConfigFilepath( const std::string& dir, const std::string& service, const std::string& type, const std::string& name)
{
	std::string tmpfilename = temporaryFilename( type, name);
	std::string cfgstoredir = strus::joinFilePath( dir, service);
	std::string rt = strus::joinFilePath( dir, service);
	if (cfgstoredir.empty() || rt.empty()) throw std::bad_alloc();
	return rt;
}

static std::string commitConfigFilepath( const std::string& tmpfilename)
{
	char const* ee = tmpfilename.c_str() + tmpfilename.size();
	while (ee > tmpfilename.c_str() && *(ee-1) != '.') {--ee;}
	std::string rt( tmpfilename.c_str(), ee-tmpfilename.c_str());
	rt.append( "conf");
	return rt;
}

static std::vector<std::string> configurationFiles( const std::string& dir, const std::string& service, const std::string& ext)
{
	std::string cfgstoredir = strus::joinFilePath( dir, service);
	std::vector<std::string> rt;
	int ec = readDirFiles( cfgstoredir, ext, rt);
	if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to read configuration files in '%s'"), cfgstoredir.c_str());
	return rt;
}

std::string Configuration::storeTemporary( const std::string& dir_, const std::string& service_, const std::string& type_, const std::string& name_, const std::string& content_)
{
	std::string rt = temporaryConfigFilepath( dir_, service_, type_, name_);
	int ec = strus::writeFile( rt, content_);
	if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to store configuration file '%s'"), rt.c_str());
	return rt;
}

void Configuration::commit( const std::string& temporaryFilename)
{
	std::string commitFilename = commitConfigFilepath( temporaryFilename);
	strus::removeFile( commitFilename);
	int ec = strus::renameFile( temporaryFilename, commitFilename);
	if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to rename configuration file '%s' to '%s' in commit"), temporaryFilename.c_str(), commitFilename.c_str());
}

void Configuration::drop( const std::string& temporaryFilename)
{
	if (!temporaryFilename.empty())
	{
		strus::removeFile( temporaryFilename);
	}
}

void Configuration::remove( const std::string& dir_, const std::string& service_, const std::string& type_, const std::string& name_)
{
	std::vector<std::string> fl_active = configurationFiles( dir_, service_, strus::string_format( "%s.%s.conf", type_.c_str(), name_.c_str()));
	std::vector<std::string> fl_failed = configurationFiles( dir_, service_, strus::string_format( "%s.%s.temp", type_.c_str(), name_.c_str()));
	std::vector<std::string> fl;
	fl.insert( fl.end(), fl_active.begin(), fl_active.end());
	fl.insert( fl.end(), fl_failed.begin(), fl_failed.end());
	std::string cfgstoredir = strus::joinFilePath( dir_, service_);
	for (auto fn : fl)
	{
		std::string fullname = strus::joinFilePath( cfgstoredir, fn);
		int ec = strus::removeFile( fullname, true/*fail ifnotexist*/);
		if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to remove configuration file '%s'"), fn.c_str());
	}
}

std::vector<Configuration> Configuration::list( const std::string& dir_, const std::string& service_)
{
	std::vector<Configuration> rt;
	std::vector<std::string> fl = configurationFiles( dir_, service_, ".conf");
	std::sort( fl.begin(), fl.end());
	std::map<std::string,std::string> cfgmap;
	for (auto& fn : fl)
	{
		std::string tt = getConfigFilenamePart( fn, 1);
		std::string nn = getConfigFilenamePart( fn, 2);
		std::string key = tt + "/" + nn;
		cfgmap[ key] = fn;
	}
	std::vector<std::string> fl_uniq;
	for (auto kv : cfgmap)
	{
		fl_uniq.push_back( kv.second);
	}
	std::sort( fl_uniq.begin(), fl_uniq.end());

	std::string cfgstoredir = strus::joinFilePath( dir_, service_);
	for (auto fn : fl_uniq)
	{
		std::string fullname = strus::joinFilePath( cfgstoredir, fn);
		std::string content_;
		int ec = strus::readFile( fullname, content_);
		if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to read configuration file '%s'"), fn.c_str());
		std::string type_ = getConfigFilenamePart( fn, 1);
		std::string name_ = getConfigFilenamePart( fn, 2);
		rt.emplace_back( dir_, service_, type_, name_, content_);
	}
	return rt;
}

void Configuration::cleanup( const std::string& dir_, const std::string& service_)
{
	std::vector<std::string> fl_delete = configurationFiles( dir_, service_, ".temp");
	std::vector<std::string> fl = configurationFiles( dir_, service_, ".conf");

	std::sort( fl.begin(), fl.end());
	std::map<std::string,std::string> cfgmap;
	for (auto& fn : fl)
	{
		std::string tt = getConfigFilenamePart( fn, 1);
		std::string nn = getConfigFilenamePart( fn, 2);
		std::string key = tt + "/" + nn;
		auto ins = cfgmap.insert( {key, fn});
		if (ins.second == false/*element exists*/)
		{
			fl_delete.push_back( ins.first->second); //... delete the older
			ins.first->second = fn; //... overwrite with the newer
		}
	}
	std::string cfgstoredir = strus::joinFilePath( dir_, service_);
	for (auto& fn : fl_delete)
	{
		std::string fullname = strus::joinFilePath( cfgstoredir, fn);
		(void)strus::removeFile( fullname);
	}
}

