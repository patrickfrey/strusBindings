require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()

def_test_server( "isrv1", ISERVER1)
call_server_checked( "PUT", ISERVER1 .. "/qryanalyzer/test", "@queryAnalysis.json" )

qryanacfg = call_server_checked_det_json( "GET", ISERVER1 .. "/qryanalyzer/test" )
if verbose then io.stderr:write( string.format("- Query analyzer configuration:\n%s\n", qryanacfg)) end

query = {
	query = {
		feature = {
		{	set = "search",
			content = {
				type = "text",
				value = "Iggy Pop"
			}
		}}
	}
}

qryana = call_server_checked_det_json( "GET", ISERVER1 .. "/qryanalyzer/test", query)
if verbose then io.stderr:write( string.format("- Analyzed query with analyzer defined by server:\n%s\n", qryana)) end

checkExpected( qryanacfg .. qryana, "@queryAnalysis.exp", "queryAnalysis.res" )

