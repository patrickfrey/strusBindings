require "string"
require "utils"

function dumpCollection( strusctx, storagePath)
	local config = string.format( "path=%s; cache=512M; statsproc=std", storagePath)
	local output = {}

	-- Get a client for the storage:
	local storage = strusctx:createStorageClient( config)

	-- Configuration of the storage:
	local output_config = {}
	for key,value in pairs( storage:config()) do
		if key == 'path' then
			output_config[ key] = getFileName( value)
		else
			output_config[ key] = value
		end
	end
	output[ "config"] = output_config
	output[ "nofdocs"] = storage:nofDocumentsInserted()

	-- Document identifiers:
	local output_docids = {}
	local dociditr = storage:docids()
	for tp in dociditr do
		table.insert( output_docids, tp)
	end
	table.sort( output_docids)
	output[ "docids"] = output_docids

	-- Term types:
	local termtypes = joinLists( storage:termTypes())
	output[ "types"] = termtypes

	-- Document data (metadata,attributes,content):
	local selectids = joinLists( "docno", "ACL", storage:attributeNames(), storage:metadataNames(), termtypes)
	local output_docs = {}
	local output_terms = {}
	for docrow in storage:select( selectids, nil, nil, 0) do
		flatdocrow = {}
		for colkey,colval in pairs(docrow) do
			if colkey ~= "docno" then
				flatdocrow[ colkey] = concatValues( colval)
			end
		end
		output_docs[ docrow.docid] = flatdocrow

		for _,termtype in ipairs( termtypes) do
			local fterms = storage:documentForwardIndexTerms( docrow.docno, termtype)
			local ftermlist = {}
			for fterm,pos in fterms do
				table.insert( ftermlist, {value=fterm,pos=pos})
			end
			output_terms[ string.format("%s:%s (f)", docrow.docid, termtype)] = ftermlist

			local sterms = storage:documentSearchIndexTerms( docrow.docno, termtype)
			local stermlist = {}
			for sterm,tf,firstpos in sterms do
				table.insert( stermlist, {value=sterm,tf=tf,firstpos=firstpos})
			end
			output_terms[ string.format("%s:%s (s)", docrow.docid, termtype)] = stermlist
		end
	end
	output[ "docs"] = output_docs
	output[ "terms"] = output_terms

	-- Term statistics:
	local dfchangelist = {}
	local nofdocs = 0

	function dfchange_less( a, b)
		if a.type < b.type then return true end
		if a.type > b.type then return false end
		if a.value < b.value then return true end
		if a.value > b.value then return false end
		if a.increment < b.increment then return true end
		if a.increment > b.increment then return false end
		return false;
	end

	local bloblist = {}
	for blob in storage:getAllStatistics() do
		blob.timestamp.unixtime = 1
		table.insert( bloblist, blob);
		local statview = strusctx:unpackStatisticBlob( blob, "std")
		nofdocs = nofdocs + statview[ "nofdocs"]
		for _,dfchange in ipairs(statview[ "dfchange"]) do
			table.insert( dfchangelist, dfchange)
		end
	end
	table.sort( dfchangelist, dfchange_less )
	output[ "statblobs"] = bloblist
	output[ "stat"] = { dfchange=dfchangelist, nofdocs=nofdocs }

	storage:close()
	return output
end

