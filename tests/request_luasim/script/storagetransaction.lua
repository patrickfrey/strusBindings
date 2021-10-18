function GET( self, inputstr, path)
	local transaction = self:get("self")
end

function PUT( self, inputstr, path)
	local transaction = self:get("self")
	if path then
		local input = schema( "document", inputstr, true).document
		transaction:insertDocument( path, input, schema);
	else
		transaction:commit()
		http_status( 201)
	end
end

function DELETE( self, inputstr, path)
	local transaction = self:get("self")
	if path then
		transaction:deleteDocument( path)
	else
		transaction:rollback()
	end
end
