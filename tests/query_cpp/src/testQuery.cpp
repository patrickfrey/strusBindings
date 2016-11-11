/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
		queryAnalyzer.addSearchIndexElement(
			"word", "default", Tokenizer("word"), normalizer_stem);

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
		transaction.insertDocument( "doc_1", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_2'><title>Title_2</title><class>2</class><text>Alice has high marks in mathematics</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_2", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_3'><title>Title_3</title><class>3</class><text>Rose needs private tuition to improve here marks</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_3", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_4'><title>Title_4</title><class>4</class><text>Jack is good in sports</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_4", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_5'><title>Title_5</title><class>5</class><text>Tom has a very long walk to school</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_5", doc);

		doc = documentAnalyzer.analyze( "<doc id='X://doc_6'><title>Title_6</title><class>6</class><text>Martha loves all scientific subjects</text></doc>");
		doc.setUserAccessRight( "nobody");
		transaction.insertDocument( "doc_6", doc);

		transaction.commit();

		// Define the query evaluation program:
		QueryEval queryEval = ctx.createQueryEval();
		queryEval.addTerm( "punct", "punct", "dot");
		queryEval.addSelectionFeature( "weighted");
		queryEval.addRestrictionFeature( "weighted");

		SummarizerConfig titlesum;
		titlesum.defineParameter( "name", "title");
		queryEval.addSummarizer( "attribute", titlesum);
		SummarizerConfig docidsum;
		docidsum.defineParameter( "name", "docid");
		queryEval.addSummarizer( "attribute", docidsum);
		SummarizerConfig matchsum;
		matchsum.defineFeature( "match", "weighted");
		queryEval.addSummarizer( "matchpos", matchsum);

		WeightingConfig weightingConfig;
		weightingConfig.defineParameter( "k1", 0.75);
		weightingConfig.defineParameter( "b", 2.1);
		weightingConfig.defineParameter( "avgdoclen", 8);
		queryEval.addWeightingFunction( "BM25", weightingConfig);

		// Create a query from a phrase:
		Query query( queryEval.createQuery( storage));
		std::vector<Term> queryterms = queryAnalyzer.analyzeField( "default", "to school");
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
		query.addMetaDataRestrictionCondition( "<=", "class", 5, true);
		query.addMetaDataRestrictionCondition( ">=", "class", 1, true);
	
		query.setMaxNofRanks( 10);
		query.setMinRank( 0);

		query.addUserName( "nobody");

		std::set<std::string> expected;
		expected.insert( "X://doc_1");
		expected.insert( "X://doc_5");

		QueryResult result = query.evaluate();
		std::vector<Rank>::const_iterator ri = result.ranks().begin(), re = result.ranks().end();
		for (std::size_t ridx=1; ri != re; ++ridx,++ri)
		{
			std::string docid;
			std::cout << "[" << ridx << "] docno=" << ri->docno() << ", weight=" << ri->weight() << std::endl;
			std::vector<SummaryElement>::const_iterator
				ai = ri->summaryElements().begin(), ae = ri->summaryElements().end();
			for (std::size_t aidx=0; ai != ae; ++aidx,++ai)
			{
				if (aidx) std::cout << ", "; else std::cout << ' ';
				std::cout << ai->name() << "[" << ai->index() << "] = "
						<< ai->value() << " " << ai->weight();
				if (ai->name() == "docid") docid = ai->value();
			}
			if (docid.empty())
			{
				throw std::runtime_error( "found document has no docid defined");
			}
			std::set<std::string>::iterator ei = expected.find( docid);
			if (ei == expected.end())
			{
				throw std::runtime_error( "found document not expected");
			}
			expected.erase( ei);
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


