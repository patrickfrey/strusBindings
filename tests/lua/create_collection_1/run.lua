require "string"
local programpathar = {}
for pp in string.gmatch( arg[0], "[^/]+") do
	table.insert( programpathar, pp)
end
table.remove( programpathar, #programpathar)
local programpath = "/" .. table.concat( programpathar, "/")

package.path = (programpath .. "/../common/utils.lua")
require "utils"
package.path = (programpath .. "/../common/createCollection.lua")
require "createCollection"
package.path = (programpath .. "/../common/dumpCollection.lua")
require "dumpCollection"

local datadir = arg[1]
local outputdir = arg[2] or '.'
local storage = outputdir .. "/storage"
local docfiles = {"A.xml","B.xml","C.xml"}

ctx = strus_Context.new()
ctx:loadModule( "analyzer_pattern")

createCollection( ctx, storage, datadir, docfiles)
local result = "collection dump:" .. dumpTree( "", dumpCollection( ctx, storage, docfiles)) .. "\n"
local expected = [[
collection dump:
string config: 
  string cache: "524288K"
  string metadata: "doclen UInt16,title_start UInt8,title_end UInt8"
  string path: "./storage"
string docids: 
  number 1: "A.xml"
  number 2: "B.xml"
  number 3: "C.xml"
string docs: 
  string A.xml: 
    string docid: "A.xml"
    string doclen: 20
    string docno: 3
    string title: "One day in Tokyo"
    string title_end: 6
    string title_start: 0
  string B.xml: 
    string docid: "B.xml"
    string doclen: 16
    string docno: 2
    string title: "A visit in New York"
    string title_end: 7
    string title_start: 0
  string C.xml: 
    string docid: "C.xml"
    string doclen: 16
    string docno: 1
    string title: "A journey through Germany"
    string title_end: 6
    string title_start: 0
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


