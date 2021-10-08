function GET( self, input, path)
	return nil
end

function PUT( self, inputstr, path)
	config = schema( "config", inputstr, false)
	self:set( "context", strus_Context.new( config.context))
	return nil
end

function POST( self, input, path)
	return nil
end



