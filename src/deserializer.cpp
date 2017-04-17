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
		throw strus::runtime_error(_TXT("expected value in serialization"));
	}
}

std::pair<ValueVariant,ValueVariant> Deserializer::getValueWithOptionalName(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of structure"));
	if (si->tag == Serialization::Open)
	{
		if (++si == se) throw strus::runtime_error(_TXT("unexpected end of structure"));
		ValueVariant name;
		ValueVariant value;
		if (si->tag == Serialization::Name)
		{
			name = *si++;
		}
		if (++si == se) throw strus::runtime_error(_TXT("unexpected end of structure"));
		if (si->tag == Serialization::Value)
		{
			value = *si++;
		}
		else
		{
			throw strus::runtime_error(_TXT("atomic value expected as value in optional name value pair"));
		}
		if (si == se || si->tag != Serialization::Close) throw strus::runtime_error(_TXT("close expected after value in optional name"));
		++si;
		return std::pair<ValueVariant,ValueVariant>( name, value);
	}
	else if (si->tag == Serialization::Value)
	{
		return std::pair<ValueVariant,ValueVariant>( ValueVariant(), *si++);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected value in serialization"));
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
			throw strus::runtime_error("unexpected element at end of structure (close expected)");
		}
		++si;
	}
}

std::vector<std::string> Deserializer::getStringList( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	std::vector<std::string> rt;
	if (si == se) return rt;
	if (si->tag == Serialization::Open)
	{
		++si;
		while (si != se && si->tag != Serialization::Close)
		{
			rt.push_back( getString( si, se));
		}
		if (si != se)
		{
			++si;
		}
	}
	else
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
		throw strus::runtime_error(_TXT("atomic value (df) or list of named arguments expected for term statistics"));
	}
	Serialization::const_iterator
		si = val.value.serialization->begin(),  
		se = val.value.serialization->end();
	while (si != se)
	{
		if (si->tag != Serialization::Name)
		{
			throw strus::runtime_error(_TXT("list of named arguments expected for term statistics as structure"));
		}
		switch (namemap.index( *si++))
		{
			case 0: rt.setDocumentFrequency( ValueVariantConv::touint64( stats));
			default: throw strus::runtime_error("unknown tag name in term statistics structure");
				break;
		}
	}
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
				default: throw strus::runtime_error("unknown tag name in document class structure");
			}
		} while (si != se && si->tag == Serialization::Name);
		Deserializer::consumeClose( si, se);
	}
	else if (si->tag == Serialization::Value)
	{
		if (si == se) throw strus::runtime_error("unexpected end of serialization: document class definition expected");

		if (si->tag == Serialization::Value)
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
		}
		else
		{
			throw strus::runtime_error(_TXT("document class definition expected"));
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		throw strus::runtime_error(_TXT("document class definition expected"));
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
	AnalyzerFunctionDef def;
	def.deserialize( si, se);
	return getNormalizer_( def.name, def.args, textproc, errorhnd);
}

Reference<TokenizerFunctionInstanceInterface> Deserializer::getTokenizer(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<TokenizerFunctionInstanceInterface>();

	AnalyzerFunctionDef def;
	def.deserialize( si, se);
	return getTokenizer_( def.name, def.args, textproc, errorhnd);
}

Reference<AggregatorFunctionInstanceInterface> Deserializer::getAggregator(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<AggregatorFunctionInstanceInterface>();

	AnalyzerFunctionDef def;
	def.deserialize( si, se);
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
			throw strus::runtime_error(_TXT("unexpected element in normalizer list structure"));
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
		return getNormalizers( si, se, textproc, errorhnd);
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
		return getTokenizer( si, se, textproc, errorhnd);
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
		return getAggregator( si, se, textproc, errorhnd);
	}
}

struct FeatureParameterPositional
{
	std::string name;
	ValueVariant value;
	bool isFeatureSet;
	std::string featureSet;

