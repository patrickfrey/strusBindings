-- Dumping table contents to a string
local function dump( data, indent)
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

local function query_terms( query)
	local function collect_terms( termtable, expression)
		if expression.arg then
			for _,arg in ipairs(expression.arg) do
				collect_terms( termtable, arg)
			end
		else
			termtable[ expression.type .. " " .. (expression.value or "")] = {type=expression.type, value=expression.value}
		end
	end
	local termtable = {}
	for _,feature in ipairs(query.feature or {}) do
		collect_terms( termtable, feature.analyzed)
	end
	local rt = {}
	for kk,vv in pairs(termtable) do table.insert( rt, vv) end
	table.sort( rt)
	return rt
end

local function get_delegate_request_result( result, errormsgprefix)
	if result:error() then
		error( errormsgprefix .. ": " .. result:error())
	elseif result:result() then
		return result:result()
	else
		error( errormsgprefix .. ": result empty")
	end
end

function GET( self, inputstr, path)
	local distqryeval = self:get("distqryeval")
	if inputstr then
		local context = self:get("context")
		local querybuilder = context:createQueryBuilder()
		local resultmerger = context:createQueryResultMerger()
		local query = schema( "query", inputstr, true).query
		local analyzer_result = send( "GET", distqryeval.analyzer, {query=query})
		yield()
		query = get_delegate_request_result( analyzer_result, "Failed to analyze query").query
		print ("++++ QUERY " .. dump(query) .. "++++")
		local statisticsquery = {statisticsquery={termstats=query_terms( query)}}
		local statistics_result = send( "GET", distqryeval.statserver, statisticsquery)
		yield()
		local statistics = get_delegate_request_result( statistics_result, "Failed to get term statistics").statistics
		query.termstats = statistics.termstats
		query.globalstats = statistics.globalstats
		query.mergeres = true
		local collector_results = {}
		for _,collector in ipairs(distqryeval.collector or {}) do
			table.insert( collector_results, send( "GET", collector, {query=query}))
		end
		yield()
		local summary_table = {}
		for _,collector_result in ipairs(collector_results or {}) do
			local queryresult = get_delegate_request_result( collector_result, "Collector query failed").queryresult
			for _,rank in ipairs(queryresult.ranks or {}) do
				querybuilder:addCollectSummary( rank.summary or {})
			end
		end
		local qryeval_results = {}
		for _,qryeval in ipairs(distqryeval.qryeval or {}) do
			table.insert( qryeval_results, send( "GET", qryeval, {query=query}))
		end
		yield()
		local resultlist = {}
		for _,qryeval_result in ipairs(qryeval_results or {}) do
			table.insert( resultlist, get_delegate_request_result( qryeval_results, "Query failed").queryresult)
		end
		resultmerger:addQueryResult( resultlist)
		local queryresult = resultmerger:evaluate()
		return {queryresult=queryresult}
	else
		return {distqryeval=distqryeval}
	end
end

function PUT( self, inputstr, path)
	if path then
		http_status( "404")
	else
		local distqryeval = schema( "distqryeval", inputstr, true).distqryeval
		self:set( "distqryeval", distqryeval)
	end
end

