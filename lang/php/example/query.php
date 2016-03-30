<?php
require "strus.php";

$queryphrase = "city";

$config = "path=storage";
$ctx = new StrusContext();

try
{
	# Get a client for the storage:
	$storage = $ctx->createStorageClient( $config);

	# Define the query analyzer to use:
	$analyzer = $ctx->createQueryAnalyzer();
	$analyzer->definePhraseType( "word", "word", "word", array( array( "stem","en"),"lc",array( "convdia","en")));

	# Define the query evaluation scheme:
	$queryeval = $ctx->createQueryEval();

	# Here we define what query features decide, what is ranked for the result:
	$queryeval->addSelectionFeature( "select");
	
	# Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
	$queryeval->addWeightingFunction( "BM25", array( "k1"=>1.5, "b"=>0.75, "avgdoclen"=>5, ".match"=>"seek"));

	# Here we define a weighting formula to combine all weighting functions to one value:
	$queryeval->addWeightingFormula( "((x + y + 0.1) * _0)", array( "x"=>0.4, "y"=>0.6));

	# Now we define what attributes of the documents are returned and how they are build.
	# The functions that extract stuff from documents for presentation are called summarizers.
	# First we add a summarizer that extracts us the title of the document:
	$queryeval->addSummarizer( "attribute", array( "name"=>"title"));

	# Then we add a summarizer that collects the sections that enclose the best matches 
	# in a ranked document:
	$queryeval->addSummarizer( "matchphrase", array( "type"=>"orig","sentencesize"=>40,"windowsize"=>60,".match"=>"seek"));

	# Now we build the query to issue:
	$query = $queryeval->createQuery( $storage);

	# First we analyze the query phrase to get the terms to find in the form as they are stored in the storage:
	$terms = $analyzer->analyzePhrase( "word", $queryphrase);
	if (count( $terms) == 0)
	{
		throw new Exception("query is empty");
	}
	# Then we iterate on the terms and create a single term feature for each term and collect
	# all terms to create a selection expression out of them:
	$selexpr = new StrusQueryExpression();

	foreach ($terms as &$term)
	{
		$expr = new StrusQueryExpression();
		# We push the query terms on the stack and create a query feature 'seek' 
		# for each of it:
		$expr->pushTerm( $term->type, $term->value);
		# We assign the features created to the set named 'seek' because they are 
		# referenced with this name in the query evaluation:
		$query->defineFeature( "seek", $expr, 1.0);
		# Each query term is also part of the selection expressions
		$selexpr->pushTerm( $term->type, $term->value);
		print "term " . $term->type . " " . $term->value . " (" . $selexpr->size() . ")";
	}
	# Create a selection feature 'select' that matches documents that contain all query terms.
	$selexpr->pushExpression( "contains", count( $terms));
	# We assign the feature created to the set named 'select' because this is the
	# name of the set defined as selection feature in the query evaluation configuration
	# ($queryeval->addSelectionFeature):
	$query->defineFeature( "select", $selexpr, 1.0);

	# Redefine a weighting function variable:
	$query->setWeightingVariables( array( "x"=>0.2));

	# Define the maximum number of best result (ranks) to return:
	$query->setMaxNofRanks( 20);
	# Define the index of the first rank (for implementing scrolling: 0 for the first, 
	# 20 for the 2nd, 40 for the 3rd page, etc.):
	$query->setMinRank( 0);
	
	# Now we evaluate the query and iterate on the result to display them:
	$results = $query->evaluate();
	$pos = 0;
	foreach ($results->ranks as &$rank)
	{
		$pos += 1;
		print "rank " . $pos . ": " . $rank->docno . " " . $rank->weight . ":\n";
		foreach ($rank->summaryElements as &$sumelem)
		{
			print "\t" . $sumelem->name . ": " . $sumelem->value . "\n";
		}
	}
	print "done\n";
	exit(0);
}
catch (Exception $err)
{
	print "Error: " . $err . "\n";
	exit(-1);
}

