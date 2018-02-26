/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface to arbitrary structure as return value
#include "introspection.hpp"
#include "serializer.hpp"
#include "private/internationalization.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/configParser.hpp"
#include "strus/segmenterInterface.hpp"
#include "strus/tokenizerFunctionInterface.hpp"
#include "strus/normalizerFunctionInterface.hpp"
#include "strus/aggregatorFunctionInterface.hpp"
#include "strus/patternLexerInterface.hpp"
#include "strus/patternMatcherInterface.hpp"
#include "strus/postingJoinOperatorInterface.hpp"
#include "strus/weightingFunctionInterface.hpp"
#include "strus/summarizerFunctionInterface.hpp"
#include "strus/scalarFunctionParserInterface.hpp"
#include "papuga/allocator.h"
#include <cstring>
#include <stdexcept>
#include <vector>
#include <utility>

using namespace strus;
using namespace strus::bindings;

static std::runtime_error introspection_error( const char* msg, ErrorBufferInterface* errorhnd)
{
	return strus::runtime_error( "%s: %s", msg, errorhnd->hasError() ? errorhnd->fetchError() : _TXT("unknown error"));
}

static void serializeIntrospection( papuga_Serialization& serialization, const char* name, IntrospectionBase* introspection)
{
	bool sc = true;
	sc &= papuga_Serialization_pushName_charp( &serialization, name);
	sc &= papuga_Serialization_pushOpen( &serialization);
	introspection->serialize( serialization);
	sc &= papuga_Serialization_pushClose( &serialization);
	if (!sc) throw std::bad_alloc();
}

/*
 * DECLARATION INTROSPECTION CONTEXT MEMBER INTERFACE
 */
#define DEFINE_CLASS_INTROSPECTION_CLASS( IntrospectionClassName, ClassName)\
class IntrospectionClassName\
	:public IntrospectionBase\
{\
public:\
	IntrospectionClassName(\
			ErrorBufferInterface* errorhnd_,\
			const ClassName* impl_)\
		:m_errorhnd(errorhnd_),m_impl(impl_){}\
	virtual void serialize( papuga_Serialization& serialization) const;\
	virtual IntrospectionBase* open( const std::string& name) const;\
	virtual std::vector<std::string> list() const;\
private:\
	ErrorBufferInterface* m_errorhnd;\
	const ClassName* m_impl;\
};\
static IntrospectionBase* createIntrospection( ErrorBufferInterface* errorhnd, const ClassName* obj)\
	{return new IntrospectionClassName( errorhnd, obj);}


DEFINE_CLASS_INTROSPECTION_CLASS( IntrospectionTraceProxy, TraceProxy)
DEFINE_CLASS_INTROSPECTION_CLASS( IntrospectionRpcClient, RpcClientInterface)
DEFINE_CLASS_INTROSPECTION_CLASS( IntrospectionModuleLoader, ModuleLoaderInterface)
DEFINE_CLASS_INTROSPECTION_CLASS( IntrospectionTextProcessor, TextProcessorInterface)
DEFINE_CLASS_INTROSPECTION_CLASS( IntrospectionQueryProcessor, QueryProcessorInterface)


/*
 * INTROSPECTION FUNCTION DESCRIPTION
 */
template <class InterfaceClassName>
class IntrospectionFunctionDescription
	:public IntrospectionBase
{
public:
	IntrospectionFunctionDescription(
			ErrorBufferInterface* errorhnd_,
			const InterfaceClassName* impl_)
		:m_errorhnd(errorhnd_),m_impl(impl_){}
	virtual void serialize( papuga_Serialization& serialization) const
	{
		Serializer::serialize( &serialization, m_impl->getDescription(), true/*deep*/);
	}
	virtual IntrospectionBase* open( const std::string& name) const
	{
		return NULL;
	}
	virtual std::vector<std::string> list() const
	{
		return std::vector<std::string>();
	}
private:
	ErrorBufferInterface* m_errorhnd;
	const InterfaceClassName* m_impl;
};

#define DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionClassName, Template, ClassName)\
typedef Template<ClassName> IntrospectionClassName;\
static IntrospectionBase* createIntrospection( ErrorBufferInterface* errorhnd, const ClassName* obj)\
	{return new IntrospectionClassName( errorhnd, obj);}

DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionSegmenter, IntrospectionFunctionDescription, SegmenterInterface)
DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionTokenizerFunction, IntrospectionFunctionDescription, TokenizerFunctionInterface)
DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionNormalizerFunction, IntrospectionFunctionDescription, NormalizerFunctionInterface)
DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionAggregatorFunction, IntrospectionFunctionDescription, AggregatorFunctionInterface)
DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionPatternLexer, IntrospectionFunctionDescription, PatternLexerInterface)
DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionPatternMatcher, IntrospectionFunctionDescription, PatternMatcherInterface)

DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionPostingJoinOperator, IntrospectionFunctionDescription, PostingJoinOperatorInterface)
DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionWeightingFunction, IntrospectionFunctionDescription, WeightingFunctionInterface)
DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionSummarizerFunction, IntrospectionFunctionDescription, SummarizerFunctionInterface)
DEFINE_CLASS_INTROSPECTION_CLASS_TPL( IntrospectionScalarFunctionParser, IntrospectionFunctionDescription, ScalarFunctionParserInterface)

/*
 * INTROSPECTION ATOMIC
 */
template <class TypeName>
class IntrospectionAtomic
	:public IntrospectionBase
{
public:
	IntrospectionAtomic(
			ErrorBufferInterface* errorhnd_,
			const TypeName& value_)
		:m_errorhnd(errorhnd_),m_value(value_){}
	virtual void serialize( papuga_Serialization& serialization) const
	{
		Serializer::serialize( &serialization, m_value, true/*deep*/);
	}
	virtual IntrospectionBase* open( const std::string& name) const
	{
		return NULL;
	}
	virtual std::vector<std::string> list() const
	{
		return std::vector<std::string>();
	}
private:
	ErrorBufferInterface* m_errorhnd;
	TypeName m_value;
};
namespace {
template <class TypeName>
IntrospectionAtomic<TypeName>* createIntrospectionAtomic( ErrorBufferInterface* errorhnd, const TypeName& val)
{
	return new IntrospectionAtomic<TypeName>( errorhnd, val);
}
}//namespace

/*
 * INTROSPECTION FUNCTION LIST
 */
template <class ProviderClass, typename ProviderClass::FunctionType functionType>
struct IntrospectionFunctionListTraits
{};
template <>
struct IntrospectionFunctionListTraits<TextProcessorInterface,TextProcessorInterface::Segmenter>
{
	typedef TextProcessorInterface ProviderClass;
	typedef SegmenterInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getSegmenterByName( name);}
};
template <>
struct IntrospectionFunctionListTraits<TextProcessorInterface,TextProcessorInterface::TokenizerFunction>
{
	typedef TextProcessorInterface ProviderClass;
	typedef TokenizerFunctionInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getTokenizer( name);}
};
template <>
struct IntrospectionFunctionListTraits<TextProcessorInterface,TextProcessorInterface::NormalizerFunction>
{
	typedef TextProcessorInterface ProviderClass;
	typedef NormalizerFunctionInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getNormalizer( name);}
};
template <>
struct IntrospectionFunctionListTraits<TextProcessorInterface,TextProcessorInterface::AggregatorFunction>
{
	typedef TextProcessorInterface ProviderClass;
	typedef AggregatorFunctionInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getAggregator( name);}
};
template <>
struct IntrospectionFunctionListTraits<TextProcessorInterface,TextProcessorInterface::PatternLexer>
{
	typedef TextProcessorInterface ProviderClass;
	typedef PatternLexerInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getPatternLexer( name);}
};
template <>
struct IntrospectionFunctionListTraits<TextProcessorInterface,TextProcessorInterface::PatternMatcher>
{
	typedef TextProcessorInterface ProviderClass;
	typedef PatternMatcherInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getPatternMatcher( name);}
};
template <>
struct IntrospectionFunctionListTraits<QueryProcessorInterface,QueryProcessorInterface::PostingJoinOperator>
{
	typedef QueryProcessorInterface ProviderClass;
	typedef PostingJoinOperatorInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getPostingJoinOperator( name);}
};
template <>
struct IntrospectionFunctionListTraits<QueryProcessorInterface,QueryProcessorInterface::WeightingFunction>
{
	typedef QueryProcessorInterface ProviderClass;
	typedef WeightingFunctionInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getWeightingFunction( name);}
};
template <>
struct IntrospectionFunctionListTraits<QueryProcessorInterface,QueryProcessorInterface::SummarizerFunction>
{
	typedef QueryProcessorInterface ProviderClass;
	typedef SummarizerFunctionInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getSummarizerFunction( name);}
};
template <>
struct IntrospectionFunctionListTraits<QueryProcessorInterface,QueryProcessorInterface::ScalarFunctionParser>
{
	typedef QueryProcessorInterface ProviderClass;
	typedef ScalarFunctionParserInterface FunctionClass;
	static const FunctionClass* get( const ProviderClass* self, const std::string& name)	{return self->getScalarFunctionParser( name);}
};

