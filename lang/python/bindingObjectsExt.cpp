#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#define FunctionObject boost::python::api::object
#include "strus/bindingObjects.hpp"
#include "objInitializers.hpp"
#include "private/internationalization.hpp"
#include "textwolf/textscanner.hpp"
#include "textwolf/charset_utf8.hpp"
#include "textwolf/charset_utf16.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/wstringiterator.hpp"
#include "textwolf/staticbuffer.hpp"

std::wstring convert_UTF8_to_UTF16( const std::string& val)
{
	std::wstring rt;
	typedef textwolf::CStringIterator SrcIterator;
	typedef textwolf::TextScanner<SrcIterator,textwolf::charset::UTF8> Scanner;
	SrcIterator src( val.c_str(), val.size());
	Scanner itr( src);
	char charbufmem[ 16];
	textwolf::StaticBuffer charbuf( charbufmem, sizeof(charbufmem));
	textwolf::charset::UTF16<> out;

	while (*itr)
	{
		textwolf::UChar ch = *itr;
		charbuf.clear();
		out.print( ch, charbuf);
		const char* ptr = charbuf.ptr();
		std::size_t ii=0, nn=charbuf.size();
		for (; ii<nn; ii+=sizeof(wchar_t))
		{
			wchar_t ch = textwolf::ByteOrderConverter<sizeof(wchar_t)>::read( (unsigned const char*)(ptr+ii));
			rt.push_back( ch);
		}
		++itr;
	}
	return rt;
}

std::string convert_UTF16_to_UTF8( const std::wstring& val)
{
	std::string rt;
	typedef textwolf::WStringIterator SrcIterator;
	typedef textwolf::TextScanner<SrcIterator,textwolf::charset::UTF16<> > Scanner;
	SrcIterator src( val.c_str(), val.size());
	Scanner itr( src);
	textwolf::charset::UTF8 out;

	while (*itr)
	{
		textwolf::UChar ch = *itr;
		out.print( ch, rt);
		++itr;
	}
	return rt;
}


void DocumentAnalyzer::addSearchIndexFeature_4(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	Tokenizer tokenizer;
	NormalizerVector normalizers;
	initTokenizer( tokenizer, tokenizer_.ptr());
	initNormalizerList( normalizers, normalizers_.ptr());
	addSearchIndexFeature( type, selectexpr, tokenizer, normalizers, StringVector());
}
void DocumentAnalyzer::addSearchIndexFeature_5(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_,
	const StringVector& options)
{
	Tokenizer tokenizer;
	NormalizerVector normalizers;
	initTokenizer( tokenizer, tokenizer_.ptr());
	initNormalizerList( normalizers, normalizers_.ptr());
	addSearchIndexFeature( type, selectexpr, tokenizer, normalizers, options);
}

void DocumentAnalyzer::addForwardIndexFeature_4(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	Tokenizer tokenizer;
	NormalizerVector normalizers;
	initTokenizer( tokenizer, tokenizer_.ptr());
	initNormalizerList( normalizers, normalizers_.ptr());
	addForwardIndexFeature( type, selectexpr, tokenizer, normalizers, StringVector());
}
void DocumentAnalyzer::addForwardIndexFeature_5(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_,
	const StringVector& options)
{
	Tokenizer tokenizer;
	NormalizerVector normalizers;
	initTokenizer( tokenizer, tokenizer_.ptr());
	initNormalizerList( normalizers, normalizers_.ptr());
	addForwardIndexFeature( type, selectexpr, tokenizer, normalizers, options);
}

void DocumentAnalyzer::defineMetaData_obj(
	const String& fieldname,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	Tokenizer tokenizer;
	NormalizerVector normalizers;
	initTokenizer( tokenizer, tokenizer_.ptr());
	initNormalizerList( normalizers, normalizers_.ptr());
	defineMetaData( fieldname, selectexpr, tokenizer, normalizers);
}

void DocumentAnalyzer::defineAggregatedMetaData_obj(
	const String& fieldname,
	const FunctionObject& function_)
{
	Aggregator function;
	initAggregator( function, function_.ptr());
	defineAggregatedMetaData( fieldname, function);
}

void DocumentAnalyzer::defineAttribute_obj(
	const String& fieldname,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	Tokenizer tokenizer;
	NormalizerVector normalizers;
	initTokenizer( tokenizer, tokenizer_.ptr());
	initNormalizerList( normalizers, normalizers_.ptr());
	defineAttribute( fieldname, selectexpr, tokenizer, normalizers);
}

void QueryAnalyzer::definePhraseType_obj(
	const String& phrasetype,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	Tokenizer tokenizer;
	NormalizerVector normalizers;
	initTokenizer( tokenizer, tokenizer_.ptr());
	initNormalizerList( normalizers, normalizers_.ptr());
	definePhraseType( phrasetype, selectexpr, tokenizer, normalizers);
}

void QueryEval::addSummarizer_obj(
	const String& resultAttribute,
	const String& name,
	const FunctionObject& config_)
{
	SummarizerConfig config;
	int rt = initSummarizerConfig( config, config_.ptr());
	if (rt) throw strus::runtime_error(_TXT("error parsing summarizer function config"));
	addSummarizer( resultAttribute, name, config);
}

void QueryEval::addWeightingFunction_obj(
	double weight,
	const String& name,
	const FunctionObject& config_)
{
	WeightingConfig config;
	int rt = initWeightingConfig( config, config_.ptr());
	if (rt) throw strus::runtime_error(_TXT("error parsing weighting function config"));
	addWeightingFunction( weight, name, config);
}


