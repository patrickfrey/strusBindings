/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* @brief Logging functions for papuga wrapping to web request logging calls
 * @file papugaLoggerFunctions.cpp
 */
#include "papugaLoggerFunctions.hpp"
#include "webRequestContext.hpp"
#include "schemas_base.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/base/utf8.hpp"
#include "papuga/valueVariant.h"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/valueVariant.hpp"
#include "papuga/serialization.h"
#include "private/internationalization.hpp"
#include <vector>
#include <cstdarg>
#include <iostream>
#include <sstream>

using namespace strus;

static bool initValueVariant_length_cut( papuga_ValueVariant& newval, const papuga_ValueVariant& val, std::size_t maxlen)
{
	papuga_init_ValueVariant_value( &newval, &val);
	if (papuga_ValueVariant_isstring( &newval) && newval.length > maxlen)
	{
		newval.length = maxlen;
		if (newval.encoding == papuga_UTF8)
		{
			while (newval.length > 0 && utf8midchr( newval.value.string[ newval.length-1]))
			{
				--newval.length;
			}
		}
		return true;
	}
	return false;
}

static void ValueVariant_appendString_maxsize( std::string& out, const papuga_ValueVariant& val, std::size_t maxItemSize, std::size_t maxContentSize, int maxStructDepth)
{
	papuga_ErrorCode errcode = papuga_Ok;
	if (papuga_ValueVariant_isstring( &val))
	{
		papuga_ValueVariant newval;
		if (initValueVariant_length_cut( newval, val, maxItemSize))
		{
			out.push_back( '\"');
			if (!papuga::ValueVariant_append_string( out, newval, errcode)) throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
			out.append( " ...\"");
		}
		else
		{
			out.push_back( '\"');
			if (!papuga::ValueVariant_append_string( out, newval, errcode)) throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
			out.push_back( '\"');
		}
	}
	else if (papuga_ValueVariant_isatomic( &val))
	{
		if (!papuga::ValueVariant_append_string( out, val, errcode)) throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
	}
	else if (val.valuetype == papuga_TypeSerialization)
	{
		if (maxStructDepth > 0)
		{
			papuga_SerializationIter itr;
			papuga_init_SerializationIter( &itr, val.value.serialization);
			out.push_back( '{');
			bool separator = false;
			for (; !papuga_SerializationIter_eof( &itr); papuga_SerializationIter_skip( &itr))
			{
				if (out.size() > maxContentSize) break;

				switch (papuga_SerializationIter_tag( &itr))
				{
					case papuga_TagValue:
						if (maxStructDepth > 0)
						{
							if (separator) out.push_back(',');
							ValueVariant_appendString_maxsize( out, *papuga_SerializationIter_value( &itr), maxItemSize, maxContentSize, maxStructDepth-1);
						}
						separator = true;
						break;
					case papuga_TagOpen:
						--maxStructDepth;
						if (maxStructDepth > 0)
						{
							if (separator) out.push_back(',');
							out.push_back( '{');
						}
						separator = false;
						break;
					case papuga_TagClose:
						if (maxStructDepth > 0)
						{
							out.push_back( '}');
						}
						++maxStructDepth;
						separator = true;
						break;
					case papuga_TagName:
						if (maxStructDepth > 0)
						{
							if (separator) out.push_back(',');
							if (!papuga::ValueVariant_append_string( out, *papuga_SerializationIter_value( &itr), errcode)) throw std::runtime_error( papuga_ErrorCode_tostring( errcode));
							out.push_back( ':');
						}
						separator = false;
						break;
				}
			}
			out.push_back( '}');
		}
		else
		{
			out.append( "{}");
		}
	}
	else
	{
		out.push_back( '<');
		out.append( papuga_Type_name( val.valuetype));
		out.push_back( '>');
	}
	if (out.size() > maxContentSize)
	{
		std::size_t newSize = maxContentSize;
		while (newSize > 0 && utf8midchr( out[ newSize-1]))
		{
			--newSize;
		}
		out.resize( newSize);
		out.append( " ...");
	}
}

