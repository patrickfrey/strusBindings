function GET( self, inputstr, path)
	local storage = self:get("storage")
	if inputstr then
	else
		return {storage=storage:introspection( path)}
	end
end

local function storageConfiguration( self, inputstr, objname)
	local config = schema( "storage", inputstr, true).storage
	config.path = "storage/" .. objname
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
	if objname then
		local storage = self:get("storage")
		if path == "transaction" then
			local tid = transaction( "storagetransaction", storage:createTransaction())
			return "transaction/" .. tid
		elseif path == "statfetch" then
			local timestamp = nil
			if inputstr then input = schema( "statfetch", inputstr, true).statfetch.timestamp end
			local iterator = timestamp and storage:getChangeStatistics( timestamp) or storage:getAllStatistics()
			local tid = transaction( "statfetch", {iterator=iterator,timestamp=storage:currentTimeStamp()})
			return "transaction/" .. tid
		end
	else
		if not path then
			objname = counter("storage")
			PUT( self, inputstr, path, objname)
			return "storage/" .. objname
		end
	end
	http_status( "404")
end

function DELETE( self, inputstr, path, objname)
	if path or not objname then
		http_status( "404")
	else
		local config = self:get("config")
		local address = {database=config.database, path=config.path}
		local context = self:get("context")
		local storage = self:get("storage")
		storage:close()
		context:destroyStorage( address)
	end
end

