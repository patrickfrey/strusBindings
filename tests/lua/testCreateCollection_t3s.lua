require "string"
require "utils"
require "config_t3s"
require "createCollection"
require "dumpCollection"

local datadir = arg[1]
local outputdir = arg[2] or '.'
local storage = outputdir .. "/storage"
local docfiles = {"A.xml","B.xml","C.xml"}

ctx = strus_Context.new()
ctx:loadModule( "analyzer_pattern")

createCollection( ctx, storage, metadata_t3s(), createDocumentAnalyzer_t3s( ctx), false, datadir, docfiles)
local result = "collection dump:" .. dumpTree( "", dumpCollection( ctx, storage)) .. "\n"
local expected = [[
collection dump:
string config:
  string cache: "524288K"
  string metadata: "doclen UInt16,title_start UInt8,title_end UInt8"
  string path: "storage"
string docids:
  number 1:
    string docno: 3
    string id: "A.xml"
  number 2:
    string docno: 2
    string id: "B.xml"
  number 3:
    string docno: 1
    string id: "C.xml"
string docs:
  string A.xml:
    string docid: "A.xml"
    string doclen: 20
    string docno: 3
    string endtitle: ""
    string orig: "One	day	in	Tokyo	Tokyo	is	a	city	that	is	completely	different	than	what	you	would	expect	as	European	citizen."
    string title: "One day in Tokyo"
    string title_end: 6
    string title_start: 0
    string word: "one	day	in	tokyo	is	a	citi	that	complet	differ	than	what	you	would	expect	as	european	citizen"
  string B.xml:
    string docid: "B.xml"
    string doclen: 16
    string docno: 2
    string endtitle: ""
    string orig: "A	visit	in	New	York	New	York	is	a	city	with	dimensions	you	can't	imagine."
    string title: "A visit in New York"
    string title_end: 7
    string title_start: 0
    string word: "in	is	a	citi	you	visit	new	york	with	dimens	can	t	imagin"
  string C.xml:
    string docid: "C.xml"
    string doclen: 16
    string docno: 1
    string endtitle: ""
    string orig: "A	journey	through	Germany	When	I	first	visited	germany	it	was	still	splitted	into	two	parts."
    string title: "A journey through Germany"
    string title_end: 6
    string title_start: 0
    string word: "a	visit	journey	through	germani	when	i	first	it	was	still	split	into	two	part"
string stat:
  string dfchange:
    number 1:
      string increment: 1
      string type: "word"
      string value: "part"
    number 2:
      string increment: 1
      string type: "word"
      string value: "two"
    number 3:
      string increment: 1
      string type: "word"
      string value: "split"
    number 4:
      string increment: 1
      string type: "word"
      string value: "still"
    number 5:
      string increment: 1
      string type: "word"
      string value: "was"
    number 6:
      string increment: 1
      string type: "word"
      string value: "first"
    number 7:
      string increment: 1
      string type: "word"
      string value: "i"
    number 8:
      string increment: 1
      string type: "word"
      string value: "when"
    number 9:
      string increment: 1
      string type: "word"
      string value: "germani"
    number 10:
      string increment: 1
      string type: "word"
      string value: "journey"
    number 11:
      string increment: 1
      string type: "word"
      string value: "can"
    number 12:
      string increment: 1
      string type: "word"
      string value: "dimens"
    number 13:
      string increment: 1
      string type: "word"
      string value: "through"
    number 14:
      string increment: 1
      string type: "word"
      string value: "with"
    number 15:
      string increment: 1
      string type: "word"
      string value: "t"
    number 16:
      string increment: 1
      string type: "word"
      string value: "imagin"
    number 17:
      string increment: 1
      string type: "word"
      string value: "new"
    number 18:
      string increment: 2
      string type: "word"
      string value: "visit"
    number 19:
      string increment: 1
      string type: "word"
      string value: "expect"
    number 20:
      string increment: 1
      string type: "word"
      string value: "citizen"
    number 21:
      string increment: 1
      string type: "word"
      string value: "into"
    number 22:
      string increment: 1
      string type: "word"
      string value: "what"
    number 23:
      string increment: 1
      string type: "word"
      string value: "it"
    number 24:
      string increment: 1
      string type: "word"
      string value: "would"
    number 25:
      string increment: 1
      string type: "word"
      string value: "than"
    number 26:
      string increment: 1
      string type: "word"
      string value: "european"
    number 27:
      string increment: 1
      string type: "word"
      string value: "differ"
    number 28:
      string increment: 1
      string type: "word"
      string value: "complet"
    number 29:
      string increment: 1
      string type: "word"
      string value: "that"
    number 30:
      string increment: 2
      string type: "word"
      string value: "citi"
    number 31:
      string increment: 3
      string type: "word"
      string value: "a"
    number 32:
      string increment: 2
      string type: "word"
      string value: "is"
    number 33:
      string increment: 2
      string type: "word"
      string value: "in"
    number 34:
      string increment: 1
      string type: "word"
      string value: "york"
    number 35:
      string increment: 2
      string type: "word"
      string value: "you"
    number 36:
      string increment: 1
      string type: "word"
      string value: "tokyo"
    number 37:
      string increment: 1
      string type: "word"
      string value: "day"
    number 38:
      string increment: 1
      string type: "word"
      string value: "as"
    number 39:
      string increment: 1
      string type: "word"
      string value: "one"
    number 40:
      string increment: 3
      string type: "endtitle"
      string value: ""
  string nofdocs: 3
string types:
  number 1: "endtitle"
  number 2: "orig"
  number 3: "word"
]]

verifyTestOutput( outputdir, result, expected)


