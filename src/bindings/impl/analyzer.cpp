/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/analyzer.hpp"
#include "impl/value/analyzerIntrospection.hpp"
#include "impl/value/featureFuncDef.hpp"
#include "impl/value/sentenceTermExpression.hpp"
#include "strus/documentAnalyzerInstanceInterface.hpp"
#include "strus/documentAnalyzerContextInterface.hpp"
#include "strus/queryAnalyzerInstanceInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/constants.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/local_ptr.hpp"
#include "papuga/serialization.h"
#include "papuga/errors.hpp"
#include "papuga/valueVariant.hpp"
#include "papuga/valueVariant.h"
#include "private/internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "callResultUtils.hpp"
#include "structDefs.hpp"
#include "valueVariantWrap.hpp"

using namespace strus;
using namespace strus::bindings;

DocumentAnalyzerImpl::DocumentAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const ValueVariant& doctype, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl()
	,m_textproc(textproc_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();

	SegmenterDef segdef( doctype);
	const SegmenterInterface* segmenter = 0;
	analyzer::SegmenterOptions segmenteropt;

	if (!segdef.segmenter.empty())
	{
		segmenter = m_textproc->getSegmenterByName( segdef.segmenter);
	}
	else
	{
		if (!segdef.mimetype.empty())
		{
			segmenter = m_textproc->getSegmenterByMimeType( segdef.mimetype);
		}
		if (!segdef.scheme.empty())
		{
			segmenteropt = m_textproc->getSegmenterOptions( segdef.scheme);
		}
	}
	if (!segmenter)
	{
		if (errorhnd->hasError())
		{
			throw std::runtime_error( errorhnd->fetchError());
		}
		else
		{
			throw std::runtime_error( _TXT("no segmenter or document class defined"));
		}
	}
	m_analyzer_impl.resetOwnership( objBuilder->createDocumentAnalyzer( segmenter, segmenteropt), "DocumentAnalyzer");
	if (!m_analyzer_impl.get())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

void DocumentAnalyzerImpl::addSearchIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers,
	const ValueVariant& priorityval,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	int priority = Deserializer::getInt( priorityval, 0);
	analyzer->addSearchIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		priority, Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::addForwardIndexFeature(
	const std::string& type,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers,
	const ValueVariant& priorityval,
	const ValueVariant& options)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	int priority = Deserializer::getInt( priorityval, 0);
	analyzer->addForwardIndexFeature(
		type, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers,
		priority, Deserializer::getFeatureOptions( options));
	funcdef.release();
}

