require "string"

function dumpCollection( strusctx, storagePath, docidList)
	local config = string.format( "path=%s; cache=512M; statsproc=default", storagePath)
	local output = {}

	-- Get a client for the storage:
	local storage = strusctx:createStorageClient( config)
	local statproc = strusctx:createStatisticsProcessor( "default")

	-- Document metadata and attributes:
	local output_docs = {}
	for docrow in storage:select( {"docno","docid","title","doclen","title_start","title_end"}, nil, nil, 0) do
		output_docs[ docrow.docid] = docrow
	end
	output[ "docs"] = output_docs

	-- Term statistics:
	local output_stat = {}
	local statitr = storage:createStatisticsIterator( true)
	local content = statitr:getNext()
	while (content) do
		local statview = statproc:decode( content)
		table.insert( output_stat, statview)
		content = statitr:getNext()
	end
	output[ "stat"] = output_stat

	-- Term types:
	local output_types = {}
	local typeitr = storage:termTypes()
	for tp in typeitr do
		table.insert( output_types, tp)
	end
	output[ "types"] = output_types

	-- Document identifiers:
	local output_docids = {}
	local dociditr = storage:docids()
	for tp in dociditr do
		table.insert( output_docids, tp)
	end
	output[ "docids"] = output_docids

	storage:close()
	return output
end

