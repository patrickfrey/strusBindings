/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "deserializer.hpp"
#include "private/internationalization.hpp"
#include "structDefs.hpp"
#include "papuga/serialization.h"
#include "papuga/valueVariant.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/utf8.hpp"
#include "strus/base/numstring.hpp"
#include "strus/base/base64.hpp"
#include "strus/lib/error.hpp"
#include "valueVariantWrap.hpp"
#include <string>
#include <limits>
#include <cstring>
#include <algorithm>

using namespace strus;
using namespace strus::bindings;

static std::runtime_error runtime_error_with_location( const char* msg, ErrorBufferInterface* errorhnd, const papuga_SerializationIter& errpos, const papuga_SerializationIter& startpos)
{
	std::string msgbuf;
	try
	{
		enum {WindowSize=32};
		static const char* errmarker = " <!> ";

		papuga_SerializationIter window[ WindowSize];
		papuga_SerializationIter seriter;
		int winpos = 0;
		int erridx = 0;
		int startidx = 0;
		int endidx = -1;
		int cnt = 0;
		std::memset( &window, 0, sizeof( window));
		papuga_init_SerializationIter_copy( &seriter, &startpos);

		while (!papuga_SerializationIter_isequal( &seriter, &errpos))
		{
			papuga_init_SerializationIter_copy( &window[ winpos++ % WindowSize], &seriter);
			papuga_SerializationIter_skip( &seriter);
		}
		erridx = winpos % WindowSize;
		if (winpos > WindowSize/2)
		{
			startidx = (winpos - (WindowSize/2)) % WindowSize;
		}
		else
		{
			startidx = 0;
		}
		endidx = startidx;
		for (; cnt < WindowSize/2; ++cnt,papuga_SerializationIter_skip( &seriter))
		{
			if (!papuga_SerializationIter_eof( &seriter))
			{
				papuga_init_SerializationIter_copy( &window[ winpos++ % WindowSize], &seriter);
			}
			else
			{
				if (endidx == startidx)
				{
					endidx = winpos % WindowSize;
				}
				std::memset( &window[ winpos++ % WindowSize], 0, sizeof(papuga_SerializationIter));
			}
		}

		std::string location_explain;
		int idx = startidx;
		for (; idx != endidx; ++idx)
		{
			if (!location_explain.empty())
			{
				location_explain.push_back( ' ');
			}
			if (idx == erridx)
			{
				location_explain.append( errmarker);
			}
			const papuga_ValueVariant* value = papuga_SerializationIter_value( &window[idx]);
			switch (papuga_SerializationIter_tag( &window[idx]))
			{
				case papuga_TagOpen:
					location_explain.push_back( '[');
					break;
				case papuga_TagClose:
					location_explain.push_back( ']');
					break;
				case papuga_TagName:
					location_explain.append( ValueVariantWrap::tostring( *value));
					location_explain.push_back( ':');
					break;
				case papuga_TagValue:
					if (!value)
					{
						location_explain.append( "<null>");
					}
					else if (!papuga_ValueVariant_defined( value))
					{
						location_explain.append( "<null>");
					}
					else if (papuga_ValueVariant_isatomic( value))
					{
						if (papuga_ValueVariant_isnumeric( value))
						{
							location_explain.append( ValueVariantWrap::tostring( *value));
						}
						else
						{
							location_explain.push_back( '"');
							location_explain.append( ValueVariantWrap::tostring( *value));
							location_explain.push_back( '"');
						}
					}
					else
					{
						location_explain.append( "<obj>");
					}
					break;
			}
		}
		if (endidx == startidx)
		{
			location_explain.append( " ...");
		}
		msgbuf.append( msg);
		if (errorhnd && errorhnd->hasError())
		{
			msgbuf.append( ": ");
			msgbuf.append(  errorhnd->fetchError());
		}
		if (!location_explain.empty())
		{
			msgbuf.append( string_format( _TXT(" (in expression at %s)"), location_explain.c_str()));
		}
		return strus::runtime_error( "%s", msgbuf.c_str());
	}
	catch (const std::exception&)
	{
		return strus::runtime_error( "%s", msg);
	}
}

static const papuga_ValueVariant* getValue( papuga_SerializationIter& seriter)
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		const papuga_ValueVariant* rt = papuga_SerializationIter_value( &seriter);
		papuga_SerializationIter_skip( &seriter);
		return rt;
	}
	else
	{
		throw strus::runtime_error( _TXT("expected value in structure"));
	}
}

unsigned int Deserializer::getUint( papuga_SerializationIter& seriter)
{
	return ValueVariantWrap::touint( *getValue( seriter));
}

int Deserializer::getInt( const papuga_ValueVariant& val, int defaultval)
{
	if (!papuga_ValueVariant_defined( &val)) return defaultval;
	return ValueVariantWrap::toint( val);
}

int Deserializer::getInt( papuga_SerializationIter& seriter)
{
	return ValueVariantWrap::toint( *getValue( seriter));
}

Index Deserializer::getIndex( papuga_SerializationIter& seriter)
{
	return ValueVariantWrap::toint( *getValue( seriter));
}

double Deserializer::getDouble( papuga_SerializationIter& seriter)
{
	return ValueVariantWrap::todouble( *getValue( seriter));
}

float Deserializer::getFloat( papuga_SerializationIter& seriter)
{
	return ValueVariantWrap::tofloat( *getValue( seriter));
}

NumericVariant Deserializer::getNumeric( papuga_SerializationIter& seriter)
{
	return ValueVariantWrap::tonumeric( *getValue( seriter));
}

static bool getCompareOperator( MetaDataRestrictionInterface::CompareOperator& res, const char* si)
{
	if (si[0] == '<')
	{
		if (si[1] == '=' && !si[2])
		{
			res = MetaDataRestrictionInterface::CompareLessEqual;
			return true;
		}
		else if (!si[1])
		{
			res = MetaDataRestrictionInterface::CompareLess;
			return true;
		}
	}
	else if (si[0] == '>')
	{
		if (si[1] == '=' && !si[2])
		{
			res = MetaDataRestrictionInterface::CompareGreaterEqual;
			return true;
		}
		else if (!si[1])
		{
			res = MetaDataRestrictionInterface::CompareGreater;
			return true;
		}
	}
	else if (si[0] == '!')
	{
		if (si[1] == '=' && !si[2])
		{
			res = MetaDataRestrictionInterface::CompareNotEqual;
			return true;
		}
	}
	else if (si[0] == '=')
	{
		if (si[1] == '=' && !si[2])
		{
			res = MetaDataRestrictionInterface::CompareEqual;
			return true;
		}
		else if (!si[1])
		{
			res = MetaDataRestrictionInterface::CompareEqual;
			return true;
		}
	}
	else if (si[0] == 'g')
	{
		if (si[1] == 't' && !si[2])
		{
			res = MetaDataRestrictionInterface::CompareGreater;
			return true;
		}
		else if (si[1] == 'e' && !si[2])
		{
			res = MetaDataRestrictionInterface::CompareGreaterEqual;
			return true;
		}
	}
	else if (si[0] == 'l')
	{
		if (si[1] == 't' && !si[2])
		{
			res = MetaDataRestrictionInterface::CompareLess;
			return true;
		}
		else if (si[1] == 'e' && !si[2])
		{
			res = MetaDataRestrictionInterface::CompareLessEqual;
			return true;
		}
	}
	else if (si[0] == 'e' && si[1] == 'q' && !si[2])
	{
		res = MetaDataRestrictionInterface::CompareEqual;
		return true;
	}
	else if (si[0] == 'n' && si[1] == 'e' && !si[2])
	{
		res = MetaDataRestrictionInterface::CompareNotEqual;
		return true;
	}
	return false;
}

MetaDataRestrictionInterface::CompareOperator Deserializer::getMetaDataCmpOp( papuga_SerializationIter& seriter)
{
	MetaDataRestrictionInterface::CompareOperator rt;
	char buf[ 32];
	const char* cmpopstr = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter), 0/*nonAsciiSubstChar*/);
	if (!cmpopstr || !getCompareOperator( rt, cmpopstr))
	{
		buf[ sizeof(buf)-1] = 0;
		throw strus::runtime_error(_TXT("unknown metadata compare operator '%s'"), buf);
	}
	papuga_SerializationIter_skip( &seriter);
	return rt;
}

std::string Deserializer::getString( papuga_SerializationIter& seriter)
{
	return ValueVariantWrap::tostring( *getValue( seriter));
}

std::string Deserializer::getBlobBase64Decoded( const papuga_ValueVariant& val)
{
	if (val.valuetype == papuga_TypeString)
	{
		if (val.encoding == papuga_UTF8 || val.encoding == papuga_Binary)
		{
			std::string rt( strus::base64DecodeLength( val.value.string, val.length), '\0');
			char* dest = const_cast<char*>( rt.c_str());
			ErrorCode errcode = (ErrorCode)0;
			std::size_t len = decodeBase64( dest, rt.size(), val.value.string, val.length, errcode);
			if (errcode) throw strus::runtime_error(_TXT("error decoding base64 encoded blob: %s"), errorCodeToString( errcode));
			rt.resize( len);
			return rt;
		}
		else
		{
			throw strus::runtime_error(_TXT("value of UTF8 or binary string for base64 encoded blob expected"));
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("value of type string for base64 encoded blob expected"));
	}
}

std::string Deserializer::getBlobBase64Decoded( papuga_SerializationIter& seriter)
{
	return Deserializer::getBlobBase64Decoded( *getValue( seriter));
}

bool Deserializer::compareName( const papuga_SerializationIter& seriter, const char* name, std::size_t namelen)
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		const papuga_ValueVariant* val = papuga_SerializationIter_value( &seriter);
		if (val->valuetype == papuga_TypeString && val->encoding == papuga_UTF8)
		{
			return (namelen == val->length && 0==std::memcmp( name, val->value.string, namelen));
		}
		else
		{
			char buf[ 256];
			const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), val, 0/*nonAsciiSubstChar*/);
			return (id && namelen < sizeof(buf) && std::strlen(id) == namelen && 0==std::memcmp( name, buf, namelen));
		}
	}
	return false;
}

const char* Deserializer::getCharpAscii( char* buf, std::size_t bufsize, papuga_SerializationIter& seriter)
{
	const papuga_ValueVariant* vp = getValue( seriter);
	const char* rt = papuga_ValueVariant_toascii( buf, bufsize, vp, 0/*nonAsciiSubstChar*/);
	if (!rt) throw strus::runtime_error(_TXT("ascii string with maximum size %u expected"), (unsigned int)bufsize);
	return rt;
}

static int getUnicodeCharValueFromString( const char* str, std::size_t length)
{
	if (!length)
	{
		throw strus::runtime_error(_TXT("single unicode character or decimal or hexadecimal number representation of it expected"));
	}
	else if (length != strus::utf8charlen( str[0]))
	{
		if (str[0] >= '0' && str[0] <= '9')
		{
			strus::NumParseError err = NumParseOk;
			int rt = strus::uintFromString( str, length, std::numeric_limits<int>::max(), err);
			if (!rt || err != NumParseOk)
			{
				throw strus::runtime_error(_TXT("failed to parse unicode character as decimal or hexadecimal number in the allowed range"));
			}
			return rt;
		}
		else
		{
			throw strus::runtime_error(_TXT("single unicode character or decimal or hexadecimal number representation of it expected"));
		}
	}
	else
	{
		return strus::utf8decode( str, length);
	}
}

int Deserializer::getCharUnicode( papuga_SerializationIter& seriter)
{
	const papuga_ValueVariant* vp = getValue( seriter);
	if (vp->valuetype == papuga_TypeInt)
	{
		return vp->value.Int;
	}
	else if (vp->valuetype == papuga_TypeString && vp->encoding == papuga_UTF8)
	{
		return getUnicodeCharValueFromString( vp->value.string, vp->length);
	}
	else 
	{
		std::string str = ValueVariantWrap::tostring( *vp);
		return getUnicodeCharValueFromString( str.c_str(), str.size());
	}
}

char Deserializer::getCharAscii( papuga_SerializationIter& seriter)
{
	int uchr = getCharUnicode( seriter);
	if (uchr > 127) throw strus::runtime_error(_TXT("single ascii character expected"));
	return uchr;
}

static std::vector<int> unicodeCharList( const char* start, std::size_t size)
{
	std::vector<int> rt;
	char const* itr = start;
	const char* end = itr + size;
	while (itr < end)
	{
		std::size_t chrlen = strus::utf8charlen( itr[0]);
		rt.push_back( strus::utf8decode( itr, chrlen));
		itr += chrlen;
	}
	return rt;
}

