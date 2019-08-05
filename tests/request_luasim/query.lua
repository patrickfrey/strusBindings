require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()

storageConfig = {
	storage = {
		database = "leveldb",
		cache_size = "500M",
		max_open_files = 512,
		write_buffer_size = "8K",
		block_size = "4K",
		metadata = {
			{ name = "doclen", type = "UInt16" }
		}
	}
}

def_server( ISERVER1, config )
call_server_checked( "PUT", ISERVER1 .. "/docanalyzer/test", "@docanalyzer.json" )
call_server_checked( "PUT", ISERVER1 .. "/qryanalyzer/test", "@qryanalyzer.json" )
if verbose then io.stderr:write( string.format("- Created document and query analyzer\n")) end

storageConfig.storage.path = "storage/test"
call_server_checked( "POST", ISERVER1 .. "/storage/test", storageConfig )
call_server_checked( "PUT", ISERVER1 .. "/inserter/test", "@inserter.json" )
if verbose then io.stderr:write( string.format("- Created storage and inserter\n")) end

TRANSACTION = from_json( call_server_checked( "POST", ISERVER1 .. "/inserter/test/transaction" )).link
if verbose then io.stderr:write( string.format("- Create transaction %s\n", TRANSACTION)) end

documents = getDirectoryFiles( SCRIPTPATH .. "/doc/xml", ".xml")
for k,path in pairs(documents) do
	fullpath = "doc/xml/" .. path
	if verbose then io.stderr:write( string.format("- Insert document %s\n", fullpath)) end
	call_server_checked( "PUT", TRANSACTION, "@" .. fullpath)
end
call_server_checked( "PUT", TRANSACTION)
if verbose then io.stderr:write( string.format("- Inserted all documents\n")) end

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

call_server_checked( "POST", ISERVER1 .. "/qryeval/test", "@qryeval.json" )
qryevalconf = call_server_checked( "GET", ISERVER1 .. "/qryeval/test")
if verbose then io.stderr:write( string.format("- Query evaluation configuration from the server:\n%s\n", qryevalconf)) end

qryres = det_qeval_result( call_server_checked( "GET", ISERVER1 .. "/qryeval/test", query))
if verbose then io.stderr:write( string.format("- Query evaluation result:\n%s\n", qryres)) end

qryana = call_server_checked( "GET", ISERVER1 .. "/qryanalyzer/test", query )
if verbose then io.stderr:write( string.format("- Query analysis:\n%s\n", qryana)) end

qryana_with_eval = {
	query = mergeValues(
			from_json( qryana)["query"],
			{eval = from_json( load_file( "qryeval.json"))["qryeval"]}
		)
}
if verbose then io.stderr:write( string.format("- Analyzed query with eval:\n%s\n", to_json(qryana_with_eval))) end
write_file( "test.json", to_json(qryana_with_eval))

qryres2 = det_qeval_result( call_server_checked( "GET", ISERVER1 .. "/storage/test", to_json(qryana_with_eval)))
if verbose then io.stderr:write( string.format("- Query evaluation result from storage with query evaluation config passed:\n%s\n", qryres2)) end

checkExpected( qryevalconf .. qryres .. qryres2, "@query.exp", "query.res" )




