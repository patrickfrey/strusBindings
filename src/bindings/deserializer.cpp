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
#include "papuga/serialization.hpp"
#include "papuga/valueVariant.hpp"
#include "strus/base/string_format.hpp"
#include "valueVariantWrap.hpp"
#include <string>
#include <cstring>

using namespace strus;
using namespace strus::bindings;

static std::runtime_error runtime_error_with_location( const char* msg, ErrorBufferInterface* errorhnd, papuga::Serialization::const_iterator si, const papuga::Serialization::const_iterator& se, const papuga::Serialization::const_iterator& start)
{
	std::string msgbuf;
	try
	{
		std::string location_explain;
		papuga::Serialization::const_iterator errorpos = si;
		for (int cnt=0; si != start && cnt < 15; --si,++cnt){}
		
		for (int cnt=0; si != se && cnt < 20; ++cnt,++si)
		{
			if (si == errorpos)
			{
				location_explain.append( " <!> ");
			}
			switch (si->tag)
			{
				case papuga_TagOpen:
					location_explain.push_back( '[');
					break;
				case papuga_TagClose:
					location_explain.push_back( ']');
					break;
				case papuga_TagName:
					if (!location_explain.empty())
					{
						location_explain.push_back( ' ');
					}
					location_explain.append( ValueVariantWrap::tostring( si->value));
					location_explain.push_back( ':');
					break;
				case papuga_TagValue:
					if (!location_explain.empty())
					{
						location_explain.push_back( ' ');
					}
					if (!papuga_ValueVariant_defined( &si->value))
					{
						location_explain.append( "<null>");
					}
					else if (papuga_ValueVariant_isatomic( &si->value))
					{
						if (papuga_ValueVariant_isnumeric( &si->value))
						{
							location_explain.append( ValueVariantWrap::tostring( si->value));
						}
						else
						{
							location_explain.push_back( '"');
							location_explain.append( ValueVariantWrap::tostring( si->value));
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
		if (si != se)
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
			msgbuf.append( string_format( _TXT("(in expression at %s)"), location_explain.c_str()));
		}
		return strus::runtime_error( msgbuf.c_str());
	}
	catch (const std::exception&)
	{
		return strus::runtime_error( msg);
	}
}

static const papuga_ValueVariant& getValue(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	if (si != se && si->tag == papuga_TagValue)
	{
		const papuga_ValueVariant& rt = si->value;
		++si;
		return rt;
	}
	else
	{
		throw strus::runtime_error(_TXT("expected value in structure"));
	}
}

unsigned int Deserializer::getUint(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	return ValueVariantWrap::touint( getValue( si, se));
}

int Deserializer::getInt(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	return ValueVariantWrap::toint( getValue( si, se));
}

Index Deserializer::getIndex(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	return ValueVariantWrap::toint( getValue( si, se));
}

double Deserializer::getDouble(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	return ValueVariantWrap::todouble( getValue( si, se));
}

std::string Deserializer::getString( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	return ValueVariantWrap::tostring( getValue( si, se));
}

const char* Deserializer::getCharp(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	papuga_ValueVariant val = getValue( si, se);
	if (val.valuetype != papuga_String) throw strus::runtime_error(_TXT("expected UTF-8 or ASCII string"));
	return val.value.string;
}

void Deserializer::consumeClose(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	if (si != se)
	{
		if (si->tag != papuga_TagClose)
		{
			throw strus::runtime_error(_TXT("unexpected element at end of structure, close expected"));
		}
		++si;
	}
}

template <typename ATOMICTYPE, ATOMICTYPE FUNC( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)>
static std::vector<ATOMICTYPE> getAtomicTypeList( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	std::vector<ATOMICTYPE> rt;
	while (si != se && si->tag != papuga_TagClose)
	{
		rt.push_back( FUNC( si, se));
	}
	return rt;
}

template <typename ATOMICTYPE, ATOMICTYPE CONV( const papuga_ValueVariant& val), ATOMICTYPE FUNC( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)>
static std::vector<ATOMICTYPE> getAtomicTypeList( const papuga_ValueVariant& val)
{
	std::vector<ATOMICTYPE> rt;
	if (val.valuetype != papuga_Serialized)
	{
		rt.push_back( CONV( val));
		return rt;
	}
	else
	{
		papuga::Serialization::const_iterator
			si = papuga::Serialization::begin( val.value.serialization),
			se = papuga::Serialization::end( val.value.serialization);
		return getAtomicTypeList<ATOMICTYPE,FUNC>( si, se);
	}
	return rt;
}

std::vector<std::string> Deserializer::getStringList( papuga::Serialization::const_iterator& si, const papuga::Serialization::const_iterator& se)
{
	return getAtomicTypeList<std::string,getString>( si, se);
}

std::vector<std::string> Deserializer::getStringList(
		const papuga_ValueVariant& val)
{
	return getAtomicTypeList<std::string,ValueVariantWrap::tostring,getString>( val);
}

std::vector<double> Deserializer::getDoubleList(
		const papuga_ValueVariant& val)
{
	return getAtomicTypeList<double,ValueVariantWrap::todouble,Deserializer::getDouble>( val);
}

std::vector<int> Deserializer::getIntList(
		const papuga_ValueVariant& val)
{
	return getAtomicTypeList<int,ValueVariantWrap::toint,Deserializer::getInt>( val);
}

std::vector<Index> Deserializer::getIndexList(
		const papuga_ValueVariant& val)
{
	return getAtomicTypeList<Index,ValueVariantWrap::toint,Deserializer::getIndex>( val);
}

static bool setFeatureOption_position( analyzer::FeatureOptions& res, const papuga_ValueVariant& val)
{
	if (papuga_ValueVariant_isequal_ascii( &val, "content"))
	{
		res.definePositionBind( analyzer::BindContent);
		return true;
	}
	else if (papuga_ValueVariant_isequal_ascii( &val, "succ"))
	{
		res.definePositionBind( analyzer::BindSuccessor);
		return true;
	}
	else if (papuga_ValueVariant_isequal_ascii( &val, "pred"))
	{
		res.definePositionBind( analyzer::BindPredecessor);
		return true;
	}
	else if (papuga_ValueVariant_isequal_ascii( &val, "unique"))
	{
		res.definePositionBind( analyzer::BindPredecessor);
		return true;
	}
	else
	{
		return false;
	}
}

analyzer::FeatureOptions Deserializer::getFeatureOptions(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	analyzer::FeatureOptions rt;
	for (; si != se && si->tag != papuga_TagClose; ++si)
	{
		if (si->tag == papuga_TagName
		&& papuga_ValueVariant_isequal_ascii( &si->value, "position")) ++si;

		if (si->tag == papuga_TagValue)
		{
			if (!setFeatureOption_position( rt, getValue(si,se)))
			{
				throw strus::runtime_error(_TXT("expected position bind option"));
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("expected feature option"));
		}
	}
	if (si != se)
	{
		++si;
	}
	return rt;
}

analyzer::FeatureOptions Deserializer::getFeatureOptions(
	const papuga_ValueVariant& options)
{
	if (options.valuetype != papuga_Serialized)
	{
		analyzer::FeatureOptions rt;
		if (papuga_ValueVariant_defined( &options))
		{
			if (!setFeatureOption_position( rt, options))
			{
				throw strus::runtime_error(_TXT("expected feature option (position bind value)"));
			}
		}
		return rt;
	}
	else
	{
		papuga::Serialization::const_iterator
			si = papuga::Serialization::begin( options.value.serialization),  
			se = papuga::Serialization::end( options.value.serialization);
		return getFeatureOptions( si, se);
	}
}

TermStatistics Deserializer::getTermStatistics(
		const papuga_ValueVariant& val)
{
	static const StructureNameMap namemap( "df", ',');
	static const char* context = _TXT("term statistics");
	TermStatistics rt;
	if (papuga_ValueVariant_isatomic( &val))
	{
		rt.setDocumentFrequency( ValueVariantWrap::touint64( val));
		return rt;
	}
	if (val.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("atomic value (df) or list of named arguments expected for %s"), context);
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( val.value.serialization),  
		se = papuga::Serialization::end( val.value.serialization);
	while (si != se)
	{
		if (si->tag != papuga_TagName)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected for %s as structure"), context);
		}
		int idx = namemap.index( si->value);
		++si;
		switch (idx)
		{
			case 0: rt.setDocumentFrequency( ValueVariantWrap::touint64( getValue(si,se)));
				break;
			default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				break;
		}
	}
	return rt;
}

GlobalStatistics Deserializer::getGlobalStatistics(
		const papuga_ValueVariant& val)
{
	static const StructureNameMap namemap( "nofdocs", ',');
	static const char* context = _TXT("global statistics");
	GlobalStatistics rt;
	if (papuga_ValueVariant_isatomic( &val))
	{
		rt.setNofDocumentsInserted( ValueVariantWrap::touint64( val));
		return rt;
	}
	if (val.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("atomic value (df) or list of named arguments expected for %s"), context);
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( val.value.serialization),
		se = papuga::Serialization::end( val.value.serialization);
	while (si != se)
	{
		if (si->tag != papuga_TagName)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected for %s as structure"), context);
		}
		int idx = namemap.index( si->value);
		++si;
		switch (idx)
		{
			case 0: rt.setNofDocumentsInserted( ValueVariantWrap::touint64( getValue(si,se)));
				break;
			default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
				break;
		}
	}
	return rt;
}

std::string Deserializer::getOptionalDefinition(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se,
		const char* name)
{
	std::string rt;
	if (si->tag == papuga_TagName)
	{
		if (papuga_ValueVariant_isequal_ascii( &si->value, name))
		{
			++si;
			if (si->tag == papuga_TagValue)
			{
				rt = ValueVariantWrap::tostring( si->value);
				++si;
			}
			else
			{
				throw strus::runtime_error(_TXT("option structure (like '%s' declaration) as element of structure expected"), name);
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("option structure (like '%s' declaration) as element of structure expected"), name);
		}
	}
	return rt;
}

analyzer::DocumentClass Deserializer::getDocumentClass(
		const papuga_ValueVariant& val)
{
	static const StructureNameMap namemap( "mimetype,encoding,scheme", ',');
	static const char* context = _TXT("document class");
	analyzer::DocumentClass rt;
	if (val.valuetype != papuga_Serialized)
	{
		rt.setMimeType( ValueVariantWrap::tostring( val));
		return rt;
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( val.value.serialization),
		se = papuga::Serialization::end( val.value.serialization);

	if (si == se) return rt;

	if (si->tag == papuga_TagName)
	{
		do
		{
			int idx = namemap.index( si->value);
			++si;
			switch (idx)
			{
				case 0: rt.setMimeType( Deserializer::getString( si, se));
					break;
				case 1: rt.setEncoding( Deserializer::getString( si, se));
					break;
				case 2: rt.setScheme( Deserializer::getString( si, se));
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
			}
		} while (si != se && si->tag == papuga_TagName);
		Deserializer::consumeClose( si, se);
	}
	else if (si->tag == papuga_TagValue)
	{
		rt.setMimeType( Deserializer::getString( si, se));
		if (si != se && si->tag != papuga_TagClose)
		{
			rt.setEncoding( Deserializer::getString( si, se));
			if (si != se && si->tag != papuga_TagClose)
			{
				rt.setScheme( Deserializer::getString( si, se));
			}
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected %s structure"), context);
	}
	return rt;
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

	Reference<NormalizerFunctionInstanceInterface> function(
			nf->createInstance( arguments, textproc));
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

	Reference<TokenizerFunctionInstanceInterface> function(
			nf->createInstance( arguments, textproc));
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

	Reference<AggregatorFunctionInstanceInterface> function(
			nf->createInstance( arguments));
	if (!function.get())
	{
		throw strus::runtime_error( _TXT("failed to create %s function instance '%s': %s"), context, name.c_str(), errorhnd->fetchError());
	}
	return function;
}

static Reference<NormalizerFunctionInstanceInterface> getNormalizer_(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<NormalizerFunctionInstanceInterface>();
	AnalyzerFunctionDef def( si, se);
	return getNormalizer_( def.name, def.args, textproc, errorhnd);
}

Reference<TokenizerFunctionInstanceInterface> Deserializer::getTokenizer(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<TokenizerFunctionInstanceInterface>();

	AnalyzerFunctionDef def( si, se);
	return getTokenizer_( def.name, def.args, textproc, errorhnd);
}

Reference<AggregatorFunctionInstanceInterface> Deserializer::getAggregator(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<AggregatorFunctionInstanceInterface>();

	AnalyzerFunctionDef def( si, se);
	return getAggregator_( def.name, def.args, textproc, errorhnd);
}

std::vector<Reference<NormalizerFunctionInstanceInterface> > Deserializer::getNormalizers(
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	std::vector< Reference<NormalizerFunctionInstanceInterface> > rt;
	static const char* context = _TXT("normalizer function list");

	while (si != se && si->tag != papuga_TagClose)
	{
		if (si->tag == papuga_TagValue)
		{
			rt.push_back( getNormalizer_( getString( si, se), std::vector<std::string>(), textproc, errorhnd));
		}
		else if (si->tag == papuga_TagOpen)
		{
			++si;
			rt.push_back( getNormalizer_( si, se, textproc, errorhnd));
		}
		else
		{
			throw strus::runtime_error(_TXT("unexpected element in %s"), context);
		}
	}
	return rt;
}

std::vector<Reference<NormalizerFunctionInstanceInterface> > Deserializer::getNormalizers(
		const papuga_ValueVariant& normalizers,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (normalizers.valuetype != papuga_Serialized)
	{
		std::vector<Reference<NormalizerFunctionInstanceInterface> > rt;
		std::string name = ValueVariantWrap::tostring( normalizers);
		rt.push_back( getNormalizer_( name, std::vector<std::string>(), textproc, errorhnd));
		return rt;
	}
	else
	{
		papuga::Serialization::const_iterator
			si = papuga::Serialization::begin( normalizers.value.serialization),
			se = papuga::Serialization::end( normalizers.value.serialization);
		try
		{
			return getNormalizers( si, se, textproc, errorhnd);
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( normalizers.value.serialization));
		}
	}
}


Reference<TokenizerFunctionInstanceInterface> Deserializer::getTokenizer(
		const papuga_ValueVariant& tokenizer,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (tokenizer.valuetype != papuga_Serialized)
	{
		std::string name = ValueVariantWrap::tostring( tokenizer);
		return getTokenizer_( name, std::vector<std::string>(), textproc, errorhnd);
	}
	else
	{
		papuga::Serialization::const_iterator
			si = papuga::Serialization::begin( tokenizer.value.serialization),
			se = papuga::Serialization::end( tokenizer.value.serialization);
		try
		{
			return getTokenizer( si, se, textproc, errorhnd);
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( tokenizer.value.serialization));
		}
	}
}

Reference<AggregatorFunctionInstanceInterface> Deserializer::getAggregator(
		const papuga_ValueVariant& aggregator,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (aggregator.valuetype != papuga_Serialized)
	{
		std::string name = ValueVariantWrap::tostring( aggregator);
		return getAggregator_( name, std::vector<std::string>(), textproc, errorhnd);
	}
	else
	{
		papuga::Serialization::const_iterator
			si = papuga::Serialization::begin( aggregator.value.serialization),
			se = papuga::Serialization::end( aggregator.value.serialization);
		try
		{
			return getAggregator( si, se, textproc, errorhnd);
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( aggregator.value.serialization));
		}
	}
}

