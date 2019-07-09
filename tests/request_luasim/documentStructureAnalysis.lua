require "config"

def_server( CSERVER1, config )
call_server( "PUT", CSERVER1 .. "/contentstats/stats", "@contentstats.json" )


