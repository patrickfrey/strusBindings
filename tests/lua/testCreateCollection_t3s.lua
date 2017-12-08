require "string"
require "utils"
require "config_t3s"
require "createCollection"
require "dumpCollection"

local datadir = arg[1] or "../data/t3s/"
local outputdir = arg[2] or '.'
local ctxconfig = getContextConfig( arg[3])
local storage = outputdir .. "/storage"
local docfiles = {"A.xml","B.xml","C.xml"}
local withrpc = (ctxconfig and ctxconfig.rpc) and true or false

ctx = strus_Context.new( ctxconfig)
local aclmap = {["A.xml"]="a",["B.xml"]="b",["C.xml"]="c"}

createCollection( ctx, storage, metadata_t3s(), createDocumentAnalyzer_t3s( ctx), false, datadir, docfiles, aclmap, withrpc)
local result = "collection dump:" .. dumpTree( dumpCollection( ctx, storage)) .. "\n"
local expected = [[
collection dump:
string config:
  string acl: "true"
  string cache: "524288K"
  string metadata: "doclen UInt16,title_start UInt8,title_end UInt8"
  string path: "storage"
string docids:
  number 1: "A.xml"
  number 2: "B.xml"
  number 3: "C.xml"
string docs:
  string A.xml:
    string ACL: "a"
    string docclass: "mimetype=application/xml, encoding=UTF-8, scheme="
    string docid: "A.xml"
    string doclen: 20
    string docno: 3
    string endtitle: ""
    string orig: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen."
    string title: "One day in Tokyo"
    string title_end: 6
    string title_start: 0
    string word: "one day in tokyo is a citi that complet differ than what you would expect as european citizen"
  string B.xml:
    string ACL: "b"
    string docclass: "mimetype=application/xml, encoding=UTF-8, scheme="
    string docid: "B.xml"
    string doclen: 16
    string docno: 2
    string endtitle: ""
    string orig: "A visit in New York New York is a city with dimensions you can't imagine."
    string title: "A visit in New York"
    string title_end: 7
    string title_start: 0
    string word: "in is a citi you visit new york with dimens can t imagin"
  string C.xml:
    string ACL: "c"
    string docclass: "mimetype=application/xml, encoding=UTF-8, scheme="
    string docid: "C.xml"
    string doclen: 16
    string docno: 1
    string endtitle: ""
    string orig: "A journey through Germany When I first visited germany it was still splitted into two parts."
    string title: "A journey through Germany"
    string title_end: 6
    string title_start: 0
    string word: "a visit journey through germani when i first it was still split into two part"
string nofdocs: 3
string stat:
  string dfchange:
    number 1:
      string increment: 3
      string type: "endtitle"
      string value: ""
    number 2:
      string increment: 3
      string type: "word"
      string value: "a"
    number 3:
      string increment: 1
      string type: "word"
      string value: "as"
    number 4:
      string increment: 1
      string type: "word"
      string value: "can"
    number 5:
      string increment: 2
      string type: "word"
      string value: "citi"
    number 6:
      string increment: 1
      string type: "word"
      string value: "citizen"
    number 7:
      string increment: 1
      string type: "word"
      string value: "complet"
    number 8:
      string increment: 1
      string type: "word"
      string value: "day"
    number 9:
      string increment: 1
      string type: "word"
      string value: "differ"
    number 10:
      string increment: 1
      string type: "word"
      string value: "dimens"
    number 11:
      string increment: 1
      string type: "word"
      string value: "european"
    number 12:
      string increment: 1
      string type: "word"
      string value: "expect"
    number 13:
      string increment: 1
      string type: "word"
      string value: "first"
    number 14:
      string increment: 1
      string type: "word"
      string value: "germani"
    number 15:
      string increment: 1
      string type: "word"
      string value: "i"
    number 16:
      string increment: 1
      string type: "word"
      string value: "imagin"
    number 17:
      string increment: 2
      string type: "word"
      string value: "in"
    number 18:
      string increment: 1
      string type: "word"
      string value: "into"
    number 19:
      string increment: 2
      string type: "word"
      string value: "is"
    number 20:
      string increment: 1
      string type: "word"
      string value: "it"
    number 21:
      string increment: 1
      string type: "word"
      string value: "journey"
    number 22:
      string increment: 1
      string type: "word"
      string value: "new"
    number 23:
      string increment: 1
      string type: "word"
      string value: "one"
    number 24:
      string increment: 1
      string type: "word"
      string value: "part"
    number 25:
      string increment: 1
      string type: "word"
      string value: "split"
    number 26:
      string increment: 1
      string type: "word"
      string value: "still"
    number 27:
      string increment: 1
      string type: "word"
      string value: "t"
    number 28:
      string increment: 1
      string type: "word"
      string value: "than"
    number 29:
      string increment: 1
      string type: "word"
      string value: "that"
    number 30:
      string increment: 1
      string type: "word"
      string value: "through"
    number 31:
      string increment: 1
      string type: "word"
      string value: "tokyo"
    number 32:
      string increment: 1
      string type: "word"
      string value: "two"
    number 33:
      string increment: 2
      string type: "word"
      string value: "visit"
    number 34:
      string increment: 1
      string type: "word"
      string value: "was"
    number 35:
      string increment: 1
      string type: "word"
      string value: "what"
    number 36:
      string increment: 1
      string type: "word"
      string value: "when"
    number 37:
      string increment: 1
      string type: "word"
      string value: "with"
    number 38:
      string increment: 1
      string type: "word"
      string value: "would"
    number 39:
      string increment: 1
      string type: "word"
      string value: "york"
    number 40:
      string increment: 2
      string type: "word"
      string value: "you"
  string nofdocs: 3
string terms:
  string A.xml:endtitle (f): 
  string A.xml:endtitle (s):
    number 1:
      string firstpos: 5
      string tf: 1
      string value: ""
  string A.xml:orig (f):
    number 1:
      string pos: 1
      string value: "One"
    number 2:
      string pos: 2
      string value: "day"
    number 3:
      string pos: 3
      string value: "in"
    number 4:
      string pos: 4
      string value: "Tokyo"
    number 5:
      string pos: 6
      string value: "Tokyo"
    number 6:
      string pos: 7
      string value: "is"
    number 7:
      string pos: 8
      string value: "a"
    number 8:
      string pos: 9
      string value: "city"
    number 9:
      string pos: 10
      string value: "that"
    number 10:
      string pos: 11
      string value: "is"
    number 11:
      string pos: 12
      string value: "completely"
    number 12:
      string pos: 13
      string value: "different"
    number 13:
      string pos: 14
      string value: "than"
    number 14:
      string pos: 15
      string value: "what"
    number 15:
      string pos: 16
      string value: "you"
    number 16:
      string pos: 17
      string value: "would"
    number 17:
      string pos: 18
      string value: "expect"
    number 18:
      string pos: 19
      string value: "as"
    number 19:
      string pos: 20
      string value: "European"
    number 20:
      string pos: 21
      string value: "citizen."
  string A.xml:orig (s): 
  string A.xml:word (f): 
  string A.xml:word (s):
    number 1:
      string firstpos: 1
      string tf: 1
      string value: "one"
    number 2:
      string firstpos: 2
      string tf: 1
      string value: "day"
    number 3:
      string firstpos: 3
      string tf: 1
      string value: "in"
    number 4:
      string firstpos: 4
      string tf: 2
      string value: "tokyo"
    number 5:
      string firstpos: 7
      string tf: 2
      string value: "is"
    number 6:
      string firstpos: 8
      string tf: 1
      string value: "a"
    number 7:
      string firstpos: 9
      string tf: 1
      string value: "citi"
    number 8:
      string firstpos: 10
      string tf: 1
      string value: "that"
    number 9:
      string firstpos: 12
      string tf: 1
      string value: "complet"
    number 10:
      string firstpos: 13
      string tf: 1
      string value: "differ"
    number 11:
      string firstpos: 14
      string tf: 1
      string value: "than"
    number 12:
      string firstpos: 15
      string tf: 1
      string value: "what"
    number 13:
      string firstpos: 16
      string tf: 1
      string value: "you"
    number 14:
      string firstpos: 17
      string tf: 1
      string value: "would"
    number 15:
      string firstpos: 18
      string tf: 1
      string value: "expect"
    number 16:
      string firstpos: 19
      string tf: 1
      string value: "as"
    number 17:
      string firstpos: 20
      string tf: 1
      string value: "european"
    number 18:
      string firstpos: 21
      string tf: 1
      string value: "citizen"
  string B.xml:endtitle (f): 
  string B.xml:endtitle (s):
    number 1:
      string firstpos: 6
      string tf: 1
      string value: ""
  string B.xml:orig (f):
    number 1:
      string pos: 1
      string value: "A"
    number 2:
      string pos: 2
      string value: "visit"
    number 3:
      string pos: 3
      string value: "in"
    number 4:
      string pos: 4
      string value: "New"
    number 5:
      string pos: 5
      string value: "York"
    number 6:
      string pos: 7
      string value: "New"
    number 7:
      string pos: 8
      string value: "York"
    number 8:
      string pos: 9
      string value: "is"
    number 9:
      string pos: 10
      string value: "a"
    number 10:
      string pos: 11
      string value: "city"
    number 11:
      string pos: 12
      string value: "with"
    number 12:
      string pos: 13
      string value: "dimensions"
    number 13:
      string pos: 14
      string value: "you"
    number 14:
      string pos: 15
      string value: "can't"
    number 15:
      string pos: 17
      string value: "imagine."
  string B.xml:orig (s): 
  string B.xml:word (f): 
  string B.xml:word (s):
    number 1:
      string firstpos: 3
      string tf: 1
      string value: "in"
    number 2:
      string firstpos: 9
      string tf: 1
      string value: "is"
    number 3:
      string firstpos: 1
      string tf: 2
      string value: "a"
    number 4:
      string firstpos: 11
      string tf: 1
      string value: "citi"
    number 5:
      string firstpos: 14
      string tf: 1
      string value: "you"
    number 6:
      string firstpos: 2
      string tf: 1
      string value: "visit"
    number 7:
      string firstpos: 4
      string tf: 2
      string value: "new"
    number 8:
      string firstpos: 5
      string tf: 2
      string value: "york"
    number 9:
      string firstpos: 12
      string tf: 1
      string value: "with"
    number 10:
      string firstpos: 13
      string tf: 1
      string value: "dimens"
    number 11:
      string firstpos: 15
      string tf: 1
      string value: "can"
    number 12:
      string firstpos: 16
      string tf: 1
      string value: "t"
    number 13:
      string firstpos: 17
      string tf: 1
      string value: "imagin"
  string C.xml:endtitle (f): 
  string C.xml:endtitle (s):
    number 1:
      string firstpos: 5
      string tf: 1
      string value: ""
  string C.xml:orig (f):
    number 1:
      string pos: 1
      string value: "A"
    number 2:
      string pos: 2
      string value: "journey"
    number 3:
      string pos: 3
      string value: "through"
    number 4:
      string pos: 4
      string value: "Germany"
    number 5:
      string pos: 6
      string value: "When"
    number 6:
      string pos: 7
      string value: "I"
    number 7:
      string pos: 8
      string value: "first"
    number 8:
      string pos: 9
      string value: "visited"
    number 9:
      string pos: 10
      string value: "germany"
    number 10:
      string pos: 11
      string value: "it"
    number 11:
      string pos: 12
      string value: "was"
    number 12:
      string pos: 13
      string value: "still"
    number 13:
      string pos: 14
      string value: "splitted"
    number 14:
      string pos: 15
      string value: "into"
    number 15:
      string pos: 16
      string value: "two"
    number 16:
      string pos: 17
      string value: "parts."
  string C.xml:orig (s): 
  string C.xml:word (f): 
  string C.xml:word (s):
    number 1:
      string firstpos: 1
      string tf: 1
      string value: "a"
    number 2:
      string firstpos: 9
      string tf: 1
      string value: "visit"
    number 3:
      string firstpos: 2
      string tf: 1
      string value: "journey"
    number 4:
      string firstpos: 3
      string tf: 1
      string value: "through"
    number 5:
      string firstpos: 4
      string tf: 2
      string value: "germani"
    number 6:
      string firstpos: 6
      string tf: 1
      string value: "when"
    number 7:
      string firstpos: 7
      string tf: 1
      string value: "i"
    number 8:
      string firstpos: 8
      string tf: 1
      string value: "first"
    number 9:
      string firstpos: 11
      string tf: 1
      string value: "it"
    number 10:
      string firstpos: 12
      string tf: 1
      string value: "was"
    number 11:
      string firstpos: 13
      string tf: 1
      string value: "still"
    number 12:
      string firstpos: 14
      string tf: 1
      string value: "split"
    number 13:
      string firstpos: 15
      string tf: 1
      string value: "into"
    number 14:
      string firstpos: 16
      string tf: 1
      string value: "two"
    number 15:
      string firstpos: 17
      string tf: 1
      string value: "part"
string types:
  number 1: "endtitle"
  number 2: "orig"
  number 3: "word"
]]

verifyTestOutput( outputdir, result, expected)


