function GET( self, input, path)
	local contentstats = self:get("context")
	return {context={context:introspection( path)}}
end

-- Dumping table contents to a string
local function dump( data, indent)
	local indent = indent or "\n"
	if type( data) == "table" then
		local rt = ""
		for k,v in pairs(data) do
			rt = rt .. indent .. k .. "=" .. dump( v, indent .. "  ")
		end
		return rt
	else
		return tostring( data)
	end
end

function PUT( self, inputstr, path)
	if path then
		http_error( "404")
	else
		local config = schema( "config", inputstr, false)
		print ("++++ CONFIG " .. dump(config))
		self:set( "context", strus_Context.new( config.context))
		self:set( "workdir", config.data.workdir or "/srv/strus")
	end
end




