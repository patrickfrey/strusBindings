require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()

storageConfig = {
	storage = {
		database = "leveldb",
		cache = "500M",
		max_open_files = 512,
		write_buffer_size = "8K",
		block_size = "4K"
	}
}
resultBuffer = ""

function postStorage()
	local storageLink = from_json( call_server_checked( "POST", ISERVER1 .. "/storage", storageConfig )).storage.link
	if verbose then io.stderr:write( string.format("- Created storage and got link %s\n", storageLink)) end
	resultBuffer = resultBuffer .. string.format("NEW LINK %s\n", storageLink)
end

function getStorageList( title)
	local storageList = from_json( call_server_checked( "GET", ISERVER1 .. "/storage", "" )).list.link
	if verbose then io.stderr:write( string.format("- Storage list [%s] %s\n", title, to_json(storageList, false))) end
	resultBuffer = resultBuffer .. string.format("LINKLIST %s: %s\n", title, to_json(storageList, false))
end

function deleteStorage( index)
	call_server_checked( "DELETE", ISERVER1 .. string.format("/storage/%d", index), "" )
end

def_test_server( "isrv", ISERVER1)
postStorage()
postStorage()
postStorage()
getStorageList( "after insert 1,2,3")

deleteStorage( 2)
getStorageList( "after delete 2")

postStorage()
getStorageList( "after reinsert 2")

checkExpected( resultBuffer, "@createStorage.exp", "createStorage.res" )

