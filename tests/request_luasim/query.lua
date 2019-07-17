require "config"
require "testUtils"
require "io"
require "os"
local json = require 'lunajson'

SCRIPTPATH = script_path()

storageConfig = {
	storage = {
		path = undef,
		database = "leveldb",
		cache_size = "500M",
		max_open_files = 512,
		write_buffer_size = "8K",
		block_size = "4K",
		metadata = [
			{ name = "doclen", type = "UInt16" }
		]
	}
}

def_server( ISERVER1, config )
call_server_checked( "PUT", ISERVER1 .. "/docanalyzer/test", "@docanalyzer.json" )

storageConfig.storage.path = "storage/test"
call_server_checked( "PUT", ISERVER1 .. "/storage/test", storageConfig )

call_server_checked( "PUT", ISERVER1 .. "/inserter/test", "@inserter.json" )
if verbose then io.stderr:write( string.format("- Created, storage, analyzer and inserter\n")) end

TRANSACTION = json.decode( call_server_checked( "POST", ISERVER1 .. "/inserter/test/transaction" )).link
if verbose then io.stderr:write( string.format("- Create transaction %s\n", TRANSACTION)) end

documents = getDirectoryFiles( SCRIPTPATH .. "/doc/xml", ".xml")
for k,path in pairs(documents) do
	fullpath = "doc/xml/" .. path
	if verbose then io.stderr:write( string.format("- Insert document %s\n", fullpath)) end
	call_server_checked( "PUT", TRANSACTION, "@" .. fullpath)
end
call_server_checked( "PUT", TRANSACTION)
if verbose then io.stderr:write( string.format("- All documents inserted\n")) end

