function GET( self, inputstr, path)
	local statserver = self:get("statserver")
	if inputstr then
		local stat = schema( "statisticsquery", inputstr, true).statisticsquery
		for _,t in ipairs(stat.termstats or {}) do
			t.df = statserver:df( t.type, t.value)
		end
		stat.globalstats = {nofdocs = statserver:nofDocuments()}
		return {statistics=stat}
	else
		return {statserver=statserver:introspection( path)}
	end
end

function PUT( self, inputstr, path)
	if path then
		local statserver = self:get("statserver")
		if path == "sync" then
			storagedefs = self:get( "storagedefs")
			-- ! NEED A STATISTICS STORAGE BASED ON LEVELDB
			local input = schema( "statisticsblob", inputstr, true).statisticsblob
			statserver:processStatisticsMessage( input)
		else
			http_status( "404")
		end
	else
		local input = schema( "statserver", inputstr, true).statserver
		local context = self:get("context")
		local statserver = context:createStatisticsMap( input)
		self:set( "statserver", statserver)
		local storagedefs = {}
		for _,link in ipairs(input.storagelinks) do
			table.insert( storagedefs, {link = link} )
		end
		self:set( "storagedefs", storagedefs)
	end
end

