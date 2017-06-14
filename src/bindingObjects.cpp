/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/bindingObjects.hpp"
#undef String
#undef StringVector
#undef NormalizerVector
#undef TermVector
#undef RankVector
#undef RankAttributeVector
#undef AttributeVector
#undef MetaDataVector

#include "strus/strus.hpp"
#include "strus/lib/rpc_client.hpp"
#include "strus/lib/rpc_client_socket.hpp"
#include "strus/lib/module.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/lib/error.hpp"
#include "strus/lib/pattern_serialize.hpp"
#include "strus/rpcClientInterface.hpp"
#include "strus/rpcClientMessagingInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "strus/statisticsBuilderInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/scalarFunctionInterface.hpp"
#include "strus/scalarFunctionParserInterface.hpp"
#include "strus/queryAnalyzerContextInterface.hpp"
#include "strus/patternMatcherInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/patternTermFeederInterface.hpp"
#include "strus/patternTermFeederInstanceInterface.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/vectorStorageTransactionInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/utf8.hpp"
#include "strus/base/symbolTable.hpp"
#include "strus/base/fileio.hpp"
#include "private/internationalization.hpp"
#include "utils.hpp"
#include "private/traceUtils.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <limits>

template <class Object>
struct ReferenceDeleter
{
	static void function( void* obj)
	{
		delete (Object*)obj;
	}
};

void Tokenizer::addArgumentInt( long arg_)
{
	addArgument( strus::utils::tostring( arg_));
}

void Tokenizer::addArgumentFloat( double arg_)
{
	addArgument( strus::utils::tostring( (float)arg_));
}

void Normalizer::addArgumentInt( long arg_)
{
	addArgument( strus::utils::tostring( arg_));
}

void Normalizer::addArgumentFloat( double arg_)
{
	addArgument( strus::utils::tostring( (float)arg_));
}

void Aggregator::addArgumentInt( long arg_)
{
	addArgument( strus::utils::tostring( arg_));
}

void Aggregator::addArgumentFloat( double arg_)
{
	addArgument( strus::utils::tostring( (float)arg_));
}

Variant::Variant()
	:m_type(Variant_UNDEFINED){}

Variant::Variant( const Variant& o)
	:m_type(o.m_type),m_buf(o.m_buf)
{
	if (m_type == Variant_TEXT)
	{
		m_value.TEXT = m_buf.c_str();
	}
	else
	{
		std::memcpy( &m_value, &o.m_value, sizeof(m_value));
	}
}

Variant::Variant( unsigned int v)
	:m_type(Variant_UINT)
{
	m_value.UINT = v;
}

Variant::Variant( int v)
	:m_type(Variant_INT)
{
	m_value.INT = v;
}

Variant::Variant( double v)
	:m_type(Variant_FLOAT)
{
	m_value.FLOAT = v;
}

Variant::Variant( const std::string& v)
	:m_type(Variant_TEXT),m_buf(v)
{
	m_value.TEXT = m_buf.c_str();
}

Variant::Variant( const char* v)
	:m_type(Variant_TEXT),m_buf(v)
{
	m_value.TEXT = m_buf.c_str();
}

void Variant::init()
{
	m_type = Variant_UNDEFINED;
	m_value.INT = 0;
	m_buf.clear();
}

void Variant::assign( const Variant& o)
{
	m_type = o.m_type;
	m_buf = o.m_buf;
	if (m_type == Variant_TEXT)
	{
		m_value.TEXT = m_buf.c_str();
	}
	else
	{
		std::memcpy( &m_value, &o.m_value, sizeof(m_value));
	}
}

void Variant::assignUint( unsigned long v)
{
	m_type = Variant_UINT;
	m_value.UINT = v;
	m_buf.clear();
}

void Variant::assignInt( long v)
{
	m_type = Variant_INT;
	m_value.INT = v;
	m_buf.clear();
}

void Variant::assignFloat( double v)
{
	m_type = Variant_FLOAT;
	m_value.FLOAT = v;
	m_buf.clear();
}

void Variant::assignText( const std::string& v)
{
	m_buf = v;
	m_type = Variant_TEXT;
	m_value.TEXT = m_buf.c_str();
}

unsigned long Variant::getUInt() const
{
	switch (m_type)
	{
		case Variant_INT: if (m_value.INT>=0) return m_value.UINT; else break;
		case Variant_UINT: return (int)m_value.UINT;
		case Variant_UNDEFINED: break;
		case Variant_FLOAT: break;
		case Variant_TEXT: break;
	}
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
	if (m_type == Variant_UINT) return m_value.UINT;
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
}

long Variant::getInt() const
{
	switch (m_type)
	{
		case Variant_INT: return (int)m_value.INT;
		case Variant_UINT: if (m_value.INT>=0) return (int)m_value.INT; else break;
		case Variant_UNDEFINED: break;
		case Variant_FLOAT: break;
		case Variant_TEXT: break;
	}
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
}

double Variant::getFloat() const
{
	switch (m_type)
	{
		case Variant_INT: return (double)m_value.INT;
		case Variant_UINT: return (double)m_value.UINT;
		case Variant_FLOAT: return m_value.FLOAT;
		case Variant_UNDEFINED: break;
		case Variant_TEXT: break;
	}
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
}

const char* Variant::getText() const
{
	if (m_type == Variant_TEXT) return m_value.TEXT;
	throw strus::runtime_error( _TXT( "illegal access of variant value"));
}

bool Variant::isEqual( const Variant& o) const
{
	if (m_type != o.m_type)
	{
		if (m_type == Variant_UINT)
		{
			if (o.m_type == Variant_INT)
			{
				if (o.m_value.INT >= 0) return (unsigned int)o.m_value.INT == m_value.UINT;
			}
		}
		else if (m_type == Variant_INT)
		{
			if (o.m_type == Variant_UINT)
			{
				if (m_value.INT >= 0) return (unsigned int)m_value.INT == o.m_value.UINT;
			}
		}
		return false;
	}
	switch (m_type)
	{
		case Variant_UNDEFINED: return true;
		case Variant_UINT: return m_value.UINT == o.m_value.UINT;
		case Variant_INT: return m_value.INT == o.m_value.INT;
		case Variant_FLOAT: {double ww = m_value.FLOAT - o.m_value.FLOAT; return (ww < 0.0)?(-ww<std::numeric_limits<double>::epsilon()):(ww<std::numeric_limits<double>::epsilon());}
		case Variant_TEXT: return (m_value.TEXT && o.m_value.TEXT)?(std::strcmp( m_value.TEXT, o.m_value.TEXT)==0):(m_value.TEXT==o.m_value.TEXT);
	}
	return false;
}

bool Term::operator < (const Term& o) const
{
	if (m_position == o.m_position)
	{
		if (m_length == o.m_length)
		{
			return (m_type == o.m_type)
				? m_value < o.m_value
				: m_type < o.m_type;
		}
		else
		{
			return m_length < o.m_length;
		}
	}
	else
	{
		return m_position < o.m_position;
	}
}

Document::Document( const Document& o)
	:m_searchIndexTerms(o.m_searchIndexTerms)
	,m_forwardIndexTerms(o.m_forwardIndexTerms)
	,m_metaData(o.m_metaData)
	,m_attributes(o.m_attributes)
	,m_users(o.m_users)
	,m_docid(o.m_docid)
{}

void Document::addSearchIndexTerm(
		const std::string& type_,
		const std::string& value_,
		const Index& position_)
{
	m_searchIndexTerms.push_back( Term( type_,value_,position_, 1/*length*/));
}

void Document::addForwardIndexTerm(
		const std::string& type_,
		const std::string& value_,
		const Index& position_)
{
	m_forwardIndexTerms.push_back( Term( type_,value_,position_, 1/*length*/));
}

void Document::setMetaData( const std::string& name_, const Variant& value_)
{
	m_metaData.push_back( MetaData( name_,value_));
}

void Document::setMetaData( const std::string& name_, double value_)
{
	m_metaData.push_back( MetaData( name_,Variant(value_)));
}

void Document::setMetaData( const std::string& name_, int value_)
{
	m_metaData.push_back( MetaData( name_,Variant(value_)));
}

void Document::setMetaData( const std::string& name_, unsigned int value_)
{
	m_metaData.push_back( MetaData( name_,Variant(value_)));
}

void Document::setAttribute( const std::string& name_, const std::string& value_)
{
	m_attributes.push_back( Attribute( name_, value_));
}

void Document::setDocid( const std::string& docid_)
{
	m_docid = docid_;
	m_attributes.push_back( Attribute( "docid", docid_));
}

void Document::setUserAccessRight( const std::string& username_)
{
	m_users.push_back( username_);
}

DocumentAnalyzer::DocumentAnalyzer( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd, const std::string& segmentername, const void* textproc_)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(ReferenceDeleter<strus::DocumentAnalyzerInterface>::function)
	,m_textproc(textproc_)
{
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = (const strus::TextProcessorInterface*)m_textproc;
	const strus::SegmenterInterface* segmenter = textproc->getSegmenterByName( segmentername);
	if (!segmenter)
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to get document document segmenter by name: %s"), errorhnd->fetchError());
	}
	m_analyzer_impl.reset( objBuilder->createDocumentAnalyzer( segmenter));
	if (!m_analyzer_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to create document analyzer: %s"), errorhnd->fetchError());
	}
}

