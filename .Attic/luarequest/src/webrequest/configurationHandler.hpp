/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Handler for accessing configuration objects
/// \file "configurationHandler.hpp"
#ifndef _STRUS_CONFIGURATION_HANDLER_IMPL_HPP_INCLUDED
#define _STRUS_CONFIGURATION_HANDLER_IMPL_HPP_INCLUDED
#include "strus/base/thread.hpp"
#include "strus/base/fileio.hpp"
#include "strus/webRequestContent.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>

#define ROOT_CONTEXT_NAME "context"

namespace strus
{
/// \brief Forward declaration
class WebRequestLoggerInterface;

struct ConfigurationDescription
{
	std::string type;
	std::string name;
	std::string method;
	WebRequestContent::Type doctype;
	std::string contentbuf;

	static WebRequestContent::Type evalDoctype( const std::string& content);

	ConfigurationDescription()
		:type(),name(),doctype(),contentbuf(){}
	ConfigurationDescription( const std::string& type_, const std::string& name_, const std::string& method_, const std::string& contentbuf_)
		:type(type_),name(name_),method(method_),doctype(evalDoctype(contentbuf_)),contentbuf(contentbuf_){}
	ConfigurationDescription( const ConfigurationDescription& o)
		:type(o.type),name(o.name),method(o.method),doctype(o.doctype),contentbuf(o.contentbuf){}
	ConfigurationDescription& operator=( const ConfigurationDescription& o)
		{type=o.type;name=o.name;method=o.method,doctype=o.doctype;contentbuf=o.contentbuf; return *this;}
#if __cplusplus >= 201103L
	ConfigurationDescription( ConfigurationDescription&& o)
		:type(std::move(o.type)),name(std::move(o.name)),method(std::move(o.method)),doctype(o.doctype),contentbuf(std::move(o.contentbuf)){}
	ConfigurationDescription& operator=( ConfigurationDescription&& o)
		{type=std::move(o.type);name=std::move(o.name);method=std::move(o.method);doctype=o.doctype;contentbuf=std::move(o.contentbuf); return *this;}
#endif

	bool valid() const	{return !type.empty();}
};

struct ConfigurationTransaction
{
	std::string type;
	std::string name;
	std::string failed_filename;
	std::string filename;

	ConfigurationTransaction()
		:type(),name(),failed_filename(),filename(){}
	ConfigurationTransaction( const std::string& type_, const std::string& name_, const std::string& failed_filename_, const std::string& filename_)
		:type(type_),name(name_),failed_filename(failed_filename_),filename(filename_){}
	ConfigurationTransaction( const ConfigurationTransaction& o)
		:type(o.type),name(o.name),failed_filename(o.failed_filename),filename(o.filename){}
	ConfigurationTransaction& operator=( const ConfigurationTransaction& o)
		{type=o.type;name=o.name;failed_filename=o.failed_filename;filename=o.filename; return *this;}
#if __cplusplus >= 201103L
	ConfigurationTransaction( ConfigurationTransaction&& o)
		:type(std::move(o.type)),name(std::move(o.name)),failed_filename(std::move(o.failed_filename)),filename(std::move(o.filename)){}
	ConfigurationTransaction& operator=( ConfigurationTransaction&& o)
		{type=std::move(o.type);name=std::move(o.name);failed_filename=std::move(o.failed_filename);filename=std::move(o.filename); return *this;}
#endif
	bool defined() const
	{
		return !filename.empty();
	}
};


/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
class ConfigurationHandler
{
public:
	ConfigurationHandler(
			WebRequestLoggerInterface* logger_,
			const std::string& configStoreDir_,
			const std::string& serviceName_);

	virtual ~ConfigurationHandler(){}

	void storeConfiguration(
			ConfigurationTransaction& transaction,
			const ConfigurationDescription& config);
	void storeConfigurationReplace(
			ConfigurationTransaction& transaction,
			const ConfigurationDescription& config);

	void commitStoreConfiguration(
			const ConfigurationTransaction& transaction);
	
	void deleteStoredConfiguration(
			const char* contextType,
			const char* contextName);

	void deleteObsoleteConfigurations();
	void clearUnfinishedTransactions();

	std::string getStoredConfigurationFile( const char* contextType, const char* contextName);
	ConfigurationDescription getStoredConfigurationFromFile( const std::string& filename);

	std::vector<ConfigurationDescription> getStoredConfigurations();

	std::vector<ConfigurationDescription> getSubConfigurations( const std::string& configstr);

	std::vector<std::string> contextNames( const std::string& name) const;
	std::vector<std::string> contextTypes() const;
	std::string allocTemporaryContextName( const std::string& contextType, const char* prefix);
	void releaseTemporaryContextName( const std::string& contextType, const std::string& contextName);

	void declareSubConfiguration( const std::string& contextType, const std::string& contextName);

private:
	std::string newConfigStorageFilename( const ConfigurationDescription& config);
	ConfigurationTransaction newConfigurationTransaction( const ConfigurationDescription& config, const std::string& filename, const std::string& failed_filename);
	std::string configurationStoreDirectory() const;
	std::vector<ConfigurationDescription> getStoredConfigurations( bool doDeleteObsolete);
	struct ContextNameDef
	{
		std::string contextType;
		std::string contextName;

		ContextNameDef()
			:contextType(),contextName(){}
		ContextNameDef( const std::string& contextType_, const std::string& contextName_)
			:contextType(contextType_),contextName(contextName_){}
		ContextNameDef( const ContextNameDef& o)
			:contextType(o.contextType),contextName(o.contextName){}

		bool operator<( const ContextNameDef& o) const
		{
			return (contextType == o.contextType) ? (contextName < o.contextName) : (contextType < o.contextType);
		}
	};

private:
	mutable strus::mutex m_mutex;
	WebRequestLoggerInterface* m_logger;
	std::string m_configStoreDir;
	std::string m_serviceName;
	char m_lastTimeStmp[16];
	int m_configCounter;
	typedef std::map<ContextNameDef,bool> ContextNameMap;
	ContextNameMap m_contextNameMap;		//< map context definitions type name pairs to stored flag
};

}//namespace
#endif



