/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Handler for accessing configuration objects
/// \file "configurationHandler.hpp"
#include "configurationHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/lib/bindings_description.hpp"
#include "strus/base/string_format.hpp"
#include "strus/errorCodes.hpp"
#include "papuga/valueVariant.h"
#include "papuga/valueVariant.hpp"
#include "papuga/allocator.h"
#include "papuga/serialization.h"
#include "papuga/errors.hpp"
#include "private/internationalization.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <utility>
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
	return se ? std::string( si, se-si) : std::string();
}

ConfigurationHandler::ConfigurationHandler(
		WebRequestLoggerInterface* logger_,
		const std::string& configStoreDir_,
		const std::string& serviceName_)
	:m_mutex()
	,m_logger(logger_)
	,m_configStoreDir(configStoreDir_)
	,m_serviceName(serviceName_)
	,m_configCounter(0)
{
	m_lastTimeStmp[0] = 0;
}

std::string ConfigurationHandler::configurationStoreDirectory() const
{
	std::string cfgdir = strus::joinFilePath( m_configStoreDir, m_serviceName);
	if (cfgdir.empty()) throw std::bad_alloc();
	return cfgdir;
}

std::string ConfigurationHandler::newConfigStorageFilename( const ConfigurationDescription& config)
{
	char timebuf[ 16];
	char idxbuf[ 16];
	time_t timer;
	struct tm* tm_info;
	
	time( &timer);
	tm_info = localtime( &timer);

	std::strftime( timebuf, sizeof(timebuf), "%Y%m%d_%H%M%S", tm_info);

	strus::unique_lock lock( m_mutex);
	if (0==std::strcmp( m_lastTimeStmp, timebuf))
	{
		m_configCounter = 0;
		std::memcpy( m_lastTimeStmp, timebuf, sizeof(m_lastTimeStmp));
	}
	std::snprintf( idxbuf, sizeof(idxbuf), "%03d", m_configCounter++);
	return strus::string_format( "%s_%s.%s.%s.%s.conf", timebuf, idxbuf, config.method.c_str(), config.type.c_str(), config.name.c_str());
}

ConfigurationTransaction ConfigurationHandler::newConfigurationTransaction( const ConfigurationDescription& config, const std::string& filename, const std::string& failed_filename)
{
	ConfigurationTransaction rt;
	std::string cfgdir = configurationStoreDirectory();
	rt.type = config.type;
	rt.name = config.name;
	rt.filename = strus::joinFilePath( cfgdir, filename);
	if (rt.filename.empty()) throw std::bad_alloc();
	rt.failed_filename = failed_filename;

	int ec = strus::writeFile( rt.failed_filename, config.contentbuf);
	if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to store configuration file '%s'"), rt.filename.c_str());
	return rt;
}

void ConfigurationHandler::storeConfiguration(
		ConfigurationTransaction& transaction,
		const ConfigurationDescription& config)
{
	std::string cfgdir = configurationStoreDirectory();
	int ec = strus::mkdirp( cfgdir);
	if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to create configuration store directory '%s': %s"), cfgdir.c_str(), ::strerror(ec));

	std::string filename = newConfigStorageFilename( config);
	std::string failed_filename = filename + ".failed";
	transaction = newConfigurationTransaction( config, filename, failed_filename);
}

void ConfigurationHandler::storeConfigurationReplace(
		ConfigurationTransaction& transaction,
		const ConfigurationDescription& config)
{
	std::string filename = getStoredConfigurationFile( config.type.c_str(), config.name.c_str());
	std::string failed_filename = newConfigStorageFilename( config) + ".failed";
	transaction = newConfigurationTransaction( config, filename, failed_filename);
}

void ConfigurationHandler::commitStoreConfiguration(
		const ConfigurationTransaction& transaction)
{
	ContextNameDef namedef( transaction.type, transaction.name);
	strus::unique_lock lock( m_mutex);
	m_contextNameMap.insert( ContextNameMap::value_type( namedef, false));

	int ec = strus::renameFile( transaction.failed_filename, transaction.filename);
	if (ec)
	{
		m_contextNameMap.erase( namedef);
		throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to commit configuration change '%s'"), transaction.filename.c_str());
	}
	m_contextNameMap[ namedef] = true;
}