DocumentAnalyzer::DocumentAnalyzer( const DocumentAnalyzer& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
	,m_textproc(o.m_textproc)
{}


static strus::analyzer::FeatureOptions getFeatureOptions(
	const std::string& options)
{
	strus::analyzer::FeatureOptions rt;
	char const* ci = options.c_str();
	const char* ce = options.c_str() + options.size();

	while (ci)
	{
		char const* cn = std::strchr( ci, ',');
		std::string item( cn?strus::utils::trim( std::string( ci, cn-ci)):strus::utils::trim( std::string( ci, ce-ci)));
		if (item.empty())
		{}
		else if (strus::utils::caseInsensitiveEquals( item, "BindPosSucc"))
		{
			rt.definePositionBind( strus::analyzer::BindSuccessor);
		}
		else if (strus::utils::caseInsensitiveEquals( item, "BindPosPred"))
		{
			rt.definePositionBind( strus::analyzer::BindPredecessor);
		}
		else if (strus::utils::caseInsensitiveEquals( item, "BindPosContent"))
		{
			rt.definePositionBind( strus::analyzer::BindContent);
		}
		else if (strus::utils::caseInsensitiveEquals( item, "BindPosUnique"))
		{
			rt.definePositionBind( strus::analyzer::BindUnique);
		}
		else
		{
			throw strus::runtime_error( _TXT( "unknown feature option '%s'"), item.c_str());
		}
		ci = cn?(cn+1):cn;
	}
	return rt;
}

static std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > getNormalizers(
		const std::vector<Normalizer>& normalizers,
		const strus::TextProcessorInterface* textproc,
		strus::ErrorBufferInterface* errorhnd)
{
	std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > rt;
	std::vector<Normalizer>::const_iterator
		ni = normalizers.begin(), ne = normalizers.end();
	for (;ni != ne; ++ni)
	{
		const strus::NormalizerFunctionInterface* nf = textproc->getNormalizer( ni->name());
		if (!nf) throw strus::runtime_error( _TXT("failed to get normalizer function '%s': %s"), ni->name().c_str(), errorhnd->fetchError());

		strus::Reference<strus::NormalizerFunctionInstanceInterface> function(
				nf->createInstance( ni->arguments(), textproc));
		if (!function.get()) throw strus::runtime_error( _TXT("failed to create normalizer function instance '%s': %s"), ni->name().c_str(), errorhnd->fetchError());

		rt.push_back( function);
	}
	return rt;
}

static strus::Reference<strus::TokenizerFunctionInstanceInterface> getTokenizer(
		const Tokenizer& tokenizer,
		const strus::TextProcessorInterface* textproc,
		strus::ErrorBufferInterface* errorhnd)
{
	const strus::TokenizerFunctionInterface* tf = textproc->getTokenizer( tokenizer.name());
	if (!tf) throw strus::runtime_error( _TXT("failed to get tokenizer function '%s': %s"), tokenizer.name().c_str(), errorhnd->fetchError());

	strus::Reference<strus::TokenizerFunctionInstanceInterface> rt(
			tf->createInstance( tokenizer.arguments(), textproc));
	if (!rt.get()) throw strus::runtime_error( _TXT("failed to create tokenizer function instance '%s': %s"), tokenizer.name().c_str(), errorhnd->fetchError());

	return rt;
}

struct FeatureFuncDef
{
	std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> > normalizers_ref;
	std::vector<strus::NormalizerFunctionInstanceInterface*> normalizers;
	strus::Reference<strus::TokenizerFunctionInstanceInterface> tokenizer;

	FeatureFuncDef( const Reference& objbuilder_impl,
			const Tokenizer& tokenizer_,
			const std::vector<Normalizer>& normalizers_,
			strus::ErrorBufferInterface* errorhnd)
	{
		const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)objbuilder_impl.get();
		const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor object: %s"), errorhnd->fetchError());

		normalizers_ref = getNormalizers( normalizers_, textproc, errorhnd);
		std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) normalizers.push_back( ni->get());
		tokenizer = getTokenizer( tokenizer_, textproc, errorhnd);
	}

	FeatureFuncDef( const Reference& objbuilder_impl,
			const std::vector<Normalizer>& normalizers_,
			strus::ErrorBufferInterface* errorhnd)
	{
		const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)objbuilder_impl.get();
		const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor object: %s"), errorhnd->fetchError());

		normalizers_ref = getNormalizers( normalizers_, textproc, errorhnd);
		std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) normalizers.push_back( ni->get());
	}

	void release()
	{
		(void)tokenizer.release();
		std::vector<strus::Reference<strus::NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) (void)ni->release();
	}
};

void DocumentAnalyzer::addSearchIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers,
	const std::string& options)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addSearchIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::addForwardIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers,
	const std::string& options)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addForwardIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::defineMetaData(
	const std::string& fieldname,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineMetaData(
		fieldname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzer::defineAggregatedMetaData(
	const std::string& fieldname,
	const Aggregator& function)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());

	const strus::AggregatorFunctionInterface* functionimpl = textproc->getAggregator( function.name());
	if (!functionimpl) throw strus::runtime_error( _TXT("failed to get aggregator function '%s': %s"), function.name().c_str(), errorhnd->fetchError());

	strus::Reference<strus::AggregatorFunctionInstanceInterface> functioninst( functionimpl->createInstance( function.arguments()));
	if (!functioninst.get())
	{
		throw strus::runtime_error( _TXT("failed to create aggregator function '%s': %s"), function.name().c_str(), errorhnd->fetchError());
	}
	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAggregatedMetaData(
		fieldname, functioninst.get());
	functioninst.release();
}

void DocumentAnalyzer::defineAttribute(
	const std::string& attribname,
	const std::string& selectexpr,
	const Tokenizer& tokenizer,
	const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAttribute(
		attribname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzer::addSearchIndexFeatureFromPatternMatch(
	const std::string& type,
	const std::string& patternTypeName,
	const std::vector<Normalizer>& normalizers,
	const std::string& options)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addSearchIndexFeatureFromPatternMatch(
		type, patternTypeName, funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::addForwardIndexFeatureFromPatternMatch(
	const std::string& type,
	const std::string& patternTypeName,
	const std::vector<Normalizer>& normalizers,
	const std::string& options)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->addForwardIndexFeatureFromPatternMatch(
		type, patternTypeName, funcdef.normalizers,
		getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzer::defineMetaDataFromPatternMatch(
	const std::string& fieldname,
	const std::string& patternTypeName,
	const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineMetaDataFromPatternMatch(
		fieldname, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzer::defineAttributeFromPatternMatch(
	const std::string& attribname,
	const std::string& patternTypeName,
	const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, normalizers, errorhnd);

	((strus::DocumentAnalyzerInterface*)m_analyzer_impl.get())->defineAttributeFromPatternMatch(
		attribname, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

static Variant getNumericVariantFromDouble( double value)
{
	if (value - std::floor( value) < std::numeric_limits<double>::epsilon())
	{
		if (value < 0.0)
		{
			return Variant( (int)(std::floor( value) + std::numeric_limits<double>::epsilon()));
		}
		else
		{
			return Variant( (unsigned int)(std::floor( value) + std::numeric_limits<double>::epsilon()));
		}
	}
	else
	{
		return Variant( value);
	}
}

static strus::NumericVariant numericVariant( const Variant& val)
{
	strus::NumericVariant rt;
	switch (val.type())
	{
		case Variant_UNDEFINED:
			break;
		case Variant_INT:
			rt = (int64_t)val.getInt();
			break;
		case Variant_UINT:
			rt = (uint64_t)val.getUInt();
			break;
		case Variant_FLOAT:
			rt = (double)val.getFloat();
			break;
		case Variant_TEXT:
			if (!rt.initFromString( val.getText()))
			{
				throw strus::runtime_error( _TXT( "failed convert variant to numeric value"));
			}
			break;
	}
	return rt;
}

static strus::PatternMatcherInstanceInterface::JoinOperation patternMatcherJoinOp( const char* opname)
{
	if (std::strcmp( opname, "sequence") == 0)
	{
		return strus::PatternMatcherInstanceInterface::OpSequence;
	}
	else if (std::strcmp( opname, "sequence_imm") == 0)
	{
		return strus::PatternMatcherInstanceInterface::OpSequenceImm;
	}
	else if (std::strcmp( opname, "sequence_struct") == 0)
	{
		return strus::PatternMatcherInstanceInterface::OpSequenceStruct;
	}
	else if (std::strcmp( opname, "within") == 0)
	{
		return strus::PatternMatcherInstanceInterface::OpWithin;
	}
	else if (std::strcmp( opname, "within_struct") == 0)
	{
		return strus::PatternMatcherInstanceInterface::OpWithinStruct;
	}
	else if (std::strcmp( opname, "any") == 0)
	{
		return strus::PatternMatcherInstanceInterface::OpAny;
	}
	else if (std::strcmp( opname, "and") == 0)
	{
		return strus::PatternMatcherInstanceInterface::OpAnd;
	}
	else
	{
		throw strus::runtime_error(_TXT("unknown operator '%s' in pattern expression"), opname);
	}
}

enum {MaxPatternTermNameId=(1<<24)};

class PatternMatchLoader
{
public:
static void loadPatterMatcher(
	strus::Reference<strus::PatternMatcherInstanceInterface>& matcherInstance, 
	strus::Reference<strus::PatternTermFeederInstanceInterface>& feederInstance, 
	const strus::TextProcessorInterface* textproc,
	const std::string& patternMatcherModule,
	const PatternMatcher& patternMatcher,
	strus::ErrorBufferInterface* errorhnd)
{
	const strus::PatternMatcherInterface* matcher = textproc->getPatternMatcher( patternMatcherModule);
	if (!matcher) throw strus::runtime_error(_TXT("failed to load matcher module: %s"), errorhnd->fetchError());
	const strus::PatternTermFeederInterface* feeder = textproc->getPatternTermFeeder();
	matcherInstance.reset( matcher->createInstance());
	feederInstance.reset( feeder->createInstance());
	if (!feederInstance.get()) throw strus::runtime_error(_TXT("failed to create pattern feeder: %s"), errorhnd->fetchError());
	if (!matcherInstance.get()) throw strus::runtime_error(_TXT("failed to create pattern matcher: %s"), errorhnd->fetchError());

	strus::SymbolTable termsymtab;
	strus::SymbolTable termtypetab;

	std::vector<PatternMatcher::StackOp>::const_iterator oi = patternMatcher.ops().begin(), oe = patternMatcher.ops().end();
	for (; oi != oe; ++oi)
	{
		switch (oi->type)
		{
			case PatternMatcher::StackOp::PushTerm:
			{
				const char* type_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Term_type];
				const char* value_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Term_value];
				if (!value_[0] && type_[0] == '~')
				{
					matcherInstance->pushPattern( type_ +1);
				}
				else
				{
					uint32_t termtypeid = termtypetab.getOrCreate( type_);
					if (termtypetab.isNew())
					{
						if (termtypeid >= MaxPatternTermNameId) throw strus::runtime_error(_TXT("too many lexems defined in pattern match program"));
						feederInstance->defineLexem( termtypeid, type_);
					}
					if (!value_[0])
					{
						matcherInstance->pushTerm( termtypeid);
					}
					else
					{
						uint32_t termsymid = termsymtab.getOrCreate(
							termSymbolKey( termtypeid, value_));
						if (termsymtab.isNew())
						{
							feederInstance->defineSymbol( termsymid, termtypeid, value_);
						}
						matcherInstance->pushTerm( termsymid);
					}
				}
				break;
			}
			case PatternMatcher::StackOp::PushPattern:
			{
				const char* name_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Pattern_name];
				matcherInstance->pushPattern( name_);
				break;
			}
			case PatternMatcher::StackOp::PushExpression:
			{
				const char* opname_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Expression_opname];
				unsigned int argc_ = (unsigned int)oi->arg[ PatternMatcher::StackOp::Expression_argc];
				int range_ = (int)oi->arg[ PatternMatcher::StackOp::Expression_range];
				unsigned int cardinality_ = (unsigned int)oi->arg[ PatternMatcher::StackOp::Expression_cardinality];
				strus::PatternMatcherInstanceInterface::JoinOperation joinop = patternMatcherJoinOp( opname_);
				matcherInstance->pushExpression( joinop, argc_, range_, cardinality_);
				break;
			}
			case PatternMatcher::StackOp::DefinePattern:
			{
				const char* name_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Pattern_name];
				bool visible_ = (int)oi->arg[ PatternMatcher::StackOp::Pattern_visible];
				matcherInstance->definePattern( name_, visible_);
				break;
			}
			case PatternMatcher::StackOp::AttachVariable:
			{
				const char* name_ = patternMatcher.strings().c_str() + oi->arg[ PatternMatcher::StackOp::Variable_name];
				matcherInstance->attachVariable( name_);
				break;
			}
		}
	}
}
private:
static std::string termSymbolKey( unsigned int termid, const std::string& name)
{
	char termidbuf[ 16];
	std::size_t termidsize = strus::utf8encode( termidbuf, termid+1);
	std::string symkey( termidbuf, termidsize);
	symkey.append( name);
	return symkey;
}
public:
static void loadPatterMatcherFromFile(
	strus::Reference<strus::PatternMatcherInstanceInterface>& matcherInstance, 
	strus::Reference<strus::PatternTermFeederInstanceInterface>& feederInstance, 
	const strus::TextProcessorInterface* textproc,
	const std::string& patternMatcherModule,
	const std::string& filename,
	strus::ErrorBufferInterface* errorhnd)
{
	const strus::PatternMatcherInterface* matcher = textproc->getPatternMatcher( patternMatcherModule);
	if (!matcher) throw strus::runtime_error(_TXT("failed to load matcher module: %s"), errorhnd->fetchError());
	const strus::PatternTermFeederInterface* feeder = textproc->getPatternTermFeeder();
	matcherInstance.reset( matcher->createInstance());
	feederInstance.reset( feeder->createInstance());
	if (!feederInstance.get()) throw strus::runtime_error(_TXT("failed to create pattern feeder: %s"), errorhnd->fetchError());
	if (!matcherInstance.get()) throw strus::runtime_error(_TXT("failed to create pattern matcher: %s"), errorhnd->fetchError());
	std::string filepath( textproc->getResourcePath( filename));
	if (filepath.empty())
	{
		throw strus::runtime_error(_TXT("path of file '%s' not found: %s"), filename.c_str(), errorhnd->fetchError());
	}
	std::string content;
	unsigned int ec = strus::readFile( filepath, content);
	if (ec) throw strus::runtime_error(_TXT("failed to read serialized patterns from file '%s': %s"), filepath.c_str(), ::strerror(ec));
	if (!strus::loadPatternMatcherFromSerialization( content, feederInstance.get(), matcherInstance.get(), errorhnd))
	{
		throw strus::runtime_error(_TXT("failed to load pattern matcher from serialization: %s"), errorhnd->fetchError());
	}
}
};

void DocumentAnalyzer::definePatternMatcherPostProc(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const PatternMatcher& patterns)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	strus::Reference<strus::PatternMatcherInstanceInterface> matcherInstance;
	strus::Reference<strus::PatternTermFeederInstanceInterface> feederInstance;
	PatternMatchLoader::loadPatterMatcher( matcherInstance, feederInstance, textproc, patternMatcherModule, patterns, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, matcherInstance.get(), feederInstance.get());
	matcherInstance.release();
	feederInstance.release();
}

void DocumentAnalyzer::definePatternMatcherPostProcFromFile(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const std::string& serializedPatternFile)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	strus::Reference<strus::PatternMatcherInstanceInterface> matcherInstance;
	strus::Reference<strus::PatternTermFeederInstanceInterface> feederInstance;
	PatternMatchLoader::loadPatterMatcherFromFile(
		matcherInstance, feederInstance, 
		textproc, patternMatcherModule, serializedPatternFile, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, matcherInstance.get(), feederInstance.get());
	matcherInstance.release();
	feederInstance.release();
}

