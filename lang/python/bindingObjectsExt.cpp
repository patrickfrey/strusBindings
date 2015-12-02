#define STRUS_BOOST_PYTHON
#include <boost/python.hpp>
#define FunctionObject boost::python::api::object
#include "strus/bindingObjects.hpp"
#include "objInitializers.hpp"
#include "private/internationalization.hpp"
#include "private/wcharString.hpp"

WString Term::ucvalue() const
{
	return convert_uft8string_to_wstring( m_value);
}

WString Attribute::ucvalue() const
{
	return convert_uft8string_to_wstring( m_value);
}

void Document::setAttribute_unicode( const String& name, const WString& value)
{
	setAttribute( name, convert_wstring_to_uft8string( value));
}

void DocumentAnalyzer::addSearchIndexFeature_4(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	addSearchIndexFeature_5( type, selectexpr, tokenizer_, normalizers_, String());
}

void DocumentAnalyzer::addSearchIndexFeature_5(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_,
	const String& options)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addSearchIndexFeature( type, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers, options);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addSearchIndexFeature( type, selectexpr, (const Tokenizer&)tokenizer, normalizers, options);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addSearchIndexFeature( type, selectexpr, tokenizer, (const NormalizerVector&)normalizers, options);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addSearchIndexFeature( type, selectexpr, tokenizer, normalizers, options);
		}
	}
}

void DocumentAnalyzer::addForwardIndexFeature_4(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	addForwardIndexFeature_5( type, selectexpr, tokenizer_, normalizers_, String());
}

void DocumentAnalyzer::addForwardIndexFeature_5(
	const String& type,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_,
	const String& options)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addForwardIndexFeature( type, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers, options);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addForwardIndexFeature( type, selectexpr, (const Tokenizer&)tokenizer, normalizers, options);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			addForwardIndexFeature( type, selectexpr, tokenizer, (const NormalizerVector&)normalizers, options);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			addForwardIndexFeature( type, selectexpr, tokenizer, normalizers, options);
		}
	}
}

void DocumentAnalyzer::defineMetaData_obj(
	const String& fieldname,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			defineMetaData( fieldname, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			defineMetaData( fieldname, selectexpr, (const Tokenizer&)tokenizer, normalizers);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			defineMetaData( fieldname, selectexpr, tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			defineMetaData( fieldname, selectexpr, tokenizer, normalizers);
		}
	}
}

void DocumentAnalyzer::defineAggregatedMetaData_obj(
	const String& fieldname,
	const FunctionObject& function_)
{
	boost::python::extract<Aggregator> function(function_);
	if (function.check())
	{
		defineAggregatedMetaData( fieldname, (const Aggregator&)function);
	}
	else
	{
		Aggregator function;
		initAggregator( function, function_.ptr());
		defineAggregatedMetaData( fieldname, function);
	}
}

void DocumentAnalyzer::defineAttribute_obj(
	const String& fieldname,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			defineAttribute( fieldname, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			defineAttribute( fieldname, selectexpr, (const Tokenizer&)tokenizer, normalizers);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			defineAttribute( fieldname, selectexpr, tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			defineAttribute( fieldname, selectexpr, tokenizer, normalizers);
		}
	}
}

Document DocumentAnalyzer::analyze_unicode_1( const WString& content)
{
	return analyze( convert_wstring_to_uft8string( content));
}

Document DocumentAnalyzer::analyze_unicode_2( const WString& content, const DocumentClass& dclass)
{
	return analyze( convert_wstring_to_uft8string( content), dclass);
}

Document DocumentAnalyzer::analyze_1( const String& content)
{
	return analyze( content);
}

Document DocumentAnalyzer::analyze_2( const String& content, const DocumentClass& dclass)
{
	return analyze( content, dclass);
}

void DocumentAnalyzeQueue::push_unicode_1( const WString& content)
{
	return push( convert_wstring_to_uft8string( content));
}

void DocumentAnalyzeQueue::push_unicode_2( const WString& content, const DocumentClass& dclass)
{
	return push( convert_wstring_to_uft8string( content), dclass);
}


