require "string"

local function readFile( path)
	local file = io.open( path, "rb") -- r read mode and b binary mode
	if not file then return nil end
	local content = file:read "*a" -- *a or *all reads the whole file
	file:close()
	return content
end

function createCollection( datadir, storagename)
	storagePath = datadir .. "/" .. storagename
	config = string.format( "path=%s; metadata=doclen UINT16, title_start UINT8, title_end UINT8", storagePath)
	ctx = strus_Context.new()
	ctx:loadModule( "analyzer_pattern")

	ctx:destroyStorage( config)
	ctx:getLastError()

	ctx:createStorage( config)
	-- Get a client for the new created storage:
	storage = ctx:createStorageClient( config)

	-- Define the document analyzer to use:
	analyzer = ctx:createDocumentAnalyzer( {"xml"})
	
	-- Define the features and attributes to store:
	analyzer:addSearchIndexFeature( "word", "/doc/text()", "word", {{"stem","en"},"lc",{"convdia","en"}})
	analyzer:addSearchIndexFeature( "word", "/doc/title()", "word", {{"stem","en"},"lc",{"convdia","en"}})
	analyzer:addSearchIndexFeature( "endtitle", "/doc/title~", "content", "empty")
	analyzer:addForwardIndexFeature( "orig", "/doc/text()", "split", "orig")
	analyzer:addForwardIndexFeature( "orig", "/doc/title()", "split", "orig")
	analyzer:defineAttribute( "title", "/doc/title()", "content", "orig")
	analyzer:defineAggregatedMetaData( "title_end", {"nextpos", "endtitle"})

	analyzer:definePatternMatcherPostProc( "coresult", "std", {"word"}, {
		{"city_that_is", {"sequence", 3, {"word","citi"},{"word","that"},{"word","is"}} },
		{"city_that", {"sequence", 2, {"word","citi"},{"word","that"}}},
		{"city_with", {"sequence", 2, {"word","citi"},{"word","with"}}}
	})
	analyzer:addSearchIndexFeatureFromPatternMatch( "word", "coresult", {"lc"})
	
	-- Read input files, analyze and insert them:
	transaction = storage:createTransaction()
	files = {}
	idx = 0
	for fnam in lfs.dir(datadir) do
		local filename = datadir..'/'..fnam
		local mode = lfs.attributes( fullPath,"mode")
		if mode=="file" and string.sub( filename,-4) == '.xml' then
			idx = idx + 1
			print( string.format( "%u process document %s", idx, filename))
			docid = string.sub( filename,-4)
			doc = analyzer:analyze( readFile( filename))
			transaction:insertDocument( docid, doc)

		end
	end
	-- Without this the documents wont be inserted:
	transaction:commit()
end