template <class FUNCTYPE>
static void instantiateQueryEvalFunctionParameter(
		const char* functionclass,
		FUNCTYPE* function,
		const std::string& name,
		const papuga_ValueVariant& value)
{
	if (papuga_ValueVariant_isnumeric( &value))
	{
		function->addNumericParameter( name, ValueVariantWrap::tonumeric( value));
	}
	else if (papuga_ValueVariant_isstring( &value))
	{
		function->addStringParameter( name, ValueVariantWrap::tostring( value));
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
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s parameter definition"), functionclass);
	if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s parameter definition"), functionclass);
		if (si->tag == papuga_TagName)
		{
			if (papuga_ValueVariant_isequal_ascii( &si->value, "feature"))
			{
				++si;
				if (si->tag == papuga_TagOpen)
				{
					++si;
					while (si->tag == papuga_TagValue)
					{
						featureParameters.push_back( FeatureParameter( paramname, Deserializer::getString( si, se)));
					}
					Deserializer::consumeClose( si, se);
				}
				else if (si->tag == papuga_TagValue)
				{
					featureParameters.push_back( FeatureParameter( paramname, Deserializer::getString( si, se)));
				}
			}
			else
			{
				throw strus::runtime_error(_TXT("unexpected tag name in %s parameter definition"), functionclass);
			}
		}
		else
		{
			while (si != se && si->tag != papuga_TagClose)
			{
				papuga_ValueVariant value = getValue( si, se);
				instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
			}
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		papuga_ValueVariant value = getValue( si, se);
		instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
	}
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionParameter(
		const char* functionclass,
		FUNCTYPE* function,
		std::string& debuginfoAttribute,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	static const StructureNameMap namemap( "name,value,feature", ',');
	if (si->tag == papuga_TagName)
	{
		if (papuga_ValueVariant_isequal_ascii( &si->value, "param"))
		{
			++si;
			if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s parameter definition"), functionclass);
			if (si->tag != papuga_TagOpen) --si;
		}
	}
	if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s parameter definition"), functionclass);
		if (si->tag == papuga_TagValue)
		{
			std::string paramname = Deserializer::getString( si, se);
			if (paramname == "debug")
			{
				debuginfoAttribute = Deserializer::getString( si, se);
			}
			else
			{
				deserializeQueryEvalFunctionParameterValue(
					functionclass, function, featureParameters, paramname, si, se);
			}
			Deserializer::consumeClose( si, se);
		}
		else if (si->tag == papuga_TagName)
		{
			std::string name;
			unsigned char name_defined = 0;
			unsigned char value_defined = 0;
			bool is_feature = false;
			papuga_ValueVariant value;
			while (si->tag == papuga_TagName)
			{
				int idx = namemap.index( si->value);
				++si;
				switch (idx)
				{
					case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s"), "name", functionclass);
						name = Deserializer::getString( si, se);
						break;
					case 1: if (value_defined++) throw strus::runtime_error(_TXT("contradicting definitions in %s parameter: only one allowed of 'value' or 'feature'"), functionclass);
						value = getValue( si, se);
						break;
					case 2:	if (value_defined++) throw strus::runtime_error(_TXT("contradicting definitions in %s parameter: only one allowed of 'value' or 'feature'"), functionclass);
						is_feature=true; value = getValue( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown name in %s parameter list"), functionclass);
				}
			}
			Deserializer::consumeClose( si, se);
			if (!name_defined || !value_defined)
			{
				throw strus::runtime_error(_TXT("incomplete %s definition"), functionclass);
			}
			if (is_feature)
			{
				featureParameters.push_back( FeatureParameter( name, ValueVariantWrap::tostring( value)));
			}
			else
			{
				if (name == "debug")
				{
					debuginfoAttribute = ValueVariantWrap::tostring(value);
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
	else if (si->tag == papuga_TagName)
	{
		std::string paramname = ValueVariantWrap::tostring( si->value);
		++si;
		if (paramname == "debug")
		{
			debuginfoAttribute = Deserializer::getString( si, se);
		}
		else
		{
			deserializeQueryEvalFunctionParameterValue(
				functionclass, function, featureParameters, paramname, si, se);
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
	if (parameters.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("list of named arguments expected as %s parameters"), functionclass);
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( parameters.value.serialization),
		se = papuga::Serialization::end( parameters.value.serialization);
	try
	{
		while (si != se)
		{
			deserializeQueryEvalFunctionParameter(
				functionclass, function, debuginfoAttribute, featureParameters, si, se);
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( parameters.value.serialization));
	}
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
		if (resultnames.valuetype != papuga_Serialized)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected as %s result name definitions"), functionclass);
		}
		papuga::Serialization::const_iterator
			si = papuga::Serialization::begin( resultnames.value.serialization),
			se = papuga::Serialization::end( resultnames.value.serialization);
		try
		{
			while (si != se)
			{
				ConfigDef resultdef( si, se);
				function->defineResultName(
						ValueVariantWrap::tostring( resultdef.value),
						resultdef.name);
			}
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( resultnames.value.serialization));
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
		if (parameter.valuetype != papuga_Serialized)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected as parameters of %s"), context);
		}
		papuga::Serialization::const_iterator
			si = papuga::Serialization::begin( parameter.value.serialization),
			se = papuga::Serialization::end( parameter.value.serialization);
		try
		{
			while (si != se)
			{
				ConfigDef cdef( si, se);
				if (cdef.name == "parser")
				{
					parsername = ValueVariantWrap::tostring( cdef.value);
				}
				else
				{
					paramlist.push_back( ParamDef(
						cdef.name,
						ValueVariantWrap::todouble( cdef.value)));
				}
			}
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( parameter.value.serialization));
		}
	}
	const ScalarFunctionParserInterface* scalarfuncparser = queryproc->getScalarFunctionParser( parsername);
	std::auto_ptr<ScalarFunctionInterface> scalarfunc( scalarfuncparser->createFunction( source, std::vector<std::string>()));
	if (!scalarfunc.get())
	{
		throw strus::runtime_error(_TXT( "failed to create %s (%s) from source: %s"), context, source.c_str(), errorhnd->fetchError());
	}
	std::vector<ParamDef>::const_iterator
		vi = paramlist.begin(),
		ve = paramlist.end();
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


bool Deserializer::skipStructure( papuga::Serialization::const_iterator si, const papuga::Serialization::const_iterator& se)
{
	if (si == se) return false;
	if (si->tag == papuga_TagValue)
	{
		++si;
		return true;
	}
	else if (si->tag == papuga_TagOpen)
	{
		int brkcnt = 1;
		++si;
		for (;si != se && brkcnt; ++si)
		{
			if (si->tag == papuga_TagOpen)
			{
				++brkcnt;
			}
			else if (si->tag == papuga_TagClose)
			{
				--brkcnt;
			}
		}
		return brkcnt == 0;
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
	ExpressionJoin
};

static ExpressionType getExpressionType( papuga::Serialization::const_iterator si, const papuga::Serialization::const_iterator& se)
{
	static const StructureNameMap keywords( "type,value,len,from,to,variable,join,range,cardinality,arg", ',');
	static const ExpressionType keywordTypeMap[10] = {
		ExpressionTerm, ExpressionTerm, ExpressionTerm,			/*type,value,len*/
		ExpressionMetaDataRange,ExpressionMetaDataRange,		/*from,to*/
		ExpressionVariableAssignment,					/*variable*/
		ExpressionJoin,ExpressionJoin,ExpressionJoin,ExpressionJoin	/*join,range,cardinality,arg*/
	};
	if (si == se) return ExpressionUnknown;
	if (si->tag == papuga_TagValue)
	{
		if (papuga_ValueVariant_isstring( &si->value))
		{
			return ExpressionTerm;
		}
		else
		{
			return ExpressionUnknown;
		}
	}
	else if (si->tag == papuga_TagOpen)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s"), "expression");

		if (si->tag == papuga_TagName)
		{
			do
			{
				int ki = keywords.index( si->value);
				if (ki<0) return ExpressionUnknown;
				ExpressionType rt = keywordTypeMap[ ki];
				if (rt != ExpressionVariableAssignment) return rt;
				++si;
				if (!Deserializer::skipStructure( si, se)) return ExpressionUnknown;
			} while (si != se && si->tag == papuga_TagName);
			return ExpressionUnknown;
		}
		else
		{
			if (si->tag == papuga_TagOpen)
			{
				++si;
				if (si != se && si->tag == papuga_TagName
				&& papuga_ValueVariant_isequal_ascii( &si->value, "variable"))
				{
					++si;
					if (!Deserializer::skipStructure( si, se)) return ExpressionUnknown;
				}
				Deserializer::consumeClose( si, se);
			}
			int argc = 0;
			for (; si != se && argc < 3; ++si,++argc)
			{
				if (si->tag != papuga_TagValue) break;
			}
			if (si == se || si->tag == papuga_TagClose)
			{
				return ExpressionTerm;
			}
			else
			{
				return ExpressionJoin;
			}
		}
	}
	else
	{
		return ExpressionUnknown;
	}
}

static void buildExpressionJoin(
		ExpressionBuilder& builder,
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	static const char* context = _TXT("join expression");
	static const StructureNameMap joinop_namemap( "variable,join,range,cardinality,arg", ',');
	enum StructureNameId {JO_variable=0,JO_join=1, JO_range=2, JO_cardinality=3, JO_arg=4};
	papuga_ErrorCode err = papuga_Ok;

	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s"), context);

	if (si->tag == papuga_TagOpen)
	{
		std::string variable;
		std::string op;
		unsigned int argc = 0;
		int range = 0;
		unsigned int cardinality = 0;
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s"), context);

		if (si->tag == papuga_TagName)
		{
			do
			{
				StructureNameId idx = (StructureNameId)joinop_namemap.index( si->value);
				++si;
				switch (idx)
				{
					case JO_variable:
						variable = Deserializer::getString( si, se);
						break;
					case JO_join:
						op = Deserializer::getString( si, se);
						break;
					case JO_range:
						range = Deserializer::getInt( si, se);
						break;
					case JO_cardinality:
						cardinality = Deserializer::getUint( si, se);
						break;
					case JO_arg:
						Deserializer::buildExpression( builder, si, se);
						++argc;
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in %s"), context);
				}
			} while (si != se && si->tag == papuga_TagName);
			Deserializer::consumeClose( si, se);
		}
		else
		{
			if (si->tag == papuga_TagOpen)
			{
				++si;
				variable = Deserializer::getOptionalDefinition( si, se, "variable");
				Deserializer::consumeClose( si, se);
			}
			if (si->tag == papuga_TagValue)
			{
				op = Deserializer::getString( si, se);
				if (si != se && si->tag == papuga_TagValue)
				{
					papuga_ValueVariant val;
					if (papuga_ValueVariant_tonumeric( &si->value, &val, &err))
					{
						++si;
						range = ValueVariantWrap::toint( val);
						if (si != se && si->tag == papuga_TagValue)
						{
							if (papuga_ValueVariant_tonumeric( &si->value, &val, &err))
							{
								++si;
								cardinality = ValueVariantWrap::touint( val);
							}
						}
					}
				}
				while (si != se && si->tag != papuga_TagClose)
				{
					Deserializer::buildExpression( builder, si, se);
					++argc;
				}
				Deserializer::consumeClose( si, se);
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
	else
	{
		throw strus::runtime_error(_TXT("unexpected element in %s"), context);
	}
}

static void builderPushTerm(
		ExpressionBuilder& builder,
		const TermDef& def)
{
	/*[-]*/fprintf( stderr, "deserializer::builderPushTerm %s '%s' %u\n", def.type.c_str(), def.value.c_str(), def.length);
	if (def.value_defined)
	{
		builder.pushTerm( def.type);
	}
	else if (def.length_defined)
	{
		builder.pushTerm( def.type, def.value, def.length);
	}
	else
	{
		builder.pushTerm( def.type, def.value);
	}
	if (!def.variable.empty())
	{
		builder.attachVariable( def.variable);
	}
}

void Deserializer::buildExpression(
		ExpressionBuilder& builder,
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	const char* context = "expression";
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s"), context);
	static const StructureNameMap namemap( "term,metadata,expression", ',');

	if (si->tag == papuga_TagName)
	{
		int idx = namemap.index( si->value);
		++si;
		switch (idx)
		{
			case 0:
			{
				builderPushTerm( builder, TermDef( si, se));
				break;
			}
			case 1:
			{
				MetaDataRangeDef def( si, se);
				builder.pushDocField( def.from, def.to);
				break;
			}
			case 2:
			{
				buildExpressionJoin( builder, si, se);
				break;
			}
			default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
		}
	}
	else
	{
		ExpressionType etype = getExpressionType( si, se);
		switch (etype)
		{
			case ExpressionUnknown:
			{
				throw strus::runtime_error(_TXT("unable to interpret %s"), context);
			}
			case ExpressionTerm:
			{
				builderPushTerm( builder, TermDef( si, se));
				break;
			}
			case ExpressionVariableAssignment:
			{
				throw strus::runtime_error(_TXT("isolated variable assignment in %s"), context);
			}
			case ExpressionMetaDataRange:
			{
				MetaDataRangeDef def( si, se);
				builder.pushDocField( def.from, def.to);
				break;
			}
			case ExpressionJoin:
			{
				buildExpressionJoin( builder, si, se);
				break;
			}
		}
	}
}

void Deserializer::buildExpression(
		ExpressionBuilder& builder,
		const papuga_ValueVariant& expression,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("expression");
	if (expression.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	else
	{
		papuga::Serialization::const_iterator
			si = papuga::Serialization::begin( expression.value.serialization),
			se = papuga::Serialization::end( expression.value.serialization);
		try
		{
			return buildExpression( builder, si, se);
		}
		catch (const std::runtime_error& err)
		{
			throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( expression.value.serialization));
		}
	}
}

void Deserializer::buildPatterns(
		ExpressionBuilder& builder,
		const papuga_ValueVariant& patterns,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "name,expression,visible", ',');
	static const char* context = _TXT("pattern");
	if (!papuga_ValueVariant_defined( &patterns)) return;
	if (patterns.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for list of %s"), context);
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( patterns.value.serialization),
		se = papuga::Serialization::end( patterns.value.serialization);
	try
	{
		while (si != se)
		{
			if (si->tag == papuga_TagName)
			{
				if (papuga_ValueVariant_isequal_ascii( &si->value, "pattern"))
				{
					++si;
					if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);
					if (si->tag != papuga_TagOpen) --si;
				}
				else
				{
					throw strus::runtime_error(_TXT("structure with optional name 'pattern' expected"), context);
				}
			}
			if (si->tag == papuga_TagOpen)
			{
				++si;
				if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), context);
				std::string name;
				bool visible = true;
				if (si->tag == papuga_TagName)
				{
					do
					{
						int ki = namemap.index( si->value);
						++si;
						switch (ki)
						{
							case 0: name = ValueVariantWrap::tostring( getValue( si, se));
								break;
							case 1: buildExpression( builder, si, se);
								break;
							case 2: visible = ValueVariantWrap::tobool( getValue( si, se));
								break;
							default:throw strus::runtime_error(_TXT("unknown element in %s definition"), context);
						}
					} while (si != se && si->tag == papuga_TagName);
				}
				else
				{
					if (si->tag == papuga_TagOpen)
					{
						++si;
						std::string visibility = Deserializer::getOptionalDefinition( si, se, "access");
						Deserializer::consumeClose( si, se);

						if (visibility == "public")
						{
							visible = true;
						}
						else if (visibility == "private")
						{
							visible = false;
						}
						else
						{
							throw strus::runtime_error(_TXT("expected 'private' or 'public' as access flag of %s"), context);
						}
					}
					if (si->tag == papuga_TagValue)
					{
						name = getString( si, se);
						buildExpression( builder, si, se);
					}
					else
					{
						throw strus::runtime_error(_TXT("error in %s definition structure"), context);
					}
				}
				Deserializer::consumeClose( si, se);
				builder.definePattern( name, visible);
			}
			else
			{
				throw strus::runtime_error(_TXT("%s definition expected to be a structure"), context);
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( patterns.value.serialization));
	}
}

template <class StorageDocumentAccess>
static void buildMetaData(
		StorageDocumentAccess* document,
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	static const StructureNameMap namemap( "name,value", ',');
	static const char* context = _TXT("document metadata");
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);

	if (si->tag != papuga_TagOpen) throw strus::runtime_error(_TXT("sub structure expected as content of %s section"), context);
	++si;

	while (si != se && si->tag != papuga_TagClose)
	{
		if (si->tag == papuga_TagName)
		{
			if (papuga_ValueVariant_isequal_ascii( &si->value, "assign"))
			{
				++si;
				if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s parameter definition"), context);
				if (si->tag != papuga_TagOpen) --si;
			}
		}
		if (si->tag == papuga_TagOpen)
		{
			if (++si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);

			if (si->tag == papuga_TagName)
			{
				unsigned char name_defined = 0;
				unsigned char value_defined = 0;
				std::string name;
				NumericVariant value;
				do
				{
					int idx = namemap.index( si->value);
					++si;
					switch (idx)
					{
						case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "name", context);
							name = Deserializer::getString( si, se);
							break;
						case 1: if (value_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "value", context);
							value = ValueVariantWrap::tonumeric( getValue( si, se));
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
					}
				}
				while (si->tag == papuga_TagName);
				if (!name_defined || !value_defined)
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				document->setMetaData( name, value);
			}
			else if (si->tag == papuga_TagValue)
			{
				std::string name = Deserializer::getString( si, se);
				if (si->tag != papuga_TagValue)
				{
					throw strus::runtime_error(_TXT("expected 2-tuple for element in %s function"), context);
				}
				NumericVariant value = ValueVariantWrap::tonumeric( getValue( si, se));
				document->setMetaData( name, value);
			}
			else
			{
				throw strus::runtime_error(_TXT("structure expected with named elements or tuple with positional arguments for %s"), context);
			}
			Deserializer::consumeClose( si, se);
		}
		else if (si->tag == papuga_TagName)
		{
			std::string name = ValueVariantWrap::tostring( si->value);
			++si;
			NumericVariant value = ValueVariantWrap::tonumeric( getValue( si, se));
			document->setMetaData( name, value);
		}
		else
		{
			throw strus::runtime_error(_TXT("named elements or structures expected for %s"), context);
		}
	}
	Deserializer::consumeClose( si, se);
}

template <class StorageDocumentAccess>
static void buildAttributes(
		StorageDocumentAccess* document,
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	static const StructureNameMap namemap( "name,value", ',');
	static const char* context = _TXT("document attributes");
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);
	if (si->tag != papuga_TagOpen) throw strus::runtime_error(_TXT("sub structure expected as content of %s section"), context);
	++si;

	while (si != se && si->tag != papuga_TagClose)
	{
		if (si->tag == papuga_TagName)
		{
			if (papuga_ValueVariant_isequal_ascii( &si->value, "attribute"))
			{
				++si;
				if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);
				if (si->tag != papuga_TagOpen) --si;
			}
		}
		if (si->tag == papuga_TagOpen)
		{
			if (++si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);

			if (si->tag == papuga_TagName)
			{
				unsigned char name_defined = 0;
				unsigned char value_defined = 0;
				std::string name;
				std::string value;
				do
				{
					int idx = namemap.index( si->value);
					++si;
					switch (idx)
					{
						case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "name", context);
							name = Deserializer::getString( si, se);
							break;
						case 1: if (value_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "value", context);
							value = Deserializer::getString( si, se);
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
					}
				}
				while (si->tag == papuga_TagName);
				if (!name_defined || !value_defined)
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				document->setAttribute( name, value);
			}
			else if (si->tag == papuga_TagValue)
			{
				std::string name = Deserializer::getString( si, se);
				if (si->tag != papuga_TagValue)
				{
					throw strus::runtime_error(_TXT("expected 2-tuple for element in %s function"), context);
				}
				std::string value = Deserializer::getString( si, se);
				document->setAttribute( name, value);
			}
			else
			{
				throw strus::runtime_error(_TXT("structure expected with named elements or tuple with positional arguments for %s"), context);
			}
			Deserializer::consumeClose( si, se);
		}
		else if (si->tag == papuga_TagName)
		{
			std::string name = ValueVariantWrap::tostring( si->value);
			++si;
			std::string value = Deserializer::getString( si, se);
			document->setAttribute( name, value);
		}
		else
		{
			throw strus::runtime_error(_TXT("named elements or structures expected for %s"), context);
		}
	}
	Deserializer::consumeClose( si, se);
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
static void buildStorageIndex(
		StorageDocumentIndexAccess* document,
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	static const StructureNameMap namemap( "type,value,pos,len", ',');
	static const char* context = _TXT("search index");
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);
	if (si->tag != papuga_TagOpen) throw strus::runtime_error(_TXT("sub structure expected as content of %s section"), context);
	++si;

	while (si != se && si->tag != papuga_TagClose)
	{
		if (si->tag == papuga_TagName)
		{
			if (papuga_ValueVariant_isequal_ascii( &si->value, "term"))
			{
				++si;
				if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);
				if (si->tag != papuga_TagOpen) --si;
			}
			else
			{
				throw strus::runtime_error(_TXT("structure with optional name 'term' expected"), context);
			}
		}
		if (si->tag == papuga_TagOpen)
		{
			if (++si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);

			if (si->tag == papuga_TagName)
			{
				unsigned char type_defined = 0;
				unsigned char value_defined = 0;
				unsigned char pos_defined = 0;
				std::string type;
				std::string value;
				unsigned int pos;
				do
				{
					int idx = namemap.index( si->value);
					++si;
					switch (idx)
					{
						case 0: if (type_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "name", context);
							type = Deserializer::getString( si, se);
							break;
						case 1: if (value_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "value", context);
							value = Deserializer::getString( si, se);
							break;
						case 2: if (pos_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "pos", context);
							pos = Deserializer::getUint( si, se);
							break;
						case 3: (void)Deserializer::getUint( si, se);
							// ... len that is part of analyzer output is ignored
							break;
						default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
					}
				}
				while (si->tag == papuga_TagName);
				if (!type_defined || !value_defined || !pos_defined)
				{
					throw strus::runtime_error(_TXT("incomplete %s definition"), context);
				}
				document->addTerm( type, value, pos);
			}
			else if (si->tag == papuga_TagValue)
			{
				std::string type = Deserializer::getString( si, se);
				if (si->tag != papuga_TagValue)
				{
					throw strus::runtime_error(_TXT("expected 3-tuple for element in %s"), context);
				}
				std::string value = Deserializer::getString( si, se);
				if (si->tag != papuga_TagValue)
				{
					throw strus::runtime_error(_TXT("expected 3-tuple for element in %s"), context);
				}
				unsigned int pos = Deserializer::getUint( si, se);
				document->addTerm( type, value, pos);
			}
			else
			{
				throw strus::runtime_error(_TXT("structure expected with named elements or tuple with positional arguments for %s"), context);
			}
			Deserializer::consumeClose( si, se);
		}
		else
		{
			throw strus::runtime_error(_TXT("named elements or structures expected for %s"), context);
		}
	}
	Deserializer::consumeClose( si, se);
}

