function GET( self, input, path)
	contentstats = self:get("context")
	return {context={context:introspection( path)}}
end

function PUT( self, inputstr, path)
	config = schema( "config", inputstr, false)
	self:set( "context", strus_Context.new( config.context))
end




