require "config"
require "testUtils"
require "io"
require "os"

-- Define a query analyzer server with embedded anlyzer configuration:
qryanalyzerConfig = from_json( load_file( "qryanalyzer.json") )
qryanalyzerConfig.qryanalyzer.id = "test"
def_test_server( "qsrv", ISERVER1, qryanalyzerConfig)

qryanalyzerDef = call_server_checked( "GET", ISERVER1 .. "/qryanalyzer/test")
if verbose then io.stderr:write( string.format("- Embedded query analyze server properties:\n%s\n", qryanalyzerDef)) end

checkExpected( qryanalyzerDef, "@embeddedConfig.exp", "embeddedConfig.res" )