void DocumentAnalyzerImpl::addSearchIndexField(
		const std::string& name,
		const std::string& scopeexpr,
		const std::string& selectexpr,
		const ValueVariant& keyexprVal)
{
	std::string keyexpr;
	if (papuga_ValueVariant_defined( &keyexprVal))
	{
		keyexpr = ValueVariantWrap::tostring( keyexprVal);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	analyzer->addSearchIndexField( name, scopeexpr, selectexpr, keyexpr);
	if (errorhnd->hasError()) throw strus::runtime_error(_TXT("error defining search index field: %s"), errorhnd->fetchError());
}

void DocumentAnalyzerImpl::addSearchIndexStructure(
		const std::string& name,
		const std::string& headerFieldName,
		const std::string& contentFieldName,
		const ValueVariant& structureTypeVal)
{
	DocumentAnalyzerInstanceInterface::StructureType structureType;
	char nambuf[ 64];
	const char* structureTypeStr = papuga_ValueVariant_toascii( nambuf, sizeof(nambuf), &structureTypeVal, 0/*nonAsciiSubstChar*/);
	if (!structureTypeStr || !DocumentAnalyzerInstanceInterface::structureTypeFromName( structureType, structureTypeStr))
	{
		throw strus::runtime_error(_TXT("invalid name for structure type, expected one of '%s'"), "cover,label,header,footer");
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	analyzer->addSearchIndexStructure( name, headerFieldName, contentFieldName, structureType);
	if (errorhnd->hasError()) throw strus::runtime_error(_TXT("error defining search index structure: %s"), errorhnd->fetchError());
}

void DocumentAnalyzerImpl::defineMetaData(
	const std::string& fieldname,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

	analyzer->defineMetaData(
		fieldname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzerImpl::defineAggregatedMetaData(
	const std::string& fieldname,
	const ValueVariant& function)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
	if (!textproc) throw strus::runtime_error( "%s", errorhnd->fetchError());

	Reference<AggregatorFunctionInstanceInterface> functioninst( Deserializer::getAggregator( function, textproc, errorhnd));
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

	analyzer->defineAggregatedMetaData(
		fieldname, functioninst.get());
	functioninst.release();
}

void DocumentAnalyzerImpl::defineAttribute(
	const std::string& attribname,
	const std::string& selectexpr,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

	analyzer->defineAttribute(
		attribname, selectexpr, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

void DocumentAnalyzerImpl::defineSubDocument(
	const std::string& subDocumentTypeName,
	const std::string& selectexpr)
{
	DocumentAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	THIS->defineSubDocument( subDocumentTypeName, selectexpr);
}

void DocumentAnalyzerImpl::defineSubContent(
		const std::string& selectexpr,
		const ValueVariant& dclass)
{
	analyzer::DocumentClass documentClass = Deserializer::getDocumentClass( dclass);
	DocumentAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();
	THIS->defineSubContent( selectexpr, documentClass);
}

static analyzer::Document* analyzeDoc( const DocumentAnalyzerInstanceInterface* THIS, const std::string& content, const analyzer::DocumentClass& dclass, ErrorBufferInterface* errorhnd)
{
	Reference<analyzer::Document> doc( new analyzer::Document( THIS->analyze( content, dclass)));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return doc.release();
}

analyzer::DocumentClass DocumentAnalyzerImpl::getDocumentClass( const std::string& content, const ValueVariant& dclass) const
{
	if (papuga_ValueVariant_defined( &dclass))
	{
		return Deserializer::getDocumentClass( dclass);
	}
	else
	{
		analyzer::DocumentClass detected_dclass;

		const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
		const TextProcessorInterface* textproc = objBuilder->getTextProcessor();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (!textproc) throw runtime_error( "%s", errorhnd->fetchError());

		enum {MaxHdrSize = 8092};
		std::size_t hdrsize = content.size() > MaxHdrSize ? MaxHdrSize : content.size();
		if (!textproc->detectDocumentClass( detected_dclass, content.c_str(), hdrsize, MaxHdrSize < content.size()))
		{
			if (errorhnd->hasError())
			{
				throw strus::runtime_error( "%s", errorhnd->fetchError());
			}
			else
			{
				throw strus::runtime_error( _TXT("could not detect document class of document to analyze"));
			}
		}
		return detected_dclass;
	}
}

analyzer::Document* DocumentAnalyzerImpl::analyzeSingle( const std::string& content, const ValueVariant& dclass) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const DocumentAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<const DocumentAnalyzerInstanceInterface>();
	analyzer::DocumentClass documentClass = getDocumentClass( content, dclass);
	return analyzeDoc( THIS, content, documentClass, errorhnd);
}

class DocumentAnalyzeIterator
{
public:
	DocumentAnalyzeIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& analyzer_, const ObjectRef& errorhnd_, const std::string& content, const analyzer::DocumentClass& dclass)
		:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_analyzer_impl(analyzer_),m_errorhnd_impl(errorhnd_),m_analyzercontext_impl()
	{
		const DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<const DocumentAnalyzerInstanceInterface>();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		DocumentAnalyzerContextInterface* analyzerContext;
		m_analyzercontext_impl.resetOwnership( analyzerContext = analyzer->createContext( dclass), "DocumentAnalyzerContext");
		if (!analyzerContext) throw strus::runtime_error( "%s", errorhnd->fetchError());
		analyzerContext->putInput( content.c_str(), content.size(), true);
	}
	virtual ~DocumentAnalyzeIterator(){}

	bool getNext( papuga_CallResult* result)
	{
		try
		{
			DocumentAnalyzerContextInterface* analyzerContext = m_analyzercontext_impl.getObject<DocumentAnalyzerContextInterface>();
			Reference<analyzer::Document> doc( new analyzer::Document());
			if (!analyzerContext->analyzeNext( *doc))
			{
				ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
				if (errorhnd->hasError())
				{
					throw strus::runtime_error( "%s", errorhnd->fetchError());
				}
				return false;
			}
			initCallResultStructureOwnership( result, doc.release());
			return true;
		}
		catch (const std::bad_alloc& err)
		{
			papuga_CallResult_reportError( result, _TXT("memory allocation error in document analyze multipart iterator get next"));
			return false;
		}
		catch (const std::runtime_error& err)
		{
			papuga_CallResult_reportError( result, _TXT("error in document analyze multipart iterator get next: %s"), err.what());
			return false;
		}
	}

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_analyzer_impl;
	ObjectRef m_errorhnd_impl;
	ObjectRef m_analyzercontext_impl;
};

static bool DocumentAnalyzeGetNext( void* self, papuga_CallResult* result)
{
	return ((DocumentAnalyzeIterator*)self)->getNext( result);
}

static void DocumentAnalyzeDeleter( void* obj)
{
	delete (DocumentAnalyzeIterator*)obj;
}

Iterator DocumentAnalyzerImpl::analyzeMultiPart(
		const std::string& content,
		const ValueVariant& dclass) const
{
	analyzer::DocumentClass documentClass = getDocumentClass( content, dclass);
	Iterator rt( new DocumentAnalyzeIterator( m_trace_impl, m_objbuilder_impl, m_analyzer_impl, m_errorhnd_impl, content, documentClass), &DocumentAnalyzeDeleter, &DocumentAnalyzeGetNext);
	rt.release();
	return rt;
}

Struct DocumentAnalyzerImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const DocumentAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<DocumentAnalyzerInstanceInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new DocumentAnalyzerIntrospection( errorhnd, analyzer));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

QueryAnalyzerImpl::QueryAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_analyzer_impl()
	,m_sentence_lexer_map_impl()
	,m_neighbour_collector_map_impl()
	,m_queryAnalyzerStruct()
	,m_textproc(textproc_)
{
	const AnalyzerObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<AnalyzerObjectBuilderInterface>();
	m_analyzer_impl.resetOwnership( objBuilder->createQueryAnalyzer(), "QueryAnalyzer");
	if (!m_analyzer_impl.get())
	{
		ErrorBufferInterface* ehnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", ehnd->fetchError());
	}
}

void QueryAnalyzerImpl::addElement(
	const std::string& featureType,
	const std::string& fieldType,
	const ValueVariant& tokenizer,
	const ValueVariant& normalizers,
	const ValueVariant& priorityval)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryAnalyzerInstanceInterface* THIS = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	int priority = Deserializer::getInt( priorityval, 0);
	THIS->addElement( featureType, fieldType, funcdef.tokenizer.get(), funcdef.normalizers, priority);
	funcdef.release();
}

static QueryAnalyzerContextInterface::GroupBy getImplicitGroupBy( const std::string& name)
{
	if (name.empty() || strus::caseInsensitiveEquals( name, "all"))
	{
		return QueryAnalyzerContextInterface::GroupAll;
	}
	else if (strus::caseInsensitiveEquals( name, "position"))
	{
		return QueryAnalyzerContextInterface::GroupByPosition;
	}
	else if (strus::caseInsensitiveEquals( name, "every"))
	{
		throw strus::runtime_error(_TXT("'%s' does not make sense as implicit grouping operation"), name.c_str());
	}
	else if (strus::caseInsensitiveEquals( name, "unique"))
	{
		throw strus::runtime_error(_TXT("'%s' does not make sense as implicit grouping operation"), name.c_str());
	}
	else
	{
		throw strus::runtime_error(_TXT("unknown group by operation '%s'"), name.c_str());
	}
}

void QueryAnalyzerImpl::defineImplicitGroupBy( const std::string& fieldtype, const std::string& groupBy_, const std::string& opname, int range, unsigned int cardinality)
{
	QueryAnalyzerContextInterface::GroupBy groupBy = getImplicitGroupBy( groupBy_);
	m_queryAnalyzerStruct.autoGroupBy( fieldtype, opname, range, cardinality, groupBy, false/*groupSingle*/);
}

TermExpression* QueryAnalyzerImpl::analyzeTermExpression_( const ValueVariant& expression, bool unique, bool schemaTypedOutput) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
	Reference<TermExpression> termexpr( new TermExpression( &m_queryAnalyzerStruct, analyzer, unique, schemaTypedOutput, errorhnd));
	if (!termexpr.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());

	QueryAnalyzerTermExpressionBuilder exprbuilder( termexpr.get());
	Deserializer::buildExpression( exprbuilder, expression, errorhnd, true);
	termexpr->analyze();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return termexpr.release();
}

