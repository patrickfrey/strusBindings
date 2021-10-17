function GET( self, inputstr, path)
	local storage = self:get("storage")
	if inputstr then
	else
		return {storage=storage:introspection( path)}
	end
end

function INIT( self, inputstr, path, storageid)
	if path then
		http_error( "404")
	else
		local input = schema( "storage", inputstr, true).storage
		input.path = self:get( "workdir") .. "/storage/" .. storageid
		local context = self:get("context")
		local storage = context:createStorageClient( input )
		self:set( "storage", storage)
	end
end

function PUT( self, inputstr, path, storageid)
	if path then
		http_error( "404")
	else
		print( "+++ STORAGE ID " .. storageid)
		local input = schema( "storage", inputstr, true).storage
		input.path = self:get( "workdir") .. "/storage/" .. storageid
		local context = self:get("context")
		local storage = context:createStorage( input )
		self:set( "storage", storage)
	end
end

function POST( self, inputstr, path, storageid)
	if storageid then -- storage addressed then create transaction
		local storage = self:get("storage")
		local tid = transaction( "storagetransaction", contentstats:createTransaction())
		return "transaction/" .. tid

	else -- no no storage addressed then create storage
		storageid = counter("storage")
		PUT( self, inputstr, path, storageid)
		return "storage/" .. storageid
	end
end

function DELETE( self, inputstr, path, storageid)
	if path or not storageid then
		http_error( "404")
	else
		local input = schema( "storageAddress", inputstr, true).storageAddress
		local context = self:get("context")
		context:destroyStorage( input)
	end
end

