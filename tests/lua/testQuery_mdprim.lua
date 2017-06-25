require "string"
require "utils"
require "config_mdprim"
require "createCollection"
require "dumpCollection"

local datadir = arg[1]
local outputdir = arg[2] or '.'
local storagedir = outputdir .. "/storage"
local docfiles = {"docsmall.xml"}
local output = {}

local ctx = strus_Context.new()
ctx:loadModule( "analyzer_pattern")

createCollection( ctx, storagedir, metadata_mdprim(), createDocumentAnalyzer_mdprim( ctx), true, datadir, docfiles)

local queryPhrase = "13"
local storageConfig = string.format( "path='%s';metadata='%s';cache=512M", storagedir, metadata_mdprim())

-- Get a client for the new created storage:
local storage = ctx:createStorageClient( storageConfig)

-- Define the query analyzer to use:
local analyzer = createQueryAnalyzer_mdprim( ctx)

-- Define the query evaluation scheme:
local queryEval = createQueryEval_t3s( ctx)

-- Now we build the query to issue:
local query = queryEval:createQuery( storage)

-- First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
local terms = analyzer:analyzeTermExpression( {"word",queryPhrase})

output[ "QueryString"] = queryPhrase
output[ "QueryTerms"] = terms

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
	query:defineFeature( "seek", term, 1.0)
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
	table.insert( output_list, string.format( "rank %u: %u %.5f", pos, result.docno, result.weight))
	for sidx,si in pairs(result.summary) do
		table.insert( output_list, string.format( "    %s: '%s'", si.name, si.value))
	end
end
output[ "ResultList"] = output_list

local result = "query evaluation:" .. dumpTree( "", output) .. "\n"
local expected = [[
]]

verifyTestOutput( outputdir, result, expected)

