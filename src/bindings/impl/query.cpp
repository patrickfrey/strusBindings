/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/query.hpp"
#include "impl/storage.hpp"
#include "papuga/serialization.hpp"
#include "metadataop.hpp"
#include "strus/queryEvalInterface.hpp"
#include "strus/queryInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "internationalization.hpp"
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
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("error in get query processor: %s"), errorhnd->fetchError());
	}
	m_queryeval_impl.resetOwnership( objBuilder->createQueryEval(), "QueryEval");
	if (!m_queryeval_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("error creating query eval: %s"), errorhnd->fetchError());
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

QueryImpl* QueryEvalImpl::createQuery( StorageClientImpl* storage) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const QueryEvalInterface* qe = m_queryeval_impl.getObject<QueryEvalInterface>();
	const StorageClientInterface* st = storage->m_storage_impl.getObject<StorageClientInterface>();
	ObjectRef query;
	query.resetOwnership( qe->createQuery( st), "Query");
	if (!query.get()) throw strus::runtime_error( _TXT("failed to create query object: %s"), errorhnd->fetchError());

	return new QueryImpl( m_trace_impl, m_objbuilder_impl, m_errorhnd_impl, storage->m_storage_impl, m_queryeval_impl, query, m_queryproc);
}

void QueryImpl::defineFeature( const std::string& set_, const ValueVariant& expr_, double weight_)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	QueryExpressionBuilder exprbuilder( THIS, m_queryproc, errorhnd);

	Deserializer::buildExpression( exprbuilder, expr_, errorhnd);
	THIS->defineFeature( set_, weight_);
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
		const ValueVariant& parameter)
{
	static char* context = _TXT("weighting variables");
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	if (parameter.valuetype != papuga_TypeSerialization)
	{
		throw strus::runtime_error(_TXT("list of variable assignments expected as argument"));
	}
	papuga::Serialization::const_iterator
		si = papuga::Serialization::begin( parameter.value.serialization),
		se = papuga::Serialization::end( parameter.value.serialization);
	KeyValueList kvlist( si, se);
	if (si != se) throw strus::runtime_error(_TXT("unexpected tokens at end of serialization of %s"), context);

	KeyValueList::const_iterator ki = kvlist.begin(), ke = kvlist.end();
	for (; ki != ke; ++ki)
	{
		THIS->setWeightingVariableValue( ki->first, ValueVariantWrap::todouble( *ki->second));
	}
}

void QueryImpl::setDebugMode( bool debug)
{
	QueryInterface* THIS = m_query_impl.getObject<QueryInterface>();
	THIS->setDebugMode( debug);
}

QueryResult* QueryImpl::evaluate() const
{
	const QueryInterface* THIS = m_query_impl.getObject<const QueryInterface>();
	return new QueryResult( THIS->evaluate());
}

std::string QueryImpl::tostring() const
{
	const QueryInterface* THIS = m_query_impl.getObject<const QueryInterface>();
	return THIS->tostring();
}



