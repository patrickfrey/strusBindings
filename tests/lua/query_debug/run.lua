require "string"
package.path = "../common/testCollection.lua"
require "testCollection"

function dumpValue(o)
	if type(o) == 'table' then
		local s = '{ '
		for k,v in pairs(o) do
			if type(k) ~= 'number' then k = '"'..k..'"' end
			s = s .. '['..k..'] = ' .. dump(v) .. ','
		end
		return s .. '} '
	else
		return tostring(o)
	end
end

queryPhrase = "New York dimensions imagine"
storagePath = arg[1] .. "/storage"
storageConfig = string.format( "path=%s", storagePath)
createCollection( storagePath, arg[1], {"A.xml","B.xml","C.xml"})

ctx = strus_Context.new()
ctx:loadModule( "analyzer_pattern")

-- Get a client for the new created storage:
storage = ctx:createStorageClient( config)

-- Define the query analyzer to use:
analyzer = ctx:createQueryAnalyzer()
analyzer:addSearchIndexElement( "word", "word", "word", {{"stem","en"},"lc",{"convdia","en"}})

-- Define the query evaluation scheme:
queryEval = ctx:createQueryEval()

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
query = queryEval:createQuery( storage)

-- First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
terms = analyzer:analyze( {"word",queryPhrase})
print( ctx:debug_serialize( {"word",queryPhrase}))
if #terms == 0 then
	error( "query is empty")
end

-- Then we iterate on the terms and create a single term feature for each term and collect
-- all terms to create a selection expression out of them:
selexpr = {}

for _,term in ipairs(terms) do
	dumpValue( term)
	-- Each query term is also part of the selection expressions
	table.insert( selexpr, {term.type, term.value, term.length})
	print( string.format( "term %s '%s' (%u)\n", term.type, term.value, #selexpr))
	-- Create a document feature 'seek'.
	query.defineFeature( "seek", {term.type, term.value, term.length}, 1.0)
end
-- We assign the feature created to the set named 'select' because this is the
-- name of the set defined as selection feature in the query evaluation configuration
-- (QueryEval.addSelectionFeature):
query:defineFeature( "select", {"contains"}, #selexpr, unpack(selexpr))

-- Define the maximum number of best result (ranks) to return:
query:setMaxNofRanks( 20)
-- Define the index of the first rank (for implementing scrolling: 0 for the first, 
-- 20 for the 2nd, 40 for the 3rd page, etc.):
query:setMinRank( 0)

-- Define the title field:
query:defineDocFieldFeature( "titlefield", "", "title_end" )

-- Enable debugging
query:setDebugMode( True )

-- Now we evaluate the query and iterate on the result to display them:
results = query:evaluate()
for pos,result in ipairs(results.ranks) do
	print( string.format( "rank %u: %u %f\n", pos, result.docno, result.weight))
	for sidx,si in pairs(result.summary) do
		print( string.format( "\t %s: '%s'\n", si.name, si.value))
	end
end
print "done"
