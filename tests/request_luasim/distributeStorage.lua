require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()

function storageServerName( storageidx)
	return string.format("isrv%d", storageidx)
end

server = {
	storage = {
			{ name = storageServerName( 1), address = ISERVER1 },
			{ name = storageServerName( 2), address = ISERVER2 },
			{ name = storageServerName( 3), address = ISERVER3 },
		},
	statserver  =	{ name = "ssrv",                address = SSERVER1 },
	distqryeval =	{ name = "qsrv",                address = QSERVER1 }
}

function serverHaveSameName( srvar)
	ai = 1
	while ai < #srvar do
		if srvar[ ai].name ~= srvar[ ai+1].name then
			return false
		end
	end
	return true
end

function storageContextName( index)
	if serverHaveSameName( server.storage) then
		return "sto" .. index
	else
		return "test"
	end
end

function serviceAddress( name, index)
	if name == "storage" or name == "inserter" or name == "qryeval" then
		return server.storage[ index].address .. "/" .. name .. "/" .. storageContextName( index) 

	elseif name == "docanalyzer" or name == "qryanalyzer" then
		return server.storage[ index].address .. "/" .. name .. "/test"

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
		cache_size = "500M",
		max_open_files = 512,
		write_buffer_size = "8K",
		block_size = "4K",
		metadata = {
			{ name = "doclen", type = "UInt16" }
		}
	}
}
function getStorageConfig( storageidx)
	cfg = storageConfig
	cfg.storage.path = string.format("storage/test%d", storageidx)
	cfg.storage.include = {
		analyzer = "test",
		qryeval  = "test"
	}
	return cfg
end

function getInserterConfig( storageidx)
	return {
		inserter = {
			include = {
				analyzer = "test",
				storage  = storageContextName( storageidx)
			}
		}
	}
end

function defStorageServer( storageidx)
	address = server.storage[ storageidx].address
	srvname = storageServerName( storageidx)
	def_test_server( srvname, address)
	if verbose then io.stderr:write( string.format("- Build storage server %s listening on %s\n", srvname, address)) end
end

function buildAnalyzer( storageidx)
	servers = {}
	for _,storage in ipairs(server.storage) do
		if not servers[ storage.name] then
			servers[ storage.name] = true

			call_server_checked( "PUT", serviceAddress( "docanalyzer", storageidx), "@docanalyzer.json" )
			call_server_checked( "PUT", serviceAddress( "qryanalyzer", storageidx), "@qryanalyzer.json" )
			call_server_checked( "PUT", serviceAddress( "qryeval", storageidx),  "@qryeval.json" )
			if verbose then io.stderr:write( string.format("- Created document analyzer, query analyzer and query eval for server %s\n", srvname)) end
		end
	end
end

function buildStorageServer( storageidx)
	srvname = storageServerName( storageidx)
	srvconfig = getStorageConfig( storageidx)

	call_server_checked( "POST", serviceAddress( "storage", storageidx),  srvconfig )
	call_server_checked( "PUT",  serviceAddress( "inserter", storageidx), getInserterConfig( storageidx) )

	if verbose then io.stderr:write( string.format("- Created storage, inserter and query eval for server %s\n", srvname)) end

	TRANSACTION = from_json( call_server_checked( "POST", serviceAddress( "inserter", storageidx) .. "/transaction" )).transaction.link
	if verbose then io.stderr:write( string.format("- Create transaction %s for server %s\n", TRANSACTION, srvname)) end
	local cntdoc = 0

	documents = getDirectoryFiles( SCRIPTPATH .. "/doc/xml", ".xml")
	for k,path in pairs(documents) do
		fullpath = "doc/xml/" .. path
		hs = hashString( fullpath, #server.storage, 1)
		if hs == storageidx then
			cntdoc = cntdoc + 1
			if verbose then io.stderr:write( string.format("- Insert document %s to server %s\n", fullpath, srvname)) end
			call_server_checked( "PUT", TRANSACTION, "@" .. fullpath)
		end
	end
	call_server_checked( "PUT", TRANSACTION)
	if verbose then io.stderr:write( string.format("- Inserted %d documents to server %s\n", cntdoc, srvname)) end
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
	if verbose then io.stderr:write( string.format("- Created statistics server %s\n", srvname)) end
end

function defDistributedQueryEvalServer()
	storages = {}
	for storageidx,storage in ipairs( server.storage) do
		table.insert( storages, serviceAddress( "storage", storageidx))
	end
	distqryevalConfig = {
		distqryeval = {
			include = {
				analyzer = "test"
			},
			statserver = { serviceAddress( "statserver") },
			storage = storages
		}
	}
	def_test_server( server.distqryeval.name, server.distqryeval.address )
	call_server_checked( "PUT", server.distqryeval.address  .. "/qryanalyzer/test", "@qryanalyzer.json" )
	call_server_checked( "PUT", serviceAddress( "distqryeval"), distqryevalConfig )
end

-- Setup all servers:
for storageidx,storage in ipairs( server.storage) do
	defStorageServer( storageidx)
	buildAnalyzer( storageidx)
	buildStorageServer( storageidx)
end
defStatisticsServer()
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
distqryevalObj.qryeval = from_json( call_server_checked( "GET", serviceAddress( "distqryeval") .. "/storage")).list.value
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

checkExpected( statserverDef .. statserverStats .. distqryevalVar .. distqryevalDef .. qryres1 .. qryres2, "@distributeStorage.exp", "distributeStorage.res" )