	FeatureParameterPositional( Serialization::const_iterator& si, const Serialization::const_iterator& se)
		:name(),value(),isFeatureSet(false),featureSet()
	{
		if (si == se || si->tag != Serialization::Name) throw strus::runtime_error(_TXT("argument name expected as as start of a summarizer function parameter"));
		std::string paramname = ValueVariantConv::tostring( *si++);

		std::pair<ValueVariant,ValueVariant> paramdecl = Deserializer::getValueWithOptionalName( si, se);
		if (paramdecl.first.defined())
		{
			if (ValueVariantConv::isequal_ascii( paramdecl.first, "feature"))
			{
				featureSet = ValueVariantConv::tostring( paramdecl.second);
				isFeatureSet = true;
			}
			else if (ValueVariantConv::isequal_ascii( paramdecl.first, "value"))
			{
				value = paramdecl.second;
			}
			else
			{
				std::string typname = ValueVariantConv::tostring( paramdecl.first);
				throw strus::runtime_error(_TXT("unexpected parameter type name '%s', expected 'feature' or 'value'"), typname.c_str());
			}
		}
		else
		{
			if (paramdecl.second.isStringType() && Deserializer::isStringWithPrefix( paramdecl.second, '.'))
			{
				featureSet = Deserializer::getPrefixStringValue( paramdecl.second, '.');
				isFeatureSet = true;
			}
			else
			{
				value = paramdecl.second;
			}
		}
	}
};

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
		std::string& debuginfoAttribute,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
		const std::string& paramname,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s function parameter definition"), functionclass);
	if (si->tag == Serialization::Open)
	{
		++si;
		while (si != se && si->tag != Serialization::Close)
		{
			ValueVariant value = getValue( si, se);
			if (value.isStringType() && Deserializer::isStringWithPrefix( value, '.'))
			{
				std::string featureSet = Deserializer::getPrefixStringValue( paramdef.value, '.');
				featureParameters.push_back( FeatureParameter( paramname, featureSet));
			}
			else
			{
				instantiateQueryEvalFunctionParameter( functionclass, function, paramname, value);
			}
		}
		Deserializer::consumeClose( si, se);
	}
	else
	{
		ValueVariant value = getValue( si, se);
		if (value.isStringType() && Deserializer::isStringWithPrefix( value, '.'))
		{
			std::string featureSet = Deserializer::getPrefixStringValue( paramdef.value, '.');
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
		const ValueVariant& parameters)
{
	static const StructureNameMap namemap( "name,value,feature", ',');
	if (si->tag == Serialization::Open)
	{
		++si;
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of %s function parameter definition"), functionclass);
		if (si->tag == Serialization::Value)
		{
			std::string paramname = Deserializer::getString( si, se);
			deserializeQueryEvalFunctionParameterValue(
				functionclass, function, debuginfoAttribute, featureParameters,
				paramname, si, se);
			Deserializer::consumeClose( si, se);
		}
		else
		{
			std::string name;
			unsigned char name_defined = 0;
			ValueVariant value;
			while (si->tag == Serialization::Name)
			{
				switch (namemap.index( *si++))
				{
					case 0: if (name_defined++) throw strus::runtime_error(_TXT("duplicate definition of '%s'"), "name");
						name = Deserializer::getString( si, se);
						break;
					case 1: if (type != Undefined) throw strus::runtime_error(_TXT("contradicting definitions in query evaluation function parameter: only one allowed of 'value', 'result', or 'feature'"));
						type = Value; value = getValue( si, se);
						break;
					case 2:	if (type != Undefined) throw strus::runtime_error(_TXT("contradicting definitions in query evaluation function parameter: only one allowed of 'value', 'result', or 'feature'"));
						type = Feature; value = getValue( si, se);
						break;
					default: throw strus::runtime_error(_TXT("unknown tag name in query evaluation function config"));
				}
			}
			Deserializer::consumeClose( si, se);
			if (!name_defined || type == Undefined)
			{
				throw strus::runtime_error(_TXT("incomplete query evaluation function definition"));
			}
			instantiateQueryEvalFunctionParameter( functionclass, function, name, value);
		}
	}
	else if (si->tag == Serialization::Name)
	{
		name = Deserializer::getString( si, se);
		if (si == se) throw strus::runtime_error(_TXT("unexpected end of query evaluation function parameter definition"));

		std::pair<ValueVariant,ValueVariant> paramdecl = Deserializer::getValueWithOptionalName( si, se);
		if (paramdecl.first.defined())
		{
			if (ValueVariantConv::isequal_ascii( paramdecl.first, "feature"))
			{
				value = paramdecl.second;
				type = Feature;
			}
			else if (ValueVariantConv::isequal_ascii( paramdecl.first, "value"))
			{
				value = paramdecl.second;
				type = Value;
			}
			else
			{
				std::string typname = ValueVariantConv::tostring( paramdecl.first);
				throw strus::runtime_error(_TXT("unexpected parameter type name '%s', expected 'feature' or 'value'"), typname.c_str());
			}
		}
		else
		{
			type = Undefined;
			value = paramdecl.second;
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("argument name expected as as start of a query evaluation function parameter"));
	}
}

template <class FUNCTYPE>
static void instantiateQueryEvalFunctionParameters(
		const char* functionclass,
		FUNCTYPE* function,
		std::string& debuginfoAttribute,
		std::vector<QueryEvalInterface::FeatureParameter>& featureParameters,
		const ValueVariant& parameters)
{
	typedef QueryEvalInterface::FeatureParameter FeatureParameter;
	if (parameters.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("list of named arguments expected as %s function parameters"), functionclass);
	}
	Serialization::const_iterator
		si = parameters.value.serialization->begin(),  
		se = parameters.value.serialization->end();
	while (si != se && si->tag != Serialization::Close)
	{
		QueryEvalFunctionParameterDef paramdef;
		paramdef.deserialize( si, se);
		switch (paramdef.type)
		{
			case QueryEvalFunctionParameterDef::Undefined:
			{
				if (paramdef.name == "debug")
				{
					debuginfoAttribute = ValueVariantConv::tostring( paramdef.value);
				}
				else if (paramdef.value.isStringType() && Deserializer::isStringWithPrefix( paramdef.value, '.'))
				{
					std::string featureSet = Deserializer::getPrefixStringValue( paramdef.value, '.');
					featureParameters.push_back( FeatureParameter( paramdef.name, featureSet));
				}
				else
				{
					instantiateQueryEvalFunctionParameter( functionclass, function, paramdef.name, paramdef.value);
				}
				break;
			}
			case QueryEvalFunctionParameterDef::Feature:
			{
				std::string featureSet = ValueVariantConv::tostring( paramdef.value);
				featureParameters.push_back( FeatureParameter( paramdef.name, featureSet));
				break;
			}
			case QueryEvalFunctionParameterDef::Value:
			{
				if (paramdef.name == "debug")
				{
					debuginfoAttribute = ValueVariantConv::tostring( paramdef.value);
				}
				else
				{
					instantiateQueryEvalFunctionParameter( functionclass, function, paramdef.name, paramdef.value);
				}
				break;
			 }
		}
	}
}

