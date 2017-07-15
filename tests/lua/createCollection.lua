require "string"
require "utils"

function createCollection( strusctx, storagePath, metadata, analyzer, multipart, datadir, fnams, aclmap, withrpc)
	local config = nil
	if not withrpc then
		config = {
			path = storagePath,
			metadata = metadata,
			cache = '512M',
			statsproc = 'default'
		}
		if aclmap then
			config.acl = true
		end
		strusctx:destroyStorage( config)
		strusctx:getLastError()

		strusctx:createStorage( config)
	end
	-- Get a client for the new created storage:
	local storage = strusctx:createStorageClient( config)

	-- Read input files, analyze and insert them:
	local transaction = storage:createTransaction()
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
			doc.attribute.docclass = string.format( "mimetype=%s, encoding=%s, scheme=%s", docclass.mimetype, docclass.encoding, docclass.scheme)
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




