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
#include <memory>
#include <boost/scoped_ptr.hpp>

int main( int , const char** )
{
	try
	{
		// Define document analyzer:
		DocumentAnalyzer documentAnalyzer;
		documentAnalyzer.addSearchIndexFeature(
			"word", "/doc/text()", FunctionDef("word"), FunctionDef("stem", "de"));
		documentAnalyzer.addForwardIndexFeature(
			"orig", "/doc/text()", FunctionDef("word"), FunctionDef("orig"));
		documentAnalyzer.defineMetaData(
			"date", "/doc/date()", FunctionDef("content"), FunctionDef("orig"));
		documentAnalyzer.defineMetaData(
			"class", "/doc/class()", FunctionDef("content"), FunctionDef("orig"));
		documentAnalyzer.defineAttribute(
			"title", "/doc/title()", FunctionDef("content"), FunctionDef("orig"));
		documentAnalyzer.defineAttribute(
			"docid", "/doc@id", FunctionDef("content"), FunctionDef("orig"));

		// Define query analyzer:
		QueryAnalyzer queryAnalyzer;
		queryAnalyzer.definePhraseType(
			"default", "word", FunctionDef("word"), FunctionDef("stem", "de"));

		// Create storage:
		try
		{
			Storage::destroy( "path=data/test");
		}
		catch (...)
		{}
		Storage::create( "path=data/test; acl=yes; metadata=doclen UINT16, class UINT16, data INT32");
		Storage storage( "path=data/test");

		// Insert documents:
		Document doc;
		doc = documentAnalyzer.analyze( "<doc id='X://doc_1'><title>Title_1</title><class>1</class><text>Bob likes to go to school</text></doc>");
		doc.setUserAccessRight( "nobody");
		storage.insertDocument( "doc_1", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_2'><title>Title_2</title><class>2</class><text>Alice has high marks in mathematics</text></doc>");
		doc.setUserAccessRight( "nobody");
		storage.insertDocument( "doc_2", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_3'><title>Title_3</title><class>3</class><text>Rose needs private tuition to improve here marks</text></doc>");
		doc.setUserAccessRight( "nobody");
		storage.insertDocument( "doc_3", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_4'><title>Title_4</title><class>4</class><text>Jack is good in sports</text></doc>");
		doc.setUserAccessRight( "nobody");
		storage.insertDocument( "doc_4", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_5'><title>Title_5</title><class>5</class><text>Tom has a very long walk to school</text></doc>");
		doc.setUserAccessRight( "nobody");
		storage.insertDocument( "doc_5", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_6'><title>Title_6</title><class>6</class><text>Martha loves all scientific subjects</text></doc>");
		doc.setUserAccessRight( "nobody");
		storage.insertDocument( "doc_6", doc);

		storage.flush();

		// Define the query evaluation program:
		QueryEval queryEval( storage);
		queryEval.addTerm( "punct", "punct", "dot");
		queryEval.addSelectionFeature( "weighted");
		queryEval.addRestrictionFeature( "weighted");
		queryEval.addWeightingFeature( "weighted");

		Summarizer titlesum( "attribute");
		titlesum.defineParameter( "name", "title");
		queryEval.addSummarizer( "title", titlesum);
		Summarizer docidsum( "attribute");
		docidsum.defineParameter( "name", "docid");
		queryEval.addSummarizer( "docid", docidsum);
		Summarizer matchsum( "matchpos");
		matchsum.defineFeature( "match", "weighted");
		queryEval.addSummarizer( "match", matchsum);

		WeightingFunction weightingFunction( "BM25");
		weightingFunction.defineParameter( "k1", 0.75);
		weightingFunction.defineParameter( "b", 2.1);
		weightingFunction.defineParameter( "avgdoclen", 8);
		queryEval.defineWeightingFunction( weightingFunction);

		// Create a query from a phrase:
		Query query( queryEval);
		std::vector<Term> queryterms = queryAnalyzer.analyzePhrase( "default", "to school");
		std::vector<Term>::const_iterator qi = queryterms.begin(), qe = queryterms.end();
		std::size_t qidx = 0;
		unsigned int maxpos = 0;
		for (; qi != qe; ++qidx,++qi)
		{
			query.pushTerm( qi->typeName(), qi->value());
			if (qi->position() > maxpos)
			{
				maxpos = qi->position();
			}
		}
		query.pushExpression( "sequence", qidx, maxpos);
		query.defineFeature( "weighted", 1.0);

		// Evaluate the query and print the result:
		query.defineMetaDataRestriction( "<=", "class", 5);
		query.defineMetaDataRestriction( ">=", "class", 1);
	
		query.setMaxNofRanks( 10);
		query.setMinRank( 0);

		query.setUserName( "nobody");
	
		std::vector<Rank> result = query.evaluate();
		std::vector<Rank>::const_iterator ri = result.begin(), re = result.end();
		for (std::size_t ridx=1; ri != re; ++ridx,++ri)
		{
			std::cout << "[" << ridx << "] docno=" << ri->docno() << ", weight=" << ri->weight();
			std::vector<RankAttribute>::const_iterator
				ai = ri->attributes().begin(), ae = ri->attributes().end();
			for (std::size_t aidx=0; ai != ae; ++aidx,++ai)
			{
				if (aidx) std::cout << ", "; else std::cout << ' ';
				std::cout << ai->name() << "=" << ai->value();
			}
			std::cout << std::endl;
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


