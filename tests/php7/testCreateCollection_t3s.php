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
$storage = $outputdir . "/storage";
$docfiles = ["A.xml","B.xml","C.xml"];
$withrpc = ($ctxconfig != NULL and isset($ctxconfig['rpc']));

$ctx = new StrusContext( $ctxconfig);
$aclmap = ["A.xml" => "a", "B.xml" => "b", "C.xml" => "c"];

createCollection( $ctx, $storage, metadata_t3s(), createDocumentAnalyzer_t3s( $ctx), False, $datadir, $docfiles, $aclmap, $withrpc);
$result = "collection dump:" . dumpTree( dumpCollection( $ctx, $storage));
$expected = <<<END_expected
collection dump:
string config: 
  string acl: "true"
  string cache: "524288K"
  string metadata: 
    string doclen: "UInt16"
    string title_end: "UInt8"
    string title_start: "UInt8"
  string path: "storage"
string docids: 
  integer 0: "A.xml"
  integer 1: "B.xml"
  integer 2: "C.xml"
string docs: 
  string A.xml: 
    string ACL: "a"
    string docclass: "mimetype=application/xml, encoding=utf-8, scheme="
    string docid: "A.xml"
    string doclen: 20
    string endtitle: ""
    string orig: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen."
    string title: "One day in Tokyo"
    string title_end: 6
    string title_start: 0
    string word: "one day in tokyo is a citi that complet differ than what you would expect as european citizen"
  string B.xml: 
    string ACL: "b"
    string docclass: "mimetype=application/xml, encoding=utf-8, scheme="
    string docid: "B.xml"
    string doclen: 16
    string endtitle: ""
    string orig: "A visit in New York New York is a city with dimensions you can't imagine."
    string title: "A visit in New York"
    string title_end: 7
    string title_start: 0
    string word: "in is a citi you visit new york with dimens can t imagin"
  string C.xml: 
    string ACL: "c"
    string docclass: "mimetype=application/xml, encoding=utf-8, scheme="
    string docid: "C.xml"
    string doclen: 16
    string endtitle: ""
    string orig: "A journey through Germany When I first visited germany it was still splitted into two parts."
    string title: "A journey through Germany"
    string title_end: 6
    string title_start: 0
    string word: "a visit journey through germani when i first it was still split into two part"
string nofdocs: 3
string stat: 
  string dfchange: 
    integer 0: 
      string increment: 3
      string type: "endtitle"
      string value: ""
    integer 1: 
      string increment: 3
      string type: "word"
      string value: "a"
    integer 2: 
      string increment: 1
      string type: "word"
      string value: "as"
    integer 3: 
      string increment: 1
      string type: "word"
      string value: "can"
    integer 4: 
      string increment: 2
      string type: "word"
      string value: "citi"
    integer 5: 
      string increment: 1
      string type: "word"
      string value: "citizen"
    integer 6: 
      string increment: 1
      string type: "word"
      string value: "complet"
    integer 7: 
      string increment: 1
      string type: "word"
      string value: "day"
    integer 8: 
      string increment: 1
      string type: "word"
      string value: "differ"
    integer 9: 
      string increment: 1
      string type: "word"
      string value: "dimens"
    integer 10: 
      string increment: 1
      string type: "word"
      string value: "european"
    integer 11: 
      string increment: 1
      string type: "word"
      string value: "expect"
    integer 12: 
      string increment: 1
      string type: "word"
      string value: "first"
    integer 13: 
      string increment: 1
      string type: "word"
      string value: "germani"
    integer 14: 
      string increment: 1
      string type: "word"
      string value: "i"
    integer 15: 
      string increment: 1
      string type: "word"
      string value: "imagin"
    integer 16: 
      string increment: 2
      string type: "word"
      string value: "in"
    integer 17: 
      string increment: 1
      string type: "word"
      string value: "into"
    integer 18: 
      string increment: 2
      string type: "word"
      string value: "is"
    integer 19: 
      string increment: 1
      string type: "word"
      string value: "it"
    integer 20: 
      string increment: 1
      string type: "word"
      string value: "journey"
    integer 21: 
      string increment: 1
      string type: "word"
      string value: "new"
    integer 22: 
      string increment: 1
      string type: "word"
      string value: "one"
    integer 23: 
      string increment: 1
      string type: "word"
      string value: "part"
    integer 24: 
      string increment: 1
      string type: "word"
      string value: "split"
    integer 25: 
      string increment: 1
      string type: "word"
      string value: "still"
    integer 26: 
      string increment: 1
      string type: "word"
      string value: "t"
    integer 27: 
      string increment: 1
      string type: "word"
      string value: "than"
    integer 28: 
      string increment: 1
      string type: "word"
      string value: "that"
    integer 29: 
      string increment: 1
      string type: "word"
      string value: "through"
    integer 30: 
      string increment: 1
      string type: "word"
      string value: "tokyo"
    integer 31: 
      string increment: 1
      string type: "word"
      string value: "two"
    integer 32: 
      string increment: 2
      string type: "word"
      string value: "visit"
    integer 33: 
      string increment: 1
      string type: "word"
      string value: "was"
    integer 34: 
      string increment: 1
      string type: "word"
      string value: "what"
    integer 35: 
      string increment: 1
      string type: "word"
      string value: "when"
    integer 36: 
      string increment: 1
      string type: "word"
      string value: "with"
    integer 37: 
      string increment: 1
      string type: "word"
      string value: "would"
    integer 38: 
      string increment: 1
      string type: "word"
      string value: "york"
    integer 39: 
      string increment: 2
      string type: "word"
      string value: "you"
  string nofdocs: 3
  string statblobs: 
    integer 0: 
      string blob: "AAAAAwALd29yZABwYXJ0AAEFBXR3bwABBQdzcGxpdAABBgZ0aWxsAAEFBXdhcwABBQdmaXJzdAABBQNpAAEFBndoZW4AAQUJZ2VybWFuaQABBQlqb3VybmV5AAEFBWNhbgABBQhkaW1lbnMAAQUJdGhyb3VnaAABBQZ3aXRoAAEFA3QAAQUIaW1hZ2luAAEFBW5ldwABBQd2aXNpdAACBQhleHBlY3QAAQUJY2l0aXplbgABBQZpbnRvAAEFBndoYXQAAQUEaXQAAQUHd291bGQAAQUGdGhhbgABBQpldXJvcGVhbgABBQhkaWZmZXIAAQUJY29tcGxldAABBQZ0aGF0AAEFBmNpdGkAAgUDYQADBQRpcwACBgNuAAIFBnlvcmsAAQcDdQACBQd0b2t5bwABBQVkYXkAAQUEYXMAAQUFb25lAAEAC2VuZHRpdGxlAAAD"
      string timestamp: 
        string counter: 0
        string unixtime: 1