void DocumentAnalyzer::defineDocument(
	const std::string& subDocumentTypeName,
	const std::string& selectexpr)
{
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	THIS->defineSubDocument( subDocumentTypeName, selectexpr);
}

static Document mapDocument( const strus::analyzer::Document& doc)
{
	Document rt;
	std::vector<strus::analyzer::Attribute>::const_iterator
		ai = doc.attributes().begin(), ae = doc.attributes().end();
	for (; ai != ae; ++ai)
	{
		if (ai->name() == strus::Constants::attribute_docid())
		{
			rt.setDocid( ai->value());
		}
		else
		{
			rt.setAttribute( ai->name(), ai->value());
		}
	}
	std::vector<strus::analyzer::MetaData>::const_iterator
		mi = doc.metadata().begin(), me = doc.metadata().end();
	for (; mi != me; ++mi)
	{
		Variant val = getNumericVariantFromDouble( mi->value());
		rt.setMetaData( mi->name(), val);
	}
	std::vector<strus::analyzer::Term>::const_iterator
		ti = doc.searchIndexTerms().begin(), te = doc.searchIndexTerms().end();
	for (; ti != te; ++ti)
	{
		rt.addSearchIndexTerm( ti->type(), ti->value(), ti->pos());
	}
	std::vector<strus::analyzer::Term>::const_iterator
		fi = doc.forwardIndexTerms().begin(), fe = doc.forwardIndexTerms().end();
	for (; fi != fe; ++fi)
	{
		rt.addForwardIndexTerm( fi->type(), fi->value(), fi->pos());
	}
	return rt;
}

static Document analyzeDocument( strus::DocumentAnalyzerInterface* THIS, const std::string& content, const strus::analyzer::DocumentClass& dclass, strus::ErrorBufferInterface* errorhnd)
{
	strus::analyzer::Document doc = THIS->analyze( content, dclass);
	return mapDocument( doc);
}

Document DocumentAnalyzer::analyze( const std::string& content)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	strus::analyzer::DocumentClass dclass;

	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
	if (!textproc->detectDocumentClass( dclass, content.c_str(), content.size()))
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( _TXT( "failed to detect document class of document to analyze (%s)"), errorhnd->fetchError());
		}
		else
		{
			throw strus::runtime_error( _TXT( "could not detect document class of document to analyze"));
		}
	}
	Document rt( analyzeDocument( THIS, content, dclass, errorhnd));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT( "failed to analyze document (%s)"), errorhnd->fetchError());
	}
	return rt;
}

Document DocumentAnalyzer::analyze( const std::string& content, const DocumentClass& dclass)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::DocumentAnalyzerInterface* THIS = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	strus::analyzer::DocumentClass documentClass( dclass.mimeType(), dclass.encoding(), dclass.scheme());

	Document rt( analyzeDocument( THIS, content, documentClass, errorhnd));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT( "failed to analyze document (%s)"), errorhnd->fetchError());
	}
	return rt;
}

DocumentAnalyzeQueue DocumentAnalyzer::createQueue() const
{
	return DocumentAnalyzeQueue( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_analyzer_impl, m_textproc);
}

DocumentAnalyzeQueue::DocumentAnalyzeQueue( const DocumentAnalyzeQueue& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
	,m_result_queue(o.m_result_queue)
	,m_result_queue_idx(o.m_result_queue_idx)
	,m_analyzerctx_queue(o.m_analyzerctx_queue)
	,m_analyzerctx_queue_idx(o.m_analyzerctx_queue_idx)
	,m_textproc(o.m_textproc)
{}

DocumentAnalyzeQueue::DocumentAnalyzeQueue( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd, const Reference& analyzer, const void* textproc_)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(analyzer)
	,m_result_queue()
	,m_result_queue_idx(0)
	,m_analyzerctx_queue()
	,m_analyzerctx_queue_idx(0)
	,m_textproc(textproc_)
{}

void DocumentAnalyzeQueue::push( const std::string& content)
{
	const strus::TextProcessorInterface* textproc = (const strus::TextProcessorInterface*)m_textproc;
	strus::analyzer::DocumentClass dclass;
	if (!textproc->detectDocumentClass( dclass, content.c_str(), content.size()))
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to detect document class: %s"), errorhnd->fetchError());
	}
	strus::DocumentAnalyzerInterface* analyzer = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	Reference analyzerContext_impl( ReferenceDeleter<strus::DocumentAnalyzerContextInterface>::function);
	strus::DocumentAnalyzerContextInterface* analyzerContext;
	analyzerContext_impl.reset( analyzerContext = analyzer->createContext( dclass));
	analyzerContext->putInput( content.c_str(), content.size(), true);
	m_analyzerctx_queue.push_back( analyzerContext_impl);
	analyzeNext();
}

void DocumentAnalyzeQueue::analyzeNext()
{
	if (m_result_queue_idx == m_result_queue.size())
	{
		m_result_queue.clear();
		while (m_analyzerctx_queue_idx < m_analyzerctx_queue.size())
		{
			strus::DocumentAnalyzerContextInterface* analyzerContext = (strus::DocumentAnalyzerContextInterface*)m_analyzerctx_queue[ m_analyzerctx_queue_idx].get();
			strus::analyzer::Document doc;
			if (analyzerContext->analyzeNext( doc))
			{
				m_result_queue.push_back( mapDocument( doc));
			}
			else
			{
				strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
				if (errorhnd->hasError())
				{
					throw strus::runtime_error( _TXT( "failed to analyze document (%s)"), errorhnd->fetchError());
				}
				++m_analyzerctx_queue_idx;
			}
		}
	}
}

void DocumentAnalyzeQueue::push( const std::string& content, const DocumentClass& dclass_)
{
	strus::analyzer::DocumentClass dclass( dclass_.mimeType(), dclass_.encoding(), dclass_.scheme());
	strus::DocumentAnalyzerInterface* analyzer = (strus::DocumentAnalyzerInterface*)m_analyzer_impl.get();
	Reference analyzerContext_impl( ReferenceDeleter<strus::DocumentAnalyzerContextInterface>::function);
	strus::DocumentAnalyzerContextInterface* analyzerContext;
	analyzerContext_impl.reset( analyzerContext = analyzer->createContext( dclass));
	analyzerContext->putInput( content.c_str(), content.size(), true);
	m_analyzerctx_queue.push_back( analyzerContext_impl);
	analyzeNext();
}

bool DocumentAnalyzeQueue::hasMore() const
{
	return (m_result_queue_idx < m_result_queue.size());
}