template <class StorageDocumentAccess>
static void buildAccessRights(
		StorageDocumentAccess* document,
		papuga::Serialization::const_iterator& si,
		const papuga::Serialization::const_iterator& se)
{
	static const char* context = _TXT("document access rights");
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), context);

	if (si->tag == papuga_TagOpen)
	{
		++si;
		while (si != se && si->tag != papuga_TagClose)
		{
			if (si->tag == papuga_TagName)
			{
				if (papuga_ValueVariant_isequal_ascii( &si->value, "user"))
				{
					++si;
				}
			}
			document->setUserAccessRight( Deserializer::getString( si, se));
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		document->setUserAccessRight( Deserializer::getString( si, se));
	}
}

template <class StorageDocumentAccess>
static void buildStorageDocument(
		StorageDocumentAccess* document,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "doctype,attributes,metadata,searchindex,forwardindex,access", ',');
	static const char* context = _TXT("document");
	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( content.value.serialization),
		se = papuga::Serialization::end( content.value.serialization);
	try
	{
		while (si != se)
		{
			if (si->tag != papuga_TagName)
			{
				throw strus::runtime_error(_TXT("section name expected for top elements of %s structure"), context);
			}
			int idx = namemap.index( si->value);
			++si;
			switch (idx)
			{
				case 0: (void)Deserializer::getString( si, se);
					// ... ignore sub document type (output of analyzer)
					break;
				case 1: buildAttributes( document, si, se);
					break;
				case 2: buildMetaData( document, si, se);
					break;
				case 3:
				{
					StorageDocumentSearchIndexAccess<StorageDocumentAccess>
						da( document);
					buildStorageIndex( &da, si, se);
					break;
				}
				case 4:
				{
					StorageDocumentForwardIndexAccess<StorageDocumentAccess>
						da( document);
					buildStorageIndex( &da, si, se);
					break;
				}
				case 5: buildAccessRights( document, si, se);
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( content.value.serialization));
	}
}

