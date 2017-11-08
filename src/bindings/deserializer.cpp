/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "deserializer.hpp"
#include "internationalization.hpp"
#include "structDefs.hpp"
#include "papuga/serialization.h"
#include "papuga/valueVariant.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/local_ptr.hpp"
#include "valueVariantWrap.hpp"
#include <string>
#include <cstring>

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
					if (!papuga_ValueVariant_defined( value))
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
		if (errorhnd->hasError())
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
		if (si[1] == '=')
		{
			res = MetaDataRestrictionInterface::CompareGreaterEqual;
			return true;
		}
		else
		{
			res = MetaDataRestrictionInterface::CompareGreater;
			return true;
		}
	}
	else if (si[0] == '!')
	{
		if (si[1] == '=')
		{
			res = MetaDataRestrictionInterface::CompareNotEqual;
			return true;
		}
	}
	else if (si[0] == '=')
	{
		if (si[1] == '=')
		{
			res = MetaDataRestrictionInterface::CompareEqual;
			return true;
		}
		else
		{
			res = MetaDataRestrictionInterface::CompareEqual;
			return true;
		}
	}
	return false;
}

MetaDataRestrictionInterface::CompareOperator Deserializer::getMetaDataCmpOp( papuga_SerializationIter& seriter)
{
	MetaDataRestrictionInterface::CompareOperator rt;
	char buf[ 32];
	const char* cmpopstr = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter));
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

const char* Deserializer::getCharpAscii( char* buf, std::size_t bufsize, papuga_SerializationIter& seriter)
{
	const papuga_ValueVariant* vp = getValue( seriter);
	const char* rt = papuga_ValueVariant_toascii( buf, bufsize, vp);
	if (!rt) throw strus::runtime_error(_TXT("ascii string with maximum size %u expected"), (unsigned int)bufsize);
	return rt;
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

template <typename ATOMICTYPE, ATOMICTYPE CONV( const papuga_ValueVariant& val), ATOMICTYPE FUNC( papuga_SerializationIter& seriter)>
static std::vector<ATOMICTYPE> getAtomicTypeList( const papuga_ValueVariant& val)
{
	std::vector<ATOMICTYPE> rt;
	if (val.valuetype != papuga_TypeSerialization)
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

std::vector<std::string> Deserializer::getStringList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<std::string,ValueVariantWrap::tostring,getString>( val);
}

std::vector<double> Deserializer::getDoubleList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<double,ValueVariantWrap::todouble,Deserializer::getDouble>( val);
}

std::vector<int> Deserializer::getIntList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<int,ValueVariantWrap::toint,Deserializer::getInt>( val);
}

std::vector<Index> Deserializer::getIndexList( const papuga_ValueVariant& val)
{
	return getAtomicTypeList<Index,ValueVariantWrap::toint,Deserializer::getIndex>( val);
}

static void setFeatureOption_position( analyzer::FeatureOptions& res, const papuga_ValueVariant* val)
{
	char buf[ 128];
	const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), val);
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

						if (!papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
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
						const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter));
						papuga_SerializationIter_skip( &seriter);
						SimpleStructureParser::setNamedValue( rt, id, getValue( seriter));
						Deserializer::consumeClose( seriter);

						if (!papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
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
					const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter));
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