Document DocumentAnalyzeQueue::fetch()
{
	if (m_result_queue_idx < m_result_queue.size())
	{
		Document rt( m_result_queue[ m_result_queue_idx++]);
		analyzeNext();
		return rt;
	}
	else
	{
		m_result_queue.clear();
		analyzeNext();
		if (m_result_queue_idx == m_result_queue.size())
		{
			throw strus::runtime_error( _TXT("no results to fetch from query analyzer queue"));
		}
		Document rt( m_result_queue[ m_result_queue_idx++]);
		analyzeNext();
		return rt;
	}
}

void PatternMatcher::pushTerm( const std::string& type_, const std::string& value_)
{
	StackOp op( StackOp::PushTerm, allocid( type_), allocid( value_));
	m_ops.push_back( op);
	m_size += 1;
}

void PatternMatcher::pushPattern( const std::string& name_)
{
	StackOp op( StackOp::PushPattern, allocid( name_));
	m_ops.push_back( op);
	m_size += 1;
}

void PatternMatcher::pushExpression( const std::string& opname_, unsigned int argc_, int range_, unsigned int cardinality_)
{
	if (argc_ > (unsigned int)m_size)
	{
		throw strus::runtime_error( _TXT("illegal operation on stack of expression (%u > %u)"), argc_, (unsigned int)m_size);
	}
	StackOp op( StackOp::PushExpression, allocid( opname_), argc_, range_, cardinality_);
	m_ops.push_back( op);
	m_size -= argc_;
	m_size += 1;
}

void PatternMatcher::definePattern( const std::string& name_, bool visible_)
{
	StackOp op( StackOp::DefinePattern, allocid( name_), visible_?1:0);
	m_ops.push_back( op);
	m_size += 1;
}

void PatternMatcher::attachVariable( const std::string& name_)
{
	StackOp op( StackOp::AttachVariable, allocid( name_));
	m_ops.push_back( op);
	m_size += 1;
}

void PatternMatcher::add( const PatternMatcher& o)
{
	std::size_t strinc = m_strings.size();
	m_strings.append( o.m_strings);
	std::vector<StackOp>::const_iterator si = o.m_ops.begin(), se = o.m_ops.end();
	for (; si != se; ++si)
	{
		StackOp op(*si);
		switch (op.type)
		{
			case StackOp::PushTerm:
				op.arg[ StackOp::Term_type] += strinc;
				op.arg[ StackOp::Term_value] += strinc;
				break;
			case StackOp::PushPattern:
				op.arg[ StackOp::Pattern_name] += strinc;
				break;
			case StackOp::PushExpression:
				op.arg[ StackOp::Expression_opname] += strinc;
				break;
			case StackOp::DefinePattern:
				op.arg[ StackOp::Pattern_name] += strinc;
				break;
			case StackOp::AttachVariable:
				op.arg[ StackOp::Variable_name] += strinc;
				break;
		}
		m_ops.push_back( op);
	}
	m_size += o.m_size;
}

std::size_t PatternMatcher::allocid( const std::string& str)
{
	std::size_t rt = m_strings.size()+1;
	m_strings.push_back('\0');
	m_strings.append( str);
	return rt;
}

QueryAnalyzer::QueryAnalyzer( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(ReferenceDeleter<strus::QueryAnalyzerInterface>::function)
{
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	m_analyzer_impl.reset( objBuilder->createQueryAnalyzer());
	if (!m_analyzer_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to create query analyzer: %s"), errorhnd->fetchError());
	}
}

QueryAnalyzer::QueryAnalyzer( const QueryAnalyzer& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
{}

QueryAnalyzeContext QueryAnalyzer::createContext() const
{
	return QueryAnalyzeContext( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_analyzer_impl);
}

void QueryAnalyzer::addSearchIndexElement(
		const std::string& featureType,
		const std::string& fieldType,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->addSearchIndexElement(
		featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzer::addSearchIndexElementFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, normalizers, errorhnd);

	THIS->addSearchIndexElementFromPatternMatch( type, patternTypeName, funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzer::addPatternLexem(
		const std::string& featureType,
		const std::string& fieldType,
		const Tokenizer& tokenizer,
		const std::vector<Normalizer>& normalizers)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	FeatureFuncDef funcdef( m_objbuilder_impl, tokenizer, normalizers, errorhnd);

	THIS->addPatternLexem( featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void QueryAnalyzer::definePatternMatcherPostProc(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const PatternMatcher& patterns)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	strus::Reference<strus::PatternMatcherInstanceInterface> matcherInstance;
	strus::Reference<strus::PatternTermFeederInstanceInterface> feederInstance;
	PatternMatchLoader::loadPatterMatcher( matcherInstance, feederInstance, textproc, patternMatcherModule, patterns, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, matcherInstance.get(), feederInstance.get());
	matcherInstance.release();
	feederInstance.release();
}

void QueryAnalyzer::definePatternMatcherPostProcFromFile(
		const std::string& patternTypeName,
		const std::string& patternMatcherModule,
		const std::string& serializedPatternFile)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	strus::Reference<strus::PatternMatcherInstanceInterface> matcherInstance;
	strus::Reference<strus::PatternTermFeederInstanceInterface> feederInstance;
	PatternMatchLoader::loadPatterMatcherFromFile(
		matcherInstance, feederInstance, 
		textproc, patternMatcherModule, serializedPatternFile, errorhnd);
	THIS->definePatternMatcherPostProc( patternTypeName, matcherInstance.get(), feederInstance.get());
	matcherInstance.release();
	feederInstance.release();
}

std::vector<Term> QueryAnalyzer::analyzeField(
		const std::string& fieldType,
		const std::string& fieldContent)
{
	std::vector<Term> rt;
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerInterface* THIS = (strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	std::auto_ptr<strus::QueryAnalyzerContextInterface> anactx( THIS->createContext());
	if (!anactx.get()) throw strus::runtime_error( _TXT("failed to create query analyzer context: %s"), errorhnd->fetchError());
	anactx->putField( 1/*field no*/, fieldType, fieldContent);
	strus::analyzer::Query qry = anactx->analyze();
	if (qry.instructions().empty() && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error in analyze query field: %s"), errorhnd->fetchError());
	}
	std::vector<strus::analyzer::Query::Instruction>::const_iterator
		ei = qry.instructions().begin(), ee = qry.instructions().end();
	for (; ei != ee; ++ei)
	{
		switch (ei->opCode())
		{
			case strus::analyzer::Query::Instruction::MetaData:
			{
				const strus::analyzer::MetaData& md = qry.metadata( ei->idx());
				rt.push_back( Term( md.name(), md.value().tostring().c_str(), 0, 1/*length*/));
				break;
			}
			case strus::analyzer::Query::Instruction::Term:
			{
				const strus::analyzer::Term& term = qry.term( ei->idx());
				rt.push_back( Term( term.type(), term.value(), term.pos(), term.len()));
				break;
			}
			case strus::analyzer::Query::Instruction::Operator:
			{
				throw strus::runtime_error(_TXT("no operator expected in result of analysis of a query field"));
			}
		}
	}
	std::sort( rt.begin(), rt.end());
	return rt;
}

QueryAnalyzeContext::QueryAnalyzeContext( const QueryAnalyzeContext& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_analyzer_impl(o.m_analyzer_impl)
	,m_analyzer_ctx_impl(o.m_analyzer_ctx_impl)
{}

QueryAnalyzeContext::QueryAnalyzeContext( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd, const Reference& analyzer)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl(analyzer)
	,m_analyzer_ctx_impl(ReferenceDeleter<strus::QueryAnalyzerContextInterface>::function)
{
	const strus::QueryAnalyzerInterface* qai = (const strus::QueryAnalyzerInterface*)m_analyzer_impl.get();
	m_analyzer_ctx_impl.reset( qai->createContext());
	if (!m_analyzer_ctx_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to create query analyzer context: %s"), errorhnd->fetchError());
	}
}

void QueryAnalyzeContext::putField(
		unsigned int fieldNo, 
		const std::string& fieldType,
		const std::string& fieldContent)
{
	strus::QueryAnalyzerContextInterface* anactx = (strus::QueryAnalyzerContextInterface*)m_analyzer_ctx_impl.get();
	anactx->putField( fieldNo, fieldType, fieldContent);
	std::vector<unsigned int> fieldnoList( &fieldNo, (&fieldNo)+1);
	anactx->groupElements( fieldNo, fieldnoList, strus::QueryAnalyzerContextInterface::GroupAll, true/*groupSingle*/);
}

std::vector<QueryTerm> QueryAnalyzeContext::analyze()
{
	std::vector<QueryTerm> rt;
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryAnalyzerContextInterface* anactx = (strus::QueryAnalyzerContextInterface*)m_analyzer_ctx_impl.get();
	strus::analyzer::Query qry = anactx->analyze();
	if (qry.instructions().empty() && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error in analyze query: %s"), errorhnd->fetchError());
	}
	std::vector<strus::analyzer::Query::Instruction>::const_iterator
		ei = qry.instructions().begin(), ee = qry.instructions().end();
	for (; ei != ee; ++ei)
	{
		switch (ei->opCode())
		{
			case strus::analyzer::Query::Instruction::MetaData:
			{
				const strus::analyzer::MetaData& md = qry.metadata( ei->idx());
				rt.push_back( QueryTerm( 0, md.name(), md.value().tostring().c_str(), 0, 1));
				break;
			}
			case strus::analyzer::Query::Instruction::Term:
			{
				const strus::analyzer::Term& term = qry.term( ei->idx());
				rt.push_back( QueryTerm( 0, term.type(), term.value(), term.pos(), term.len()));
				break;
			}
			case strus::analyzer::Query::Instruction::Operator:
			{
				if (rt.size() < ei->nofOperands()) throw strus::runtime_error(_TXT("internal: corrupt query structure"));
				std::size_t idx = rt.size() - ei->nofOperands();
				for (; idx < rt.size(); ++idx)
				{
					rt[ idx].m_field = ei->idx();
				}
			}
		}
	}
	return rt;
}


StorageClient::StorageClient( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_storage_impl(ReferenceDeleter<strus::StorageClientInterface>::function)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();

	m_storage_impl.reset( strus::createStorageClient( objBuilder, errorhnd, config_));
	if (!m_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create storage client: %s"), errorhnd->fetchError());
	}
}

StorageClient::StorageClient( const StorageClient& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
{}

Index StorageClient::nofDocumentsInserted() const
{
	strus::StorageClientInterface* THIS = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->nofDocumentsInserted();
}

StorageTransaction StorageClient::createTransaction() const
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return StorageTransaction( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl);
}

StatisticsIterator StorageClient::createInitStatisticsIterator( bool sign) const
{
	strus::StorageClientInterface* storage = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	Reference iter( ReferenceDeleter<strus::StatisticsIteratorInterface>::function);
	iter.reset( storage->createAllStatisticsIterator( sign));
	if (!iter.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return StatisticsIterator( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter);
}

StatisticsIterator StorageClient::createUpdateStatisticsIterator() const
{
	strus::StorageClientInterface* storage = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	Reference iter( ReferenceDeleter<strus::StatisticsIteratorInterface>::function);
	iter.reset( storage->createChangeStatisticsIterator());
	if (!iter.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return StatisticsIterator( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter);
}

DocumentBrowser StorageClient::createDocumentBrowser()
{
	if (!m_objbuilder_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return DocumentBrowser( m_objbuilder_impl, m_trace_impl, m_storage_impl, m_errorhnd_impl);
}

void StorageClient::close()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	bool preverr = errorhnd->hasError();
	m_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcher::VectorStorageSearcher( const Reference& storageref, const Reference& trace, const Index& range_from, const Index& range_to, const Reference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_searcher_impl(ReferenceDeleter<strus::VectorStorageSearchInterface>::function)
	,m_trace_impl( trace)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)storageref.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	m_searcher_impl.reset( storage->createSearcher( range_from, range_to));
	if (!m_searcher_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage searcher: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcher::VectorStorageSearcher( const VectorStorageSearcher& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_searcher_impl(o.m_searcher_impl)
	,m_trace_impl(o.m_trace_impl)
{}

std::vector<VecRank> VectorStorageSearcher::findSimilar( const std::vector<double>& vec, unsigned int maxNofResults) const
{
	strus::VectorStorageSearchInterface* searcher = (strus::VectorStorageSearchInterface*)m_searcher_impl.get();
	if (!searcher) throw strus::runtime_error( _TXT("calling vector storage searcher method after close"));

	std::vector<strus::VectorStorageSearchInterface::Result> res = searcher->findSimilar( vec, maxNofResults);
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	std::vector<VecRank> rt;
	std::vector<strus::VectorStorageSearchInterface::Result>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( VecRank( ri->featidx(), ri->weight()));
	}
	return rt;
}

std::vector<VecRank> VectorStorageSearcher::findSimilarFromSelection( const IndexVector& featidxlist, const FloatVector& vec, unsigned int maxNofResults) const
{
	strus::VectorStorageSearchInterface* searcher = (strus::VectorStorageSearchInterface*)m_searcher_impl.get();
	if (!searcher) throw strus::runtime_error( _TXT("calling vector storage searcher method after close"));

	std::vector<strus::VectorStorageSearchInterface::Result> res = searcher->findSimilarFromSelection( featidxlist, vec, maxNofResults);
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	std::vector<VecRank> rt;
	std::vector<strus::VectorStorageSearchInterface::Result>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( VecRank( ri->featidx(), ri->weight()));
	}
	return rt;
}

void VectorStorageSearcher::close()
{
	if (!m_searcher_impl.get()) throw strus::runtime_error( _TXT("calling storage searcher method after close"));
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	bool preverr = errorhnd->hasError();
	m_searcher_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage searcher close: %s"), errorhnd->fetchError());
	}
}

VectorStorageClient::VectorStorageClient( const VectorStorageClient& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_vector_storage_impl(o.m_vector_storage_impl)
	,m_config(o.m_config)
{}

void VectorStorageClient::close()
{
	if (!m_vector_storage_impl.get()) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	bool preverr = errorhnd->hasError();
	m_vector_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcher VectorStorageClient::createSearcher( const Index& range_from, const Index& range_to) const
{
	return VectorStorageSearcher( m_vector_storage_impl, m_trace_impl, range_from, range_to, m_errorhnd_impl);
}

VectorStorageTransaction VectorStorageClient::createTransaction()
{
	return VectorStorageTransaction( m_objbuilder_impl, m_vector_storage_impl, m_trace_impl, m_errorhnd_impl, m_config);
}

std::vector<std::string> VectorStorageClient::conceptClassNames() const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	std::vector<std::string> rt = storage->conceptClassNames();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept class names: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::vector<Index> VectorStorageClient::conceptFeatures( const std::string& conceptClass, const Index& conceptid) const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	std::vector<strus::Index> res = storage->conceptFeatures( conceptClass, conceptid);
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept features: %s"), errorhnd->fetchError());
	}
	std::vector<Index> rt;
	std::vector<strus::Index>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( *ri);
	}
	return rt;
}

unsigned int VectorStorageClient::nofConcepts( const std::string& conceptClass) const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofConcepts( conceptClass);
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of concepts: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::vector<Index> VectorStorageClient::featureConcepts( const std::string& conceptClass, const Index& index) const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::vector<strus::Index> res = storage->featureConcepts( conceptClass, index);
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature concepts: %s"), errorhnd->fetchError());
	}
	std::vector<Index> rt;
	std::vector<strus::Index>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( *ri);
	}
	return rt;
}

std::vector<double> VectorStorageClient::featureVector( const Index& index) const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::vector<double> rt = storage->featureVector( index);
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature vector of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClient::featureName( const Index& index) const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::string rt = storage->featureName( index);
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

Index VectorStorageClient::featureIndex( const std::string& name) const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	Index rt = storage->featureIndex( name);
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %s: %s"), name.c_str(), errorhnd->fetchError());
	}
	return rt;
}

unsigned int VectorStorageClient::nofFeatures() const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofFeatures();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of features defined: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClient::config() const
{
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::string rt = storage->config();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the storage configuration string: %s"), errorhnd->fetchError());
	}
	return rt;
}

