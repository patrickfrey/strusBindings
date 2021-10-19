require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()

if #argv == 0 then
	resultFile = "distributeStorage.res"
	expectedFile = "distributeStorage.exp"
else
	resultFile = "distributeStorage." .. argv[ 1] .. ".res"
	expectedFile = "distributeStorage." .. argv[ 1] .. ".exp"
end

if argv[ 1] == "1server" then
server = {
	storage = {
			{ name = "srv", address = ISERVER1, context="test1" },
			{ name = "srv", address = ISERVER1, context="test2" },
			{ name = "srv", address = ISERVER1, context="test3" },
		},
	statserver  =	{ name = "srv", address = ISERVER1, context="test" },
	qryanalyzer =	{ name = "srv", address = ISERVER1, context="test" },
	distqryeval =	{ name = "srv", address = ISERVER1, context="test" }
}
else
server = {
	storage = {
			{ name = "isrv1", address = ISERVER1, context="test1" },
			{ name = "isrv2", address = ISERVER2, context="test2" },
			{ name = "isrv3", address = ISERVER3, context="test3" },
		},
	statserver  =	{ name = "ssrv",  address = SSERVER1, context="test" },
	qryanalyzer =	{ name = "vsrv",  address = VSERVER1, context="test" },
	distqryeval =	{ name = "qsrv",  address = QSERVER1, context="test" }
}
end

function serviceAddress( name, index)
	if name == "storage" or name == "inserter" then
		return server.storage[ index].address .. "/" .. name .. "/" .. server.storage[ index].context

	elseif name == "docanalyzer" then
		return server.storage[ index].address .. "/" .. name .. "/test"

	elseif name == "qryeval" then
		return server.storage[ index].address .. "/qryeval/" .. server.storage[ index].context

	elseif name == "collector" then
		return server.storage[ index].address .. "/qryeval/collector_" .. server.storage[ index].context

	elseif name == "sentanalyzer" then
		return server.vstorage.address .. "/qryanalyzer/" .. name

	elseif name == "qryanalyzer" then
		return server.qryanalyzer.address .. "/" .. name .. "/test"

	elseif name == "statserver" then
		return server.statserver.address .. "/" .. name .. "/test"

	elseif name == "distqryeval" then
		return server.distqryeval.address .. "/" .. name .. "/test"

	else
		error( "unknown service " .. name )
	end
end

storageConfig = {
	storage = {
		database = "leveldb",
		statsproc = "std",
		cache = "500M",
		max_open_files = 512,
		write_buffer_size = "8K",
		block_size = "4K"
	}
}
metadataConfig = {
	storage = {
		metadata = {
			{op="add", name="doclen", type="UINT16"}
		}
	}
}

function getStorageConfig( storageidx)
	return storageConfig
end

function getQueryEvalConfig( storageidx, content)
	cfg = from_json( content)
	cfg.qryeval.include = {
		storage = server.storage[ storageidx].context
	}
	return cfg
end

function getInserterConfig( storageidx)
	return {
		inserter = {
			include = {
				analyzer = "test",
				storage  = server.storage[ storageidx].context
			}
		}
	}
end

function defStorageServer( storageidx)
	for storageidx,storage in ipairs( server.storage) do
		def_test_server( storage.name, storage.address)
		if verbose then io.stderr:write( string.format("- Build storage server %s listening on %s\n", storage.name, storage.address)) end
	end
end

function buildDocumentAnalyzer()
	servers = {}
	for storageidx,storage in ipairs(server.storage) do
		if not servers[ storage.name] then
			servers[ storage.name] = true

			call_server_checked( "PUT", serviceAddress( "docanalyzer", storageidx), "@docanalyzer.json" )
			if verbose then io.stderr:write( string.format("- Created document analyzer for server %s\n", storage.name)) end
		end
	end
end

function buildQueryEval()
	servers = {}
	for storageidx,storage in ipairs(server.storage) do
		call_server_checked( "PUT", serviceAddress( "collector", storageidx), getQueryEvalConfig( storageidx, "@qryeval_collector.json") )
		call_server_checked( "PUT", serviceAddress( "qryeval", storageidx), getQueryEvalConfig( storageidx, "@qryeval.json") )
		if verbose then io.stderr:write( string.format("- Created query evaluations for server %s\n", storage.name)) end
	end
end

