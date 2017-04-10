/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "bindingUtils.hpp"
#include "valueVariantConv.hpp"
#include "internationalization.hpp"
#include "strus/bindings/serialization.hpp"
#include <string>

using namespace strus;
using namespace strus::bindings;

static const ValueVariant& getValueWithOptionalName(
		const char* ascii_name,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	if (si->tag == Serialization::Open && ValueVariantConv::isequal_ascii( *si, ascii_name))
	{
		++si;
		if (si == se || si->tag != Serialization::Value) throw strus::runtime_error(_TXT("value expected after open (optional name)"));
		const ValueVariant& rt = *si++;
		if (si == se || si->tag != Serialization::Close) throw strus::runtime_error(_TXT("close expeced after name"));
		++si;
		return rt;
	}
	else if (si->tag == Serialization::Value)
	{
		return *si++;
	}
	else
	{
		throw strus::runtime_error(_TXT("expected variant value value (optionally with name)"));
	}
}

std::string Deserializer::getStringWithOptionalName(
		const char* name,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
	std::string rt;
	if (si->tag == Serialization::Open && ValueVariantConv::isequal_ascii( *si, name))
	{
		++si;
		if (si == se || si->tag != Serialization::Value) throw strus::runtime_error(_TXT("value expected after open (optional name)"));
		rt = ValueVariantConv::tostring( *si++);
		if (si == se || si->tag != Serialization::Close) throw strus::runtime_error(_TXT("close expeced after name"));
		++si;
	}
	else if (si->tag == Serialization::Value)
	{
		rt = ValueVariantConv::tostring( *si++);
	}
	else
	{
		throw strus::runtime_error(_TXT("expected variant value value (optionally with name)"));
	}
	return rt;
}

std::string Deserializer::getString( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	if (si == se) return std::string();
	if (si->tag == Serialization::Value)
	{
		return ValueVariantConv::tostring( *si++);
	}
	else
	{
		throw strus::runtime_error( _TXT( "expected value in string serialization"));
	}
}

std::vector<std::string> Deserializer::getStringList( Serialization::const_iterator& si, const Serialization::const_iterator& se)
{
	std::vector<std::string> rt;
	if (si == se) return rt;
	if (si->tag == Serialization::Index)
	{
		do
		{
			++si;
			rt.push_back( getString( si, se));
		} while (si->tag == Serialization::Index);
	}
	else
	{
		rt.push_back( getString( si, se));
	}
	if (si != se)
	{
		if (si->tag == Serialization::Close)
		{
			++si;
		}
		else
		{
			throw strus::runtime_error( _TXT( "expected close or end after string list serialization"));
		}
	}
	return rt;
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
		if (si->tag == Serialization::Open)
		{
			if (ValueVariantConv::isequal_ascii( *si, "position"))
			{
				++si;
				if (si != se && si->tag == Serialization::Value
					&& setFeatureOption_position( rt, *si))
				{
					++si;
				}
				else
				{
					throw strus::runtime_error(_TXT("expected position bind option"));
				}
			}
			if (si != se && si->tag == Serialization::Close)
			{
				++si;
			}
			else
			{
				throw strus::runtime_error(_TXT("expected close tag after position bind option"));
			}
		}
		else
		{
			if (si != se && si->tag == Serialization::Value
				&& setFeatureOption_position( rt, *si))
			{
				++si;
			}
			else
			{
				throw strus::runtime_error(_TXT("expected feature option (position bind value)"));
			}
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

	std::string name = Deserializer::getStringWithOptionalName( "name", si, se);
	std::vector<std::string> args;
	if (si != se && si->tag == Serialization::Open)
	{
		if (si->length == 0
		||  ValueVariantConv::isequal_ascii( *si, "arg"))
		{
			++si;
			args = Deserializer::getStringList( si, se);
		}
		else
		{
			throw strus::runtime_error( _TXT("expected list of normalizer arguments as only sub structure of a normalizer"));
		}
	}
	return getNormalizer_( name, args, textproc, errorhnd);
}

Reference<TokenizerFunctionInstanceInterface> Deserializer::getTokenizer(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<TokenizerFunctionInstanceInterface>();

	std::string name = Deserializer::getStringWithOptionalName( "name", si, se);
	std::vector<std::string> args;
	if (si != se && si->tag == Serialization::Open)
	{
		if (si->length == 0
		||  ValueVariantConv::isequal_ascii( *si, "arg"))
		{
			++si;
			args = getStringList( si, se);
		}
		else
		{
			throw strus::runtime_error( _TXT("expected list of tokenizer arguments as only sub structure of a normalizer"));
		}
	}
	return getTokenizer_( name, args, textproc, errorhnd);
}

Reference<AggregatorFunctionInstanceInterface> Deserializer::getAggregator(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) return Reference<AggregatorFunctionInstanceInterface>();

	std::string name = Deserializer::getStringWithOptionalName( "name", si, se);
	std::vector<std::string> args;
	if (si != se && si->tag == Serialization::Open)
	{
		if (si->length == 0
		||  ValueVariantConv::isequal_ascii( *si, "arg"))
		{
			++si;
			args = getStringList( si, se);
		}
		else
		{
			throw strus::runtime_error( _TXT("expected list of aggregator arguments as only sub structure of a normalizer"));
		}
	}
	return getAggregator_( name, args, textproc, errorhnd);
}

