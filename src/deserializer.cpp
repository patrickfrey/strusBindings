/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "deserializer.hpp"
#include "valueVariantConv.hpp"
#include "internationalization.hpp"
#include "structDefs.hpp"
#include "wcharString.hpp"
#include "strus/bindings/serialization.hpp"
#include <string>

using namespace strus;
using namespace strus::bindings;

static void throw_error_with_location( const char* msg, ErrorBufferInterface* errorhnd, Serialization::const_iterator si, const Serialization::const_iterator& se, const Serialization::const_iterator& start)
{
	std::string msgbuf;
	try
	{
		std::string location_explain;
		Serialization::const_iterator errorpos = si;
		for (int cnt=0; si != start && cnt < 5; --si,++cnt){}
		
		for (int cnt=0; si != se && cnt < 15; ++cnt,++si)
		{
			if (si == errorpos)
			{
				location_explain.append( " <!> ");
			}
			switch (si->tag)
			{
				case Serialization::Open:
					location_explain.push_back( '[');
					break;
				case Serialization::Close:
					location_explain.push_back( ']');
					break;
				case Serialization::Name:
					location_explain.append( ValueVariantConv::tostring( *si));
					location_explain.push_back( ':');
					break;
				case Serialization::Value:
					if (!si->defined())
					{
						location_explain.append( "<null>");
					}
					else if (si->isAtomicType())
					{
						if (si->isNumericType())
						{
							location_explain.append( ValueVariantConv::tostring( *si));
						}
						else
						{
							location_explain.push_back( '"');
							location_explain.append( ValueVariantConv::tostring( *si));
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
		throw strus::runtime_error( msgbuf);
	}
	catch (const std::exception&)
	{
		throw strus::runtime_error( msg);
	}
}

static const ValueVariant& getValue(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si != se && si->tag == Serialization::Value)
	{
		return *si++;
	}
	else
	{
		throw strus::runtime_error(_TXT("expected value in structure"));
	}
}

unsigned int Deserializer::getUint(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	return ValueVariantConv::touint( getValue( si, se));
}

unsigned int Deserializer::getInt(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	return ValueVariantConv::toint( getValue( si, se));
}

std::string Deserializer::getString( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	return ValueVariantConv::tostring( getValue( si, se));
}

void Deserializer::consumeClose(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si != se)
	{
		if (si->tag != Serialization::Close)
		{
			throw strus::runtime_error(_TXT("unexpected element at end of %s, close expected"), "structure");
		}
		++si;
	}
}

std::vector<std::string> Deserializer::getStringList( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	std::vector<std::string> rt;
	while (si != se && si->tag != Serialization::Close)
	{
		rt.push_back( getString( si, se));
	}
	return rt;
}

std::vector<std::string> Deserializer::getStringList(
		const ValueVariant& val)
{
	std::vector<std::string> rt;
	if (val.type != ValueVariant::StrusSerialization)
	{
		rt.push_back( ValueVariantConv::tostring( val));
		return rt;
	}
	else
	{
		Serialization::const_iterator
			si = val.value.serialization->begin(),  
			se = val.value.serialization->end();
		return getStringList( si, se);
	}
}

std::vector<int> Deserializer::getIntList(
		const ValueVariant& val)
{
	std::vector<int> rt;
	if (val.type != ValueVariant::StrusSerialization)
	{
		rt.push_back( ValueVariantConv::toint( val));
	}
	else
	{
		Serialization::const_iterator
			si = val.value.serialization->begin(),  
			se = val.value.serialization->end();

		while (si != se && si->tag != Serialization::Close)
		{
			rt.push_back( getInt( si, se));
		}
	}
	return rt;
}

std::vector<Index> Deserializer::getIndexList(
		const ValueVariant& val)
{
	std::vector<Index> rt;
	if (val.type != ValueVariant::StrusSerialization)
	{
		rt.push_back( ValueVariantConv::toint( val));
	}
	else
	{
		Serialization::const_iterator
			si = val.value.serialization->begin(),  
			se = val.value.serialization->end();

		while (si != se && si->tag != Serialization::Close)
		{
			rt.push_back( getInt( si, se));
		}
	}
	return rt;
}

std::string Deserializer::getPrefixStringValue(
		const ValueVariant& val,
		unsigned char prefix)
{
	if (val.type == ValueVariant::String)
	{
		if (val.length > 1 && (unsigned char)val.value.string[0] == prefix)
		{
			return std::string( val.value.string+1, val.length-1);
		}
	}
	else if (val.type == ValueVariant::WString)
	{
		if (val.length > 1 && val.value.wstring[0] == prefix)
		{
			return convert_w16string_to_uft8string( val.value.wstring+1, val.length-1);
		}
	}
	throw strus::runtime_error(_TXT("expected string with '%c' prefix"), prefix);
}

bool Deserializer::isStringWithPrefix(
		const ValueVariant& val,
		unsigned char prefix)
{
	if (val.type == ValueVariant::String)
	{
		if (val.length > 1 && (unsigned char)val.value.string[0] == prefix)
		{
			return true;
		}
	}
	else if (val.type == ValueVariant::WString)
	{
		if (val.length > 1 && val.value.wstring[0] == prefix)
		{
			return true;
		}
	}
	return false;
}

static int getStringPrefix(
		const ValueVariant& val,
		const char* prefixList)
{
	if (val.type == ValueVariant::String)
	{
		if (val.length > 1)
		{
			const char* pi = std::strchr( prefixList, (char)val.value.string[0]);
			if (!pi) return -1;
			return pi - prefixList;
		}
	}
	else if (val.type == ValueVariant::WString)
	{
		if (val.length > 1)
		{
			if (val.value.wstring[0] >= 127) return -1;
			char chr = (char)val.value.wstring[0];
			const char* pi = std::strchr( prefixList, chr);
			if (!pi) return -1;
			return pi - prefixList;
		}
	}
	return -1;
}

static bool setFeatureOption_position( analyzer::FeatureOptions& res, const ValueVariant& val)
{
	if (ValueVariantConv::isequal_ascii( val, "content"))
	{
		res.definePositionBind( analyzer::BindContent);
		return true;
	}
	else if (ValueVariantConv::isequal_ascii( val, "succ"))
	{
		res.definePositionBind( analyzer::BindSuccessor);
		return true;
	}
	else if (ValueVariantConv::isequal_ascii( val, "pred"))
	{
		res.definePositionBind( analyzer::BindPredecessor);
		return true;
	}
	else if (ValueVariantConv::isequal_ascii( val, "unique"))
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
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	analyzer::FeatureOptions rt;
	for (; si != se && si->tag != Serialization::Close; ++si)
	{
		if (si->tag == Serialization::Name
		&& ValueVariantConv::isequal_ascii( *si, "position")) ++si;

		if (si->tag == Serialization::Value)
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
	const ValueVariant& options)
{
	if (options.type != ValueVariant::StrusSerialization)
	{
		analyzer::FeatureOptions rt;
		if (!setFeatureOption_position( rt, options))
		{
			throw strus::runtime_error(_TXT("expected feature option (position bind value)"));
		}
		return rt;
	}
	else
	{
		Serialization::const_iterator
			si = options.value.serialization->begin(),  
			se = options.value.serialization->end();
		return getFeatureOptions( si, se);
	}
}

TermStatistics Deserializer::getTermStatistics(
		const ValueVariant& val)
{
	static const StructureNameMap namemap( "df", ',');
	TermStatistics rt;
	if (val.isAtomicType())
	{
		rt.setDocumentFrequency( ValueVariantConv::touint64( val));
		return rt;
	}
	if (val.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("atomic value (df) or list of named arguments expected for %s"), "term statistics");
	}
	Serialization::const_iterator
		si = val.value.serialization->begin(),  
		se = val.value.serialization->end();
	while (si != se)
	{
		if (si->tag != Serialization::Name)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected for %s as structure"), "term statistics");
		}
		switch (namemap.index( *si++))
		{
			case 0: rt.setDocumentFrequency( ValueVariantConv::touint64( getValue(si,se)));
				break;
			default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), "term statistics");
				break;
		}
	}
	return rt;
}

