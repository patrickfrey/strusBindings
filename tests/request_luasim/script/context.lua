function GET( self, input, path)
	local contentstats = self:get("context")
	return {context={context:introspection( path)}}
end

function PUT( self, inputstr, path)
	if path then
		http_status( "404")
	else
		local config = schema( "config", inputstr, false)
		local context = strus_Context.new( config.context)
		for _,module in ipairs( config.extensions.modules or {}) do
			context:loadModule( module)
		end
		if config.extensions.directory then
			context:addModulePath( config.extensions.directory)
		end
		if config.data.resources then
			context:addResourcePath( config.data.resources)
		end
		if config.data.workdir then
			context:defineWorkingDirectory( config.data.workdir)
		end
		self:set( "context", context)
	end
end
