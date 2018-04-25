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
#include <cstddef>
#include <string>
#include <vector>
#include <set>

#define ROOT_CONTEXT_NAME "context"

namespace strus
{
/// \brief Forward declaration
class WebRequestLoggerInterface;

struct ConfigurationDescription
{
	std::string type;
	std::string name;
	std::string doctype;
	std::string contentbuf;

	ConfigurationDescription()
		:type(),name(),doctype(),contentbuf(){}
	ConfigurationDescription( const std::string& type_, const std::string& name_, const std::string& doctype_, const std::string& contentbuf_)
		:type(type_),name(name_),doctype(doctype_),contentbuf(contentbuf_){}
	ConfigurationDescription( const ConfigurationDescription& o)
		:type(o.type),name(o.name),doctype(o.doctype),contentbuf(o.contentbuf){}

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
};


/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
class ConfigurationHandler
{
public:
	ConfigurationHandler(
			WebRequestLoggerInterface* logger_,
			const std::string& config_store_dir_,
			const char** context_typenames);

	virtual ~ConfigurationHandler(){}

	void storeConfiguration(
			ConfigurationTransaction& transaction,
			const ConfigurationDescription& config);

	void commitStoreConfiguration(
			const ConfigurationTransaction& transaction);
	
	void deleteStoredConfiguration(
			const char* contextType,
			const char* contextName);

	void deleteObsoleteConfigurations();
	void clearUnfinishedTransactions();

	ConfigurationDescription getStoredConfiguration(
			const char* contextType,
			const char* contextName);

	std::vector<ConfigurationDescription> getStoredConfigurations();

	std::vector<ConfigurationDescription> getSubConfigurations( const std::string& configstr);

	void declareSubConfiguration( const char* contextType, const char* contextName);

	std::vector<std::string> contextNames( const std::string& name) const;
	std::vector<std::string> contextTypes() const;

private:
	std::vector<ConfigurationDescription> getStoredConfigurations( bool doDeleteObsolete);
	typedef std::pair<std::string,std::string> ContextNameDef;

private:
	mutable strus::mutex m_mutex;
	WebRequestLoggerInterface* m_logger;
	std::string m_config_store_dir;
	enum {MaxConfigCounter=999};
	int m_config_counter;
	std::set<std::string> m_context_typenames;	//< defined context types
	std::set<ContextNameDef> m_context_names;	//< context definitions type name pairs
};

}//namespace
#endif