GlobalStatistics Deserializer::getGlobalStatistics(
		const ValueVariant& val)
{
	static const StructureNameMap namemap( "nofdocs", ',');
	GlobalStatistics rt;
	if (val.isAtomicType())
	{
		rt.setNofDocumentsInserted( ValueVariantConv::touint64( val));
		return rt;
	}
	if (val.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("atomic value (df) or list of named arguments expected for %s"), "term statistics");
	}
	Serialization::const_iterator
		si = val.value.serialization->begin(),  
		se = val.value.serialization->end();
	while (si != se)
	{
		if (si->tag != Serialization::Name)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected for %s as structure"), "global statistics");
		}
		switch (namemap.index( *si++))
		{
			case 0: rt.setNofDocumentsInserted( ValueVariantConv::touint64( getValue(si,se)));
				break;
			default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), "global statistics");
				break;
		}
	}
	return rt;
}

analyzer::DocumentClass Deserializer::getDocumentClass(
		const ValueVariant& val)
{
	static const StructureNameMap namemap( "mimetype,encoding,scheme", ',');
	analyzer::DocumentClass rt;
	if (val.type != ValueVariant::StrusSerialization)
	{
		rt.setMimeType( ValueVariantConv::tostring( val));
		return rt;
	}
	Serialization::const_iterator
		si = val.value.serialization->begin(),  
		se = val.value.serialization->end();

	if (si == se) return rt;

	if (si->tag == Serialization::Name)
	{
		do
		{
			switch (namemap.index( *si++))
			{
				case 0: rt.setMimeType( Deserializer::getString( si, se));
					break;
				case 1: rt.setEncoding( Deserializer::getString( si, se));
					break;
				case 2: rt.setScheme( Deserializer::getString( si, se));
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), "document class");
			}
		} while (si != se && si->tag == Serialization::Name);
		Deserializer::consumeClose( si, se);
	}
	else if (si->tag == Serialization::Value)
	{
		rt.setMimeType( Deserializer::getString( si, se));
		if (si != se && si->tag != Serialization::Close)
		{
			rt.setEncoding( Deserializer::getString( si, se));
			if (si != se && si->tag != Serialization::Close)
			{
				rt.setScheme( Deserializer::getString( si, se));
			}
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected %s structure"), "document class");
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
	if (!nf) throw strus::runtime_error( _TXT("failed to get normalizer function '%s': %s"), name.c_str(), errorhnd->fetchError());

	Reference<NormalizerFunctionInstanceInterface> function(
			nf->createInstance( arguments, textproc));
	if (!function.get())
	{
		throw strus::runtime_error( _TXT("failed to create normalizer function instance '%s': %s"), name.c_str(), errorhnd->fetchError());
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
	if (!nf) throw strus::runtime_error( _TXT("failed to get tokenizer function '%s': %s"), name.c_str(), errorhnd->fetchError());

	Reference<TokenizerFunctionInstanceInterface> function(
			nf->createInstance( arguments, textproc));
	if (!function.get())
	{
		throw strus::runtime_error( _TXT("failed to create tokenizer function instance '%s': %s"), name.c_str(), errorhnd->fetchError());
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
	if (!nf) throw strus::runtime_error( _TXT("failed to get aggregator function '%s': %s"), name.c_str(), errorhnd->fetchError());

	Reference<AggregatorFunctionInstanceInterface> function(
			nf->createInstance( arguments));
	if (!function.get())
	{
		throw strus::runtime_error( _TXT("failed to create aggregator function instance '%s': %s"), name.c_str(), errorhnd->fetchError());
	}
	return function;
}

static Reference<NormalizerFunctionInstanceInterface> getNormalizer_(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<NormalizerFunctionInstanceInterface>();
	AnalyzerFunctionDef def( si, se);
	return getNormalizer_( def.name, def.args, textproc, errorhnd);
}

Reference<TokenizerFunctionInstanceInterface> Deserializer::getTokenizer(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<TokenizerFunctionInstanceInterface>();

	AnalyzerFunctionDef def( si, se);
	return getTokenizer_( def.name, def.args, textproc, errorhnd);
}

Reference<AggregatorFunctionInstanceInterface> Deserializer::getAggregator(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<AggregatorFunctionInstanceInterface>();

	AnalyzerFunctionDef def( si, se);
	return getAggregator_( def.name, def.args, textproc, errorhnd);
}

std::vector<Reference<NormalizerFunctionInstanceInterface> > Deserializer::getNormalizers(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	std::vector< Reference<NormalizerFunctionInstanceInterface> > rt;

	while (si != se && si->tag != Serialization::Close)
	{
		if (si->tag == Serialization::Value)
		{
			rt.push_back( getNormalizer_( getString( si, se), std::vector<std::string>(), textproc, errorhnd));
		}
		else if (si->tag == Serialization::Open)
		{
			++si;
			rt.push_back( getNormalizer_( si, se, textproc, errorhnd));
		}
		else
		{
			throw strus::runtime_error(_TXT("unexpected element in %s structure"), "normalizer list");
		}
	}
	if (si != se)
	{
		++si;
	}
	return rt;
}

std::vector<Reference<NormalizerFunctionInstanceInterface> > Deserializer::getNormalizers(
		const ValueVariant& normalizers,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (normalizers.type != ValueVariant::StrusSerialization)
	{
		std::vector<Reference<NormalizerFunctionInstanceInterface> > rt;
		std::string name = ValueVariantConv::tostring( normalizers);
		rt.push_back( getNormalizer_( name, std::vector<std::string>(), textproc, errorhnd));
		return rt;
	}
	else
	{
		Serialization::const_iterator
			si = normalizers.value.serialization->begin(),  
			se = normalizers.value.serialization->end();
		try
		{
			return getNormalizers( si, se, textproc, errorhnd);
		}
		catch (const std::runtime_error& err)
		{
			throw_error_with_location( err.what(), errorhnd, si, se, normalizers.value.serialization->begin());
		}
	}
}


Reference<TokenizerFunctionInstanceInterface> Deserializer::getTokenizer(
		const ValueVariant& tokenizer,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (tokenizer.type != ValueVariant::StrusSerialization)
	{
		std::string name = ValueVariantConv::tostring( tokenizer);
		return getTokenizer_( name, std::vector<std::string>(), textproc, errorhnd);
	}
	else
	{
		Serialization::const_iterator
			si = tokenizer.value.serialization->begin(),
			se = tokenizer.value.serialization->end();
		try
		{
			return getTokenizer( si, se, textproc, errorhnd);
		}
		catch (const std::runtime_error& err)
		{
			throw_error_with_location( err.what(), errorhnd, si, se, tokenizer.value.serialization->begin());
		}
	}
}

Reference<AggregatorFunctionInstanceInterface> Deserializer::getAggregator(
		const ValueVariant& aggregator,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (aggregator.type != ValueVariant::StrusSerialization)
	{
		std::string name = ValueVariantConv::tostring( aggregator);
		return getAggregator_( name, std::vector<std::string>(), textproc, errorhnd);
	}
	else
	{
		Serialization::const_iterator
			si = aggregator.value.serialization->begin(),
			se = aggregator.value.serialization->end();
		try
		{
			return getAggregator( si, se, textproc, errorhnd);
		}
		catch (const std::runtime_error& err)
		{
			throw_error_with_location( err.what(), errorhnd, si, se, aggregator.value.serialization->begin());
		}
	}
}

template <class FUNCTYPE>
static void instantiateQueryEvalFunctionParameter(
		const char* functionclass,
		FUNCTYPE* function,
		const std::string& name,
		const ValueVariant& value)
{
	if (value.isNumericType())
	{
		function->addNumericParameter( name, ValueVariantConv::tonumeric( value));
	}
	else if (value.isStringType())
	{
		function->addStringParameter( name, ValueVariantConv::tostring( value));
	}
	else
	{
		throw strus::runtime_error(_TXT("atomic value expected as %s function argument (%s)"), functionclass, name.c_str());
	}
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionParameterValue(
		const char* functionclass,
		FUNCTYPE* function,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
		const std::string& paramname,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s function parameter definition"), functionclass);
	if (si->tag == Serialization::Open)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s function parameter definition"), functionclass);
		if (si->tag == Serialization::Name)
		{
			if (ValueVariantConv::isequal_ascii( *si, "feature"))
			{
				++si;
				if (si->tag == Serialization::Open)
				{
					++si;
					while (si->tag == Serialization::Value)
					{
						featureParameters.push_back( FeatureParameter( paramname, Deserializer::getString( si, se)));
					}
					Deserializer::consumeClose( si, se);
				}
				else if (si->tag == Serialization::Value)
				{
					featureParameters.push_back( FeatureParameter( paramname, Deserializer::getString( si, se)));
				}
			}
			else
			{
				throw strus::runtime_error(_TXT("unexpected tag name in %s function parameter definition"), functionclass);
			}
		}
		else
		{
			while (si != se && si->tag != Serialization::Close)
			{
				ValueVariant value = getValue( si, se);
				if (value.isStringType() && Deserializer::isStringWithPrefix( value, '.'))
				{
					std::string featureSet = Deserializer::getPrefixStringValue( value, '.');
					featureParameters.push_back( FeatureParameter( paramname, featureSet));
				}
				else
				{
					instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
				}
			}
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		ValueVariant value = getValue( si, se);
		if (value.isStringType() && Deserializer::isStringWithPrefix( value, '.'))
		{
			std::string featureSet = Deserializer::getPrefixStringValue( value, '.');
			featureParameters.push_back( FeatureParameter( paramname, featureSet));
		}
		else
		{
			instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
		}
	}
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionParameter(
		const char* functionclass,
		FUNCTYPE* function,
		std::string& debuginfoAttribute,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	static const StructureNameMap namemap( "name,value,feature", ',');
	if (si->tag == Serialization::Open)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s function parameter definition"), functionclass);
		if (si->tag == Serialization::Value)
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
		else if (si->tag == Serialization::Name)
		{
			std::string name;
			unsigned char name_defined = 0;
			unsigned char value_defined = 0;
			bool is_feature = false;
			ValueVariant value;
			while (si->tag == Serialization::Name)
			{
				switch (namemap.index( *si++))
				{
					case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s' in %s function"), "name", functionclass);
						name = Deserializer::getString( si, se);
						break;
					case 1: if (value_defined++) throw strus::runtime_error(_TXT("contradicting definitions in %s function parameter: only one allowed of 'value' or 'feature'"), functionclass);
						value = getValue( si, se);
						break;
					case 2:	if (value_defined++) throw strus::runtime_error(_TXT("contradicting definitions in %s function parameter: only one allowed of 'value' or 'feature'"), functionclass);
						is_feature=true; value = getValue( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown name in %s function parameter list"), functionclass);
				}
			}
			Deserializer::consumeClose( si, se);
			if (!name_defined || !value_defined)
			{
				throw strus::runtime_error(_TXT("incomplete %s function definition"), functionclass);
			}
			if (is_feature)
			{
				featureParameters.push_back( FeatureParameter( name, ValueVariantConv::tostring( value)));
			}
			else
			{
				if (name == "debug")
				{
					debuginfoAttribute = ValueVariantConv::tostring(value);
				}
				else
				{
					instantiateQueryEvalFunctionParameter( functionclass, function, name, value);
				}
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("incomplete %s function definition"), functionclass);
		}
	}
	else if (si->tag == Serialization::Name)
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
	}
	else
	{
		throw strus::runtime_error(_TXT("format error in %s function parameter structure"), functionclass);
	}
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionParameters(
		const char* functionclass,
		FUNCTYPE* function,
		std::string& debuginfoAttribute,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
		const ValueVariant& parameters)
{
	if (parameters.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("list of named arguments expected as %s function parameters"), functionclass);
	}
	Serialization::const_iterator
		si = parameters.value.serialization->begin(),
		se = parameters.value.serialization->end();
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
		throw_error_with_location( err.what(), errorhnd, si, se, parameters.value.serialization->begin());
	}
}

template <class FUNCTYPE>
static void deserializeQueryEvalFunctionResultNames(
	const char* functionclass, FUNCTYPE* function, const ValueVariant& resultnames)
{
	if (resultnames.defined())
	{
		if (resultnames.type != ValueVariant::StrusSerialization)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected as %s function result name definitions"), functionclass);
		}
		Serialization::const_iterator
			si = resultnames.value.serialization->begin(),
			se = resultnames.value.serialization->end();
		try
		{
			while (si != se)
			{
				ConfigDef resultdef( si, se);
				function->defineResultName(
						ValueVariantConv::tostring( resultdef.value),
						ValueVariantConv::tostring( resultdef.name));
			}
		}
		catch (const std::runtime_error& err)
		{
			throw_error_with_location( err.what(), errorhnd, si, se, resultnames.value.serialization->begin());
		}
	}
}

void Deserializer::buildSummarizerFunction(
		QueryEvalInterface* queryeval,
		const std::string& functionName,
		const ValueVariant& parameters,
		const ValueVariant& resultnames,
		const QueryProcessorInterface* queryproc,
		ErrorBufferInterface* errorhnd)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	std::vector<FeatureParameter> featureParameters;

	const SummarizerFunctionInterface* sf = queryproc->getSummarizerFunction( functionName);
	if (!sf) throw strus::runtime_error( _TXT("%s function not defined: '%s'"), "summarizer", functionName.c_str());

	Reference<SummarizerFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating %s function '%s': %s"), "summarizer", functionName.c_str(), errorhnd->fetchError());

	std::string debuginfoAttribute("debug");

	deserializeQueryEvalFunctionParameters(
		"summarizer", function.get(), debuginfoAttribute, featureParameters, parameters);

	deserializeQueryEvalFunctionResultNames(
		"summarizer", function.get(), resultnames);

	queryeval->addSummarizerFunction( functionName, function.get(), featureParameters, debuginfoAttribute);
	function.release();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to define %s function '%s': %s"), "summarizer", functionName.c_str(), errorhnd->fetchError());
	}
}

