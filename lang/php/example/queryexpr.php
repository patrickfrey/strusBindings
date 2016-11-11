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
	$analyzer->addSearchIndexElement( "word", "word", "word", array( array( "stem","en"),"lc",array( "convdia","en")));

	# Define the query evaluation scheme:
	$queryeval = $ctx->createQueryEval();

	# Here we define what query features decide, what is ranked for the result:
	$queryeval->addSelectionFeature( "select");

	# Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
	$queryeval->addWeightingFunction( "BM25", array( "k1"=>0.75, "b"=>2.1, "avgdoclen"=>1000, ".match"=>"seek"));

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
	$terms = $analyzer->analyzeField( "word", $queryphrase);
	if (count( $terms) == 0)
	{
		throw new Exception("query is empty");
	}
	# Then we iterate on the terms and create a single term feature for each term and collect
	# all terms to create a selection expression out of them:
	$selexpr = array( "contains" );

	foreach ($terms as &$term)
	{
		# We assign the features created to the set named 'seek' because they are 
		# referenced with this name in the query evaluation:
		$query->defineFeature( "seek", array( $term->type,$term->value), 1.0 );
		# Each query term is also part of the selection expressions
		$selexpr[] = array( $term->type, $term->value );
	}
	# We assign the feature created to the set named 'select' because this is the
	# name of the set defined as selection feature in the query evaluation configuration
	# ($queryeval->addSelectionFeature):
	$query->defineFeature( "select", $selexpr, 1.0);
	
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
	exit(1);
}
?>

