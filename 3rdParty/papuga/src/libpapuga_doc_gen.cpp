/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library for libpapuga_doc_gen for generating documentation out of a doxygen like syntax with help of some templates
/// \file libpapuga_doc_gen.cpp
#include "papuga/lib/doc_gen.hpp"
#include "private/dll_tags.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <list>
#include <limits>
#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <algorithm>

using namespace papuga;

#undef PAPUGA_LOWLEVEL_DEBUG

static bool isSpace( unsigned char ch)
{
	return (ch <= 32);
}
static bool isAlpha( unsigned char ch)
{
	return ((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_';
}
static bool isDigit( unsigned char ch)
{
	return (ch >= '0' && ch <= '9');
}
static bool isAlnum( unsigned char ch)
{
	return isAlpha(ch) || isDigit(ch);
}
static std::string parseIdentifier( char const*& si, const char* se)
{
	std::string rt;
	for (; si != se && isAlnum(*si); ++si) rt.push_back(*si);
	if (rt.empty()) throw std::runtime_error("identifier expected");
	return rt;
}
static std::string parseToken( char const*& si, const char* se)
{
	std::string rt;
	for (; si != se && !isSpace(*si); ++si) rt.push_back(*si);
	if (rt.empty()) throw std::runtime_error("identifier expected");
	return rt;
}
static std::string parseToEoln( char const*& si, const char* se)
{
	std::string rt;
	for (; si != se && *si != '\n'; ++si) rt.push_back(*si);
	return rt;
}
static unsigned int parseInteger( char const*& si, const char* se)
{
	int rt = 0;
	bool sign = false;
	if (*si == '-')
	{
		sign = true;
		++si;
	}
	if (si == se || !isDigit(*si))
	{
		throw std::runtime_error("integer number expected");
	}
	for (; si != se && isDigit(*si); ++si)
	{
		rt = rt * 10 + (unsigned char)(*si - '0');
	}
	return sign?(-rt):rt;
}
static bool skipSpaces( char const*& si, const char* se)
{
	while (si != se && isSpace(*si)) ++si;
	return si != se;
}
static bool skipLineSpaces( char const*& si, const char* se)
{
	while (si != se && *si != '\n' && isSpace(*si)) ++si;
	return si != se && *si != '\n';
}
static bool skipToEoln( char const*& si, const char* se)
{
	while (si != se && *si != '\n') ++si;
	return si != se;
}
static std::string trimString( char const* si, char const* se)
{
	std::string rt;
	for (; si != se && isSpace(*si); ++si){}
	for (; si != se && isSpace(*(se-1)); --se){}
	return std::string( si, se-si);
}
static bool startsWith( const std::string& tag, char const* si, const char* se)
{
	char const* ti = tag.c_str();
	for (; si != se && *si == *ti; ++si,++ti){}
	if (*ti) return false;
	return true;
}
static std::vector<std::string> splitBySpaces( const std::string& val)
{
	std::vector<std::string> rt;
	char const* ti = val.c_str();
	const char* te = ti + val.size();
	skipSpaces( ti, te);
	while (*ti)
	{
		const char* tkstart = ti;
		while (*ti && !isSpace( *ti)) ++ti;
		rt.push_back( std::string( tkstart, ti-tkstart));
		skipSpaces( ti, te);
	}
	return rt;
}
static std::runtime_error EXCEPTION( const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buf[ 2048];
	std::vsnprintf( buf, sizeof( buf), fmt, ap);
	va_end(ap);
	return std::runtime_error( buf);
}

class TemplateContent
{
public:
	TemplateContent()
		:m_chunks(){}
	TemplateContent( const std::string& content, const std::string& sb, const std::string& eb)
	{
		char const* ci = content.c_str();
		char const* cn = std::strstr( content.c_str(), sb.c_str());
		while (cn)
		{
			m_chunks.push_back( Chunk( Chunk::Content, std::string(ci,cn-ci)));
			const char* ce = std::strstr( cn+sb.size(), eb.c_str());
			if (!ce) throw EXCEPTION("unterminated variable reference, missing '%s'", eb.c_str());
			m_chunks.push_back( Chunk( Chunk::Variable, trimString( cn+sb.size(), ce)));
			ci = ce+eb.size();
			cn = std::strstr( ci, sb.c_str());
		}
		if (*ci)
		{
			m_chunks.push_back( Chunk( Chunk::Content, ci));
		}
	}
	TemplateContent( const TemplateContent& o)
		:m_chunks(o.m_chunks){}

	const std::vector<std::string> variables() const
	{
		std::set<std::string> rt;
		std::vector<Chunk>::const_iterator ci = m_chunks.begin(), ce = m_chunks.end();
		for (; ci != ce; ++ci)
		{
			if (ci->type == Chunk::Variable)
			{
				rt.insert( ci->content);
			}
		}
		return std::vector<std::string>( rt.begin(), rt.end());
	}

	std::string tostring( const std::string& indent, int maxvaluelen) const
	{
		std::ostringstream out;
		std::vector<Chunk>::const_iterator ci = m_chunks.begin(), ce = m_chunks.end();
		for (; ci != ce; ++ci)
		{
			std::string content( ci->content);
			if (maxvaluelen > 0 && (int)content.size() > maxvaluelen)
			{
				content.resize( maxvaluelen);
				content.append( "...");
			}
			out << indent << ci->typenam() << " [" << content << "]" << std::endl;
		}
		return out.str();
	}

	std::string expand( const std::map<std::string,std::string>& map, const std::map<std::string,std::string>& emptymap) const
	{
		std::string rt;
		std::vector<Chunk>::const_iterator ci = m_chunks.begin(), ce = m_chunks.end();
		for (; ci != ce; ++ci)
		{
			switch (ci->type)
			{
				case Chunk::Variable:
				{
					std::map<std::string,std::string>::const_iterator mi = map.find( ci->content);
					if (mi == map.end())
					{
						throw EXCEPTION("internal: variable '%s' not defined", ci->content.c_str());
					}
					if (mi->second.empty())
					{
						mi = emptymap.find( ci->content);
						if (mi != emptymap.end())
						{
							rt.append( mi->second);
						}
					}
					else
					{
						rt.append( mi->second);
					}
					break;
				}
				case Chunk::Content:
					rt.append( ci->content);
					break;
			}
		}
		return rt;
	}

private:
	struct Chunk
	{
		enum Type
		{
			Content,
			Variable
		};
		static const char* typenam( Type i)
		{
			static const char* ar[] = {"content","variable"};
			return ar[i];
		}
		Type type;
		std::string content;

		Chunk( Type type_, const std::string& content_)
			:type(type_), content(content_){}
		Chunk( const Chunk& o)
			:type(o.type), content(o.content){}

		const char* typenam() const
		{
			return typenam( type);
		}
	};
	std::vector<Chunk> m_chunks;
};

static unsigned int getLineNo( const char* start, char const* si)
{
	unsigned int rt = 1;
	for (; si != start; --si)
	{
		if (*si == '\n')
		{
			rt++;
		}
	}
	return rt;
}

static void parseTagDeclaration( std::string& variable, std::string& tag, char const*& si, const char* se)
{
	if (!skipLineSpaces( si, se) || !isAlpha(*si))
	{
		throw EXCEPTION("expected tag/variable name as first variable/template argument");
	}
	tag = parseIdentifier( si, se);
	variable = tag;
	if (skipLineSpaces( si, se) && *si == '=')
	{
		++si;
		if (!skipLineSpaces( si, se))
		{
			throw EXCEPTION("expected tag name after '=' in variable/template declaration");
		}
		tag = parseIdentifier( si, se);
	}
}

struct TemplateDeclaration
{
	std::string variable;		//< destination variable name
	std::string tag;		//< name of triggering event tag
	std::string depvar;		//< variable this template depends on (is mapped only if this variable exists)
	unsigned int groupid;		//< id of the group of this template or 0
	TemplateContent content;	//< content of this template

	TemplateDeclaration( const TemplateDeclaration& o)
		:variable(o.variable),tag(o.tag),depvar(o.depvar),groupid(o.groupid),content(o.content){}
	TemplateDeclaration( char const*& si, const char* se)
		:variable(),tag(),depvar(),groupid(0),content()
	{
		parseTagDeclaration( variable, tag, si, se);
		if (!skipLineSpaces( si, se))
		{
			throw EXCEPTION("missing template arguments");
		}
		if (*si == '?' and isAlpha(si[1]))
		{
			++si;
			depvar = parseIdentifier( si, se);
			if (!skipLineSpaces( si, se))
			{
				throw EXCEPTION("missing template arguments");
			}
		}
		std::vector<std::string> arg;
		while (skipLineSpaces( si, se))
		{
			arg.push_back( parseToken( si, se));
		}
		if (arg.size() < 3)
		{
			throw EXCEPTION("too few template arguments");
		}
		if (arg.size() > 3)
		{
			throw EXCEPTION("too many template arguments");
		}
		if (si == se)
		{
			throw EXCEPTION("unexpected end of template declaration");
		}
		++si;
		const char* endtemplate = std::strstr( si, arg[0].c_str());
		if (!endtemplate)
		{
			throw EXCEPTION("unterminated template content");
		}
		std::string contentsrc( si, endtemplate-si);
		content = TemplateContent( contentsrc, arg[1], arg[2]);
		si = endtemplate + arg[0].size();
	}

	std::string tostring( int maxvaluelen) const
	{
		std::ostringstream out;
		if (groupid)
		{
			out << "[" << groupid << "] ";
		}
		if (tag == variable)
		{
			out << variable;
		}
		else
		{
			out << variable << "=" << tag;
		}
		if (!depvar.empty())
		{
			out << " ?" << depvar;
		}
		out << std::endl << content.tostring(". ", maxvaluelen) << std::endl;
		return out.str();
	}
};

typedef std::string (*EncoderFunction)( const std::string& content);

static std::string xmlencode( const std::string& content)
{
	std::string rt;
	std::string::const_iterator ci = content.begin(), ce = content.end();
	for (; ci != ce; ++ci)
	{
		if (*ci == '<')
		{
			rt.append( "&lt;");
		}
		else if (*ci == '>')
		{
			rt.append( "&gt;");
		}
		else if (*ci == '&')
		{
			rt.append( "&amp;");
		}
		else
		{
			rt.push_back( *ci);
		}
	}
	return rt;
}

static std::string tolowercase( const std::string& content)
{
	std::string rt( content);
	std::transform( content.begin(), content.end(), rt.begin(), ::tolower);
	return rt;
}

static std::string touppercase( const std::string& content)
{
	std::string rt( content);
	std::transform( content.begin(), content.end(), rt.begin(), ::tolower);
	return rt;
}

static EncoderFunction getEncoder( const std::string& funcname)
{
	if (funcname == "xmlencode")
	{
		return &xmlencode;
	}
	else if (funcname == "upcase")
	{
		return &touppercase;
	}
	else if (funcname == "locase")
	{
		return &tolowercase;
	}
	else
	{
		throw EXCEPTION("unknown encoder name '%s'", funcname.c_str());
	}
}

static const char* getEncoderName( const EncoderFunction func)
{
	if (func == &xmlencode)
	{
		return "xmlencode";
	}
	else if (func == &touppercase)
	{
		return "upcase";
	}
	else if (func == &tolowercase)
	{
		return "locase";
	}
	return 0;
}

static std::pair<int,int> parseIndexRange( char const*& si, const char* se)
{
	std::pair<int,int> rt( 0, std::numeric_limits<int>::max());
	if (*si == '[')
	{
		++si;
		if (!skipLineSpaces( si, se))
		{
			throw EXCEPTION("unterminated array range");
		}
		if (isDigit(*si) || *si == '-')
		{
			rt.first = parseInteger( si, se);
			if (!skipLineSpaces( si, se))
			{
				throw EXCEPTION("unterminated array range");
			}
			if (*si == ']')
			{
				rt.second = rt.first;
			}
		}
		if (*si == ':')
		{
			++si;
			if (!skipLineSpaces( si, se))
			{
				throw EXCEPTION("unterminated array range");
			}
			if (isDigit(*si) || *si == '-')
			{
				rt.second = parseInteger( si, se);
				if (!skipLineSpaces( si, se))
				{
					throw EXCEPTION("unterminated array range");
				}
			}
		}
		if (*si == ']')
		{
			++si;
		}
		else
		{
			throw EXCEPTION("unexpected token in array range");
		}
	}
	return rt;
}

struct VariableDeclaration
{
	std::string variable;
	std::string tag;
	std::pair<int,int> index;
	EncoderFunction encoder;

	VariableDeclaration( const VariableDeclaration& o)
		:variable(o.variable),tag(o.tag),index(o.index),encoder(o.encoder){}
	VariableDeclaration( char const*& si, const char* se)
		:index( std::pair<int,int>(0,std::numeric_limits<int>::max())),encoder(0)
	{
		parseTagDeclaration( variable, tag, si, se);
		if (skipLineSpaces( si, se))
		{
			if (*si == '[')
			{
				index = parseIndexRange( si, se);
			}
		}
		if (skipLineSpaces( si, se))
		{
			if (isAlpha(*si))
			{
				encoder = getEncoder( parseIdentifier( si, se));
			}
			else
			{
				throw EXCEPTION("unexpected token, expected array range and/or encoder name");
			}
		}
		if (skipLineSpaces( si, se))
		{
			throw EXCEPTION("extra tokens at end of variable declaration");
		}
	}
	std::string tostring() const
	{
		std::ostringstream idxstr;
		if (index.first != 0 || index.second != std::numeric_limits<int>::max())
		{
			idxstr << "[";
			if (index.first != 0) idxstr << index.first;
			if (index.first != index.second)
			{
				idxstr << ":";
				if (index.second != std::numeric_limits<int>::max()) idxstr << index.second;
			}
			idxstr << "]";
		}
		if (encoder)
		{
			idxstr << " " << getEncoderName( encoder);
		}
		std::ostringstream out;
		if (tag == variable)
		{
			out << variable << idxstr.str() << std::endl;
		}
		else
		{
			out << variable << "=" << tag << idxstr.str() << std::endl;
		}
		return out.str();
	}

	std::string getValue( const std::string& annvalue) const
	{
		if (index.first == 0 && index.second >= (int)annvalue.size())
		{
			if (encoder)
			{
				return encoder( annvalue);
			}
			else
			{
				return annvalue;
			}
		}
		std::string rt;
		std::vector<std::string> valuear = splitBySpaces( annvalue);
		std::size_t endidx = 0;
		std::size_t startidx = 0;
		if (index.second < 0)
		{
			std::size_t ofs = (std::size_t)-index.second;
			endidx = (ofs > valuear.size()) ? 0:(valuear.size()-ofs);
		}
		else
		{
			endidx = index.second;
		}
		if (index.first < 0)
		{
			std::size_t ofs = (std::size_t)-index.first;
			startidx = (ofs > valuear.size()) ? 0:(valuear.size()-ofs);
		}
		else
		{
			startidx = index.first;
		}
		if (endidx >= valuear.size())
		{
			endidx = valuear.size()-1;
		}
		for (std::size_t vidx=startidx; vidx <= endidx; ++vidx)
		{
			if (vidx > startidx)
			{
				rt.push_back( ' ');
			}
			rt.append( valuear[ vidx]);
		}
		if (encoder)
		{
			return encoder( rt);
		}
		else
		{
			return rt;
		}
	}
};

struct IndexDeclaration
{
	typedef std::vector<std::string> TagVariantList;
	typedef std::vector<TagVariantList> TagHierarchy;

	std::string variable;
	std::string tag;
	TagHierarchy taghierarchy;
	EncoderFunction encoder;

	IndexDeclaration( const IndexDeclaration& o)
		:variable(o.variable),tag(o.tag),taghierarchy(o.taghierarchy),encoder(o.encoder){}
	IndexDeclaration( char const*& si, const char* se)
		:variable(),tag(),taghierarchy(),encoder(0)
	{
		parseTagDeclaration( variable, tag, si, se);
		while (skipLineSpaces( si, se) && *si == '{')
		{
			TagVariantList node;
			++si;
			while (skipLineSpaces( si, se))
			{
				if (!isAlpha(*si)) throw EXCEPTION( "list of identifiers expected in tag group of index");
				node.push_back( parseIdentifier( si, se));
				if (!skipLineSpaces( si, se)) throw EXCEPTION( "unterminated tag group of index");
				if (*si == '}') break;
				if (*si == ',')
				{
					++si;
				}
				else
				{
					throw EXCEPTION( "unexpected token in tag group of index");
				}
			}
			if (*si != '}') throw EXCEPTION( "unterminated tag group of index");
			++si;
			taghierarchy.push_back( node);
		}
		if (skipLineSpaces( si, se))
		{
			encoder = getEncoder( parseIdentifier( si, se));
		}
		if (skipLineSpaces( si, se))
		{
			throw EXCEPTION("extra tokens at end of index declaration");
		}
	}

	std::string tostring() const
	{
		std::ostringstream idxstr;
		TagHierarchy::const_iterator hi = taghierarchy.begin(), he = taghierarchy.end();
		for (; hi != he; ++hi)
		{
			idxstr << " (";
			TagVariantList::const_iterator vi = hi->begin(), ve = he->end();
			for (int vidx=0; vi != ve; ++vi,++vidx)
			{
				if (vidx) idxstr << ",";
				idxstr << *vi;
			}
			idxstr << ")";
		}
		if (encoder)
		{
			idxstr << " " << getEncoderName( encoder);
		}
		std::ostringstream out;
		if (tag == variable)
		{
			out << variable << idxstr.str() << std::endl;
		}
		else
		{
			out << variable << "=" << tag << idxstr.str() << std::endl;
		}
		return out.str();
	}
};

class DocGenerator
{
public:
	DocGenerator( std::ostream& errchn, const std::string& src)
		:m_eolncomment(),m_templates(),m_variables(),m_namespaces(),m_emptydeclmap(),m_indices(),m_ignoreset(),m_groupmap(),m_groupcnt(0)
	{
		std::set<std::string> referencedVariables;
		std::set<std::string> definedVariables;
		const char* start = src.c_str();
		char const* si = src.c_str();
		const char* se = si + src.size();
		bool have_comment = false;
		try
		{
			//[0] Parse template source and declare objects:
			while (si != se)
			{
				if (skipSpaces( si, se))
				{
					if (isAlpha( *si))
					{
						std::string id = parseIdentifier( si, se);
						if (id == "comment")
						{
							if (have_comment)
							{
								throw EXCEPTION("only one comment declaration allowed");
							}
							have_comment = true;
							if (skipLineSpaces( si, se))
							{
								m_eolncomment = parseToken( si, se);
							}
							if (skipLineSpaces( si, se))
							{
								throw EXCEPTION("unexpected token after end of line declaration");
							}
						}
						else if (id == "template")
						{
							m_templates.push_back( TemplateDeclaration( si, se));
							const TemplateDeclaration& tdecl = m_templates.back();
							if (m_templates.size() > 1)
							{
								definedVariables.insert( tdecl.variable);
							}
							std::vector<std::string> usedvars = tdecl.content.variables();
							if (!tdecl.depvar.empty())
							{
								std::vector<std::string>::const_iterator
									ui = usedvars.begin(), ue = usedvars.end();
								for (; ui != ue; ++ui)
								{
									if (tdecl.depvar == *ui) break;
								}
								if (ui == ue) throw EXCEPTION("expected dependent variable to be referenced in content of template");
							}
							referencedVariables.insert( usedvars.begin(), usedvars.end());
						}
						else if (id == "empty")
						{
							if (!skipLineSpaces( si, se))
							{
								throw EXCEPTION("expected identifier after '%s'", "empty");
							}
							std::string name = parseIdentifier( si, se);
							if (skipLineSpaces( si, se))
							{
								if (m_emptydeclmap.find( name) != m_emptydeclmap.end())
								{
									throw EXCEPTION("duplicate definition of 'empty %s'", name.c_str());
								}
								m_emptydeclmap[ name] = parseToEoln( si, se);
							}
						}
						else if (id == "variable")
						{
							m_variables.push_back( VariableDeclaration( si, se));
							definedVariables.insert( m_variables.back().variable);
						}
						else if (id == "namespace")
						{
							m_namespaces.push_back( VariableDeclaration( si, se));
							definedVariables.insert( m_namespaces.back().variable);
						}
						else if (id == "index")
						{
							m_indices.push_back( IndexDeclaration( si, se));
							definedVariables.insert( m_indices.back().variable);
						}
						else if (id == "group")
						{
							while (skipLineSpaces( si, se))
							{
								if (isAlpha(*si))
								{
									std::string id = parseIdentifier( si, se);
									if (m_groupmap.find( id) != m_groupmap.end())
									{
										throw EXCEPTION("overlapping group definitions");
									}
									m_groupmap[ id] = (m_groupcnt+1);
								}
								else
								{
									throw EXCEPTION("list of identifiers expected in group declaration", "group");
								}
							}
							++m_groupcnt;
						}
						else if (id == "ignore")
						{
							while (skipLineSpaces( si, se))
							{
								if (isAlpha(*si))
								{
									std::string id = parseIdentifier( si, se);
									m_ignoreset.insert( id);
								}
								else
								{
									throw EXCEPTION("list of identifiers expected in %s declaration", "ignore");
								}
							}
						}
						else
						{
							throw EXCEPTION("unknown template command '%s'", id.c_str());
						}
					}
					else if (*si == '#')
					{
						//... comment (until end of line)
						if (skipToEoln( si, se)) ++si;
					}
					else
					{
						throw EXCEPTION("identifier expected at start of line");
					}
				}
			}{
				//[1] Check group references:
				std::map<std::string,unsigned int>::const_iterator
					gi = m_groupmap.begin(), ge = m_groupmap.end();
				for (; gi != ge; ++gi)
				{
					std::vector<TemplateDeclaration>::const_iterator
						ti = m_templates.begin(), te = m_templates.end();
					for (; ti != te; ++ti)
					{
						if (ti->tag == gi->first) break;
					}
					if (ti == te)
					{
						throw EXCEPTION( "undefined template '%s' referenced in group", gi->first.c_str());
					}
				}
			}{
				//[2] Assign group identifiers:
				std::vector<TemplateDeclaration>::iterator
					ti = m_templates.begin(), te = m_templates.end();
				for (; ti != te; ++ti)
				{
					std::map<std::string,unsigned int>::const_iterator
						gi = m_groupmap.find( ti->tag);
					if (gi != m_groupmap.end())
					{
						ti->groupid = gi->second;
					}
				}
			}{
				//[3] Check elements to ignore (issue warning if they are referenced):
				std::vector<TemplateDeclaration>::iterator
					ti = m_templates.begin(), te = m_templates.end();
				for (; ti != te; ++ti)
				{
					if (m_ignoreset.find( ti->tag) != m_ignoreset.end())
					{
						errchn << "referencing tag '" << ti->tag << "' declared as to ignore" << std::endl;
					}
				}
				std::vector<VariableDeclaration>::const_iterator
					vi = m_variables.begin(), ve = m_variables.end();
				for (; vi != ve; ++vi)
				{
					if (m_ignoreset.find( vi->tag) != m_ignoreset.end())
					{
						errchn << "referencing tag '" << vi->tag << "' declared as to ignore" << std::endl;
					}
				}
				std::vector<VariableDeclaration>::const_iterator
					ni = m_namespaces.begin(), ne = m_namespaces.end();
				for (; ni != ne; ++ni)
				{
					if (m_ignoreset.find( ni->tag) != m_ignoreset.end())
					{
						errchn << "referencing tag '" << ni->tag << "' declared as to ignore" << std::endl;
					}
				}
				std::map<std::string,std::string>::const_iterator
					ei = m_emptydeclmap.begin(), ee = m_emptydeclmap.end();
				for (; ei != ee; ++ei)
				{
					if (m_ignoreset.find( ei->first) != m_ignoreset.end())
					{
						errchn << "referencing tag '" << ei->first << "' declared as to ignore" << std::endl;
					}
				}
			}{
				//[4] Check if at least one template (main template) defined:
				if (m_templates.empty())
				{
					throw EXCEPTION("at least one template (toplevel template) must be defined");
				}
			}{
				//[5] Check variable references:
				std::set<std::string>::const_iterator
					ri = referencedVariables.begin(), re = referencedVariables.end();
				for (; ri != re; ++ri)
				{
					if (definedVariables.find( *ri) == definedVariables.end())
					{
						errchn << "undefined item '" << *ri << "' referenced in template" << std::endl;
					}
				}
				std::set<std::string>::const_iterator
					di = definedVariables.begin(), de = definedVariables.end();
				for (; di != de; ++di)
				{
					if (referencedVariables.find( *di) == referencedVariables.end())
					{
						errchn << "unused item '" << *di << "' defined" << std::endl;
					}
				}
			}
		}
		catch (const std::bad_alloc&)
		{
			throw std::bad_alloc();
		}
		catch (const std::runtime_error& err)
		{
			throw EXCEPTION( "error in template source on line %u: %s", getLineNo(start,si), err.what());
		}
	}

	DocGenerator( const DocGenerator& o)
		:m_eolncomment(o.m_eolncomment)
		,m_templates(o.m_templates)
		,m_variables(o.m_variables)
		,m_namespaces(o.m_namespaces)
		,m_emptydeclmap(o.m_emptydeclmap)
		,m_indices(o.m_indices)
		,m_ignoreset(o.m_ignoreset)
		,m_groupmap(o.m_groupmap)
		,m_groupcnt(o.m_groupcnt){}

	std::string tostring( int maxvaluelen=0) const
	{
		std::ostringstream out;
		if (!m_eolncomment.empty())
		{
			out << "comment " << m_eolncomment << std::endl;
		}
		std::vector<TemplateDeclaration>::const_iterator
			ti = m_templates.begin(), te = m_templates.end();
		for (; ti != te; ++ti)
		{
			out << "template " << ti->tostring( maxvaluelen) << std::endl;
		}
		std::vector<VariableDeclaration>::const_iterator
			vi = m_variables.begin(), ve = m_variables.end();
		for (; vi != ve; ++vi)
		{
			out << "variable " << vi->tostring() << std::endl;
		}
		std::map<std::string,std::string>::const_iterator
			ei = m_emptydeclmap.begin(), ee = m_emptydeclmap.end();
		for (; ei != ee; ++ei)
		{
			out << "empty " << ei->first << " " << ei->second << std::endl;
		}
		std::vector<VariableDeclaration>::const_iterator
			ni = m_namespaces.begin(), ne = m_namespaces.end();
		for (; ni != ne; ++ni)
		{
			out << "namespace " << ni->tostring() << std::endl;
		}
		std::set<std::string>::const_iterator
			xi = m_ignoreset.begin(), xe = m_ignoreset.end();
		for (; xi != xe; ++xi)
		{
			out << "ignore " << *xi << std::endl;
		}
		unsigned int gi=1, ge=m_groupcnt+1;
		for (; gi != ge; ++gi)
		{
			out << "group";
			GroupMap::const_iterator
				mi = m_groupmap.begin(), me = m_groupmap.end();
			for (; mi != me; ++mi)
			{
				out << " " << mi->first;
			}
			out << std::endl;
		}
		return out.str();
	}

	std::string generate( std::ostream& warnings, const std::string& content, const std::map<std::string,std::string>& varmap, bool verbose)
	{
		char const* si = content.c_str();
		const char* start = si;
		const char* se = si + content.size();
		try
		{
			int timestmp = 0;
			std::list<NamespaceInstance> nslist;
			std::list<TemplateInstance> tplist;
			tplist.push_back( TemplateInstance( 0, m_templates[0], ++timestmp));
			tplist.back().assignMap( varmap);
			std::string mainTemplateTagName = m_templates[0].tag;

			std::vector<std::vector<int> > idxar( m_indices.size(), std::vector<int>());
			while (si != se)
			{
				if (!skipSpaces( si, se)) break;
				const char* linestart = si;

				if (startsWith( m_eolncomment, si, se))
				{
					si += m_eolncomment.size();
					if (skipLineSpaces( si, se) && *si == '@' && isAlpha( *(si+1)))
					{
						++si;
						Annotation ann = parseAnnotation( si, se, ++timestmp);
						if (verbose)
						{
							warnings << "parse @" << ann.tag << " [" << ann.value << "]" << std::endl;
						}
#ifdef PAPUGA_LOWLEVEL_DEBUG
						printState( std::cerr, tplist);
#endif
						if (m_ignoreset.find( ann.tag) != m_ignoreset.end())
						{
							continue;
						}
						bool action = false;
						//[1] Close all namespaces reaching end of scope
						closeEndOfScopeNamespaces( nslist, ann.tag);
						//[2] Close all templates reaching end of scope
						closeEndOfScopeTemplates( warnings, tplist, ann.tag, verbose);
						if (tplist.empty() || m_templates[ tplist.front().idx].tag != mainTemplateTagName)
						{
							throw EXCEPTION( "main template reached end of scope before termination");
						}
						//[3] Open namespaces triggered by the current tag
						action |= openTriggeredNamespaces( nslist, ann);
						//[4] Open templates triggered by the current tag
						action |= openTriggeredTemplates( warnings, tplist, nslist, ann, verbose);
						//[5] Assign variables to top level templates triggered by the annoration found:
						action |= assignAnnotationVariables( tplist, ann);
						//[6] Update indices and assign requested index variable values 
						action |= assignIndexVariables( idxar, tplist, ann);
						//[7] Check if the annotation had meaning (effect) in the current scope:
						if (!action)
						{
							warnings << "unresolved annotation found at line " << getLineNo( content.c_str(), linestart) << ": '" << ann.tag << "'" << std::endl;
						}
					}
					else
					{
						if (skipToEoln( si, se)) ++si;
					}
				}
				else
				{
					if (skipToEoln( si, se)) ++si;
				}
			}
			while (tplist.size() > 1)
			{
				closeEndOfScopeTemplates( warnings, tplist, m_templates[ tplist.back().idx].tag, verbose);
			}
			if (tplist.empty())
			{
				throw EXCEPTION( "internal: no templates left, all out of scope, no result available");
			}
			return getDocGenResult( tplist);
		}
		catch (const std::bad_alloc&)
		{
			throw std::bad_alloc();
		}
		catch (const std::runtime_error& err)
		{
			char buf[ 2048];
			std::snprintf( buf, sizeof(buf), "error in doc source on line %u: %s", getLineNo(start,si), err.what());
			throw std::runtime_error( buf);
		}
	}

private:
	struct Annotation
	{
		std::string tag;
		std::string value;
		int timestmp;

		Annotation( const std::string& tag_, const std::string& value_, int timestmp_)
			:tag(tag_),value(value_),timestmp(timestmp_){}
		Annotation( const Annotation& o)
			:tag(o.tag),value(o.value),timestmp(o.timestmp){}
	};

	Annotation parseAnnotation( char const*& si, const char* se, int timestmp)
	{
		std::string tag = parseIdentifier( si, se);
		std::string content;
		if (skipLineSpaces( si, se))
		{
			content = parseToEoln( si, se);
			if (si != se)
			{
				char const* lookahead = si+1;
				while (skipLineSpaces( lookahead, se))
				{
					if (startsWith( m_eolncomment, lookahead, se))
					{
						lookahead += m_eolncomment.size();
						if (!isSpace(*lookahead)) break;
						const char* followstart = lookahead+1;
						if (skipLineSpaces( lookahead, se) && *lookahead != '@')
						{
							si = followstart;
							content += "\n" + parseToEoln( si, se);
							if (si == se) break;
							lookahead = si+1;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
		return Annotation( tag, content, timestmp);
	}

	struct NamespaceInstance
	{
		std::size_t idx;
		std::string value;

		NamespaceInstance( std::size_t idx_, const std::string& value_)
			:idx(idx_),value(value_) {}
		NamespaceInstance( const NamespaceInstance& o)
			:idx(o.idx),value(o.value){}
	};

	struct TemplateInstance
	{
		std::size_t idx;
		std::map<std::string,std::string> map;
		int timestmp;

		TemplateInstance( std::size_t idx_, const TemplateDeclaration& decl, int timestmp_)
			:idx(idx_),map(),timestmp(timestmp_)
		{
			std::vector<std::string> vars = decl.content.variables();
			std::vector<std::string>::const_iterator vi = vars.begin(), ve = vars.end();
			for (; vi != ve; ++vi)
			{
				map[ *vi] = std::string();
			}
		}
		TemplateInstance( const TemplateInstance& o)
			:idx(o.idx),map(o.map),timestmp(o.timestmp){}

		bool hasVariable( const std::string& name) const
		{
			return map.find( name) != map.end();
		}
		void assignVariable( const std::string& name, const std::string& value)
		{
			std::map<std::string,std::string>::iterator mi = map.find( name);
			if (mi == map.end()) throw EXCEPTION("internal: assigned undefined variable");
			if (!mi->second.empty())
			{
				throw EXCEPTION( "variable '%s' assigned twice", name.c_str());
			}
			mi->second = value;
		}
		void assignMap( const std::map<std::string,std::string>& map_)
		{
			std::map<std::string,std::string>::const_iterator
				mi = map_.begin(), me =map_.end();
			for (; mi != me; ++mi)
			{
				if (map.find( mi->first) != map.end())
				{
					throw EXCEPTION( "unknown variable '%s' assigned", mi->first.c_str());
				}
				map[ mi->first] = mi->second;
			}
		}
		static std::string mapToString( const std::map<std::string,std::string>& strmap)
		{
			std::ostringstream out;
			std::map<std::string,std::string>::const_iterator
				mi = strmap.begin(), me=strmap.end();
			for (int midx=0; mi != me; ++mi,++midx)
			{
				if (midx) out << ", ";
				std::string value = mi->second;
				if (value.size() > 10)
				{
					value.resize( 10);
					value.append( "...");
				}
				out << mi->first << " -> '" << value << "'";
			}
			return out.str();
		}
		std::string tostring() const
		{
			std::ostringstream out;
			out << "[" << idx << "] " << mapToString( map) << std::endl;
			return out.str();
		}
	};

#ifdef PAPUGA_LOWLEVEL_DEBUG
	void printState( std::ostream& out, const std::list<TemplateInstance>& tplist) const
	{
		std::list<TemplateInstance>::const_iterator ti = tplist.begin(), te = tplist.end();
		int maxtimestmp = 0;
		for (; ti != te; ++ti) if (ti->timestmp > maxtimestmp) maxtimestmp = ti->timestmp;
		for (ti = tplist.begin(); ti != te; ++ti)
		{
			out << "    " << ((ti->timestmp == maxtimestmp)?"*":">")
				<< m_templates[ ti->idx].variable << " "
				<< TemplateInstance::mapToString( ti->map) << std::endl;
		}
	}
#endif

	void appendTemplateVariable( std::ostream& warnings, std::list<TemplateInstance>& tplist, const std::string& variable, const std::string& value, int evtimestmp, bool verbose) const
	{
		int timestmp = 0;
		std::list<TemplateInstance>::iterator ti = tplist.begin(), te = tplist.end();
		for (;ti != te; ++ti)
		{
			if (evtimestmp >= ti->timestmp)
			{
				std::map<std::string,std::string>::iterator mi = ti->map.find( variable);
				if (mi != ti->map.end())
				{
					if (timestmp < ti->timestmp)
					{
						timestmp = ti->timestmp;
					}
				}
			}
		}
		ti = tplist.begin();
		for (;ti != te; ++ti)
		{
			if (timestmp == ti->timestmp)
			{
				std::map<std::string,std::string>::iterator mi = ti->map.find( variable);
				if (mi != ti->map.end())
				{
					if (verbose)
					{
						warnings << "append var " << variable << "='" << std::string( value.c_str(), std::min( value.size(), (std::size_t)30)) << "' to template " << m_templates[ ti->idx].variable << std::endl;
					}
					mi->second += value;
				}
			}
		}
	}

	std::string getDocGenResult( std::list<TemplateInstance>& tplist) const
	{
		if (tplist.size() != 1) throw EXCEPTION( "internal: call getDocGenResult without templates at end of scope closed");
		return m_templates[ tplist.front().idx].content.expand( tplist.front().map, m_emptydeclmap);
	}
	bool closeEndOfScopeVariable( std::ostream& warnings, std::list<TemplateInstance>& tplist, const std::string& variable, unsigned int depth, bool verbose) const
	{
		bool rt = false;
		if (!depth) throw EXCEPTION( "circular variable reference");
		std::list<TemplateInstance>::iterator ti = tplist.end();
		while (ti != tplist.begin())
		{
			--ti;
			if (m_templates[ ti->idx].variable == variable)
			{
				if (closeEndOfScopeVariables( warnings, tplist, m_templates[ ti->idx].content.variables(), depth-1, verbose))
				{
					ti = tplist.end();
					rt = true;
					continue;
				}
				if (isCompleteTemplateInstance( *ti))
				{
					std::string content = m_templates[ ti->idx].content.expand( ti->map, m_emptydeclmap);
					int evtimestmp = ti->timestmp;
					ti = tplist.erase( ti);
					appendTemplateVariable( warnings, tplist, variable, content, evtimestmp, verbose);
					if (verbose)
					{
						warnings << "close sub template " << variable << " = '" << std::string( content.c_str(), std::min( content.size(), (std::size_t)30)) << "'" << std::endl;
					}
				}
				else
				{
					ti = tplist.erase( ti);
					//.... do not assign template content if no event received*/
					if (verbose)
					{
						warnings << "incomplete sub template '" << variable << "' discarded" << std::endl;
					}
				}

			}
		}
		return rt;
	}
	bool closeEndOfScopeVariables( std::ostream& warnings, std::list<TemplateInstance>& tplist, const std::vector<std::string>& variables, unsigned int depth, bool verbose) const
	{
		bool rt = false;
		std::vector<std::string>::const_iterator vi = variables.begin(), ve = variables.end();
		for (; vi != ve; ++vi)
		{
			rt |= closeEndOfScopeVariable( warnings, tplist, *vi, depth-1, verbose);
		}
		return rt;
	}
	bool isCompleteTemplateInstance( const TemplateInstance& ti) const
	{
		const TemplateDeclaration& tdecl = m_templates[ ti.idx];
		std::map<std::string,std::string>::const_iterator mi,me;
		if (!tdecl.depvar.empty())
		{
			mi = ti.map.find( tdecl.depvar);
			if (mi == ti.map.end()) throw EXCEPTION( "internal: bad dependent variable reference");

			if (!mi->second.empty()) return true;
			if (m_emptydeclmap.find( tdecl.depvar) != m_emptydeclmap.end()) return true;
		}
		else
		{
			mi = ti.map.begin(), me = ti.map.end();
			for (; mi != me; ++mi)
			{
				if (!mi->second.empty()) return true;
				if (m_emptydeclmap.find( mi->first) != m_emptydeclmap.end()) return true;
			}
		}
		return false;
	}
	void closeEndOfScopeTemplates( std::ostream& warnings, std::list<TemplateInstance>& tplist, const std::string& tagname, bool verbose) const
	{
		unsigned int taggroupid = 0;
		GroupMap::const_iterator gi = m_groupmap.find( tagname);
		if (gi != m_groupmap.end())
		{
			taggroupid = gi->second;
		}

		std::list<TemplateInstance>::iterator ti = tplist.end();
		while (ti != tplist.begin())
		{
			--ti;
			if (m_templates[ ti->idx].tag == tagname
			||	(taggroupid && m_templates[ ti->idx].groupid == taggroupid))
			{
				if (closeEndOfScopeVariables( warnings, tplist, m_templates[ ti->idx].content.variables(), m_templates.size(), verbose))
				{
					ti = tplist.end();
					continue;
				}
				if (isCompleteTemplateInstance( *ti))
				{
					std::string var = m_templates[ ti->idx].variable;
					std::string content = m_templates[ ti->idx].content.expand( ti->map, m_emptydeclmap);
					int evtimestmp = ti->timestmp;
					ti = tplist.erase( ti);
					appendTemplateVariable( warnings, tplist, var, content, evtimestmp, verbose);
					if (verbose)
					{
						warnings << "close template " << var << " = '" << std::string( content.c_str(), std::min( content.size(), (std::size_t)30)) << "'" << std::endl;
					}
				}
				else
				{
					std::string var = m_templates[ ti->idx].variable;
					ti = tplist.erase( ti);
					//.... do not assign template content if no event received*/
					if (verbose)
					{
						warnings << "incomplete template '" << var << "' discarded" << std::endl;
					}
				}
			}
		}
	}
	void closeEndOfScopeNamespaces( std::list<NamespaceInstance>& nslist, const std::string& tagname) const
	{
		unsigned int taggroupid = 0;
		GroupMap::const_iterator gi = m_groupmap.find( tagname);
		if (gi != m_groupmap.end())
		{
			taggroupid = gi->second;
		}
		std::list<NamespaceInstance>::iterator ni = nslist.end();
		while (ni != nslist.begin())
		{
			--ni;
			unsigned int nsgroupid = 0;
			gi = m_groupmap.find( m_namespaces[ ni->idx].tag);
			if (gi != m_groupmap.end())
			{
				nsgroupid = gi->second;
			}
			if (m_namespaces[ ni->idx].tag == tagname || (taggroupid && nsgroupid == taggroupid))
			{
				ni = nslist.erase( ni);
			}
		}
	}
	bool openTriggeredNamespaces( std::list<NamespaceInstance>& nslist, const Annotation& ann) const
	{
		bool rt = false;
		std::vector<VariableDeclaration>::const_iterator
			ni = m_namespaces.begin(), ne = m_namespaces.end();
		for (std::size_t nidx=0; ni != ne; ++ni,++nidx)
		{
			if (ann.tag == ni->tag)
			{
				std::string value = ni->getValue( ann.value);
				nslist.push_back( NamespaceInstance( nidx, value));
				rt = true;
			}
		}
		return rt;
	}
	int isRequestedVariable( const std::list<TemplateInstance>& tplist, const std::string& variable) const
	{
		if (tplist.empty()) return false;
		std::list<TemplateInstance>::const_iterator
			ti = tplist.begin(), te = tplist.end();
		for (; ti != te; ++ti)
		{
			if (ti->idx == 0 || ti->hasVariable( variable))
			{
				return true;
			}
		}
		return false;
	}
	bool openTriggeredTemplates( std::ostream& warnings, std::list<TemplateInstance>& tplist, const std::list<NamespaceInstance>& nslist, const Annotation& ann, bool verbose) const
	{
		bool rt = false;
		std::vector<TemplateDeclaration>::const_iterator
			ti = m_templates.begin(), te = m_templates.end();
		for (std::size_t tidx=0; ti != te; ++ti,++tidx)
		{
			if (ann.tag == ti->tag && isRequestedVariable( tplist, ti->variable))
			{
				tplist.push_back( TemplateInstance( tidx, m_templates[ tidx], ann.timestmp));
				TemplateInstance& ctp = tplist.back();
				std::list<NamespaceInstance>::const_iterator
					ni = nslist.begin(), ne = nslist.end();
				for (; ni != ne; ++ni)
				{
					const VariableDeclaration& var = m_namespaces[ ni->idx];
					if (ctp.hasVariable( var.variable))
					{
						ctp.assignVariable( var.variable, ni->value);
					}
				}
				if (verbose)
				{
					warnings << "open template " << m_templates[ tidx].variable << std::endl;
				}
				rt = true;
			}
		}
		return rt;
	}
	bool assignVariableValuesToTemplates( std::list<TemplateInstance>& tplist, const std::string& variable, const std::string& value) const
	{
		if (tplist.empty()) return false;
		bool rt = false;
		// Evaluate the top level scope where the variable can be assigned:
		std::list<TemplateInstance>::iterator te = tplist.end(), ti = tplist.end();
		--ti;
		const TemplateDeclaration& backtdecl = m_templates[ ti->idx];
		while (ti != tplist.begin())
		{
			const TemplateDeclaration& tdecl = m_templates[ ti->idx];
			if (tdecl.tag != backtdecl.tag || tplist.back().timestmp != ti->timestmp)
			{
				++ti;
				break;
			}
			else
			{
				--ti;
			}
		}
		for (; ti != te; ++ti)
		{
			if (ti->hasVariable( variable))
			{
				ti->assignVariable( variable, value);
				rt = true;
			}
		}
		return rt;
	}

	bool assignAnnotationVariables( std::list<TemplateInstance>& tplist, const Annotation& ann) const
	{
		bool rt = false;

		// Find all candidate declarations and assign the annotation value to them:
		std::vector<VariableDeclaration>::const_iterator
			vi = m_variables.begin(), ve = m_variables.end();
		for (; vi != ve; ++vi)
		{
			if (vi->tag == ann.tag)
			{
				std::string value = vi->getValue( ann.value);
				rt |= assignVariableValuesToTemplates( tplist, vi->variable, value);
			}
		}
		return rt;
	}

	static std::string tagHierarchyVariantToString( const IndexDeclaration::TagVariantList& vlist)
	{
		std::string rt;
		IndexDeclaration::TagVariantList::const_iterator
			vi = vlist.begin(), ve = vlist.end();
		for (int vidx=0; vi != ve; ++vi,++vidx)
		{
			if (vidx) rt.push_back(',');
			rt.append( *vi);
		}
		return rt;
	}

	bool assignIndexVariables( std::vector<std::vector<int> >& idxar, std::list<TemplateInstance>& tplist, const Annotation& ann) const
	{
		bool rt = false;
		std::vector<IndexDeclaration>::const_iterator di = m_indices.begin(), de = m_indices.end();
		std::size_t didx = 0;
		for (; di != de; ++di,++didx)
		{
			std::size_t tidx = 0;
			IndexDeclaration::TagHierarchy::const_iterator
				ti = di->taghierarchy.begin(), te = di->taghierarchy.end();
			for (; ti != te; ++ti,++tidx)
			{
				IndexDeclaration::TagVariantList::const_iterator
					vi = ti->begin(), ve = ti->end();
				for (; vi != ve && ann.tag != *vi; ++vi){}
				if (vi != ve) break;
			}
			if (ti != te)
			{
				if (idxar[ didx].size() == tidx)
				{
					idxar[ didx].push_back( 1);
				}
				else if (idxar[ didx].size() < tidx)
				{
					std::string thstr = tagHierarchyVariantToString( di->taghierarchy[ tidx-1]);
					throw EXCEPTION( "gap in index hierarchy, missing link {%s}", thstr.c_str());
				}
				else
				{
					while (idxar[ didx].size() > tidx+1)
					{
						idxar[ didx].pop_back();
					}
					++idxar[ didx][ tidx];
				}
			}
		}
		di = m_indices.begin(), de = m_indices.end();
		for (int didx=0; di != de; ++di,++didx)
		{
			if (ann.tag == di->tag)
			{
				std::ostringstream idxval;
				std::vector<int>::const_iterator
					xi = idxar[ didx].begin(), xe = idxar[ didx].end();
				for (int xidx=0; xi != xe; ++xi,++xidx)
				{
					if (xidx) idxval << "_";
					idxval << *xi;
				}
				rt |= assignVariableValuesToTemplates( tplist, di->variable, idxval.str());
			}
		}
		return rt;
	}

private:
	std::string m_eolncomment;
	std::vector<TemplateDeclaration> m_templates;
	std::vector<VariableDeclaration> m_variables;
	std::vector<VariableDeclaration> m_namespaces;
	std::map<std::string,std::string> m_emptydeclmap;
	std::vector<IndexDeclaration> m_indices;
	std::set<std::string> m_ignoreset;
	typedef std::map<std::string, unsigned int> GroupMap;
	GroupMap m_groupmap;
	unsigned int m_groupcnt;
};


DLL_PUBLIC bool papuga::generateDoc(
	std::ostream& out,
	std::ostream& err,
	const std::string& templatesrc,
	const std::string& docsrc,
	const std::map<std::string,std::string>& varmap,
	bool verbose)
{
	try
	{
		DocGenerator docgen( err, templatesrc);
#ifdef PAPUGA_LOWLEVEL_DEBUG
		std::cerr << docgen.tostring( 20) << std::endl;
#endif
		out << docgen.generate( err, docsrc, varmap, verbose) << std::endl;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		err << "memory allocation error" << std::endl;
		return false;
	}
	catch (const std::runtime_error& exc)
	{
		err << exc.what() << std::endl;
		return false;
	}
	catch (...)
	{
		err << "uncaught exception in code generator";
		return false;
	}
}