void Deserializer::buildWeightingFunction(
		QueryEvalInterface* queryeval,
		const std::string& functionName,
		const ValueVariant& parameters,
		const QueryProcessorInterface* queryproc,
		ErrorBufferInterface* errorhnd)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	std::vector<FeatureParameter> featureParameters;

	const WeightingFunctionInterface* sf = queryproc->getWeightingFunction( functionName);
	if (!sf) throw strus::runtime_error( _TXT("%s function not defined: '%s'"), "weighting", functionName.c_str());

	Reference<WeightingFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating %s function '%s': %s"), "weighting", functionName.c_str(), errorhnd->fetchError());

	std::string debuginfoAttribute("debug");
	deserializeQueryEvalFunctionParameters(
		"weighting", function.get(), debuginfoAttribute, featureParameters, parameters);

	queryeval->addWeightingFunction( functionName, function.get(), featureParameters, debuginfoAttribute);
	function.release();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to define %s function '%s': %s"), "weighting", functionName.c_str(), errorhnd->fetchError());
	}
}

void Deserializer::buildWeightingFormula(
		QueryEvalInterface* queryeval,
		const std::string& source,
		const ValueVariant& parameter,
		const QueryProcessorInterface* queryproc,
		ErrorBufferInterface* errorhnd)
{
	std::string parsername;
	typedef std::pair<std::string,double> ParamDef;
	std::vector<ParamDef> paramlist;
	if (parameter.defined())
	{
		if (parameter.type != ValueVariant::StrusSerialization)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected as %s function parameters"), "scalar");
		}
		Serialization::const_iterator
			si = parameter.value.serialization->begin(),
			se = parameter.value.serialization->end();
		try
		{
			while (si != se)
			{
				ConfigDef cdef( si, se);
				if (ValueVariantConv::isequal_ascii( cdef.name, "parser"))
				{
					parsername = ValueVariantConv::tostring( cdef.value);
				}
				else
				{
					paramlist.push_back( ParamDef(
						ValueVariantConv::tostring( cdef.name),
						ValueVariantConv::todouble( cdef.name)));
				}
			}
		}
		catch (const std::runtime_error& err)
		{
			throw_error_with_location( err.what(), errorhnd, si, se, parameter.value.serialization->begin());
		}
	}
	const ScalarFunctionParserInterface* scalarfuncparser = queryproc->getScalarFunctionParser( parsername);
	std::auto_ptr<ScalarFunctionInterface> scalarfunc( scalarfuncparser->createFunction( source, std::vector<std::string>()));
	if (!scalarfunc.get())
	{
		throw strus::runtime_error(_TXT( "failed to create %s function (%s) from source: %s"), "scalar", source.c_str(), errorhnd->fetchError());
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
		throw strus::runtime_error(_TXT("failed to define %s function (%s): %s"), "scalar", source.c_str(), errorhnd->fetchError());
	}
}


