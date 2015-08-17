package net.strus.example;
import net.strus.api.*;
import java.io.*;
import java.util.List;

public class QueryNoAnalyzer
{
	public static RankVector evaluateQuery( StorageClient storage, QueryEval queryEval, String[] terms)
	{
		Query query = queryEval.createQuery( storage);
		for (String term : terms) {
			// We push the query terms on the stack and create a feature for each of it:
			query.pushTerm( "word", term);
			// Ever feature is duplicated on the stack, because we use them to 
			// build the selection expression that selects all documents for ranking
			// that contain all terms
			query.pushDuplicate();
			// We assign the features created to the set named 'seek' because they are 
			// referenced with this name in the query evaluation:
			query.defineFeature( "seek", 1.0);
		}
		if (terms.length > 0)
		{
			// Now we build the selection expression with the terms pushed as duplicates on
			// the stack in the loop before:
			query.pushExpression( "contains", terms.length);
			// We assign the feature created to the set named 'select' because this is the
			// name of the set defined as selection feature in the query evaluation configuration
			// (QueryEval.addSelectionFeature):
			query.defineFeature( "select");
		}
		query.setMaxNofRanks( 20);
		query.setMinRank( 0);
		return query.evaluate();
	}

	public static QueryEval createQueryEval( StrusContext ctx) {
		QueryEval queryEval = ctx.createQueryEval();
		// Here we define what query features decide, what is ranked for the result:
		queryEval.addSelectionFeature( "select");
		// Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
		WeightingConfig weighting = new WeightingConfig();
		weighting.defineParameter( "k1", 0.75);
		weighting.defineParameter( "b", 2.1);
		weighting.defineParameter( "avgdoclen", 1000);
		weighting.defineFeature( "match", "seek");
		queryEval.addWeightingFunction( 1.0, "BM25", weighting);
	
		// Now we define what attributes of the documents are returned and how they are build.
		// The functions that extract stuff from documents for presentation are called summarizers.
		// First we add a summarizer that extracts us the title of the document:
		SummarizerConfig sum_title = new SummarizerConfig();
		sum_title.defineParameter( "name", "title");
		queryEval.addSummarizer( "title", "attribute", sum_title);
	
		// Then we add a summarizer that collects the sections that enclose the best matches 
		// in a ranked document:
		SummarizerConfig sum_match = new SummarizerConfig();
		sum_match.defineParameter( "type", "orig");
		sum_match.defineParameter( "nof", 4);
		sum_match.defineParameter( "len", 60);
		sum_match.defineFeature( "match", "seek");
		queryEval.addSummarizer( "summary", "matchphrase", sum_match);
		// Now we are done:
		return queryEval;
	}

	public static void main( String[] args) {
		StrusContext ctx = new StrusContext();
		String config = "path=storage";
		if (args.length == 0)
		{
			args = new String[1];
			args[0] = "citi";
		}
		StorageClient storage = ctx.createStorageClient( config);
		QueryEval queryEval = createQueryEval( ctx);
		RankVector results = evaluateQuery( storage, queryEval, args);
		System.out.println( "Number of results: " + results.size());

		// We iterate on the results returned and we print them:
		int pos = 0;
		for (Rank result : results)
		{
			++pos;
			System.out.println( "rank " + pos + ": " + result.docno() + " " + result.weight() + ":");
			RankAttributeVector attributes = result.attributes();
			for (RankAttribute attribute : attributes)
			{
				System.out.println( "\t" + attribute.name() + ": " + attribute.value());
			}
		}
		System.out.println( "done");
	}
} 