static void buildStorageDocumentDeletes(
		StorageDocumentUpdateInterface* document,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "attributes,metadata,searchindex,forwardindex,access", ',');
	static const char* context = _TXT("document update deletes");
	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( content.value.serialization),
		se = papuga::Serialization::end( content.value.serialization);
	try
	{
		while (si != se)
		{
			if (si->tag != papuga_TagName)
			{
				throw strus::runtime_error(_TXT("section name expected for top elements of %s structure"), context);
			}
			int idx = namemap.index( si->value);
			++si;
			switch (idx)
			{
				case 0:
				{
					std::vector<std::string> deletes( Deserializer::getStringList( si, se));
					std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
					for (; di != de; ++di)
					{
						document->clearAttribute( *di);
					}
					break;
				}
				case 1: buildMetaData( document, si, se);
				{
					std::vector<std::string> deletes( Deserializer::getStringList( si, se));
					std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
					for (; di != de; ++di)
					{
						document->setMetaData( *di, NumericVariant());
					}
					break;
				}
				case 2:
				{
					std::vector<std::string> deletes( Deserializer::getStringList( si, se));
					std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
					for (; di != de; ++di)
					{
						document->clearSearchIndexTerm( *di);
					}
					break;
				}
				case 3:
				{
					std::vector<std::string> deletes( Deserializer::getStringList( si, se));
					std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
					for (; di != de; ++di)
					{
						document->clearForwardIndexTerm( *di);
					}
					break;
				}
				case 4:
				{
					if (si != se && si->tag == papuga_TagValue
						&& papuga_ValueVariant_isequal_ascii( &si->value, "*"))
					{
						document->clearUserAccessRights();
					}
					else
					{
						std::vector<std::string> deletes( Deserializer::getStringList( si, se));
						std::vector<std::string>::const_iterator di = deletes.begin(), de = deletes.end();
						for (; di != de; ++di)
						{
							document->clearUserAccessRight( *di);
						}
					}
					break;
				}
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( content.value.serialization));
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
	buildStorageDocument( document, content, errorhnd);
	buildStorageDocumentDeletes( document, deletes, errorhnd);
}

void Deserializer::buildStatistics(
		StatisticsBuilderInterface* builder,
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "dfchange,nofdocs", ',');
	static const char* context = _TXT("statistics");

	if (!papuga_ValueVariant_defined( &content)) return;
	if (content.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), context);
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( content.value.serialization),
		se = papuga::Serialization::end( content.value.serialization);
	try
	{
		while (si != se)
		{
			if (si->tag != papuga_TagName)
			{
				throw strus::runtime_error(_TXT("section name expected for top elements of %s structure"), context);
			}
			int idx = namemap.index( si->value);
			++si;
			switch (idx)
			{
				case 0:
				{
					if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s"), context);
					if (si->tag == papuga_TagOpen)
					{
						++si;
						while (si != se && si->tag != papuga_TagClose)
						{
							if (si->tag == papuga_TagName && papuga_ValueVariant_isequal_ascii( &si->value, "dfchange"))
							{
								//... skip name of structure if defined (for example for XML)
								++si;
							}
							DfChangeDef dfchg( si, se);
							builder->addDfChange( dfchg.termtype, dfchg.termvalue, dfchg.increment);
						}
						Deserializer::consumeClose( si, se);
					}
				}
				case 1:
				{
					builder->setNofDocumentsInsertedChange( Deserializer::getInt( si, se));
				}
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), context);
			}
		}
	}	
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( content.value.serialization));
	}
}