bool Deserializer::skipStructure( Serialization::const_iterator si, const Serialization::const_iterator& se)
{
	if (si == se) return false;
	if (si->tag == Serialization::Value)
	{
		++si;
		return true;
	}
	else if (si->tag == Serialization::Open)
	{
		int brkcnt = 1;
		++si;
		for (;si != se && brkcnt; ++si)
		{
			if (si->tag == Serialization::Open)
			{
				++brkcnt;
			}
			else if (si->tag == Serialization::Close)
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

static ExpressionType getExpressionType( Serialization::const_iterator si, const Serialization::const_iterator& se)
{
	static const StructureNameMap keywords( "type,value,len,from,to,variable,join,range,cardinality,arg", ',');
	static const ExpressionType keywordTypeMap[10] = {
		ExpressionTerm, ExpressionTerm, ExpressionTerm,			/*type,value,len*/
		ExpressionMetaDataRange,ExpressionMetaDataRange,		/*from,to*/
		ExpressionVariableAssignment,					/*variable*/
		ExpressionJoin,ExpressionJoin,ExpressionJoin,ExpressionJoin	/*join,range,cardinality,arg*/
	};
	if (si == se) return ExpressionUnknown;
	if (si->tag == Serialization::Value)
	{
		if (si->isStringType())
		{
			int pi = getStringPrefix( *si, "=@");
			switch (pi)
			{
				case 0: return ExpressionVariableAssignment;
				case 1: return ExpressionMetaDataRange;
				default: return ExpressionTerm;
			}
		}
		else
		{
			return ExpressionUnknown;
		}
	}
	else if (si->tag == Serialization::Open)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s"), "expression");

		if (si->tag == Serialization::Name)
		{
			do
			{
				int ki = keywords.index( *si);
				if (ki<0) return ExpressionUnknown;
				ExpressionType rt = keywordTypeMap[ ki];
				if (rt != ExpressionVariableAssignment) return rt;
				++si;
				if (!Deserializer::skipStructure( si, se)) return ExpressionUnknown;
			} while (si != se && si->tag == Serialization::Name);
			return ExpressionUnknown;
		}
		else
		{
			for (; si != se && si->tag == Serialization::Value; ++si)
			{
				int pi = getStringPrefix( *si, "=@");
				if (pi < 0) break;
				switch (pi)
				{
					case 0: ++si; break;
					case 1: return ExpressionMetaDataRange;
					default: break;
				}
			}
			int argc = 0;
			for (; si != se && si->tag == Serialization::Value && argc < 3; ++si,++argc)
			{}
			if (si == se || si->tag == Serialization::Close)
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
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	static const StructureNameMap joinop_namemap( "variable,join,range,cardinality,arg", ',');
	enum StructureNameId {JO_UNKNOWN=-1,JO_variable=0,JO_join=1, JO_range=2, JO_cardinality=3, JO_arg=4};

	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s"), "sub expression");

	if (si->tag == Serialization::Open)
	{
		std::string variable;
		std::string op;
		unsigned int argc = 0;
		int range = 0;
		unsigned int cardinality = 0;

		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s"), "sub expression");

		if (si->tag == Serialization::Name)
		{
			do
			{
				switch ((StructureNameId)joinop_namemap.index( *si++))
				{
					case JO_UNKNOWN: throw strus::runtime_error(_TXT("unknown tag name in %s"),"sub expression");
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
				}
			} while (si != se && si->tag == Serialization::Name);
			Deserializer::consumeClose( si, se);
		}
		else if (si->tag == Serialization::Value)
		{
			if (Deserializer::isStringWithPrefix( *si, '='))
			{
				variable = Deserializer::getPrefixStringValue( *si++, '=');
				if (si == se || si->tag != Serialization::Value)
				{
					throw strus::runtime_error(_TXT("expected variable assignment,term, metadata range or posting sub expression"));
				}
			}
			op = Deserializer::getString( si, se);
			if (si != se && si->tag == Serialization::Value)
			{
				ValueVariant val = *si++;
				if (ValueVariantConv::try_convertToNumber( val))
				{
					range = ValueVariantConv::toint( val);
					if (si != se && si->tag == Serialization::Value)
					{
						ValueVariant val = *si++;
						cardinality = ValueVariantConv::touint( val);
					}
				}
			}
			while (si != se && si->tag != Serialization::Close)
			{
				Deserializer::buildExpression( builder, si, se);
				++argc;
			}
			Deserializer::consumeClose( si, se);
		}
		else
		{
			throw strus::runtime_error(_TXT("unexpected element in %s"), "sub expression");
		}
		builder.pushExpression( op, argc, range, cardinality);
		if (!variable.empty())
		{
			builder.attachVariable( variable);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("unexpected element in %s"), "sub expression");
	}
}

void Deserializer::buildExpression(
		ExpressionBuilder& builder,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	ExpressionType etype = getExpressionType( si, se);
	switch (etype)
	{
		case ExpressionUnknown:
		{
			throw strus::runtime_error(_TXT("unable to interpret expression"));
		}
		case ExpressionTerm:
		{
			TermDef def( si, se);
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
		case ExpressionVariableAssignment:
		{
			throw strus::runtime_error(_TXT("isolated variable assignment in structure, must be at the beginning of an expression"));
		}
		case ExpressionMetaDataRange:
		{
			MetaDataRangeDef def( si, se);
			builder.pushDocField( def.from, def.to);
		}
		case ExpressionJoin:
		{
			buildExpressionJoin( builder, si, se);
		}
	};
}

void Deserializer::buildExpression(
		ExpressionBuilder& builder,
		const ValueVariant& expression,
		ErrorBufferInterface* errorhnd)
{
	if (expression.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), "expression");
	}
	else
	{
		Serialization::const_iterator
			si = expression.value.serialization->begin(),
			se = expression.value.serialization->end();
		try
		{
			return buildExpression( builder, si, se);
		}
		catch (const std::runtime_error& err)
		{
			throw_error_with_location( err.what(), errorhnd, si, se, expression.value.serialization->begin());
		}
	}
}

void Deserializer::buildPatterns(
		ExpressionBuilder& builder,
		const ValueVariant& patterns,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "name,pattern,visible", ',');
	if (patterns.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), "patterns");
	}
	Serialization::const_iterator
		si = patterns.value.serialization->begin(),
		se = patterns.value.serialization->end();
	try
	{
		while (si != se)
		{
			if (si->tag == Serialization::Open)
			{
				++si;
				if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s definition"), "pattern");
				std::string name;
				bool visible = true;
				if (si->tag == Serialization::Name)
				{
					do
					{
						int ki = namemap.index( *si++);
						if (ki<0) throw strus::runtime_error(_TXT("unknown element in %s definition"), "pattern");
						switch (ki)
						{
							case 0:
								name = ValueVariantConv::tostring( getValue( si, se));
								break;
							case 1:
								buildExpression( builder, si, se);
								break;
							case 2:
								visible = ValueVariantConv::tobool( getValue( si, se));
								break;
						}
					} while (si != se && si->tag == Serialization::Name);
				}
				else if (si->tag == Serialization::Value)
				{
					if (Deserializer::isStringWithPrefix( *si, '.'))
					{
						name = Deserializer::getPrefixStringValue( *si, '.');
						visible = false;
					}
					else
					{
						name = getString( si, se);
						visible = true;
					}
					buildExpression( builder, si, se);
				}
				else
				{
					throw strus::runtime_error(_TXT("error in %s definition structure"), "pattern");
				}
				Deserializer::consumeClose( si, se);
				builder.definePattern( name, visible);
			}
			else
			{
				throw strus::runtime_error(_TXT("%s definition expected to be a structure"), "pattern");
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		throw_error_with_location( err.what(), errorhnd, si, se, patterns.value.serialization->begin());
	}
}

static void buildInsertMetaData(
		StorageDocumentInterface* document,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), _TXT("document meta data"));
}

