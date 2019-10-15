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
	statserver  =	{ name = "ssrv1",               address = SSERVER1 },
	distqryeval =	{ name = "qsrv1",               address = QSERVER1 }
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
		return server.statserver1.address .. "/" .. name .. "/test"

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

nofStorages = 0
function defStorageServer( storageidx, serveraddr)
	if storageidx > nofStorages then nofStorages = storageidx end
	srvname = storageServerName( storageidx)
	def_test_server( srvname, serveraddr)
	if verbose then io.stderr:write( string.format("- Build storage server %s listening on %s\n", srvname, serveraddr)) end
end

function buildStorageServer( storageidx)
	srvname = storageServerName( storageidx)
	srvconfig = getStorageConfig( storageidx)
	srvaddress = serverAddress( srvname)

	call_server_checked( "PUT", serviceAddress( "docanalyzer", storageidx), "@docanalyzer.json" )
	call_server_checked( "PUT", serviceAddress( "qryanalyzer", storageidx), "@qryanalyzer.json" )
	if verbose then io.stderr:write( string.format("- Created document and query analyzer for server %s\n", srvname)) end

	call_server_checked( "POST", serviceAddress( "storage", storageidx),  srvconfig )
	call_server_checked( "PUT",  serviceAddress( "inserter", storageidx), getInserterConfig( storageidx) )
	call_server_checked( "PUT", serviceAddress( "qryeval", storageidx),  "@qryeval.json" )

	if verbose then io.stderr:write( string.format("- Created storage, inserter and query eval for server %s\n", srvname)) end

	TRANSACTION = from_json( call_server_checked( "POST", serviceAddress( "inserter", storageidx) .. "/transaction" )).transaction.link
	if verbose then io.stderr:write( string.format("- Create transaction %s for server %s\n", TRANSACTION, srvname)) end
	local cntdoc = 0

	documents = getDirectoryFiles( SCRIPTPATH .. "/doc/xml", ".xml")
	for k,path in pairs(documents) do
		fullpath = "doc/xml/" .. path
		hs = hashString( fullpath, nofStorages, 1)
		if hs == storageidx then
			cntdoc = cntdoc + 1
			if verbose then io.stderr:write( string.format("- Insert document %s to server %s\n", fullpath, srvname)) end
			call_server_checked( "PUT", TRANSACTION, "@" .. fullpath)
		end
	end
	call_server_checked( "PUT", TRANSACTION)
	if verbose then io.stderr:write( string.format("- Inserted %d documents to server %s\n", cntdoc, srvname)) end
end

defStorageServer( 1, ISERVER1 )
defStorageServer( 2, ISERVER2 )
defStorageServer( 3, ISERVER3 )
buildStorageServer( 1)
buildStorageServer( 2)
buildStorageServer( 3)

function storageAddress( serverurl)
	return serverurl .. "/storage/test"
end
function qryevalAddress( serverurl)
	return serverurl .. "/qryeval/test"
end
function statserverAddress( serverurl)
	return serverurl .. "/statserver/test"
end

-- Define a statistics server for the 3 storage servers defined:
statserverConfig = {
	statserver = {
		id = "test",
		proc = "std",
		blocks = "100K",
		storage = {
			storageAddress( serverAddress( storageServerName( 1))),
			storageAddress( serverAddress( storageServerName( 2))),
			storageAddress( serverAddress( storageServerName( 3)))
		}
	}
}

def_test_server( "ssrv1", SSERVER1)
call_server_checked( "PUT", SSERVER1  .. "/statserver/test", statserverConfig )

statserverDef = call_server_checked( "GET", SSERVER1 .. "/statserver/test")
if verbose then io.stderr:write( string.format("- Statistics server configuration from server:\n%s\n", statserverDef)) end

-- Define a 2nd variant of the statistics server for testing delegate calls issued by embedded sub configuration:
SSERVER2=ISERVER4
def_test_server( "ssrv2", SSERVER2, statserverConfig)

statserverDef_embedded = call_server_checked( "GET", SSERVER2 .. "/statserver/test")
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

statserverStats = call_server_checked( "GET", SSERVER1 .. "/statserver/test", query_analyzed)
if verbose then io.stderr:write( string.format("- Statistics server query result:\n%s\n", statserverStats)) end

-- Define queryeval server for distributed query:
distqryevalConfig = {
	distqryeval = {
		include = {
			analyzer = "test"
		},
		statserver = {
			statserverAddress(SSERVER1)
		},
		qryeval = {
			qryevalAddress(ISERVER1),
			qryevalAddress(ISERVER2),
			qryevalAddress(ISERVER3)
		}
	}
}

def_test_server( "qsrv1", QSERVER1)
call_server_checked( "PUT", QSERVER1  .. "/qryanalyzer/test", "@qryanalyzer.json" )
call_server_checked( "PUT", QSERVER1  .. "/distqryeval/test", distqryevalConfig )

distqryevalObj = {}
distqryevalObj.statserver = from_json( call_server_checked( "GET", QSERVER1 .. "/distqryeval/test/statserver")).list.value
distqryevalObj.qryeval = from_json( call_server_checked( "GET", QSERVER1 .. "/distqryeval/test/qryeval")).list.value
distqryevalDef = to_json( {distqryeval = distqryevalObj} )
distqryevalVar = call_server_checked( "GET", QSERVER1 .. "/distqryeval/test")
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

qryres1 = det_qeval_result( call_server_checked( "GET", QSERVER1 .. "/distqryeval/test", query))
if verbose then io.stderr:write( string.format("- Distributed query evaluation result:\n%s\n", qryres1)) end
qryres2 = det_qeval_result( call_server_checked( "GET", QSERVER1 .. "/distqryeval/test", query2))
if verbose then io.stderr:write( string.format("- Distributed query evaluation result with analysis passed:\n%s\n", qryres2)) end

checkExpected( statserverDef .. statserverDef_embedded .. statserverStats .. distqryevalVar .. distqryevalDef .. qryres1 .. qryres2, "@distributeStorage.exp", "distributeStorage.res" )