static std::vector<std::string> getLogArgument( int structDepth, std::size_t nof_arguments, va_list arguments, std::size_t nof_itypes, const papuga_RequestLogItem* itype, papuga_ErrorCode& errcode)
{
	std::vector<std::string> rt( nof_itypes);
	std::size_t nofargs = 0;
	int ai=0, ae=nof_arguments;
	for(; ai < ae; ++ai)
	{
		typedef int inttype;
		typedef const char* charp;

		papuga_RequestLogItem aitype = (papuga_RequestLogItem)va_arg( arguments, inttype);
		int ei=0, ee=nof_itypes;
		for(; ei < ee && itype[ ei] != aitype; ++ei){}
		switch (aitype)
		{
			case papuga_LogItemClassName:
			case papuga_LogItemMethodName:
			case papuga_LogItemMessage:
			case papuga_LogItemResultVariable:
				if (ei < ee) rt[ei] = va_arg( arguments,charp);
				break;
			case papuga_LogItemResult:
			{
				if (ei < ee)
				{
					papuga_ValueVariant* val = va_arg( arguments, papuga_ValueVariant*);
					if (val)
					{
						ValueVariant_appendString_maxsize( rt[ei], *val, WebRequestContext::MaxLogItemSize, WebRequestContext::MaxLogContentSize, WebRequestContext::MaxLogStructDepth);
					}
					else
					{
						rt[ei] = "<NULL>";
					}
				}
				break;
			}
			case papuga_LogItemArgc:
			{
				nofargs = va_arg( arguments,size_t);
				if (ei < ee)
				{
					std::ostringstream num;
					num << nofargs;
					rt[ei] = num.str();
				}
				break;
			}
			case papuga_LogItemArgv:
			{
				if (ei < ee)
				{
					papuga_ValueVariant* ar = va_arg( arguments, papuga_ValueVariant*);
					std::size_t ii=0, ie=nofargs;
					for (; ii!=ie; ++ii)
					{
						if (ii) rt[ ei].append( ", ");
						std::string argstr;
						ValueVariant_appendString_maxsize( argstr, ar[ ii], WebRequestContext::MaxLogItemSize, WebRequestContext::MaxLogContentSize, WebRequestContext::MaxLogStructDepth);
						rt[ ei].append( argstr);
					}
				}
				break;
			}
		}
	}
	return rt;
}

void strus::papugaLogMethodCall( void* self_, int nofItems, ...)
{
	WebRequestLoggerInterface* self = (WebRequestLoggerInterface*)self_;
	va_list arguments;
	va_start( arguments, nofItems );

	enum {nof_itypes=5};
	static const papuga_RequestLogItem itypes[nof_itypes] = {
		papuga_LogItemClassName,
		papuga_LogItemMethodName,
		papuga_LogItemArgv,
		papuga_LogItemResult,
		papuga_LogItemResultVariable
	};
	try
	{
		papuga_ErrorCode errcode = papuga_Ok;
		std::vector<std::string> args = getLogArgument( self->structDepth(), nofItems, arguments, nof_itypes, itypes, errcode);
		if (errcode == papuga_Ok)
		{
			self->logMethodCall( args[0].c_str(), args[1].c_str(), args[2].c_str(), args[3].c_str(), args[3].size(), args[4].c_str());
		}
		else
		{
			self->logError( papuga_ErrorCode_tostring( errcode));
		}
	}
	catch (const std::bad_alloc&)
	{
		self->logError( papuga_ErrorCode_tostring( papuga_NoMemError));
	}
	catch (const std::runtime_error& err)
	{
		self->logError( err.what());
	}
	catch (...)
	{
		self->logError( papuga_ErrorCode_tostring( papuga_UncaughtException));
	}
	va_end( arguments);
}

void strus::papugaLogContentEvent( void* self_, const char* title, int itemid, const papuga_ValueVariant* value)
{
	WebRequestLoggerInterface* self = (WebRequestLoggerInterface*)self_;
	try
	{
		const char* item =  itemid >= 0 ? strus::webrequest::AutomatonNameSpace::itemName( (strus::webrequest::AutomatonNameSpace::Item)itemid) : 0;

		if (value && papuga_ValueVariant_defined( value))
		{
			papuga_ErrorCode errcode = papuga_Ok;
			std::string valuestr = papuga::ValueVariant_tostring( *value, errcode);

			if (valuestr.empty() && errcode != papuga_Ok)
			{
				self->logError( papuga_ErrorCode_tostring( errcode));
			}
			else
			{
				self->logContentEvent( title, item, valuestr.c_str(), valuestr.size());
			}
		}
		else
		{
			self->logContentEvent( title, item, 0, 0);
		}
	}
	catch (const std::bad_alloc&)
	{
		self->logError( papuga_ErrorCode_tostring( papuga_NoMemError));
	}
	catch (const std::runtime_error& err)
	{
		self->logError( err.what());
	}
	catch (...)
	{
		self->logError( papuga_ErrorCode_tostring( papuga_UncaughtException));
	}
}


