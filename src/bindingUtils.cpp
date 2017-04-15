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

analyzer::DocumentClass Deserializer::getDocumentClass(
		const ValueVariant& val)
{
	analyzer::DocumentClass rt;
	if (val.type != ValueVariant::StrusSerialization)
	{
		rt.setMimeType( ValueVariantConv::tostring( val));
		return rt;
	}
	Serialization::const_iterator
		si = val.value.serialization->begin(),  
		se = val.value.serialization->end();

	static const StructureNameMap namemap( "mimetype,encoding,scheme", ',');
	if (si == se) return rt;

	if (si->tag == Serialization::Name)
	{
		do
		{
			switch (namemap.index( *si++))
			{
				case -1: throw strus::runtime_error("unknown tag name in document class structure");
				case 0: rt.setMimeType( Deserializer::getString( si, se));
					break;
				case 1: rt.setEncoding( Deserializer::getString( si, se));
					break;
				case 2: rt.setScheme( Deserializer::getString( si, se));
					break;
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


