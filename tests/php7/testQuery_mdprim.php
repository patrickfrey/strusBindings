<?php
include_once "utils.php";
include_once "config_mdprim.php";
include_once "createCollection.php";
include_once "dumpCollection.php";

try
{
	$datadir = "../data/mdprim/";
	if (isset($argv[1])) {
		$datadir = $argv[1];
	}
	$outputdir = '.';
	if (isset($argv[2])) {
		$outputdir = $argv[2];
	}
	$ctxconfig = getContextConfig( $argv[3]);
	$storagedir = $outputdir . "/storage";
	$docfiles = ["doc1000.xml"];
	$output = [];
	$withrpc = ($ctxconfig != NULL and isset($ctxconfig['rpc']));
	
	$storageConfig = NULL;
	$ctx = new StrusContext( $ctxconfig);
	if (!$withrpc) {
		$storageConfig = "path='" . $storagedir . "';metadata='" . metadata_mdprim() . "';cache=512M";
	}
	
	$aclmap = [];
	for ($i = 1; $i < 1000; $i++) {
		$usr = [];
		if ($i > 500) {
			array_push( $usr, 'large');
		} else {
			array_push( $usr, 'small');
		}
		if ($i < 50) {
			array_push( $usr, 'tiny');
		}
		if ($i > 950) {
			array_push( $usr, 'huge');
		}
		$aclmap[ "$i"] = $usr;
	}
	
	createCollection( $ctx, $storagedir, metadata_mdprim(), createDocumentAnalyzer_mdprim( $ctx), True, $datadir, $docfiles, $aclmap, $withrpc);
	
	$queryPhrase = "2 3";
	
	// Get a client for the new created storage:
	$storage = $ctx->createStorageClient( $storageConfig);
	
	// Define the query analyzer to use:
	$analyzer = createQueryAnalyzer_mdprim( $ctx);
	
	// Define the query evaluation scheme:
	$queryEval = createQueryEval_mdprim( $ctx);
	
	// Now we build the query to issue:
	$query = $queryEval->createQuery( $storage);
	
	// First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
	$terms = $analyzer->analyzeTermExpression( ["word",$queryPhrase]);
	
	// $texpr = $analyzer->analyzeTermExpression( ["union", ["sequence", 10, ["sequence", 10, ["word","2"], ["word","3"]], ["word","5"]], ["sequence", 10, ["sequence", 10, ["word","2"], ["word","5"]], ["word","7"]] ] );
	$mexpr = $analyzer->analyzeMetaDataExpression( [ [["<","cross","010 "],[">","cross"," 14"]], [">","factors"," 0 "] ] );
	$texpr_plain = [
				["sequence_imm", ["word","2"], ["word","2"], ["word","2"], ["word","2"]],
				["sequence_imm", ["sequence_imm", ["word","2"], ["word","3"]], ["union", ["sequence_imm", ["word","3"], ["word","5"]], ["word","5"], ["word","7"], ["word","11"]]]
			];
	$texpr = $analyzer->analyzeTermExpression( $texpr_plain);
	
	$output[ "QueryString"] = $queryPhrase;
	$output[ "QueryTerms"] = $terms;
	$output[ "QueryExpr"] = $texpr;
	$output[ "QueryRestr"] = $mexpr;
	
	if (empty( $terms)) {
		fwrite(STDERR, "query is empty");
		exit(1);
	}
	
	// Then we iterate on the terms and create a single term feature for each term and collect
	// all terms to create a selection expression out of them:
	$selexpr = ["contains", 0, 2];
	
	foreach ($terms as $term) {
		// Each query term is also part of the selection expressions
		array_push( $selexpr, $term);
	}
	$query->addFeature( "seek", $texpr);
	
	// We assign the feature created to the set named 'select' because this is the
	// name of the set defined as selection feature in the query evaluation configuration
	// (QueryEval.addSelectionFeature):
	$query->addFeature( "select", $selexpr);
	
	// Define the maximum number of best result (ranks) to return:
	$query->setMaxNofRanks( 20);
	// Define the index of the first rank (for implementing scrolling: 0 for the first, 
	// 20 for the 2nd, 40 for the 3rd page, etc.):
	$query->setMinRank( 0);
	
	// Query restriction
	$query->addMetaDataRestriction( [ [["<","cross",10],[">","cross",14]], [">","factors",0] ] );
	$query->addMetaDataRestriction( $mexpr );
	
	// Enable debugging
	$query->setDebugMode( False );
	
	// Restrict document access
	$query->addAccess( ["tiny","huge","small"] );
	
	// Dump query to output
	$output[ "QueryDump"] = $query->introspection( NULL);
	
	// Now we evaluate the query and iterate on the result to display them:
	$results = $query->evaluate();
	$output[ "QueryResult"] = $results;
	$output_list = [];
	foreach ($results->ranks as $pos => $result) {
		$weightstr = number_format((float)$result->weight, 5, '.', '');
		array_push( $output_list, "rank $pos: $weightstr");
		foreach ($result->summary as $sidx => $si) {
			array_push( $output_list, "    " . $si->name . ": '" . $si->value . "'");
		}
	}
	
	$output[ "ResultList"] = $output_list;
	
	$result = "query evaluation:" . dumpTreeWithFilter( $output, ['docno']);
}
catch (Error $e)
{
	echo "ERROR in script: $e\n";
	exit( -1);
}
catch (Exception $e)
{
	echo "ERROR in script: $e\n";
	exit( -1);
}
$expected = <<<END_expected
query evaluation:
string QueryDump: 
  string debug: "false"
  string docs: 
  string eval: 
    string formula: 
      integer 0: 
        string arg: 0
        string ip: 0
        string opcode: "Arg"
      integer 1: 
        string arg: 1
        string ip: 1
        string opcode: "Arg"
      integer 2: 
        string ip: 2
        string opcode: "Div"
      integer 3: 
        string arg: 1000
        string ip: 3
        string opcode: "Push"
      integer 4: 
        string arg: 2
        string ip: 4
        string opcode: "Arg"
      integer 5: 
        string ip: 5
        string opcode: "Sub"
      integer 6: 
        string arg: 1000000
        string ip: 6
        string opcode: "Push"
      integer 7: 
        string ip: 7
        string opcode: "Div"
      integer 8: 
        string ip: 8
        string opcode: "Add"
    string selection_sets: 
      integer 0: "select"
    string summarizers: 
      integer 0: 
        string debugattr: "debug_attribute"
        string def: 
          string attribute: "docid"
          string resultname: "docid"
        string function: "attribute"
      integer 1: 
        string debugattr: "debug_metadata"
        string def: 
          string metaname: "cross"
          string resultname: "cross"
        string function: "metadata"
      integer 2: 
        string debugattr: "debug_metadata"
        string def: 
          string metaname: "factors"
          string resultname: "factors"
        string function: "metadata"
      integer 3: 
        string debugattr: "debug_metadata"
        string def: 
          string metaname: "lo"
          string resultname: "lo"
        string function: "metadata"
      integer 4: 
        string debugattr: "debug_metadata"
        string def: 
          string metaname: "hi"
          string resultname: "hi"
        string function: "metadata"
      integer 5: 
        string debugattr: "debug_forwardindex"
        string def: 
          string N: 100
          string resultname: "word"
          string type: "word"
        string function: "forwardindex"
    string weighting: 
      integer 0: 
        string debugattr: "debug_weighting"
        string def: 
        string function: "tf"
        string parameter: 
          string match: "seek"
      integer 1: 
        string debugattr: "debug"
        string def: 
          string name: "doclen"
          string weight: 1
        string function: "metadata"
      integer 2: 
        string debugattr: "debug"
        string def: 
          string name: "docidx"
          string weight: 1
        string function: "metadata"
    string weighting_sets: 
      integer 0: "seek"
  string feature: 
    integer 0: 
      string set: "seek"
      string struct: 
        string arg: 
          integer 0: 
            string arg: 
              integer 0: 
                string node: "term"
                string type: "word"
                string value: 2
                string var: 
              integer 1: 
                string node: "term"
                string type: "word"
                string value: 3
                string var: 
            string cardinality: 0
            string node: "expression"
            string op: "sequence_imm"
            string range: 0
            string var: 
          integer 1: 
            string arg: 
              integer 0: 
                string arg: 
                  integer 0: 
                    string node: "term"
                    string type: "word"
                    string value: 3
                    string var: 
                  integer 1: 
                    string node: "term"
                    string type: "word"
                    string value: 5
                    string var: 
                string cardinality: 0
                string node: "expression"
                string op: "sequence_imm"
                string range: 0
                string var: 
              integer 1: 
                string node: "term"
                string type: "word"
                string value: 5
                string var: 
              integer 2: 
                string node: "term"
                string type: "word"
                string value: 7
                string var: 
              integer 3: 
                string node: "term"
                string type: "word"
                string value: 11
                string var: 
            string cardinality: 0
            string node: "expression"
            string op: "union"
            string range: 0
            string var: 
        string cardinality: 0
        string node: "expression"
        string op: "sequence_imm"
        string range: 0
        string var: 
      string weight: 1
    integer 1: 
      string set: "seek"
      string struct: 
        string arg: 
          integer 0: 
            string node: "term"
            string type: "word"
            string value: 2
            string var: 
          integer 1: 
            string node: "term"
            string type: "word"
            string value: 2
            string var: 
          integer 2: 
            string node: "term"
            string type: "word"
            string value: 2
            string var: 
          integer 3: 
            string node: "term"
            string type: "word"
            string value: 2
            string var: 
        string cardinality: 0
        string node: "expression"
        string op: "sequence_imm"
        string range: 0
        string var: 
      string weight: 1
    integer 2: 
      string set: "select"
      string struct: 
        string arg: 
          integer 0: 
            string node: "term"
            string type: "word"
            string value: 2
            string var: 
          integer 1: 
            string node: "term"
            string type: "word"
            string value: 3
            string var: 
        string cardinality: 2
        string node: "expression"
        string op: "contains"
        string range: 0
        string var: 
      string weight: 1
  string maxNofRanks: 20
  string minRank: 0
  string user: 
    integer 0: "tiny"
    integer 1: "huge"
    integer 2: "small"
