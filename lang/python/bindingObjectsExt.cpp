#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#define FunctionObject boost::python::api::object
#include "strus/bindingObjects.hpp"
#include "objInitializers.hpp"
#include "private/internationalization.hpp"

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


