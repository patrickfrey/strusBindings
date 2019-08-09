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
	if (!se) se = std::strchr( si, '\0');
	return std::string( si, se-si);
}

ConfigurationHandler::ConfigurationHandler(
		WebRequestLoggerInterface* logger_,
		const std::string& config_store_dir_,
		const std::string& service_name_,
		const char** context_typenames)
	:m_mutex()
	,m_logger(logger_)
	,m_config_store_dir(config_store_dir_)
	,m_service_name(service_name_)
	,m_config_counter(0)
{
	char const** ci = context_typenames;
	for (; *ci; ++ci) m_context_typenames.insert( std::string(*ci));
}

std::string ConfigurationHandler::configurationStoreDirectory() const
{
	std::string cfgdir = strus::joinFilePath( m_config_store_dir, m_service_name);
	if (cfgdir.empty()) throw std::bad_alloc();
	return cfgdir;
}

void ConfigurationHandler::storeConfiguration(
		ConfigurationTransaction& transaction,
		const ConfigurationDescription& config)
{
	strus::unique_lock lock( m_mutex);
	char timebuf[ 256];
	char idxbuf[ 32];
	time_t timer;
	struct tm* tm_info;
	
	time( &timer);
	tm_info = localtime( &timer);
	WebRequestContent::Type doctype = webRequestContentFromTypeName( config.doctype.c_str());
	if (doctype == WebRequestContent::Unknown) throw strus::runtime_error( _TXT("unknown content type of configuration"));
	const char* doctypeName = WebRequestContent::typeName( doctype);

	std::strftime( timebuf, sizeof(timebuf), "%Y%m%d_%H%M%S", tm_info);
	std::snprintf( idxbuf, sizeof(idxbuf), "%03d", m_config_counter++);
	if (m_config_counter == MaxConfigCounter) m_config_counter = 0;

	std::string filename = strus::string_format( "%s_%s.%s.%s.%s.conf", timebuf, idxbuf, doctypeName, config.type.c_str(), config.name.c_str());
	std::string cfgdir = configurationStoreDirectory();
	transaction.type = config.type;
	transaction.name = config.name;
	transaction.filename = strus::joinFilePath( cfgdir, filename);
	if (transaction.filename.empty()) throw std::bad_alloc();
	transaction.failed_filename = transaction.filename + ".failed";
	int ec = strus::mkdirp( cfgdir);
	if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to create configuration store directory '%s'"), cfgdir.c_str());

	ec = strus::writeFile( transaction.failed_filename, config.contentbuf);
	if (ec) throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to store configuration file '%s'"), transaction.filename.c_str());
}

void ConfigurationHandler::commitStoreConfiguration(
		const ConfigurationTransaction& transaction)
{
	strus::unique_lock lock( m_mutex);

	ContextNameDef namedef( transaction.type, transaction.name);
	m_context_names.insert( namedef);

	int ec = strus::renameFile( transaction.failed_filename, transaction.filename);
	if (ec)
	{
		m_context_names.erase( namedef);
		throw strus::runtime_error_ec( (ErrorCode)ec, _TXT("failed to commit configuration change '%s'"), transaction.filename.c_str());
	}
}

void ConfigurationHandler::deleteStoredConfiguration(
		const char* contextType,
		const char* contextName)
{
	strus::unique_lock lock( m_mutex);

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
	m_context_names.erase( namedef);
}

