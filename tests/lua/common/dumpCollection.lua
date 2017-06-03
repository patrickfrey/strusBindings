require "string"

function dumpCollection( strusctx, storagePath, docidList)
	local config = string.format( "path=%s; cache=512M; statsproc=default", storagePath)

	-- Get a client for the storage:
	local storage = strusctx:createStorageClient( config)
	local statproc = strusctx:createStatisticsProcessor( "default")

	local browser = storage:createDocumentBrowser()
	for _,docid in ipairs(docidList) do
		dumpstr = browser:get( storage:documentNumber( docid), {"docid","title","doclen","title_start","title_end"})
		print( string.format( "DUMP: %s", dumpValue( dumpstr)))
	end
	local statitr = storage:createStatisticsIterator( true)
	local content = statitr:getNext()
	while (content) do
		local statview = statproc:decode( content)
		print( string.format( "STATS: %s", dumpValue( statview)))
		content = statitr:getNext()
	end

	storage:close()
end




