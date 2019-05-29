<?php
include_once "utils.php";
include_once "config_mdprim.php";
include_once "createCollection.php";
include_once "dumpCollection.php";

$datadir = "../data/mdprim/";
if (isset($argv[1])) {
	$datadir = $argv[1];
}
$outputdir = '.';
if (isset($argv[2])) {
	$outputdir = $argv[2];
}
$ctxconfig = getContextConfig( $argv[3]);
$storage = $outputdir . "/storage";
$docfiles = ["doc10.xml"];
$withrpc = ($ctxconfig != NULL and isset($ctxconfig['rpc']));

$ctx = new StrusContext( $ctxconfig);
$aclmap = ["1"=>'A',"2"=>'A',"3"=>'A',"4"=>'A',"5"=>'A',"6"=>'B',"7"=>'B',"8"=>'B',"9"=>'B',"10"=>'B'];

createCollection( $ctx, $storage, metadata_mdprim(), createDocumentAnalyzer_mdprim( $ctx), True, $datadir, $docfiles, $aclmap, $withrpc);
$result = "collection dump:" . dumpTree( dumpCollection( $ctx, $storage));
$expected = <<<END_expected
collection dump:
string config: 
  string acl: "true"
  string cache: "524288K"
  string metadata: 
    string cross: "UInt8"
    string docidx: "UInt32"
    string doclen: "UInt16"
    string factors: "UInt8"
    string hi: "UInt16"
    string lo: "UInt16"
  string path: "storage"
string docids: 
  integer 0: 1
  integer 1: 2
  integer 2: 3
  integer 3: 4
  integer 4: 5
  integer 5: 6
  integer 6: 7
  integer 7: 8
  integer 8: 9
  integer 9: 10
string docs: 
  integer 1: 
    string ACL: "A"
    string cross: 1
    string docid: 1
    string docidx: 1
    string doclen: 0
    string factors: 0
    string hi: 0
    string lo: 0
    string word: 
  integer 2: 
    string ACL: "A"
    string cross: 2
    string docid: 2
    string docidx: 2
    string doclen: 1
    string factors: 0
    string hi: 2
    string lo: 2
    string word: 2
  integer 3: 
    string ACL: "A"
    string cross: 3
    string docid: 3
    string docidx: 3
    string doclen: 1
    string factors: 0
    string hi: 3
    string lo: 3
    string word: 3
  integer 4: 
    string ACL: "A"
    string cross: 4
    string docid: 4
    string docidx: 4
    string doclen: 2
    string factors: 1
    string hi: 2
    string lo: 2
    string word: "2 2"
  integer 5: 
    string ACL: "A"
    string cross: 5
    string docid: 5
    string docidx: 5
    string doclen: 1
    string factors: 0
    string hi: 5
    string lo: 5
    string word: 5
  integer 6: 
    string ACL: "B"
    string cross: 6
    string docid: 6
    string docidx: 6
    string doclen: 2
    string factors: 1
    string hi: 3
    string lo: 2
    string word: "2 3"
  integer 7: 
    string ACL: "B"
    string cross: 7
    string docid: 7
    string docidx: 7
    string doclen: 1
    string factors: 0
    string hi: 7
    string lo: 7
    string word: 7
  integer 8: 
    string ACL: "B"
    string cross: 8
    string docid: 8
    string docidx: 8
    string doclen: 3
    string factors: 2
    string hi: 2
    string lo: 2
    string word: "2 2 2"
  integer 9: 
    string ACL: "B"
    string cross: 9
    string docid: 9
    string docidx: 9
    string doclen: 2
    string factors: 1
    string hi: 3
    string lo: 3
    string word: "3 3"
  integer 10: 
    string ACL: "B"
    string cross: 1
    string docid: 10
    string docidx: 10
    string doclen: 2
    string factors: 1
    string hi: 5
    string lo: 2
    string word: "2 5"
string nofdocs: 10
string stat: 
  string dfchange: 
    integer 0: 
      string increment: 5
      string type: "word"
      string value: 2
    integer 1: 
      string increment: 3
      string type: "word"
      string value: 3
    integer 2: 
      string increment: 2
      string type: "word"
      string value: 5
    integer 3: 
      string increment: 1
      string type: "word"
      string value: 7
  string nofdocs: 10
  string statblobs: 
    integer 0: 
      string blob: "AAAACgAId29yZAA1AAIFAzMAAwUDNwABBQMyAAU="
      string timestamp: 
        string counter: 0
        string unixtime: 1
string terms: 
  string 10:word (f): 
    integer 0: 
      string pos: 1
      string value: 2
    integer 1: 
      string pos: 2
      string value: 5
  string 10:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 1
      string value: 2
    integer 1: 
      string firstpos: 2
      string tf: 1
      string value: 5
  string 1:word (f): 
  string 1:word (s): 
  string 2:word (f): 
    integer 0: 
      string pos: 1
      string value: 2
  string 2:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 1
      string value: 2
  string 3:word (f): 
    integer 0: 
      string pos: 1
      string value: 3
  string 3:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 1
      string value: 3
  string 4:word (f): 
    integer 0: 
      string pos: 1
      string value: 2
    integer 1: 
      string pos: 2
      string value: 2
  string 4:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 2
      string value: 2
  string 5:word (f): 
    integer 0: 
      string pos: 1
      string value: 5
  string 5:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 1
      string value: 5
  string 6:word (f): 
    integer 0: 
      string pos: 1
      string value: 2
    integer 1: 
      string pos: 2
      string value: 3
  string 6:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 1
      string value: 2
    integer 1: 
      string firstpos: 2
      string tf: 1
      string value: 3
  string 7:word (f): 
    integer 0: 
      string pos: 1
      string value: 7
  string 7:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 1
      string value: 7
  string 8:word (f): 
    integer 0: 
      string pos: 1
      string value: 2
    integer 1: 
      string pos: 2
      string value: 2
    integer 2: 
      string pos: 3
      string value: 2
  string 8:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 3
      string value: 2
  string 9:word (f): 
    integer 0: 
      string pos: 1
      string value: 3
    integer 1: 
      string pos: 2
      string value: 3
  string 9:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 2
      string value: 3
string types: 
  integer 0: "word"
END_expected;

verifyTestOutput( $outputdir, $result, $expected);
?>


