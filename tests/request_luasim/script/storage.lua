function GET( self, inputstr, path)
	local storage = self:get("storage")
	if inputstr then
	else
		return {storage=storage:introspection( path)}
	end
end

local function storageConfiguration( self, inputstr, storageid)
	local config = schema( "storage", inputstr, true).storage
	config.path = self:get( "workdir") .. "/storage/" .. storageid
	self:set( "config", config)
	return config
end

function INIT( self, inputstr, path, storageid)
	if path then
		http_error( "404")
	else
		local config = storageConfiguration( self, inputstr, storageid)
		local context = self:get("context")
		local storage = context:createStorageClient( config )
		self:set( "storage", storage)
	end
end

function PUT( self, inputstr, path, storageid)
	if path then
		http_error( "404")
	else
		local config = storageConfiguration( self, inputstr, storageid)
		local context = self:get("context")
		local storage = context:createStorage( config )
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
		local config = self:get("config")
		local address = {database=config.database, path=config.path}
		local context = self:get("context")
		context:destroyStorage( address)
	end
end

