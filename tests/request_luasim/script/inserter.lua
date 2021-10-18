function GET( self, inputstr, path)
	inserter = self:get("inserter")
	if inputstr then
		http_status( "404")
	else
		return {inserter=inserter:introspection( path)}
	end
end

function PUT( self, inputstr, path)
	if path then
		http_status( "404")
	else
		local input = schema( "qryeval", inputstr, true).qryeval
		self:inherit( "analyzer", input.analyzer)
		self:inherit( "storage", input.storage)
		local context = self:get("context")
		local storage = self:get("storage")
		local analyzer = self:get("analyzer")
		local inserter = context:createInserter( storage, analyzer)
		self:set( "inserter", inserter)
	end
end

function POST( self, inputstr, path, objname)
	if objname then -- storage addressed then create transaction
		local inserter = self:get("inserter")
		local tid = transaction( "insertertransaction", inserter:createTransaction())
		return "transaction/" .. tid

	else -- no no storage addressed then create storage
		objname = counter("inserter")
		PUT( self, inputstr, path)
		return "inserter/" .. objname
	end
end
