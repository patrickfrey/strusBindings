/*
 * Copyright (c) 2016 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Parser for Strus interfaces
/// \file interfaceParser.cpp
#include "interfaceParser.hpp"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace strus;

static bool endsWith( const std::string& ident, const std::string& tail)
{
	if (tail.size() > ident.size()) return false;
	std::size_t tailidx = ident.size() - tail.size();
	return 0==std::strcmp( ident.c_str()+tailidx, tail.c_str());
}
static bool isSpace( char ch)
{
	return ((unsigned char)ch <= 32);
}
static void skipSpaces( char const*& si, const char* se)
{
	while (si != se && isSpace(*si)) ++si;
}
static void skipToEoln( char const*& si, const char* se)
{
	while (si != se && *si != '\n') ++si;
}
static void skipToStr( char const*& si, const char* se, const char* str)
{
	std::size_t strsize = std::strlen(str);
	while (si != se)
	{
		if (si + strsize < se && *si == str[0])
		{
			if (0==std::memcmp( si, str, strsize))
			{
				return;
			}
		}
		else if (*si == '\\' && si+1 < se)
		{
			++si;
		}
		++si;
	}
}
static void skipSpacesAndComments( char const*& si, const char* se)
{
	while (si != se)
	{
		skipSpaces( si, se);
		if (si + 1 < se && si[0] == '/' && si[1] == '/')
		{
			skipToEoln( si, se);
		}
		else if (si + 1 < se && si[0] == '/' && si[1] == '*')
		{
			skipToStr( si, se, "*/");
			if (si != se) si += 2;
		}
		else
		{
			break;
		}
	}
}
static bool isAlpha( char ch)
{
	if (ch == '_') return true;
	if ((ch|32) >= 'a' && (ch|32) <= 'z') return true;
	return false;
}
static bool isDigit( char ch)
{
	return (ch >= '0' && ch <= '9');
}
static bool isAlnum( char ch)
{
	return isAlpha(ch) || isDigit(ch);
}
static std::string parseIdentifier( char const*& si, const char* se)
{
	std::string rt;
	for (; si != se && isAlnum(*si); ++si)
	{
		rt.push_back( *si);
	}
	return rt;
}
static std::string parseStructureIdentifier( char const*& si, const char* se)
{
	std::string rt;
	while (si < se && isAlpha(*si))
	{
		rt.append( parseIdentifier( si, se));
		skipSpacesAndComments( si, se);
		if (si+1 < se && si[0] == ':' && si[1] == ':')
		{
			rt.append( "::");
			si += 2;
			skipSpacesAndComments( si, se);
		}
		else if (si+1 < se && si[0] == '-' && si[1] == '>')
		{
			rt.append( "->");
			si += 2;
			skipSpacesAndComments( si, se);
		}
		else if (si != se && *si == '.')
		{
			rt.append( ".");
			++si;
			skipSpacesAndComments( si, se);
		}
		else
		{
			break;
		}
	}
	return rt;
}
static std::string parseNumericValue( char const*& si, const char* se)
{
	std::string rt;
	if (si != se && (*si == '-' || *si == '+'))
	{
		rt.push_back( *si++);
	}
	for (; si != se && isDigit(*si); ++si)
	{
		rt.push_back( *si);
	}
	if (si != se && *si == '.')
	{
		rt.push_back( *si++);
	}
	for (; si != se && isDigit(*si); ++si)
	{
		rt.push_back( *si);
	}
	return rt;
}
static bool isOperator( char ch)
{
	static const char* ar = "<>()*&::,";
	return std::strchr( ar, ch) != 0;
}
static void skipBrackets( char const*& si, const char* se, char sb, char eb)
{
	if (si == se || *si != sb) throw std::runtime_error("internal: illegal call of skipBrackets");
	int cnt = 1;
	++si;
	skipSpacesAndComments( si, se);

	while (si != se)
	{
		if (*si == sb)
		{
			++cnt;
		}
		else if (*si == eb)
		{
			--cnt;
			if (cnt == 0)
			{
				++si;
				return;
			}
		}
		++si;
	}
	throw std::runtime_error("unbalanced brackets in source");
}
static void skipStructure( char const*& si, const char* se)
{
	skipSpacesAndComments( si, se);
	while (si != se && isAlpha(*si))
	{
		parseIdentifier( si, se);
		skipSpacesAndComments( si, se);
	}
	if (si == se)
	{
		throw std::runtime_error( "unexpected end of source in structure");
	}
	if (*si == '(')
	{
		skipBrackets( si, se, '(', ')');
		skipSpacesAndComments( si, se);
	}
	if (*si == '{')
	{
		skipBrackets( si, se, '{', '}');
		skipSpacesAndComments( si, se);
	}
	if (*si == ';')
	{
		++si;
	}
}