std::vector<int> Deserializer::getCharListUnicode( papuga_SerializationIter& seriter)
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		std::vector<int> rt;
		papuga_SerializationIter_skip( &seriter);
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			rt.push_back( getCharUnicode( seriter));
		}
		consumeClose( seriter);
		return rt;
	}
	else
	{
		const papuga_ValueVariant* vp = getValue( seriter);
		if (vp->valuetype == papuga_TypeInt)
		{
			std::vector<int> rt;
			rt.push_back( vp->value.Int);
			return rt;
		}
		else if (vp->valuetype == papuga_TypeString)
		{
			if (vp->encoding == papuga_UTF8)
			{
				const char* str = vp->value.string;
				return unicodeCharList( str, vp->length);
			}
			else
			{
				std::string str = ValueVariantWrap::tostring( *vp);
				return unicodeCharList( str.c_str(), str.size());
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("string or list of integers expected as unicode character list"));
		}
	}
}

void Deserializer::consumeClose( papuga_SerializationIter& seriter)
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagClose)
	{
		if (papuga_SerializationIter_eof( &seriter))
		{
			throw strus::runtime_error( _TXT("unexpected eof, close expected element at end of structure"));
		}
		papuga_SerializationIter_skip( &seriter);
	}
	else
	{
		throw strus::runtime_error( _TXT("close expected element at end of structure"));
	}
}

template <typename ATOMICTYPE, ATOMICTYPE FUNC( papuga_SerializationIter& seriter)>
static std::vector<ATOMICTYPE> getAtomicTypeList( papuga_SerializationIter& seriter)
{
	std::vector<ATOMICTYPE> rt;
	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		rt.push_back( FUNC( seriter));
	}
	return rt;
}

template <typename ATOMICTYPE, ATOMICTYPE FUNC( papuga_SerializationIter& seriter)>
static std::vector<ATOMICTYPE> getAtomicTypeListAsValue( papuga_SerializationIter& seriter)
{
	std::vector<ATOMICTYPE> rt;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			rt.push_back( FUNC( seriter));
		}
		Deserializer::consumeClose( seriter);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		rt.push_back( FUNC( seriter));
	}
	else
	{
		throw strus::runtime_error( _TXT("list of atomic types expected"));
	}
	return rt;
}

template <typename ATOMICTYPE, ATOMICTYPE CONV( const papuga_ValueVariant& val), ATOMICTYPE FUNC( papuga_SerializationIter& seriter)>
static std::vector<ATOMICTYPE> getAtomicTypeList( const papuga_ValueVariant& val)
{
	std::vector<ATOMICTYPE> rt;
	if (!papuga_ValueVariant_defined( &val))
	{
		return rt;
	}
	else if (val.valuetype != papuga_TypeSerialization)
	{
		rt.push_back( CONV( val));
		return rt;
	}
	else
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, val.value.serialization);

		rt = getAtomicTypeList<ATOMICTYPE,FUNC>( seriter);
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization"));
	}
	return rt;
}

std::vector<std::string> Deserializer::getStringList( papuga_SerializationIter& seriter)
{
	return getAtomicTypeList<std::string,getString>( seriter);
}

std::vector<std::string> Deserializer::getStringListAsValue( papuga_SerializationIter& seriter)
{
	return getAtomicTypeListAsValue<std::string,getString>( seriter);
}

std::vector<std::string> Deserializer::getStringList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<std::string,ValueVariantWrap::tostring,getString>( val);
}

std::vector<double> Deserializer::getDoubleList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<double,ValueVariantWrap::todouble,Deserializer::getDouble>( val);
}

std::vector<float> Deserializer::getFloatList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<float,ValueVariantWrap::tofloat,Deserializer::getFloat>( val);
}

std::vector<int> Deserializer::getIntList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<int,ValueVariantWrap::toint,Deserializer::getInt>( val);
}

std::vector<Index> Deserializer::getIndexList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<Index,ValueVariantWrap::toint,Deserializer::getIndex>( val);
}

IndexRange Deserializer::getIndexRange( papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "start,end", ',');
	enum StructureNameId {I_start=0,I_end=1};
	static const char* context = _TXT("index range");

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		char defined[ 2] = {0,0};
		int start = 0;
		int end = 0;

		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch ((StructureNameId)idx)
			{
				case I_start:
					if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s structure"), namemap.name(idx), context);
					start = getIndex( seriter);
				break;
				case I_end:
					if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s structure"), namemap.name(idx), context);
					end = getIndex( seriter);
				break;
				default:
					throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
			}
		}
		return IndexRange( start, end);
	}
	else
	{
		Index start = getIndex( seriter);
		Index end = getIndex( seriter);
		return IndexRange( start, end);
	}
}

IndexRange Deserializer::getIndexRangeAsValue( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("index range");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		IndexRange rt = getIndexRange( seriter);
		Deserializer::consumeClose( seriter);
		return rt;
	}
	else
	{
		throw strus::runtime_error(_TXT("expected structure for %s"), context);
	}
}

analyzer::DocumentStructure::PositionRange Deserializer::getAnalyzerPositionRange( papuga_SerializationIter& seriter)
{
	IndexRange res = getIndexRange( seriter);
	return analyzer::DocumentStructure::PositionRange( res.start(), res.end());
}

analyzer::DocumentStructure::PositionRange Deserializer::getAnalyzerPositionRangeAsValue( papuga_SerializationIter& seriter)
{
	IndexRange res = getIndexRangeAsValue( seriter);
	return analyzer::DocumentStructure::PositionRange( res.start(), res.end());
}

static void setFeatureOption_position( analyzer::FeatureOptions& res, const papuga_ValueVariant* val)
{
	char buf[ 128];
	const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), val, 0/*nonAsciiSubstChar*/);
	if (!id) throw strus::runtime_error(_TXT("cannot convert value to feature position option"));

	if (0==std::strcmp( id, "content"))
	{
		res.definePositionBind( analyzer::BindContent);
	}
	else if (0==std::strcmp( id, "succ"))
	{
		res.definePositionBind( analyzer::BindSuccessor);
	}
	else if (0==std::strcmp( id, "pred"))
	{
		res.definePositionBind( analyzer::BindPredecessor);
	}
	else if (0==std::strcmp( id, "unique"))
	{
		res.definePositionBind( analyzer::BindPredecessor);
	}
	else
	{
		throw strus::runtime_error(_TXT("unknown feature position option: '%s'"), id);
	}
}


template <class SimpleStructureParser, class Structure>
class SimpleStructureParserBase
{
public:
	static Structure parse( papuga_SerializationIter& seriter, const char* context)
	{
		try
		{
			Structure rt;
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				int pos = 0;
				do
				{
					SimpleStructureParser::setPositionalValue( rt, pos++, getValue( seriter));
				}
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagValue);
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
				{
					static const StructureNameMap metanamemap( "name,value", ',');
					for (;;)
					{
						std::string name;
						const papuga_ValueVariant* value = 0;
						do
						{
							int idx = metanamemap.index( *papuga_SerializationIter_value( &seriter));
							papuga_SerializationIter_skip( &seriter);
							switch (idx)
							{
								case 0: name = Deserializer::getString( seriter); break;
								case 1: value = getValue( seriter); break;
								default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
							}
							papuga_SerializationIter_skip( &seriter);
						}
						while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
						if (!value) throw strus::runtime_error(_TXT("missing definition of value"));
						SimpleStructureParser::setNamedValue( rt, name.c_str(), value);
						Deserializer::consumeClose( seriter);

						if (papuga_SerializationIter_tag( &seriter) != papuga_TagOpen)
						{
							break;
						}
						papuga_SerializationIter_skip( &seriter);
					}
				}
				else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
				{
					for(;;)
					{
						char buf[ 128];
						const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter), 0/*nonAsciiSubstChar*/);
						papuga_SerializationIter_skip( &seriter);
						SimpleStructureParser::setNamedValue( rt, id, getValue( seriter));
						Deserializer::consumeClose( seriter);

						if (papuga_SerializationIter_tag( &seriter) != papuga_TagOpen)
						{
							break;
						}
						papuga_SerializationIter_skip( &seriter);
					}
				}
				Deserializer::consumeClose( seriter);
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				do
				{
					char buf[ 128];
					const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter), 0/*nonAsciiSubstChar*/);
					if (!id) throw strus::runtime_error(_TXT("unknown identifier"));
					papuga_SerializationIter_skip( &seriter);
					SimpleStructureParser::setNamedValue( rt, id, getValue( seriter));
				}
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
			}
			return rt;
		}
		catch (const std::runtime_error& err)
		{
			throw strus::runtime_error( _TXT("error parsing %s: %s"), context, err.what());
		}
	}

	static Structure parse( const papuga_ValueVariant& structvalue, const char* context)
	{
		Structure rt;
		if (structvalue.valuetype == papuga_TypeSerialization)
		{
			papuga_SerializationIter seriter;
			papuga_init_SerializationIter( &seriter, structvalue.value.serialization);
			if (papuga_SerializationIter_eof( &seriter)) return rt;
			rt = parse( seriter, context);
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of %s"), context);
		}
		else if (papuga_ValueVariant_defined( &structvalue))
		{
			SimpleStructureParser::setSingleValue( rt, &structvalue);
		}
		return rt;
	}
};

class FeatureOptionsParser
	:public SimpleStructureParserBase<FeatureOptionsParser,analyzer::FeatureOptions>
{
public:
	static void setNamedValue( analyzer::FeatureOptions& ths, const char* name, const papuga_ValueVariant* value)
	{
		static const StructureNameMap namemap( "position", ',');

		int idx = namemap.index( name);
		switch (idx)
		{
			case 0: setFeatureOption_position( ths, value);
				break;
			default: throw strus::runtime_error(_TXT("unknown tag name '%s'"), name);
				break;
		}
	}
	static void setSingleValue( analyzer::FeatureOptions& ths, const papuga_ValueVariant* value)
	{
		setFeatureOption_position( ths, value);
	}
	static void setPositionalValue( analyzer::FeatureOptions& ths, int pos, const papuga_ValueVariant* value)
	{
		if (pos != 0) throw strus::runtime_error(_TXT("to many positional values defined in structure"));
		setSingleValue( ths, value);
	}
};

analyzer::FeatureOptions Deserializer::getFeatureOptions( papuga_SerializationIter& seriter)
{
	return FeatureOptionsParser::parse( seriter, _TXT("feature option list"));
}

analyzer::FeatureOptions Deserializer::getFeatureOptions( const papuga_ValueVariant& options)
{
	return FeatureOptionsParser::parse( options, _TXT("feature option list"));
}

class TermStatisticsParser
	:public SimpleStructureParserBase<TermStatisticsParser,TermStatistics>
{
public:
	static void setNamedValue( TermStatistics& ths, const char* name, const papuga_ValueVariant* value)
	{
		static const StructureNameMap namemap( "df", ',');

		int idx = namemap.index( name);
		switch (idx)
		{
			case 0: ths.setDocumentFrequency( ValueVariantWrap::touint64( *value));
				break;
			default: throw strus::runtime_error(_TXT("unknown tag name '%s'"), name);
				break;
		}
	}
	static void setSingleValue( TermStatistics& ths, const papuga_ValueVariant* value)
	{
		ths.setDocumentFrequency( ValueVariantWrap::touint64( *value));
	}
	static void setPositionalValue( TermStatistics& ths, int pos, const papuga_ValueVariant* value)
	{
		if (pos != 0) throw strus::runtime_error(_TXT("to many positional values defined in structure"));
		setSingleValue( ths, value);
	}
};

TermStatistics Deserializer::getTermStatistics( const papuga_ValueVariant& val)
{
	return TermStatisticsParser::parse( val, _TXT("term statistics"));
}

class GlobalStatisticsParser
	:public SimpleStructureParserBase<GlobalStatisticsParser,GlobalStatistics>
{
public:
	static void setNamedValue( GlobalStatistics& ths, const char* name, const papuga_ValueVariant* value)
	{
		static const StructureNameMap namemap( "nofdocs", ',');

		int idx = namemap.index( name);
		switch (idx)
		{
			case 0: ths.setNofDocumentsInserted( ValueVariantWrap::touint64( *value));
				break;
			default: throw strus::runtime_error(_TXT("unknown tag name '%s'"), name);
				break;
		}
	}
	static void setSingleValue( GlobalStatistics& ths, const papuga_ValueVariant* value)
	{
		ths.setNofDocumentsInserted( ValueVariantWrap::touint64( *value));
	}
	static void setPositionalValue( GlobalStatistics& ths, int pos, const papuga_ValueVariant* value)
	{
		if (pos != 0) throw strus::runtime_error(_TXT("to many positional values defined in structure"));
		setSingleValue( ths, value);
	}
};

GlobalStatistics Deserializer::getGlobalStatistics( const papuga_ValueVariant& val)
{
	return GlobalStatisticsParser::parse( val, _TXT("term statistics"));
}

