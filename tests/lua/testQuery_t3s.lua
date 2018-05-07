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
	storageConfig = string.format( "path='%s';metadata='%s';cache=512M", storagedir, metadata_t3s())
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
local terms = analyzer:analyzeTermExpression( {"word",queryPhrase})
local qexpr = analyzer:analyzeTermExpression( {
					"sequence", 10,
						{"sequence", 2, {"word","completely"}, {"word","different"}},
						{"sequence", 3, {"word","you"}, {"word","expect"}}
				})

output[ "QueryString"] = queryPhrase
output[ "QueryTerms"] = terms
output[ "QueryExpression"] = qexpr

if #terms == 0 then
	error( "query is empty")
end

if not withrpc then
	-- ... For RPC there are no posting iterators implemented
	local postings = {}
	for _,qe in ipairs( terms ) do
		for docno,pos in storage:postings( qe) do
			postings[ string.format( "term %u doc %s", _, storage:docid(docno))] = pos
		end
	end
	for _,qe in ipairs( qexpr ) do
		for docno,pos in storage:postings( qe) do
			postings[ string.format( "expr %u doc %s", _, storage:docid(docno))] = pos
		end
	end
	output[ "postings"] = postings;
end

-- Then we iterate on the terms and create a single term feature for each term and collect
-- all terms to create a selection expression out of them:
local selexpr = {}

for _,term in ipairs(terms) do
	-- Each query term is also part of the selection expressions
	table.insert( selexpr, term)
	-- Create a document feature 'seek'.
	query:addFeature( "seek", term, 1.0)
end
-- We assign the feature created to the set named 'select' because this is the
-- name of the set defined as selection feature in the query evaluation configuration
-- (QueryEval.addSelectionFeature):
query:addFeature( "select", {"contains", 0, 1, table.unpack(selexpr)})

-- Define the maximum number of best result (ranks) to return:
query:setMaxNofRanks( 20)
-- Define the index of the first rank (for implementing scrolling: 0 for the first, 
-- 20 for the 2nd, 40 for the 3rd page, etc.):
query:setMinRank( 0)

-- Define the title field:
query:addFeature( "titlefield", {from="title_start", to="title_end"} )

-- Enable debugging
query:setDebugMode( false )

-- Dump query to output
output[ "QueryDump"] = query:tostring()

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
string QueryDump: "query evaluation program:
SELECT select;
EVAL  BM25( b=2.1, k1=0.75, avgdoclen=1000, metadata_doclen=, match= %seek);
SUMMARIZE attribute( metaname='title', resultname='title');
SUMMARIZE attribute( metaname='docid', resultname='docid');
SUMMARIZE matchphrase( type='orig', matchmark=(,), floatingmark=(... , ...), name para=para, name phrase=phrase, name docstart=docstart, paragraphsize=300, sentencesize=40, windowsize=30, cardinality='0', maxdf='0.1', match= %seek, title= %titlefield);
feature 'seek' 1.00000: 
  term word 'citi'

feature 'seek' 1.00000: 
  term word 'visit'

feature 'seek' 1.00000: 
  term word 'tokyo'

feature 'select' 1.00000: 
  contains range=0 cardinality=1:
    term word 'citi'
    term word 'visit'
    term word 'tokyo'

feature 'titlefield' 1.00000: 
  docfield title_start : title_end

maxNofRanks = 20
minRank = 0
"
string QueryExpression:
  number 1:
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
      string summary:
        number 1:
          string index: -1
          string name: "title"
          string value: "One day in Tokyo"
          string weight: 1
        number 2:
          string index: -1
          string name: "docid"
          string value: "A.xml"
          string weight: 1
        number 3:
          string index: -1
          string name: "docstart"
          string value: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen. ..."
          string weight: 1
      string weight: 0.64282
    number 2:
      string summary:
        number 1:
          string index: -1
          string name: "title"
          string value: "A visit in New York"
          string weight: 1
        number 2:
          string index: -1
          string name: "docid"
          string value: "B.xml"
          string weight: 1
        number 3:
          string index: -1
          string name: "docstart"
          string value: "A visit in New York New York is a city with dimensions you can't imagine. ..."
          string weight: 1
      string weight: 0.00017
    number 3:
      string summary:
        number 1:
          string index: -1
          string name: "title"
          string value: "A journey through Germany"
          string weight: 1
        number 2:
          string index: -1
          string name: "docid"
          string value: "C.xml"
          string weight: 1
        number 3:
          string index: -1
          string name: "docstart"
          string value: "A journey through Germany When I first visited germany it was still splitted into two parts. ..."
          string weight: 1
      string weight: 0.00009
string QueryString: "City visit tokyo"
string QueryTerms:
  number 1:
    string type: "word"
    string value: "citi"
  number 2:
    string type: "word"
    string value: "visit"
  number 3:
    string type: "word"
    string value: "tokyo"
string ResultList:
  number 1: "rank 1: 0.64282"
  number 2: "    title: 'One day in Tokyo'"
  number 3: "    docid: 'A.xml'"
  number 4: "    docstart: 'One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen. ...'"
  number 5: "rank 2: 0.00017"
  number 6: "    title: 'A visit in New York'"
  number 7: "    docid: 'B.xml'"
  number 8: "    docstart: 'A visit in New York New York is a city with dimensions you can't imagine. ...'"
  number 9: "rank 3: 0.00009"
  number 10: "    title: 'A journey through Germany'"
  number 11: "    docid: 'C.xml'"
  number 12: "    docstart: 'A journey through Germany When I first visited germany it was still splitted into two parts. ...'"
string postings:
  string expr 1 doc A.xml:
    number 1: 12
  string term 1 doc A.xml:
    number 1: 9
  string term 1 doc B.xml:
    number 1: 11
  string term 2 doc B.xml:
    number 1: 2
  string term 2 doc C.xml:
    number 1: 9
  string term 3 doc A.xml:
    number 1: 4
    number 2: 6
]]
verifyTestOutput( outputdir, result, expected)

