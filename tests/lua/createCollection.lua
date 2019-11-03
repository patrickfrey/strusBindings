require "string"
require "utils"
require "math"

function createCollection( strusctx, storagePath, metadata, analyzer, multipart, datadir, fnams, aclmap, withrpc)
	local config = nil
	if not withrpc then
		config = {
			path = storagePath,
			cache = '512M',
			statsproc = 'std'
		}
		if aclmap then
			config.acl = true
		end
		if strusctx:storageExists( config) then
			strusctx:destroyStorage( config)
		end
		strusctx:createStorage( config)
	end
	-- Get a client for the new created storage:
	local storage = strusctx:createStorageClient( config)

	local transaction = storage:createTransaction()
	-- Create the meta table structure:
	if (math.random(10) <= 5) then
		local metadatacmds = {}
		for mi,md in ipairs(metadata) do
			table.insert( metadatacmds, {"add", md[0], md[1]} )
		end
		transaction:updateMetaDataTable( metadatacmds)
	else
		transaction:defineMetaDataTable( metadata)
	end
	transaction:commit()

	-- Read input files, analyze and insert them:
	local files = {}
	local idx = 0
	for _,fnam in ipairs(fnams) do
		local filename = datadir..'/'..fnam
		idx = idx + 1
		if multipart then
			for doc in analyzer:analyzeMultiPart( readFile( filename)) do
				if aclmap then
					doc.access = aclmap[ doc.attribute.docid]
				end
				transaction:insertDocument( doc.attribute.docid, doc)
			end
		else
			local content = readFile( filename)
			local docclass = strusctx:detectDocumentClass( content)
			local doc = analyzer:analyzeSingle( content)
			doc.attribute.docid = fnam
			doc.attribute.docclass = string.format( "mimetype=%s, encoding=%s, schema=%s", docclass.mimetype, docclass.encoding, docclass.schema)
			if aclmap then
				doc.access = aclmap[ doc.attribute.docid]
			end
			transaction:insertDocument( fnam, doc)
		end
	end
	-- Without this the documents wont be inserted:
	transaction:commit()
	storage:close()
end