TimeStamp Deserializer::getTimeStamp( papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "unixtime,counter", ',');
	enum StructureNameId {I_unixtime=0,I_counter=1};
	static const char* context = _TXT("timestamp");

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		bool defined[ 2] = {false,false};
		int unixtime = 0;
		int counter = 0;

		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			StructureNameId idx = (StructureNameId)namemap.index( *papuga_SerializationIter_value( &seriter));
			if (idx >= 0 && defined[idx])
			{
				throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), namemap.name(idx), context);
			}
			defined[idx] = true;

			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case I_unixtime:
					unixtime = Deserializer::getInt( seriter);
					break;
				case I_counter:
					counter = Deserializer::getInt( seriter);
					break;
				default:
					throw strus::runtime_error(_TXT("unknown definition in %s expected one of %s"), context, "{unixtime,counter}");
					break;
			}
		}
		return TimeStamp( unixtime, counter);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		int unixtime = 0;
		int counter = 0;
		if (papuga_ValueVariant_defined( papuga_SerializationIter_value( &seriter)))
		{
			unixtime = Deserializer::getInt( seriter);
		}
		else
		{
			papuga_SerializationIter_skip( &seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			counter = Deserializer::getInt( seriter);
		}
		return TimeStamp( unixtime, counter);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagClose)
	{
		return TimeStamp( 0, 0);
	}
	else
	{
		throw strus::runtime_error(_TXT("%s definition expected"), context);
	}
}

TimeStamp Deserializer::getTimeStamp( const papuga_ValueVariant& tmstmp)
{
	static const char* context = _TXT("timestamp");

	if (!papuga_ValueVariant_defined( &tmstmp))
	{
		return TimeStamp();
	}
	else if (tmstmp.valuetype == papuga_TypeSerialization)
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, tmstmp.value.serialization);
		if (papuga_SerializationIter_eof( &seriter)) return TimeStamp();
		TimeStamp rt = getTimeStamp( seriter);
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of %s"), context);
		return rt;
	}
	else
	{
		return TimeStamp( ValueVariantWrap::toint( tmstmp));
	}
}

StatisticsMessage Deserializer::getStatisticsMessage( papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "timestamp,blob", ',');
	enum StructureNameId {I_timestamp=0,I_blob=1};
	static const char* context = _TXT("statistics message");

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		bool defined[ 2] = {false,false};
		TimeStamp timestamp;
		std::string blob;

		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			StructureNameId idx = (StructureNameId)namemap.index( *papuga_SerializationIter_value( &seriter));
			if (idx >= 0 && defined[idx])
			{
				throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), namemap.name(idx), context);
			}
			defined[idx] = true;

			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case I_timestamp:
					if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
					{
						timestamp = getTimeStamp( *papuga_SerializationIter_value( &seriter));
						papuga_SerializationIter_skip( &seriter);
					}
					else if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
					{
						papuga_SerializationIter_skip( &seriter);
						timestamp = getTimeStamp( seriter);
						Deserializer::consumeClose( seriter);
					}
					else
					{
						throw strus::runtime_error(_TXT("structure or value expected for %s in %s"), "timestamp", context);
					}
					break;
				case I_blob:
					blob = getBlobBase64Decoded( seriter);
					break;
				default:
					throw strus::runtime_error(_TXT("unknown definition in %s expected one of %s"), context, "{unixtime,counter}");
					break;
			}
		}
		return StatisticsMessage( blob, timestamp);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		TimeStamp timestamp = getTimeStamp( *papuga_SerializationIter_value( &seriter));
		std::string blob = getBlobBase64Decoded( seriter);
		return StatisticsMessage( blob, timestamp);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		TimeStamp timestamp = getTimeStamp( *papuga_SerializationIter_value( &seriter));
		Deserializer::consumeClose( seriter);
		std::string blob = getBlobBase64Decoded( seriter);
		return StatisticsMessage( blob, timestamp);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagClose)
	{
		return StatisticsMessage();
	}
	else
	{
		throw strus::runtime_error(_TXT("%s definition expected"), context);
	}
}

StatisticsMessage Deserializer::getStatisticsMessage( const papuga_ValueVariant& msg)
{
	static const char* context = _TXT("statistics message");

	if (!papuga_ValueVariant_defined( &msg))
	{
		return StatisticsMessage();
	}
	else if (msg.valuetype == papuga_TypeSerialization)
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, msg.value.serialization);
		if (papuga_SerializationIter_eof( &seriter)) return StatisticsMessage();
		StatisticsMessage rt = getStatisticsMessage( seriter);
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of %s"), context);
		return rt;
	}
	else
	{
		std::string blob = Deserializer::getBlobBase64Decoded( msg);
		return StatisticsMessage( blob, TimeStamp());
	}
}

SummaryElement Deserializer::getSummaryElement( papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "name,value,weight,index", ',');
	enum StructureNameId {I_name=0,I_value=1,I_weight=2,I_index=3};
	static const char* context = _TXT("summary element");

	std::string name_;
	std::string value_;
	double weight_=1.0;
	int index_=-1;

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		bool defined[ 4] = {false,false,false,false};

		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			StructureNameId idx = (StructureNameId)namemap.index( *papuga_SerializationIter_value( &seriter));
			if (idx >= 0 && defined[idx])
			{
				throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), namemap.name(idx), context);
			}
			defined[idx] = true;

			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case I_name: name_=getString( seriter); break;
				case I_value: value_=getString( seriter); break;
				case I_weight: weight_=getFloat( seriter); break;
				case I_index: index_=getIndex( seriter); break;
			}
		}
		if (!defined[ I_name]) throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_name), context);
		if (!defined[ I_value]) throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_value), context);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		name_=getString( seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			value_=getString( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_value), context);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			weight_=getFloat( seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			index_=getIndex( seriter);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("expected non empty structure for %s"), context);
	}
	return SummaryElement( name_, value_, weight_, index_);
}

std::vector<SummaryElement> Deserializer::getSummaryElementList( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("summary element list");
	std::vector<SummaryElement> rt;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				papuga_SerializationIter_skip( &seriter);
				rt.push_back( getSummaryElement( seriter));
				consumeClose( seriter);
			}
			else
			{
				throw strus::runtime_error(_TXT("expected structures as elements of %s"), context);
			}
		}
	}
	else
	{
		rt.push_back( getSummaryElement( seriter));
	}
	return rt;
}

std::vector<SummaryElement> Deserializer::getSummaryElementListValue( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("summary element list");
	std::vector<SummaryElement> rt;
	if (papuga_SerializationIter_tag( &seriter) != papuga_TagOpen)
	{
		throw strus::runtime_error(_TXT("expected structure for %s"), context);
	}
	papuga_SerializationIter_skip( &seriter);
	rt = getSummaryElementList( seriter);
	consumeClose( seriter);
	return rt;
}

ResultDocument Deserializer::getResultDocument( papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "docno,weight,field,summary", ',');
	enum StructureNameId {I_docno=0,I_weight=1,I_field=2,I_summary=3};
	static const char* context = _TXT("result document");

	strus::Index docno_=0;
	double weight_=0.0;
	strus::IndexRange field_;
	std::vector<SummaryElement> summary_;

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		bool defined[ 4] = {false,false,false,false};

		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			StructureNameId idx = (StructureNameId)namemap.index( *papuga_SerializationIter_value( &seriter));
			if (idx >= 0 && defined[idx])
			{
				throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), namemap.name(idx), context);
			}
			defined[idx] = true;

			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case I_docno: docno_=getIndex( seriter); break;
				case I_weight: weight_=getFloat( seriter); break;
				case I_field: field_=getIndexRangeAsValue( seriter); break;
				case I_summary: summary_=getSummaryElementListValue( seriter); break;
			}
		}
		if (!defined[ I_docno]) throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_docno), context);
		if (!defined[ I_weight]) throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_weight), context);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		docno_=getIndex( seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			weight_=getFloat( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_weight), context);
		}
		if (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			field_=getIndexRangeAsValue( seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			summary_=getSummaryElementListValue( seriter);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("expected non empty structure for %s"), context);
	}
	return ResultDocument( docno_, field_, weight_, summary_);
}

std::vector<ResultDocument> Deserializer::getResultDocumentList( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("result document list");
	std::vector<ResultDocument> rt;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				papuga_SerializationIter_skip( &seriter);
				rt.push_back( getResultDocument( seriter));
				consumeClose( seriter);
			}
			else
			{
				throw strus::runtime_error(_TXT("expected structures as elements of %s"), context);
			}
		}
	}
	else
	{
		rt.push_back( getResultDocument( seriter));
	}
	return rt;
}

std::vector<ResultDocument> Deserializer::getResultDocumentListValue( papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("result document list");
	std::vector<ResultDocument> rt;
	if (papuga_SerializationIter_tag( &seriter) != papuga_TagOpen)
	{
		throw strus::runtime_error(_TXT("expected structure for %s"), context);
	}
	papuga_SerializationIter_skip( &seriter);
	rt = getResultDocumentList( seriter);
	consumeClose( seriter);
	return rt;
}

QueryResult Deserializer::getQueryResult( papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "evalpass,nofranked,nofvisited,ranks", ',');
	enum StructureNameId {I_evalpass=0,I_nofranked=1,I_nofvisited=2,I_ranks=3};
	static const char* context = _TXT("query result");

	int evaluationPass_=0;
	int nofRanked_=0;
	int nofVisited_=0;
	std::vector<ResultDocument> ranks_;
	
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		bool defined[ 4] = {false,false,false,false};

		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			StructureNameId idx = (StructureNameId)namemap.index( *papuga_SerializationIter_value( &seriter));
			if (idx >= 0 && defined[idx])
			{
				throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), namemap.name(idx), context);
			}
			defined[idx] = true;

			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case I_evalpass: evaluationPass_=getIndex( seriter); break;
				case I_nofranked: nofRanked_=getInt( seriter); break;
				case I_nofvisited: nofVisited_=getInt( seriter); break;
				case I_ranks: ranks_=getResultDocumentListValue( seriter); break;
			}
		}
		if (!defined[ I_nofranked]) throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_nofranked), context);
		if (!defined[ I_nofvisited]) throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_nofvisited), context);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		evaluationPass_=getIndex( seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			nofRanked_=getInt( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_nofranked), context);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			nofVisited_=getInt( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("missing mandatory element %s in %s"), namemap.name(I_nofvisited), context);
		}
		if (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			ranks_=getResultDocumentListValue( seriter);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("expected non empty structure for %s"), context);
	}
	return QueryResult( evaluationPass_, nofRanked_, nofVisited_, ranks_);
}

QueryResult Deserializer::getQueryResult( const papuga_ValueVariant& res)
{
	static const char* context = _TXT("query result");
	if (res.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("expected non empty structure for %s"), context);
	}
	else
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, res.value.serialization);
		return getQueryResult( seriter);
	}
}

std::vector<QueryResult> Deserializer::getQueryResultList( papuga_SerializationIter& seriter)
{
	std::vector<QueryResult> rt;
	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue
		&&  papuga_SerializationIter_value( &seriter)->valuetype == papuga_TypeSerialization)
		{
			rt.push_back( getQueryResult( *papuga_SerializationIter_value( &seriter)));
			papuga_SerializationIter_skip( &seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			rt.push_back( getQueryResult( seriter));
			consumeClose( seriter);
		}
		else
		{
			rt.push_back( getQueryResult( seriter));
			// ... single result returned as list with one element
		}
	}
	return rt;
}

std::vector<QueryResult> Deserializer::getQueryResultList( const papuga_ValueVariant& res)
{
	static const char* context = _TXT("query result list");
	if (res.valuetype == papuga_TypeSerialization)
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, res.value.serialization);
		return getQueryResultList( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected structure for %s"), context);
	}
}

const papuga_ValueVariant* Deserializer::getOptionalDefinition( papuga_SerializationIter& seriter, const char* name)
{
	const papuga_ValueVariant* rt = 0;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		char buf[ 128];
		const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter), 0/*nonAsciiSubstChar*/);

		if (id && 0==std::strcmp( id, name))
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				rt = papuga_SerializationIter_value( &seriter);
				papuga_SerializationIter_skip( &seriter);
			}
			else
			{
				throw strus::runtime_error(_TXT("option structure (like '%s' declaration) as element of structure expected"), name);
			}
		}
	}
	return rt;
}