enum TokenType
{
	TokenOperator,
	TokenIdentifier,
	TokenVariable,
	TokenSuffix
};
static const char* tokenTypeName( TokenType i)
{
	static const char* ar[] = {"Operator","Identifier","Variable","Suffix"};
	return ar[i];
}

class TokenDef
{
public:
	TokenDef( const TokenDef& o)
		:m_type(o.m_type),m_value(o.m_value){}
	TokenDef( const TokenType& t, const std::string& v)
		:m_type(t),m_value(v){}

	const TokenType& type() const		{return m_type;}
	const std::string& value() const	{return m_value;}

private:
	TokenType m_type;
	std::string m_value;
};


enum OutputType
{
	OutputString,
	OuputVariable
};
static const char* outputTypeName( OutputType i)
{
	static const char* ar[] = {"String","Variable"};
	return ar[i];
}

class OutputDef
{
public:
	OutputDef()
		:m_chunks(){}
	OutputDef( const OutputDef& o)
		:m_chunks(o.m_chunks){}
	OutputDef( const std::string& src)
	{
		char const* oi = src.c_str();
		const char* oe = oi + src.size();
		std::string value;
		while (oi != oe)
		{
			if (*oi == '$')
			{
				if (!value.empty())
				{
					m_chunks.push_back( Chunk( OutputString, value));
					value.clear();
				}
				++oi;
				if (oi == oe || !isAlpha(*oi))
				{
					throw std::runtime_error("identifier expected after '$' in output definition");
				}
				m_chunks.push_back( Chunk( OuputVariable, parseIdentifier( oi, oe)));
				if (oi != oe && *oi == '~')
				{
					++oi;
				}
			}
			else
			{
				value.push_back(*oi);
				++oi;
			}
		}
		if (!value.empty())
		{
			m_chunks.push_back( Chunk( OutputString, value));
			value.clear();
		}
	}

	void print( 
			std::string &out,
			const std::map<std::string,std::string>& defmap,
			const std::string& name,
			const std::string& value) const
	{
		std::vector<Chunk>::const_iterator ci = m_chunks.begin(), ce = m_chunks.end();
		for (; ci != ce; ++ci)
		{
			switch (ci->m_type)
			{
				case OutputString:
					out.append( ci->m_value);
					break;
				case OuputVariable:
					if (ci->m_value == "name" && !name.empty())
					{
						out.append( name);
					}
					else if (ci->m_value == "value" && !value.empty())
					{
						out.append( value);
					}
					else
					{
						std::map<std::string,std::string>::const_iterator
							di = defmap.find( ci->m_value);
						if (di != defmap.end())
						{
							out.append( di->second);
						}
						else
						{
							throw std::runtime_error( std::string("variable '") + ci->m_value + "' in output is not defined");
						}
					}
					break;
			}
		}
	}

	std::string tostring() const
	{
		std::ostringstream out;
		std::vector<Chunk>::const_iterator ci = m_chunks.begin(), ce = m_chunks.end();
		for (int cidx=0; ci != ce; ++ci,++cidx)
		{
			if (cidx) out << ", ";
			out << outputTypeName(ci->m_type) << " '" << ci->m_value << "'";
		}
		return out.str();
	}

private:
	struct Chunk
	{
		OutputType m_type;
		std::string m_value;

		Chunk( const Chunk& o)
			:m_type(o.m_type),m_value(o.m_value){}
		Chunk( const OutputType& type_, const std::string& value_)
			:m_type(type_),m_value(value_){}
	};
	std::vector<Chunk> m_chunks;
};


class VariableType::Impl
{
public:
	Impl( const Impl& o)
		:m_source(o.m_source),m_scope_class(o.m_scope_class),m_scope_method(o.m_scope_method),m_tokendefs(o.m_tokendefs),m_output(o.m_output){}

