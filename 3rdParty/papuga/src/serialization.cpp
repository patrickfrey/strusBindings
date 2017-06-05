/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Some functions on serialization using C++ features like STL
/// \file serialization.cpp
#include "papuga/serialization.hpp"
#include "papuga/valueVariant.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace papuga;

static bool Serialization_print( std::ostream& out, std::string indent, const papuga_Serialization& serialization, papuga_ErrorCode& errcode)
{
	Serialization::const_iterator
		si = Serialization::begin( &serialization),
		se = Serialization::end( &serialization);
	for (; si != se; ++si)
	{
		switch (si->tag)
		{
			case papuga_TagOpen:
				out << indent << "open" << std::endl;
				indent.append( "  ");
				break;
			case papuga_TagClose:
				if (indent.size() < 2)
				{
					errcode = papuga_TypeError;
					return false;
				}
				indent.resize( indent.size()-2);
				out << indent << "close" << std::endl;
				break;
			case papuga_TagName:
				out << indent << "name " << ValueVariant_tostring( si->value, errcode) << std::endl;
				break;
			case papuga_TagValue:
				if (!papuga_ValueVariant_defined( &si->value))
				{
					out << indent << "value NULL" << std::endl;
				}
				else if (!papuga_ValueVariant_isatomic( &si->value))
				{
					if (si->value.valuetype == papuga_TypeSerialization)
					{
						if (!Serialization_print( out, indent, *si->value.value.serialization, errcode)) return false;
					}
					else if (si->value.valuetype == papuga_TypeHostObject)
					{
						out << indent << "value HOSTOBJ" << std::endl;
					}
					else if (si->value.valuetype == papuga_TypeIterator)
					{
						out << indent << "value ITERATOR" << std::endl;
					}
					else
					{
						errcode = papuga_TypeError;
						return false;
					}
				}
				else if (papuga_ValueVariant_isstring( &si->value))
				{
					out << indent << "value '" << ValueVariant_tostring( si->value, errcode) << "'" << std::endl;
				}
				else
				{
					out << indent << "value " << ValueVariant_tostring( si->value, errcode) << std::endl;
				}
				break;
			default:
			{
				errcode = papuga_TypeError;
				return false;
			}
		}
	}
	return true;
}

std::string Serialization::tostring( const papuga_Serialization& serialization, papuga_ErrorCode& errcode)
{
	try
	{
		std::string indent;
		std::ostringstream out;
		Serialization::const_iterator
			si = Serialization::begin( &serialization),
			se = Serialization::end( &serialization);
		for (; si != se; ++si)
		{
			switch (si->tag)
			{
				case papuga_TagOpen:
					out << indent << "open" << std::endl;
					indent.append( "  ");
					break;
				case papuga_TagClose:
					if (indent.size() < 2)
					{
						errcode = papuga_TypeError;
						return std::string();
					}
					indent.resize( indent.size()-2);
					out << indent << "close" << std::endl;
					break;
				case papuga_TagName:
					out << indent << "name " << ValueVariant_tostring( si->value, errcode) << std::endl;
					break;
				case papuga_TagValue:
					if (!papuga_ValueVariant_defined( &si->value))
					{
						out << indent << "value NULL" << std::endl;
					}
					else if (!papuga_ValueVariant_isatomic( &si->value))
					{
						if (si->value.valuetype == papuga_TypeSerialization)
						{
							
						}
					}
					else if (papuga_ValueVariant_isstring( &si->value))
					{
						out << indent << "value '" << ValueVariant_tostring( si->value, errcode) << "'" << std::endl;
					}
					else
					{
						out << indent << "value " << ValueVariant_tostring( si->value, errcode) << std::endl;
					}
					break;
				default:
				{
					errcode = papuga_TypeError;
					return std::string();
				}
			}
		}
		return out.str();
	}
	catch (const std::bad_alloc&)
	{
		errcode = papuga_NoMemError;
		return std::string();
	}
}

