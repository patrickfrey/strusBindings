function GET( self, input, path)
	return nil
end

function PUT( self, input, path)
	schema( "config", input, false)
	return nil
end

function POST( self, input, path)
	return nil
end