string terms: 
  string A.xml:endtitle (f): 
  string A.xml:endtitle (s): 
    integer 0: 
      string firstpos: 5
      string tf: 1
      string value: ""
  string A.xml:orig (f): 
    integer 0: 
      string pos: 1
      string value: "One"
    integer 1: 
      string pos: 2
      string value: "day"
    integer 2: 
      string pos: 3
      string value: "in"
    integer 3: 
      string pos: 4
      string value: "Tokyo"
    integer 4: 
      string pos: 6
      string value: "Tokyo"
    integer 5: 
      string pos: 7
      string value: "is"
    integer 6: 
      string pos: 8
      string value: "a"
    integer 7: 
      string pos: 9
      string value: "city"
    integer 8: 
      string pos: 10
      string value: "that"
    integer 9: 
      string pos: 11
      string value: "is"
    integer 10: 
      string pos: 12
      string value: "completely"
    integer 11: 
      string pos: 13
      string value: "different"
    integer 12: 
      string pos: 14
      string value: "than"
    integer 13: 
      string pos: 15
      string value: "what"
    integer 14: 
      string pos: 16
      string value: "you"
    integer 15: 
      string pos: 17
      string value: "would"
    integer 16: 
      string pos: 18
      string value: "expect"
    integer 17: 
      string pos: 19
      string value: "as"
    integer 18: 
      string pos: 20
      string value: "European"
    integer 19: 
      string pos: 21
      string value: "citizen."
  string A.xml:orig (s): 
  string A.xml:word (f): 
  string A.xml:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 1
      string value: "one"
    integer 1: 
      string firstpos: 2
      string tf: 1
      string value: "day"
    integer 2: 
      string firstpos: 3
      string tf: 1
      string value: "in"
    integer 3: 
      string firstpos: 4
      string tf: 2
      string value: "tokyo"
    integer 4: 
      string firstpos: 7
      string tf: 2
      string value: "is"
    integer 5: 
      string firstpos: 8
      string tf: 1
      string value: "a"
    integer 6: 
      string firstpos: 9
      string tf: 1
      string value: "citi"
    integer 7: 
      string firstpos: 10
      string tf: 1
      string value: "that"
    integer 8: 
      string firstpos: 12
      string tf: 1
      string value: "complet"
    integer 9: 
      string firstpos: 13
      string tf: 1
      string value: "differ"
    integer 10: 
      string firstpos: 14
      string tf: 1
      string value: "than"
    integer 11: 
      string firstpos: 15
      string tf: 1
      string value: "what"
    integer 12: 
      string firstpos: 16
      string tf: 1
      string value: "you"
    integer 13: 
      string firstpos: 17
      string tf: 1
      string value: "would"
    integer 14: 
      string firstpos: 18
      string tf: 1
      string value: "expect"
    integer 15: 
      string firstpos: 19
      string tf: 1
      string value: "as"
    integer 16: 
      string firstpos: 20
      string tf: 1
      string value: "european"
    integer 17: 
      string firstpos: 21
      string tf: 1
      string value: "citizen"
  string B.xml:endtitle (f): 
  string B.xml:endtitle (s): 
    integer 0: 
      string firstpos: 6
      string tf: 1
      string value: ""
  string B.xml:orig (f): 
    integer 0: 
      string pos: 1
      string value: "A"
    integer 1: 
      string pos: 2
      string value: "visit"
    integer 2: 
      string pos: 3
      string value: "in"
    integer 3: 
      string pos: 4
      string value: "New"
    integer 4: 
      string pos: 5
      string value: "York"
    integer 5: 
      string pos: 7
      string value: "New"
    integer 6: 
      string pos: 8
      string value: "York"
    integer 7: 
      string pos: 9
      string value: "is"
    integer 8: 
      string pos: 10
      string value: "a"
    integer 9: 
      string pos: 11
      string value: "city"
    integer 10: 
      string pos: 12
      string value: "with"
    integer 11: 
      string pos: 13
      string value: "dimensions"
    integer 12: 
      string pos: 14
      string value: "you"
    integer 13: 
      string pos: 15
      string value: "can't"
    integer 14: 
      string pos: 17
      string value: "imagine."
  string B.xml:orig (s): 
  string B.xml:word (f): 
  string B.xml:word (s): 
    integer 0: 
      string firstpos: 3
      string tf: 1
      string value: "in"
    integer 1: 
      string firstpos: 9
      string tf: 1
      string value: "is"
    integer 2: 
      string firstpos: 1
      string tf: 2
      string value: "a"
    integer 3: 
      string firstpos: 11
      string tf: 1
      string value: "citi"
    integer 4: 
      string firstpos: 14
      string tf: 1
      string value: "you"
    integer 5: 
      string firstpos: 2
      string tf: 1
      string value: "visit"
    integer 6: 
      string firstpos: 4
      string tf: 2
      string value: "new"
    integer 7: 
      string firstpos: 5
      string tf: 2
      string value: "york"
    integer 8: 
      string firstpos: 12
      string tf: 1
      string value: "with"
    integer 9: 
      string firstpos: 13
      string tf: 1
      string value: "dimens"
    integer 10: 
      string firstpos: 15
      string tf: 1
      string value: "can"
    integer 11: 
      string firstpos: 16
      string tf: 1
      string value: "t"
    integer 12: 
      string firstpos: 17
      string tf: 1
      string value: "imagin"
  string C.xml:endtitle (f): 
  string C.xml:endtitle (s): 
    integer 0: 
      string firstpos: 5
      string tf: 1
      string value: ""
  string C.xml:orig (f): 
    integer 0: 
      string pos: 1
      string value: "A"
    integer 1: 
      string pos: 2
      string value: "journey"
    integer 2: 
      string pos: 3
      string value: "through"
    integer 3: 
      string pos: 4
      string value: "Germany"
    integer 4: 
      string pos: 6
      string value: "When"
    integer 5: 
      string pos: 7
      string value: "I"
    integer 6: 
      string pos: 8
      string value: "first"
    integer 7: 
      string pos: 9
      string value: "visited"
    integer 8: 
      string pos: 10
      string value: "germany"
    integer 9: 
      string pos: 11
      string value: "it"
    integer 10: 
      string pos: 12
      string value: "was"
    integer 11: 
      string pos: 13
      string value: "still"
    integer 12: 
      string pos: 14
      string value: "splitted"
    integer 13: 
      string pos: 15
      string value: "into"
    integer 14: 
      string pos: 16
      string value: "two"
    integer 15: 
      string pos: 17
      string value: "parts."
  string C.xml:orig (s): 
  string C.xml:word (f): 
  string C.xml:word (s): 
    integer 0: 
      string firstpos: 1
      string tf: 1
      string value: "a"
    integer 1: 
      string firstpos: 9
      string tf: 1
      string value: "visit"
    integer 2: 
      string firstpos: 2
      string tf: 1
      string value: "journey"
    integer 3: 
      string firstpos: 3
      string tf: 1
      string value: "through"
    integer 4: 
      string firstpos: 4
      string tf: 2
      string value: "germani"
    integer 5: 
      string firstpos: 6
      string tf: 1
      string value: "when"
    integer 6: 
      string firstpos: 7
      string tf: 1
      string value: "i"
    integer 7: 
      string firstpos: 8
      string tf: 1
      string value: "first"
    integer 8: 
      string firstpos: 11
      string tf: 1
      string value: "it"
    integer 9: 
      string firstpos: 12
      string tf: 1
      string value: "was"
    integer 10: 
      string firstpos: 13
      string tf: 1
      string value: "still"
    integer 11: 
      string firstpos: 14
      string tf: 1
      string value: "split"
    integer 12: 
      string firstpos: 15
      string tf: 1
      string value: "into"
    integer 13: 
      string firstpos: 16
      string tf: 1
      string value: "two"
    integer 14: 
      string firstpos: 17
      string tf: 1
      string value: "part"
string types: 
  integer 0: "endtitle"
  integer 1: "orig"
  integer 2: "word"
END_expected;

verifyTestOutput( $outputdir, $result, $expected);
?>

