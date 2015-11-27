set(Boost_USE_MULTITHREADED ON)
find_package( Boost 1.53.0 COMPONENTS atomic QUIET )
if( Boost_ATOMIC_FOUND )
	find_package( Boost 1.53.0 REQUIRED COMPONENTS thread system date_time atomic python )
else()
	find_package( Boost 1.53.0 REQUIRED COMPONENTS thread system date_time python )
endif()


