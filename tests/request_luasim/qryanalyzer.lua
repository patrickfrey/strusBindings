require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()

def_test_server( "isrv1", ISERVER1)
call_server_checked( "PUT", ISERVER1 .. "/qryanalyzer/test", "@qryanalyzer.json" )

qryanacfg = call_server_checked( "GET", ISERVER1 .. "/qryanalyzer/test" )
if verbose then io.stderr:write( string.format("- Query analyzer configuration:\n%s\n", qryanacfg)) end

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
qryanacfg2 = {
	element = {
		{
			type = "word",
			field = "text",
			tokenizer = {
				name = "word"
			},
			normalizer = {{
				name = "uc"
			}}
		}
	}
}
query_with_analyzer = {
	query = mergeValues( query, {query = {analyzer = qryanacfg2}} )
}

qryana1 = call_server_checked( "GET", ISERVER1 .. "/qryanalyzer/test", query)
if verbose then io.stderr:write( string.format("- Analyzed query with analyzer defined by server:\n%s\n", qryana1)) end
qryana2 = call_server_checked( "GET", ISERVER1 .. "/qryanalyzer/test", query_with_analyzer)
if verbose then io.stderr:write( string.format("- Analyzed query with analyzer passed as content:\n%s\n", qryana2)) end

checkExpected( qryanacfg .. qryana1 .. qryana2, "@qryanalyzer.exp", "qryanalyzer.res" )


