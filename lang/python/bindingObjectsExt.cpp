#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#define FunctionObject boost::python::api::object
#include "strus/bindingObjects.hpp"
#include "objInitializers.hpp"
#include "private/internationalization.hpp"
#include "private/wcharString.hpp"

WString Term::ucvalue() const
{
	return convert_UTF8_to_UTF16( m_value);
}

WString Attribute::ucvalue() const
{
	return convert_UTF8_to_UTF16( m_value);
}

void Document::setAttribute_unicode( const String& name, const WString& value)
{
	setAttribute( name, convert_UTF16_to_UTF8( value));
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

Document DocumentAnalyzer::analyze_unicode_1( const WString& content)
{
	return analyze( convert_UTF16_to_UTF8( content));
}

Document DocumentAnalyzer::analyze_unicode_2( const WString& content, const DocumentClass& dclass)
{
	return analyze( convert_UTF16_to_UTF8( content), dclass);
}

Document DocumentAnalyzer::analyze_1( const String& content)
{
	return analyze( content);
}

Document DocumentAnalyzer::analyze_2( const String& content, const DocumentClass& dclass)
{
	return analyze( content, dclass);
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

TermVector QueryAnalyzer::analyzePhrase_unicode(
		const String& phraseType,
		const WString& phraseContent) const
{
	return analyzePhrase( phraseType, convert_UTF16_to_UTF8( phraseContent));
}

void QueryAnalyzeQueue::push_unicode( const String& phraseType, const WString& phraseContent)
{
	push( phraseType, convert_UTF16_to_UTF8( phraseContent));
}

void StorageClient::deleteDocument_unicode( const WString& docid)
{
	deleteDocument( convert_UTF16_to_UTF8( docid));
}

void StorageClient::deleteUserAccessRights_unicode( const WString& username)
{
	deleteUserAccessRights( convert_UTF16_to_UTF8( username));
}

WString RankAttribute::ucvalue() const
{
	return convert_UTF8_to_UTF16( m_value);
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

void Query::pushTerm_unicode( const String& type_, const WString& value_)
{
	pushTerm( type_, convert_UTF16_to_UTF8( value_));
}

void Query::addUserName_unicode( const WString& username_)
{
	addUserName( convert_UTF16_to_UTF8( username_));
}

StorageClient Context::createStorageClient_0()
{
	return createStorageClient();
}

StorageClient Context::createStorageClient_1( const String& config_)
{
	return createStorageClient( config_);
}

StorageClient Context::createStorageClient_unicode( const WString& config_)
{
	return createStorageClient( convert_UTF16_to_UTF8( config_));
}

void Context::addModulePath_unicode( const WString& paths_)
{
	addModulePath( convert_UTF16_to_UTF8( paths_));
}

void Context::addResourcePath_unicode( const WString& paths_)
{
	addResourcePath( convert_UTF16_to_UTF8( paths_));
}

void Context::createStorage_unicode( const WString& config_)
{
	return createStorage( convert_UTF16_to_UTF8( config_));
}

void Context::destroyStorage_unicode( const WString& config_)
{
	return destroyStorage( convert_UTF16_to_UTF8( config_));
}

DocumentClass Context::detectDocumentClass_unicode( const WString& content)
{
	return detectDocumentClass( convert_UTF16_to_UTF8( content));
}

DocumentAnalyzer Context::createDocumentAnalyzer_unicode( const WString& segmentername_)
{
	return createDocumentAnalyzer( convert_UTF16_to_UTF8( segmentername_));
}