std::string Deserializer::getStorageConfigString(
		const papuga_ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const char* context = _TXT("storage configuration");
	if (!papuga_ValueVariant_defined( &content)) return std::string();
	if (papuga_ValueVariant_isstring( &content))
	{
		return ValueVariantWrap::tostring( content);
	}
	if (content.valuetype != papuga_Serialized)
	{
		throw strus::runtime_error(_TXT("serialized structure or string expected for %s"), context);
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( content.value.serialization),
		se = papuga::Serialization::end( content.value.serialization);
	try
	{
		std::string rt;
		while (si != se)
		{
			if (rt.empty()) rt.push_back(';');
			ConfigDef item( si, se);
			rt.append( item.name);
			rt.push_back('=');
			if (papuga_ValueVariant_isstring( &item.value))
			{
				std::string value = ValueVariantWrap::tostring( item.value);
				if (std::strchr( value.c_str(), '\"') == 0)
				{
					rt.push_back('\"');
					rt.append( value);
					rt.push_back('\"');
				}
				else if (std::strchr( value.c_str(), '\'') == 0)
				{
					rt.push_back('\'');
					rt.append( value);
					rt.push_back('\'');
				}
				else
				{
					throw strus::runtime_error(_TXT("cannot build configuration string with values containing both type of quotes (\' and \")"));
				}
			}
			else if (papuga_ValueVariant_isatomic( &item.value))
			{
				std::string value = ValueVariantWrap::tostring( item.value);
				rt.append( value);
			}
			else
			{
				throw strus::runtime_error(_TXT("atomic values expected for %s"), context);
			}
		}
		return rt;
	}
	catch (const std::runtime_error& err)
	{
		throw runtime_error_with_location( err.what(), errorhnd, si, se, papuga::Serialization::begin( content.value.serialization));
	}
}
