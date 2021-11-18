require "string"
require "utils"
require "config_t3s"
require "createCollection"
require "dumpCollection"

local datadir = arg[1] or "../data/t3s/"
local outputdir = arg[2] or '.'
local ctxconfig = getContextConfig( arg[3])
local storagedir = outputdir .. "/storage"
local docfiles = {"A.xml","B.xml","C.xml"}
local output = {}
local withrpc = (ctxconfig and ctxconfig.rpc) and true or false

local ctx = strus_Context.new( ctxconfig)
local storageConfig = nil

if not withrpc then
        storageConfig = string.format( "path='%s';cache=512M", storagedir)
end

createCollection( ctx, storagedir, metadata_t3s(), createDocumentAnalyzer_t3s( ctx), false, datadir, docfiles, nil, withrpc)

local queryPhrase = "City visit tokyo"

-- Get a client for the new created storage:
local storage = ctx:createStorageClient( storageConfig)

-- Define the query analyzer to use:
local analyzer = createQueryAnalyzer_t3s( ctx)

-- Define the query evaluation scheme:
local queryEval = createQueryEval_t3s( ctx)

-- Now we build the query to issue:
local query = queryEval:createQuery( storage)

-- First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
local queryAnalyzed = analyzer:analyzeTermExpression( {"word",queryPhrase})
local exprAnalyzed = analyzer:analyzeTermExpression( {
                                        "sequence", 10,
                                                {"sequence", 2, {"word","completely"}, {"word","different"}},
                                                {"sequence", 3, {"word","you"}, {"word","expect"}}
                                })

output[ "QueryString"] = queryPhrase
output[ "QueryAnalyzed"] = queryAnalyzed
output[ "QueryExpression"] = exprAnalyzed

if not withrpc then
        -- ... For RPC there are no posting iterators implemented
        local postings = {}
        for docno,pos in storage:postings( queryAnalyzed) do
                postings[ string.format( "query analyzed doc %s", storage:docid(docno))] = pos
        end
        for docno,pos in storage:postings( exprAnalyzed) do
                postings[ string.format( "expr analyzed doc %s", storage:docid(docno))] = pos
        end
        output[ "postings"] = postings;
end

-- Create a document feature 'seek'.
query:addFeature( "seek", queryAnalyzed, 1.0)

-- We assign the feature created to the set named 'select' because this is the
-- name of the set defined as selection feature in the query evaluation configuration
-- (QueryEval.addSelectionFeature):
query:addFeature( "select", {"contains", 0, 1, queryAnalyzed})

-- Define the maximum number of best result (ranks) to return:
query:setMaxNofRanks( 20)
-- Define the index of the first rank (for implementing scrolling: 0 for the first,
-- 20 for the 2nd, 40 for the 3rd page, etc.):
query:setMinRank( 0)

-- Dump query to output
output[ "QueryDump"] = query:introspection()

-- Now we evaluate the query and iterate on the result to display them:
local results = query:evaluate()
output[ "QueryResult"] = results
local output_list = {}
for pos,result in ipairs(results.ranks) do
        table.insert( output_list, string.format( "rank %u: %.5f", pos, result.weight))
        for sidx,si in pairs(result.summary) do
                table.insert( output_list, string.format( "    %s: '%s'", si.name, si.value))
        end
end
output[ "ResultList"] = output_list

local result = "query evaluation:" .. dumpTreeWithFilter( output, {'docno'}) .. "\n"
local expected = [[
query evaluation:
string QueryAnalyzed:
  string arg:
    number 1:
      string type: "word"
      string value: "citi"
    number 2:
      string type: "word"
      string value: "visit"
    number 3:
      string type: "word"
      string value: "tokyo"
  string op: "union"
string QueryDump:
  string eval:
    string selection_sets:
      number 1: "select"
    string summarizers:
      number 1:
        string function: "attribute"
        string param:
          string attribute: "title"
      number 2:
        string function: "attribute"
        string param:
          string attribute: "docid"
      number 3:
        string feature:
          string match: "seek"
        string function: "matchphrase"
        string param:
          string cluster: 0.10000
          string dist_close: 8
          string dist_imm: 2
          string dist_near: 40
          string dist_sentence: 20
          string entity: ""
          string ffbase: 0.10000
          string maxdf: 1
          string sentences: 2
          string text: "orig"
    string weighting:
      number 1:
        string feature:
          string match: "seek"
        string function: "bm25"
        string param:
          string avgdoclen: 1000
          string b: 2.10000
          string k1: 0.75000
          string metadata_doclen: "doclen"
    string weighting_sets:
      number 1: "seek"
  string feature:
    number 1:
      string set: "seek"
      string struct:
        string arg:
          number 1:
            string node: "term"
            string type: "word"
            string value: "citi"
          number 2:
            string node: "term"
            string type: "word"
            string value: "visit"
          number 3:
            string node: "term"
            string type: "word"
            string value: "tokyo"
        string node: "expression"
        string op: "union"
    number 2:
      string set: "select"
      string struct:
        string arg:
          number 1:
            string arg:
              number 1:
                string node: "term"
                string type: "word"
                string value: "citi"
              number 2:
                string node: "term"
                string type: "word"
                string value: "visit"
              number 3:
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
    number 1:
      string arg:
        number 1:
          string type: "word"
          string value: "complet"
        number 2:
          string type: "word"
          string value: "differ"
      string op: "sequence"
      string range: 2
    number 2:
      string arg:
        number 1:
          string type: "word"
          string value: "you"
        number 2:
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
    number 1:
      string field:
        string end: 0
        string start: 0
      string summary:
        number 1:
          string name: "title"
          string value: "A journey through Germany"
        number 2:
          string name: "docid"
          string value: "C.xml"
        number 3:
          string name: "summary"
          string value: "A journey through Germany When I first visited germany it was still splitted into two parts."
      string weight: 0.00009
    number 2:
      string field:
        string end: 0
        string start: 0
      string summary:
        number 1:
          string name: "title"
          string value: "A visit in New York"
        number 2:
          string name: "docid"
          string value: "B.xml"
        number 3:
          string name: "summary"
          string value: "A visit in New York New York is a city with dimensions you can't imagine."
      string weight: 0.00003
    number 3:
      string field:
        string end: 0
        string start: 0
      string summary:
        number 1:
          string name: "title"
          string value: "One day in Tokyo"
        number 2:
          string name: "docid"
          string value: "A.xml"
        number 3:
          string name: "summary"
          string value: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen."
      string weight: 0.00002
string QueryString: "City visit tokyo"
string ResultList:
  number 1: "rank 1: 0.00009"
  number 2: "    title: 'A journey through Germany'"
  number 3: "    docid: 'C.xml'"
  number 4: "    summary: 'A journey through Germany When I first visited germany it was still splitted into two parts.'"
  number 5: "rank 2: 0.00003"
  number 6: "    title: 'A visit in New York'"
  number 7: "    docid: 'B.xml'"
  number 8: "    summary: 'A visit in New York New York is a city with dimensions you can't imagine.'"
  number 9: "rank 3: 0.00002"
  number 10: "    title: 'One day in Tokyo'"
  number 11: "    docid: 'A.xml'"
  number 12: "    summary: 'One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen.'"
string postings:
  string expr analyzed doc A.xml:
    number 1: 12
  string query analyzed doc A.xml:
    number 1: 4
    number 2: 6
    number 3: 9
  string query analyzed doc B.xml:
    number 1: 2
    number 2: 11
  string query analyzed doc C.xml:
    number 1: 9
]]
verifyTestOutput( outputdir, result, expected)
