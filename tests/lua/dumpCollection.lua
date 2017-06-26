require "string"

function dumpCollection( strusctx, storagePath)
	local config = string.format( "path=%s; cache=512M; statsproc=default", storagePath)
	local output = {}

	-- Get a client for the storage:
	local storage = strusctx:createStorageClient( config)

	-- Configuration of the storage:
	local output_config = {}
	for key,value in pairs(storage:config()) do
		if key == 'path' then
			output_config[ key] = getFileName( value)
		else
			output_config[ key] = value
		end
	end
	output[ "config"] = output_config

	-- Document identifiers:
	local output_docids = {}
	local dociditr = storage:docids()
	for tp in dociditr do
		table.insert( output_docids, {id=tp,docno=storage:documentNumber(tp)})
	end
	output[ "docids"] = output_docids

	-- Term types:
	local termtypes = {}
	local typeitr = storage:termTypes()
	for tp in typeitr do
		table.insert( termtypes, tp)
	end
	output[ "types"] = termtypes

	-- Document data (metadata,attributes,content):
	local selectids = {"docno"}
	local attrids = {}
	for _,name in ipairs( storage:attributeNames()) do
		table.insert( attrids, name)
		table.insert( selectids, name)
	end
	for _,name in ipairs( storage:metadataNames()) do
		table.insert( attrids, name)
		table.insert( selectids, name)
	end
	for _,name in ipairs( termtypes) do
		table.insert( selectids, name)
	end
	local output_docs = {}
	for docrow in storage:select( selectids, nil, nil, 0) do
		output_docs[ docrow.docid] = docrow
	end
	output[ "docs"] = output_docs

	-- Term statistics:
	local output_stat = {}
	for blob in storage:getAllStatistics( true) do
		local statview = strusctx:unpackStatisticBlob( blob, "default")
		local dfchangelist = {}
		for _,dfchange in ipairs(statview[ "dfchange"]) do
			table.insert( dfchangelist, dfchange)
		end
		output_stat[ "dfchange"] = dfchangelist
		output_stat[ "nofdocs"] = (output_stat[ "nofdocs"] or 0) + statview[ "nofdocs"]
	end
	output[ "stat"] = output_stat

	storage:close()
	return output
end