class DocumentClassParser
	:public SimpleStructureParserBase<DocumentClassParser,analyzer::DocumentClass>
{
public:
	static void setNamedValue( analyzer::DocumentClass& ths, const char* name, const papuga_ValueVariant* value)
	{
		static const StructureNameMap namemap( "mimetype,encoding,schema", ',');

		int idx = namemap.index( name);
		switch (idx)
		{
			case 0: ths.setMimeType( ValueVariantWrap::tostring( *value));
				break;
			case 1: ths.setEncoding( ValueVariantWrap::tostring( *value));
				break;
			case 2: ths.setSchema( ValueVariantWrap::tostring( *value));
				break;
			default: throw strus::runtime_error(_TXT("unknown tag name '%s'"), name);
		}
	}
	static void setSingleValue( analyzer::DocumentClass& ths, const papuga_ValueVariant* value)
	{
		ths.setMimeType( ValueVariantWrap::tostring( *value));
	}
	static void setPositionalValue( analyzer::DocumentClass& ths, int pos, const papuga_ValueVariant* value)
	{
		switch (pos)
		{
			case 0: ths.setMimeType( ValueVariantWrap::tostring( *value));
				break;
			case 1: ths.setEncoding( ValueVariantWrap::tostring( *value));
				break;
			case 2: ths.setSchema( ValueVariantWrap::tostring( *value));
				break;
			default: throw strus::runtime_error(_TXT("to many positional values defined in structure"));
		}
	}
};

analyzer::DocumentClass Deserializer::getDocumentClass(
		const papuga_ValueVariant& val)
{
	return DocumentClassParser::parse( val, _TXT("document class"));
}

static Reference<NormalizerFunctionInstanceInterface> getNormalizer_(
		const std::string& name,
		const std::vector<std::string> arguments,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("normalizer function");
	const NormalizerFunctionInterface* nf = textproc->getNormalizer( name);
	if (!nf) throw strus::runtime_error( _TXT("failed to get %s '%s': %s"), context, name.c_str(), errorhnd->fetchError());

	Reference<NormalizerFunctionInstanceInterface> function( nf->createInstance( arguments, textproc));
	if (!function.get())
	{
		throw strus::runtime_error( _TXT("failed to create %s instance '%s': %s"),
						context, name.c_str(), errorhnd->fetchError());
	}
	return function;
}

static Reference<TokenizerFunctionInstanceInterface> getTokenizer_(
		const std::string& name,
		const std::vector<std::string> arguments,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	const TokenizerFunctionInterface* nf = textproc->getTokenizer( name);
	static const char* context = _TXT("tokenizer function");
	if (!nf) throw strus::runtime_error( _TXT("failed to get %s '%s': %s"),
						context, name.c_str(), errorhnd->fetchError());

	Reference<TokenizerFunctionInstanceInterface> function( nf->createInstance( arguments, textproc));
	if (!function.get())
	{
		throw strus::runtime_error( _TXT("failed to create %s function instance '%s': %s"), context, name.c_str(), errorhnd->fetchError());
	}
	return function;
}

static Reference<AggregatorFunctionInstanceInterface> getAggregator_(
		const std::string& name,
		const std::vector<std::string> arguments,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	const AggregatorFunctionInterface* nf = textproc->getAggregator( name);
	static const char* context = _TXT("aggregator function");
	if (!nf) throw strus::runtime_error( _TXT("failed to get %s function '%s': %s"), context, name.c_str(), errorhnd->fetchError());

	Reference<AggregatorFunctionInstanceInterface> function( nf->createInstance( arguments));
	if (!function.get())
	{
		throw strus::runtime_error( _TXT("failed to create %s function instance '%s': %s"), context, name.c_str(), errorhnd->fetchError());
	}
	return function;
}

static Reference<NormalizerFunctionInstanceInterface> getNormalizer_(
		papuga_SerializationIter& seriter,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (papuga_SerializationIter_tag(&seriter) == papuga_TagClose) return Reference<NormalizerFunctionInstanceInterface>();
	AnalyzerFunctionDef def( seriter);
	return getNormalizer_( def.name, def.args, textproc, errorhnd);
}

Reference<TokenizerFunctionInstanceInterface> Deserializer::getTokenizer(
		papuga_SerializationIter& seriter,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (papuga_SerializationIter_tag(&seriter) == papuga_TagClose) return Reference<TokenizerFunctionInstanceInterface>();
	AnalyzerFunctionDef def( seriter);
	return getTokenizer_( def.name, def.args, textproc, errorhnd);
}

Reference<AggregatorFunctionInstanceInterface> Deserializer::getAggregator(
		papuga_SerializationIter& seriter,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (papuga_SerializationIter_tag(&seriter) == papuga_TagClose) return Reference<AggregatorFunctionInstanceInterface>();
	AnalyzerFunctionDef def( seriter);
	return getAggregator_( def.name, def.args, textproc, errorhnd);
}

std::vector<Reference<NormalizerFunctionInstanceInterface> > Deserializer::getNormalizers(
		papuga_SerializationIter& seriter,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context =  _TXT("normalizers");
	std::vector< Reference<NormalizerFunctionInstanceInterface> > rt;
	while (papuga_SerializationIter_tag(&seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag(&seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			rt.push_back( getNormalizer_( seriter, textproc, errorhnd));
			if (papuga_SerializationIter_tag(&seriter) == papuga_TagClose)
			{
				if (papuga_SerializationIter_eof(&seriter))
				{
					throw strus::runtime_error( _TXT("unexpected eof in serialization of %s"), context);
				}
				papuga_SerializationIter_skip( &seriter);
			}
			else
			{
				throw strus::runtime_error( _TXT("unexpected tag in serialization of %s"), context);
			}
		}
		else if (papuga_SerializationIter_tag(&seriter) == papuga_TagValue)
		{
			std::string normalizername = ValueVariantWrap::tostring( *papuga_SerializationIter_value(&seriter));
			papuga_SerializationIter_skip( &seriter);
			rt.push_back( getNormalizer_( normalizername, std::vector<std::string>(), textproc, errorhnd));
		}
		else
		{
			rt.push_back( getNormalizer_( seriter, textproc, errorhnd));
		}
	}
	return rt;
}

std::vector<Reference<NormalizerFunctionInstanceInterface> > Deserializer::getNormalizers(
		const papuga_ValueVariant& normalizers,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context =  _TXT("normalizers");
	std::vector<Reference<NormalizerFunctionInstanceInterface> > rt;
	if (normalizers.valuetype != papuga_TypeSerialization)
	{
		std::string name = ValueVariantWrap::tostring( normalizers);
		rt.push_back( getNormalizer_( name, std::vector<std::string>(), textproc, errorhnd));
		return rt;
	}
	else
	{
		papuga_SerializationIter seriter, serstart;
		papuga_init_SerializationIter( &serstart, normalizers.value.serialization);
		papuga_init_SerializationIter( &seriter, normalizers.value.serialization);
		try
		{
			rt = getNormalizers( seriter, textproc, errorhnd);
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
		}
	}
	return rt;
}


Reference<TokenizerFunctionInstanceInterface> Deserializer::getTokenizer(
		const papuga_ValueVariant& tokenizer,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context =  _TXT("tokenizer");
	if (tokenizer.valuetype != papuga_TypeSerialization)
	{
		std::string name = ValueVariantWrap::tostring( tokenizer);
		return getTokenizer_( name, std::vector<std::string>(), textproc, errorhnd);
	}
	else
	{
		Reference<TokenizerFunctionInstanceInterface> rt;
		papuga_SerializationIter seriter, serstart;
		papuga_init_SerializationIter( &serstart, tokenizer.value.serialization);
		papuga_init_SerializationIter( &seriter, tokenizer.value.serialization);
		try
		{
			rt = getTokenizer( seriter, textproc, errorhnd);
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
		}
		return rt;
	}
}

Reference<AggregatorFunctionInstanceInterface> Deserializer::getAggregator(
		const papuga_ValueVariant& aggregator,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context =  _TXT("aggregator");
	if (aggregator.valuetype != papuga_TypeSerialization)
	{
		std::string name = ValueVariantWrap::tostring( aggregator);
		return getAggregator_( name, std::vector<std::string>(), textproc, errorhnd);
	}
	else
	{
		Reference<AggregatorFunctionInstanceInterface> rt;
		papuga_SerializationIter seriter, serstart;
		papuga_init_SerializationIter( &serstart, aggregator.value.serialization);
		papuga_init_SerializationIter( &seriter, aggregator.value.serialization);
		try
		{
			rt = getAggregator( seriter, textproc, errorhnd);
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
		}
		return rt;
	}
}

template <class FUNCTYPE>
static void instantiateQueryEvalFunctionParameter(
		const char* functionclass,
		FUNCTYPE* function,
		const std::string& name,
		const papuga_ValueVariant* value)
{
	if (papuga_ValueVariant_isnumeric( value))
	{
		function->addNumericParameter( name, ValueVariantWrap::tonumeric( *value));
	}
	else if (papuga_ValueVariant_isstring( value))
	{
		function->addStringParameter( name, ValueVariantWrap::tostring( *value));
	}
	else
	{
		throw strus::runtime_error(_TXT("atomic value expected as %s argument (%s)"), functionclass, name.c_str());
	}
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionParameter(
		const char* functionclass,
		FUNCTYPE* function,
		std::string& debuginfoAttribute,
		papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "name,value", ',');
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);

		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			// ... 2 tuple (pair) of values interpreted as key/value pair
			std::string paramname = Deserializer::getString( seriter);
			if (paramname == "debug")
			{
				debuginfoAttribute = Deserializer::getString( seriter);
			}
			else
			{
				const papuga_ValueVariant* value = getValue( seriter);
				instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
			}
			Deserializer::consumeClose( seriter);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			std::string name;
			unsigned char name_defined = 0;
			const papuga_ValueVariant* value = 0;
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
				papuga_SerializationIter_skip( &seriter);
				switch (idx)
				{
					case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "name", functionclass);
						name = Deserializer::getString( seriter);
						break;
					case 1: if (value) throw strus::runtime_error(_TXT("contradicting definitions in %s parameter: only one allowed of 'value' or 'feature'"), functionclass);
						value = getValue( seriter);
						break;
					default: throw strus::runtime_error(_TXT("unknown name in %s parameter list"), functionclass);
				}
			}
			Deserializer::consumeClose( seriter);
			if (!name_defined || !value)
			{
				throw strus::runtime_error( _TXT( "incomplete definition of %s"), functionclass);
			}
			if (name == "debug")
			{
				debuginfoAttribute = ValueVariantWrap::tostring( *value);
			}
			else
			{
				instantiateQueryEvalFunctionParameter( functionclass, function, name, value);
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("incomplete %s definition"), functionclass);
		}
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		std::string paramname = ValueVariantWrap::tostring( *papuga_SerializationIter_value( &seriter));
		papuga_SerializationIter_skip( &seriter);
		if (paramname == "debug")
		{
			debuginfoAttribute = Deserializer::getString( seriter);
		}
		else
		{
			const papuga_ValueVariant* value = getValue( seriter);
			instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("format error in %s parameter structure"), functionclass);
	}
}