string QueryExpr: 
  integer 0: 
    string arg: 
      integer 0: 
        string type: "word"
        string value: 2
      integer 1: 
        string type: "word"
        string value: 2
      integer 2: 
        string type: "word"
        string value: 2
      integer 3: 
        string type: "word"
        string value: 2
    string op: "sequence_imm"
  integer 1: 
    string arg: 
      integer 0: 
        string arg: 
          integer 0: 
            string type: "word"
            string value: 2
          integer 1: 
            string type: "word"
            string value: 3
        string op: "sequence_imm"
      integer 1: 
        string arg: 
          integer 0: 
            string arg: 
              integer 0: 
                string type: "word"
                string value: 3
              integer 1: 
                string type: "word"
                string value: 5
            string op: "sequence_imm"
          integer 1: 
            string type: "word"
            string value: 5
          integer 2: 
            string type: "word"
            string value: 7
          integer 3: 
            string type: "word"
            string value: 11
        string op: "union"
    string op: "sequence_imm"
string QueryRestr: 
  integer 0: 
    integer 0: 
      string name: "cross"
      string op: "<"
      string value: 10
    integer 1: 
      string name: "cross"
      string op: ">"
      string value: 14
  integer 1: 
    string name: "factors"
    string op: ">"
    string value: 0
