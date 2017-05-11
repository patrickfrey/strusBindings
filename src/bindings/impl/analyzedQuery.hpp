/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_ANALYZED_QUERY_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_ANALYZED_QUERY_HPP_INCLUDED
#include "strus/analyzer/query.hpp"
#include "queryAnalyzerStruct.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \brief Analyzed query structure
class AnalyzedQuery
{
public:
	/// \brief Constructor
	AnalyzedQuery( const analyzer::Query& query_, const std::vector<QueryAnalyzerStruct::Operator>& operators_, bool labeledOutput_)
		:m_query(query_),m_operators(operators_),m_labeledOutput(labeledOutput_){}
	/// \brief Copy constructor
	AnalyzedQuery( const AnalyzedQuery& o)
		:m_query(o.m_query),m_operators(o.m_operators),m_labeledOutput(o.m_labeledOutput){}

	/// \brief Reference to query structure
	const analyzer::Query& query() const					{return m_query;}
	/// \brief Operators defined
	const std::vector<QueryAnalyzerStruct::Operator>& operators() const	{return m_operators;}
	/// \brief Defines the serialization of output depending on input (positional
	bool labeledOutput() const						{return m_labeledOutput;}

private:
	analyzer::Query m_query;
	std::vector<QueryAnalyzerStruct::Operator> m_operators;
	bool m_labeledOutput;
};

}}//namespace
#endif

