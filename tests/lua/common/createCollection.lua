require "string"

function createCollection( strusctx, storagePath, datadir, fnams)
	local config = {
		path = storagePath,
		metadata = 'doclen UINT16, title_start UINT8, title_end UINT8',
		cache = '512M',
		statsproc = 'default'
	}
	strusctx:destroyStorage( config)
	strusctx:getLastError()

	strusctx:createStorage( config)
	-- Get a client for the new created storage:
	local storage = strusctx:createStorageClient( config)

	-- Define the document analyzer to use:
	local analyzer = strusctx:createDocumentAnalyzer( {"xml"})

	-- Define the features and attributes to store:
	analyzer:addSearchIndexFeature( "word", "/doc/text()", "word", {{"stem","en"},"lc",{"convdia","en"}})
	analyzer:addSearchIndexFeature( "word", "/doc/title()", "word", {{"stem","en"},"lc",{"convdia","en"}})
	analyzer:addSearchIndexFeature( "endtitle", "/doc/title~", "content", "empty")
	analyzer:addForwardIndexFeature( "orig", "/doc/text()", "split", "orig")
	analyzer:addForwardIndexFeature( "orig", "/doc/title()", "split", "orig")
	analyzer:defineAttribute( "title", "/doc/title()", "content", "orig")
	analyzer:defineAggregatedMetaData( "title_end", {"nextpos", "endtitle"})
	analyzer:defineAggregatedMetaData( "doclen", {"count", "word"})

	analyzer:definePatternMatcherPostProc( "coresult", "std", {"word"}, {
		{"city_that_is", {"sequence", 3, {"word","citi"},{"word","that"},{"word","is"}} },
		{"city_that", {"sequence", 2, {"word","citi"},{"word","that"}}},
		{"city_with", {"sequence", 2, {"word","citi"},{"word","with"}}}
	})
	analyzer:addSearchIndexFeatureFromPatternMatch( "word", "coresult", {"lc"})
	
	-- Read input files, analyze and insert them:
	local transaction = storage:createTransaction()
	local files = {}
	local idx = 0
	for _,fnam in ipairs(fnams) do
		local filename = datadir..'/'..fnam
		idx = idx + 1
		doc = analyzer:analyze( readFile( filename))
		table.insert( doc.attributes, {name="docid", value=fnam})
		transaction:insertDocument( fnam, doc)
	end
	-- Without this the documents wont be inserted:
	transaction:commit()
	storage:close()
end




