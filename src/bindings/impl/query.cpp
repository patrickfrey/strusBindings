/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/query.hpp"
#include "impl/storage.hpp"
#include "impl/value/structViewIntrospection.hpp"
#include "papuga/serialization.h"
#include "strus/queryEvalInterface.hpp"
#include "strus/queryInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/base/local_ptr.hpp"
#include "private/internationalization.hpp"
#include "valueVariantWrap.hpp"
#include "callResultUtils.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "structDefs.hpp"

using namespace strus;
using namespace strus::bindings;

QueryEvalImpl::QueryEvalImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_queryeval_impl()
{
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
	m_queryproc = objBuilder->getQueryProcessor();
	if (!m_queryproc)
	{
		ErrorBufferInterface* ehnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", ehnd->fetchError());
	}
	m_queryeval_impl.resetOwnership( objBuilder->createQueryEval(), "QueryEval");
	if (!m_queryeval_impl.get())
	{
		ErrorBufferInterface* ehnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", ehnd->fetchError());
	}
}

void QueryEvalImpl::addTerm(
		const std::string& set_,
		const std::string& type_,
		const std::string& value_)
{
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();
	queryeval->addTerm( set_, type_, value_);
}

void QueryEvalImpl::addSelectionFeature( const std::string& set_)
{
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();
	queryeval->addSelectionFeature( set_);
}

void QueryEvalImpl::addRestrictionFeature( const std::string& set_)
{
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();
	queryeval->addRestrictionFeature( set_);
}

void QueryEvalImpl::addExclusionFeature( const std::string& set_)
{
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();
	queryeval->addExclusionFeature( set_);
}

void QueryEvalImpl::addSummarizer(
		const ValueVariant& summaryid,
		const std::string& name,
		const ValueVariant& parameters,
		const ValueVariant& featuresets)
{
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	std::string summaryidstr;
	if (papuga_ValueVariant_defined( &summaryid))
	{
		summaryidstr = ValueVariantWrap::tostring( summaryid);
	}
	Deserializer::buildSummarizerFunction(
		queryeval, summaryidstr, name, parameters, featuresets, m_queryproc, errorhnd);
}

void QueryEvalImpl::addWeightingFunction(
		const std::string& name,
		const ValueVariant& parameters,
		const ValueVariant& featuresets)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();

	Deserializer::buildWeightingFunction(
		queryeval, name, parameters, featuresets, m_queryproc, errorhnd);
}

void QueryEvalImpl::defineWeightingFormula(
		const std::string& source,
		const ValueVariant& parameter)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();

	Deserializer::buildWeightingFormula( queryeval, source, parameter, m_queryproc, errorhnd);
}

void QueryEvalImpl::usePositionInformation( const std::string& featureset, bool yes)
{
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();
	queryeval->usePositionInformation( featureset, yes);
}

QueryImpl* QueryEvalImpl::createQuery( StorageClientImpl* storage) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryEvalInterface* qe = m_queryeval_impl.getObject<QueryEvalInterface>();
	const StorageClientInterface* st = storage->m_storage_impl.getObject<StorageClientInterface>();
	ObjectRef query;
	query.resetOwnership( qe->createQuery( st), "Query");
	if (!query.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());

	return new QueryImpl( m_trace_impl, m_objbuilder_impl, m_errorhnd_impl, storage->m_storage_impl, m_queryeval_impl, query, m_queryproc);
}

Struct QueryEvalImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new StructViewIntrospection( errorhnd, queryeval->view()));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

void QueryImpl::addFeature( const std::string& set_, const ValueVariant& expr_, double weight_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryExpressionBuilder exprbuilder( THIS, m_queryproc, errorhnd);

	Deserializer::buildExpression( exprbuilder, expr_, errorhnd, true);
	if (errorhnd->hasError()) throw strus::runtime_error( "%s", errorhnd->fetchError());
	unsigned int ii=0, nn = exprbuilder.stackSize();
	if (nn == 0) throw strus::runtime_error( _TXT("feature defined without expression"));
	for (; ii<nn; ++ii)
	{
		THIS->defineFeature( set_, weight_);
	}
}

void QueryImpl::addMetaDataRestriction( const ValueVariant& expression)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	Deserializer::buildMetaDataRestriction( THIS, expression, errorhnd);
}

void QueryImpl::defineTermStatistics( const std::string& type_, const std::string& value_, const ValueVariant& stats_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	TermStatistics stats = Deserializer::getTermStatistics( stats_);
	THIS->defineTermStatistics( type_, value_, stats);
}

void QueryImpl::defineGlobalStatistics( const ValueVariant& stats_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	GlobalStatistics stats = Deserializer::getGlobalStatistics( stats_);
	THIS->defineGlobalStatistics( stats);
}

void QueryImpl::addDocumentEvaluationSet( const ValueVariant& docnolist_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	std::vector<Index> docnolist = Deserializer::getIndexList( docnolist_);
	THIS->addDocumentEvaluationSet( docnolist);
}

