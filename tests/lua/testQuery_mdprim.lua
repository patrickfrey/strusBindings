require "string"
require "utils"
require "config_mdprim"
require "createCollection"
require "dumpCollection"

local datadir = arg[1]
local outputdir = arg[2] or '.'
local storagedir = outputdir .. "/storage"
local docfiles = {"doc1000.xml"}
local output = {}

local ctx = strus_Context.new()
ctx:loadModule( "analyzer_pattern")
local aclmap = {}
for i=1,1000 do
	local usr = {}
	if i > 500 then
		table.insert( usr, 'large')
	else
		table.insert( usr, 'small')
	end
	if i < 50 then
		table.insert( usr, 'tiny')
	end
	if i > 950 then
		table.insert( usr, 'huge')
	end
	aclmap[ tostring(i)] = usr
end
createCollection( ctx, storagedir, metadata_mdprim(), createDocumentAnalyzer_mdprim( ctx), true, datadir, docfiles, aclmap)

local queryPhrase = "2 3"
local storageConfig = string.format( "path='%s';metadata='%s';cache=512M", storagedir, metadata_mdprim())

-- Get a client for the new created storage:
local storage = ctx:createStorageClient( storageConfig)

-- Define the query analyzer to use:
local analyzer = createQueryAnalyzer_mdprim( ctx)

-- Define the query evaluation scheme:
local queryEval = createQueryEval_mdprim( ctx)

-- Now we build the query to issue:
local query = queryEval:createQuery( storage)

-- First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
local terms = analyzer:analyzeTermExpression( {"word",queryPhrase})
-- local texpr = analyzer:analyzeTermExpression( {"union", {"sequence", 10, {"sequence", 10, {"word","2"}, {"word","3"}}, {"word","5"}}, {"sequence", 10, {"sequence", 10, {"word","2"}, {"word","5"}}, {"word","7"}} } )
local mexpr = analyzer:analyzeMetaDataExpression( { {{"<","cross","012 "},{">","cross"," 24"}}, {">","factors"," 0 "} } )
local texpr_plain = {
			{"sequence_imm", {"word","2"}, {"word","3"}},
			{"sequence_imm", {"sequence_imm", {"word","2"}, {"word","3"}}, {"word","5"}}
		}
local texpr = analyzer:analyzeTermExpression( texpr_plain)

output[ "QueryString"] = queryPhrase
output[ "QueryTerms"] = terms
output[ "QueryExpr"] = texpr
output[ "QueryRestr"] = mexpr

if #terms == 0 then
	error( "query is empty")
end

-- Then we iterate on the terms and create a single term feature for each term and collect
-- all terms to create a selection expression out of them:
local selexpr = {}

for _,term in ipairs(terms) do
	-- Each query term is also part of the selection expressions
	table.insert( selexpr, term)
	-- Create a document feature 'seek'.
--	query:addFeature( "seek", term, 1.0)
end
for _,texprelem in ipairs(texpr) do
--	query:addFeature( "seek", texprelem)
end
query:addFeature( "seek", texpr_plain)

-- We assign the feature created to the set named 'select' because this is the
-- name of the set defined as selection feature in the query evaluation configuration
-- (QueryEval.addSelectionFeature):
query:addFeature( "select", {"contains", 0, 1, table.unpack(selexpr)})

-- Define the maximum number of best result (ranks) to return:
query:setMaxNofRanks( 20)
-- Define the index of the first rank (for implementing scrolling: 0 for the first, 
-- 20 for the 2nd, 40 for the 3rd page, etc.):
query:setMinRank( 0)

-- Query restriction
query:addMetaDataRestriction( { {{"<","cross",12},{">","cross",24}}, {">","factors",0} } )
query:addMetaDataRestriction( mexpr )

-- Enable debugging
query:setDebugMode( false )

-- Restrict document access
query:addAccess( {"tiny","huge","small"} )

-- Dump query to output
output[ "QueryDump"] = query:tostring()

-- Now we evaluate the query and iterate on the result to display them:
local results = query:evaluate()
output[ "QueryResult"] = results
local output_list = {}
for pos,result in ipairs(results.ranks) do
	table.insert( output_list, string.format( "rank %u: %u %.5f", pos, result.docno, result.weight))
	for sidx,si in pairs(result.summary) do
		table.insert( output_list, string.format( "    %s: '%s'", si.name, si.value))
	end