TermExpression* QueryAnalyzerImpl::analyzeTermExpression( const ValueVariant& expression) const
{
	return analyzeTermExpression_( expression, false/*unique*/, false/*schemaTypedOutput*/);
}

TermExpression* QueryAnalyzerImpl::analyzeSingleTermExpression( const ValueVariant& expression) const
{
	return analyzeTermExpression_( expression, true/*unique*/, false/*schemaTypedOutput*/);
}

TermExpression* QueryAnalyzerImpl::analyzeSchemaTermExpression( const ValueVariant& expression) const
{
	return analyzeTermExpression_( expression, false/*unique*/, true/*schemaTypedOutput*/);
}

MetaDataExpression* QueryAnalyzerImpl::analyzeMetaDataExpression_( const ValueVariant& expression, bool schemaTypedOutput) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();
	Reference<MetaDataExpression> metaexpr( new MetaDataExpression( analyzer, schemaTypedOutput, errorhnd));
	if (!metaexpr.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());

	Deserializer::buildMetaDataRestriction( metaexpr.get(), expression, errorhnd);
	metaexpr->analyze();

	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return metaexpr.release();
}

MetaDataExpression* QueryAnalyzerImpl::analyzeMetaDataExpression( const ValueVariant& expression) const
{
	return analyzeMetaDataExpression_( expression, false/*schemaTypedOutput*/);
}

