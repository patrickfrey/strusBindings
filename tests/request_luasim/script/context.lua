function GET( self, input, path)
	local contentstats = self:get("context")
	return {context={context:introspection( path)}}
end

function PUT( self, inputstr, path)
	if path then
		http_error( "404")
	else
		local config = schema( "config", inputstr, false)
		self:set( "context", strus_Context.new( config.context))
		self:set( "workdir", config.data.workdir or "/srv/strus")
	end
end