	explicit Impl( const std::string& pattern, const std::string& scope_class_, const std::string& scope_method_)
		:m_source(pattern),m_scope_class(scope_class_),m_scope_method(scope_method_)
	{
		char const* si = pattern.c_str();
		const char* se = si + pattern.size();
		skipSpaces( si, se);

		while (si != se)
		{
			if (isAlpha(*si))
			{
				m_tokendefs.push_back( TokenDef( TokenIdentifier, parseIdentifier( si, se)));
			}
			else if (isOperator(*si))
			{
				m_tokendefs.push_back( TokenDef( TokenOperator, std::string( si++, 1)));
			}
			else if (*si == '$')
			{
				++si;
				if (si == se || !isAlpha(*si))
				{
					throw std::runtime_error("identifier expeced in pattern after '$'");
				}
				m_tokendefs.push_back( TokenDef( TokenVariable, parseIdentifier( si, se)));
				if (*si == '~')
				{
					++si;
					if (si == se || !isAlpha(*si))
					{
						throw std::runtime_error("identifier expeced in pattern after '~' (suffix definition)");
					}
					m_tokendefs.push_back( TokenDef( TokenSuffix, parseIdentifier( si, se)));
				}
			}
			else
			{
				throw std::runtime_error( std::string("unknown token in pattern at ") + "'" + si + "'");
			}
			skipSpaces( si, se);
		}
		defineOutput( "scopedtype", pattern.c_str());
		defineOutput( "type", pattern.c_str());
	}

	bool parse( std::map<std::string,std::string>& result_defmap, char const*& src, const char* end) const
	{
		std::map<std::string,std::string> defmap;
		char const* si = src;
		const char* se = end;
		std::vector<TokenDef>::const_iterator
			ti = m_tokendefs.begin(),
			te = m_tokendefs.end();
		for (; ti != te; ++ti)
		{
			skipSpaces( si, se);
			if (si == se) return false;
			switch (ti->type())
			{
				case TokenOperator:
				{
					std::string::const_iterator
						vi = ti->value().begin(), ve = ti->value().end();
					for (; vi != ve && si != se; ++vi,++si)
					{
						if (*si != *vi) return false;
					}
					if (vi != ve) return false;
					break;
				}
				case TokenIdentifier:
				{
					std::string::const_iterator
						vi = ti->value().begin(), ve = ti->value().end();
					for (; vi != ve && si != se; ++vi,++si)
					{
						if (*si != *vi) return false;
					}
					if (vi != ve) return false;
					if (si != se && isAlnum(*si)) return false;
					break;
				}
				case TokenVariable:
				{
					if (!isAlpha(*si)) return false;
					std::string variableid = ti->value();
					std::string ident = parseIdentifier( si, se);
					if (ti+1 != te && (ti+1)->type() == TokenSuffix)
					{
						++ti;
						if (!endsWith( ident, ti->value())) return false;
						ident.resize( ident.size() - ti->value().size());
					}
					defmap[ variableid] = ident;
					break;
				}
				case TokenSuffix:
					throw std::runtime_error("unexpected token element (TokenSuffix not after TokenVariable) in pattern definition");
			}
		}
		result_defmap = defmap;
		src = si;
		return true;
	}

	void defineOutput( const char* eventname, const char* output)
	{
		m_output[ eventname] = OutputDef( output);
	}

	std::string
		expand(
			const char* eventname,
			const std::map<std::string,std::string>& defmap,
			const std::string& name,
			const std::string& value) const
	{
		std::string out;
		std::map<std::string,OutputDef>::const_iterator oi = m_output.find( eventname);
		if (oi != m_output.end())
		{
			try
			{
				oi->second.print( out, defmap, name, value);
			}
			catch (const std::runtime_error& err)
			{
				throw std::runtime_error( std::string("error in event '") + eventname + "' of pattern '" + m_source + "': " + err.what());
			}
		}
		return out;
	}

	const std::vector<TokenDef>& tokendefs() const
	{
		return m_tokendefs;
	}
	const std::string& source() const
	{
		return m_source;
	}
	const std::string& scope_class() const
	{
		return m_scope_class;
	}
	const std::string& scope_method() const
	{
		return m_scope_method;
	}

	std::string tostring() const
	{
		std::ostringstream out;
		if (!m_scope_class.empty())
		{
			out << m_source << " {scope " << m_scope_class;
			if (!m_scope_method.empty())
			{
				out << "::" << m_scope_method;
			}
			out << "}";
		}
		out << " [";
		std::vector<TokenDef>::const_iterator ti = m_tokendefs.begin(), te = m_tokendefs.end();
		for (int tidx=0; ti != te; ++ti,++tidx)
		{
			if (tidx) out << ", ";
			out << tokenTypeName(ti->type()) << " '" << ti->value() << "'";
		}
		out << "]:" << std::endl;
		std::map<std::string,OutputDef>::const_iterator oi = m_output.begin(), oe = m_output.end();
		for (;oi != oe; ++oi)
		{
			out << "\t" << oi->first << ": " << oi->second.tostring() << std::endl;
		}
		return out.str();
	}

private:
	std::string m_source;
	std::string m_scope_class;
	std::string m_scope_method;
	std::vector<TokenDef> m_tokendefs;
	std::map<std::string,OutputDef> m_output;
};