MetaDataExpression* QueryAnalyzerImpl::analyzeSchemaMetaDataExpression( const ValueVariant& expression) const
{
	return analyzeMetaDataExpression_( expression, true/*schemaTypedOutput*/);
}

struct SentenceLexerPrivateImpl
{
	std::string fieldType;
	ObjectRef lexer_impl;
	FeatureFuncDef featureFuncDef;
	QueryFeatureExpansionMap expansionMap;

	SentenceLexerPrivateImpl( const std::string& fieldType_, const ObjectRef& lexer_impl_, const FeatureFuncDef& featureFuncDef_, const QueryFeatureExpansionMap& expansionMap_)
		:fieldType(fieldType_),lexer_impl(lexer_impl_),featureFuncDef(featureFuncDef_),expansionMap(expansionMap_){}
	SentenceLexerPrivateImpl( const SentenceLexerPrivateImpl& o)
		:fieldType(o.fieldType),lexer_impl(o.lexer_impl),featureFuncDef(o.featureFuncDef),expansionMap(o.expansionMap){}
};

typedef std::vector<SentenceLexerPrivateImpl> SentenceLexerArrayPrivateImpl;

template <typename ELEMTYPE>
void ObjectRef_addToArray( ObjectRef& objref, const ELEMTYPE& elem)
{
	typedef std::vector<ELEMTYPE> ELEMARRAY;

	ObjectRef objref_copy = objref;
	ObjectRef objref_new;
	ELEMARRAY* ar = objref_copy.getObject<ELEMARRAY>();
	if (ar)
	{
		objref_new.resetOwnership( ar = new ELEMARRAY( *ar), objref_copy.name());
	}
	else
	{
		if (objref_copy.get()) throw strus::runtime_error(_TXT( "logic error: type mismatch accessing '%s'"), objref_copy.name());
		objref_new.resetOwnership( ar = new ELEMARRAY(), objref_copy.name());
	}
	ar->push_back( elem);
	objref = objref_new;
}

void QueryAnalyzerImpl::addSentenceType(
		const std::string& fieldType,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers,
		const ValueVariant& expansion,
		SentenceLexerImpl* lexer)
{
	QueryFeatureExpansionMap expansionmap( expansion);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	ObjectRef_addToArray( m_sentence_lexer_map_impl,
			SentenceLexerPrivateImpl(
				fieldType, lexer->m_lexer_impl,
				FeatureFuncDef( m_textproc, tokenizer, normalizers, errorhnd),
				expansionmap));
}

struct CollectorPrivateImpl
{
	std::string fieldType;
	char prefixSeparator;
	ObjectRef lexer_impl;
	FeatureFuncDef featureFuncDef;

