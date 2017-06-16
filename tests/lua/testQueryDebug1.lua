require "string"
require "utils"
require "createCollection"
require "dumpCollection"

local datadir = arg[1]
local outputdir = arg[2] or '.'
local storage = outputdir .. "/storage"
local docfiles = {"A.xml","B.xml","C.xml"}

ctx = strus_Context.new()
ctx:loadModule( "analyzer_pattern")

createCollection( ctx, storage, datadir, docfiles)

queryPhrase = "New York dimensions imagine"
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
queryEval:addWeightingFunction(
	"BM25pff", {
			{"k1",1.2}, {"b", 0.75}, {"avgdoclen", 1000},
			{"metadata_doclen", "doclen"},
			{"titleinc", 4.0}, {"windowsize", 40}, {'cardinality', "70%"},
			{"ffbase", 0.4},
			{"maxdf", 0.9},
			{"para", {feature="para"}}, {"struct", {feature="sentence"}},
			{"match", {feature="seek"}}, {"title", {feature="titlefield"}},
			{"debug","debug_BM25pff"}
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
local terms = analyzer:analyze( {type="word",value=queryPhrase})
print (string.format( "QUERY '%s' %s", queryPhrase, dumpValue( terms)))

print( ctx:debug_serialize( {"word",queryPhrase}))
if #terms == 0 then
	error( "query is empty")
end

-- Then we iterate on the terms and create a single term feature for each term and collect
-- all terms to create a selection expression out of them:
local selexpr = {}

for _,term in ipairs(terms) do
	print (string.format( "TERM %s", dumpValue( term)))
	-- Each query term is also part of the selection expressions
	table.insert( selexpr, {term[1], term[2]})
	print( string.format( "term %s '%s' (%u)\n", term[1], term[2], #selexpr))
	-- Create a document feature 'seek'.
	query:defineFeature( "seek", {term[1], term[2]}, 1.0)
end
-- We assign the feature created to the set named 'select' because this is the
-- name of the set defined as selection feature in the query evaluation configuration
-- (QueryEval.addSelectionFeature):
query:defineFeature( "select", {"contains", #selexpr, table.unpack(selexpr)})

-- Define the maximum number of best result (ranks) to return:
query:setMaxNofRanks( 20)
-- Define the index of the first rank (for implementing scrolling: 0 for the first, 
-- 20 for the 2nd, 40 for the 3rd page, etc.):
query:setMinRank( 0)

-- Define the title field:
query:defineFeature( "titlefield", {from="title_start", to="title_end"} )

-- Enable debugging
query:setDebugMode( true )

-- Now we evaluate the query and iterate on the result to display them:
local results = query:evaluate()
for pos,result in ipairs(results.ranks) do
	print( string.format( "rank %u: %u %f\n", pos, result.docno, result.weight))
	for sidx,si in pairs(result.summary) do
		print( string.format( "\t %s: '%s'\n", si.name, si.value))
	end
end
print "done"
