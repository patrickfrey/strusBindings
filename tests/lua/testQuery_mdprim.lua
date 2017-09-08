require "string"
require "utils"
require "config_mdprim"
require "createCollection"
require "dumpCollection"

local datadir = arg[1] or "../data/mdprim/"
local outputdir = arg[2] or '.'
local ctxconfig = getContextConfig( arg[3])
local storagedir = outputdir .. "/storage"
local docfiles = {"doc1000.xml"}
local output = {}
local withrpc = (ctxconfig and ctxconfig.rpc) and true or false

local ctx = strus_Context.new( ctxconfig)
local storageConfig = nil

if not withrpc then
	ctx:loadModule( "analyzer_pattern")
	storageConfig = string.format( "path='%s';metadata='%s';cache=512M", storagedir, metadata_mdprim())
end

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
createCollection( ctx, storagedir, metadata_mdprim(), createDocumentAnalyzer_mdprim( ctx), true, datadir, docfiles, aclmap, withrpc)

local queryPhrase = "2 3"

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
local mexpr = analyzer:analyzeMetaDataExpression( { {{"<","cross","010 "},{">","cross"," 14"}}, {">","factors"," 0 "} } )
local texpr_plain = {
			{"sequence_imm", {"word","2"}, {"word","2"}, {"word","2"}, {"word","2"}},
			{"sequence_imm", {"sequence_imm", {"word","2"}, {"word","3"}}, {"union", {"sequence_imm", {"word","3"}, {"word","5"}}, {"word","5"}, {"word","7"}, {"word","11"}}}
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
end
query:addFeature( "seek", texpr)

-- We assign the feature created to the set named 'select' because this is the
-- name of the set defined as selection feature in the query evaluation configuration
-- (QueryEval.addSelectionFeature):
query:addFeature( "select", {"contains", 0, 2, table.unpack(selexpr)})

-- Define the maximum number of best result (ranks) to return:
query:setMaxNofRanks( 20)
-- Define the index of the first rank (for implementing scrolling: 0 for the first, 
-- 20 for the 2nd, 40 for the 3rd page, etc.):
query:setMinRank( 0)

-- Query restriction
query:addMetaDataRestriction( { {{"<","cross",10},{">","cross",14}}, {">","factors",0} } )
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

local result = "query evaluation:" .. dumpTree( output) .. "\n"
local expected = [[
query evaluation:
string QueryDump: "query evaluation program:
SELECT select;
EVAL  tf( match= %seek);
EVAL  metadata( name=doclen, weight=1);
SUMMARIZE attribute( metaname='docid', resultname='docid');
SUMMARIZE metadata( metaname='cross', resultname='cross');
SUMMARIZE metadata( metaname='factors', resultname='factors');
SUMMARIZE metadata( metaname='lo', resultname='lo');
SUMMARIZE metadata( metaname='hi', resultname='hi');
SUMMARIZE forwardindex( type='word' resultname='word' N=100);
feature 'seek' 1.00000: 
  sequence_imm range=0 cardinality=0:
    sequence_imm range=0 cardinality=0:
      term word '2'
      term word '3'
    union range=0 cardinality=0:
      sequence_imm range=0 cardinality=0:
        term word '3'
        term word '5'
      term word '5'
      term word '7'
      term word '11'

feature 'seek' 1.00000: 
  sequence_imm range=0 cardinality=0:
    term word '2'
    term word '2'
    term word '2'
    term word '2'

feature 'select' 1.00000: 
  contains range=0 cardinality=2:
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
        string value: "2"
      number 3:
        string type: "word"
        string value: "2"
      number 4:
        string type: "word"
        string value: "2"
    string op: "sequence_imm"
  number 2:
    string arg:
      number 1:
        string arg:
          number 1:
            string type: "word"
            string value: "2"
          number 2:
            string type: "word"
            string value: "3"
        string op: "sequence_imm"
      number 2:
        string arg:
          number 1:
            string arg:
              number 1:
                string type: "word"
                string value: "3"
              number 2:
                string type: "word"
                string value: "5"
            string op: "sequence_imm"
          number 2:
            string type: "word"
            string value: "5"
          number 3:
            string type: "word"
            string value: "7"
          number 4:
            string type: "word"
            string value: "11"
        string op: "union"
    string op: "sequence_imm"
string QueryRestr:
  number 1:
    number 1:
      string name: "cross"
      string op: "<"
      string value: 10
    number 2:
      string name: "cross"
      string op: ">"
      string value: 14
  number 2:
    string name: "factors"
    string op: ">"
    string value: 0
string QueryResult:
  string nofranked: 70
  string nofvisited: 91
  string pass: 0
  string ranks:
    number 1:
      string docno: 377
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "384"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "15"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "7"
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
          string value: "2"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "2"
          string weight: 1
        number 11:
          string index: 6
          string name: "word"
          string value: "2"
          string weight: 1
        number 12:
          string index: 7
          string name: "word"
          string value: "2"
          string weight: 1
        number 13:
          string index: 8
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 0.50000
    number 2:
      string docno: 376
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "960"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "15"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "7"
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
          string value: "2"
          string weight: 1
        number 11:
          string index: 6
          string name: "word"
          string value: "2"
          string weight: 1
        number 12:
          string index: 7
          string name: "word"
          string value: "3"
          string weight: 1
        number 13:
          string index: 8
          string name: "word"
          string value: "5"
          string weight: 1
      string weight: 0.50000
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
      string weight: 0.33333
    number 4:
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
      string weight: 0.33333
    number 5:
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
      string weight: 0.33333
    number 6:
      string docno: 716
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "96"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "15"
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
          string value: "2"
          string weight: 1
        number 11:
          string index: 6
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 0.33333
    number 7:
      string docno: 599
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "288"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "18"
          string weight: 1
        number 3:
          string index: -1
          string name: "factors"
          string value: "6"
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
          string value: "2"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "2"
          string weight: 1
        number 11:
          string index: 6
          string name: "word"
          string value: "3"
          string weight: 1
        number 12:
          string index: 7
          string name: "word"
          string value: "3"
          string weight: 1
      string weight: 0.28571
    number 8:
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
      string weight: 0.25000
    number 9:
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
      string weight: 0.25000
    number 10:
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
      string weight: 0.25000
    number 11:
      string docno: 640
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "132"
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
          string value: "11"
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
          string value: "11"
          string weight: 1
      string weight: 0.25000
    number 12:
      string docno: 627
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "150"
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
          string value: "5"
          string weight: 1
      string weight: 0.25000
    number 13:
      string docno: 559
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "330"
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
          string value: "11"
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
          string value: "11"
          string weight: 1
      string weight: 0.25000
    number 14:
      string docno: 454
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "294"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "15"
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
          string value: "7"
          string weight: 1
        number 9:
          string index: 4
          string name: "word"
          string value: "7"
          string weight: 1
      string weight: 0.25000
    number 15:
      string docno: 7
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "966"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "21"
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
          string value: "23"
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
        number 9:
          string index: 4
          string name: "word"
          string value: "23"
          string weight: 1
      string weight: 0.25000
    number 16:
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
      string weight: 0.20000
    number 17:
      string docno: 770
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "180"
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
          string value: "5"
          string weight: 1
      string weight: 0.20000
    number 18:
      string docno: 677
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "120"
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
          string value: "5"
          string weight: 1
      string weight: 0.20000
    number 19:
      string docno: 598
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "168"
          string weight: 1
        number 2:
          string index: -1
          string name: "cross"
          string value: "15"
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
          string value: "7"
          string weight: 1
      string weight: 0.20000
    number 20:
      string docno: 342
      string summary:
        number 1:
          string index: -1
          string name: "docid"
          string value: "420"
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
          string value: "5"
          string weight: 1
        number 10:
          string index: 5
          string name: "word"
          string value: "7"
          string weight: 1
      string weight: 0.20000
string QueryString: "2 3"
string QueryTerms:
  number 1:
    string type: "word"
    string value: "2"
  number 2:
    string type: "word"
    string value: "3"
string ResultList:
  number 1: "rank 1: 377 0.50000"
  number 2: "    docid: '384'"
  number 3: "    cross: '15'"
  number 4: "    factors: '7'"
  number 5: "    lo: '2'"
  number 6: "    hi: '3'"
  number 7: "    word: '2'"
  number 8: "    word: '2'"
  number 9: "    word: '2'"
  number 10: "    word: '2'"
  number 11: "    word: '2'"
  number 12: "    word: '2'"
  number 13: "    word: '2'"
  number 14: "    word: '3'"
  number 15: "rank 2: 376 0.50000"
  number 16: "    docid: '960'"
  number 17: "    cross: '15'"
  number 18: "    factors: '7'"
  number 19: "    lo: '2'"
  number 20: "    hi: '5'"
  number 21: "    word: '2'"
  number 22: "    word: '2'"
  number 23: "    word: '2'"
  number 24: "    word: '2'"
  number 25: "    word: '2'"
  number 26: "    word: '2'"
  number 27: "    word: '3'"
  number 28: "    word: '5'"
  number 29: "rank 3: 983 0.33333"
  number 30: "    docid: '30'"
  number 31: "    cross: '3'"
  number 32: "    factors: '2'"
  number 33: "    lo: '2'"
  number 34: "    hi: '5'"
  number 35: "    word: '2'"
  number 36: "    word: '3'"
  number 37: "    word: '5'"
  number 38: "rank 4: 828 0.33333"
  number 39: "    docid: '240'"
  number 40: "    cross: '6'"
  number 41: "    factors: '5'"
  number 42: "    lo: '2'"
  number 43: "    hi: '5'"
  number 44: "    word: '2'"
  number 45: "    word: '2'"
  number 46: "    word: '2'"
  number 47: "    word: '2'"
  number 48: "    word: '3'"
  number 49: "    word: '5'"
  number 50: "rank 5: 809 0.33333"
  number 51: "    docid: '42'"
  number 52: "    cross: '6'"
  number 53: "    factors: '2'"
  number 54: "    lo: '2'"
  number 55: "    hi: '7'"
  number 56: "    word: '2'"
  number 57: "    word: '3'"
  number 58: "    word: '7'"
  number 59: "rank 6: 716 0.33333"
  number 60: "    docid: '96'"
  number 61: "    cross: '15'"
  number 62: "    factors: '5'"
  number 63: "    lo: '2'"
  number 64: "    hi: '3'"
  number 65: "    word: '2'"
  number 66: "    word: '2'"
  number 67: "    word: '2'"
  number 68: "    word: '2'"
  number 69: "    word: '2'"
  number 70: "    word: '3'"
  number 71: "rank 7: 599 0.28571"
  number 72: "    docid: '288'"
  number 73: "    cross: '18'"
  number 74: "    factors: '6'"
  number 75: "    lo: '2'"
  number 76: "    hi: '3'"
  number 77: "    word: '2'"
  number 78: "    word: '2'"
  number 79: "    word: '2'"
  number 80: "    word: '2'"
  number 81: "    word: '2'"
  number 82: "    word: '3'"
  number 83: "    word: '3'"
  number 84: "rank 8: 996 0.25000"
  number 85: "    docid: '210'"
  number 86: "    cross: '3'"
  number 87: "    factors: '3'"
  number 88: "    lo: '2'"
  number 89: "    hi: '7'"
  number 90: "    word: '2'"
  number 91: "    word: '3'"
  number 92: "    word: '5'"
  number 93: "    word: '7'"
  number 94: "rank 9: 789 0.25000"
  number 95: "    docid: '60'"
  number 96: "    cross: '6'"
  number 97: "    factors: '3'"
  number 98: "    lo: '2'"
  number 99: "    hi: '5'"
  number 100: "    word: '2'"
  number 101: "    word: '2'"
  number 102: "    word: '3'"
  number 103: "    word: '5'"
  number 104: "rank 10: 774 0.25000"
  number 105: "    docid: '90'"
  number 106: "    cross: '9'"
  number 107: "    factors: '3'"
  number 108: "    lo: '2'"
  number 109: "    hi: '5'"
  number 110: "    word: '2'"
  number 111: "    word: '3'"
  number 112: "    word: '3'"
  number 113: "    word: '5'"
  number 114: "rank 11: 640 0.25000"
  number 115: "    docid: '132'"
  number 116: "    cross: '6'"
  number 117: "    factors: '3'"
  number 118: "    lo: '2'"
  number 119: "    hi: '11'"
  number 120: "    word: '2'"
  number 121: "    word: '2'"
  number 122: "    word: '3'"
  number 123: "    word: '11'"
  number 124: "rank 12: 627 0.25000"
  number 125: "    docid: '150'"
  number 126: "    cross: '6'"
  number 127: "    factors: '3'"
  number 128: "    lo: '2'"
  number 129: "    hi: '5'"
  number 130: "    word: '2'"
  number 131: "    word: '3'"
  number 132: "    word: '5'"
  number 133: "    word: '5'"
  number 134: "rank 13: 559 0.25000"
  number 135: "    docid: '330'"
  number 136: "    cross: '6'"
  number 137: "    factors: '3'"
  number 138: "    lo: '2'"
  number 139: "    hi: '11'"
  number 140: "    word: '2'"
  number 141: "    word: '3'"
  number 142: "    word: '5'"
  number 143: "    word: '11'"
  number 144: "rank 14: 454 0.25000"
  number 145: "    docid: '294'"
  number 146: "    cross: '15'"
  number 147: "    factors: '3'"
  number 148: "    lo: '2'"
  number 149: "    hi: '7'"
  number 150: "    word: '2'"
  number 151: "    word: '3'"
  number 152: "    word: '7'"
  number 153: "    word: '7'"
  number 154: "rank 15: 7 0.25000"
  number 155: "    docid: '966'"
  number 156: "    cross: '21'"
  number 157: "    factors: '3'"
  number 158: "    lo: '2'"
  number 159: "    hi: '23'"
  number 160: "    word: '2'"
  number 161: "    word: '3'"
  number 162: "    word: '7'"
  number 163: "    word: '23'"
  number 164: "rank 16: 875 0.20000"
  number 165: "    docid: '300'"
  number 166: "    cross: '3'"
  number 167: "    factors: '4'"
  number 168: "    lo: '2'"
  number 169: "    hi: '5'"
  number 170: "    word: '2'"
  number 171: "    word: '2'"
  number 172: "    word: '3'"
  number 173: "    word: '5'"
  number 174: "    word: '5'"
  number 175: "rank 17: 770 0.20000"
  number 176: "    docid: '180'"
  number 177: "    cross: '9'"
  number 178: "    factors: '4'"
  number 179: "    lo: '2'"
  number 180: "    hi: '5'"
  number 181: "    word: '2'"
  number 182: "    word: '2'"
  number 183: "    word: '3'"
  number 184: "    word: '3'"
  number 185: "    word: '5'"
  number 186: "rank 18: 677 0.20000"
  number 187: "    docid: '120'"
  number 188: "    cross: '3'"
  number 189: "    factors: '4'"
  number 190: "    lo: '2'"
  number 191: "    hi: '5'"
  number 192: "    word: '2'"
  number 193: "    word: '2'"
  number 194: "    word: '2'"
  number 195: "    word: '3'"
  number 196: "    word: '5'"
  number 197: "rank 19: 598 0.20000"
  number 198: "    docid: '168'"
  number 199: "    cross: '15'"
  number 200: "    factors: '4'"
  number 201: "    lo: '2'"
  number 202: "    hi: '7'"
  number 203: "    word: '2'"
  number 204: "    word: '2'"
  number 205: "    word: '2'"
  number 206: "    word: '3'"
  number 207: "    word: '7'"
  number 208: "rank 20: 342 0.20000"
  number 209: "    docid: '420'"
  number 210: "    cross: '6'"
  number 211: "    factors: '4'"
  number 212: "    lo: '2'"
  number 213: "    hi: '7'"
  number 214: "    word: '2'"
  number 215: "    word: '2'"
  number 216: "    word: '3'"
  number 217: "    word: '5'"
  number 218: "    word: '7'"
]]

verifyTestOutput( outputdir, result, expected)