	CollectorPrivateImpl( const std::string& fieldType_, char prefixSeparator_, const ObjectRef& lexer_impl_, const FeatureFuncDef& featureFuncDef_)
		:fieldType(fieldType_),prefixSeparator(prefixSeparator_),lexer_impl(lexer_impl_),featureFuncDef(featureFuncDef_){}
	CollectorPrivateImpl( const CollectorPrivateImpl& o)
		:fieldType(o.fieldType),prefixSeparator(o.prefixSeparator),lexer_impl(o.lexer_impl),featureFuncDef(o.featureFuncDef){}
};

typedef std::vector<CollectorPrivateImpl> CollectorArrayPrivateImpl;

void QueryAnalyzerImpl::addCollectorType(
		const std::string& fieldType,
		const std::string& prefixSeparator,
		const ValueVariant& normalizers,
		SentenceLexerImpl* lexer)
{
	char prefixSeparatorChar = strus::Constants::standard_word2vec_type_feature_separator();
	if (!prefixSeparator.empty())
	{
		if (prefixSeparator.size() != 1) throw std::runtime_error(_TXT( "single ASCII character expected as word type prefix separator"));
		prefixSeparatorChar = prefixSeparator[0];
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	ObjectRef_addToArray( m_neighbour_collector_map_impl,
			CollectorPrivateImpl(
				fieldType, prefixSeparatorChar, lexer->m_lexer_impl,
				FeatureFuncDef( m_textproc, normalizers, errorhnd)));
}

static std::string normalize_field(
	const char* tok, std::size_t toksize, 
	std::vector<NormalizerFunctionInstanceInterface*>::const_iterator ci,
	const std::vector<NormalizerFunctionInstanceInterface*>::const_iterator& ce)
{
	if (ci == ce) return std::string( tok, toksize);

	std::string rt;
	std::string origstr;
	for (; ci != ce; ++ci)
	{
		rt = (*ci)->normalize( tok, toksize);
		if (ci + 1 != ce)
		{
			if (!rt.empty() && rt[0] == '\0')
			{
				std::string reslist;
				char const* vi = rt.c_str();
				char const* ve = vi + rt.size();
				for (++vi; vi < ve; vi = std::strchr( vi, '\0')+1)
				{
					std::string partres = normalize_field( vi, std::strlen(vi), ci+1, ce);
					if (!partres.empty() && partres[0] == '\0')
					{
						reslist.append( partres);
					}
					else if (reslist.empty())
					{
						reslist.push_back( '\0');
						reslist.append( partres);
					}
					else
					{
						reslist.append( partres);
					}
				}
				return reslist;
			}
			else
			{
				origstr.swap( rt);
				tok = origstr.c_str();
				toksize = origstr.size();
			}
		}
	}
	return rt;
}

static void appendNormTokenToSentenceFields( std::vector<std::string>& result, const std::string& normtok)
{
	if (!normtok.empty() && normtok[0] == '\0')
	{
		char const* vi = normtok.c_str();
		char const* ve = vi + normtok.size();
		for (++vi; vi < ve; vi = std::strchr( vi, '\0')+1)
		{
			result.push_back( vi);
		}
	}
	else
	{
		result.push_back( normtok);
	}
	
}

SentenceTermExpression* QueryAnalyzerImpl::analyzeSentence(
		const std::string& fieldType,
		const std::string& fieldContent,
		int maxNofResults,
		double minWeight)
{
	ObjectRef sentence_lexer_map_impl_copy = m_sentence_lexer_map_impl;
	SentenceLexerArrayPrivateImpl* sar = sentence_lexer_map_impl_copy.getObject<SentenceLexerArrayPrivateImpl>();
	if (!sar) throw strus::runtime_error(_TXT( "unknown '%s' (none defined)"), "sentence analyzer");
	SentenceLexerArrayPrivateImpl::const_iterator si = sar->begin(), se = sar->end();
	for (; si != se && !strus::caseInsensitiveEquals( si->fieldType, fieldType); ++si){}
	if (si == se) throw strus::runtime_error(_TXT( "unknown '%s' (not found)"), "sentence analyzer");

	const SentenceLexerInstanceInterface* lexer = si->lexer_impl.getObject<SentenceLexerInstanceInterface>();

	std::vector<std::string> fields;
	const TokenizerFunctionInstanceInterface* tokenizer = si->featureFuncDef.tokenizer.get();
	if (tokenizer)
	{
		std::vector<analyzer::Token> tokens = tokenizer->tokenize( fieldContent.c_str(), fieldContent.size());
		std::vector<analyzer::Token>::const_iterator ti = tokens.begin(), te = tokens.end();
		for (; ti != te; ++ti)
		{
			std::string normtok
				= normalize_field(
					fieldContent.c_str() + ti->origpos().ofs(), ti->origsize(),
					si->featureFuncDef.normalizers.begin(), si->featureFuncDef.normalizers.end());
			appendNormTokenToSentenceFields( fields, normtok);
		}
	}
	else
	{
		std::string normtok
				= normalize_field(
					fieldContent.c_str(), fieldContent.size(),
					si->featureFuncDef.normalizers.begin(), si->featureFuncDef.normalizers.end());
		appendNormTokenToSentenceFields( fields, normtok);
	}
	std::vector<SentenceGuess> guess = lexer->call( fields, maxNofResults, minWeight);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError()) throw strus::runtime_error( "%s", errorhnd->fetchError());

	Reference<SentenceTermExpression> termexpr( new SentenceTermExpression( si->expansionMap, lexer, guess));
	return termexpr.release();
}

