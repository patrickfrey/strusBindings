require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()

def_test_server( "csrv1", CSERVER1)
call_server_checked( "PUT", CSERVER1 .. "/contentstats/stats", "@contentstats.json" )

print( "TRANSACTION " .. call_server_checked( "POST", CSERVER1 .. "/contentstats/stats/transaction" ))
TRANSACTION = from_json( call_server_checked( "POST", CSERVER1 .. "/contentstats/stats/transaction" )).transaction.link
if verbose then io.stderr:write( string.format("- Create transaction %s\n", TRANSACTION)) end

documents = getDirectoryFiles( SCRIPTPATH .. "/doc/xml", ".xml")
for k,path in pairs(documents) do
	fullpath = "doc/xml/" .. path
	if verbose then io.stderr:write( string.format("- Insert document %s\n", fullpath)) end
	call_server_checked( "PUT", TRANSACTION, "@" .. fullpath)
end

OUTPUT = call_server_checked( "GET", TRANSACTION )
if verbose then io.stderr:write( string.format("- Inspect document analysis:\n%s\n", OUTPUT)) end

checkExpected( OUTPUT, "@documentStructureAnalysis.exp", "documentStructureAnalysis.res" )