function buildStorageServer()
	for storageidx,storage in ipairs( server.storage) do
		call_server_checked( "POST", serviceAddress( "storage", storageidx),  getStorageConfig( storageidx))
		call_server_checked( "PUT",  serviceAddress( "inserter", storageidx), getInserterConfig( storageidx))
		if verbose then io.stderr:write( string.format("- Created storage, inserter and query eval for server %s\n", storage.name)) end

		TRANSACTION = from_json( call_server_checked( "POST", serviceAddress( "storage", storageidx) .. "/transaction" )).transaction.link
		if verbose then io.stderr:write( string.format("- Create transaction for meta data definitions %s\n", TRANSACTION)) end
		call_server_checked( "PUT", TRANSACTION, metadataConfig)
		call_server_checked( "PUT", TRANSACTION)
		if verbose then io.stderr:write( string.format("- Defined meta data schema\n")) end

		TRANSACTION = from_json( call_server_checked( "POST", serviceAddress( "inserter", storageidx) .. "/transaction" )).transaction.link
		if verbose then io.stderr:write( string.format("- Create transaction %s for server %s\n", TRANSACTION, storage.name)) end
		local cntdoc = 0

		documents = getDirectoryFiles( SCRIPTPATH .. "/doc/xml", ".xml")
		for k,path in pairs(documents) do
			fullpath = "doc/xml/" .. path
			hs = hashString( fullpath, #server.storage, 1)
			if hs == storageidx then
				cntdoc = cntdoc + 1
				if verbose then io.stderr:write( string.format("- Insert document %s to server %s\n", fullpath, storage.name)) end
				call_server_checked( "PUT", TRANSACTION, "@" .. fullpath)
			end
		end
		call_server_checked( "PUT", TRANSACTION)
		if verbose then io.stderr:write( string.format("- Inserted %d documents to server %s\n", cntdoc, storage.name)) end
	end
end

function defStatisticsServer()
	storages = {}
	for storageidx,storage in ipairs( server.storage) do
		table.insert( storages, serviceAddress( "storage", storageidx))
	end
	statserverConfig = {
		statserver = {
			id = "test",
			proc = "std",
			blocks = "100K",
			storage = storages
		}
	}
	def_test_server( server.statserver.name, server.statserver.address )
	call_server_checked( "PUT", serviceAddress( "statserver"), statserverConfig )
	if verbose then io.stderr:write( string.format("- Created statistics server %s\n", server.statserver.name)) end
end

function defQueryAnalyzeServer()
	def_test_server( server.qryanalyzer.name, server.qryanalyzer.address )
	call_server_checked( "PUT", server.qryanalyzer.address  .. "/qryanalyzer/test", "@qryanalyzer.json" )
	if verbose then io.stderr:write( string.format("- Created query analyze server %s\n", server.qryanalyzer.name)) end
end

function defDistributedQueryEvalServer()
	qryevals = {}
	collectors = {}
	for storageidx,storage in ipairs( server.storage) do
		table.insert( qryevals, serviceAddress( "qryeval", storageidx))
		table.insert( collectors, serviceAddress( "collector", storageidx))
	end
	distqryevalConfig = {
		distqryeval = {
			analyzer = { serviceAddress( "qryanalyzer") },
			statserver = { serviceAddress( "statserver") },
			collector = collectors,
			qryeval = qryevals,
			config = {separator = "#"}
		}
	}
	def_test_server( server.distqryeval.name, server.distqryeval.address )
	call_server_checked( "PUT", serviceAddress( "distqryeval"), distqryevalConfig )
end

-- Setup all servers:
defStorageServer()
buildDocumentAnalyzer()
buildStorageServer()
buildQueryEval()
defStatisticsServer()
defQueryAnalyzeServer()
defDistributedQueryEvalServer()


-- Get statistics server introspection:
statserverDef = call_server_checked( "GET", serviceAddress( "statserver"))
if verbose then io.stderr:write( string.format("- Statistics server configuration from server:\n%s\n", statserverDef)) end

-- Call the statistics server for statistics of an analyzed query:
query_analyzed = {
query = {
	feature = {
		{
			content = {
				{type = "text", value = "Iggy Pop"}
			},
			analyzed = {
				{
					term = { type = "word", value = "iggy"}
				},{
					term = { type = "word", value = "pop"}
				},{
					term = { type = "entity", value = "iggy_pop"}
				}
			},
			set = "search"
		}
	}
}}
statserverStats = call_server_checked( "GET", serviceAddress( "statserver"), query_analyzed)
if verbose then io.stderr:write( string.format("- Statistics server query result:\n%s\n", statserverStats)) end

-- Get distributed queryeval server introspection:
distqryevalObj = {}
distqryevalObj.statserver = from_json( call_server_checked( "GET", serviceAddress( "distqryeval") .. "/statserver")).list.value
distqryevalObj.collector = from_json( call_server_checked( "GET", serviceAddress( "distqryeval") .. "/collector")).list.value
distqryevalObj.qryeval = from_json( call_server_checked( "GET", serviceAddress( "distqryeval") .. "/qryeval")).list.value
distqryevalDef = to_json( {distqryeval = distqryevalObj} )
distqryevalVar = call_server_checked( "GET", serviceAddress( "distqryeval") )
if verbose then io.stderr:write( string.format("- Distributed query evaluation object names from server:\n%s\n", distqryevalVar)) end
if verbose then io.stderr:write( string.format("- Distributed query evaluation server configuration from server:\n%s\n", distqryevalDef)) end

-- Evaluate the same queries as in the 'query.lua' test with the distributed query evaluation:
query = {
	query = {
		feature = {
		{	set = "search",
			content = {
				term = {
					type = "text",
					value = "Iggy Pop"
				}
			}
		}}
	}
}

query2 = {
	query = {
		feature = {
		{	set = "search",
			content = {
				term = {
					type = "text",
					value = "Iggy Pop"
				}
			},
			analyzed = {
				term = {
					type = "word",
					value = "songwriter"
				}
			}
		}}
	}
}

qryres1 = det_qeval_result( call_server_checked( "GET", serviceAddress( "distqryeval"), query))
if verbose then io.stderr:write( string.format("- Distributed query evaluation result:\n%s\n", qryres1)) end
qryres2 = det_qeval_result( call_server_checked( "GET", serviceAddress( "distqryeval"), query2))
if verbose then io.stderr:write( string.format("- Distributed query evaluation result with analysis passed:\n%s\n", qryres2)) end

checkExpected( statserverDef .. statserverStats .. distqryevalVar .. distqryevalDef .. qryres1 .. qryres2, "@" .. expectedFile, resultFile )