string QueryResult: 
  string evalpass: 0
  string nofranked: 70
  string nofvisited: 91
  string ranks: 
    integer 0: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 384
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 15
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 7
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 3
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 2
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 2
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 2
          string weight: 1
        integer 10: 
          string index: 6
          string name: "word"
          string value: 2
          string weight: 1
        integer 11: 
          string index: 7
          string name: "word"
          string value: 2
          string weight: 1
        integer 12: 
          string index: 8
          string name: "word"
          string value: 3
          string weight: 1
      string weight: 0.50062
    integer 1: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 960
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 15
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 7
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 2
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 2
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 2
          string weight: 1
        integer 10: 
          string index: 6
          string name: "word"
          string value: 2
          string weight: 1
        integer 11: 
          string index: 7
          string name: "word"
          string value: 3
          string weight: 1
        integer 12: 
          string index: 8
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.50004
    integer 2: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 30
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 3
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 2
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 3
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.3343
    integer 3: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 42
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 6
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 2
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 7
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 3
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 7
          string weight: 1
      string weight: 0.33429
    integer 4: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 96
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 15
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 5
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 3
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 2
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 2
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 2
          string weight: 1
        integer 10: 
          string index: 6
          string name: "word"
          string value: 3
          string weight: 1
      string weight: 0.33424
    integer 5: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 240
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 6
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 5
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 2
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 2
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 3
          string weight: 1
        integer 10: 
          string index: 6
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.33409
    integer 6: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 288
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 18
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 6
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 3
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 2
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 2
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 2
          string weight: 1
        integer 10: 
          string index: 6
          string name: "word"
          string value: 3
          string weight: 1
        integer 11: 
          string index: 7
          string name: "word"
          string value: 3
          string weight: 1
      string weight: 0.28643
    integer 7: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 60
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 6
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 3
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 3
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.25094
    integer 8: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 90
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 9
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 3
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 3
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 3
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.25091
    integer 9: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 132
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 6
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 3
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 11
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 3
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 11
          string weight: 1
      string weight: 0.25087
    integer 10: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 150
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 6
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 3
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 3
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 5
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.25085
    integer 11: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 210
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 3
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 3
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 7
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 3
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 5
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 7
          string weight: 1
      string weight: 0.25079
    integer 12: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 294
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 15
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 3
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 7
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 3
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 7
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 7
          string weight: 1
      string weight: 0.25071
    integer 13: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 330
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 6
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 3
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 11
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 3
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 5
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 11
          string weight: 1
      string weight: 0.25067
    integer 14: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 966
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 21
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 3
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 23
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 3
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 7
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 23
          string weight: 1
      string weight: 0.25003
    integer 15: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 120
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 3
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 4
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 2
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 3
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.20088
    integer 16: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 168
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 15
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 4
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 7
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 2
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 3
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 7
          string weight: 1
      string weight: 0.20083
    integer 17: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 180
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 9
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 4
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 3
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 3
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.20082
    integer 18: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 300
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 3
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 4
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 5
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 3
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 5
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 5
          string weight: 1
      string weight: 0.2007
    integer 19: 
      string summary: 
        integer 0: 
          string index: -1
          string name: "docid"
          string value: 420
          string weight: 1
        integer 1: 
          string index: -1
          string name: "cross"
          string value: 6
          string weight: 1
        integer 2: 
          string index: -1
          string name: "factors"
          string value: 4
          string weight: 1
        integer 3: 
          string index: -1
          string name: "lo"
          string value: 2
          string weight: 1
        integer 4: 
          string index: -1
          string name: "hi"
          string value: 7
          string weight: 1
        integer 5: 
          string index: 1
          string name: "word"
          string value: 2
          string weight: 1
        integer 6: 
          string index: 2
          string name: "word"
          string value: 2
          string weight: 1
        integer 7: 
          string index: 3
          string name: "word"
          string value: 3
          string weight: 1
        integer 8: 
          string index: 4
          string name: "word"
          string value: 5
          string weight: 1
        integer 9: 
          string index: 5
          string name: "word"
          string value: 7
          string weight: 1
      string weight: 0.20058
