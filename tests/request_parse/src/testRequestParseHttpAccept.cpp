#include "webRequestUtils.hpp"
#include "papuga/encoding.h"
#include "papuga/typedefs.h"
#include "papuga/requestParser.h"
#include <iostream>

static void testFindStringEncoding( const char* testid, const char* http_accept_charset,  papuga_StringEncoding input,  papuga_StringEncoding expected)
{
	std::cerr << "execute accept charset test '" << testid << "'" << std::endl;
	papuga_StringEncoding res = strus::getResultStringEncoding( http_accept_charset, input);
	if (res != expected)
	{
		std::cerr << "find accepted string encoding result does not match in test '" << testid
			<< "', got '" << papuga_stringEncodingName( res)
			<< "', expected '" << papuga_stringEncodingName( expected) << "'" << std::endl;
		exit( 1);
	}	
}

static void testFindContentType( const char* testid, const char* http_accept,  papuga_ContentType input,  papuga_ContentType expected)
{
	std::cerr << "execute accept content type test '" << testid << "'" << std::endl;
	papuga_ContentType res = strus::getResultContentType( http_accept, input);
	if (res != expected)
	{
		std::cerr << "find accepted content type result does not match in test '" << testid
			<< "', got '" << papuga_ContentType_mime( res)
			<< "', expected '" << papuga_ContentType_mime( expected) << "'" << std::endl;
		exit( 2);
	}	
}


int main( int argc, const char* argv[])
{
	testFindStringEncoding( "empty accept, default UTF8", "", papuga_UTF8, papuga_UTF8);
	testFindStringEncoding( "empty accept, default UTF16BE", "", papuga_UTF16BE, papuga_UTF16BE);
	testFindContentType( "empty accept, default application/json", "", papuga_ContentType_JSON, papuga_ContentType_JSON);
	testFindContentType( "empty accept, default application/xml", "", papuga_ContentType_XML, papuga_ContentType_XML);
	testFindContentType( "accept list without weight, default application/json", "application/xml,application/json", papuga_ContentType_JSON, papuga_ContentType_JSON);
	testFindContentType( "accept list (with '+') without weight, default application/json", "application/xml+json", papuga_ContentType_JSON, papuga_ContentType_JSON);
	testFindContentType( "accept list without weight, default application/xml", "application/xml,application/json", papuga_ContentType_XML, papuga_ContentType_XML);
	testFindContentType( "accept list (with '+') without weight, default application/xml", "application/xml+json", papuga_ContentType_XML, papuga_ContentType_XML);

	testFindContentType( "accept list with weight (default application/json)", "application/xml;charset=UTF-8;q=0.75,text/plain;q=0.8,application/json;q=0.6", papuga_ContentType_JSON, papuga_ContentType_JSON);
	testFindContentType( "accept list with weight (no default, find best - 1)", "application/xml;charset=UTF-8;q=0.75,text/plain;q=0.8,application/json;q=0.6", papuga_ContentType_Unknown, papuga_ContentType_XML);
	testFindContentType( "accept list with weight (no default, find best - 2)", "application/xml;charset=UTF-8;q=0.75,text/plain;q=0.8,application/json;q=0.9", papuga_ContentType_Unknown, papuga_ContentType_JSON);
	testFindContentType( "accept list (with '+') with weight (default application/json)", "application/xml+json;charset=UTF-8;q=0.75,text/plain;q=0.8", papuga_ContentType_JSON, papuga_ContentType_JSON);
	testFindContentType( "accept list (with '+') with weight (no default, find best - 1)", "application/xml+json;q=0.75;charset=UTF-8,text/plain;q=0.8", papuga_ContentType_Unknown, papuga_ContentType_XML);
	testFindContentType( "accept list (with '+') with weight (no default, find best - 2)", "application/json+xml;q=0.7;charset=UTF-8,text/plain;q=0.8", papuga_ContentType_Unknown, papuga_ContentType_JSON);
	
	testFindStringEncoding( "single accept without weight", "utf-8", papuga_UTF8, papuga_UTF8);
	testFindStringEncoding( "single with case diff accept without weight", "uTf-8", papuga_UTF8, papuga_UTF8);
	testFindStringEncoding( "accept list without weight (default match UTF-8)", "utf-8, UTF-16", papuga_UTF32BE, papuga_UTF8);
	testFindStringEncoding( "accept list without weight (default match non UTF-8)", "utf-32BE, UTF-16", papuga_UTF32BE, papuga_UTF32BE);
	testFindStringEncoding( "accept list without weight (2nd match)", "iso-latin, UTF-16BE", papuga_UTF32BE, papuga_UTF16BE);

	testFindStringEncoding( "accept list with weight (matching default)", "utf-8, UTF-16; q=0.2, utf-32; q=0.5, utf16LE; q=0.3", papuga_UTF32BE, papuga_UTF32BE);
	testFindStringEncoding( "accept list with weight (no default)", "utf-8, UTF-16; q=0.2, utf-32; q=0.5, utf16LE; q=0.3", papuga_Binary, papuga_UTF32BE);
	std::cerr << "OK" << std::endl;
	return 0;
}