static std::vector<QueryEvalInterface::FeatureParameter> getFeatureParameters(
		const papuga_ValueVariant& featureParameters)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	std::vector<FeatureParameter> rt;
	static const char* context = "feature parameter";
	static const StructureNameMap namemap( "role,set", ',');

	if (!papuga_ValueVariant_defined( &featureParameters))
	{
		return rt;
	}
	else if (featureParameters.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error( _TXT( "structure expected for %s"), context);
	}
	else
	{
		papuga_SerializationIter seriter;
		papuga_init_SerializationIter( &seriter, featureParameters.value.serialization);
		while (!papuga_SerializationIter_eof( &seriter))
		{
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				papuga_SerializationIter_skip( &seriter);

				if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
				{
					// ... 2 tuple (pair) of values interpreted as key/value pair
					std::string feature = Deserializer::getString( seriter);
					std::vector<std::string> sets = Deserializer::getStringListAsValue( seriter);
					std::vector<std::string>::const_iterator si = sets.begin(), se = sets.end();
					for (; si != se; ++si)
					{
						rt.push_back( FeatureParameter( feature, *si));
					}
					Deserializer::consumeClose( seriter);
				}
				else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
				{
					std::string feature;
					std::vector<std::string> sets;

					unsigned char defined[2] = {false,false};
					while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
					{
						int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
						papuga_SerializationIter_skip( &seriter);
						switch (idx)
						{
							case 0: if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "name", context);
								feature = Deserializer::getString( seriter);
								break;
							case 1: if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "set", context);
								sets = Deserializer::getStringListAsValue( seriter);
								break;
							default: throw strus::runtime_error(_TXT("unknown name in %s definition"), context);
						}
					}
					Deserializer::consumeClose( seriter);
					if (!defined[0] || !defined[1])
					{
						throw strus::runtime_error( _TXT( "incomplete definition of %s"), context);
					}
					std::vector<std::string>::const_iterator si = sets.begin(), se = sets.end();
					for (; si != se; ++si)
					{
						rt.push_back( FeatureParameter( feature, *si));
					}
				}
				else
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				std::string feature = ValueVariantWrap::tostring( *papuga_SerializationIter_value( &seriter));
				papuga_SerializationIter_skip( &seriter);
				std::vector<std::string> sets = Deserializer::getStringListAsValue( seriter);
				std::vector<std::string>::const_iterator si = sets.begin(), se = sets.end();
				for (; si != se; ++si)
				{
					rt.push_back( FeatureParameter( feature, *si));
				}
			}
			else
			{
				throw strus::runtime_error(_TXT("format error in %s list"), context);
			}
		}
	}
	return rt;
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionParameters(
		const char* functionclass,
		FUNCTYPE* function,
		std::string& debuginfoAttribute,
		const papuga_ValueVariant& parameters,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("query evaluation function parameter list");
	if (parameters.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("list of named arguments expected as %s parameters"), functionclass);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &seriter, parameters.value.serialization);
	papuga_init_SerializationIter( &serstart, parameters.value.serialization);
	try
	{
		while (papuga_SerializationIter_tag(&seriter) != papuga_TagClose)
		{
			deserializeQueryEvalFunctionParameter( functionclass, function, debuginfoAttribute, seriter);
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
	if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
}

void Deserializer::buildSummarizerFunction(
		QueryEvalInterface* queryeval,
		const std::string& summarizerId,
		const std::string& functionName,
		const papuga_ValueVariant& parameters,
		const papuga_ValueVariant& features,
		const QueryProcessorInterface* queryproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("summarizer function");
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;

	const SummarizerFunctionInterface* sf = queryproc->getSummarizerFunction( functionName);
	if (!sf) throw strus::runtime_error( _TXT("%s not defined: '%s'"), context, functionName.c_str());

	Reference<SummarizerFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating %s '%s': %s"), context, functionName.c_str(), errorhnd->fetchError());

	std::string debuginfoAttribute("debug");

	deserializeQueryEvalFunctionParameters( context, function.get(), debuginfoAttribute, parameters, errorhnd);

	std::vector<FeatureParameter> featureParameters = getFeatureParameters( features);

	queryeval->addSummarizerFunction( summarizerId, function.get(), featureParameters, debuginfoAttribute);
	function.release();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to define %s '%s': %s"), context, functionName.c_str(), errorhnd->fetchError());
	}
}

void Deserializer::buildWeightingFunction(
		QueryEvalInterface* queryeval,
		const std::string& functionName,
		const papuga_ValueVariant& parameters,
		const papuga_ValueVariant& features,
		const QueryProcessorInterface* queryproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("weighting function");
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;

	const WeightingFunctionInterface* sf = queryproc->getWeightingFunction( functionName);
	if (!sf) throw strus::runtime_error( _TXT("%s not defined: '%s'"), context, functionName.c_str());

	Reference<WeightingFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating %s '%s': %s"), context, functionName.c_str(), errorhnd->fetchError());

	std::string debuginfoAttribute("debug");
	deserializeQueryEvalFunctionParameters( context, function.get(), debuginfoAttribute, parameters, errorhnd);

	std::vector<FeatureParameter> featureParameters = getFeatureParameters( features);

	queryeval->addWeightingFunction( function.get(), featureParameters, debuginfoAttribute);
	function.release();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to define %s function '%s': %s"), context, functionName.c_str(), errorhnd->fetchError());
	}
}

void Deserializer::buildWeightingFormula(
		QueryEvalInterface* queryeval,
		const std::string& source,
		const papuga_ValueVariant& parameter,
		const QueryProcessorInterface* queryproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("scalar function");
	static const char* namemapdef = "name,value";
	std::string parsername;
	typedef std::pair<std::string,double> ParamDef;
	std::vector<ParamDef> paramlist;
	if (papuga_ValueVariant_defined( &parameter))
	{
		KeyValueList kvlist( parameter, namemapdef);
		KeyValueList::const_iterator ki = kvlist.begin(), ke = kvlist.end();
		for (; ki != ke; ++ki)
		{
			if (ki->first == "parser")
			{
				parsername = ValueVariantWrap::tostring( *ki->second);
			}
			else
			{
				paramlist.push_back( ParamDef(
					ki->first, ValueVariantWrap::todouble( *ki->second)));
			}
		}
	}
	const ScalarFunctionParserInterface* scalarfuncparser = queryproc->getScalarFunctionParser( parsername);
	strus::local_ptr<ScalarFunctionInterface> scalarfunc( scalarfuncparser->createFunction( source, std::vector<std::string>()));
	if (!scalarfunc.get())
	{
		throw strus::runtime_error(_TXT( "failed to create %s (%s) from source: %s"), context, source.c_str(), errorhnd->fetchError());
	}
	std::vector<ParamDef>::const_iterator vi = paramlist.begin(), ve = paramlist.end();
	for (; vi != ve; ++vi)
	{
		scalarfunc->setDefaultVariableValue( vi->first, vi->second);
	}
	queryeval->defineWeightingFormula( scalarfunc.get());
	scalarfunc.release();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to define %s (%s): %s"), context, source.c_str(), errorhnd->fetchError());
	}
}


bool Deserializer::skipStructure( papuga_SerializationIter& seriter)
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		papuga_SerializationIter_skip( &seriter);
		return true;
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		int brkcnt = 1;
		papuga_SerializationIter_skip( &seriter);
		for (;brkcnt;papuga_SerializationIter_skip( &seriter))
		{
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				++brkcnt;
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagClose)
			{
				--brkcnt;
				if (papuga_SerializationIter_eof( &seriter))
				{
					return false;
				}
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

enum ExpressionType {
	ExpressionUnknown,
	ExpressionEmpty,
	ExpressionTerm,
	ExpressionVariableAssignment,
	ExpressionJoin,
	ExpressionList
};

static ExpressionType getExpressionType( const papuga_SerializationIter& seriter_)
{
	static const StructureNameMap keywords( "type,value,len,variable,op,range,cardinality,arg", ',');
	static const ExpressionType keywordTypeMap[10] = {
		ExpressionTerm, ExpressionTerm, ExpressionTerm,			/*type,value,len*/
		ExpressionVariableAssignment,					/*variable*/
		ExpressionJoin,ExpressionJoin,ExpressionJoin,ExpressionJoin	/*op,range,cardinality,arg*/
	};
	papuga_SerializationIter seriter;
	papuga_init_SerializationIter_copy( &seriter, &seriter_);

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		do
		{
			int ki = keywords.index( *papuga_SerializationIter_value( &seriter));
			if (ki<0) return ExpressionUnknown;
			ExpressionType rt = keywordTypeMap[ ki];
			if (rt != ExpressionVariableAssignment) return rt;
			papuga_SerializationIter_skip( &seriter);
			if (!Deserializer::skipStructure( seriter)) return ExpressionUnknown;
		} while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
		return ExpressionUnknown;
	}
	else
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				char buf[ 128];
				const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter), 0/*nonAsciiSubstChar*/);

				if (id && 0==std::strcmp( id, "variable"))
				{
					papuga_SerializationIter_skip( &seriter);
					if (!Deserializer::skipStructure( seriter)) return ExpressionUnknown;
					Deserializer::consumeClose( seriter);
				}
				else
				{
					return ExpressionList;
				}
			}
			else
			{
				return ExpressionList;
			}
		}
		int argc = 0;
		for (; argc < 3 && papuga_SerializationIter_tag( &seriter) == papuga_TagValue; papuga_SerializationIter_skip( &seriter),++argc){}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagClose)
		{
			return argc ? ExpressionTerm : ExpressionEmpty;
		}
		else
		{
			return ExpressionJoin;
		}
	}
}

static void builderPushTerm(
		ExpressionBuilder& builder,
		const QueryTermDef& def)
{
	if (def.length_defined)
	{
		builder.pushTerm( def.type, def.value, def.length);
	}
	else if (def.value_defined)
	{
		builder.pushTerm( def.type, def.value);
	}
	else
	{
		builder.pushTerm( def.type);
	}
	if (!def.variable.empty())
	{
		builder.attachVariable( def.variable);
	}
}

static void buildExpressionJoin( ExpressionBuilder& builder, papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("join expression");
	static const StructureNameMap joinop_namemap( "variable,op,range,cardinality,arg", ',');
	enum StructureNameId {JO_variable=0,JO_op=1, JO_range=2, JO_cardinality=3, JO_arg=4};
	papuga_ErrorCode err = papuga_Ok;

	std::string variable;
	std::string op;
	unsigned int argc = 0;
	int range = 0;
	unsigned int cardinality = 0;

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		int defined[ 6] = {0,0,0,0,0,0};
		do
		{
			StructureNameId idx = (StructureNameId)joinop_namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case JO_variable:
					if (defined[JO_variable]++) throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), "variable", context);
					variable = Deserializer::getString( seriter);
					break;
				case JO_op:
					if (defined[JO_op]++) throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), "op", context);
					op = Deserializer::getString( seriter);
					break;
				case JO_range:
					if (defined[JO_range]++) throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), "range", context);
					range = Deserializer::getInt( seriter);
					break;
				case JO_cardinality:
					if (defined[JO_cardinality]++) throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), "cardinality", context);
					cardinality = Deserializer::getUint( seriter);
					break;
				case JO_arg:
					if (defined[JO_arg]++) throw strus::runtime_error(_TXT("duplicate definition of %s in %s"), "arg", context);
					if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
					{
						papuga_SerializationIter_skip( &seriter);
						if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
							{
								papuga_SerializationIter_skip( &seriter);
								Deserializer::buildExpression( builder, seriter, false);
								Deserializer::consumeClose( seriter);
								++argc;
							}
						}
						else
						{
							Deserializer::buildExpression( builder, seriter, false);
							++argc;
						}
						Deserializer::consumeClose( seriter);
					}
					else
					{
						throw strus::runtime_error( _TXT("structure expected for term argument in %s"), context);
					}
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s"), context);
			}
		} while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
	}
	else
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			const papuga_ValueVariant* variable_ = NULL;
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				variable_ = Deserializer::getOptionalDefinition( seriter, "variable");
				if (variable_) variable = ValueVariantWrap::tostring( *variable_);
			}
			Deserializer::consumeClose( seriter);
		}
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			op = Deserializer::getString( seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				papuga_ValueVariant val;
				if (papuga_ValueVariant_tonumeric( papuga_SerializationIter_value( &seriter), &val, &err))
				{
					range = ValueVariantWrap::toint( val);
					papuga_SerializationIter_skip( &seriter);

					if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
					{
						if (papuga_ValueVariant_tonumeric( papuga_SerializationIter_value( &seriter), &val, &err))
						{
							cardinality = ValueVariantWrap::touint( val);
							papuga_SerializationIter_skip( &seriter);
						}
					}
				}
			}
			while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
			{
				if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
				{
					papuga_SerializationIter_skip( &seriter);
					Deserializer::buildExpression( builder, seriter, false);
					Deserializer::consumeClose( seriter);
					++argc;
				}
				else
				{
					throw strus::runtime_error( _TXT("structure expected for term argument in %s"), context);
				}
			}
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			throw strus::runtime_error( _TXT("list instead of single structure passed to %s build expression"), context);
		}
		else
		{
			throw strus::runtime_error(_TXT("unexpected element in %s"), context);
		}
	}
	builder.pushExpression( op, argc, range, cardinality);
	if (!variable.empty())
	{
		builder.attachVariable( variable);
	}
}

static void buildExpressionTyped( ExpressionBuilder& builder, ExpressionType etype, papuga_SerializationIter& seriter, bool allowLists)
{
	const char* context = "expression";
	switch (etype)
	{
		case ExpressionUnknown:
			throw strus::runtime_error(_TXT("unable to interpret %s"), context);
		case ExpressionEmpty:
			throw strus::runtime_error(_TXT("empty structure as %s"), context);
		case ExpressionTerm:
		{
			builderPushTerm( builder, QueryTermDef( seriter));
			break;
		}
		case ExpressionVariableAssignment:
		{
			throw strus::runtime_error(_TXT("isolated variable assignment in %s"), context);
		}
		case ExpressionJoin:
		{
			buildExpressionJoin( builder, seriter);
			break;
		}
		case ExpressionList:
		{
			if (!allowLists) throw strus::runtime_error(_TXT("single expression expected in %s"), context);
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				papuga_SerializationIter_skip( &seriter);
				Deserializer::buildExpression( builder, seriter, false);
				Deserializer::consumeClose( seriter);
			}
		}
	}
}

