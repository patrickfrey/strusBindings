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
$queryAnalyzed = $analyzer->analyzeTermExpression( ["word",$queryPhrase]);
$exprAnalyzed = $analyzer->analyzeTermExpression( [
                                        "sequence", 10,
                                                ["sequence", 2, ["word","completely"], ["word","different"]],
                                                ["sequence", 3, ["word","you"], ["word","expect"]]
                                ]);

$output[ "QueryString"] = $queryPhrase;
$output[ "QueryAnalyzed"] = $queryAnalyzed;
$output[ "QueryExpression"] = $exprAnalyzed;

if (!$withrpc) {
        // ... For RPC there are no posting iterators implemented
        $postings = [];
        foreach ($storage->postings( $queryAnalyzed) as $po) {
                $docno = $po[0];
                $pos = $po[1];
                $postings[ "query analyzed doc " . $storage->docid($docno) ] = $pos;
        }
        foreach ($storage->postings( $exprAnalyzed) as $po) {
                $docno = $po[0];
                $pos = $po[1];
                $postings[ "expr analyzed doc " . $storage->docid($docno) ] = $pos;
        }
        $output[ "postings"] = $postings;
}

// Then we iterate on the terms and create a single term feature for each term and collect
// all terms to create a selection expression out of them:
$selexpr = ["contains", 0, 1, $queryAnalyzed];

// Create a document feature 'seek'.
$query->addFeature( "seek", $queryAnalyzed, 1.0);

// We assign the feature created to the set named 'select' because this is the
// name of the set defined as selection feature in the query evaluation configuration
// (QueryEval.addSelectionFeature):
$query->addFeature( "select", $selexpr);

// Define the maximum integer of best result (ranks) to return:
$query->setMaxNofRanks( 20);
// Define the index of the first rank (for implementing scrolling: 0 for the first,
// 20 for the 2nd, 40 for the 3rd page, etc.):
$query->setMinRank( 0);

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
string QueryAnalyzed:
  string arg:
    integer 0:
      string type: "word"
      string value: "citi"
    integer 1:
      string type: "word"
      string value: "visit"
    integer 2:
      string type: "word"
      string value: "tokyo"
  string op: "union"
string QueryDump:
  string eval:
    string selection_sets:
      integer 0: "select"
    string summarizers:
      integer 0:
        string function: "attribute"
        string param:
          string attribute: "title"
      integer 1:
        string function: "attribute"
        string param:
          string attribute: "docid"
      integer 2:
        string feature:
          string match: "seek"
        string function: "matchphrase"
        string param:
          string cluster: 0.1
          string dist_close: 8
          string dist_imm: 2
          string dist_near: 40
          string dist_sentence: 20
          string entity: ""
          string ffbase: 0.1
          string maxdf: 1
          string sentences: 2
          string text: "orig"
    string weighting:
      integer 0:
        string feature:
          string match: "seek"
        string function: "bm25"
        string param:
          string avgdoclen: 1000
          string b: 2.1
          string k1: 0.75
          string metadata_doclen: "doclen"
    string weighting_sets:
      integer 0: "seek"
  string feature:
    integer 0:
      string set: "seek"
      string struct:
        string arg:
          integer 0:
            string node: "term"
            string type: "word"
            string value: "citi"
          integer 1:
            string node: "term"
            string type: "word"
            string value: "visit"
          integer 2:
            string node: "term"
            string type: "word"
            string value: "tokyo"
        string node: "expression"
        string op: "union"
    integer 1:
      string set: "select"
      string struct:
        string arg:
          integer 0:
            string arg:
              integer 0:
                string node: "term"
                string type: "word"
                string value: "citi"
              integer 1:
                string node: "term"
                string type: "word"
                string value: "visit"
              integer 2:
                string node: "term"
                string type: "word"
                string value: "tokyo"
            string node: "expression"
            string op: "union"
        string cardinality: 1
        string node: "expression"
        string op: "contains"
  string merge: 0
  string minrank: 0
  string nofranks: 20
string QueryExpression:
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
      string field:
        string end: 0
        string start: 0
      string summary:
        integer 0:
          string name: ""
          string value: "A journey through Germany"
        integer 1:
          string name: ""
          string value: "C.xml"
        integer 2:
          string name: "summary"
          string value: "A journey through Germany When I first visited germany it was still splitted into two parts."
      string weight: 9.0E-5
    integer 1:
      string field:
        string end: 0
        string start: 0
      string summary:
        integer 0:
          string name: ""
          string value: "A visit in New York"
        integer 1:
          string name: ""
          string value: "B.xml"
        integer 2:
          string name: "summary"
          string value: "A visit in New York New York is a city with dimensions you can't imagine."
      string weight: 3.0E-5
    integer 2:
      string field:
        string end: 0
        string start: 0
      string summary:
        integer 0:
          string name: ""
          string value: "One day in Tokyo"
        integer 1:
          string name: ""
          string value: "A.xml"
        integer 2:
          string name: "summary"
          string value: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen."
      string weight: 2.0E-5
string QueryString: "City visit tokyo"
string ResultList:
  integer 0: "rank 0: 0.00009"
  integer 1: "     'A journey through Germany'"
  integer 2: "     'C.xml'"
  integer 3: "    summary 'A journey through Germany When I first visited germany it was still splitted into two parts.'"
  integer 4: "rank 1: 0.00003"
  integer 5: "     'A visit in New York'"
  integer 6: "     'B.xml'"
  integer 7: "    summary 'A visit in New York New York is a city with dimensions you can't imagine.'"
  integer 8: "rank 2: 0.00002"
  integer 9: "     'One day in Tokyo'"
  integer 10: "     'A.xml'"
  integer 11: "    summary 'One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen.'"
string postings:
  string expr analyzed doc A.xml:
    integer 0: 12
  string query analyzed doc A.xml:
    integer 0: 4
    integer 1: 6
    integer 2: 9
  string query analyzed doc B.xml:
    integer 0: 2
    integer 1: 11
  string query analyzed doc C.xml:
    integer 0: 9
END_expected;

verifyTestOutput( $outputdir, $result, $expected);
?>


