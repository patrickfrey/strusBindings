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
			query.pushTerm( "word", term);
			query.pushDuplicate();
			query.defineFeature( "seek", 1.0);
		}
		if (terms.length > 0)
		{
			query.pushExpression( "contains", terms.length);
			query.defineFeature( "select", 1.0);
		}
		query.setMaxNofRanks( 20);
		query.setMinRank( 0);
		return query.evaluate();
	}

	public static QueryEval createQueryEval( StrusContext ctx) {
		QueryEval queryEval = ctx.createQueryEval();
		queryEval.addSelectionFeature( "select");
		WeightingConfig weighting = new WeightingConfig();
		weighting.defineParameter( "k1", 0.75);
		weighting.defineParameter( "b", 2.1);
		weighting.defineParameter( "avgdoclen", 1000);
		weighting.defineFeature( "match", "seek");
	
		queryEval.addWeightingFunction( 1.0, "BM25", weighting);
	
		SummarizerConfig sum_title = new SummarizerConfig();
		sum_title.defineParameter( "name", "title");
		queryEval.addSummarizer( "title", "attribute", sum_title);
	
		SummarizerConfig sum_match = new SummarizerConfig();
		sum_match.defineParameter( "type", "orig");
		sum_match.defineParameter( "nof", 4);
		sum_match.defineParameter( "len", 60);
		sum_match.defineFeature( "match", "seek");
		queryEval.addSummarizer( "summary", "matchphrase", sum_match);
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