void Deserializer::buildExpression( ExpressionBuilder& builder, papuga_SerializationIter& seriter, bool allowLists)
{
	const char* context = "expression";
	static const StructureNameMap keywords( "expression,term,list", ',');
	int ki;
	ExpressionType etype = ExpressionUnknown;

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName && 0 <= (ki = keywords.index( *papuga_SerializationIter_value( &seriter))))
	{
		papuga_SerializationIter_skip( &seriter);
		if (papuga_SerializationIter_tag( &seriter) != papuga_TagOpen)
		{
			throw strus::runtime_error(_TXT("unable to interpret %s"), context);
		}
		papuga_SerializationIter_skip( &seriter);
		switch (ki) 
		{
			case 0: etype = ExpressionJoin; break;
			case 1: etype = ExpressionTerm; break;
			case 2: etype = ExpressionList; break;
		}
		buildExpressionTyped( builder, etype, seriter, allowLists);
		Deserializer::consumeClose( seriter);
	}
	else
	{
		etype = getExpressionType( seriter);
		buildExpressionTyped( builder, etype, seriter, allowLists);
	}
}

void Deserializer::buildExpression( ExpressionBuilder& builder, const papuga_ValueVariant& expression, ErrorBufferInterface* errorhnd, bool allowLists)
{
	static const char* context = _TXT("expression");
	if (expression.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	else
	{
		papuga_SerializationIter seriter, serstart;
		papuga_init_SerializationIter( &seriter, expression.value.serialization);
		papuga_init_SerializationIter( &serstart, expression.value.serialization);
		try
		{
			buildExpression( builder, seriter, allowLists);
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
		}
	}
}

void Deserializer::buildPattern( ExpressionBuilder& builder, papuga_SerializationIter& seriter, ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "name,expression,format,visible", ',');
	static const char* context = _TXT("pattern");

	std::string name;
	std::string formatstring;
	bool visible = true;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		do
		{
			int ki = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (ki)
			{
				case 0: name = ValueVariantWrap::tostring( *getValue( seriter));
					break;
				case 1: buildExpression( builder, seriter, false);
					break;
				case 2: formatstring = Deserializer::getString( seriter);
					break;
				case 3: visible = ValueVariantWrap::tobool( *getValue( seriter));
					break;
				default:throw strus::runtime_error(_TXT("unknown element in %s definition"), context);
			}
		} while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
	}
	else
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				const papuga_ValueVariant* visibility = getOptionalDefinition( seriter, "visibility");
				if (visibility)
				{
					char buf[ 128];
					const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), visibility, 0/*nonAsciiSubstChar*/);
	
					if (id && 0==std::strcmp( id, "public"))
					{
						visible = true;
					}
					else if (0==std::strcmp( id, "private"))
					{
						visible = false;
					}
					else
					{
						throw strus::runtime_error(_TXT("expected 'private' or 'public' as access flag of %s"), context);
					}
				}
				else
				{
					const papuga_ValueVariant* format_ = Deserializer::getOptionalDefinition( seriter, "format");
					if (format_) formatstring = ValueVariantWrap::tostring( *format_);
				}
			}
			Deserializer::consumeClose( seriter);
		}
		if (papuga_SerializationIter_tag(&seriter) == papuga_TagValue)
		{
			name = getString( seriter);
			buildExpression( builder, seriter, false);
		}
		else
		{
			throw strus::runtime_error(_TXT("error in %s definition structure"), context);
		}
	}
	builder.definePattern( name, formatstring, visible);
}

void Deserializer::buildPatterns( ExpressionBuilder& builder, const papuga_ValueVariant& patterns, ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("pattern list");

	if (!papuga_ValueVariant_defined( &patterns)) return;
	if (patterns.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &seriter, patterns.value.serialization);
	papuga_init_SerializationIter( &serstart, patterns.value.serialization);
	try
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				papuga_SerializationIter_skip( &seriter);
				buildPattern( builder, seriter, errorhnd);
				Deserializer::consumeClose( seriter);
			}
		}
		else
		{
			buildPattern( builder, seriter, errorhnd);
		}
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}

template <class StorageDocumentAccess>
static void buildMetaData( StorageDocumentAccess* document, papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "name,value", ',');
	static const char* context = _TXT("document metadata");

	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);

			if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				unsigned char name_defined = 0;
				unsigned char value_defined = 0;
				std::string name;
				NumericVariant value;
				do
				{
					int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
					papuga_SerializationIter_skip( &seriter);
					switch (idx)
					{
						case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "name", context);
							name = Deserializer::getString( seriter);
							break;
						case 1: if (value_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "value", context);
							value = ValueVariantWrap::tonumeric( *getValue( seriter));
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
					}
				}
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
				if (!name_defined || !value_defined)
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				document->setMetaData( name, value);
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				std::string name = Deserializer::getString( seriter);
				NumericVariant value = ValueVariantWrap::tonumeric( *getValue( seriter));
				document->setMetaData( name, value);
			}
			else
			{
				throw strus::runtime_error(_TXT("structure expected with named elements or tuple with positional arguments for %s"), context);
			}
			Deserializer::consumeClose( seriter);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			std::string name = ValueVariantWrap::tostring( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			NumericVariant value = ValueVariantWrap::tonumeric( *getValue( seriter));
			document->setMetaData( name, value);
		}
		else
		{
			throw strus::runtime_error(_TXT("named elements or structures expected for %s"), context);
		}
	}
}

template <class StorageDocumentAccess>
static void buildAttributes(
		StorageDocumentAccess* document,
		papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "name,value", ',');
	static const char* context = _TXT("document attributes");

	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);

			if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				unsigned char name_defined = 0;
				unsigned char value_defined = 0;
				std::string name;
				std::string value;
				do
				{
					int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
					papuga_SerializationIter_skip( &seriter);
					switch (idx)
					{
						case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "name", context);
							name = Deserializer::getString( seriter);
							break;
						case 1: if (value_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "value", context);
							value = Deserializer::getString( seriter);
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
					}
				}
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
				if (!name_defined || !value_defined)
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				document->setAttribute( name, value);
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				std::string name = Deserializer::getString( seriter);
				std::string value = Deserializer::getString( seriter);
				document->setAttribute( name, value);
			}
			else
			{
				throw strus::runtime_error(_TXT("structure expected with named elements or tuple with positional arguments for %s"), context);
			}
			Deserializer::consumeClose( seriter);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			std::string name = ValueVariantWrap::tostring( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			std::string value = Deserializer::getString( seriter);
			document->setAttribute( name, value);
		}
		else
		{
			throw strus::runtime_error(_TXT("named elements or structures expected for %s"), context);
		}
	}
}

template <class StorageDocumentAccess>
struct StorageDocumentForwardIndexAccess
{
	StorageDocumentForwardIndexAccess( StorageDocumentAccess* doc_)
		:m_doc(doc_),m_maxpos(0){}
	void addTerm( const std::string& type_, const std::string& value_, const Index& position_)
	{
		if ((std::size_t)position_ > (std::size_t)Constants::storage_max_position_info())
		{
			// Cut positions away that are out of range. Issue a warning later:
			if ((std::size_t)position_ > m_maxpos)
			{
				m_maxpos = position_;
			}
		}
		else
		{
			m_doc->addForwardIndexTerm( type_, value_, position_);
		}
	}

private:
	StorageDocumentAccess* m_doc;
	std::size_t m_maxpos;
};

template <class StorageDocumentAccess>
struct StorageDocumentSearchIndexAccess
{
	StorageDocumentSearchIndexAccess( StorageDocumentAccess* doc_)
		:m_doc(doc_),m_maxpos(0){}
	void addTerm( const std::string& type_, const std::string& value_, const Index& position_)
	{
		if ((std::size_t)position_ > (std::size_t)Constants::storage_max_position_info())
		{
			// Cut positions away that are out of range. Issue a warning later:
			if ((std::size_t)position_ > m_maxpos)
			{
				m_maxpos = position_;
			}
		}
		else
		{
			m_doc->addSearchIndexTerm( type_, value_, position_);
		}
	}
private:
	StorageDocumentAccess* m_doc;
	std::size_t m_maxpos;
};

template <class StorageDocumentIndexAccess>
static void buildStorageIndex( StorageDocumentIndexAccess* document, papuga_SerializationIter& seriter, const char* context)
{
	static const StructureNameMap namemap( "type,value,pos,len", ',');

	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				unsigned char type_defined = 0;
				unsigned char value_defined = 0;
				unsigned char pos_defined = 0;
				std::string type;
				std::string value;
				unsigned int pos = 0;
				do
				{
					int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
					papuga_SerializationIter_skip( &seriter);
					switch (idx)
					{
						case 0: if (type_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "name", context);
							type = Deserializer::getString( seriter);
							break;
						case 1: if (value_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "value", context);
							value = Deserializer::getString( seriter);
							break;
						case 2: if (pos_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "pos", context);
							pos = Deserializer::getUint( seriter);
							break;
						case 3: (void)Deserializer::getUint( seriter);
							// ... len that is part of analyzer output is ignored
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
					}
				}
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
				if (!type_defined || !value_defined || !pos_defined)
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				document->addTerm( type, value, pos);
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				std::string type = Deserializer::getString( seriter);
				std::string value = Deserializer::getString( seriter);
				unsigned int pos = Deserializer::getUint( seriter);
				if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
				{
					papuga_SerializationIter_skip( &seriter);
					// ... len that is part of analyzer output is ignored
				}
				document->addTerm( type, value, pos);
			}
			else
			{
				throw strus::runtime_error(_TXT("structure expected with named elements or tuple with positional arguments for %s"), context);
			}
			Deserializer::consumeClose( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("named elements or structures expected for %s"), context);
		}
	}
}

template <class StorageDocumentIndexAccess>
static void buildStorageStructures( StorageDocumentIndexAccess* document, papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "name,source,sink", ',');
	enum StructureId {_name,_source,_sink};
	static const char* context = _TXT("search structures");

	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				unsigned char defined[3] = {0,0,0};
				std::string name;
				IndexRange source;
				IndexRange sink;
				do
				{
					int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
					papuga_SerializationIter_skip( &seriter);
					switch ((StructureId)idx)
					{
						case _name: if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), namemap.name(idx), context);
							name = Deserializer::getString( seriter);
							break;
						case _source: if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), namemap.name(idx), context);
							source = Deserializer::getIndexRangeAsValue( seriter);
							break;
						case _sink: if (defined[idx]++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), namemap.name(idx), context);
							sink = Deserializer::getIndexRangeAsValue( seriter);
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
					}
				}
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
				if (!defined[0] || !defined[1] || !defined[2])
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				if (source.end() <= (strus::Index)Constants::storage_max_position_info()
					&& sink.end() <= (strus::Index)Constants::storage_max_position_info())
				{
					document->addSearchIndexStructure( name, source, sink);
				}
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				std::string name = Deserializer::getString( seriter);
				IndexRange source = Deserializer::getIndexRangeAsValue( seriter);
				IndexRange sink = Deserializer::getIndexRangeAsValue( seriter);
				document->addSearchIndexStructure( name, source, sink);
			}
			else
			{
				throw strus::runtime_error(_TXT("array or dictionary expected for %s"), context);
			}
			Deserializer::consumeClose( seriter);
		}
		else
		{
			throw strus::runtime_error(_TXT("list of structures expected for %s"), context);
		}
	}
}

template <class StorageDocumentAccess>
static void buildAccessRights( StorageDocumentAccess* document, papuga_SerializationIter& seriter)
{
	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		document->setUserAccessRight( Deserializer::getString( seriter));
	}
}

template <class StorageDocumentAccess>
static void buildAttributesValue(
		StorageDocumentAccess* document,
		papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("document attributes");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		buildAttributes( document, seriter);
		Deserializer::consumeClose( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "attribute", context);
	}
}

template <class StorageDocumentAccess>
static void buildMetaDataValue(
		StorageDocumentAccess* document,
		papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("document metadata");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		buildMetaData( document, seriter);
		Deserializer::consumeClose( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "metadata", context);
	}
}

template <class StorageDocumentAccess>
static void buildStorageSearchIndexValue(
		StorageDocumentAccess* document,
		papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("document search index");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		StorageDocumentSearchIndexAccess<StorageDocumentAccess> da( document);
		papuga_SerializationIter_skip( &seriter);
		buildStorageIndex( &da, seriter, context);
		Deserializer::consumeClose( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "searchindex", context);
	}
}

template <class StorageDocumentAccess>
static void buildStorageSearchStructureValue(
		StorageDocumentAccess* document,
		papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("document search structures");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		buildStorageStructures( document, seriter);
		Deserializer::consumeClose( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "searchindex", context);
	}
}

template <class StorageDocumentAccess>
static void buildStorageForwardIndexValue(
		StorageDocumentAccess* document,
		papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("document forward index");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		StorageDocumentForwardIndexAccess<StorageDocumentAccess> da( document);
		papuga_SerializationIter_skip( &seriter);
		buildStorageIndex( &da, seriter, context);
		Deserializer::consumeClose( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "forwardindex", context);
	}
}

