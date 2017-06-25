require "string"
require "utils"
require "config_mdprim"
require "createCollection"
require "dumpCollection"

local datadir = arg[1]
local outputdir = arg[2] or '.'
local storage = outputdir .. "/storage"
local docfiles = {"doc.xml"}

ctx = strus_Context.new( {trace={log="dump",file="stdout"}})
ctx:loadModule( "analyzer_pattern")

createCollection( ctx, storage, metadata_mdprim(), createDocumentAnalyzer_mdprim( ctx), true, datadir, docfiles)
local result = "collection dump:" .. dumpTree( "", dumpCollection( ctx, storage)) .. "\n"
local expected = [[
]]

verifyTestOutput( outputdir, result, expected)