const papuga_ValueVariant* Deserializer::getOptionalDefinition( papuga_SerializationIter& seriter, const char* name)
{
	const papuga_ValueVariant* rt = 0;
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
	{
		char buf[ 128];
		const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter));

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
		static const StructureNameMap namemap( "mimetype,encoding,scheme", ',');

		int idx = namemap.index( name);
		switch (idx)
		{
			case 0: ths.setMimeType( ValueVariantWrap::tostring( *value));
				break;
			case 1: ths.setEncoding( ValueVariantWrap::tostring( *value));
				break;
			case 2: ths.setScheme( ValueVariantWrap::tostring( *value));
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
			case 2: ths.setScheme( ValueVariantWrap::tostring( *value));
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
	const NormalizerFunctionInterface* nf = textproc->getNormalizer( name);
	static const char* context = _TXT("normalizer function");
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
					throw strus::runtime_error( _TXT("unexpected eof in serialization of %s"), "normalizers");
				}
				papuga_SerializationIter_skip( &seriter);
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
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), "normalizers");
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
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), "tokenizer");
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
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), "aggretator");
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
static void deserializeQueryEvalFunctionParameterValue(
		const char* functionclass,
		FUNCTYPE* function,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
		const std::string& paramname,
		papuga_SerializationIter& seriter)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	if (papuga_SerializationIter_tag(&seriter) == papuga_TagClose)
	{
		throw strus::runtime_error(_TXT("unexpected end of %s parameter definition"), functionclass);
	}
	else if (papuga_SerializationIter_tag(&seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip(&seriter);

		if (papuga_SerializationIter_tag(&seriter) == papuga_TagName)
		{
			char buf[ 128];
			const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter));

			if (id && 0==std::strcmp( id, "feature"))
			{
				papuga_SerializationIter_skip(&seriter);
				if (papuga_SerializationIter_tag(&seriter) == papuga_TagOpen)
				{
					papuga_SerializationIter_skip(&seriter);
					while (papuga_SerializationIter_tag(&seriter) == papuga_TagValue)
					{
						featureParameters.push_back( FeatureParameter( paramname, Deserializer::getString( seriter)));
					}
					Deserializer::consumeClose( seriter);
				}
				else if (papuga_SerializationIter_tag(&seriter) == papuga_TagValue)
				{
					featureParameters.push_back( FeatureParameter( paramname, Deserializer::getString( seriter)));
				}
			}
			else
			{
				throw strus::runtime_error(_TXT("unexpected tag name in %s parameter definition"), functionclass);
			}
		}
		else
		{
			while (papuga_SerializationIter_tag(&seriter) != papuga_TagClose)
			{
				const papuga_ValueVariant* value = getValue( seriter);
				instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
			}
		}
		Deserializer::consumeClose( seriter);
	}
	else
	{
		const papuga_ValueVariant* value = getValue( seriter);
		instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
	}
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionParameter(
		const char* functionclass,
		FUNCTYPE* function,
		std::string& debuginfoAttribute,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
		papuga_SerializationIter& seriter)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	static const StructureNameMap namemap( "name,value,feature", ',');
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
				deserializeQueryEvalFunctionParameterValue(
					functionclass, function, featureParameters, paramname, seriter);
			}
			Deserializer::consumeClose( seriter);
		}
		else if (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
		{
			std::string name;
			unsigned char name_defined = 0;
			bool is_feature = false;
			const papuga_ValueVariant* value = 0;
			while (papuga_SerializationIter_tag( &seriter) == papuga_TagName)
			{
				int idx = namemap.index( *papuga_SerializationIter_value( &seriter));
				switch (idx)
				{
					case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "name", functionclass);
						name = Deserializer::getString( seriter);
						break;
					case 1: if (value) throw strus::runtime_error(_TXT("contradicting definitions in %s parameter: only one allowed of 'value' or 'feature'"), functionclass);
						value = getValue( seriter);
						break;
					case 2:	if (value) throw strus::runtime_error(_TXT("contradicting definitions in %s parameter: only one allowed of 'value' or 'feature'"), functionclass);
						is_feature=true; value = getValue( seriter);
						break;
					default: throw strus::runtime_error(_TXT("unknown name in %s parameter list"), functionclass);
				}
			}
			Deserializer::consumeClose( seriter);
			if (!name_defined || !value)
			{
				throw strus::runtime_error( _TXT( "incomplete definition of %s"), functionclass);
			}
			if (is_feature)
			{
				featureParameters.push_back( FeatureParameter( name, ValueVariantWrap::tostring( *value)));
			}
			else
			{
				if (name == "debug")
				{
					debuginfoAttribute = ValueVariantWrap::tostring( *value);
				}
				else
				{
					instantiateQueryEvalFunctionParameter( functionclass, function, name, value);
				}
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
			deserializeQueryEvalFunctionParameterValue( functionclass, function, featureParameters, paramname, seriter);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("format error in %s parameter structure"), functionclass);
	}
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionParameters(
		const char* functionclass,
		FUNCTYPE* function,
		std::string& debuginfoAttribute,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
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
			deserializeQueryEvalFunctionParameter(
					functionclass, function, debuginfoAttribute, featureParameters, seriter);
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
	if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s"), context);
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionResultNames(
		const char* functionclass,
		FUNCTYPE* function,
		const papuga_ValueVariant& resultnames,
		ErrorBufferInterface* errorhnd)
{
	if (papuga_ValueVariant_defined( &resultnames))
	{
		if (resultnames.valuetype != papuga_TypeSerialization)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected as %s result name definitions"), functionclass);
		}
		papuga_SerializationIter seriter, serstart;
		papuga_init_SerializationIter( &seriter, resultnames.value.serialization);
		papuga_init_SerializationIter( &serstart, resultnames.value.serialization);
		try
		{
			KeyValueList kvlist( seriter);
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error( _TXT("unexpected tokens at end of serialization of %s result name definitions"), functionclass);

			KeyValueList::const_iterator ki = kvlist.begin(), ke = kvlist.end();
			for (; ki != ke; ++ki)
			{
				function->defineResultName( ValueVariantWrap::tostring( *ki->second), ki->first);
			}
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
		}
	}
}

