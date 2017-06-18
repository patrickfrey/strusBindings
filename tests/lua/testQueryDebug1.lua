require "string"
require "utils"
require "createCollection"
require "dumpCollection"

local datadir = arg[1]
local outputdir = arg[2] or '.'
local storage = outputdir .. "/storage"
local docfiles = {"A.xml","B.xml","C.xml"}
local output = {}

ctx = strus_Context.new()
ctx:loadModule( "analyzer_pattern")

createCollection( ctx, storage, datadir, docfiles)

queryPhrase = "City visit"
storageConfig = string.format( "path='%s';metadata='doclen UINT16,title_start UINT8,title_end UINT8';cache=512M", storage)

-- Get a client for the new created storage:
local storage = ctx:createStorageClient( storageConfig)

-- Define the query analyzer to use:
local analyzer = ctx:createQueryAnalyzer()
analyzer:addSearchIndexElement( "word", "word", "word", {{"stem","en"},"lc",{"convdia","en"}})

-- Define the query evaluation scheme:
local queryEval = ctx:createQueryEval()

-- Here we define what query features decide, what is ranked for the result:
queryEval:addSelectionFeature( "select")
	
-- Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
queryEval:addWeightingFunction(
	"BM25", {
			k1=0.75, b=2.1, avgdoclen=1000, match={feature="seek"}, debug="debug_BM25"
		})

-- Now we define what attributes of the documents are returned and how they are build.
-- The functions that extract stuff from documents for presentation are called summarizers.
-- First we add a summarizer that extracts us the title of the document:
queryEval:addSummarizer( "attribute", {{"name", "title"},{"debug","debug_attribute"}})

-- Then we add a summarizer that collects the sections that enclose the best matches 
-- in a ranked document:
queryEval:addSummarizer(
	"matchphrase", {
			{"type","orig"}, {"sentencesize",40}, {"windowsize",30},
			{"match",{feature="seek"}},{"title",{feature="titlefield"}},
			{"debug","debug_matchphrase"}
		})

-- Now we build the query to issue:
local query = queryEval:createQuery( storage)

-- First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
local terms = analyzer:analyze( {"word",queryPhrase})
print( string.format( "query expression [%s]", dumpValue(
		ctx:debug_serialize( {
					"sequence", 10,
						{"sequence", 2, {"word","completely"}, {"word","different"}},
						{"sequence", 3, {"word","you"}, {"word","expect"}}
					})
)))
local qexpr = analyzer:analyze( {
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

-- Then we iterate on the terms and create a single term feature for each term and collect
-- all terms to create a selection expression out of them:
local selexpr = {}

for _,term in ipairs(terms) do
	-- Each query term is also part of the selection expressions
	table.insert( selexpr, {term.type, term.value})
	-- Create a document feature 'seek'.
	query:defineFeature( "seek", {term.type, term.value}, 1.0)
end
-- We assign the feature created to the set named 'select' because this is the
-- name of the set defined as selection feature in the query evaluation configuration
-- (QueryEval.addSelectionFeature):
query:defineFeature( "select", {"contains", 0, 1, table.unpack(selexpr)})

-- Define the maximum number of best result (ranks) to return:
query:setMaxNofRanks( 20)
-- Define the index of the first rank (for implementing scrolling: 0 for the first, 
-- 20 for the 2nd, 40 for the 3rd page, etc.):
query:setMinRank( 0)

-- Define the title field:
query:defineFeature( "titlefield", {from="title_start", to="title_end"} )

-- Enable debugging
query:setDebugMode( false )

-- Dump query to output
output[ "QueryDump"] = query:tostring()

-- Now we evaluate the query and iterate on the result to display them:
local results = query:evaluate()
output[ "QueryResult"] = results
local output_list = {}
for pos,result in ipairs(results.ranks) do
	table.insert( output_list, string.format( "rank %u: %u %.6f", pos, result.docno, result.weight))
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
EVAL  BM25( b=2.1, k1=0.75, avgdoclen=1000, metadata_doclen=, match= %seek);
SUMMARIZE attribute( metaname='title', resultname='title');
SUMMARIZE matchphrase( type='orig', matchmark=(,), floatingmark=(... , ...), name para=para, name phrase=phrase, name docstart=docstart, paragraphsize=300, sentencesize=40, windowsize=30, cardinality='0', maxdf='0.1', match= %seek, title= %titlefield);
feature 'seek' 1.00000: 
  term word 'citi'

feature 'seek' 1.00000: 
  term word 'visit'

feature 'select' 1.00000: 
  contains range=0 cardinality=1:
    term word 'citi'
    term word 'visit'

feature 'titlefield' 1.00000: 
  docfield title_start : title_end

maxNofRanks = 20
minRank = 0
"
string QueryResult: 
  string nofranked: 3
  string nofvisited: 3
  string pass: 0
  string ranks: 
    number 1: 
      string docno: 2
      string summary: 
        number 1: 
          string index: -1
          string name: "title"
          string value: "A visit in New York"
          string weight: 1
        number 2: 
          string index: -1
          string name: "phrase"
          string value: "A visit in New York New York is a city ..."
          string weight: 1
      string weight: 0.000172
    number 2: 
      string docno: 1
      string summary: 
        number 1: 
          string index: -1
          string name: "title"
          string value: "A journey through Germany"
          string weight: 1
        number 2: 
          string index: -1
          string name: "docstart"
          string value: "A journey through Germany When I first visited germany it was still splitted into two parts. ..."
          string weight: 1
      string weight: 0.000087
    number 3: 
      string docno: 3
      string summary: 
        number 1: 
          string index: -1
          string name: "title"
          string value: "One day in Tokyo"
          string weight: 1
        number 2: 
          string index: -1
          string name: "docstart"
          string value: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen. ..."
          string weight: 1
      string weight: 0.000083
string QueryString: "City visit"
string QueryTerms: 
  number 1: 
    string len: 1
    string pos: 1
    string type: "word"
    string value: "citi"
  number 2: 
    string len: 1
    string pos: 2
    string type: "word"
    string value: "visit"
string ResultList: 
  number 1: "rank 1: 2 0.000172"
  number 2: "    title: 'A visit in New York'"
  number 3: "    phrase: 'A visit in New York New York is a city ...'"
  number 4: "rank 2: 1 0.000087"
  number 5: "    title: 'A journey through Germany'"
  number 6: "    docstart: 'A journey through Germany When I first visited germany it was still splitted into two parts. ...'"
  number 7: "rank 3: 3 0.000083"
  number 8: "    title: 'One day in Tokyo'"
  number 9: "    docstart: 'One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen. ...'"
]]
verifyTestOutput( outputdir, result, expected)

