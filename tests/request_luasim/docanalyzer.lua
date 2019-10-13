require "config"
require "testUtils"
require "io"
require "os"

SCRIPTPATH = script_path()

def_test_server( "isrv1", ISERVER1)

call_server_checked( "PUT", ISERVER1 .. "/docanalyzer/test", "@docanalyzer.json" )
docanaconf = call_server_checked( "GET", ISERVER1 .. "/docanalyzer/test" )
if verbose then io.stderr:write( string.format("- Document analyzer configuration from the server:\n%s\n", docanaconf)) end

testdoc = "doc/xml/David_Bowie.xml"
docana = call_server_checked( "GET", ISERVER1 .. "/docanalyzer/test", "@" .. testdoc )
if verbose then io.stderr:write( string.format("- Document '%s' analyzed:\n%s\n", testdoc, docana)) end

checkExpected( docanaconf .. docana, "@docanalyzer.exp", "docanalyzer.res" )

