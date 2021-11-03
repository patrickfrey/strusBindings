-- Dumping table contents to a string
local function dump( data, indent)
	local indent = indent or "\n"
	if type( data) == "nil" then
		return "nil"
	elseif type( data) == "table" then
		local rt = ""
		for k,v in pairs(data) do
			rt = rt .. indent .. k .. "=" .. dump( v, indent .. "  ")
		end
		return rt
	else
		return tostring( data)
	end
end

function GET( self, inputstr, path)
	local statstorage = self:get("statstorage")
	if inputstr then
		local stat = schema( "statisticsquery", inputstr, true).statisticsquery
		for _,t in ipairs(stat.termstats or {}) do
			t.df = statserver:documentFrequency( t.type, t.value)
		end
		stat.globalstats = {nofdocs = statserver:nofDocuments()}
		return {statistics=stat}
	else
		return {statserver=statserver:introspection( path)}
	end
end

local function statserverConfiguration( self, inputstr, objname)
	local config = schema( "statserver", inputstr, true).statserver
	config.path = "statstorage/" .. objname
	self:set( "config", config)
	return config
end

local function updateStats( statstorage, links)
	local post_requests = {}
	for _,storagelink in ipairs( links or {}) do
		local timestamp = statstorage:storageTimeStamp( storagelink)
		local query = timestamp >= 0 and {statfetch = {timestamp=timestamp}} or {statfetch={}}
		post_requests[ storagelink] = send( "POST", storagelink .. "/statfetch", query )
	end
	yield()
	local transactions = {}
	for storagelink,request in pairs( post_requests or {}) do
		if request.error then
			log( "ERROR", "update stats", "server " .. storagelink .. ": " .. request.error)
		else
			transactions[ storagelink] = {link=request.result}
		end
	end
	local index = 0
	while next(transactions) do
		for storagelink,transaction in pairs( transactions) do
			transaction.data = send( "GET", transaction.link .. "/" .. index)
		end
		yield()
		local finished = {}
		for storagelink,transaction in pairs( transactions) do
			if transaction.data.error then
				log( "ERROR", "update stats", "server " .. storagelink .. " [" .. index .. "]: " .. request.error)
				table.insert( finished, storagelink)
			elseif transaction.data.result then
				local msg = schema( "statisticsblob", transaction.data.result, true).statisticsblob
				statstorage:putStatisticsMessage( msg)
			else
				table.insert( finished, storagelink)
			end
		end
		for _,si in ipairs(finished) do
			transactions[ si] = nil
		end
		index = index + 1
	end
end

function INIT( self, inputstr, path, objname)
	if path then
		http_status( "404")
	else
		local config = statserverConfiguration( self, inputstr, objname)
		local context = self:get("context")
		local statstorage = context:createStatisticsStorageClient( config )
		self:set( "statstorage", statstorage)

		local links = statstorage:storageList()
		updateStats( statstorage, links)
	end
end

function PUT( self, inputstr, path, objname)
	if path then
		local statstorage = self:get("statstorage")
		if path == "storagelink" then
			local storagelinks = schema( "storagelink", inputstr, true).storagelink.link
			for _,storageid in ipairs(storagelinks) do
				statstorage:defineStorage( storageid)
			end
			updateStats( statstorage, storagelinks)
		else
			http_status( "404")
		end
	else
		local config = statserverConfiguration( self, inputstr, objname)
		local context = self:get("context")
		local storagelinks = config.storagelinks
		config.storagelinks = nil
		context:createStatisticsStorage( config )
		local statstorage = context:createStatisticsStorageClient( config )
		self:set( "statstorage", statstorage)

		for _,storageid in ipairs(storagelinks) do
			statstorage:defineStorage( storageid)
		end
		updateStats( statstorage, storagelinks)
	end
end

function POST( self, inputstr, path, objname)
	if not objname then
		if not path then
			objname = counter("statstorage")
			PUT( self, inputstr, path, objname)
			return "statstorage/" .. objname
		end
	end
	http_status( "404")
end

function DELETE( self, inputstr, path, objname)
	if path or not objname then
		http_status( "404")
	else
		local config = self:get("config")
		local address = {database=config.database, path=config.path}
		local context = self:get("context")
		local statstorage = self:get("statstorage")
		statstorage:close()
		context:destroyStorage( address)
	end
end