void ConfigurationHandler::clearUnfinishedTransactions()
{
	std::string cfgdir = configurationStoreDirectory();
	if (strus::isDir( cfgdir))
	{
		strus::unique_lock lock( m_mutex);
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

ConfigurationDescription ConfigurationHandler::getStoredConfiguration(
		const char* contextType,
		const char* contextName)
{
	std::string cfgdir = configurationStoreDirectory();

	strus::unique_lock lock( m_mutex);
	std::vector<std::string> configFileNames;

	int ec = strus::readDirFiles( cfgdir, ".conf", configFileNames);
	if (ec) throw strus::runtime_error_ec( ec, _TXT("error loading stored configuration: %s"), std::strerror(ec));

	std::sort( configFileNames.begin(), configFileNames.end(), greater());
	std::vector<std::string>::const_iterator ci = configFileNames.begin(), ce = configFileNames.end();
	for (; ci != ce; ++ci)
	{
		std::string doctype = getConfigFilenamePart( *ci, 1);
		if (doctype.empty()) continue;
		std::string candidateContextType = getConfigFilenamePart( *ci, 2);
		std::string candidateContextName = getConfigFilenamePart( *ci, 3);
		if (candidateContextType != contextType || candidateContextName != contextName) continue;
		std::string date = getConfigFilenamePart( *ci, 0);
		std::string filepath = strus::joinFilePath( cfgdir, *ci);
		std::string contentbuf;
		ec = strus::readFile( filepath, contentbuf);
		if (ec) throw strus::runtime_error_ec( ec, _TXT("error reading stored configuration file %s: %s"), filepath.c_str(), std::strerror(ec));

		return ConfigurationDescription( contextType, contextName, doctype, contentbuf);
	}
	return ConfigurationDescription();
}

std::vector<ConfigurationDescription> ConfigurationHandler::getStoredConfigurations( bool doDeleteObsolete)
{
	std::string cfgdir = configurationStoreDirectory();
	std::vector<ConfigurationDescription> rt;

	if (strus::isDir( cfgdir))
	{
		strus::unique_lock lock( m_mutex);

		typedef std::pair<std::string,std::string> ConfigItem;
		std::set<ConfigItem> configItemSet;
		std::vector<std::string> configFileNames;

		int ec = strus::readDirFiles( cfgdir, ".conf", configFileNames);
		if (ec) throw strus::runtime_error_ec( ec, _TXT("error loading stored configuration in %s"), cfgdir.c_str());
	
		std::sort( configFileNames.begin(), configFileNames.end(), greater());
		std::vector<std::string>::const_iterator ci = configFileNames.begin(), ce = configFileNames.end();
		for (; ci != ce; ++ci)
		{
			std::string doctype = getConfigFilenamePart( *ci, 1);
			if (doctype.empty()) continue;
			std::string contextType = getConfigFilenamePart( *ci, 2);
			std::string contextName = getConfigFilenamePart( *ci, 3);
			std::string date = getConfigFilenamePart( *ci, 0);
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
	
			rt.push_back( ConfigurationDescription( contextType, contextName, doctype, contentbuf));
		}
		std::reverse( rt.begin(), rt.end());
	}
	return rt;
}

std::vector<ConfigurationDescription> ConfigurationHandler::getStoredConfigurations()
{
	return ConfigurationHandler::getStoredConfigurations( false);
}

void ConfigurationHandler::deleteObsoleteConfigurations()
{
	ConfigurationHandler::getStoredConfigurations( true);
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
					if (taglevel == 1)
					{
						char nambuf[ 128];
						const char* nam = papuga_ValueVariant_toascii( nambuf, sizeof(nambuf), papuga_SerializationIter_value( &itr), 0/*non ascii subst*/);
						if (nam && 0==std::strcmp( nam, "id"))
						{
							papuga_SerializationIter_skip( &itr);
							if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
	
							if (papuga_SerializationIter_tag( &itr) == papuga_TagValue)
							{
								std::size_t idlen;
								const char* idval = papuga_ValueVariant_tostring( papuga_SerializationIter_value( &itr), &allocator, &idlen, &errcode);
								if (!idval) throw std::bad_alloc();
								subconfidid = std::string( idval, idlen);
							}
						}
						else
						{
							if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
						}
					}
					else
					{
						if (!papuga_Serialization_push( &ser, papuga_SerializationIter_tag( &itr), papuga_SerializationIter_value( &itr))) throw std::bad_alloc();
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
					papuga_UTF8, NULL/*rootname*/, NULL/*elemname*/, &subcfglen, &errcode);
	if (!subcfgstr) throw strus::runtime_error_ec( papugaErrorToErrorCode( errcode), _TXT("failed to load sub configuration"));
	const char* doctype = WebRequestContent::typeName( WebRequestContent::JSON);
	return ConfigurationDescription( name, subconfidid.empty() ? name : subconfidid, doctype, std::string(subcfgstr, subcfglen));
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
						if (nam && m_context_typenames.find( nam) != m_context_typenames.end())
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


std::vector<std::string> ConfigurationHandler::contextNames( const std::string& name) const
{
	std::vector<std::string> rt;
	ContextNameDef cndef( name, std::string());
	strus::unique_lock lock( m_mutex);
	std::set<ContextNameDef>::const_iterator ci = m_context_names.upper_bound( cndef);
	for (; ci != m_context_names.end() && ci->first == name; ++ci)
	{
		rt.push_back( ci->second);
	}
	return rt;
}

std::vector<std::string> ConfigurationHandler::contextTypes() const
{
	std::set<std::string> res;
	strus::unique_lock lock( m_mutex);
	std::set<ContextNameDef>::const_iterator ci = m_context_names.begin();
	for (; ci != m_context_names.end(); ++ci)
	{
		res.insert( ci->first);
	}
	return std::vector<std::string>( res.begin(), res.end());
}

void ConfigurationHandler::declareSubConfiguration( const char* contextType, const char* contextName)
{
	strus::unique_lock lock( m_mutex);
	ContextNameDef namedef( contextType, contextName);
	m_context_names.insert( namedef);
}

