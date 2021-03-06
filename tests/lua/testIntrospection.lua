-- require "strus" 
-- ... Loading strus with the Lua module "strus.lua" is only needed when running the test with the official Lua interpreter and not with strusLua
require "utils"
local datadir = arg[1] or "../data/mdprim/"
local outputdir = arg[2] or '.'

local ctxconfig = { trace = { log = "dump", file = "stdout" } }
local ctx = strus_Context.new( ctxconfig )

ctx:addModulePath( "/usr/local/strus/lib/modules" )
ctx:addModulePath( "/usr/lib/modules" )
ctx:addResourcePath( { "/usr/share/resources", "/usr/share/strus" })
ctx:defineWorkingDirectory( "/home/johndoe" )
ctx:endConfig()

local outmap = {}
outmap[ 'struct_env' ] = ctx:introspection("env")
outmap[ 'textproc_segmenter' ] = ctx:introspection( { "textproc", "segmenter" } )

local result = "introspection:" .. dumpTree( outmap) .. "\n"
local expected = [[
introspection:
string struct_env:
  string module: 
  string moduledir:
    number 1: "/usr/local/strus/lib/modules"
    number 2: "/usr/lib/modules"
  string resourcedir:
    number 1: "/usr/share/resources"
    number 2: "/usr/share/strus"
  string workdir: "/home/johndoe"
string textproc_segmenter:
  string link:
    number 1: "cjson"
    number 2: "plain"
    number 3: "textwolf"
    number 4: "tsv"
]]

verifyTestOutput( outputdir, result, expected)