void ConfigurationHandler::deleteStoredConfiguration(
		const char* contextType,
		const char* contextName)
{
	std::string fileext = strus::string_format( ".%s.%s.conf", contextType, contextName);
	std::vector<std::string> files;
	std::string cfgdir = configurationStoreDirectory();
	int ec = strus::readDirFiles( cfgdir, fileext, files);
	if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to read files '*%s' in config store directory '%s'"), fileext.c_str(), cfgdir.c_str());

	std::vector<std::string>::const_iterator fi = files.begin(), fe = files.end();
	for (; fi != fe; ++fi)
	{
		std::string filepath = strus::joinFilePath( cfgdir, *fi);
		ec = strus::removeFile( filepath, true);
		if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to remove file %s: %s"), filepath.c_str(), std::strerror(ec));
	}
	ContextNameDef namedef( contextType, contextName);
	strus::unique_lock lock( m_mutex);
	m_contextNameMap.erase( namedef);
}

void ConfigurationHandler::clearUnfinishedTransactions()
{
	std::string cfgdir = configurationStoreDirectory();
	if (strus::isDir( cfgdir))
	{
		std::string fileext = strus::string_format( ".conf.failed");
		std::vector<std::string> files;
		int ec = strus::readDirFiles( cfgdir, fileext, files);
		if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to read files '*%s' in config store directory '%s'"), fileext.c_str(), cfgdir.c_str());

		std::vector<std::string>::const_iterator fi = files.begin(), fe = files.end();
		for (; fi != fe; ++fi)
		{
			std::string filepath = strus::joinFilePath( cfgdir, *fi);
			ec = strus::removeFile( filepath, true);
			if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to clear unfinished transaction (file %s): %s"), filepath.c_str(), std::strerror(ec));
		}
	}
}

std::string ConfigurationHandler::getStoredConfigurationFile( const char* contextType, const char* contextName)
{
	std::string cfgdir = configurationStoreDirectory();

	strus::unique_lock lock( m_mutex);
	std::vector<std::string> configFileNames;

	std::string fileext = strus::string_format( ".%s.%s.conf", contextType, contextName);

	int ec = strus::readDirFiles( cfgdir, fileext.c_str(), configFileNames);
	if (ec) throw strus::runtime_error_ec( ec, _TXT("error loading stored configuration: %s"), std::strerror(ec));

	std::sort( configFileNames.begin(), configFileNames.end(), greater());
	return configFileNames.empty() ? std::string() : *configFileNames.begin();
}

static WebRequestContent::Type detectContentType( const std::string& content)
{
	char const* ci = content.c_str();
	while (*ci && (unsigned char)*ci <= 32) ++ci;
	switch (*ci)
	{
		case '<': return WebRequestContent::XML;
		case '{': return WebRequestContent::JSON;
		default: return WebRequestContent::Unknown;
	}
}

WebRequestContent::Type ConfigurationDescription::evalDoctype( const std::string& content)
{
	return detectContentType( content);
}

ConfigurationDescription ConfigurationHandler::getStoredConfigurationFromFile( const std::string& filename)
{
	std::string cfgdir = configurationStoreDirectory();
	std::string method = getConfigFilenamePart( filename, 1);
	std::string contextType = getConfigFilenamePart( filename, 2);
	std::string contextName = getConfigFilenamePart( filename, 3);
	std::string filepath = strus::joinFilePath( cfgdir, filename);

	std::string contentbuf;
	int ec = strus::readFile( filepath, contentbuf);
	if (ec) throw strus::runtime_error_ec( ec, _TXT("error reading stored configuration file %s: %s"), filepath.c_str(), std::strerror(ec));

	return ConfigurationDescription( contextType, contextName, method, contentbuf);
}

