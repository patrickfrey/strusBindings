strus PHP bindings	 {#mainpage}
==================

The strus PHP interface provides objects for accessing the 
retrieval storage, indexing documents and queries and evaluating queries.
It has been built using SWIG based on a wrapper C++ interface (@ref strus/bindingObjects.hpp).

<b>Because of lacking namespaces all type names of the strus PHP extension have the prefix <i>Strus</i></b>.
The class names in the interface documentation are without the <i>Strus</i> prefix. 
I failed to persuade doxygen to create the names with the <i>Strus</i> prefix.

The entry point of a strus application with PHP is the context (@ref Context) object (<b>StrusContext</b>).
It is the root object from which all other objects are created.
It can be constructed either as proxy, that redirects all method calls to an RpcServer
or it can be constructed as instance running in the PHP environment.
Because it is most likely that PHP will run in a webserver context, it is recommended
to use the RPC proxy variant there.

You'll find a tutorial based on a docker image for how to build a search engine with strus and PHP <a href="http://www.codeproject.com/Articles/1009582/Building-a-search-engine-with-Strus-tutorial">here</a>.
The following examples illustrate the usage of the PHP bindings.
They are taken from tests built and executed in the strusBindings project.

\section createCollection Create a collection, analyze and insert 3 documents 
\code
<?php
require "strus.php";

$config = "path=storage; metadata=doclen UINT16";
$ctx = new StrusContext();
try {
	$ctx->destroyStorage( $config);
	# ... delete the storage files if they already exists
} catch( Exception $e ) {
}

# Create a new storage:
$ctx->createStorage( $config);
# Get a client for the new created storage:
$storage = $ctx->createStorageClient( $config);

# Define the document analyzer to use:
$analyzer = $ctx->createDocumentAnalyzer();

# Define the features and attributes to store:
$analyzer->defineAttribute( "title", "/doc/title()", "content", "orig");
$analyzer->addSearchIndexFeature( "word", "/doc/text()", "word", [["stem","en"],"lc",["convdia","en"]], []);
$analyzer->addForwardIndexFeature( "orig", "/doc/text()", "split", "orig", []);

# Read input files, analyze and insert them:
$datadir = "./data/";
$datafiles = scandir( $datadir);
foreach ($datafiles as &$datafile)
{
	if ($datafile[0] != '.')
	{
		print "process document " . $datafile . "\n";
		$docid = substr( $datafile, 0, strlen($datafile) -4);
		$fullname = $datadir . $datafile;
		$doc = $analyzer->analyze( file_get_contents( $fullname));
		$storage->insertDocument( $docid, $doc);
	}
}
# Without this the documents wont be inserted:
$storage->flush();
print "done";
?>
\endcode

\section status Query some status info, the number of documents inserted
\code
<?php
require "strus.php";

$ctx = new StrusContext();

$config = "path=storage; metadata=doclen UINT16";
$storage = $ctx->createStorageClient( $config);

# Query the number of documents inserted:
$nofDocuments = $storage->nofDocumentsInserted();

# Output:
print "Number of documents inserted: $nofDocuments\n";
?>
\endcode

\section query Issue a BM25 query on the created collection (first example) and show the best ranked documents
\code
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
	$analyzer->definePhraseType( "word", "word", "word", [["stem","en"],"lc",["convdia","en"]]);

	# Define the query evaluation scheme:
	$queryeval = $ctx->createQueryEval();

	# Here we define what query features decide, what is ranked for the result:
	$queryeval->addSelectionFeature( "select");
	
	# Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
	$queryeval->addWeightingFunction( 1.0, "BM25", ["k1"=>0.75, "b"=>2.1, "avgdoclen"=>1000, ".match"=>"seek"]);
	
	# Now we define what attributes of the documents are returned and how they are build.
	# The functions that extract stuff from documents for presentation are called summarizers.
	# First we add a summarizer that extracts us the title of the document:
	$queryeval->addSummarizer( "title", "attribute", ["name"=>"title"]);

	# Then we add a summarizer that collects the sections that enclose the best matches 
	# in a ranked document:
	$queryeval->addSummarizer( "summary", "matchphrase", ["type"=>"orig","nof"=>4,"len"=>60,".match"=>"seek"]);

	# Now we build the query to issue:
	$query = $queryeval->createQuery( $storage);
	
	# First we analyze the query phrase to get the terms to find in the form as they are stored in the storage:
	$terms = $analyzer->analyzePhrase( "word", $queryphrase);
	if (count( $terms) == 0)
	{
		throw Exception("query is empty");
	}
	# Then we iterate on the terms and create a single term feature for each term and collect
	# all terms to create a selection expression out of them:
	$selexpr = [ "contains" ];

	foreach ($terms as &$term)
	{
		# We assign the features created to the set named 'seek' because they are 
		# referenced with this name in the query evaluation:
		$query->defineFeature( "seek", [ $term->type,$term->value], 1.0 );
		# Each query term is also part of the selection expressions
		$selexpr[] = [ $term->type, $term->value ];
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
	foreach ($results as &$result)
	{
		$pos += 1;
		print "rank " . $pos . ": " . $result->docno . " " . $result->weight . ":\n";
		foreach ($result->attributes as &$attribute)
		{
			print "\t" . $attribute->name . ": " . $attribute->value . "\n";
		}
	}
	print "done\n";
}
catch (Exception $err)
{
	print "Error: " . $err . "\n";
}
?>
\endcode



