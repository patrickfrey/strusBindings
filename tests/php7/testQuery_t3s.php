<?php
include_once "utils.php";
include_once "config_t3s.php";
include_once "createCollection.php";
include_once "dumpCollection.php";

$datadir = "../data/t3s/";
if (isset($argv[1])) {
	$datadir = $argv[1];
}
$outputdir = '.';
if (isset($argv[2])) {
	$outputdir = $argv[2];
}
$ctxconfig = getContextConfig( $argv[3]);
$storagedir = $outputdir . "/storage";
$docfiles = ["A.xml","B.xml","C.xml"];
$withrpc = ($ctxconfig != NULL and isset($ctxconfig['rpc']));

$storageConfig = NULL;
$ctx = new StrusContext( $ctxconfig);
if (!$withrpc) {
	$storageConfig = "path='" . $storagedir . "';cache=512M";
}

createCollection( $ctx, $storagedir, metadata_t3s(), createDocumentAnalyzer_t3s( $ctx), False, $datadir, $docfiles, NULL, $withrpc);
$queryPhrase = "City visit tokyo";

// Get a client for the new created storage:
$storage = $ctx->createStorageClient( $storageConfig);

// Define the query analyzer to use:
$analyzer = createQueryAnalyzer_t3s( $ctx);

// Define the query evaluation scheme:
$queryEval = createQueryEval_t3s( $ctx);

// Now we build the query to issue:
$query = $queryEval->createQuery( $storage);

// First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
$terms = $analyzer->analyzeTermExpression( ["word",$queryPhrase]);
$qexpr = $analyzer->analyzeTermExpression( [
					"sequence", 10,
						["sequence", 2, ["word","completely"], ["word","different"]],
						["sequence", 3, ["word","you"], ["word","expect"]]
				]);

$output[ "QueryString"] = $queryPhrase;
$output[ "QueryTerms"] = $terms;
$output[ "QueryExpression"] = $qexpr;

if (empty( $terms)) {
	fwrite(STDERR, "query is empty");
	exit(1);
}

if (!$withrpc) {
	// ... For RPC there are no posting iterators implemented
	$postings = [];
	foreach ($terms as $ti => $qe) {
		foreach ($storage->postings( $qe) as $po) {
			$docno = $po[0];
			$pos = $po[1];
			$postings[ "term $ti doc " . $storage->docid($docno) ] = $pos;
		}
	}
	foreach ($qexpr as $qi => $qe) {
		foreach ($storage->postings( $qe) as $po) {
			$docno = $po[0];
			$pos = $po[1];
			$postings[ "expr $qi doc " . $storage->docid($docno) ] = $pos;
		}
	}
	$output[ "postings"] = $postings;
}

// Then we iterate on the terms and create a single term feature for each term and collect
// all terms to create a selection expression out of them:
$selexpr = ["contains", 0, 1];

foreach ($terms as $term) {
	// Each query term is also part of the selection expressions
	array_push( $selexpr, $term);
	// Create a document feature 'seek'.
	$query->addFeature( "seek", $term, 1.0);
}
// We assign the feature created to the set named 'select' because this is the
// name of the set defined as selection feature in the query evaluation configuration
// (QueryEval.addSelectionFeature):
$query->addFeature( "select", $selexpr);

// Define the maximum integer of best result (ranks) to return:
$query->setMaxNofRanks( 20);
// Define the index of the first rank (for implementing scrolling: 0 for the first, 
// 20 for the 2nd, 40 for the 3rd page, etc.):
$query->setMinRank( 0);

// Define the title field:
$query->addFeature( "titlefield", [from => "title_start", to => "title_end"] );

// Enable debugging
$query->setDebugMode( False );

// Dump query to output
$output[ "QueryDump"] = $query->introspection();

// Now we evaluate the query and iterate on the result to display them:
$results = $query->evaluate();
$output[ "QueryResult"] = $results;
$output_list = [];
foreach ($results->ranks as $pos => $result) {
	$weightstr = number_format((float)$result->weight, 5, '.', '');
	$docno = $result->docno;
	array_push( $output_list, "rank $pos: $weightstr");
	foreach ($result->summary as $sidx => $si) {
		array_push( $output_list, "    " . $si->name . " '" . $si->value . "'");
	}
}
$output[ "ResultList"] = $output_list;