VectorStorageClient::VectorStorageClient( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_vector_storage_impl(ReferenceDeleter<strus::VectorStorageClientInterface>::function)
	,m_config(config)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();

	m_vector_storage_impl.reset( strus::createVectorStorageClient( objBuilder, errorhnd, config));
	if (!m_vector_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage client: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransaction::VectorStorageTransaction( const VectorStorageTransaction& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_vector_storage_impl(o.m_vector_storage_impl)
	,m_vector_transaction_impl(o.m_vector_transaction_impl)
{}

void VectorStorageTransaction::addFeature( const std::string& name, const std::vector<double>& vec)
{
	strus::VectorStorageTransactionInterface* transaction = (strus::VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (!transaction)
	{
		strus::StorageClientInterface* storage = (strus::StorageClientInterface*)m_vector_storage_impl.get();
		m_vector_transaction_impl.reset( (strus::StorageTransactionInterface*)storage->createTransaction());
		if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
		transaction = (strus::VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	}
	transaction->addFeature( name, vec);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

void VectorStorageTransaction::defineFeatureConceptRelation( const std::string& relationTypeName, const Index& featidx, const Index& conidx)
{
	strus::VectorStorageTransactionInterface* transaction = (strus::VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (!transaction)
	{
		strus::StorageClientInterface* storage = (strus::StorageClientInterface*)m_vector_storage_impl.get();
		m_vector_transaction_impl.reset( (strus::StorageTransactionInterface*)storage->createTransaction());
		if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
		transaction = (strus::VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	}
	transaction->defineFeatureConceptRelation( relationTypeName, featidx, conidx);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

bool VectorStorageTransaction::commit()
{
	strus::VectorStorageTransactionInterface* transaction = (strus::VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));

	bool rt = transaction->commit();
	if (!rt)
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT("failed to complete vector storage building (done): %s"), errorhnd->fetchError());
		}
	}
	return rt;
}

void VectorStorageTransaction::rollback()
{
	strus::VectorStorageTransactionInterface* transaction = (strus::VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));
	transaction->rollback();
}

void VectorStorageTransaction::close()
{
	if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	bool preverr = errorhnd->hasError();
	m_vector_transaction_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransaction::VectorStorageTransaction( const Reference& objbuilder, const Reference& storageref, const Reference& trace, const Reference& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_vector_storage_impl(storageref)
	,m_vector_transaction_impl(ReferenceDeleter<strus::VectorStorageTransactionInterface>::function)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::VectorStorageClientInterface* storage = (strus::VectorStorageClientInterface*)m_vector_storage_impl.get();

	m_vector_transaction_impl.reset( storage->createTransaction());
	if (!m_vector_transaction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage transaction: %s"), errorhnd->fetchError());
	}
}

StorageTransaction::StorageTransaction( const Reference& objbuilder_, const Reference& trace_, const Reference& errorhnd_, const Reference& storage_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_storage_impl(storage_)
	,m_transaction_impl(ReferenceDeleter<strus::StorageTransactionInterface>::function)
{}

void StorageTransaction::insertDocument( const std::string& docid, const Document& doc)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::StorageClientInterface* storage = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)storage->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	std::auto_ptr<strus::StorageDocumentInterface> document( transaction->createDocument( docid));
	if (!document.get()) throw strus::runtime_error( _TXT("failed to create document with id '%s' to insert: %s"), docid.c_str(), errorhnd->fetchError());

	std::vector<Attribute>::const_iterator
		ai = doc.attributes().begin(), ae = doc.attributes().end();
	for (; ai != ae; ++ai)
	{
		document->setAttribute( ai->name(), ai->value());
	}
	std::vector<MetaData>::const_iterator
		mi = doc.metaData().begin(), me = doc.metaData().end();
	for (; mi != me; ++mi)
	{
		document->setMetaData( mi->name(), numericVariant( mi->value()));
	}
	std::vector<Term>::const_iterator
		ti = doc.searchIndexTerms().begin(), te = doc.searchIndexTerms().end();
	for (; ti != te; ++ti)
	{
		document->addSearchIndexTerm( ti->type(), ti->value(), ti->position());
	}
	std::vector<Term>::const_iterator
		fi = doc.forwardIndexTerms().begin(), fe = doc.forwardIndexTerms().end();
	for (; fi != fe; ++fi)
	{
		document->addForwardIndexTerm( fi->type(), fi->value(), fi->position());
	}
	std::vector<std::string>::const_iterator
		ui = doc.users().begin(), ue = doc.users().end();
	for (; ui != ue; ++ui)
	{
		document->setUserAccessRight( *ui);
	}
	document->done();
}

void StorageTransaction::deleteDocument( const std::string& docId)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::StorageClientInterface* storage = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)storage->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for deleting document: %s"), errorhnd->fetchError());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteDocument( docId);
}

void StorageTransaction::deleteUserAccessRights( const std::string& username)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::StorageClientInterface* storage = (strus::StorageClientInterface*)m_storage_impl.get();
	if (!m_transaction_impl.get())
	{
		m_transaction_impl.reset( (strus::StorageTransactionInterface*)storage->createTransaction());
		if (!m_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for deleting user access rights: %s"), errorhnd->fetchError());
	}
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	transaction->deleteUserAccessRights( username);
}

void StorageTransaction::commit()
{
	strus::StorageTransactionInterface* transaction = (strus::StorageTransactionInterface*)m_transaction_impl.get();
	if (transaction)
	{
		if (!transaction->commit())
		{
			m_transaction_impl.reset();
			strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
			throw strus::runtime_error( _TXT("error flushing storage operations: %s"), errorhnd->fetchError());
		}
		m_transaction_impl.reset();
	}
}

void StorageTransaction::rollback()
{
	m_transaction_impl.reset();
}

StatisticsIterator::StatisticsIterator( const StatisticsIterator& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_iter_impl(o.m_iter_impl){}

StatisticsIterator::StatisticsIterator( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd_, const Reference& storage_, const Reference& iter_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_storage_impl(storage_)
	,m_iter_impl(iter_)
{}

std::string StatisticsIterator::getNext()
{
	strus::StatisticsIteratorInterface* iter = (strus::StatisticsIteratorInterface*)m_iter_impl.get();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const char* outmsg;
	std::size_t outmsgsize;
	if (!iter->getNext( outmsg, outmsgsize))
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( _TXT("error fetching statistics message: %s"), errorhnd->fetchError());
		}
	}
	return std::string( outmsg, outmsgsize);
}

StatisticsProcessor::StatisticsProcessor( const Reference& objbuilder_, const Reference& trace_, const std::string& name_, const Reference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_statsproc(0)
{
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	m_statsproc = objBuilder->getStatisticsProcessor( name_);
	if (!m_statsproc)
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT( "error getting statistics message processor: %s"), errorhnd->fetchError());
		}
		throw strus::runtime_error(_TXT( "error statistics message processor not defined"));
	}
}

