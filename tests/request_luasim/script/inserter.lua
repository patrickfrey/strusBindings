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
		local input = schema( "inserter", inputstr, true).inserter
		self:inherit( "docanalyzer", input.analyzer)
		self:inherit( "storage", input.storage)
		local context = self:get("context")
		local inserter = context:createInserter( self:get("storage"), self:get("docanalyzer"))
		self:set( "inserter", inserter)
	end
end

function POST( self, inputstr, path, objname)
	if objname then
		if path == "transaction" then
			local inserter = self:get("inserter")
			local tid = transaction( "insertertransaction", inserter:createTransaction())
			return "transaction/" .. tid
		end

	else
		if not path then
			objname = counter("inserter")
			PUT( self, inputstr, path, objname)
			return "inserter/" .. objname
		end
	end
	http_status( "404")
end

