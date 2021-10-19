function GET( self, inputstr, path)
	local transaction = self:get("self")
end

function PUT( self, inputstr, path)
	local transaction = self:get("self")
	if path then
		local input = schema( "document", inputstr, true).document
		transaction:insertDocument( path, input, schema);
	elseif inputstr then
		local root = docroot( inputstr)
		print("+++ ROOT " .. root)
		if root == "metadataUpdate" then
			local input = schema( "metadataUpdate", inputstr, true).metadataUpdate
			transaction:updateMetaDataTable( input)
		else
			http_status( 400)
		end
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
