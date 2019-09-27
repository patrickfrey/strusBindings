require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()
nofStorages = 3

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

function buildStorageServer( serveridx, serveraddr)
	if verbose then io.stderr:write( string.format("- Build storage server %d listening on %s\n", serveridx, serveraddr)) end
	config.service.name = string.format("isrv%d", serveridx)

	def_server( serveraddr, config )
	call_server_checked( "PUT", serveraddr  .. "/docanalyzer/test", "@docanalyzer.json" )
	call_server_checked( "PUT", serveraddr  .. "/qryanalyzer/test", "@qryanalyzer.json" )
	if verbose then io.stderr:write( string.format("- Created document and query analyzer for server %d\n", serveridx)) end

	storageConfig.storage.path = string.format("storage/test%d", serveridx)
	call_server_checked( "POST", serveraddr .. "/storage/test",  storageConfig )
	call_server_checked( "PUT",  serveraddr .. "/inserter/test", "@inserter.json" )
	call_server_checked( "PUT", serveraddr .. "/qryeval/test",  "@qryeval.json" )

	if verbose then io.stderr:write( string.format("- Created storage, inserter and query eval for server %d\n", serveridx)) end

	TRANSACTION = from_json( call_server_checked( "POST", serveraddr .. "/inserter/test/transaction" )).link
	if verbose then io.stderr:write( string.format("- Create transaction %s for server %d\n", TRANSACTION, serveridx)) end
	local cntdoc = 0

	documents = getDirectoryFiles( SCRIPTPATH .. "/doc/xml", ".xml")
	for k,path in pairs(documents) do
		fullpath = "doc/xml/" .. path
		hs = hashString( fullpath, nofStorages, 1)
		if hs == serveridx then
			cntdoc = cntdoc + 1
			if verbose then io.stderr:write( string.format("- Insert document %s to server %d\n", fullpath, serveridx)) end
			call_server_checked( "PUT", TRANSACTION, "@" .. fullpath)
		end
	end
	call_server_checked( "PUT", TRANSACTION)
	if verbose then io.stderr:write( string.format("- Inserted %d documents to server %d\n", cntdoc, serveridx)) end
end

buildStorageServer( 1, ISERVER1)
buildStorageServer( 2, ISERVER2)
buildStorageServer( 3, ISERVER3)

function storageAddress( serverurl)
	return serverurl .. "/storage/test"
end
function statserverAddress( serverurl)
	return serverurl .. "/statserver/test"
end

-- Define a statistics server for the 3 storage servers defined:
statserverConfig = {
	statserver = {
		proc = "std",
		blocks = "100K",
		storage = {
			storageAddress(ISERVER1),
			storageAddress(ISERVER2),
			storageAddress(ISERVER3)
		}
	}
}

config.service.name = "ssrv1"
def_server( SSERVER1, config)
call_server_checked( "PUT", SSERVER1  .. "/statserver/test", statserverConfig )

statserverDef = call_server_checked( "GET", SSERVER1 .. "/statserver/test")
if verbose then io.stderr:write( string.format("- Statistics server configuration from server:\n%s\n", statserverDef)) end

-- Define a 2nd variant of the statistics server for testing delegate calls issued by embedded sub configuration:
SSERVER2=ISERVER4
config_SSERVER2 = mergeValues(
			statserverConfig,
			config
		)
config_SSERVER2.service.name = "ssrv2"
config_SSERVER2.statserver.id = "test"

if verbose then io.stderr:write( string.format("- Merged statistics server configuration:\n%s\n", to_json(config_SSERVER2))) end
def_server( SSERVER2, config_SSERVER2)

statserverDef_configured = call_server_checked( "GET", SSERVER2 .. "/statserver/test")
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
		storage = {
			storageAddress(ISERVER1),
			storageAddress(ISERVER2),
			storageAddress(ISERVER3)
		}
	}
}

config.service.name = "qsrv1"
def_server( QSERVER1, config)
call_server_checked( "PUT", QSERVER1  .. "/qryanalyzer/test", "@qryanalyzer.json" )
call_server_checked( "PUT", QSERVER1  .. "/distqryeval/test", distqryevalConfig )

distqryevalObj = {}
distqryevalObj.statserver = from_json( call_server_checked( "GET", QSERVER1 .. "/distqryeval/test/statserver")).list.value
distqryevalObj.storage = from_json( call_server_checked( "GET", QSERVER1 .. "/distqryeval/test/storage")).list.value
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
if verbose then io.stderr:write( string.format("- Query evaluation result:\n%s\n", qryres1)) end
-- qryres2 = det_qeval_result( call_server_checked( "GET", QSERVER1 .. "/distqryeval/test", query2))
-- if verbose then io.stderr:write( string.format("- Query evaluation result with analysis passed:\n%s\n", qryres2)) end

if verbose then io.stderr:write( string.format("- Distributed query evaluation:\n%s\n", qryres1)) end
-- if verbose then io.stderr:write( string.format("- Distributed query evaluation with analysis passed:\n%s\n", qryres2)) end


checkExpected( statserverDef .. statserverDef_configured .. statserverStats .. distqryevalVar .. distqryevalDef, "@distributeStorage.exp", "distributeStorage.res" )