void QueryImpl::setMaxNofRanks( int maxNofRanks_)
{
	m_maxNofRanks = maxNofRanks_;
}

void QueryImpl::setMinRank( int minRank_)
{
	m_minRank = minRank_;
}

void QueryImpl::useMergeResult( bool yes)
{
	m_useMergeResult = yes;
}

void QueryImpl::addAccess( const ValueVariant& userlist_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	std::vector<std::string> userlist = Deserializer::getStringList( userlist_);
	std::vector<std::string>::const_iterator ui = userlist.begin(), ue = userlist.end();
	for (; ui != ue; ++ui)
	{
		THIS->addAccess( *ui);
	}
}

void QueryImpl::setWeightingVariables(
		const ValueVariant& parameter)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();

	KeyValueList kvlist( parameter, "name,value");
	KeyValueList::const_iterator ki = kvlist.begin(), ke = kvlist.end();
	for (; ki != ke; ++ki)
	{
		THIS->setWeightingVariableValue( ki->first, ValueVariantWrap::todouble( *ki->second));
	}
}

QueryResult* QueryImpl::evaluate() const
{
	const QueryInterface* THIS = m_query_impl.getObject<const QueryInterface>();
	Reference<QueryResult> result;
	if (m_useMergeResult)
	{
		result.reset( new QueryResult( THIS->evaluate( 0, m_minRank + m_maxNofRanks)));
	}
	else
	{
		result.reset( new QueryResult( THIS->evaluate( m_minRank, m_maxNofRanks)));
	}
	if (result->ranks().empty())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( "%s", errorhnd->fetchError());
		}
	}
	return result.release();
}

Struct QueryImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryInterface* query = m_query_impl.getObject<QueryInterface>();

	StructView view = query->view();
	view
		( "minrank", m_minRank)
		( "nofranks", m_maxNofRanks)
		( "merge", m_useMergeResult);
	strus::local_ptr<IntrospectionBase> ictx( new StructViewIntrospection( errorhnd, view));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

void QueryResultMergerImpl::setMaxNofRanks( int maxNofRanks_)
{
	m_maxNofRanks = maxNofRanks_;
}

void QueryResultMergerImpl::setMinRank( int minRank_)
{
	m_minRank = minRank_;
}

void QueryResultMergerImpl::addQueryResult( const ValueVariant& res)
{
	std::vector<QueryResult> partvec( Deserializer::getQueryResultList( res));
	m_ar.insert( m_ar.end(), partvec.begin(), partvec.end());
}

void QueryResultMergerImpl::useMergeResult( bool yes)
{
	m_useMergeResult = yes;
}

QueryResult* QueryResultMergerImpl::evaluate() const
{
	return new QueryResult( QueryResult::merge( m_ar, m_minRank, m_maxNofRanks));
}

static StructView getView( const SummaryElement& res);
static StructView getView( const ResultDocument& res);
static StructView getView( const QueryResult& res);

template <class Element>
static StructView getView( const std::vector<Element>& ar)
{
	StructView rt;
	typename std::vector<Element>::const_iterator ai = ar.begin(), ae = ar.end();
	for (; ai != ae; ++ai) rt( getView( *ai));
	return rt;
}

static StructView getView( const SummaryElement& res)
{
	return StructView()("name",res.name())("value",res.value())("weight",res.weight())("index",res.index());
}

static StructView getView( const ResultDocument& res)
{
	return StructView()("docno",res.docno())("weight",res.weight())("summary",getView(res.summaryElements()));
}

static StructView getView( const QueryResult& res)
{
	return StructView()("evalpass",res.evaluationPass())("nofranked",res.nofRanked())("nofvisited",res.nofVisited())("ranks", getView(res.ranks()));
}

Struct QueryResultMergerImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	StructView view;
	view
		( "minrank", m_minRank)
		( "nofranks", m_maxNofRanks)
		( "results", getView(m_ar));
	strus::local_ptr<IntrospectionBase> ictx( new StructViewIntrospection( errorhnd, view));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

void QueryBuilderImpl::setMaxNofRanks( int maxNofRanks_)
{
	m_obj.setMaxNofRanks( maxNofRanks_);
}

void QueryBuilderImpl::setMinRank( int minRank_)
{
	m_obj.setMinRank( minRank_);
}

void QueryBuilderImpl::addFeature( const ValueVariant& feature)
{
	m_obj.addFeature( feature);
}

void QueryBuilderImpl::addRestriction( const ValueVariant& restriction)
{
	m_obj.addRestriction( restriction);
}

void QueryBuilderImpl::addCollectSummary( const ValueVariant& summary)
{
	m_obj.addCollectSummary( Deserializer::getSummary( summary));
}

Struct QueryBuilderImpl::getFeatures() const
{
	return Struct( m_obj.getFeatures());
}

Struct QueryBuilderImpl::getRestrictions() const
{
	return Struct( m_obj.getRestrictions());
}

Struct QueryBuilderImpl::introspection( const ValueVariant& path) const
{
	Struct rt;
	rt.release();
	return rt;
}