VariableType::VariableType( const char* pattern_, const char* scope_class_, const char* scope_method_)
{
	m_impl = new Impl( pattern_, scope_class_?scope_class_:"", scope_method_?scope_method_:"");
}

VariableType::VariableType( const VariableType& o)
{
	m_impl = new Impl( *o.m_impl);
}

VariableType::~VariableType()
{
	delete m_impl;
}

const std::string& VariableType::source() const
{
	return m_impl->source();
}

const std::string& VariableType::scope_class() const
{
	return m_impl->scope_class();
}

const std::string& VariableType::scope_method() const
{
	return m_impl->scope_method();
}

VariableType& VariableType::operator()( const char* eventname, const char* output)
{
	m_impl->defineOutput( eventname, output);
	return *this;
}

bool VariableType::parse( std::map<std::string,std::string>& defmap, char const*& si, const char* se) const
{
	return m_impl->parse( defmap, si, se);
}

std::string VariableType::expand(
		const char* eventname,
		const std::map<std::string,std::string>& defmap,
		const std::string& name, const std::string& value) const
{
	return m_impl->expand( eventname, defmap, name, value);
}

std::string VariableType::tostring() const
{
	return m_impl->tostring();
}

std::string VariableValue::tostring() const
{
	std::ostringstream out;
	out << m_type->tostring();
	std::map<std::string,std::string>::const_iterator di = m_defmap.begin(), de = m_defmap.end();
	for (;di != de; ++di)
	{
		out << "\t" << di->first << " = '" << di->second << "'" << std::endl;
	}
	return out.str();
	
}

std::string MethodDef::tostring() const
{
	std::ostringstream out;
	out << "{ " << m_returnvalue.tostring() << "} METHOD " << m_name << "( ";
	std::vector<VariableValue>::const_iterator pi = m_param.begin(), pe = m_param.end();
	for (int pidx=0; pi != pe; ++pi,++pidx)
	{
		out << "{ " << pi->tostring() << " }";
	}
	out << " )";
	if (m_isconst) out << "const" << std::endl;
	return out.str();
}

std::string ClassDef::tostring() const
{
	std::ostringstream out;
	out << "CLASS " << m_name << std::endl;
	std::vector<MethodDef>::const_iterator mi = m_methodar.begin(), me = m_methodar.end();
	for (; mi != me; ++mi)
	{
		out << mi->tostring();
	}
	out << std::endl;
	return out.str();
}

VariableType& TypeSystem::defineType( const char* pattern, const char* scope_class, const char* scope_method)
{
	m_variableTypes.push_back( VariableType( pattern, scope_class, scope_method));
	return m_variableTypes.back();
}

VariableValue TypeSystem::parse( const std::string& scope_class, const std::string& scope_method, char const*& si, const char* se) const
{
	char const* maxend = 0;		//< maximum length (end pos) of types parsed == winning type
	int maxpkt = 0;			//< maximum points in scope match of == winning type in case of equal length
	VariableValue rt;		//< Variable definition found
	std::vector<VariableType>::const_iterator
		vi = m_variableTypes.begin(),
		ve = m_variableTypes.end();
	for (; vi != ve; ++vi)
	{
		// Evaluate scope match points:
		int pkt = 0;
		if (!vi->scope_class().empty() && vi->scope_class() == scope_class)
		{
			if (vi->scope_method().empty())
			{
				pkt = 1;
			}
			else if (vi->scope_method() == scope_method)
			{
				pkt = 2;
			}
		}
		// Parse variable type and check if it is better than the current best match:
		std::map<std::string,std::string> defmap;
		const char* enddef = si;
		if (vi->parse( defmap, enddef, se))
		{
			if (maxend <= enddef)
			{
				if (maxend < enddef || maxpkt < pkt)
				{
					rt = VariableValue( &*vi, defmap);
					maxend = enddef;
					maxpkt = pkt;
				}
			}
		}
	}
	if (maxend)
	{
		si = maxend;
		return rt;
	}
	else
	{
		std::string msg( si, se-si>60?60:(se-si));
		throw std::runtime_error(std::string("no variable type found for '") + msg + "'");
	}
}

