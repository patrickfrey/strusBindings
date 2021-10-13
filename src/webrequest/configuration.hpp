/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Accessing web request configuration objects
/// \file "configuration.hpp"
#ifndef _STRUS_WEBREQUEST_CONFIGURATION_HPP_INCLUDED
#define _STRUS_WEBREQUEST_CONFIGURATION_HPP_INCLUDED
#include "strus/base/thread.hpp"
#include "strus/base/fileio.hpp"
#include <cstddef>
#include <utility>
#include <vector>
#include <string>

namespace strus
{

class Configuration
{
public:
	Configuration()
		:m_dir(),m_service(),m_type(),m_name(),m_content(){}
	Configuration( const std::string& dir_, const std::string& service_, const std::string& type_, const std::string& name_, const std::string& content_)
		:m_dir(dir_),m_service(service_),m_type(type_),m_name(name_),m_content(content_){}
	Configuration( const Configuration& o)
		:m_dir(o.m_dir),m_service(o.m_service),m_type(o.m_type),m_name(o.m_name),m_content(o.m_content){}

	static std::string storeTemporary( const std::string& dir_, const std::string& service_, const std::string& type_, const std::string& name_, const std::string& content_);
	static void commit( const std::string& temporaryFilename);
	static void drop( const std::string& temporaryFilename);
	static void remove( const std::string& dir_, const std::string& service_, const std::string& type_, const std::string& name_);
	static std::vector<Configuration> list( const std::string& dir_, const std::string& service_);
	static void cleanup( const std::string& dir_, const std::string& service_);

	const std::string& dir() const noexcept 	{return m_dir;}
	const std::string& service() const noexcept 	{return m_service;}
	const std::string& type() const noexcept 	{return m_type;}
	const std::string& name() const noexcept 	{return m_name;}
	const std::string& content() const noexcept 	{return m_content;}

private:
	std::string m_dir;
	std::string m_service;
	std::string m_type;
	std::string m_name;
	std::string m_content;
};

}//namespace
#endif