StatisticsMessage StatisticsProcessor::decode( const std::string& blob) const
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::StatisticsProcessorInterface* proc = (const strus::StatisticsProcessorInterface*)m_statsproc;
	std::auto_ptr<strus::StatisticsViewerInterface> viewer( proc->createViewer( blob.c_str(), blob.size()));
	std::vector<DocumentFrequencyChange> dflist;
	strus::StatisticsViewerInterface::DocumentFrequencyChange rec;
	while (viewer->nextDfChange( rec))
	{
		dflist.push_back( DocumentFrequencyChange( rec.type(), rec.value(), rec.increment()));
	}
	int nofdocs = viewer->nofDocumentsInsertedChange();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "error statistics message structure from blob: %s"), errorhnd->fetchError());
	}
	return StatisticsMessage( dflist, nofdocs);
}

std::string StatisticsProcessor::encode( const StatisticsMessage& msg) const
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::StatisticsProcessorInterface* proc = (const strus::StatisticsProcessorInterface*)m_statsproc;
	std::auto_ptr<strus::StatisticsBuilderInterface> builder( proc->createBuilder());
	std::vector<DocumentFrequencyChange>::const_iterator
			dfi = msg.documentFrequencyChangeList().begin(),
			dfe = msg.documentFrequencyChangeList().end();
	for (; dfi != dfe; ++dfi)
	{
		builder->addDfChange( dfi->type().c_str(), dfi->value().c_str(), dfi->increment());
	}
	builder->setNofDocumentsInsertedChange( msg.nofDocumentsInsertedChange());
	std::string rt;
	const char* blk;
	std::size_t blksize;
	if (builder->fetchMessage( blk, blksize))
	{
		rt.append( blk, blksize);
	}
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "error creating blob from statistics message structure: %s"), errorhnd->fetchError());
	}
	return rt;
}


QueryEval::QueryEval( const Reference& objbuilder, const Reference& trace, const Reference& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_queryeval_impl(ReferenceDeleter<strus::QueryEvalInterface>::function)
{
	const strus::StorageObjectBuilderInterface* objBuilder = (const strus::StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	m_queryproc = objBuilder->getQueryProcessor();
	if (!m_queryproc)
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("error in get query processor: %s"), errorhnd->fetchError());
	}
	m_queryeval_impl.reset( objBuilder->createQueryEval());
	if (!m_queryeval_impl.get())
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error( _TXT("error creating query eval: %s"), errorhnd->fetchError());
	}
}

QueryEval::QueryEval( const QueryEval& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_queryproc(o.m_queryproc)
{}

void QueryEval::addTerm(
		const std::string& set_,
		const std::string& type_,
		const std::string& value_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addTerm( set_, type_, value_);
}

void QueryEval::addSelectionFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addSelectionFeature( set_);
}

void QueryEval::addRestrictionFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addRestrictionFeature( set_);
}

void QueryEval::addExclusionFeature( const std::string& set_)
{
	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	queryeval->addExclusionFeature( set_);
}

void QueryEval::addSummarizer(
		const std::string& name,
		const SummarizerConfig& config,
		const std::string& debugAttributeName)
{
	typedef strus::QueryEvalInterface::FeatureParameter FeatureParameter;

	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::QueryProcessorInterface* queryproc = (const strus::QueryProcessorInterface*)m_queryproc;

	const strus::SummarizerFunctionInterface* sf = queryproc->getSummarizerFunction( name);
	if (!sf) throw strus::runtime_error( _TXT("summarizer function not defined: '%s'"), name.c_str());

	strus::Reference<strus::SummarizerFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating summarizer function: %s"), errorhnd->fetchError());

	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	std::map<std::string,Variant>::const_iterator
		pi = config.m_parameters.begin(), pe = config.m_parameters.end();
	for (; pi != pe; ++pi)
	{
		if (pi->second.m_type == Variant_TEXT)
		{
			function->addStringParameter( pi->first, pi->second.m_value.TEXT);
		}
		else
		{
			function->addNumericParameter( pi->first, numericVariant( pi->second));
		}
	}
	std::vector<FeatureParameter> featureParameters;
	std::map<std::string,std::string>::const_iterator
		fi = config.m_features.begin(), fe = config.m_features.end();
	for (; fi != fe; ++fi)
	{
		featureParameters.push_back( FeatureParameter( fi->first, fi->second));
	}
	std::map<std::string,std::string>::const_iterator
		ri = config.m_resultnamemap.begin(), re = config.m_resultnamemap.end();
	for (; ri != re; ++ri)
	{
		function->defineResultName( ri->first, ri->second);
	}
	queryeval->addSummarizerFunction( name, function.get(), featureParameters, debugAttributeName);
	function.release();
}

void QueryEval::addWeightingFunction(
		const std::string& name,
		const WeightingConfig& config,
		const std::string& debugAttributeName)
{
	typedef strus::QueryEvalInterface::FeatureParameter FeatureParameter;

	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::QueryProcessorInterface* queryproc = (const strus::QueryProcessorInterface*)m_queryproc;

	const strus::WeightingFunctionInterface* sf = queryproc->getWeightingFunction( name);
	if (!sf) throw strus::runtime_error( _TXT("weighting function not defined: '%s'"), name.c_str());

	strus::Reference<strus::WeightingFunctionInstanceInterface> function( sf->createInstance( queryproc));
	if (!function.get()) throw strus::runtime_error( _TXT("error creating weighting function instance '%s': '%s'"), name.c_str(), errorhnd->fetchError());

	strus::QueryEvalInterface* queryeval = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	std::map<std::string,Variant>::const_iterator
		pi = config.m_parameters.begin(), pe = config.m_parameters.end();
	for (; pi != pe; ++pi)
	{
		if (pi->second.type() == Variant_TEXT)
		{
			function->addStringParameter( pi->first, pi->second.getText());
		}
		else
		{
			function->addNumericParameter( pi->first, numericVariant( pi->second));
		}
	}
	std::vector<FeatureParameter> featureParameters;
	std::map<std::string,std::string>::const_iterator
		fi = config.m_features.begin(), fe = config.m_features.end();
	for (; fi != fe; ++fi)
	{
		featureParameters.push_back( FeatureParameter( fi->first, fi->second));
	}
	queryeval->addWeightingFunction( name, function.get(), featureParameters, debugAttributeName);
	function.release();
}

void QueryEval::addWeightingFormula( const std::string& source, const FunctionVariableConfig& defaultParameter)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryEvalInterface* qe = (strus::QueryEvalInterface*)m_queryeval_impl.get();

	const strus::QueryProcessorInterface* queryproc = (const strus::QueryProcessorInterface*)m_queryproc;
	const strus::ScalarFunctionParserInterface* scalarfuncparser = queryproc->getScalarFunctionParser("");
	std::auto_ptr<strus::ScalarFunctionInterface> scalarfunc( scalarfuncparser->createFunction( source, std::vector<std::string>()));
	if (!scalarfunc.get())
	{
		throw strus::runtime_error(_TXT( "failed to create scalar function (weighting formula) from source: %s"), errorhnd->fetchError());
	}
	std::map<std::string,double>::const_iterator
		vi = defaultParameter.m_variables.begin(),
		ve = defaultParameter.m_variables.end();
	for (; vi != ve; ++vi)
	{
		scalarfunc->setDefaultVariableValue( vi->first, vi->second);
	}
	qe->defineWeightingFormula( scalarfunc.get());
	scalarfunc.release();
}

Query QueryEval::createQuery( const StorageClient& storage) const
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryEvalInterface* qe = (strus::QueryEvalInterface*)m_queryeval_impl.get();
	strus::StorageClientInterface* st = (strus::StorageClientInterface*)storage.m_storage_impl.get();
	Reference query( ReferenceDeleter<strus::QueryInterface>::function);
	query.reset( qe->createQuery( st));
	if (!query.get()) throw strus::runtime_error( _TXT("failed to create query object: %s"), errorhnd->fetchError());

	return Query( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, storage.m_storage_impl, m_queryeval_impl, query, m_queryproc);
}


std::size_t QueryExpression::allocid( const std::string& str)
{
	std::size_t rt = m_strings.size()+1;
	m_strings.push_back('\0');
	m_strings.append( str);
	return rt;
}

void QueryExpression::pushTerm( const std::string& type_, const std::string& value_, const Index& length_)
{
	StackOp op( StackOp::PushTerm, allocid( type_), allocid( value_), length_);
	m_ops.push_back( op);
	m_size += 1;
}

void QueryExpression::pushDocField( const std::string& metadata_start_, const std::string& metadata_end_)
{
	StackOp op( StackOp::PushDocField, allocid( metadata_start_), allocid( metadata_end_));
	m_ops.push_back( op);
	m_size += 1;
}

