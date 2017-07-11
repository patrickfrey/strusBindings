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
			if (!ce) throw std::runtime_error("unterminated variable reference");
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
		throw std::runtime_error("expected tag/variable name as first variable/template argument");
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
			throw std::runtime_error("expected tag name after '=' in variable/template declaration");
		}
		tag = parseIdentifier( si, se);
	}
}

struct TemplateDeclaration
{
	std::string variable;
	std::string tag;
	TemplateContent content;

	TemplateDeclaration( const TemplateDeclaration& o)
		:variable(o.variable),tag(o.tag),content(o.content){}
	TemplateDeclaration( char const*& si, const char* se)
	{
		parseTagDeclaration( variable, tag, si, se);
		if (!skipLineSpaces( si, se))
		{
			throw std::runtime_error("missing template arguments");
		}
		std::vector<std::string> arg;
		while (skipLineSpaces( si, se))
		{
			arg.push_back( parseToken( si, se));
		}
		if (arg.size() < 3)
		{
			throw std::runtime_error("too few template arguments");
		}
		if (arg.size() > 3)
		{
			throw std::runtime_error("too many template arguments");
		}
		if (si == se)
		{
			throw std::runtime_error("unexpected end of template declaration");
		}
		++si;
		const char* endtemplate = std::strstr( si, arg[0].c_str());
		if (!endtemplate)
		{
			throw std::runtime_error("unterminated template content");
		}
		std::string contentsrc( si, endtemplate-si);
		content = TemplateContent( contentsrc, arg[1], arg[2]);
		si = endtemplate + arg[0].size();
	}

	std::string tostring() const
	{
		std::ostringstream out;
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
				if (!skipLineSpaces( si, se)) throw std::runtime_error("unterminated array range");
				if (isDigit(*si) || *si == '-')
				{
					index.first = parseInteger( si, se);
					if (!skipLineSpaces( si, se)) throw std::runtime_error("unterminated array range");
					if (*si == ']')
					{
						index.second = index.first;
					}
				}
				if (*si == ':')
				{
					++si;
					if (!skipLineSpaces( si, se)) throw std::runtime_error("unterminated array range");
					if (isDigit(*si) || *si == '-')
					{
						index.second = parseInteger( si, se);
						if (!skipLineSpaces( si, se)) throw std::runtime_error("unterminated array range");
					}
				}
				if (*si == ']')
				{
					++si;
				}
				else
				{
					throw std::runtime_error("unexpected token in array range");
				}
			}
			else
			{
				throw std::runtime_error("unexpected token, expected array range or nothing");
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
};

class DocGenerator
{
public:
	DocGenerator( const std::string& src)
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
								throw std::runtime_error("only one comment declaration allowed");
							}
							have_comment = true;
							if (skipLineSpaces( si, se))
							{
								m_eolncomment = parseToken( si, se);
							}
							if (skipLineSpaces( si, se))
							{
								throw std::runtime_error("unexpected token after end of line declaration");
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
					}
					else
					{
						throw std::runtime_error("identifier expected at start of line");
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
			char buf[ 2048];
			std::snprintf( buf, sizeof(buf), "error in template source on line %u: %s", getLineNo(start,si), err.what());
			throw std::runtime_error( buf);
		}
	}

	DocGenerator( const DocGenerator& o)
		:m_eolncomment(o.m_eolncomment),m_templates(o.m_templates),m_variables(o.m_variables){}

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
		return out.str();
	}

private:
	std::string m_eolncomment;
	std::vector<TemplateDeclaration> m_templates;
	std::vector<VariableDeclaration> m_variables;
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
	{
		char buf[ 1024];
		std::snprintf( buf, sizeof(buf), "error reading file '%s': %s", filename.c_str(), std::strerror(err));
		throw std::runtime_error( buf);
	}
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
		std::string docsrc = readFile( argv[2]);
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

