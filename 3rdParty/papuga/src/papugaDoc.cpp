/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Program for map a doxygen style file with a template
/// \file papugaDoc.cpp
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
#include <cerrno>

#define PAPUGA_LOWLEVEL_DEBUG

static void printUsage()
{
	std::cerr << "papugaDoc <template> <inputfile>" << std::endl;
	std::cerr << "Description : Program for generating documentation, mapping an inputfile with a template" << std::endl;
	std::cerr << "<template>  : File with the template." << std::endl;
	std::cerr << "<inputfile> : input file to map" << std::endl;
}

static bool isSpace( unsigned char ch)
{
	return (ch <= 32);
}
static bool isAlpha( unsigned char ch)
{
	return ((ch|32) >= 'a' || (ch|32) <= 'z' || ch == '_');
}
static bool isDigit( unsigned char ch)
{
	return (ch >= '0' || ch <= '9');
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
			m_chunks.push_back( Chunk( Chunk::Variable, trimString( ci+sb.size(), ce)));
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

	std::string tostring() const
	{
		std::ostringstream out;
		std::vector<Chunk>::const_iterator ci = m_chunks.begin(), ce = m_chunks.end();
		for (; ci != ce; ++ci)
		{
			out << ci->typenam() << " [" << ci->content << "]" << std::endl;
		}
		return out.str();
	}

	std::string expand( const std::map<std::string,std::string>& map) const
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
					rt.append( mi->second);
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
			static const char* ar[] = {"variable","content"};
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
	if (!skipLineSpaces( si, se))
	{
		throw std::runtime_error("missing variable/template arguments");
	}
	if (*si == '=')
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
	std::string variable;
	std::string tag;
	unsigned int groupid;
	TemplateContent content;

	TemplateDeclaration( const TemplateDeclaration& o)
		:variable(o.variable),tag(o.tag),groupid(o.groupid),content(o.content){}
	TemplateDeclaration( char const*& si, const char* se)
	{
		parseTagDeclaration( variable, tag, si, se);
		if (!skipLineSpaces( si, se))
		{
			throw EXCEPTION("missing template arguments");
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

	std::string tostring() const
	{
		std::ostringstream out;
		if (groupid)
		{
			out << "[" << groupid << "] ";
		}
		if (tag == variable)
		{
			out << variable << " " << content.tostring() << std::endl;
		}
		else
		{
			out << variable << "=" << tag << " " << content.tostring() << std::endl;
		}
		return out.str();
	}
};

struct VariableDeclaration
{
	std::string variable;
	std::string tag;
	std::pair<int,int> index;

	VariableDeclaration( const VariableDeclaration& o)
		:variable(o.variable),tag(o.tag),index(o.index){}
	VariableDeclaration( char const*& si, const char* se)
		:index( std::pair<int,int>(0,std::numeric_limits<int>::max()))
	{
		parseTagDeclaration( variable, tag, si, se);
		if (skipLineSpaces( si, se))
		{
			if (*si == '[')
			{
				++si;
				if (!skipLineSpaces( si, se))
				{
					throw EXCEPTION("unterminated array range");
				}
				if (isDigit(*si) || *si == '-')
				{
					index.first = parseInteger( si, se);
					if (!skipLineSpaces( si, se))
					{
						throw EXCEPTION("unterminated array range");
					}
					if (*si == ']')
					{
						index.second = index.first;
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
						index.second = parseInteger( si, se);
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
			else
			{
				throw EXCEPTION("unexpected token, expected array range or nothing");
			}
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
			return annvalue;
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
		if (endidx > valuear.size())
		{
			endidx = valuear.size();
		}
		for (std::size_t vidx=startidx; vidx < endidx; ++vidx)
		{
			if (vidx > startidx)
			{
				rt.push_back( ' ');
			}
			rt.append( valuear[ vidx]);
		}
		return rt;
	}
};

class DocGenerator
{
public:
	DocGenerator( const std::string& src)
		:m_eolncomment(),m_templates(),m_variables(),m_groupmap(),m_groupcnt(0)
	{
		const char* start = src.c_str();
		char const* si = src.c_str();
		const char* se = si + src.size();
		bool have_comment = false;
		try
		{
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
						}
						else if (id == "variable")
						{
							m_variables.push_back( VariableDeclaration( si, se));
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
									throw EXCEPTION("list of identifiers expected in group declaration");
								}
							}
							++m_groupcnt;
						}
					}
					else
					{
						throw EXCEPTION("identifier expected at start of line");
					}
				}
			}
			// Check group references:
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
			// Assign group identifiers:
			std::vector<TemplateDeclaration>::iterator
				ti = m_templates.begin(), te = m_templates.end();
			for (; ti != te; ++ti)
			{
				gi = m_groupmap.find( ti->tag);
				if (gi != m_groupmap.end())
				{
					ti->groupid = gi->second;
				}
			}
			// Check if at least one template defined:
			if (m_templates.empty())
			{
				throw EXCEPTION("at least one template (toplevel template) must be defined");
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
		:m_eolncomment(o.m_eolncomment),m_templates(o.m_templates),m_variables(o.m_variables),m_groupmap(o.m_groupmap),m_groupcnt(o.m_groupcnt){}

	std::string tostring() const
	{
		std::ostringstream out;
		if (!m_eolncomment.empty())
		{
			out << "comment " << m_eolncomment << std::endl;
		}
		std::vector<TemplateDeclaration>::const_iterator ti = m_templates.begin(), te = m_templates.end();
		for (; ti != te; ++ti)
		{
			out << "template " << ti->tostring() << std::endl;
		}
		std::vector<VariableDeclaration>::const_iterator vi = m_variables.begin(), ve = m_variables.end();
		for (; vi != ve; ++vi)
		{
			out << "variable " << vi->tostring() << std::endl;
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

	const std::string& comment() const				{return m_eolncomment;}
	const std::vector<TemplateDeclaration> templates() const	{return m_templates;}
	const std::vector<VariableDeclaration> variables() const	{return m_variables;}
	const std::map<std::string, unsigned int> groupmap() const	{return m_groupmap;}
	unsigned int groupcnt() const					{return m_groupcnt;}

	std::string generate( const std::string& content, const std::map<std::string,std::string>& varmap)
	{
		char const* si = content.c_str();
		const char* start = si;
		const char* se = si + content.size();
		try
		{
			std::list<TemplateInstance> tplist;
			tplist.push_back( TemplateInstance( 0, m_templates[0]));
			tplist.back().assignMap( varmap);
			std::string mainTemplateTagName = m_templates[0].tag;

			while (si != se)
			{
				if (!skipSpaces( si, se)) break;
				if (startsWith( m_eolncomment, si, se))
				{
					si += m_eolncomment.size();
					if (skipLineSpaces( si, se) && *si == '@' && isAlpha( *(si+1)))
					{
						++si;
						Annotation ann = parseAnnotation( si, se);
						bool action = false;
						//[1] First close all templates reaching end of scope
						closeEndOfScopeTemplates( tplist, ann.tag);
						if (tplist.empty() || m_templates[ tplist.front().idx].tag != mainTemplateTagName)
						{
							throw EXCEPTION( "main template reached end of scope before termination");
						}
						//[2] Open templates triggered by the current tag
						action |= openTriggeredTemplates( tplist, ann.tag);
						//[3] Assign variables to top level templates triggered by the annoration found:
						action |= assignAnnotationVariables( tplist, ann);

						//[4] Check if the annotation had meaning (effect) in the current scope:
						if (!action)
						{
							throw EXCEPTION( "unresolved annotation found '%s'", ann.tag.c_str());
						}
					}
				}
			}
			while (tplist.size() > 1)
			{
				closeEndOfScopeTemplates( tplist, m_templates[ tplist.back().idx].tag);
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

		Annotation( const std::string& tag_, const std::string& value_)
			:tag(tag_),value(value_){}
		Annotation( const Annotation& o)
			:tag(o.tag),value(o.value){}
	};
	struct VariableAssignment
	{
		std::string variable;
		std::string value;

		VariableAssignment( const std::string& variable_, const std::string& value_)
			:variable(variable_),value(value_){}
		VariableAssignment( const VariableAssignment& o)
			:variable(o.variable),value(o.value){}
	};

	Annotation parseAnnotation( char const*& si, const char* se)
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
				}
			}
		}
		return Annotation( tag, content);
	}

	struct TemplateInstance
	{
		std::size_t idx;
		std::map<std::string,std::string> map;

		TemplateInstance( std::size_t idx_, const TemplateDeclaration& decl)
			:idx(idx_),map()
		{
			std::vector<std::string> vars = decl.content.variables();
			std::vector<std::string>::const_iterator vi = vars.begin(), ve = vars.end();
			for (; vi != ve; ++vi)
			{
				map[ *vi] = std::string();
			}
		}
		TemplateInstance( const TemplateInstance& o)
			:idx(o.idx),map(o.map){}

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
	};

	void appendTemplateVariable( std::list<TemplateInstance>& tplist, const std::string& variable, const std::string& value) const
	{
		unsigned int count = 0;
		std::list<TemplateInstance>::iterator ti = tplist.begin(), te = tplist.end();
		while (ti != te)
		{
			std::map<std::string,std::string>::iterator mi = ti->map.find( variable);
			if (mi != ti->map.end())
			{
				++count;
				mi->second += value;
			}
		}
		if (count == 0)
		{
			throw EXCEPTION( "variable cannot be assigned '%s'", variable.c_str());
		}
		if (count > 1)
		{
			throw EXCEPTION( "multiple assignments of variable '%s'", variable.c_str());
		}
	}

	std::string getDocGenResult( std::list<TemplateInstance>& tplist) const
	{
		if (tplist.size() != 1) throw EXCEPTION( "internal: call getDocGenResult without templates at end of scope closed");
		return m_templates[ tplist.front().idx].content.expand( tplist.front().map);
	}
	void closeEndOfScopeTemplates( std::list<TemplateInstance>& tplist, const std::string& tagname) const
	{
		unsigned int taggroupid = 0;
		GroupMap::const_iterator gi = m_groupmap.find( tagname);
		if (gi != m_groupmap.end())
		{
			taggroupid = gi->second;
		}

		std::list<TemplateInstance>::iterator ti = tplist.begin(), te = tplist.end();
		std::vector<VariableAssignment> results;
		while (ti != te)
		{
			if (m_templates[ ti->idx].tag == tagname
			||	(taggroupid && m_templates[ ti->idx].groupid == taggroupid))
			{
				std::string var = m_templates[ ti->idx].variable;
				std::string content = m_templates[ ti->idx].content.expand( ti->map);
				ti = tplist.erase( ti);
				results.push_back( VariableAssignment( var, content));
			}
			else
			{
				++ti;
			}
		}
		std::vector<VariableAssignment>::const_iterator ri = results.begin(), re = results.end();
		for (; ri != re; ++ri)
		{
			appendTemplateVariable( tplist, ri->variable, ri->value);
		}
	}

	bool openTriggeredTemplates( std::list<TemplateInstance>& tplist, const std::string& tagname) const
	{
		bool rt = false;
		std::vector<TemplateDeclaration>::const_iterator
			ti = m_templates.begin(), te = m_templates.end();
		for (std::size_t tidx=0; ti != te; ++ti,++tidx)
		{
			if (tagname == ti->tag)
			{
				tplist.push_back( TemplateInstance( tidx, *ti));
				rt = true;
			}
		}
		return rt;
	}

	bool assignAnnotationVariables( std::list<TemplateInstance>& tplist, const Annotation& ann) const
	{
		bool rt = false;
		if (tplist.empty()) throw EXCEPTION("unresolved annotation '%s' in source", ann.tag.c_str());
		// Evaluate the top level scope where the variable can be assigned:
		std::list<TemplateInstance>::iterator te = tplist.end(), ti = tplist.end();
		--ti;
		const TemplateDeclaration& backtdecl = m_templates[ ti->idx];
		while (ti != tplist.begin())
		{
			const TemplateDeclaration& tdecl = m_templates[ ti->idx];
			if (tdecl.tag != backtdecl.tag)
			{
				++ti;
				break;
			}
			else
			{
				--ti;
			}
		}
		// Find all candidate declarations and assign the annotation value to them:
		std::vector<VariableDeclaration>::const_iterator
			vi = m_variables.begin(), ve = m_variables.end();
		for (; vi != ve; ++vi)
		{
			if (vi->tag == ann.tag)
			{
				std::list<TemplateInstance>::iterator ai = ti, ae = te;
				for (; ai != ae; ++ai)
				{
					if (ai->hasVariable( vi->variable))
					{
						std::string value = vi->getValue( ann.value);
						ai->assignVariable( vi->variable, value);
						rt = true;
					}
				}
			}
		}
		return rt;
	}

private:
	std::string m_eolncomment;
	std::vector<TemplateDeclaration> m_templates;
	std::vector<VariableDeclaration> m_variables;
	typedef std::map<std::string, unsigned int> GroupMap;
	GroupMap m_groupmap;
	unsigned int m_groupcnt;
};

static std::string readFile( const std::string& filename)
{
	int err = 0;
	std::string rt;
	FILE* fh = ::fopen( filename.c_str(), "rb");
	if (!fh)
	{
		err = errno;
		goto ERROR;
	}
	unsigned int nn;
	enum {bufsize=(1<<12)};
	char buf[ bufsize];

	while (!!(nn=::fread( buf, 1/*nmemb*/, bufsize, fh)))
	{
		rt.append( buf, nn);
	}
	if (!feof( fh))
	{
		err = ::ferror( fh);
		::fclose( fh);
		goto ERROR;
	}
	::fclose( fh);
	return rt;
ERROR:
	throw EXCEPTION( "error reading file '%s': %s", filename.c_str(), std::strerror(err));
}


int main( int argc, const char** argv)
{
	int ec = 0;
	try
	{
		if (argc < 3)
		{
			printUsage();
			throw std::runtime_error( "too few arguments");
		}
		DocGenerator docgen( readFile( argv[1]));
#ifdef PAPUGA_LOWLEVEL_DEBUG
		std::cerr << docgen.tostring() << std::endl;
#endif
		std::map<std::string,std::string> varmap;
		std::string docsrc = readFile( argv[2]);
		int argi = 3;
		for (; argi != argc; ++argi)
		{
			if (!isAlpha(argv[argi][0])) throw EXCEPTION( "expected assignment <var>=<value> as argument: %s", argv[argi]);
			char const* si = argv[argi];
			const char* start = si;
			for (; *si && *si != '='; ++si){}
			if (!*si) throw EXCEPTION( "expected assignment <var>=<value> as argument: %s", argv[argi]);
			std::string name( start, si-start);
			std::string filename( si+1);
			std::string value = readFile( filename);
			if (varmap.find( name) != varmap.end())
			{
				throw EXCEPTION( "duplicate assignment: %s", argv[argi]);
			}
			varmap[ name] = value;
		}
		std::string result = docgen.generate( docsrc, varmap);
		std::cout << result << std::endl;
		return 0;
	}
	catch (const std::bad_alloc&)
	{
		std::cerr << "memory allocation error" << std::endl;
		return 12/*ENOMEM*/;
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << "error: " << err.what() << std::endl;
		return ec ? ec:-1;
	}
	catch (...)
	{
		std::cerr << "uncaught exception in code generator" << std::endl;
		return -1;
	}
}