Struct QueryAnalyzerImpl::getCollectedNeighbours(
		const std::string& fieldType,
		const ValueVariant& fields_,
		const std::string& searchType,
		int maxNofResults,
		double minSimilarity,
		double minNormalizedWeight)
{
	Struct rt;
	ObjectRef m_neighbour_collector_map_impl_copy = m_neighbour_collector_map_impl;
	CollectorArrayPrivateImpl* car = m_neighbour_collector_map_impl_copy.getObject<CollectorArrayPrivateImpl>();
	if (!car) throw strus::runtime_error(_TXT( "unknown '%s' (none defined)"), "sentence analyzer");
	CollectorArrayPrivateImpl::const_iterator ci = car->begin(), ce = car->end();
	for (; ci != ce && !strus::caseInsensitiveEquals( ci->fieldType, fieldType); ++ci){}
	if (ci == ce) throw strus::runtime_error(_TXT( "unknown '%s' (not found)"), "sentence analyzer");

	std::vector<WeightedString> fields = Deserializer::getWeightedStringList( fields_);
	const SentenceLexerInstanceInterface* lexer = ci->lexer_impl.getObject<SentenceLexerInstanceInterface>();

	std::vector<WeightedSentenceTerm> termlist;
	std::vector<WeightedString>::const_iterator fi = fields.begin(), fe = fields.end();
	for (; fi != fe; ++fi)
	{
		char const* fstr = std::strchr( fi->value.c_str(), ci->prefixSeparator);
		std::size_t flen = 0;
		char const* tstr = fi->value.c_str();
		std::size_t tlen = 0;
		if (fstr && ci->prefixSeparator)
		{
			tlen = fstr - tstr;
			fstr += 1;
			flen = std::strlen( fstr);
		}
		else
		{
			throw strus::runtime_error(_TXT( "missing separator in token '%s'"), fi->value.c_str());
		}
		std::string normtok
			= normalize_field(
				fstr+1, flen-1,
				ci->featureFuncDef.normalizers.begin(), ci->featureFuncDef.normalizers.end());
		termlist.push_back( WeightedSentenceTerm( std::string( tstr, tlen), std::string( fstr, flen), fi->weight));
	}
	std::vector<WeightedString> similarValues;
	std::vector<WeightedSentenceTerm> similarTerms
		= lexer->similarTerms( searchType, termlist, minSimilarity, maxNofResults, minNormalizedWeight);
	std::vector<WeightedSentenceTerm>::const_iterator si = similarTerms.begin(), se = similarTerms.end();
	for (; si != se; ++si)
	{
		similarValues.push_back( WeightedString( si->value(), si->weight()));
	}
	Serializer::serialize( &rt.serialization, similarValues, false);
	rt.release();
	return rt;
	
}

Struct QueryAnalyzerImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<QueryAnalyzerInstanceInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new QueryAnalyzerIntrospection( errorhnd, analyzer));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