std::string TypeSystem::tostring() const
{
	std::ostringstream out;
	std::vector<VariableType>::const_iterator
		vi = m_variableTypes.begin(), ve = m_variableTypes.end();
	for (; vi != ve; ++vi)
	{
		out << vi->tostring();
	}
	return out.str();
}

static unsigned int lineNumber( const char* begin, const char* at)
{
	char const* si = begin;
	unsigned int rt = 0;
	for (; si != at; ++si)
	{
		if (*si == '\n') ++rt;
	}
	return rt+1;
}

static unsigned int colNumber( const char* begin, const char* at)
{
	char const* si = at;
	unsigned int rt = 0;
	for (; si != begin; --si,++rt)
	{
		if (*si == '\t') rt+=7;
		if (*si == '\n') break;
	}
	return rt?rt:1;
}

void InterfacesDef::addSource( const std::string& source)
{
	unsigned int brkcnt = 0;
	char const* si = source.c_str();
	const char* se = si + source.size();
	try
	{
		skipSpacesAndComments( si, se);
		while (si != se)
		{
			if (*si == ';')
			{
				++si;
			}
			else if (isAlpha( *si))
			{
				std::string ident = parseIdentifier( si,se);
				if (ident == "namespace")
				{
					skipToStr( si, se, "{");
					if (si == se) throw std::runtime_error("unexpected end of source ('{' expected after namespace)");
					++si;
					++brkcnt;
				}
				else if (ident == "typedef" || ident == "using")
				{
					skipStructure( si, se);
				}
				else if (ident == "enum")
				{
					skipStructure( si, se);
				}
				else if (ident == "struct")
				{
					skipStructure( si, se);
				}
				else if (ident == "class")
				{
					skipSpacesAndComments( si, se);
					if (si == se || !isAlpha(*si))
					{
						throw std::runtime_error("class name expected after 'class'");
					}
					std::string className( parseIdentifier( si, se));
					skipSpacesAndComments( si, se);
					if (si == se)
					{
						throw std::runtime_error("unexpected end of source after class name definition");
					}
					if (*si == ';') continue;
					if (*si == '{')
					{
						char const* endClass = si++;
						skipBrackets( endClass, se, '{', '}');
	
						parseClass( className, si, endClass-1);
						si = endClass;
					}
				}
			}
			else if (*si == '#')
			{
				skipToEoln( si, se);
			}
			else if (*si == '}' && brkcnt > 0)
			{
				--brkcnt;
				++si;
			}
			else
			{
				throw std::runtime_error( std::string("unexpected character at '") + *si + "'");
			}
			skipSpacesAndComments( si, se);
		}
	}
	catch (const std::runtime_error& err)
	{
		std::ostringstream linemsg;
		linemsg << "line " << lineNumber( source.c_str(), si) << " column " << colNumber( source.c_str(), si);
		throw std::runtime_error( std::string("error in interface source at ") + linemsg.str() + ": " + err.what());
	}
}

static std::string guessMethodName( const char* si, const char* se)
{
	char const* xi = (char const*)std::memchr( si, '(', se-si);
	if (!xi) return std::string();

	for (--xi; xi > si && isSpace(*xi); --xi){}
	if (!isAlnum(*xi)) return std::string();
	for (;xi > si && isAlnum(*xi); --xi){}
	++xi;
	return parseIdentifier( xi, se);
}

