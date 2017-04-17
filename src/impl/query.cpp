/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/query.hpp"
#include "impl/storage.hpp"
#include "strus/queryEvalInterface.hpp"
#include "strus/queryInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "internationalization.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"

using namespace strus;
using namespace strus::bindings;

QueryEvalImpl::QueryEvalImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd)
	:m_errorhnd_impl(errorhnd)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_queryeval_impl()
{
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
	m_queryproc = objBuilder->getQueryProcessor();
	if (!m_queryproc)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("error in get query processor: %s"), errorhnd->fetchError());
	}
	m_queryeval_impl.resetOwnership( objBuilder->createQueryEval());
	if (!m_queryeval_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("error creating query eval: %s"), errorhnd->fetchError());
	}
}

QueryEvalImpl::QueryEvalImpl( const QueryEvalImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_queryproc(o.m_queryproc)
{}

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
		const std::string& name,
		const ValueVariant& parameters,
		const ValueVariant& resultnames)
{
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	Deserializer::buildSummarizerFunction(
		queryeval, name, parameters, resultnames, m_queryproc, errorhnd);
}

void QueryEvalImpl::addWeightingFunction(
		const std::string& name,
		const ValueVariant& parameter)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();

	Deserializer::buildWeightingFunction(
		queryeval, name, parameter, m_queryproc, errorhnd);
}

void QueryEvalImpl::addWeightingFormula(
		const std::string& source,
		const ValueVariant& parameter)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryEvalInterface* queryeval = m_queryeval_impl.getObject<QueryEvalInterface>();

	Deserializer::buildWeightingFormula( queryeval, source, parameter, m_queryproc, errorhnd);
}

CallResult QueryEvalImpl::createQuery( const StorageClientImpl& storage) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryEvalInterface* qe = m_queryeval_impl.getObject<QueryEvalInterface>();
	const StorageClientInterface* st = storage.m_storage_impl.getObject<StorageClientInterface>();
	HostObjectReference query;
	query.resetOwnership( qe->createQuery( st));
	if (!query.get()) throw strus::runtime_error( _TXT("failed to create query object: %s"), errorhnd->fetchError());

	return callResultObject( new QueryImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, storage.m_storage_impl, m_queryeval_impl, query, m_queryproc));
}


QueryImpl::QueryImpl( const QueryImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_queryeval_impl(o.m_queryeval_impl)
	,m_query_impl(o.m_query_impl)
	,m_queryproc(o.m_queryproc)
{}

void QueryImpl::defineFeature( const std::string& set_, const ValueVariant& expr_, double weight_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryExpressionBuilder exprbuilder( THIS, m_queryproc, errorhnd);

	Deserializer::buildExpression( exprbuilder, expr_);
	THIS->defineFeature( set_, weight_);
}

static MetaDataRestrictionInterface::CompareOperator getCompareOp( const char* compareOp)
{
	MetaDataRestrictionInterface::CompareOperator cmpop;
	if (compareOp[0] == '<')
	{
		if (compareOp[1] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareLess;
		}
		else if (compareOp[1] == '=' && compareOp[2] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareLessEqual;
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
			cmpop = MetaDataRestrictionInterface::CompareGreater;
		}
		else if (compareOp[1] == '=' && compareOp[2] == '\0')
		{
			cmpop = MetaDataRestrictionInterface::CompareGreaterEqual;
		}
		else
		{
			throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
		}
	}
	else if (compareOp[0] == '=' && compareOp[1] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareEqual;
	}
	else if (compareOp[0] == '=' && compareOp[1] == '=' && compareOp[2] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareEqual;
	}
	else if (compareOp[0] == '!' && compareOp[1] == '=' && compareOp[2] == '\0')
	{
		cmpop = MetaDataRestrictionInterface::CompareNotEqual;
	}
	else
	{
		throw strus::runtime_error( _TXT("unknown compare operator '%s', expected one of '=','!=','>','<','<=','>='"), compareOp);
	}
	return cmpop;
}

void QueryImpl::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		const ValueVariant& operand, bool newGroup)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	MetaDataRestrictionInterface::CompareOperator cmpop = getCompareOp( compareOp);
	THIS->addMetaDataRestrictionCondition( cmpop, name, ValueVariantConv::tonumeric(operand), newGroup);
}

void QueryImpl::defineTermStatistics( const std::string& type_, const std::string& value_, const ValueVariant& stats_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	TermStatistics stats;
	stats.setDocumentFrequency( stats_.df());
	THIS->defineTermStatistics( type_, value_, stats);
}

void QueryImpl::defineGlobalStatistics( const GlobalStatistics& stats_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	GlobalStatistics stats;
	stats.setNofDocumentsInserted( stats_.nofdocs());
	THIS->defineGlobalStatistics( stats);
}

void QueryImpl::addDocumentEvaluationSet(
		const std::vector<Index>& docnolist_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	std::vector<Index> docnolist;
	std::vector<Index>::const_iterator di = docnolist_.begin(), de = docnolist_.end();
	for (; di != de; ++di) docnolist.push_back( *di);
	THIS->addDocumentEvaluationSet( docnolist);
}

void QueryImpl::setMaxNofRanks( unsigned int maxNofRanks_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	THIS->setMaxNofRanks( maxNofRanks_);
}

void QueryImpl::setMinRank( unsigned int minRank_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	THIS->setMinRank( minRank_);
}

void QueryImpl::addUserName( const std::string& username_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	THIS->addUserName( username_);
}

void QueryImpl::setWeightingVariables(
		const FunctionVariableConfig& parameter)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	std::map<std::pair<std::string,double> >::const_iterator
		vi = parameter.m_variables.begin(),
		ve = parameter.m_variables.end();
	for (; vi != ve; ++vi)
	{
		THIS->setWeightingVariableValue( vi->first, vi->second);
	}
}

void QueryImpl::setDebugMode( bool debug)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	THIS->setDebugMode( debug);
}

CallResult QueryImpl::evaluate() const
{
	std::vector<Rank> ranks;
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	QueryResult res = THIS->evaluate();
	if (res.ranks().empty() && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to evaluate query: %s"), errorhnd->fetchError());
	}
	QueryResult rt( res.evaluationPass(), res.nofDocumentsRanked(), res.nofDocumentsVisited());
	std::vector<ResultDocument>::const_iterator
		ri = res.ranks().begin(), re = res.ranks().end();
	for (;ri != re; ++ri)
	{
		Rank reselem;
		reselem.m_docno = (unsigned int)ri->docno();
		reselem.m_weight = ri->weight();
		std::vector<SummaryElement>::const_iterator
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

CallResult QueryImpl::tostring() const
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	std::string rt( THIS->tostring());
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to map query to string: %s"), errorhnd->fetchError());
	}
	return rt;
}