template <class FUNCTYPE>
static void instantiateQueryEvalFunctionResultNames(
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
		while (si != se)
		{
			ConfigDef resultdef;
			resultdef.deserialize( si, se);
			function->defineResultName(
					ValueVariantConv::tostring( resultdef.value),
					ValueVariantConv::tostring( resultdef.name));
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

	instantiateQueryEvalFunctionParameters(
		"summarizer", function.get(), debuginfoAttribute, featureParameters, parameters);

	instantiateQueryEvalFunctionResultNames(
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
	instantiateQueryEvalFunctionParameters(
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
		while (si != se)
		{
			ConfigDef cdef;
			cdef.deserialize( si, se);
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
		if (si == se) throw strus::runtime_error("unexpected end of expression");

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

	if (si == se) throw strus::runtime_error(_TXT("unexpected end of sub expression"));

	if (si->tag == Serialization::Open)
	{
		std::string variable;
		std::string op;
		unsigned int argc = 0;
		int range = 0;
		unsigned int cardinality = 0;

		if (si == se) throw strus::runtime_error("unexpected end of sub expression: term expression tree expected");

		if (si->tag == Serialization::Name)
		{
			do
			{
				switch ((StructureNameId)joinop_namemap.index( *si++))
				{
					case JO_UNKNOWN: throw strus::runtime_error("unknown tag name in sub expression");
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
			throw strus::runtime_error(_TXT("unexpected element in sub expression"));
		}
		builder.pushExpression( op, argc, range, cardinality);
		if (!variable.empty())
		{
			builder.attachVariable( variable);
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("unexpected element in sub expression"));
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
			TermDef def;
			def.deserialize( si, se);
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
			MetaDataRangeDef def;
			def.deserialize( si, se);
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
		const ValueVariant& expression)
{
	if (expression.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for expression"));
	}
	else
	{
		Serialization::const_iterator
			si = expression.value.serialization->begin(),  
			se = expression.value.serialization->end();
		return buildExpression( builder, si, se);
	}
}

void Deserializer::buildPatterns(
		ExpressionBuilder& builder,
		const ValueVariant& patterns)
{
	static const StructureNameMap namemap( "name,pattern,visible", ',');
	if (patterns.type != ValueVariant::StrusSerialization)
	{
		throw strus::runtime_error(_TXT("serialized structure expected for patterns"));
	}
	Serialization::const_iterator
		si = patterns.value.serialization->begin(),  
		se = patterns.value.serialization->end();

	if (si->tag == Serialization::Open)
	{
		++si;
		if (si == se) throw strus::runtime_error("unexpected end of pattern");
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