template <class InterfaceClassName, typename InterfaceClassName::FunctionType functionType>
class IntrospectionFunctionList
	:public IntrospectionBase
{
public:
	typedef IntrospectionFunctionListTraits<InterfaceClassName,functionType> Traits;

	IntrospectionFunctionList(
			ErrorBufferInterface* errorhnd_,
			const InterfaceClassName* impl_)
		:m_errorhnd(errorhnd_),m_impl(impl_){}
	virtual void serialize( papuga_Serialization& serialization) const
	{
		std::vector<std::string> flst = m_impl->getFunctionList( functionType);
		std::vector<std::string>::const_iterator fi = flst.begin(), fe = flst.end();
		for (; fi != fe; ++fi)
		{
			const typename Traits::FunctionClass* func = Traits::get( m_impl, *fi);
			strus::local_ptr<IntrospectionBase> introspection( createIntrospection( m_errorhnd, func));
			serializeIntrospection( serialization, fi->c_str(), introspection.get());
		}
	}
	virtual IntrospectionBase* open( const std::string& name) const
	{
		
		return createIntrospection( m_errorhnd, Traits::get( m_impl, name));
	}
	virtual std::vector<std::string> list() const
	{
		return m_impl->getFunctionList( functionType);
	}
private:
	ErrorBufferInterface* m_errorhnd;
	const InterfaceClassName* m_impl;
};

void IntrospectionContext::serialize( papuga_Serialization& serialization) const
{
	if (m_moduleloader)
	{
		strus::local_ptr<IntrospectionBase> introspection( createIntrospection( m_errorhnd, m_moduleloader));
		serializeIntrospection( serialization, "env", introspection.get());
	}
	if (m_trace)
	{
		strus::local_ptr<IntrospectionBase> introspection( createIntrospection( m_errorhnd, m_trace));
		serializeIntrospection( serialization, "trace", introspection.get());
	}
	if (m_rpc)
	{
		strus::local_ptr<IntrospectionBase> introspection( createIntrospection( m_errorhnd, m_rpc));
		serializeIntrospection( serialization, "rpc", introspection.get());
	}
	if (m_threads > 0)
	{
		Serializer::serializeWithName( &serialization, "threads", (papuga_Int)m_threads, true/*deep*/);
	}
	if (m_textproc)
	{
		strus::local_ptr<IntrospectionBase> introspection( createIntrospection( m_errorhnd, m_textproc));
		serializeIntrospection( serialization, "textproc", introspection.get());
	}
	if (m_queryproc)
	{
		strus::local_ptr<IntrospectionBase> introspection( createIntrospection( m_errorhnd, m_queryproc));
		serializeIntrospection( serialization, "queryproc", introspection.get());
	}
}
IntrospectionBase* IntrospectionContext::open( const std::string& name) const
{
	if (name == "env" && m_moduleloader) return createIntrospection( m_errorhnd, m_moduleloader);
	if (name == "trace" && m_trace) return createIntrospection( m_errorhnd, m_trace);
	if (name == "rpc" && m_rpc) return createIntrospection( m_errorhnd, m_rpc);
	if (name == "queryproc" && m_queryproc) return createIntrospection( m_errorhnd, m_queryproc);
	if (name == "textproc" && m_textproc) return createIntrospection( m_errorhnd, m_textproc);
	return NULL;
}
std::vector<std::string> IntrospectionContext::list() const
{
	std::vector<std::string> rt;
	if (m_moduleloader) rt.push_back( "env");
	if (m_trace) rt.push_back( "trace");
	if (m_rpc) rt.push_back( "rpc");
	if (m_queryproc) rt.push_back( "queryproc");
	if (m_textproc) rt.push_back( "textproc");
	return rt;
}