$result = "query evaluation:" . dumpTreeWithFilter( $output, ['docno']);
$expected = <<<END_expected
query evaluation:
string QueryDump: 
  string debug: "false"
  string eval: 
    string selection_sets: 
      integer 0: "select"
    string summarizers: 
      integer 0: 
        string function: "attribute"
        string param: 
          string attribute: "title"
          string debug: "debug_attribute"
          string result: "title"
      integer 1: 
        string function: "attribute"
        string param: 
          string attribute: "docid"
          string debug: "debug_attribute"
          string result: "docid"
      integer 2: 
        string feature: 
          string match: "seek"
          string title: "titlefield"
        string function: "matchphrase"
        string param: 
          string cardinality: 0
          string debug: "debug_matchphrase"
          string features: 
            string docstart: "docstart"
            string para: "para"
            string phrase: "phrase"
          string floatingmark: 
            string end: " ..."
            string start: "... "
          string matchmark: 
            string end: ""
            string start: ""
          string maxdf: 0.1
          string paragraphsize: 300
          string sentencesize: 40
          string type: "orig"
          string windowsize: 30
    string weighting: 
      integer 0: 
        string feature: 
          string match: "seek"
        string function: "BM25"
        string param: 
          string avgdoclen: 1000
          string b: 2.1
          string debug: "debug_weighting"
          string k1: 0.75
          string metadata_doclen: "doclen"
    string weighting_sets: 
      integer 0: "seek"
  string feature: 
    integer 0: 
      string set: "seek"
      string struct: 
        string node: "term"
        string type: "word"
        string value: "citi"
        string var: 
      string weight: 1
    integer 1: 
      string set: "seek"
      string struct: 
        string node: "term"
        string type: "word"
        string value: "visit"
        string var: 
      string weight: 1
    integer 2: 
      string set: "seek"
      string struct: 
        string node: "term"
        string type: "word"
        string value: "tokyo"
        string var: 
      string weight: 1
    integer 3: 
      string set: "select"
      string struct: 
        string arg: 
          integer 0: 
            string node: "term"
            string type: "word"
            string value: "citi"
            string var: 
          integer 1: 
            string node: "term"
            string type: "word"
            string value: "visit"
            string var: 
          integer 2: 
            string node: "term"
            string type: "word"
            string value: "tokyo"
            string var: 
        string cardinality: 1
        string node: "expression"
        string op: "contains"
        string range: 0
        string var: 
      string weight: 1
    integer 4: 
      string set: "titlefield"
      string struct: 
        string end: "title_end"
        string node: "docfield"
        string start: "title_start"
        string var: 
      string weight: 1
  string merge: 0
  string minrank: 0
  string nofranks: 20
string QueryExpression: 
  integer 0: 
    string arg: 
      integer 0: 
        string arg: 
          integer 0: 
            string type: "word"
            string value: "complet"
          integer 1: 
            string type: "word"
            string value: "differ"
        string op: "sequence"
        string range: 2
      integer 1: 
        string arg: 
          integer 0: 
            string type: "word"
            string value: "you"
          integer 1: 
            string type: "word"
            string value: "expect"
        string op: "sequence"
        string range: 3
    string op: "sequence"
    string range: 10
string QueryResult: 
  string evalpass: 0
  string nofranked: 3
  string nofvisited: 3
  string ranks: 
    integer 0: 
      string summary: 
        integer 0: 
          string name: "title"
          string value: "One day in Tokyo"
        integer 1: 
          string name: "docid"
          string value: "A.xml"
        integer 2: 
          string name: "docstart"
          string value: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen. ..."
      string weight: 0.64282
    integer 1: 
      string summary: 
        integer 0: 
          string name: "title"
          string value: "A visit in New York"
        integer 1: 
          string name: "docid"
          string value: "B.xml"
        integer 2: 
          string name: "docstart"
          string value: "A visit in New York New York is a city with dimensions you can't imagine. ..."
      string weight: 0.00017
    integer 2: 
      string summary: 
        integer 0: 
          string name: "title"
          string value: "A journey through Germany"
        integer 1: 
          string name: "docid"
          string value: "C.xml"
        integer 2: 
          string name: "docstart"
          string value: "A journey through Germany When I first visited germany it was still splitted into two parts. ..."
      string weight: 9.0E-5
string QueryString: "City visit tokyo"
string QueryTerms: 
  integer 0: 
    string type: "word"
    string value: "citi"
  integer 1: 
    string type: "word"
    string value: "visit"
  integer 2: 
    string type: "word"
    string value: "tokyo"
string ResultList: 
  integer 0: "rank 0: 0.64282"
  integer 1: "    title 'One day in Tokyo'"
  integer 2: "    docid 'A.xml'"
  integer 3: "    docstart 'One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen. ...'"
  integer 4: "rank 1: 0.00017"
  integer 5: "    title 'A visit in New York'"
  integer 6: "    docid 'B.xml'"
  integer 7: "    docstart 'A visit in New York New York is a city with dimensions you can't imagine. ...'"
  integer 8: "rank 2: 0.00009"
  integer 9: "    title 'A journey through Germany'"
  integer 10: "    docid 'C.xml'"
  integer 11: "    docstart 'A journey through Germany When I first visited germany it was still splitted into two parts. ...'"
string postings: 
  string expr 0 doc A.xml: 
    integer 0: 12
  string term 0 doc A.xml: 
    integer 0: 9
  string term 0 doc B.xml: 
    integer 0: 11
  string term 1 doc B.xml: 
    integer 0: 2
  string term 1 doc C.xml: 
    integer 0: 9
  string term 2 doc A.xml: 
    integer 0: 4
    integer 1: 6
END_expected;

verifyTestOutput( $outputdir, $result, $expected);
?>


