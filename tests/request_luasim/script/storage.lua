function GET( self, inputstr, path)
	local storage = self:get("storage")
	if inputstr then
	else
		return {storage=storage:introspection( path)}
	end
end

local function storageConfiguration( self, inputstr, objname)
	local config = schema( "storage", inputstr, true).storage
	config.path = self:get( "workdir") .. "/storage/" .. objname
	self:set( "config", config)
	return config
end

function INIT( self, inputstr, path, objname)
	if path then
		http_status( "404")
	else
		local config = storageConfiguration( self, inputstr, objname)
		local context = self:get("context")
		local storage = context:createStorageClient( config )
		self:set( "storage", storage)
	end
end

function PUT( self, inputstr, path, objname)
	if path then
		http_status( "404")
	else
		local config = storageConfiguration( self, inputstr, objname)
		local context = self:get("context")
		context:createStorage( config )
		local storage = context:createStorageClient( config )
		self:set( "storage", storage)
	end
end

function POST( self, inputstr, path, objname)
	if objname then -- storage addressed then create transaction
		local storage = self:get("storage")
		local tid = transaction( "storagetransaction", storage:createTransaction())
		return "transaction/" .. tid

	else -- no no storage addressed then create storage
		objname = counter("storage")
		PUT( self, inputstr, path, objname)
		return "storage/" .. objname
	end
end

function DELETE( self, inputstr, path, objname)
	if path or not objname then
		http_status( "404")
	else
		local config = self:get("config")
		local address = {database=config.database, path=config.path}
		local context = self:get("context")
		context:destroyStorage( address)
	end
end