void IntrospectionRpcClient::serialize( papuga_Serialization& serialization) const
{
	Serializer::serializeWithName( &serialization, "config", strus::getConfigStringItems( m_impl->config(), m_errorhnd), true/*deep*/);
	if (m_errorhnd->hasError()) throw introspection_error( _TXT("serialization error"), m_errorhnd);
}
IntrospectionBase* IntrospectionRpcClient::open( const std::string& name) const
{
	typedef std::vector<std::pair<std::string,std::string> > CfgItems;
	CfgItems items = strus::getConfigStringItems( m_impl->config(), m_errorhnd);
	CfgItems::const_iterator ci = items.begin(), ce = items.end();
	for (; ci != ce; ++ci)
	{
		if (name == ci->first) return createIntrospectionAtomic( m_errorhnd, ci->second);
	}
	return NULL;
}
std::vector<std::string> IntrospectionRpcClient::list() const
{
	std::vector<std::string> rt;
	typedef std::vector<std::pair<std::string,std::string> > CfgItems;
	CfgItems items = strus::getConfigStringItems( m_impl->config(), m_errorhnd);
	CfgItems::const_iterator ci = items.begin(), ce = items.end();
	for (; ci != ce; ++ci)
	{
		rt.push_back( ci->first);
	}
	return rt;
}

void IntrospectionTraceProxy::serialize( papuga_Serialization& serialization) const
{
	Serializer::serializeWithName( &serialization, "config", strus::getConfigStringItems( m_impl->config(), m_errorhnd), true/*deep*/);
	if (m_errorhnd->hasError()) throw introspection_error( _TXT("serialization error"), m_errorhnd);
}
IntrospectionBase* IntrospectionTraceProxy::open( const std::string& name) const
{
	typedef std::vector<std::pair<std::string,std::string> > CfgItems;
	CfgItems items = strus::getConfigStringItems( m_impl->config(), m_errorhnd);
	CfgItems::const_iterator ci = items.begin(), ce = items.end();
	for (; ci != ce; ++ci)
	{
		if (name == ci->first) return createIntrospectionAtomic( m_errorhnd, ci->second);
	}
	return NULL;
}
std::vector<std::string> IntrospectionTraceProxy::list() const
{
	std::vector<std::string> rt;
	typedef std::vector<std::pair<std::string,std::string> > CfgItems;
	CfgItems items = strus::getConfigStringItems( m_impl->config(), m_errorhnd);
	CfgItems::const_iterator ci = items.begin(), ce = items.end();
	for (; ci != ce; ++ci)
	{
		rt.push_back( ci->first);
	}
	return rt;
}

void IntrospectionModuleLoader::serialize( papuga_Serialization& serialization) const
{
	Serializer::serializeWithName( &serialization, "moduledir", m_impl->modulePaths(), true/*deep*/);
	Serializer::serializeWithName( &serialization, "modules", m_impl->modules(), true/*deep*/);
	Serializer::serializeWithName( &serialization, "resourcedir", m_impl->resourcePaths(), true/*deep*/);
	Serializer::serializeWithName( &serialization, "workdir", m_impl->workdir(), true/*deep*/);
}
IntrospectionBase* IntrospectionModuleLoader::open( const std::string& name) const
{
	if (name == "moduledir") return createIntrospectionAtomic( m_errorhnd, m_impl->modulePaths());
	if (name == "modules") return createIntrospectionAtomic( m_errorhnd, m_impl->modules());
	if (name == "resourcedir") return createIntrospectionAtomic( m_errorhnd, m_impl->resourcePaths());
	if (name == "workdir") return createIntrospectionAtomic( m_errorhnd, m_impl->workdir());
	return NULL;
}
std::vector<std::string> IntrospectionModuleLoader::list() const
{
	std::vector<std::string> rt;
	rt.push_back( "moduledir");
	rt.push_back( "modules");
	rt.push_back( "resourcedir");
	rt.push_back( "workdir");
	return rt;
}

typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::Segmenter> IntrospectionSegmenterList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::TokenizerFunction> IntrospectionTokenizerFunctionList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::NormalizerFunction> IntrospectionNormalizerFunctionList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::AggregatorFunction> IntrospectionAggregatorFunctionList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::PatternLexer> IntrospectionPatternLexerList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::PatternMatcher> IntrospectionPatternMatcherList;

void IntrospectionTextProcessor::serialize( papuga_Serialization& serialization) const
{
	{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionSegmenterList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "segmenter", introspection.get());
	}{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionTokenizerFunctionList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "tokenizer", introspection.get());
	}{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionNormalizerFunctionList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "normalizer", introspection.get());
	}{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionAggregatorFunctionList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "aggregator", introspection.get());
	}{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionPatternLexerList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "patternlexer", introspection.get());
	}{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionPatternMatcherList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "patternmatcher", introspection.get());
	}
}
IntrospectionBase* IntrospectionTextProcessor::open( const std::string& name) const
{
	if (name == "segmenter") return new IntrospectionSegmenterList( m_errorhnd, m_impl);
	if (name == "tokenizer") return new IntrospectionTokenizerFunctionList( m_errorhnd, m_impl);
	if (name == "normalizer") return new IntrospectionNormalizerFunctionList( m_errorhnd, m_impl);
	if (name == "aggregator") return new IntrospectionAggregatorFunctionList( m_errorhnd, m_impl);
	if (name == "patternlexer") return new IntrospectionPatternLexerList( m_errorhnd, m_impl);
	if (name == "patternmatcher") return new IntrospectionPatternMatcherList( m_errorhnd, m_impl);
	return NULL;
}
std::vector<std::string> IntrospectionTextProcessor::list() const
{
	std::vector<std::string> rt;
	rt.push_back( "segmenter");
	rt.push_back( "tokenizer");
	rt.push_back( "normalizer");
	rt.push_back( "aggregator");
	rt.push_back( "patternlexer");
	rt.push_back( "patternmatcher");
	return rt;
}

typedef IntrospectionFunctionList<QueryProcessorInterface, QueryProcessorInterface::PostingJoinOperator> IntrospectionPostingJoinOperatorList;
typedef IntrospectionFunctionList<QueryProcessorInterface, QueryProcessorInterface::WeightingFunction> IntrospectionWeightingFunctionList;
typedef IntrospectionFunctionList<QueryProcessorInterface, QueryProcessorInterface::SummarizerFunction> IntrospectionSummarizerFunctionList;
typedef IntrospectionFunctionList<QueryProcessorInterface, QueryProcessorInterface::ScalarFunctionParser> IntrospectionScalarFunctionParserList;

void IntrospectionQueryProcessor::serialize( papuga_Serialization& serialization) const
{
	{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionPostingJoinOperatorList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "segmenter", introspection.get());
	}{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionWeightingFunctionList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "weightfunc", introspection.get());
	}{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionSummarizerFunctionList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "summarizer", introspection.get());
	}{
		strus::local_ptr<IntrospectionBase> introspection( new IntrospectionScalarFunctionParserList( m_errorhnd, m_impl));
		serializeIntrospection( serialization, "scalarfunc", introspection.get());
	}
}
IntrospectionBase* IntrospectionQueryProcessor::open( const std::string& name) const
{
	if (name == "joinop") return new IntrospectionPostingJoinOperatorList( m_errorhnd, m_impl);
	if (name == "weightfunc") return new IntrospectionWeightingFunctionList( m_errorhnd, m_impl);
	if (name == "summarizer") return new IntrospectionSummarizerFunctionList( m_errorhnd, m_impl);
	if (name == "scalarfunc") return new IntrospectionScalarFunctionParserList( m_errorhnd, m_impl);
	return NULL;
}
std::vector<std::string> IntrospectionQueryProcessor::list() const
{
	std::vector<std::string> rt;
	rt.push_back( "joinop");
	rt.push_back( "weightfunc");
	rt.push_back( "summarizer");
	rt.push_back( "scalarfunc");
	return rt;
}


