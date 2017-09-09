/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "utils.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace strus;
using namespace strus::utils;

std::string utils::tolower( const std::string& val)
{
	return boost::algorithm::to_lower_copy( val);
}

std::string utils::trim( const std::string& val)
{
	return boost::algorithm::trim_copy( val);
}

bool utils::caseInsensitiveEquals( const std::string& val1, const std::string& val2)
{
	return boost::algorithm::iequals( val1, val2);
}

bool utils::caseInsensitiveStartsWith( const std::string& val, const std::string& prefix)
{
	return boost::algorithm::istarts_with( val, prefix);
}

int utils::toint( const std::string& val)
{
	try
	{
		return boost::lexical_cast<int>( val);
	}
	catch (const boost::bad_lexical_cast& err)
	{
		throw std::runtime_error( std::string( "failed to convert string '") + val + "' to integer: " + err.what());
	}
}

std::string utils::tostring( long val)
{
	try
	{
		return boost::lexical_cast<std::string>( val);
	}
	catch (...)
	{
		throw std::runtime_error( "failed to convert number to string (out of memory)");
	}
}

std::string utils::tostring( float val)
{
	try
	{
		return boost::lexical_cast<std::string>( val);
	}
	catch (...)
	{
		throw std::runtime_error( "failed to convert number to string (out of memory)");
	}
}

