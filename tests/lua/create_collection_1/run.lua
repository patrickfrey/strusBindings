package.path = "../common/utils.lua"
require "utils"
package.path = "../common/testCollection.lua"
require "testCollection"
package.path = "../common/dumpCollection.lua"
require "dumpCollection"

ctx = strus_Context.new()
ctx:loadModule( "analyzer_pattern")

createCollection( ctx, arg[1] .. "/storage", arg[1], {"A.xml","B.xml","C.xml"})

dumpCollection( ctx, arg[1] .. "/storage", {"A.xml","B.xml","C.xml"})
print "done"


