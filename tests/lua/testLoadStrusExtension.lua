-- require "strus" 
-- ... Loading strus with the Lua module "strus.lua" is only needed when running the test with the official Lua interpreter and not with strusLua

local ctxconfig = { trace = { log = "dump", file = "stdout" } }
local ctx = strus_Context.new( ctxconfig )


