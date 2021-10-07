function GET( self, input, path)
	return nil
end

function dump( data, indent)
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
	config = schema( "config", inputstr, false)
	self:set( "context", strus_Context.new( config.context))
	return nil
end

function POST( self, input, path)
	return nil
end