void QueryAnalyzer::definePhraseType_obj(
	const String& phrasetype,
	const String& selectexpr,
	const FunctionObject& tokenizer_,
	const FunctionObject& normalizers_)
{
	boost::python::extract<Tokenizer> tokenizer(tokenizer_);
	if (tokenizer.check())
	{
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			definePhraseType( phrasetype, selectexpr, (const Tokenizer&)tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			definePhraseType( phrasetype, selectexpr, (const Tokenizer&)tokenizer, normalizers);
		}
	}
	else
	{
		Tokenizer tokenizer;
		initTokenizer( tokenizer, tokenizer_.ptr());
		boost::python::extract<NormalizerVector> normalizers(normalizers_);
		if (normalizers.check())
		{
			definePhraseType( phrasetype, selectexpr, tokenizer, (const NormalizerVector&)normalizers);
		}
		else
		{
			NormalizerVector normalizers;
			initNormalizerList( normalizers, normalizers_.ptr());
			definePhraseType( phrasetype, selectexpr, tokenizer, normalizers);
		}
	}
}

TermVector QueryAnalyzer::analyzePhrase_unicode(
		const String& phraseType,
		const WString& phraseContent) const
{
	return analyzePhrase( phraseType, convert_wstring_to_uft8string( phraseContent));
}

void QueryAnalyzeQueue::push_unicode( const String& phraseType, const WString& phraseContent)
{
	push( phraseType, convert_wstring_to_uft8string( phraseContent));
}

void StorageClient::deleteDocument_unicode( const WString& docid)
{
	deleteDocument( convert_wstring_to_uft8string( docid));
}

void StorageClient::deleteUserAccessRights_unicode( const WString& username)
{
	deleteUserAccessRights( convert_wstring_to_uft8string( username));
}

WString RankAttribute::ucvalue() const
{
	return convert_uft8string_to_wstring( m_value);
}


void QueryEval::addSummarizer_obj(
	const String& resultAttribute,
	const String& name,
	const FunctionObject& config_)
{
	boost::python::extract<SummarizerConfig> config(config_);
	if (config.check())
	{
		addSummarizer( resultAttribute, name, (const SummarizerConfig&)config);
	}
	else
	{
		SummarizerConfig config;
		initSummarizerConfig( config, config_.ptr());
		addSummarizer( resultAttribute, name, config);
	}
}

void QueryEval::addWeightingFunction_obj(
	double weight,
	const String& name,
	const FunctionObject& config_)
{
	boost::python::extract<WeightingConfig> config(config_);
	if (config.check())
	{
		addWeightingFunction( weight, name, (const WeightingConfig&)config);
	}
	else
	{
		WeightingConfig config;
		initWeightingConfig( config, config_.ptr());
		addWeightingFunction( weight, name, config);
	}
}

std::size_t QueryExpression::allocid( const WString& str)
{
	return allocid( convert_wstring_to_uft8string( str));
}

void QueryExpression::pushTerm_unicode( const String& type_, const WString& value_)
{
	StackOp op( StackOp::PushTerm, allocid( type_), allocid( value_));
	m_ops.push_back(op);
	m_size += 1;
}

void QueryExpression::attachVariable_unicode( const WString& name_)
{
	StackOp op( StackOp::AttachVariable, allocid( name_));
	m_ops.push_back(op);
}

void Query::addUserName_unicode( const WString& username_)
{
	addUserName( convert_wstring_to_uft8string( username_));
}

void Query::defineFeature_expr_2( const String& set_, const FunctionObject& expr_)
{
	defineFeature_expr_3( set_, expr_, 1.0);
}

void Query::defineFeature_expr_3( const String& set_, const FunctionObject& expr_, double weight_)
{
	boost::python::extract<QueryExpression> expr(expr_);
	if (expr.check())
	{
		defineFeature( set_, (const QueryExpression&)expr, weight_);
	}
	else
	{
		QueryExpression expr;
		initQueryExpression( expr, expr_.ptr());
		defineFeature( set_, expr, weight_);
	}
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
	return createStorageClient( convert_wstring_to_uft8string( config_));
}

WString DocumentFrequencyChange::ucvalue() const
{
	return convert_uft8string_to_wstring( m_value);
}

void Context::addModulePath_unicode( const WString& paths_)
{
	addModulePath( convert_wstring_to_uft8string( paths_));
}

void Context::addResourcePath_unicode( const WString& paths_)
{
	addResourcePath( convert_wstring_to_uft8string( paths_));
}

void Context::createStorage_unicode( const WString& config_)
{
	return createStorage( convert_wstring_to_uft8string( config_));
}

void Context::destroyStorage_unicode( const WString& config_)
{
	return destroyStorage( convert_wstring_to_uft8string( config_));
}

DocumentClass Context::detectDocumentClass_unicode( const WString& content)
{
	return detectDocumentClass( convert_wstring_to_uft8string( content));
}

DocumentAnalyzer Context::createDocumentAnalyzer_unicode( const WString& segmentername_)
{
	return createDocumentAnalyzer( convert_wstring_to_uft8string( segmentername_));
}