std::vector<ConfigurationDescription> ConfigurationHandler::getStoredConfigurations( bool doDeleteObsolete)
{
	std::string cfgdir = configurationStoreDirectory();
	std::vector<ConfigurationDescription> rt;

	if (!strus::isDir( cfgdir)) return rt;

	typedef std::pair<std::string,std::string> ConfigItem;
	std::set<ConfigItem> configItemSet;
	std::vector<std::string> configFileNames;

	int ec = strus::readDirFiles( cfgdir, ".conf", configFileNames);
	if (ec) throw strus::runtime_error_ec( ec, _TXT("error loading stored configuration in %s"), cfgdir.c_str());

	std::sort( configFileNames.begin(), configFileNames.end(), greater());
	std::vector<std::string>::const_iterator ci = configFileNames.begin(), ce = configFileNames.end();
	for (; ci != ce; ++ci)
	{
		std::string method = getConfigFilenamePart( *ci, 1);
		std::string contextType = getConfigFilenamePart( *ci, 2);
		std::string contextName = getConfigFilenamePart( *ci, 3);
		std::string filepath = strus::joinFilePath( cfgdir, *ci);

		if (!configItemSet.insert( ConfigItem( contextType, contextName)).second)
		{
			if (doDeleteObsolete)
			{
				ec = strus::removeFile( filepath, true);
				if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to remove file %s: %s"), filepath.c_str(), std::strerror(ec));
			}
			continue;
		}
		std::string contentbuf;
		ec = strus::readFile( filepath, contentbuf);
		if (ec) throw strus::runtime_error_ec( ec, _TXT("error reading stored configuration file %s: %s"), filepath.c_str(), std::strerror(ec));

		rt.push_back( ConfigurationDescription( contextType, contextName, method, contentbuf));
	}
	std::reverse( rt.begin(), rt.end());
	return rt;
}

std::vector<ConfigurationDescription> ConfigurationHandler::getStoredConfigurations()
{
	return ConfigurationHandler::getStoredConfigurations( false);
}

void ConfigurationHandler::deleteObsoleteConfigurations()
{
	(void)ConfigurationHandler::getStoredConfigurations( true);
}

static ConfigurationDescription createSubConfig( const std::string& name, papuga_Allocator& allocator, papuga_SerializationIter& itr)
{
	papuga_ErrorCode errcode = papuga_Ok;
	std::string subconfidid;
	papuga_Serialization ser;
	papuga_init_Serialization( &ser, &allocator);
	int taglevel = 0;

	if (!papuga_Serialization_pushName_string( &ser, name.c_str(), name.size())) throw std::bad_alloc();
	if (papuga_SerializationIter_tag( &itr) == papuga_TagOpen)
	{
		++taglevel;
		if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
		papuga_SerializationIter_skip( &itr); 

		for (; taglevel > 0 && !papuga_SerializationIter_eof( &itr); papuga_SerializationIter_skip( &itr))
		{
			switch (papuga_SerializationIter_tag( &itr))
			{
				case papuga_TagValue:
					if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
					break;
				case papuga_TagOpen:
					if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
					++taglevel;
					break;
				case papuga_TagClose:
					if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
					--taglevel;
					break;
				case papuga_TagName:
					if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
					if (taglevel == 1)
					{
						char nambuf[ 128];
						const char* nam = papuga_ValueVariant_toascii( nambuf, sizeof(nambuf), papuga_SerializationIter_value( &itr), 0/*non ascii subst*/);
						if (nam && 0==std::strcmp( nam, "id") && papuga_TagValue == papuga_SerializationIter_follow_tag( &itr))
						{
							papuga_SerializationIter_skip( &itr);
							if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
							if (papuga_ValueVariant_isatomic( papuga_SerializationIter_value( &itr)))
							{
								subconfidid = papuga::ValueVariant_tostring( *papuga_SerializationIter_value( &itr), errcode);
								if (errcode != papuga_Ok) throw std::bad_alloc();
							}
						}
					}
			}
		}
	}
	else if (papuga_SerializationIter_tag( &itr) == papuga_TagValue)
	{
		if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
	}
	papuga_ValueVariant subconfigval;
	papuga_init_ValueVariant_serialization( &subconfigval, &ser);
	std::size_t subcfglen;
	const char* subcfgstr = (const char*)papuga_ValueVariant_tojson(
					&subconfigval, &allocator, getBindingsInterfaceDescription()->structs,
					papuga_UTF8, true/*beautyfied*/, NULL/*rootname*/, NULL/*elemname*/,
					&subcfglen, &errcode);
	if (!subcfgstr) throw strus::runtime_error_ec( papugaErrorToErrorCode( errcode), _TXT("failed to load sub configuration"));
	return ConfigurationDescription( name, subconfidid.empty() ? name : subconfidid, std::string()/*method*/, std::string(subcfgstr, subcfglen));
}