void InterfacesDef::parseClass( const std::string& className, char const*& si, const char* se)
{
	ClassDef classDef( className);
	while (si != se)
	{
		skipSpacesAndComments( si, se);
		if (si == se) break;
		if (*si == ';')
		{
			++si;
		}
		else if (*si == '~')
		{
			++si;
			skipStructure( si, se);
		}
		else if (isAlpha( *si))
		{
			char const* start = si;
			std::string ident = parseIdentifier( si,se);
			if (ident == "namespace")
			{
				skipStructure( si, se);
			}
			else if (ident == "enum")
			{
				skipStructure( si, se);
			}
			else if (ident == "typedef" || ident == "using")
			{
				skipStructure( si, se);
			}
			else if (ident == "struct")
			{
				skipStructure( si, se);
			}
			else if (ident == "class")
			{
				char const* sn = si;
				skipStructure( sn, se);
				std::string subClassName( className);
				subClassName.append( "::");
				subClassName.append( parseIdentifier( si, sn));
				skipSpacesAndComments( si, sn);
				if (si == sn)
				{
					throw std::runtime_error("unexpected end of source after sub class name definition");
				}
				if (*si == ';') continue;
				if (*si == '{')
				{
					char const* endClass = si++;
					skipBrackets( endClass, sn, '{', '}');
					parseClass( subClassName, si, endClass-1);
				}
				skipSpacesAndComments( si, sn);
				if (si != sn)
				{
					throw std::runtime_error("unexpected token after subclass definition");
				}
			}
			else if (ident == "virtual")
			{
			}
			else if (ident == "private" || ident == "public" || ident == "protected")
			{
				skipSpacesAndComments( si, se);
				if (si == se)
				{
					throw std::runtime_error("unexpected end of source after class name definition");
				}
				if (*si != ':')
				{
					throw std::runtime_error("expected ':' after private/public/protected");
				}
				++si;
			}
			else
			{
				std::string methodName( className.empty()?std::string():guessMethodName( si, se));
				si = start;
				VariableValue retvaltype = m_typeSystem->parse( className, methodName, si, se);

				skipSpacesAndComments( si, se);
				if (si == se || !isAlpha( *si))
				{
					throw std::runtime_error("method name expected after type definition");
				}
				if (methodName != parseIdentifier( si, se))
				{
					si = start;
					throw std::runtime_error("could not parse return type (method name expected after type)");
				}
				skipSpacesAndComments( si, se);
				if (si == se || *si != '(')
				{
					throw std::runtime_error("open bracket '(' expected after method name");
				}
				char const* endParams = si;
				skipBrackets( endParams, se, '(', ')');
				++si;
				std::vector<VariableValue>
					params = parseParameters( className, methodName, si, endParams-1);
				si = endParams;
				skipSpacesAndComments( si, se);
				bool isconst = false;
				if (si != se)
				{
					if (si+5 < se && 0==std::memcmp( si, "const", 5))
					{
						si+=5;
						skipSpacesAndComments( si, se);
						isconst = true;
					}
					if (si != se && *si == '=')
					{
						++si;
						skipSpacesAndComments( si, se);
						if (si == se || *si != '0')
						{
							throw std::runtime_error("expected '0' after '=' in method declaration");
						}
						++si;
					}
				}
				classDef.addMethod( MethodDef( methodName, retvaltype, params, isconst));
			}
		}
		else
		{
			throw std::runtime_error( std::string("unexpected character '") + *si + "' in structure definition");
		}
		skipSpacesAndComments( si, se);
	}
	m_classdefar.push_back( classDef);
}

static void skipDefaultParameterValue( char const*& si, const char* se)
{
	skipSpacesAndComments( si, se);
	if (si == se)
	{
		throw std::runtime_error("expected value after '=' in method parameter declaration");
	}
	if (isAlpha(*si))
	{
		(void)parseStructureIdentifier( si, se);
		if (si != se && *si == '(')
		{
			skipBrackets( si, se, '(', ')');
			skipSpacesAndComments( si, se);
		}
	}
	else if (isDigit(*si) || *si == '-' || *si == '+')
	{
		(void)parseNumericValue( si, se);
	}
}

std::vector<VariableValue> InterfacesDef::parseParameters(
		const std::string& scope_class,
		const std::string& scope_method,
		char const*& si, const char* se)
{
	std::vector<VariableValue> rt;
	while (si != se)
	{
		const char* paramstart = si;
		rt.push_back( m_typeSystem->parse( scope_class, scope_method, si, se));

		skipSpacesAndComments( si, se);
		if (si != se && isAlpha( *si))
		{
			(void)parseIdentifier(si,se);	//... name of parameter is not interesting
			skipSpacesAndComments( si, se);
			if (si != se && *si == '=')
			{
				++si;
				skipDefaultParameterValue( si, se);
			}
		}
		skipSpacesAndComments( si, se);
		if (si != se)
		{
			if (*si == ',')
			{
				++si;
				continue;
			}
			else
			{
				std::string paramstr( paramstart, si - paramstart);
				std::string paramfollow( si, se-si>20?20:(se-si));
				throw std::runtime_error( std::string("comma expected as separator in method argument list [") + paramstr + "|>" + paramfollow + "]");
			}
		}
	}
	return rt;
}

std::string InterfacesDef::tostring() const
{
	std::ostringstream out;
	std::vector<ClassDef>::const_iterator ci = m_classdefar.begin(), ce = m_classdefar.end();
	for (; ci != ce; ++ci)
	{
		out << ci->tostring();
	}
	out << std::endl;
	return out.str();
}

