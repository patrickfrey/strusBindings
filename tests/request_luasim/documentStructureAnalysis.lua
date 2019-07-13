require "config"
require "testUtils"
require "io"
local json = require 'lunajson'

SCRIPTPATH = script_path()

def_server( CSERVER1, config )
call_server_checked( "PUT", CSERVER1 .. "/contentstats/stats", "@contentstats.json" )
set_time( 1)

TRANSACTION = json.decode( call_server_checked( "POST", CSERVER1 .. "/contentstats/stats/transaction" )).link
io.stderr:write( string.format("- Create transaction %s\n", TRANSACTION))

documents = getDirectoryFiles( SCRIPTPATH .. "/doc/xml", ".xml")
for k,path in pairs(documents) do
	fullpath = "doc/xml/" .. path
	io.stderr:write( string.format("- Insert document %s\n", fullpath))
	call_server_checked( "PUT", TRANSACTION, "@" .. fullpath)
end

