/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#include "strus/bindingObjects.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <map>
#include <set>
#include <memory>

int main( int , const char** )
{
	try
	{
		Context ctx;
		DocumentAnalyzer documentAnalyzer( ctx.createDocumentAnalyzer());
		// Define document analyzer:
		std::string noopt;
		std::vector<Normalizer> normalizer_stem;
		normalizer_stem.push_back( Normalizer("stem", "de"));
		std::vector<Normalizer> normalizer_orig;
		normalizer_orig.push_back( Normalizer("orig"));
		
		documentAnalyzer.addSearchIndexFeature(
			"word", "/doc/text()", Tokenizer("word"), normalizer_stem, noopt);
		documentAnalyzer.addForwardIndexFeature(
			"orig", "/doc/text()", Tokenizer("word"), normalizer_orig, noopt);
		documentAnalyzer.defineMetaData(
			"date", "/doc/date()", Tokenizer("content"), normalizer_orig);
		documentAnalyzer.defineMetaData(
			"class", "/doc/class()", Tokenizer("content"), normalizer_orig);
		documentAnalyzer.defineAttribute(
			"title", "/doc/title()", Tokenizer("content"), normalizer_orig);
		documentAnalyzer.defineAttribute(
			"docid", "/doc@id", Tokenizer("content"), normalizer_orig);

		// Define query analyzer:
		QueryAnalyzer queryAnalyzer( ctx.createQueryAnalyzer());
		queryAnalyzer.definePhraseType(
			"default", "word", Tokenizer("word"), normalizer_stem);

		// Create storage:
		try
		{
			ctx.destroyStorage( "path=storage");
		}
		catch (...)
		{}
		ctx.createStorage( "path=storage; acl=yes; metadata=doclen UINT16, class UINT16, data INT32");
		StorageClient storage( ctx.createStorageClient( "path=storage"));
		StorageTransaction transaction = storage.createTransaction();

		// Insert documents:
		Document doc;
		doc = documentAnalyzer.analyze( "<doc id='X://doc_1'><title>Title_1</title><class>1</class><text>Bob likes to go to school</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_1", doc, true);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_2'><title>Title_2</title><class>2</class><text>Alice has high marks in mathematics</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_2", doc, true);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_3'><title>Title_3</title><class>3</class><text>Rose needs private tuition to improve here marks</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_3", doc, true);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_4'><title>Title_4</title><class>4</class><text>Jack is good in sports</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_4", doc, true);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_5'><title>Title_5</title><class>5</class><text>Tom has a very long walk to school</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_5", doc, true);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_6'><title>Title_6</title><class>6</class><text>Martha loves all scientific subjects</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_6", doc, true);

		transaction.commit();

		// Define the query evaluation program:
		QueryEval queryEval = ctx.createQueryEval();
		queryEval.addTerm( "punct", "punct", "dot");
		queryEval.addSelectionFeature( "weighted");
		queryEval.addRestrictionFeature( "weighted");

		SummarizerConfig titlesum;
		titlesum.defineParameter( "name", "title");
		queryEval.addSummarizer( "title", "attribute", titlesum);
		SummarizerConfig docidsum;
		docidsum.defineParameter( "name", "docid");
		queryEval.addSummarizer( "docid", "attribute", docidsum);
		SummarizerConfig matchsum;
		matchsum.defineFeature( "match", "weighted");
		queryEval.addSummarizer( "match", "matchpos", matchsum);

		WeightingConfig weightingConfig;
		weightingConfig.defineParameter( "k1", 0.75);
		weightingConfig.defineParameter( "b", 2.1);
		weightingConfig.defineParameter( "avgdoclen", 8);
		queryEval.addWeightingFunction( 1.0, "BM25", weightingConfig);

		// Create a query from a phrase:
		Query query( queryEval.createQuery( storage));
		std::vector<Term> queryterms = queryAnalyzer.analyzePhrase( "default", "to school");
		std::vector<Term>::const_iterator qi = queryterms.begin(), qe = queryterms.end();
		std::size_t qidx = 0;
		unsigned int maxpos = 0;
		QueryExpression expr;
		for (; qi != qe; ++qidx,++qi)
		{
			expr.pushTerm( qi->type(), qi->value());
			if (qi->position() > maxpos)
			{
				maxpos = qi->position();
			}
		}
		expr.pushExpression( "sequence", qidx, maxpos);
		query.defineFeature( "weighted", expr, 1.0);

		// Evaluate the query and print the result:
		query.defineMetaDataRestriction( "<=", "class", 5);
		query.defineMetaDataRestriction( ">=", "class", 1);
	
		query.setMaxNofRanks( 10);
		query.setMinRank( 0);

		query.addUserName( "nobody");

		std::set<unsigned int> expected;
		expected.insert( 1);
		expected.insert( 5);

		std::vector<Rank> result = query.evaluate();
		std::vector<Rank>::const_iterator ri = result.begin(), re = result.end();
		for (std::size_t ridx=1; ri != re; ++ridx,++ri)
		{
			std::cout << "[" << ridx << "] docno=" << ri->docno() << ", weight=" << ri->weight();
			if (expected.find( ri->docno()) == expected.end())
			{
				throw std::runtime_error( "found document not expected");
			}
			expected.erase( ri->docno());
			std::vector<RankAttribute>::const_iterator
				ai = ri->attributes().begin(), ae = ri->attributes().end();
			for (std::size_t aidx=0; ai != ae; ++aidx,++ai)
			{
				if (aidx) std::cout << ", "; else std::cout << ' ';
				std::cout << ai->name() << "=" << ai->value();
			}
			std::cout << std::endl;
		}
		if (!expected.empty())
		{
			throw std::runtime_error( "expected document not found");
		}
		std::cerr << "Ok. query test with bindings passed." << std::endl;
		return 0;
	}
	catch (const std::exception& err)
	{
		std::cerr << "EXCEPTION " << err.what() << std::endl;
	}
	return -1;
}