template <class StorageDocumentAccess>
static void buildAccessRightsValue(
		StorageDocumentAccess* document,
		papuga_SerializationIter& seriter)
{
	static const char* context = _TXT("document access");
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		buildAccessRights( document, seriter);
		Deserializer::consumeClose( seriter);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		if (papuga_SerializationIter_value( &seriter)->valuetype == papuga_TypeVoid)
		{
			papuga_SerializationIter_skip( &seriter);
		}
		else
		{
			document->setUserAccessRight( Deserializer::getString( seriter));
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "access", context);
	}
}

template <class StorageDocumentAccess>
static void buildStorageDocument(
		StorageDocumentAccess* document,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "docid,doctype,attribute,metadata,forwardindex,searchindex,searchstruct,access", ',');
	enum StructureId {_docid,_doctype,_attribute,_metadata,_forwardindex,_searchindex,_searchstruct,_access};
	static const char* context = _TXT("document");
	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &serstart, content.value.serialization);
	papuga_init_SerializationIter( &seriter, content.value.serialization);
	try
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
				papuga_SerializationIter_skip( &seriter);
				switch ((StructureId)idx)
				{
					case _docid: (void)Deserializer::getString( seriter);
						// ... ignore sub document type (output of analyzer)
						break;
					case _doctype: (void)Deserializer::getString( seriter);
						// ... ignore document identifier (part of schema)
						break;
					case _attribute: buildAttributesValue( document, seriter);
						break;
					case _metadata: buildMetaDataValue( document, seriter);
						break;
					case _forwardindex: buildStorageForwardIndexValue( document, seriter);
						break;
					case _searchindex: buildStorageSearchIndexValue( document, seriter);
						break;
					case _searchstruct: buildStorageSearchStructureValue( document, seriter);
						break;
					case _access: buildAccessRightsValue( document, seriter);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				}
			}
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			(void)Deserializer::getString( seriter);		if (papuga_SerializationIter_eof( &seriter)) return;
			buildAttributesValue( document, seriter);		if (papuga_SerializationIter_eof( &seriter)) return;
			buildMetaDataValue( document, seriter);			if (papuga_SerializationIter_eof( &seriter)) return;
			buildStorageForwardIndexValue( document, seriter);	if (papuga_SerializationIter_eof( &seriter)) return;
			buildStorageSearchIndexValue( document, seriter);	if (papuga_SerializationIter_eof( &seriter)) return;
			buildStorageSearchStructureValue( document, seriter);	if (papuga_SerializationIter_eof( &seriter)) return;
			buildAccessRightsValue( document, seriter);
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
		}
		else
		{
			throw strus::runtime_error(_TXT("%s definition expected to be a dictionary"), context);
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}

static void buildStorageDocumentDeletes(
		StorageDocumentUpdateInterface* document,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "attribute,metadata,forwardindex,searchindex,access", ',');
	enum StructureId {_attribute,_metadata,_forwardindex,_searchindex,_access};
	static const char* context = _TXT("document update deletes");
	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &serstart, content.value.serialization);
	papuga_init_SerializationIter( &seriter, content.value.serialization);
	try
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
				papuga_SerializationIter_skip( &seriter);
				switch ((StructureId)idx)
				{
					case _attribute: {
							std::vector<std::string> deletes = Deserializer::getStringListAsValue( seriter);
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							for (; di != de; ++di)
							{
								document->clearAttribute( *di);
							}
						}
						break;

					case _metadata: {
							std::vector<std::string> deletes = Deserializer::getStringListAsValue( seriter);
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							for (; di != de; ++di)
							{
								document->setMetaData( *di, NumericVariant());
							}
						}
						break;

					case _forwardindex: {
							std::vector<std::string> deletes = Deserializer::getStringListAsValue( seriter);
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							for (; di != de; ++di)
							{
								document->clearForwardIndexTerm( *di);
							}
						}
						break;

					case _searchindex: {
							std::vector<std::string> deletes = Deserializer::getStringListAsValue( seriter);
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							for (; di != de; ++di)
							{
								document->clearSearchIndexTerm( *di);
							}
						}
						break;

					case _access: {
							std::vector<std::string> deletes = Deserializer::getStringListAsValue( seriter);
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							if (std::find( di, de, std::string("*")) != deletes.end())
							{
								document->clearUserAccessRights();
							}
							else for (; di != de; ++di)
							{
								document->clearForwardIndexTerm( *di);
							}
						}
						break;

					default: {
							throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
					}
				}
			}
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens in serialization of %s"), context);
		}
		else
		{
			throw strus::runtime_error(_TXT("%s definition expected to be a dictionary"), context);
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}

void Deserializer::buildInsertDocument(
		StorageDocumentInterface* document,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	buildStorageDocument( document, content, errorhnd);
}

void Deserializer::buildUpdateDocument(
		StorageDocumentUpdateInterface* document,
		const papuga_ValueVariant& content,
		const papuga_ValueVariant& deletes,
		ErrorBufferInterface* errorhnd)
{
	buildStorageDocumentDeletes( document, deletes, errorhnd);
	buildStorageDocument( document, content, errorhnd);
}

void Deserializer::buildStatistics(
		StatisticsBuilderInterface* builder,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "dfchange,nofdocs", ',');
	static const char* context = _TXT("statistics");

	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &serstart, content.value.serialization);
	papuga_init_SerializationIter( &seriter, content.value.serialization);
	try
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
				papuga_SerializationIter_skip( &seriter);
				switch (idx)
				{
					case 0:
					{
						if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
							{
								DfChangeDef dfchg( seriter);
								builder->addDfChange( dfchg.termtype.c_str(), dfchg.termvalue.c_str(), dfchg.increment);
							}
							Deserializer::consumeClose( seriter);
						}
						break;
					}
					case 1:
					{
						builder->addNofDocumentsInsertedChange( Deserializer::getInt( seriter));
						break;
					}
					default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				}
			}
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
		}
		else
		{
			throw strus::runtime_error(_TXT("%s definition expected to be a dictionary"), context);
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}

template <class INTERFACE>
struct WrapMetaDataRestriction
{
	WrapMetaDataRestriction( INTERFACE* self_)
		:m_self(self_){}

	void addCondition(
			const MetaDataRestrictionInterface::CompareOperator& opr,
			const std::string& name,
			const papuga_ValueVariant& operand,
			bool newGroup)
	{
		m_self->addCondition( opr, name, ValueVariantWrap::tonumeric( operand), newGroup);
	}
	void done(){}

private:
	INTERFACE* m_self;
};

template <>
struct WrapMetaDataRestriction<QueryInterface>
{
	WrapMetaDataRestriction( QueryInterface* self_)
		:m_self(self_){}

	void addCondition(
			const MetaDataRestrictionInterface::CompareOperator& opr,
			const std::string& name,
			const papuga_ValueVariant& operand,
			bool newGroup)
	{
		m_self->addMetaDataRestrictionCondition( opr, name, ValueVariantWrap::tonumeric( operand), newGroup);
	}
	void done(){}
	
private:
	QueryInterface* m_self;
};

template <>
struct WrapMetaDataRestriction<MetaDataExpression>
{
	WrapMetaDataRestriction( MetaDataExpression* self_)
		:m_self(self_),m_termc(0){}

	void addCondition(
			const MetaDataRestrictionInterface::CompareOperator& opr,
			const std::string& name,
			const papuga_ValueVariant& operand,
			bool newGroup)
	{
		if (newGroup && m_termc > 1)
		{
			m_self->pushOperator( MetaDataExpression::OperatorOR, m_termc);
			m_termc = 0;
		}
		m_self->pushCompare( opr, name, ValueVariantWrap::tostring( operand));
		if (newGroup)
		{
			m_termc = 1;
		}
		else
		{
			++m_termc;
		}
	}
	void done()
	{
		if (m_termc > 1)
		{
			m_self->pushOperator( MetaDataExpression::OperatorOR, m_termc);
			m_termc = 0;
		}
	}

private:
	MetaDataExpression* m_self;
	unsigned int m_termc;
};

template <class Builder>
static void buildMetaDataRestrictionConditionJoin( Builder& builder, papuga_SerializationIter& seriter, bool doGroup)
{
	bool newGroup = true;
	while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		MetaDataCompareDef def( seriter);
		builder.addCondition( def.cmpop, def.name, def.value, newGroup);
		if (doGroup) newGroup = false;
		Deserializer::consumeClose( seriter);
	}
}

template <class Builder>
static void buildMetaDataRestrictionConditionJoinStruct( Builder& builder, papuga_SerializationIter& seriter, bool doGroup)
{
	static const char* context = "metadata condition expression";
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			buildMetaDataRestrictionConditionJoin( builder, seriter, doGroup);
		}
		else
		{
			MetaDataCompareDef def( seriter);
			builder.addCondition( def.cmpop, def.name, def.value, true/*new group*/);
		}
		Deserializer::consumeClose( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected after tag in %s"), context);
	}
}

template <class Builder>
static void buildMetaDataRestrictionNamed( Builder& builder, papuga_SerializationIter& seriter)
{
	static const char* context = "metadata condition expression";
	static const StructureNameMap namemap( "union,condition,op,name,value", ',');

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
		if (idx < 0) throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
		if (idx >= 2)
		{
			MetaDataCompareDef def( seriter);
			builder.addCondition( def.cmpop, def.name, def.value, true/*new group*/);
		}
		else
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				if (idx == 0/*union*/)
				{
					if (papuga_SerializationIter_follow_tag( &seriter) == papuga_TagName)
					{
						papuga_SerializationIter_skip( &seriter);
						if (Deserializer::compareName( seriter, "condition", 9/*strlen condition*/))
						{
							papuga_SerializationIter_skip( &seriter);
							buildMetaDataRestrictionConditionJoinStruct( builder, seriter, true/*do group*/);
						}
						else
						{
							throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
						}
					}
					else
					{
						buildMetaDataRestrictionConditionJoinStruct( builder, seriter, false/*do not group*/);
					}
					
				}
				else /*condition*/
				{
					buildMetaDataRestrictionConditionJoinStruct( builder, seriter, false);
				}
			}
			else
			{
				throw strus::runtime_error(_TXT("structure expected after tag in %s"), context);
			}
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("name expected opening named structure in %s"), context);
	}
}

template <class RestrictionInterface>
static void buildMetaDataRestriction_( RestrictionInterface* builderobj, papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "union,condition", ',');
	typedef WrapMetaDataRestriction<RestrictionInterface> Builder;
	Builder builder( builderobj);

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		buildMetaDataRestrictionNamed( builder, seriter);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		if (papuga_SerializationIter_follow_tag( &seriter) == papuga_TagName)
		{
			papuga_SerializationIter_skip( &seriter);
			buildMetaDataRestrictionNamed( builder, seriter);
		}
		else while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				buildMetaDataRestrictionNamed( builder, seriter);
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				buildMetaDataRestrictionConditionJoin( builder, seriter, true/*do group*/);
			}
			else
			{
				MetaDataCompareDef def( seriter);
				builder.addCondition( def.cmpop, def.name, def.value, true/*new group*/);
			}
			Deserializer::consumeClose( seriter);
		}
	}
	else if (!papuga_SerializationIter_eof( &seriter))
	{
		MetaDataCompareDef def( seriter);
		builder.addCondition( def.cmpop, def.name, def.value, true/*new group*/);
	}
	builder.done();
}

void Deserializer::buildMetaDataRestriction(
		MetaDataRestrictionInterface* builder,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("metadata restriction");

	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}

	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &serstart, content.value.serialization);
	papuga_init_SerializationIter( &seriter, content.value.serialization);
	try
	{
		buildMetaDataRestriction_<MetaDataRestrictionInterface>( builder, seriter);
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( _TXT("error defining metadata restriction: %s"), errorhnd->fetchError());
		}
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}

void Deserializer::buildMetaDataRestriction(
		QueryInterface* builder,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("metadata restriction");

	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &serstart, content.value.serialization);
	papuga_init_SerializationIter( &seriter, content.value.serialization);
	try
	{
		buildMetaDataRestriction_<QueryInterface>( builder, seriter);
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT("error defining %s: %s"), context, errorhnd->fetchError());
		}
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}

void Deserializer::buildMetaDataRestriction(
		MetaDataExpression* builder,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("metadata restriction");

	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &serstart, content.value.serialization);
	papuga_init_SerializationIter( &seriter, content.value.serialization);
	try
	{
		buildMetaDataRestriction_<MetaDataExpression>( builder, seriter);
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT("error defining %s: %s"), context, errorhnd->fetchError());
		}
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}

std::string Deserializer::getConfigString( papuga_SerializationIter& seriter)
{
	ConfigDef cfg( seriter);
	return cfg.cfgstring;
}

