function PUT( self, inputstr, path)
	local transaction = self:get("self")
	if inputstr then
		local schema = { mimetype = mimetype( inputstr), encoding = encoding( inputstr) }
		transaction:insertDocument( path, inputstr, schema);
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

