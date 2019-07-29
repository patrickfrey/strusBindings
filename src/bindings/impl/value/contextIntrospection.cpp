/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of the root object
#include "contextIntrospection.hpp"
#include "introspectionTemplates.hpp"
#include "serializer.hpp"
#include "private/internationalization.hpp"
#include "strus/base/stdint.h"
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
	virtual ~IntrospectionClassName(){}\
	virtual void serialize( papuga_Serialization& serialization, bool substructure);\
	virtual IntrospectionBase* open( const std::string& name);\
	virtual std::vector<IntrospectionLink> list();\
private:\
	ErrorBufferInterface* m_errorhnd;\
	const ClassName* m_impl;\
};\
static IntrospectionBase* createIntrospection( ErrorBufferInterface* errorhnd, const ClassName* obj)\
	{return new IntrospectionClassName( errorhnd, obj);}

#define DEFINE_CONFIG_INTROSPECTION( IntrospectionClassName, ClassName)\
typedef IntrospectionConfig<ClassName> IntrospectionClassName;\
static IntrospectionBase* createIntrospection( ErrorBufferInterface* errorhnd, const ClassName* obj)\
	{return new IntrospectionClassName( errorhnd, obj);}

DEFINE_CONFIG_INTROSPECTION( IntrospectionTraceProxy, TraceProxy)
DEFINE_CONFIG_INTROSPECTION( IntrospectionRpcClient, RpcClientInterface)
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
	virtual ~IntrospectionFunctionDescription(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		if (substructure)
		{
			bool sc = true;
			sc &= papuga_Serialization_pushOpen( &serialization);
			Serializer::serialize( &serialization, m_impl->view(), true/*deep*/);
			sc &= papuga_Serialization_pushClose( &serialization);
			if (!sc) throw std::bad_alloc();
		}
		else
		{
			Serializer::serialize( &serialization, m_impl->view(), true/*deep*/);
		}
	}
	virtual IntrospectionBase* open( const std::string& name)
	{
		return NULL;
	}
	virtual std::vector<IntrospectionLink> list()
	{
		return std::vector<IntrospectionLink>();
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
namespace {
template <class TypeName>
IntrospectionBase* createIntrospectionAtomic( ErrorBufferInterface* errorhnd, const TypeName& val)
{
	return new IntrospectionAtomic<TypeName>( errorhnd, val);
}
template <class TypeName>
IntrospectionBase* createIntrospectionStructure( ErrorBufferInterface* errorhnd, const TypeName& val)
{
	return new IntrospectionStructure<TypeName>( errorhnd, val);
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
	virtual ~IntrospectionFunctionList(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure)
	{
		serializeMembers( serialization, substructure);
	}
	virtual IntrospectionBase* open( const std::string& name)
	{
		return createIntrospection( m_errorhnd, Traits::get( m_impl, name));
	}
	virtual std::vector<IntrospectionLink> list()
	{
		return IntrospectionLink::getList( false/*autoexpand*/, m_impl->getFunctionList( functionType));
	}
private:
	ErrorBufferInterface* m_errorhnd;
	const InterfaceClassName* m_impl;
};

void ContextIntrospection::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}
IntrospectionBase* ContextIntrospection::open( const std::string& name)
{
	if (name == "env" && m_moduleloader) return createIntrospection( m_errorhnd, m_moduleloader);
	if (name == "trace" && m_trace) return createIntrospection( m_errorhnd, m_trace);
	if (name == "rpc" && m_rpc) return createIntrospection( m_errorhnd, m_rpc);
	if (name == "queryproc" && m_queryproc) return createIntrospection( m_errorhnd, m_queryproc);
	if (name == "textproc" && m_textproc) return createIntrospection( m_errorhnd, m_textproc);
	if (name == "threads") return createIntrospectionAtomic( m_errorhnd, (int64_t)m_threads);
	return NULL;
}
std::vector<IntrospectionLink> ContextIntrospection::list()
{
	static const char* ar[] = {"env","trace","rpc","queryproc","textproc","threads",NULL};
	return getList( ar);
}


void IntrospectionModuleLoader::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}
IntrospectionBase* IntrospectionModuleLoader::open( const std::string& name)
{
	if (name == "moduledir") return createIntrospectionStructure( m_errorhnd, m_impl->modulePaths());
	if (name == "module") return createIntrospectionStructure( m_errorhnd, m_impl->modules());
	if (name == "resourcedir") return createIntrospectionStructure( m_errorhnd, m_impl->resourcePaths());
	if (name == "workdir") return createIntrospectionAtomic( m_errorhnd, m_impl->workingDirectory());
	return NULL;
}
std::vector<IntrospectionLink> IntrospectionModuleLoader::list()
{
	static const char* ar[] = {"moduledir","module","resourcedir","workdir",NULL};
	return getList( ar);
}

typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::Segmenter> IntrospectionSegmenterList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::TokenizerFunction> IntrospectionTokenizerFunctionList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::NormalizerFunction> IntrospectionNormalizerFunctionList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::AggregatorFunction> IntrospectionAggregatorFunctionList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::PatternLexer> IntrospectionPatternLexerList;
typedef IntrospectionFunctionList<TextProcessorInterface, TextProcessorInterface::PatternMatcher> IntrospectionPatternMatcherList;

void IntrospectionTextProcessor::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}
IntrospectionBase* IntrospectionTextProcessor::open( const std::string& name)
{
	if (name == "segmenter") return new IntrospectionSegmenterList( m_errorhnd, m_impl);
	if (name == "tokenizer") return new IntrospectionTokenizerFunctionList( m_errorhnd, m_impl);
	if (name == "normalizer") return new IntrospectionNormalizerFunctionList( m_errorhnd, m_impl);
	if (name == "aggregator") return new IntrospectionAggregatorFunctionList( m_errorhnd, m_impl);
	if (name == "patternlexer") return new IntrospectionPatternLexerList( m_errorhnd, m_impl);
	if (name == "patternmatcher") return new IntrospectionPatternMatcherList( m_errorhnd, m_impl);
	return NULL;
}
std::vector<IntrospectionLink> IntrospectionTextProcessor::list()
{
	static const char* ar[] = {"segmenter","tokenizer","normalizer","aggregator","patternlexer","patternmatcher",NULL};
	return getList( ar);
}

typedef IntrospectionFunctionList<QueryProcessorInterface, QueryProcessorInterface::PostingJoinOperator> IntrospectionPostingJoinOperatorList;
typedef IntrospectionFunctionList<QueryProcessorInterface, QueryProcessorInterface::WeightingFunction> IntrospectionWeightingFunctionList;
typedef IntrospectionFunctionList<QueryProcessorInterface, QueryProcessorInterface::SummarizerFunction> IntrospectionSummarizerFunctionList;
typedef IntrospectionFunctionList<QueryProcessorInterface, QueryProcessorInterface::ScalarFunctionParser> IntrospectionScalarFunctionParserList;

void IntrospectionQueryProcessor::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}
IntrospectionBase* IntrospectionQueryProcessor::open( const std::string& name)
{
	if (name == "joinop") return new IntrospectionPostingJoinOperatorList( m_errorhnd, m_impl);
	if (name == "weightfunc") return new IntrospectionWeightingFunctionList( m_errorhnd, m_impl);
	if (name == "summarizer") return new IntrospectionSummarizerFunctionList( m_errorhnd, m_impl);
	if (name == "scalarfunc") return new IntrospectionScalarFunctionParserList( m_errorhnd, m_impl);
	return NULL;
}
std::vector<IntrospectionLink> IntrospectionQueryProcessor::list()
{
	static const char* ar[] = {"joinop","weightfunc","summarizer","scalarfunc",NULL};
	return getList( ar);
}