std::vector<ConfigurationDescription> ConfigurationHandler::getSubConfigurations( const std::string& configstr)
{
	std::vector<ConfigurationDescription> rt;
	papuga_ErrorCode errcode = papuga_Ok;
	papuga_Allocator allocator;
	char allocator_mem[ 4096];
	papuga_init_Allocator( &allocator, allocator_mem, sizeof(allocator_mem));

	try
	{
		papuga_ValueVariant configstruct;
		if (!papuga_init_ValueVariant_json( &configstruct, &allocator, papuga_UTF8, configstr.c_str(), configstr.size(), &errcode)) goto EXIT;
		if (configstruct.valuetype != papuga_TypeSerialization) goto EXIT;
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, configstruct.value.serialization);
		int taglevel = 0;

		while (!papuga_SerializationIter_eof( &seriter))
		{
			switch (papuga_SerializationIter_tag( &seriter))
			{
				case papuga_TagValue:
					break;
				case papuga_TagOpen:
					++taglevel;
					break;
				case papuga_TagClose:
					--taglevel;
					break;
				case papuga_TagName:
					if (taglevel == 0)
					{
						char nambuf[ 128];
						const char* nam = papuga_ValueVariant_toascii( nambuf, sizeof(nambuf), papuga_SerializationIter_value( &seriter), 0/*non ascii subst*/);
						if (nam && m_contextTypeNames.find( nam) != m_contextTypeNames.end())
						{
							papuga_SerializationIter_skip( &seriter);
							rt.push_back( createSubConfig( nam, allocator, seriter));
							continue;
						}
					}
					break;
			}
			papuga_SerializationIter_skip( &seriter);
		}
	}
	catch (const std::bad_alloc&)
	{
		papuga_destroy_Allocator( &allocator);
		throw std::bad_alloc();
	}
EXIT:
	papuga_destroy_Allocator( &allocator);
	if (errcode != papuga_Ok) throw papuga::error_exception( errcode, _TXT("parse sub configurations"));
	return rt;
}

std::string ConfigurationHandler::allocTemporaryContextName( const std::string& contextType, const char* prefix)
{
	ContextNameDef cndef( contextType, std::string());
	strus::unique_lock lock( m_mutex);
	ContextNameMap::const_iterator start = m_contextNameMap.upper_bound( cndef);
	char ibuf[ 64];
	int idx = 1;

	for (;;++idx)
	{
		std::snprintf( ibuf, sizeof(ibuf), "%s%d", prefix, idx);
		ContextNameMap::const_iterator ci = start;
		bool found = false;
		for (; !found && ci != m_contextNameMap.end() && ci->first.contextType == contextType; ++ci)
		{
			found = (ci->first.contextName == ibuf);
		}
		if (!found)
		{
			cndef.contextName = ibuf;
			m_contextNameMap.insert( ContextNameMap::value_type( cndef, false));
			return cndef.contextName;
		}
	}
}

void ConfigurationHandler::releaseTemporaryContextName( const std::string& contextType, const std::string& contextName)
{
	ContextNameDef cndef( contextType, contextName);
	strus::unique_lock lock( m_mutex);
	ContextNameMap::const_iterator di = m_contextNameMap.find( cndef);
	if (!di->second)
	{
		m_contextNameMap.erase( cndef);
	}
}

std::vector<std::string> ConfigurationHandler::contextTypes() const
{
	std::set<std::string> res;
	strus::unique_lock lock( m_mutex);
	ContextNameMap::const_iterator ci = m_contextNameMap.begin();
	for (; ci != m_contextNameMap.end(); ++ci)
	{
		if (ci->second)
		{
			res.insert( ci->first.contextType);
		}
	}
	return std::vector<std::string>( res.begin(), res.end());
}

std::vector<std::string> ConfigurationHandler::contextNames( const std::string& contextType) const
{
	std::vector<std::string> rt;
	ContextNameDef cndef( contextType, std::string());
	strus::unique_lock lock( m_mutex);
	ContextNameMap::const_iterator ci = m_contextNameMap.upper_bound( cndef);
	for (; ci != m_contextNameMap.end() && ci->first.contextType == contextType; ++ci)
	{
		if (ci->second)
		{
			rt.push_back( ci->first.contextName);
		}
	}
	return rt;
}

void ConfigurationHandler::declareSubConfiguration( const std::string& contextType, const std::string& contextName)
{
	strus::unique_lock lock( m_mutex);
	ContextNameDef namedef( contextType, contextName);
	m_contextNameMap.insert( ContextNameMap::value_type( namedef, false));
}