string QueryString: "2 3"
string QueryTerms: 
  integer 0: 
    string type: "word"
    string value: 2
  integer 1: 
    string type: "word"
    string value: 3
string ResultList: 
  integer 0: "rank 0: 0.50062"
  integer 1: "    docid: '384'"
  integer 2: "    cross: '15'"
  integer 3: "    factors: '7'"
  integer 4: "    lo: '2'"
  integer 5: "    hi: '3'"
  integer 6: "    word: '2'"
  integer 7: "    word: '2'"
  integer 8: "    word: '2'"
  integer 9: "    word: '2'"
  integer 10: "    word: '2'"
  integer 11: "    word: '2'"
  integer 12: "    word: '2'"
  integer 13: "    word: '3'"
  integer 14: "rank 1: 0.50004"
  integer 15: "    docid: '960'"
  integer 16: "    cross: '15'"
  integer 17: "    factors: '7'"
  integer 18: "    lo: '2'"
  integer 19: "    hi: '5'"
  integer 20: "    word: '2'"
  integer 21: "    word: '2'"
  integer 22: "    word: '2'"
  integer 23: "    word: '2'"
  integer 24: "    word: '2'"
  integer 25: "    word: '2'"
  integer 26: "    word: '3'"
  integer 27: "    word: '5'"
  integer 28: "rank 2: 0.33430"
  integer 29: "    docid: '30'"
  integer 30: "    cross: '3'"
  integer 31: "    factors: '2'"
  integer 32: "    lo: '2'"
  integer 33: "    hi: '5'"
  integer 34: "    word: '2'"
  integer 35: "    word: '3'"
  integer 36: "    word: '5'"
  integer 37: "rank 3: 0.33429"
  integer 38: "    docid: '42'"
  integer 39: "    cross: '6'"
  integer 40: "    factors: '2'"
  integer 41: "    lo: '2'"
  integer 42: "    hi: '7'"
  integer 43: "    word: '2'"
  integer 44: "    word: '3'"
  integer 45: "    word: '7'"
  integer 46: "rank 4: 0.33424"
  integer 47: "    docid: '96'"
  integer 48: "    cross: '15'"
  integer 49: "    factors: '5'"
  integer 50: "    lo: '2'"
  integer 51: "    hi: '3'"
  integer 52: "    word: '2'"
  integer 53: "    word: '2'"
  integer 54: "    word: '2'"
  integer 55: "    word: '2'"
  integer 56: "    word: '2'"
  integer 57: "    word: '3'"
  integer 58: "rank 5: 0.33409"
  integer 59: "    docid: '240'"
  integer 60: "    cross: '6'"
  integer 61: "    factors: '5'"
  integer 62: "    lo: '2'"
  integer 63: "    hi: '5'"
  integer 64: "    word: '2'"
  integer 65: "    word: '2'"
  integer 66: "    word: '2'"
  integer 67: "    word: '2'"
  integer 68: "    word: '3'"
  integer 69: "    word: '5'"
  integer 70: "rank 6: 0.28643"
  integer 71: "    docid: '288'"
  integer 72: "    cross: '18'"
  integer 73: "    factors: '6'"
  integer 74: "    lo: '2'"
  integer 75: "    hi: '3'"
  integer 76: "    word: '2'"
  integer 77: "    word: '2'"
  integer 78: "    word: '2'"
  integer 79: "    word: '2'"
  integer 80: "    word: '2'"
  integer 81: "    word: '3'"
  integer 82: "    word: '3'"
  integer 83: "rank 7: 0.25094"
  integer 84: "    docid: '60'"
  integer 85: "    cross: '6'"
  integer 86: "    factors: '3'"
  integer 87: "    lo: '2'"
  integer 88: "    hi: '5'"
  integer 89: "    word: '2'"
  integer 90: "    word: '2'"
  integer 91: "    word: '3'"
  integer 92: "    word: '5'"
  integer 93: "rank 8: 0.25091"
  integer 94: "    docid: '90'"
  integer 95: "    cross: '9'"
  integer 96: "    factors: '3'"
  integer 97: "    lo: '2'"
  integer 98: "    hi: '5'"
  integer 99: "    word: '2'"
  integer 100: "    word: '3'"
  integer 101: "    word: '3'"
  integer 102: "    word: '5'"
  integer 103: "rank 9: 0.25087"
  integer 104: "    docid: '132'"
  integer 105: "    cross: '6'"
  integer 106: "    factors: '3'"
  integer 107: "    lo: '2'"
  integer 108: "    hi: '11'"
  integer 109: "    word: '2'"
  integer 110: "    word: '2'"
  integer 111: "    word: '3'"
  integer 112: "    word: '11'"
  integer 113: "rank 10: 0.25085"
  integer 114: "    docid: '150'"
  integer 115: "    cross: '6'"
  integer 116: "    factors: '3'"
  integer 117: "    lo: '2'"
  integer 118: "    hi: '5'"
  integer 119: "    word: '2'"
  integer 120: "    word: '3'"
  integer 121: "    word: '5'"
  integer 122: "    word: '5'"
  integer 123: "rank 11: 0.25079"
  integer 124: "    docid: '210'"
  integer 125: "    cross: '3'"
  integer 126: "    factors: '3'"
  integer 127: "    lo: '2'"
  integer 128: "    hi: '7'"
  integer 129: "    word: '2'"
  integer 130: "    word: '3'"
  integer 131: "    word: '5'"
  integer 132: "    word: '7'"
  integer 133: "rank 12: 0.25071"
  integer 134: "    docid: '294'"
  integer 135: "    cross: '15'"
  integer 136: "    factors: '3'"
  integer 137: "    lo: '2'"
  integer 138: "    hi: '7'"
  integer 139: "    word: '2'"
  integer 140: "    word: '3'"
  integer 141: "    word: '7'"
  integer 142: "    word: '7'"
  integer 143: "rank 13: 0.25067"
  integer 144: "    docid: '330'"
  integer 145: "    cross: '6'"
  integer 146: "    factors: '3'"
  integer 147: "    lo: '2'"
  integer 148: "    hi: '11'"
  integer 149: "    word: '2'"
  integer 150: "    word: '3'"
  integer 151: "    word: '5'"
  integer 152: "    word: '11'"
  integer 153: "rank 14: 0.25003"
  integer 154: "    docid: '966'"
  integer 155: "    cross: '21'"
  integer 156: "    factors: '3'"
  integer 157: "    lo: '2'"
  integer 158: "    hi: '23'"
  integer 159: "    word: '2'"
  integer 160: "    word: '3'"
  integer 161: "    word: '7'"
  integer 162: "    word: '23'"
  integer 163: "rank 15: 0.20088"
  integer 164: "    docid: '120'"
  integer 165: "    cross: '3'"
  integer 166: "    factors: '4'"
  integer 167: "    lo: '2'"
  integer 168: "    hi: '5'"
  integer 169: "    word: '2'"
  integer 170: "    word: '2'"
  integer 171: "    word: '2'"
  integer 172: "    word: '3'"
  integer 173: "    word: '5'"
  integer 174: "rank 16: 0.20083"
  integer 175: "    docid: '168'"
  integer 176: "    cross: '15'"
  integer 177: "    factors: '4'"
  integer 178: "    lo: '2'"
  integer 179: "    hi: '7'"
  integer 180: "    word: '2'"
  integer 181: "    word: '2'"
  integer 182: "    word: '2'"
  integer 183: "    word: '3'"
  integer 184: "    word: '7'"
  integer 185: "rank 17: 0.20082"
  integer 186: "    docid: '180'"
  integer 187: "    cross: '9'"
  integer 188: "    factors: '4'"
  integer 189: "    lo: '2'"
  integer 190: "    hi: '5'"
  integer 191: "    word: '2'"
  integer 192: "    word: '2'"
  integer 193: "    word: '3'"
  integer 194: "    word: '3'"
  integer 195: "    word: '5'"
  integer 196: "rank 18: 0.20070"
  integer 197: "    docid: '300'"
  integer 198: "    cross: '3'"
  integer 199: "    factors: '4'"
  integer 200: "    lo: '2'"
  integer 201: "    hi: '5'"
  integer 202: "    word: '2'"
  integer 203: "    word: '2'"
  integer 204: "    word: '3'"
  integer 205: "    word: '5'"
  integer 206: "    word: '5'"
  integer 207: "rank 19: 0.20058"
  integer 208: "    docid: '420'"
  integer 209: "    cross: '6'"
  integer 210: "    factors: '4'"
  integer 211: "    lo: '2'"
  integer 212: "    hi: '7'"
  integer 213: "    word: '2'"
  integer 214: "    word: '2'"
  integer 215: "    word: '3'"
  integer 216: "    word: '5'"
  integer 217: "    word: '7'"
END_expected;

verifyTestOutput( $outputdir, $result, $expected);
?>
