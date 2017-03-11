/*
 * Copyright (c) 2016 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Filling tables for the Strus bindings interface parser
/// \file fillTypeTables.cpp
#include "fillTypeTables.hpp"
#include "interfaceParser.hpp"
#include <stdexcept>

using namespace strus;

void strus::fillTypeTables( TypeSystem& typesystem)
{
	typesystem.defineType( "void")
	;
	typesystem.defineType( "const Index&")
	;
	typesystem.defineType( "const GlobalCounter&")
	;
	typesystem.defineType( "Index")
	;
	typesystem.defineType( "const analyzer::SegmenterOptions&")
	;
	typesystem.defineType( "analyzer::SegmenterOptions")
	;
	typesystem.defineType( "const SegmenterPosition&")
	;
	typesystem.defineType( "SegmenterPosition&")
	;
	typesystem.defineType( "unsigned int")
	;
	typesystem.defineType( "int")
	;
	typesystem.defineType( "int&")
	;
	typesystem.defineType( "std::size_t")
	;
	typesystem.defineType( "bool")
	;
	typesystem.defineType( "float")
	;
	typesystem.defineType( "double")
	;
	typesystem.defineType( "std::string")
	;
	typesystem.defineType( "const std::string&")
	;
	typesystem.defineType( "std::string&")
	;
	typesystem.defineType( "const std::vector<std::string>&")
	;
	typesystem.defineType( "std::vector<std::string>")
	;
	typesystem.defineType( "const std::vector<Index>&")
	;
	typesystem.defineType( "std::vector<Index>")
	;
	typesystem.defineType( "std::vector<Result>", "VectorStorageSearchInterface")
	;
	typesystem.defineType( "const std::vector<unsigned int>&")
	;
	typesystem.defineType( "std::vector<unsigned int>")
	;
	typesystem.defineType( "const std::vector<double>&")
	;
	typesystem.defineType( "std::vector<double>")
	;
	typesystem.defineType( "const char*")
	;
	typesystem.defineType( "const char**")
	;
	typesystem.defineType( "const char* $bufname, std::size_t")
	;
	typesystem.defineType( "const char*& $bufname, std::size_t&")
	;
	typesystem.defineType( "const double* $bufname, std::size_t")
	;
	typesystem.defineType( "const double* $bufname, unsigned int")
	;
	typesystem.defineType( "const NumericVariant&")
	;
	typesystem.defineType( "NumericVariant")
	;
	typesystem.defineType( "const analyzer::DocumentClass&")
	;
	typesystem.defineType( "analyzer::DocumentClass&")
	;
	typesystem.defineType( "const TermStatistics&")
	;
	typesystem.defineType( "const GlobalStatistics&")
	;
	typesystem.defineType( "const CompareOperator&", "MetaDataRestriction")
	;
	typesystem.defineType( "MetaDataRestrictionInterface::CompareOperator")
	;
	typesystem.defineType( "const DatabaseOptions&")
	;
	typesystem.defineType( "const ConfigType&", "Database")
	;
	typesystem.defineType( "const ConfigType&", "Storage")
	;
	typesystem.defineType( "const analyzer::FeatureOptions&")
	;
	typesystem.defineType( "const GroupBy&", "QueryAnalyzer")
	;
	typesystem.defineType( "const SummaryElement&")
	;
	typesystem.defineType( "std::vector<SummaryElement>")
	;
	typesystem.defineType( "const SummarizationVariable&")
	;
	typesystem.defineType( "const std::vector<SummarizationVariable>& ")
	;
	typesystem.defineType( "Term&", "DocumentTermIterator")
	;
	typesystem.defineType( "const Slice&", "DatabaseCursor")
	;
	typesystem.defineType( "Slice", "DatabaseCursor")
	;
	typesystem.defineType( "const analyzer::Query&")
	;
	typesystem.defineType( "analyzer::Query&")
	;
	typesystem.defineType( "analyzer::Query")
	;
	typesystem.defineType( "const analyzer::Document&")
	;
	typesystem.defineType( "analyzer::Document&")
	;
	typesystem.defineType( "analyzer::Document")
	;
	typesystem.defineType( "const analyzer::Attribute&")
	;
	typesystem.defineType( "const analyzer::MetaData&")
	;
	typesystem.defineType( "const analyzer::Term&")
	;
	typesystem.defineType( "std::vector<analyzer::Term>")
	;
	typesystem.defineType( "const std::vector<analyzer::Term>&")
	;
	typesystem.defineType( "std::vector<analyzer::TermArray>")
	;
	typesystem.defineType( "const analyzer::Token&")
	;
	typesystem.defineType( "const analyzer::PatternLexem&")
	;
	typesystem.defineType( "std::vector<analyzer::Token>")
	;
	typesystem.defineType( "std::vector<analyzer::PatternLexem>")
	;
	typesystem.defineType( "analyzer::PositionBind")
	;
	typesystem.defineType( "const analyzer::TokenMarkup&")
	;
	typesystem.defineType( "std::vector<analyzer::PatternMatcherResult>")
	;
	typesystem.defineType( "const std::vector<analyzer::PatternMatcherResult>&")
	;
	typesystem.defineType( "analyzer::PatternMatcherStatistics")
	;
	typesystem.defineType( "PatternMatcherInstanceInterface::JoinOperation")
	;
	typesystem.defineType( "JoinOperation")
	;
	typesystem.defineType( "const WeightedDocument&")
	;
	typesystem.defineType( "const ResultDocument&")
	;
	typesystem.defineType( "QueryResult")
	;
	typesystem.defineType( "const FeatureParameter&", "QueryEval")
	;
	typesystem.defineType( "const std::vector<FeatureParameter>&", "QueryEval")
	;
	typesystem.defineType( "const Phrase&", "QueryAnalyzer")
	;
	typesystem.defineType( "const std::vector<Phrase>&", "QueryAnalyzer")
	;
	typesystem.defineType( "const DocumentStatisticsType&", "StorageClient")
	;
	typesystem.defineType( "const BuilderOptions&", "StatisticsProcessor")
	;
	typesystem.defineType( "DocumentFrequencyChange&", "StatisticsViewer")
	;
	typesystem.defineType( "const FunctionType&", "QueryProcessor")
	;
	typesystem.defineType( "const FunctionType&", "TextProcessor")
	;
	typesystem.defineType( "Description", "PostingJoinOperator")
	;
	typesystem.defineType( "FunctionDescription")
	;
	typesystem.defineType( "$objid~Interface*")
	;
	//Define explicit pass by reference exceptions:
	static const char* interfacePassByReferenceException[7][2] = {
		{"Database", "restoreDatabase"},
		{"Storage", "createStorage"},
		{"SummarizerFunctionContext", "addSummarizationFeature"},
		{"WeightingFunctionContext", "addWeightingFeature"},
		{"SummarizerFunctionInstance", "createFunctionContext"},
		{"WeightingFunctionInstance", "createFunctionContext"},
		{0,0}
	};
	for (std::size_t ei=0; interfacePassByReferenceException[ei][0]; ++ei)
	{
		const char* classname = interfacePassByReferenceException[ei][0];
		const char* methodname = interfacePassByReferenceException[ei][1];
		typesystem.defineType( "$objid~Interface*", classname, methodname)
		;
	}
	typesystem.defineType( "const $objid~Interface*")
	;
	typesystem.defineType( "const std::vector<Reference<$objid~Interface> >&")
	;
	typesystem.defineType( "const std::vector<$objid~Interface*>&")
	;
	typesystem.defineType( "const char*&")
	;
	typesystem.defineType( "const std::string&")
	;
	typesystem.defineType( "std::ostream&")
	;
}