void Deserializer::buildSummarizerFunction(
		QueryEvalInterface* queryeval,
		const std::string& functionName,
		const papuga_ValueVariant& parameters,
		const papuga_ValueVariant& resultnames,
		const QueryProcessorInterface* queryproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("summarizer function");
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	std::vector<FeatureParameter> featureParameters;

	const SummarizerFunctionInterface* sf = queryproc->getSummarizerFunction( functionName);
	if (!sf) throw strus::runtime_error( _TXT("%s not defined: '%s'"), context, functionName.c_str());

	Reference<SummarizerFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating %s '%s': %s"), context, functionName.c_str(), errorhnd->fetchError());

	std::string debuginfoAttribute("debug");

	deserializeQueryEvalFunctionParameters(
		context, function.get(), debuginfoAttribute, featureParameters, parameters, errorhnd);

	deserializeQueryEvalFunctionResultNames(
		context, function.get(), resultnames, errorhnd);

	queryeval->addSummarizerFunction( functionName, function.get(), featureParameters, debuginfoAttribute);
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
		const QueryProcessorInterface* queryproc,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("weighting function");
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	std::vector<FeatureParameter> featureParameters;

	const WeightingFunctionInterface* sf = queryproc->getWeightingFunction( functionName);
	if (!sf) throw strus::runtime_error( _TXT("%s not defined: '%s'"), context, functionName.c_str());

	Reference<WeightingFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating %s '%s': %s"), context, functionName.c_str(), errorhnd->fetchError());

	std::string debuginfoAttribute("debug");
	deserializeQueryEvalFunctionParameters(
		context, function.get(), debuginfoAttribute, featureParameters, parameters, errorhnd);

	queryeval->addWeightingFunction( functionName, function.get(), featureParameters, debuginfoAttribute);
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
	std::string parsername;
	typedef std::pair<std::string,double> ParamDef;
	std::vector<ParamDef> paramlist;
	if (papuga_ValueVariant_defined( &parameter))
	{
		if (parameter.valuetype != papuga_TypeSerialization)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected as parameters of %s"), context);
		}
		papuga_SerializationIter seriter, serstart;
		papuga_init_SerializationIter( &seriter, parameter.value.serialization);
		papuga_init_SerializationIter( &serstart, parameter.value.serialization);
		try
		{
			KeyValueList kvlist( seriter);
			if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);

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
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
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
	ExpressionTerm,
	ExpressionVariableAssignment,
	ExpressionMetaDataRange,
	ExpressionJoin,
	ExpressionList
};

static ExpressionType getExpressionType( const papuga_SerializationIter& seriter_)
{
	static const StructureNameMap keywords( "type,value,len,from,to,variable,op,range,cardinality,arg", ',');
	static const ExpressionType keywordTypeMap[10] = {
		ExpressionTerm, ExpressionTerm, ExpressionTerm,			/*type,value,len*/
		ExpressionMetaDataRange,ExpressionMetaDataRange,		/*from,to*/
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
				const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), papuga_SerializationIter_value( &seriter));

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
			return ExpressionTerm;
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
		int defined[ 5] = {0,0,0,0,0};
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
			papuga_SerializationIter sernext;
			papuga_init_SerializationIter_copy( &sernext, &seriter);
			papuga_SerializationIter_skip( &sernext);
			const papuga_ValueVariant* variable_ = Deserializer::getOptionalDefinition( sernext, "variable");
			if (variable_)
			{
				variable = ValueVariantWrap::tostring( *variable_);
				Deserializer::consumeClose( sernext);
				papuga_init_SerializationIter_copy( &seriter, &sernext);
			}
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