std::string Deserializer::getConfigString( const papuga_ValueVariant& content)
{
	ConfigDef cfg( content);
	return cfg.cfgstring;
}

int Deserializer::buildSentencePatternExpressionArguments(
		SentenceAnalyzerInstanceInterface* analyzer,
		papuga_SerializationIter& seriter,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("sentence expression argument list");
	int argc = 0;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
		{
			buildSentencePatternExpression( analyzer, seriter, errorhnd);
			++argc;
		}
		consumeClose( seriter);
		return argc;
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for %s"), context);
	}
}

void Deserializer::buildSentencePatternExpression(
		SentenceAnalyzerInstanceInterface* analyzer,
		papuga_SerializationIter& seriter,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "type,value,weight,op,min,max,arg", ',');
	static const StructureNameMap op_namemap( "seq,alt,repeat", ',');
	static const char* context = _TXT("sentence expression");

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		int repeat_min = 0;
		int repeat_max = -1;
		int argc = -1;
		enum OpType {OpSeq=0,OpAlt,OpRepeat};
		OpType opType;
		std::string feat_type;
		std::string feat_value;
		double weight = 1.0;
		enum EType {ETypeNone,ETypeTerm,ETypeSeq,ETypeAlt,ETypeRepeat};
		EType eType = ETypeNone;

		papuga_SerializationIter_skip( &seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagClose)
		{
			eType = ETypeNone;
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			do
			{
				int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
				papuga_SerializationIter_skip( &seriter);
				switch (idx)
				{
					case 0/*type*/:
						if (eType != ETypeNone && eType != ETypeTerm && argc >= 0) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
						eType = ETypeTerm;
						feat_type = Deserializer::getString( seriter);
					break;
					case 1/*value*/:
						if (eType != ETypeNone && eType != ETypeTerm && argc >= 0) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
						eType = ETypeTerm;
						feat_value = Deserializer::getString( seriter);
					break;
					case 2/*weight*/:
						if (eType != ETypeNone && eType != ETypeTerm && argc >= 0) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
						eType = ETypeTerm;
						weight = Deserializer::getFloat( seriter);
					break;
					case 3/*op*/:
						if (papuga_SerializationIter_tag( &seriter) != papuga_TagValue) throw strus::runtime_error(_TXT("value expected as argument of '%s' in '%s'"), namemap.name(idx), context);
						opType = (OpType)op_namemap.index( *papuga_SerializationIter_value( &seriter));
						papuga_SerializationIter_skip( &seriter);
						switch (opType)
						{
							case OpSeq:
								if (eType != ETypeNone && eType != ETypeSeq) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
								eType = ETypeSeq;
							break;
							case OpAlt:
								if (eType != ETypeNone && eType != ETypeAlt) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
								eType = ETypeAlt;
							break;
							case OpRepeat:
								if (eType != ETypeNone && eType != ETypeRepeat) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
								eType = ETypeRepeat;
							break;
							default: throw strus::runtime_error(_TXT("unknown operation 'op' in %s structure"), context);
						}
						break;
					break;
					case 4/*min*/:
						if (eType != ETypeNone && eType != ETypeRepeat) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
						eType = ETypeRepeat;
						repeat_min = getUint( seriter);
					break;
					case 5/*max*/:
						if (eType != ETypeNone && eType != ETypeRepeat) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
						eType = ETypeRepeat;
						repeat_max = getInt( seriter);
					break;
					case 6/*arg*/:
						if (eType == ETypeTerm) throw strus::runtime_error(_TXT("conflicting declarations in '%s'"), context);
						argc = buildSentencePatternExpressionArguments( analyzer, seriter, errorhnd);
					break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				}
			} while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			const papuga_ValueVariant* op = 0;
			const papuga_ValueVariant* arg1 = 0;
			const papuga_ValueVariant* arg2 = 0;

			op = papuga_SerializationIter_value( &seriter);
			papuga_SerializationIter_skip( &seriter);

			if (papuga_SerializationIter_tag( &seriter) == papuga_TagClose)
			{
				if (papuga_ValueVariant_isnumeric( op))
				{
					weight = ValueVariantWrap::todouble( *op);
					eType = ETypeNone;
				}
				else
				{
					feat_type = ValueVariantWrap::tostring( *op);
					eType = ETypeTerm;
				}
			}
			else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
			{
				feat_type = ValueVariantWrap::tostring( *op);

				arg1 = papuga_SerializationIter_value( &seriter);
				papuga_SerializationIter_skip( &seriter);

				if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
				{
					arg2 = papuga_SerializationIter_value( &seriter);
					papuga_SerializationIter_skip( &seriter);
				}
				if (papuga_SerializationIter_tag( &seriter) == papuga_TagClose)
				{
					if (papuga_ValueVariant_isstring( arg1))
					{
						feat_value = ValueVariantWrap::tostring( *arg1);
						eType = ETypeTerm;
						arg1 = 0;
					}
					else if (!arg2)
					{
						weight = ValueVariantWrap::todouble( *arg1);
						eType = ETypeTerm;
						arg1 = 0;
					}
					else
					{
						feat_value = ValueVariantWrap::tostring( *arg1);
						weight = ValueVariantWrap::todouble( *arg2);
						eType = ETypeTerm;
						arg1 = 0;
						arg2 = 0;
					}
				}
			}
			if (eType == ETypeNone)
			{
				opType = (OpType)op_namemap.index( feat_type);
				switch (opType)
				{
					case OpSeq:
						if (arg1) throw strus::runtime_error(_TXT("too many arguments for operation '%s' in %s structure"), feat_type.c_str(), context);
						eType = ETypeSeq;
						break;
					case OpAlt:
						if (arg1) throw strus::runtime_error(_TXT("too many arguments for operation '%s' in %s structure"), feat_type.c_str(), context);
						eType = ETypeAlt;
						break;
					case OpRepeat:
						eType = ETypeRepeat;
						if (arg1)
						{
							repeat_min = ValueVariantWrap::toint( *arg1);
							if (arg2)
							{
								repeat_max = ValueVariantWrap::toint( *arg2);
							}
							arg1 = 0;
						}
						break;
					default: throw strus::runtime_error(_TXT("unknown operation '%s' in %s structure"), feat_type.c_str(), context);
				}
				feat_type.clear();
				argc = buildSentencePatternExpressionArguments( analyzer, seriter, errorhnd);
			}
			argc = buildSentencePatternExpressionArguments( analyzer, seriter, errorhnd);
		}
		else
		{
			throw strus::runtime_error(_TXT("dictionary or list of values expected for '%s'"), context);
		}
		switch (eType)
		{
			case ETypeNone:
				analyzer->pushNone( weight);
			break;
			case ETypeTerm:
				analyzer->pushTerm( feat_type, feat_value, weight);
			break;
			case ETypeSeq:
				analyzer->pushSequenceImm( argc);
			break;
			case ETypeAlt:
				analyzer->pushAlt( argc);
			break;
			case ETypeRepeat:
				analyzer->pushRepeat( repeat_min, repeat_max);
			break;
		}
		consumeClose( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("structure expected for %s"), context);
	}
}

static void buildSentenceAnalyzerLinkDef( SentenceLexerInstanceInterface* lexer, papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "chr,subst", ',');
	static const char* context = _TXT("sentence analyzer/lexer link definition");

	std::vector<int> link_chrs;
	char link_subst = 0;

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		do
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0/*chr*/:
					if (!link_chrs.empty()) throw strus::runtime_error(_TXT("duplicate defition of '%s' in '%s' structure"), namemap.name(idx), context);
					link_chrs = Deserializer::getCharListUnicode( seriter);
					break;
				case 1/*subst*/:
					if (link_subst) throw strus::runtime_error(_TXT("duplicate defition of '%s' in '%s' structure"), namemap.name(idx), context);
					link_subst = Deserializer::getCharAscii( seriter);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name for '%s' in '%s' structure"), namemap.name(idx), context);
			}
		}
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		link_chrs = Deserializer::getCharListUnicode( seriter);
		link_subst = Deserializer::getCharAscii( seriter);
	}
	else
	{
		throw strus::runtime_error(_TXT("dictionary or list of values expected for %s"), context);
	}
	if (!link_chrs.empty() && link_subst)
	{
		std::vector<int>::const_iterator li = link_chrs.begin(), le = link_chrs.end();
		for (; li != le; ++li)
		{
			lexer->addLink( *li, link_subst);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("incomplete definition of %s"), context);
	}
}

static void buildSentenceAnalyzerSentenceDef(
		SentenceAnalyzerInstanceInterface* analyzer,
		papuga_SerializationIter& seriter,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "name,weight,sentpattern", ',');
	static const char* context = _TXT("sentence analyzer/lexer sentence pattern definition");
	std::string classname;
	double weight = 1.0;
	bool weight_defined = false;
	bool pattern_defined = false;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		do
		{
			int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
			papuga_SerializationIter_skip( &seriter);
			switch (idx)
			{
				case 0/*name*/:
					if (!classname.empty()) throw strus::runtime_error(_TXT("duplicate defition of '%s' in '%s' structure"), namemap.name(idx), context);
					classname = Deserializer::getString( seriter);
					break;
				case 1/*weight*/:
					if (weight_defined) throw strus::runtime_error(_TXT("duplicate defition of '%s' in '%s' structure"), namemap.name(idx), context);
					weight_defined = true;
					weight = Deserializer::getFloat( seriter);
					break;
				case 2/*sentpattern*/:
					if (pattern_defined) throw strus::runtime_error(_TXT("duplicate defition of '%s' in '%s' structure"), namemap.name(idx), context);
					pattern_defined = true;
					Deserializer::buildSentencePatternExpression( analyzer, seriter, errorhnd);
					break;
			}
		}
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		classname = Deserializer::getString( seriter);
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
		{
			weight = Deserializer::getFloat( seriter);
		}
		Deserializer::buildSentencePatternExpression( analyzer, seriter, errorhnd);
	}
	else
	{
		throw strus::runtime_error(_TXT("dictionary of list of values expected for %s"), context);
	}
	if (!pattern_defined) throw strus::runtime_error(_TXT("missing defition of '%s' in '%s' structure"), "sentpattern", context);
	analyzer->defineSentence( classname, weight);
}

void Deserializer::buildSentenceAnalyzer(
		SentenceAnalyzerInstanceInterface* analyzer,
		SentenceLexerInstanceInterface* lexer,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "separator,space,link,groupsim,sentence", ',');
	static const char* context = _TXT("sentence analyzer/lexer");

	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &serstart, content.value.serialization);
	papuga_init_SerializationIter( &seriter, content.value.serialization);
	try
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			do
			{
				int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
				papuga_SerializationIter_skip( &seriter);
				switch (idx)
				{
					case 0/*separator*/:
					{
						std::vector<int> separators = Deserializer::getCharListUnicode( seriter);
						std::vector<int>::const_iterator si = separators.begin(), se = separators.end();
						for (; si != se; ++si)
						{
							lexer->addSeparator( *si);
						}
						break;
					}
					case 1/*space*/:
					{
						std::vector<int> spaces = Deserializer::getCharListUnicode( seriter);
						std::vector<int>::const_iterator si = spaces.begin(), se = spaces.end();
						for (; si != se; ++si)
						{
							lexer->addSpace( *si);
						}
						break;
					}
					case 2/*link*/:
					{
						if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
							{
								while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
								{
									papuga_SerializationIter_skip( &seriter);
									buildSentenceAnalyzerLinkDef( lexer, seriter);
									Deserializer::consumeClose( seriter);
								}
							}
							else
							{
								buildSentenceAnalyzerLinkDef( lexer, seriter);
							}
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for value of '%s' in %s"), namemap.name(idx), context);
						}
						break;
					}
					case 3:/*groupsim*/
					{
						if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
						{
							lexer->defineGroupSimilarityDistance( Deserializer::getDouble( seriter));
						}
						else
						{
							throw strus::runtime_error(_TXT("value expected for '%s' in %s"), namemap.name(idx), context);
						}
						break;
					}
					case 4:/*sentence*/
					{
						if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
							{
								while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
								{
									papuga_SerializationIter_skip( &seriter);
									buildSentenceAnalyzerSentenceDef( analyzer, seriter, errorhnd);
									Deserializer::consumeClose( seriter);
								}
							}
							else
							{
								buildSentenceAnalyzerSentenceDef( analyzer, seriter, errorhnd);
							}
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for value of '%s' in %s"), namemap.name(idx), context);
						}
						break;
					}
					default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				}
			}
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
		}
		else
		{
			throw strus::runtime_error(_TXT("structure expected with named elements for %s"), context);
		}
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT("error defining %s: %s"), context, errorhnd->fetchError());
		}
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}

