require "os"

print( string.format( "LD_LIBRARY_PATH=%s", os.getenv( 'LD_LIBRARY_PATH')))
print( string.format( "STRUS_MODULE_PATH=%s", os.getenv( 'STRUS_MODULE_PATH')))