void Deserializer::buildExpression( ExpressionBuilder& builder, papuga_SerializationIter& seriter, bool allowLists)
{
	const char* context = "expression";

	ExpressionType etype = getExpressionType( seriter);
	switch (etype)
	{
		case ExpressionUnknown:
		{
			throw strus::runtime_error(_TXT("unable to interpret %s"), context);
		}
		case ExpressionTerm:
		{
			builderPushTerm( builder, QueryTermDef( seriter));
			break;
		}
		case ExpressionVariableAssignment:
		{
			throw strus::runtime_error(_TXT("isolated variable assignment in %s"), context);
		}
		case ExpressionMetaDataRange:
		{
			MetaDataRangeDef def( seriter);
			builder.pushDocField( def.from, def.to);
			break;
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
				buildExpression( builder, seriter, false);
				Deserializer::consumeClose( seriter);
			}
		}
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
	static const StructureNameMap namemap( "name,expression,visible", ',');
	static const char* context = _TXT("pattern");

	std::string name;
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
				case 2: visible = ValueVariantWrap::tobool( *getValue( seriter));
					break;
				default:throw strus::runtime_error(_TXT("unknown element in %s definition"), context);
			}
		} while (papuga_SerializationIter_tag( &seriter) == papuga_TagName);
	}
	else
	{
		if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter sernext;
			papuga_init_SerializationIter_copy( &sernext, &seriter);
			papuga_SerializationIter_skip( &sernext);
			const papuga_ValueVariant* visibility = getOptionalDefinition( sernext, "visibility");
			if (visibility)
			{
				char buf[ 128];
				const char* id = papuga_ValueVariant_toascii( buf, sizeof(buf), visibility);

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
				Deserializer::consumeClose( sernext);
				papuga_init_SerializationIter_copy( &seriter, &sernext);
			}
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
	builder.definePattern( name, visible);
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
		:m_doc(doc_){}
	void addTerm( const std::string& type_, const std::string& value_, const Index& position_)
	{
		m_doc->addForwardIndexTerm( type_, value_, position_);
	}
private:
	StorageDocumentAccess* m_doc;
};

template <class StorageDocumentAccess>
struct StorageDocumentSearchIndexAccess
{
	StorageDocumentSearchIndexAccess( StorageDocumentAccess* doc_)
		:m_doc(doc_){}
	void addTerm( const std::string& type_, const std::string& value_, const Index& position_)
	{
		m_doc->addSearchIndexTerm( type_, value_, position_);
	}
private:
	StorageDocumentAccess* m_doc;
};

template <class StorageDocumentIndexAccess>
static void buildStorageIndex( StorageDocumentIndexAccess* document, papuga_SerializationIter& seriter)
{
	static const StructureNameMap namemap( "type,value,pos,len", ',');
	static const char* context = _TXT("search index");

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

template <class StorageDocumentAccess>
static void buildAccessRights( StorageDocumentAccess* document, papuga_SerializationIter& seriter)
{
	while (papuga_SerializationIter_tag( &seriter) != papuga_TagClose)
	{
		document->setUserAccessRight( Deserializer::getString( seriter));
	}
}

template <class StorageDocumentAccess>
static void buildStorageDocument(
		StorageDocumentAccess* document,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "doctype,attribute,metadata,searchindex,forwardindex,access", ',');
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
				switch (idx)
				{
					case 0: (void)Deserializer::getString( seriter);
						// ... ignore sub document type (output of analyzer)
						break;
					case 1: if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							buildAttributes( document, seriter);
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "attribute", context);
						}
						break;
					case 2: if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							buildMetaData( document, seriter);
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "metadata", context);
						}
						break;
					case 3: if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							StorageDocumentSearchIndexAccess<StorageDocumentAccess> da( document);
							papuga_SerializationIter_skip( &seriter);
							buildStorageIndex( &da, seriter);
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "searchindex", context);
						}
						break;
					case 4:if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							StorageDocumentForwardIndexAccess<StorageDocumentAccess> da( document);
							papuga_SerializationIter_skip( &seriter);
							buildStorageIndex( &da, seriter);
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "forwardindex", context);
						}
						break;
					case 5: if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							buildAccessRights( document, seriter);
							Deserializer::consumeClose( seriter);
							break;
						}
						else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
						{
							document->setUserAccessRight( Deserializer::getString( seriter));
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "access", context);
						}
						break;
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

