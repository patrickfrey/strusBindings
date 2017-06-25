require "string"
require "utils"

function createCollection( strusctx, storagePath, metadata, analyzer, multipart, datadir, fnams)
	local config = {
		path = storagePath,
		metadata = metadata,
		cache = '512M',
		statsproc = 'default'
	}
	strusctx:destroyStorage( config)
	strusctx:getLastError()

	strusctx:createStorage( config)
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
				transaction:insertDocument( doc.attribute.docid, doc)
			end
		else
			doc = analyzer:analyzeSingle( readFile( filename))
			doc.attribute.docid = fnam
			transaction:insertDocument( fnam, doc)
		end
	end
	-- Without this the documents wont be inserted:
	transaction:commit()
	storage:close()
end