void QueryExpression::pushExpression( const std::string& opname_, unsigned int argc_, int range_, unsigned int cardinality_)
{
	if (argc_ > (unsigned int)m_size)
	{
		throw strus::runtime_error( _TXT("illegal operation on stack of expression (%u > %u)"), argc_, (unsigned int)m_size);
	}
	StackOp op( StackOp::PushExpression, allocid( opname_), argc_, range_, cardinality_);
	m_ops.push_back( op);
	m_size -= argc_;
	m_size += 1;
}

void QueryExpression::attachVariable( const std::string& name_)
{
	StackOp op( StackOp::AttachVariable, allocid( name_));
	m_ops.push_back( op);
}

void QueryExpression::add( const QueryExpression& o)
{
	std::size_t strinc = m_strings.size();
	m_strings.append( o.m_strings);
	std::vector<StackOp>::const_iterator si = o.m_ops.begin(), se = o.m_ops.end();
	for (; si != se; ++si)
	{
		StackOp op(*si);
		switch (op.type)
		{
			case StackOp::PushTerm:
				op.arg[ StackOp::Term_type] += strinc;
				op.arg[ StackOp::Term_value] += strinc;
				break;
			case StackOp::PushDocField:
				op.arg[ StackOp::Term_metastart] += strinc;
				op.arg[ StackOp::Term_metaend] += strinc;
				break;
			case StackOp::PushExpression:
				op.arg[ StackOp::Expression_opname] += strinc;
				break;
			case StackOp::AttachVariable:
				op.arg[ StackOp::Variable_name] += strinc;
				break;
		}
		m_ops.push_back( op);
	}
	m_size += o.m_size;
}

Query::Query( const Query& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_query_impl(o.m_query_impl)
	,m_queryproc(o.m_queryproc)
{}

void Query::defineFeature( const std::string& set_, const QueryExpression& expr_, double weight_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	const strus::QueryProcessorInterface* queryproc = (const strus::QueryProcessorInterface*)m_queryproc;

	if (expr_.size() != 1)
	{
		throw strus::runtime_error( _TXT("the argument expression of defineFeature does not contain exactly one node (%u)"), (unsigned int)expr_.size());
	}
	std::vector<QueryExpression::StackOp>::const_iterator ei = expr_.m_ops.begin(), ee = expr_.m_ops.end();
	for (; ei != ee; ++ei)
	{
		switch (ei->type)
		{
			case QueryExpression::StackOp::PushTerm:
			{
				const char* type_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Term_type];
				const char* value_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Term_value];
				Index length_ = ei->arg[ QueryExpression::StackOp::Term_length];
				THIS->pushTerm( type_, value_, length_);
				break;
			}
			case QueryExpression::StackOp::PushDocField:
			{
				const char* start_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Term_metastart];
				const char* end_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Term_metaend];
				THIS->pushDocField( start_, end_);
				break;
			}
			case QueryExpression::StackOp::PushExpression:
			{
				const char* opname_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Expression_opname];
				unsigned int argc_ = (unsigned int)ei->arg[ QueryExpression::StackOp::Expression_argc];
				int range_ = (int)ei->arg[ QueryExpression::StackOp::Expression_range];
				unsigned int cardinality_ = (unsigned int)ei->arg[ QueryExpression::StackOp::Expression_cardinality];

				const strus::PostingJoinOperatorInterface* joinopr = queryproc->getPostingJoinOperator( opname_);
				if (!joinopr) throw strus::runtime_error( _TXT("posting join operator not defined: '%s'"), opname_);

				THIS->pushExpression( joinopr, argc_, range_, cardinality_);
				break;
			}
			case QueryExpression::StackOp::AttachVariable:
			{
				const char* name_ = expr_.m_strings.c_str() + ei->arg[ QueryExpression::StackOp::Variable_name];
				THIS->attachVariable( name_);
				break;
			}
		}
	}
	THIS->defineFeature( set_, weight_);
}

void Query::defineDocFieldFeature( const std::string& set_, const std::string& metadataStart, const std::string& metadataEnd)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->pushDocField( metadataStart, metadataEnd);
	THIS->defineFeature( set_, 1.0);
}

static strus::MetaDataRestrictionInterface::CompareOperator getCompareOp( const char* compareOp)
{
	strus::MetaDataRestrictionInterface::CompareOperator cmpop;
	if (compareOp[0] == '<')
	{
		if (compareOp[1] == '\0')
		{
			cmpop = strus::MetaDataRestrictionInterface::CompareLess;
		}
		else if (compareOp[1] == '=' && compareOp[2] == '\0')
		{
			cmpop = strus::MetaDataRestrictionInterface::CompareLessEqual;
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
		}
	}
	else if (compareOp[0] == '>')
	{
		if (compareOp[1] == '\0')
		{
			cmpop = strus::MetaDataRestrictionInterface::CompareGreater;
		}
		else if (compareOp[1] == '=' && compareOp[2] == '\0')
		{
			cmpop = strus::MetaDataRestrictionInterface::CompareGreaterEqual;
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
		}
	}
	else if (compareOp[0] == '=' && compareOp[1] == '\0')
	{
		cmpop = strus::MetaDataRestrictionInterface::CompareEqual;
	}
	else if (compareOp[0] == '=' && compareOp[1] == '=' && compareOp[2] == '\0')
	{
		cmpop = strus::MetaDataRestrictionInterface::CompareEqual;
	}
	else if (compareOp[0] == '!' && compareOp[1] == '=' && compareOp[2] == '\0')
	{
		cmpop = strus::MetaDataRestrictionInterface::CompareNotEqual;
	}
	else
	{
		throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
	}
	return cmpop;
}

void Query::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		const Variant& operand, bool newGroup)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	strus::MetaDataRestrictionInterface::CompareOperator cmpop = getCompareOp( compareOp);
	THIS->addMetaDataRestrictionCondition( cmpop, name, numericVariant(operand), newGroup);
}

void Query::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		double value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void Query::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		unsigned int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void Query::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void Query::defineTermStatistics( const std::string& type_, const std::string& value_, const TermStatistics& stats_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	strus::TermStatistics stats;
	stats.setDocumentFrequency( stats_.df());
	THIS->defineTermStatistics( type_, value_, stats);
}

void Query::defineGlobalStatistics( const GlobalStatistics& stats_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	strus::GlobalStatistics stats;
	stats.setNofDocumentsInserted( stats_.nofdocs());
	THIS->defineGlobalStatistics( stats);
}

void Query::addDocumentEvaluationSet(
		const std::vector<Index>& docnolist_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	std::vector<strus::Index> docnolist;
	std::vector<Index>::const_iterator di = docnolist_.begin(), de = docnolist_.end();
	for (; di != de; ++di) docnolist.push_back( *di);
	THIS->addDocumentEvaluationSet( docnolist);
}

void Query::setMaxNofRanks( unsigned int maxNofRanks_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setMaxNofRanks( maxNofRanks_);
}

void Query::setMinRank( unsigned int minRank_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setMinRank( minRank_);
}

void Query::addUserName( const std::string& username_)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->addUserName( username_);
}

void Query::setWeightingVariables(
		const FunctionVariableConfig& parameter)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	std::map<std::string,double>::const_iterator
		vi = parameter.m_variables.begin(),
		ve = parameter.m_variables.end();
	for (; vi != ve; ++vi)
	{
		THIS->setWeightingVariableValue( vi->first, vi->second);
	}
}

void Query::setDebugMode( bool debug)
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	THIS->setDebugMode( debug);
}

QueryResult Query::evaluate() const
{
	std::vector<Rank> ranks;
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	strus::QueryResult res = THIS->evaluate();
	if (res.ranks().empty() && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to evaluate query: %s"), errorhnd->fetchError());
	}
	QueryResult rt( res.evaluationPass(), res.nofRanked(), res.nofVisited());
	std::vector<strus::ResultDocument>::const_iterator
		ri = res.ranks().begin(), re = res.ranks().end();
	for (;ri != re; ++ri)
	{
		Rank reselem;
		reselem.m_docno = (unsigned int)ri->docno();
		reselem.m_weight = ri->weight();
		std::vector<strus::SummaryElement>::const_iterator
			ai = ri->summaryElements().begin(), ae = ri->summaryElements().end();
	
		for (;ai != ae; ++ai)
		{
			SummaryElement elem( ai->name(), ai->value(), ai->weight(), ai->index());
			reselem.m_summaryElements.push_back( elem);
		}
		rt.m_ranks.push_back( reselem);
	}
	return QueryResult( rt);
}

std::string Query::tostring() const
{
	strus::QueryInterface* THIS = (strus::QueryInterface*)m_query_impl.get();
	std::string rt( THIS->tostring());
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to map query to string: %s"), errorhnd->fetchError());
	}
	return rt;
}

DocumentBrowser::DocumentBrowser( const DocumentBrowser& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_restriction_impl(o.m_restriction_impl)
	,m_postingitr_impl(o.m_postingitr_impl)
	,m_attributereader_impl(o.m_attributereader_impl)
	,m_docno(o.m_docno)
{}

DocumentBrowser::DocumentBrowser( const Reference& objbuilder_impl_, const Reference& trace_impl_, const Reference& storage_impl_, const Reference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_impl_)
	,m_objbuilder_impl(objbuilder_impl_)
	,m_storage_impl(storage_impl_)
	,m_restriction_impl( ReferenceDeleter<strus::MetaDataRestrictionInterface>::function)
	,m_postingitr_impl( ReferenceDeleter<strus::PostingIteratorInterface>::function)
	,m_attributereader_impl( ReferenceDeleter<strus::AttributeReaderInterface>::function)
	,m_docno(0)
{
	const strus::StorageClientInterface* storage = (const strus::StorageClientInterface*)m_storage_impl.get();
	m_restriction_impl.reset( storage->createMetaDataRestriction());
	if (!m_restriction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create meta data restriction interface for browsing documents"));
	}
}

void DocumentBrowser::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		const Variant& value, bool newGroup)
{
	strus::MetaDataRestrictionInterface* restriction = (strus::MetaDataRestrictionInterface*)m_restriction_impl.get();
	if (!restriction)
	{
		throw strus::runtime_error( _TXT("it is not allowed to add more restrictions to a document browser after the first call of next()"));
	}
	strus::MetaDataRestrictionInterface::CompareOperator cmpop = getCompareOp( compareOp);
	restriction->addCondition( cmpop, name, numericVariant(value), newGroup);
}

void DocumentBrowser::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		double value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void DocumentBrowser::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		unsigned int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void DocumentBrowser::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