end
output[ "ResultList"] = output_list

local result = "query evaluation:" .. dumpTree( "", output) .. "\n"
local expected = [[
query evaluation:
string QueryDump: "query evaluation program:
SELECT select;
EVAL  tf( match= %seek);
SUMMARIZE attribute( metaname='docid', resultname='docid');
SUMMARIZE metadata( metaname='cross', resultname='cross');
SUMMARIZE metadata( metaname='factors', resultname='factors');
SUMMARIZE metadata( metaname='lo', resultname='lo');
SUMMARIZE metadata( metaname='hi', resultname='hi');
SUMMARIZE forwardindex( type='word' resultname='word' N=100);
feature 'seek' 1.00000: 
  sequence range=1 cardinality=0:
    term word '2'
    term word '3'

feature 'select' 1.00000: 
  contains range=0 cardinality=1:
    term word '2'
    term word '3'

maxNofRanks = 20
minRank = 0
user access one of : tiny, huge, small
"
string QueryExpr:
  number 1:
    string arg:
      number 1:
        string type: "word"
        string value: "2"
      number 2:
        string type: "word"
        string value: "3"
    string op: "sequence"
    string range: 1
string QueryRestr:
  number 1:
    number 1:
      string name: "cross"
      string op: "<"
      string value: 12
    number 2:
      string name: "cross"
      string op: ">"
      string value: 24
  number 2:
    string name: "factors"
    string op: ">"
    string value: 0
string QueryResult:
  string nofranked: 183
  string nofvisited: 367
  string pass: 0
  string ranks:
    number 1:
      string docno: 996
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "210"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "3"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "3"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "7"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "5"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "7"
          string weight: 1
      string weight: 1
    number 2:
      string docno: 990
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "6"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "6"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "1"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "3"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 1
    number 3:
      string docno: 983
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "30"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "3"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "2"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "5"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "5"
          string weight: 1
      string weight: 1
    number 4:
      string docno: 968
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "12"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "3"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "2"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "3"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 1
    number 5:
      string docno: 928
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "24"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "6"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "3"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "3"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "2"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 1
    number 6:
      string docno: 920
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "252"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "4"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "7"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "3"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "7"
          string weight: 1
      string weight: 1
    number 7:
      string docno: 904
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "18"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "2"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "3"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 1
    number 8:
      string docno: 891
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "324"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "5"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "3"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "3"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "3"
          string weight: 1
        number 11:
          string index: 6
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 1
    number 9:
      string docno: 890
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "54"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "3"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "3"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 1
    number 10:
      string docno: 879
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "36"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "3"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "3"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 1
    number 11:
      string docno: 875
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "300"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "3"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "4"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "5"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "5"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "5"
          string weight: 1
      string weight: 1
    number 12:
      string docno: 839
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "102"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "3"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "2"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "17"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "17"
          string weight: 1
      string weight: 1
    number 13:
      string docno: 828
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "240"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "6"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "5"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "5"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "2"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "2"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "3"
          string weight: 1
        number 11:
          string index: 6
          string name: "word"
          string value: "5"
          string weight: 1
      string weight: 1
    number 14:
      string docno: 818
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "108"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "4"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "3"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "3"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 1
    number 15:
      string docno: 809
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "42"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "6"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "2"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "7"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "7"
          string weight: 1
      string weight: 1
    number 16:
      string docno: 789
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "60"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "6"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "3"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "5"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "5"
          string weight: 1
      string weight: 1
    number 17:
      string docno: 784
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "312"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "6"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "4"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "13"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "2"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "2"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "3"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "13"
          string weight: 1
      string weight: 1
    number 18:
      string docno: 782
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "126"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "3"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "7"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "7"
          string weight: 1
      string weight: 1
    number 19:
      string docno: 776
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "270"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "4"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "5"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "3"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "5"
          string weight: 1
      string weight: 1
    number 20:
      string docno: 774
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "90"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "9"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "3"
          string weight: 1
        number 4:
          string index: -1
          string name: "lo"
          string value: "2"
          string weight: 1
        number 5:
          string index: -1
          string name: "hi"
          string value: "5"
          string weight: 1
        number 6:
          string index: 1
          string name: "word"
          string value: "2"
          string weight: 1
        number 7:
          string index: 2
          string name: "word"
          string value: "3"
          string weight: 1
        number 8:
          string index: 3
          string name: "word"
          string value: "3"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "5"
          string weight: 1
      string weight: 1
string QueryString: "2 3"
string QueryTerms:
  number 1:
    string type: "word"
    string value: "2"
  number 2:
    string type: "word"
    string value: "3"
string ResultList:
  number 1: "rank 1: 996 1.00000"
  number 2: "    docid: '210'"
  number 3: "    cross: '3'"
  number 4: "    factors: '3'"
  number 5: "    lo: '2'"
  number 6: "    hi: '7'"
  number 7: "    word: '2'"
  number 8: "    word: '3'"
  number 9: "    word: '5'"
  number 10: "    word: '7'"
  number 11: "rank 2: 990 1.00000"
  number 12: "    docid: '6'"
  number 13: "    cross: '6'"
  number 14: "    factors: '1'"
  number 15: "    lo: '2'"
  number 16: "    hi: '3'"
  number 17: "    word: '2'"
  number 18: "    word: '3'"
  number 19: "rank 3: 983 1.00000"
  number 20: "    docid: '30'"
  number 21: "    cross: '3'"
  number 22: "    factors: '2'"
  number 23: "    lo: '2'"
  number 24: "    hi: '5'"
  number 25: "    word: '2'"
  number 26: "    word: '3'"
  number 27: "    word: '5'"
  number 28: "rank 4: 968 1.00000"
  number 29: "    docid: '12'"
  number 30: "    cross: '3'"
  number 31: "    factors: '2'"
  number 32: "    lo: '2'"
  number 33: "    hi: '3'"
  number 34: "    word: '2'"
  number 35: "    word: '2'"
  number 36: "    word: '3'"
  number 37: "rank 5: 928 1.00000"
  number 38: "    docid: '24'"
  number 39: "    cross: '6'"
  number 40: "    factors: '3'"
  number 41: "    lo: '2'"
  number 42: "    hi: '3'"
  number 43: "    word: '2'"
  number 44: "    word: '2'"
  number 45: "    word: '2'"
  number 46: "    word: '3'"
  number 47: "rank 6: 920 1.00000"
  number 48: "    docid: '252'"
  number 49: "    cross: '9'"
  number 50: "    factors: '4'"
  number 51: "    lo: '2'"
  number 52: "    hi: '7'"
  number 53: "    word: '2'"
  number 54: "    word: '2'"
  number 55: "    word: '3'"
  number 56: "    word: '3'"
  number 57: "    word: '7'"
  number 58: "rank 7: 904 1.00000"
  number 59: "    docid: '18'"
  number 60: "    cross: '9'"
  number 61: "    factors: '2'"
  number 62: "    lo: '2'"
  number 63: "    hi: '3'"
  number 64: "    word: '2'"
  number 65: "    word: '3'"
  number 66: "    word: '3'"
  number 67: "rank 8: 891 1.00000"
  number 68: "    docid: '324'"
  number 69: "    cross: '9'"
  number 70: "    factors: '5'"
  number 71: "    lo: '2'"
  number 72: "    hi: '3'"
  number 73: "    word: '2'"
  number 74: "    word: '2'"
  number 75: "    word: '3'"
  number 76: "    word: '3'"
  number 77: "    word: '3'"
  number 78: "    word: '3'"
  number 79: "rank 9: 890 1.00000"
  number 80: "    docid: '54'"
  number 81: "    cross: '9'"
  number 82: "    factors: '3'"
  number 83: "    lo: '2'"
  number 84: "    hi: '3'"
  number 85: "    word: '2'"
  number 86: "    word: '3'"
  number 87: "    word: '3'"
  number 88: "    word: '3'"
  number 89: "rank 10: 879 1.00000"
  number 90: "    docid: '36'"
  number 91: "    cross: '9'"
  number 92: "    factors: '3'"
  number 93: "    lo: '2'"
  number 94: "    hi: '3'"
  number 95: "    word: '2'"
  number 96: "    word: '2'"
  number 97: "    word: '3'"
  number 98: "    word: '3'"
  number 99: "rank 11: 875 1.00000"
  number 100: "    docid: '300'"
  number 101: "    cross: '3'"
  number 102: "    factors: '4'"
  number 103: "    lo: '2'"
  number 104: "    hi: '5'"
  number 105: "    word: '2'"
  number 106: "    word: '2'"
  number 107: "    word: '3'"
  number 108: "    word: '5'"
  number 109: "    word: '5'"
  number 110: "rank 12: 839 1.00000"
  number 111: "    docid: '102'"
  number 112: "    cross: '3'"
  number 113: "    factors: '2'"
  number 114: "    lo: '2'"
  number 115: "    hi: '17'"
  number 116: "    word: '2'"
  number 117: "    word: '3'"
  number 118: "    word: '17'"
  number 119: "rank 13: 828 1.00000"
  number 120: "    docid: '240'"
  number 121: "    cross: '6'"
  number 122: "    factors: '5'"
  number 123: "    lo: '2'"
  number 124: "    hi: '5'"
  number 125: "    word: '2'"
  number 126: "    word: '2'"
  number 127: "    word: '2'"
  number 128: "    word: '2'"
  number 129: "    word: '3'"
  number 130: "    word: '5'"
  number 131: "rank 14: 818 1.00000"
  number 132: "    docid: '108'"
  number 133: "    cross: '9'"
  number 134: "    factors: '4'"
  number 135: "    lo: '2'"
  number 136: "    hi: '3'"
  number 137: "    word: '2'"
  number 138: "    word: '2'"
  number 139: "    word: '3'"
  number 140: "    word: '3'"
  number 141: "    word: '3'"
  number 142: "rank 15: 809 1.00000"
  number 143: "    docid: '42'"
  number 144: "    cross: '6'"
  number 145: "    factors: '2'"
  number 146: "    lo: '2'"
  number 147: "    hi: '7'"
  number 148: "    word: '2'"
  number 149: "    word: '3'"
  number 150: "    word: '7'"
  number 151: "rank 16: 789 1.00000"
  number 152: "    docid: '60'"
  number 153: "    cross: '6'"
  number 154: "    factors: '3'"
  number 155: "    lo: '2'"
  number 156: "    hi: '5'"
  number 157: "    word: '2'"
  number 158: "    word: '2'"
  number 159: "    word: '3'"
  number 160: "    word: '5'"
  number 161: "rank 17: 784 1.00000"
  number 162: "    docid: '312'"
  number 163: "    cross: '6'"
  number 164: "    factors: '4'"
  number 165: "    lo: '2'"
  number 166: "    hi: '13'"
  number 167: "    word: '2'"
  number 168: "    word: '2'"
  number 169: "    word: '2'"
  number 170: "    word: '3'"
  number 171: "    word: '13'"
  number 172: "rank 18: 782 1.00000"
  number 173: "    docid: '126'"
  number 174: "    cross: '9'"
  number 175: "    factors: '3'"
  number 176: "    lo: '2'"
  number 177: "    hi: '7'"
  number 178: "    word: '2'"
  number 179: "    word: '3'"
  number 180: "    word: '3'"
  number 181: "    word: '7'"
  number 182: "rank 19: 776 1.00000"
  number 183: "    docid: '270'"
  number 184: "    cross: '9'"
  number 185: "    factors: '4'"
  number 186: "    lo: '2'"
  number 187: "    hi: '5'"
  number 188: "    word: '2'"
  number 189: "    word: '3'"
  number 190: "    word: '3'"
  number 191: "    word: '3'"
  number 192: "    word: '5'"
  number 193: "rank 20: 774 1.00000"
  number 194: "    docid: '90'"
  number 195: "    cross: '9'"
  number 196: "    factors: '3'"
  number 197: "    lo: '2'"
  number 198: "    hi: '5'"
  number 199: "    word: '2'"
  number 200: "    word: '3'"
  number 201: "    word: '3'"
  number 202: "    word: '5'"
]]

verifyTestOutput( outputdir, result, expected)