static void buildInsertAttributes(
		StorageDocumentInterface* document,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), _TXT("document attributes"));
}

static void buildInsertSearchIndex(
		StorageDocumentInterface* document,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), _TXT("document search index"));
}

static void buildInsertForwardIndex(
		StorageDocumentInterface* document,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), _TXT("document forward index"));
}

static void buildUserAccessRights(
		StorageDocumentInterface* document,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), _TXT("document user access rights"));
}


void Deserializer::buildInsertDocument(
		StorageDocumentInterface* document,
		const ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	static const StructureNameMap namemap( "doctype,attributes,metadata,searchindex,forwardindex,access", ',');
	if (content.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for %s"), "document");
	}
	Serialization::const_iterator
		si = patterns.value.serialization->begin(),
		se = patterns.value.serialization->end();
	try
	{
		while (si != se)
		{
			if (si->tag != Serialization::Name)
			{
				throw strus::runtime_error(_TXT("section name expected for top elements of %s structure"), "document");
			}
			switch (namemap.index( *si++))
			{
				case 0: break;
				case 1: document->setMetaData
					break;
				case 1: document->setMetaData
					break;
				default: throw strus::runtime_error(_TXT("unknown tag name in %s structure"), "document");
					break;
			}
			++si;
			
			if (si == se)
			if (si->tag == Serialization::Open)
			{
				++si;
				if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s structure"), "document");
				std::string name;
				bool visible = true;
				if (si->tag == Serialization::Name)
				{
					do
					{
						int ki = namemap.index( *si++);
						if (ki<0) throw strus::runtime_error(_TXT("unknown element in pattern definition"));
						switch (ki)
						{
							case 0:
								name = ValueVariantConv::tostring( getValue( si, se));
								break;
							case 1:
								buildExpression( builder, si, se);
								break;
							case 2:
								visible = ValueVariantConv::tobool( getValue( si, se));
								break;
						}
					} while (si != se && si->tag == Serialization::Name);
				}
				else if (si->tag == Serialization::Value)
				{
					if (Deserializer::isStringWithPrefix( *si, '.'))
					{
						name = Deserializer::getPrefixStringValue( *si, '.');
						visible = false;
					}
					else
					{
						name = getString( si, se);
						visible = true;
					}
					buildExpression( builder, si, se);
				}
				else
				{
					throw strus::runtime_error(_TXT("error in pattern definition structure"));
				}
				Deserializer::consumeClose( si, se);
				builder.definePattern( name, visible);
			}
			else
			{
				throw strus::runtime_error(_TXT("pattern definition expected to be a structure"));
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		throw_error_with_location( err.what(), errorhnd, si, se, content.value.serialization->begin());
	}
}

void Deserializer::buildUpdateDocument(
		StorageDocumentUpdateInterface* document,
		const ValueVariant& content,
		ErrorBufferInterface* errorhnd)
{
	
}