Index DocumentBrowser::skipDoc( const Index& docno_)
{
	if (!m_postingitr_impl.get())
	{
		const strus::StorageClientInterface* storage = (const strus::StorageClientInterface*)m_storage_impl.get();
		strus::MetaDataRestrictionInterface* restriction = (strus::MetaDataRestrictionInterface*)m_restriction_impl.get();
		m_postingitr_impl.reset( storage->createBrowsePostingIterator( restriction, 1));
		if (!m_postingitr_impl.get())
		{
			throw strus::runtime_error( _TXT("failed to create posting iterator for document browser"));
		}
	}
	strus::PostingIteratorInterface* itr = (strus::PostingIteratorInterface*)m_postingitr_impl.get();
	return m_docno = itr->skipDoc( docno_);
}

std::string DocumentBrowser::attribute( const std::string& name)
{
	if (m_docno)
	{
		if (!m_attributereader_impl.get())
		{
			const strus::StorageClientInterface* storage = (const strus::StorageClientInterface*)m_storage_impl.get();
			m_attributereader_impl.reset( storage->createAttributeReader());
			if (!m_attributereader_impl.get())
			{
				throw strus::runtime_error( _TXT("failed to create attribute reader for document browser"));
			}
		}
		const strus::AttributeReaderInterface* reader = (strus::AttributeReaderInterface*)m_attributereader_impl.get();
		strus::Index elemhnd = reader->elementHandle( name.c_str());
		if (!elemhnd)
		{
			throw strus::runtime_error( _TXT("document attribute name %s is not defined"), name.c_str());
		}
		return reader->getValue( elemhnd);
	}
	else
	{
		return std::string();
	}
}


static strus::ErrorBufferInterface* createErrorBuffer_( unsigned int maxNofThreads)
{
	strus::ErrorBufferInterface* errorhnd = strus::createErrorBuffer_standard( 0, maxNofThreads);
	if (!errorhnd)
	{
		throw strus::runtime_error( _TXT("failed to create error buffer object: %s"), errorhnd->fetchError());
	}
	return errorhnd;
}

static strus::ModuleLoaderInterface* createModuleLoader_( strus::ErrorBufferInterface* errorhnd)
{
	strus::ModuleLoaderInterface* rt = strus::createModuleLoader( errorhnd);
	if (!rt)
	{
		throw strus::runtime_error( _TXT("failed to create module loader object: %s"), errorhnd->fetchError());
	}
	return rt;
}

Context::Context()
	:m_errorhnd_impl( ReferenceDeleter<strus::ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<strus::TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	strus::ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.reset( errorhnd);
	strus::ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
}

Context::Context( unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<strus::ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<strus::TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	strus::ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);
	strus::ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
}

Context::Context( unsigned int maxNofThreads, const std::string& tracecfg)
	:m_errorhnd_impl( ReferenceDeleter<strus::ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<strus::TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	strus::ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);
	strus::ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
	if (!tracecfg.empty())
	{
		m_trace_impl.reset( new strus::TraceProxy( moduleLoader, tracecfg, errorhnd));
	}
}

Context::Context( const std::string& connectionstring)
	:m_errorhnd_impl( ReferenceDeleter<strus::ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<strus::TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	strus::ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.reset( errorhnd);

	std::auto_ptr<strus::RpcClientMessagingInterface> messaging;
	messaging.reset( strus::createRpcClientMessaging( connectionstring.c_str(), errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( strus::createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

Context::Context( const std::string& connectionstring, unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<strus::ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<strus::ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<strus::RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<strus::TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<strus::StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	strus::ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);

	std::auto_ptr<strus::RpcClientMessagingInterface> messaging;
	messaging.reset( strus::createRpcClientMessaging( connectionstring.c_str(), errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( strus::createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

Context::Context( const Context& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_moduleloader_impl(o.m_moduleloader_impl)
	,m_rpc_impl(o.m_rpc_impl)
	,m_trace_impl( o.m_trace_impl)
	,m_storage_objbuilder_impl(o.m_storage_objbuilder_impl)
	,m_analyzer_objbuilder_impl(o.m_analyzer_objbuilder_impl)
	,m_textproc(o.m_textproc)
{}

void Context::checkErrors() const
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("unhandled error: %s"), errorhnd->fetchError());
	}
}

void Context::loadModule( const std::string& name_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot load modules in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	if (!moduleLoader->loadModule( name_))
	{
		strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
		throw strus::runtime_error(_TXT("failed to load module: %s"), errorhnd->fetchError());
	}
}

void Context::addModulePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a module path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addModulePath( paths_);
}

void Context::addResourcePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a resource path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addResourcePath( paths_);
}

StatisticsProcessor Context::createStatisticsProcessor( const std::string& name)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StatisticsProcessor( m_storage_objbuilder_impl, m_trace_impl, name, m_errorhnd_impl);
}

void Context::initStorageObjBuilder()
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::StorageObjectBuilderInterface* storageObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		strus::RpcClientInterface* client = (strus::RpcClientInterface*)m_rpc_impl.get();
		storageObjectBuilder = client->createStorageObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
		storageObjectBuilder = moduleLoader->createStorageObjectBuilder();
	}
	else
	{
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
	}
	if (!storageObjectBuilder)
	{
		throw strus::runtime_error(_TXT("failed to create storage object builder: %s"), errorhnd->fetchError());
	}
	strus::TraceProxy* tp = (strus::TraceProxy*)m_trace_impl.get();
	if (tp)
	{
		strus::StorageObjectBuilderInterface* storageObjectBuilder_proxy = tp->createProxy( storageObjectBuilder);
		if (!storageObjectBuilder_proxy)
		{
			delete storageObjectBuilder;
			throw strus::runtime_error(_TXT("failed to create storage object builder trace proxy: %s"), errorhnd->fetchError());
		}
		storageObjectBuilder = storageObjectBuilder_proxy;
	}
	m_storage_objbuilder_impl.reset( storageObjectBuilder);
}

void Context::initAnalyzerObjBuilder()
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	strus::AnalyzerObjectBuilderInterface* analyzerObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		strus::RpcClientInterface* client = (strus::RpcClientInterface*)m_rpc_impl.get();
		analyzerObjectBuilder = client->createAnalyzerObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		strus::ModuleLoaderInterface* moduleLoader = (strus::ModuleLoaderInterface*)m_moduleloader_impl.get();
		analyzerObjectBuilder = moduleLoader->createAnalyzerObjectBuilder();
	}
	else
	{
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
	}
	if (!analyzerObjectBuilder)
	{
		throw strus::runtime_error( _TXT("failed to create analyzer object builder: %s"), errorhnd->fetchError());
	}
	strus::TraceProxy* tp = (strus::TraceProxy*)m_trace_impl.get();
	if (tp)
	{
		strus::AnalyzerObjectBuilderInterface* analyzerObjectBuilder_proxy = tp->createProxy( analyzerObjectBuilder);
		if (!analyzerObjectBuilder_proxy)
		{
			delete analyzerObjectBuilder;
			throw strus::runtime_error(_TXT("failed to create storage object builder trace proxy: %s"), errorhnd->fetchError());
		}
		analyzerObjectBuilder = analyzerObjectBuilder_proxy;
	}
	m_analyzer_objbuilder_impl.reset( analyzerObjectBuilder);
}

DocumentClass Context::detectDocumentClass( const std::string& content)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	const strus::AnalyzerObjectBuilderInterface* objBuilder = (const strus::AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
	const strus::TextProcessorInterface* textproc;
	if (m_textproc)
	{
		textproc = (const strus::TextProcessorInterface*)m_textproc;
	}
	else
	{
		textproc = objBuilder->getTextProcessor();
		m_textproc = textproc;
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
	}
	strus::analyzer::DocumentClass dclass;
	if (textproc->detectDocumentClass( dclass, content.c_str(), content.size()))
	{
		return DocumentClass( dclass.mimeType(), dclass.encoding(), dclass.scheme());
	}
	else
	{
		if (errorhnd->hasError()) throw strus::runtime_error( _TXT("failed to detect document class: %s"), errorhnd->fetchError());
		return DocumentClass();
	}
}

StorageClient Context::createStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, config_);
}

StorageClient Context::createStorageClient()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, std::string());
}

VectorStorageClient Context::createVectorStorageClient()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return VectorStorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, std::string());
}

VectorStorageClient Context::createVectorStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return VectorStorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, config_);
}

DocumentAnalyzer Context::createDocumentAnalyzer( const std::string& segmentername_)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	if (!m_textproc)
	{
		strus::AnalyzerObjectBuilderInterface* objBuilder = (strus::AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
		m_textproc = objBuilder->getTextProcessor();
		if (!m_textproc)
		{
			strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
			throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
		}
	}
	return DocumentAnalyzer( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl, segmentername_, m_textproc);
}

QueryAnalyzer Context::createQueryAnalyzer()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return QueryAnalyzer( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl);
}

QueryEval Context::createQueryEval()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return QueryEval( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl);
}

void Context::createStorage( const std::string& config_)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	std::string dbname;
	std::string storagecfg( config_);
	(void)strus::extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	strus::StorageObjectBuilderInterface* objBuilder = (strus::StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const strus::DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const strus::StorageInterface* sti = objBuilder->getStorage();
	if (!sti) throw strus::runtime_error( _TXT("failed to get storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());
}

void Context::createVectorStorage( const std::string& config_)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	std::string dbname;
	std::string storagename;
	std::string storagecfg( config_);
	(void)strus::extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);
	if (!strus::extractStringFromConfigString( dbname, storagename, "storage", errorhnd))
	{
		storagename = strus::Constants::standard_vector_storage();
	}
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	strus::StorageObjectBuilderInterface* objBuilder = (strus::StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const strus::DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const strus::VectorStorageInterface* sti = objBuilder->getVectorStorage( storagename);
	if (!sti) throw strus::runtime_error( _TXT("failed to get vector storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create vector storage: %s"), errorhnd->fetchError());
}

void Context::destroyStorage( const std::string& config_)
{
	strus::ErrorBufferInterface* errorhnd = (strus::ErrorBufferInterface*)m_errorhnd_impl.get();
	std::string dbname;
	std::string storagecfg( config_);
	(void)strus::extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	strus::StorageObjectBuilderInterface* objBuilder = (strus::StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	if (!objBuilder) throw strus::runtime_error( _TXT("failed to get object builder: %s"), errorhnd->fetchError());
	const strus::DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	if (!dbi->destroyDatabase( storagecfg)) throw strus::runtime_error( _TXT("failed to destroy database: %s"), errorhnd->fetchError());
}

void Context::close()
{
	m_analyzer_objbuilder_impl.reset();
	m_storage_objbuilder_impl.reset();
	if (m_rpc_impl.get()) ((strus::RpcClientInterface*)m_rpc_impl.get())->close();
	m_moduleloader_impl.reset();
}