std::vector<Reference<NormalizerFunctionInstanceInterface> > Deserializer::getNormalizers(
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		const TextProcessorInterface* textproc,
		ErrorBufferInterface* errorhnd)
{
	std::vector< Reference<NormalizerFunctionInstanceInterface> > rt;
	if (si == se) return rt;

	if (si->tag == Serialization::Index)
	{
		do
		{
			++si;
			rt.push_back( getNormalizer_( si, se, textproc, errorhnd));
		} while (si != se && si->tag == Serialization::Index);
		if (si != se)
		{
			if (si->tag == Serialization::Close)
			{
				++si;
			}
			else
			{
				throw strus::runtime_error( _TXT("expected close tag after normalizer structure list"));
			}
		}
	}
	else
	{
		rt.push_back( getNormalizer_( si, se, textproc, errorhnd));
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

void Deserializer::buildPatternLexems(
		PreProcPatternExpressionBuilder& builder,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		ErrorBufferInterface* errorhnd)
{
	
}

void Deserializer::buildPatternLexems(
		PreProcPatternExpressionBuilder& builder,
		const ValueVariant& regexdefs,
		ErrorBufferInterface* errorhnd)
{
	
}

static bool isMetadataRangeValue( const ValueVariant& val)
{
	if (val.type == ValueVariant::String)
	{
		if (val.length > 0 && val.value.string[0] == '@') return true;
	}
	else if (val.type == ValueVariant::WString)
	{
		if (val.length > 0 && val.value.wstring[0] == (unsigned char)'@') return true;
	}
	return false;
}

static bool isTermExpression( Serialization::const_iterator si, const Serialization::const_iterator& se)
{
	if (si == se) return false;
	if (si->tag == Serialization::Value && !isMetadataRangeValue( *si))
	{
		return true;
	}
	if (si->tag == Serialization::Open)
	{
		return (ValueVariantConv::isequal_ascii( *si, "type")
			|| ValueVariantConv::isequal_ascii( *si, "value")
			||  ValueVariantConv::isequal_ascii( *si, "len"));
	}
	if (si->tag != Serialization::Index) return false;
	++si;
	if (si == se) return false;
	if (si->tag == Serialization::Value && !isMetadataRangeValue( *si))
	{
		++si;
		if (si == se || si->tag == Serialization::Close) return true;
		if (si->tag != Serialization::Index) return false;
		++si;
		if (si->tag == Serialization::Value)
		{
			++si;
			if (si == se || si->tag == Serialization::Close) return true;
			if (si->tag != Serialization::Index) return false;
			if (si->tag == Serialization::Value)
			{
				++si;
				if (si == se || si->tag == Serialization::Close) return true;
			}
		}
	}
	return false;
}

static bool isMetadataRangeExpression( Serialization::const_iterator si, const Serialization::const_iterator& se)
{
	if (si == se) return false;
	if (si->tag == Serialization::Open)
	{
		return (ValueVariantConv::isequal_ascii( *si, "from")
			|| ValueVariantConv::isequal_ascii( *si, "to"));
	}
	if (si->tag == Serialization::Index)
	{
		++si;
	}
	if (si != se && si->tag == Serialization::Value)
	{
		if (si->type == ValueVariant::String)
		{
			if (si->length > 0 && si->value.string[0] == '@') return true;
		}
		else if (si->type == ValueVariant::WString)
		{
			if (si->length > 0 && si->value.wstring[0] == (unsigned char)'@') return true;
		}
	}
	return false;
}

static void fillStructure(
		ValueVariant* star,
		const char** ascii_names,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se)
{
}

static void buildExpression_term(
		ExpressionBuilder& builder,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		ErrorBufferInterface* errorhnd)
{
	std::size_t argc;
	ValueVariant argv[3];
	if (si == se) throw strus::runtime_error(_TXT("error parsing term expression"));
	if (si->tag == Serialization::Value)
	{
		argc = 1;
		argv[0] = *si;
	}
	else
	{
		for (argc=0; si != se && si->tag == Serialization::Index && argc < 3; ++argc)
		{
			if (++si == se) throw strus::runtime_error("unexpected end of expression");
			if (si->tag == Serialization::Value)
			{
				argv[argc] = *si;
			}
			else
			{
				throw strus::runtime_error(_TXT("error parsing term expression"));
			}
			++si;
		}
		if (si == se || si->tag == Serialization::Close)
		{
			if (argc == 1)
			{
				builder.pushTerm(
					ValueVariantConv::tostring(argv[0]),
					std::string());
			}
			else if (argc == 2)
			{
				builder.pushTerm(
					ValueVariantConv::tostring(argv[0]),
					ValueVariantConv::tostring(argv[1]));
			}
			else if (argc == 3)
			{
				builder.pushTerm(
					ValueVariantConv::tostring(argv[0]),
					ValueVariantConv::tostring(argv[1]),
					ValueVariantConv::touint(argv[2]));
			}
		}
		else
		{
			throw strus::runtime_error(_TXT("unexpected tokens at end of term expression"));
		}
	}
}

void Deserializer::buildExpression(
		ExpressionBuilder& builder,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		ErrorBufferInterface* errorhnd)
{
	if (si == se) throw strus::runtime_error("unexpected end of expression");
	if (si->tag == Serialization::Value)
	{
		builder.pushTerm( ValueVariantConv::tostring(*si++),std::string());
		return;
	}
	else if (si->tag == Serialization::Index)
	{
		if (++si == se) throw strus::runtime_error("unexpected end of expression");
		if (si->tag == Serialization::Value)
		{
			const ValueVariant& name = *si++;
			if (si == se)
			{
				builder.pushTerm(
					ValueVariantConv::tostring(name),
					std::string());
				return;
			}
			if (si->tag == Serialization::Close)
			{
				builder.pushTerm(
					ValueVariantConv::tostring(name),
					std::string());
				++si;
				return;
			}
			if (si->tag != Serialization::Index) throw strus::runtime_error("unexpected element in expression (index expected)");
			if (++si == se) throw strus::runtime_error("unexpected end of expression");
			if (si->tag == Serialization::Open)
			{
				
			}
			else if (si->tag == Serialization::Value)
			{
				const ValueVariant& value = *si++;
				if (si == se)
				{
					builder.pushTerm( 
						ValueVariantConv::tostring(name),
						ValueVariantConv::tostring(value));
					return;
				}
				if (si->tag == Serialization::Close)
				{
					builder.pushTerm( 
						ValueVariantConv::tostring(name),
						ValueVariantConv::tostring(value));
					++si;
					return;
				}
				if (si->tag != Serialization::Index) throw strus::runtime_error("unexpected element in expression (index expected)");
				if (++si == se) throw strus::runtime_error("unexpected end of expression");
				if (si->tag == Serialization::Value)
				{
					ValueVariant length = *si;
					if (ValueVariantConv::try_convertToNumber( length))
					{
						++si;
						builder.pushTerm( 
							ValueVariantConv::tostring(name),
							ValueVariantConv::tostring(value),
							ValueVariantConv::touint(value));
					}
					else
					{
						builder.pushTerm( 
							ValueVariantConv::tostring(name),
							ValueVariantConv::tostring(value));
					}
				}
			}
			if (si->tag != Serialization::Index) throw strus::runtime_error("unexpected element in expression (index expected)");
		}
	}
	else if (ValueVariantConv::isequal_ascii( *si, "name"))
	{
		
	}
	return;
}

void Deserializer::buildExpression(
		ExpressionBuilder& builder,
		const ValueVariant& expression,
		ErrorBufferInterface* errorhnd)
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
		return buildExpression( builder, si, se, errorhnd);
	}
}

void Deserializer::buildFeatures(
		ExpressionBuilder& builder,
		Serialization::const_iterator& si,
		const Serialization::const_iterator& se,
		ErrorBufferInterface* errorhnd)
{
	
}

void Deserializer::buildFeatures(
		ExpressionBuilder& builder,
		const ValueVariant& expressions,
		ErrorBufferInterface* errorhnd)
{
}