static void buildStorageDocumentDeletes(
		StorageDocumentUpdateInterface* document,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "attribute,metadata,searchindex,forwardindex,access", ',');
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
				switch (idx)
				{
					case 0: if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							std::vector<std::string> deletes( Deserializer::getStringList( seriter));
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							for (; di != de; ++di)
							{
								document->clearAttribute( *di);
							}
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "attribute", context);
						}
						break;

					case 1: if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							std::vector<std::string> deletes( Deserializer::getStringList( seriter));
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							for (; di != de; ++di)
							{
								document->setMetaData( *di, NumericVariant());
							}
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "metadata", context);
						}
						break;

					case 2: if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							std::vector<std::string> deletes( Deserializer::getStringList( seriter));
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							for (; di != de; ++di)
							{
								document->clearSearchIndexTerm( *di);
							}
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "searchindex", context);
						}
						break;

					case 3: if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							std::vector<std::string> deletes( Deserializer::getStringList( seriter));
							std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
							for (; di != de; ++di)
							{
								document->clearForwardIndexTerm( *di);
							}
							Deserializer::consumeClose( seriter);
						}
						else
						{
							throw strus::runtime_error(_TXT("structure expected for section %s in %s definition"), "forwardindex", context);
						}
						break;
					case 4: 
					{
						std::vector<std::string> deletes;
						if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
						{
							papuga_SerializationIter_skip( &seriter);
							deletes = Deserializer::getStringList( seriter);
							Deserializer::consumeClose( seriter);
						}
						else if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
						{
							deletes.push_back( Deserializer::getString( seriter));
						}
						std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
						for (; di != de; ++di)
						{
							if (*di == "*") break;
						}
						if (di != de)
						{
							//... clear all
							document->clearUserAccessRights();
						}
						else for (di = deletes.begin(); di != de; ++di)
						{
							document->clearUserAccessRight( *di);
						}
						break;
					}
					default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
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
						builder->setNofDocumentsInsertedChange( Deserializer::getInt( seriter));
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

template <class RestrictionInterface>
static void buildMetaDataRestriction_( RestrictionInterface* builderobj, papuga_SerializationIter& seriter)
{
	WrapMetaDataRestriction<RestrictionInterface> builder( builderobj);

	if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
		{
			papuga_SerializationIter_skip( &seriter);
			if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
			{
				bool newGroup = true;
				while (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
				{
					papuga_SerializationIter_skip( &seriter);
					MetaDataCompareDef def( seriter);
					builder.addCondition( def.cmpop, def.name, def.value, newGroup);
					newGroup = false;
					Deserializer::consumeClose( seriter);
				}
			}
			else
			{
				MetaDataCompareDef def( seriter);
				builder.addCondition( def.cmpop, def.name, def.value, true);
			}
			Deserializer::consumeClose( seriter);
		}
	}
	else
	{
		MetaDataCompareDef def( seriter);
		builder.addCondition( def.cmpop, def.name, def.value, true);
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
			throw strus::runtime_error(_TXT("error defining metadata restriction: %s"), errorhnd->fetchError());
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
			throw strus::runtime_error(_TXT("error defining metadata restriction: %s"), errorhnd->fetchError());
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

std::string Deserializer::getSubConfigString( papuga_SerializationIter& seriter)
{
	if (papuga_SerializationIter_tag( &seriter) == papuga_TagValue)
	{
		return Deserializer::getString( seriter);
	}
	else if (papuga_SerializationIter_tag( &seriter) == papuga_TagOpen)
	{
		papuga_SerializationIter_skip( &seriter);
		ConfigDef item( seriter);
		Deserializer::consumeClose( seriter);
		return item.cfgstring;
	}
	else
	{
		throw strus::runtime_error(_TXT("expected sub structure or string for config"));
	}
}

std::string Deserializer::getConfigString(
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("storage configuration");
	if (!papuga_ValueVariant_defined( &content)) return std::string();
	if (papuga_ValueVariant_isstring( &content))
	{
		return ValueVariantWrap::tostring( content);
	}
	if (content.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure or string expected for %s"), context);
	}
	papuga_SerializationIter seriter, serstart;
	papuga_init_SerializationIter( &serstart, content.value.serialization);
	papuga_init_SerializationIter( &seriter, content.value.serialization);
	try
	{
		ConfigDef cfg( seriter);
		if (!papuga_SerializationIter_eof( &seriter)) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);
		return cfg.cfgstring;
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, seriter, serstart);
	}
}


