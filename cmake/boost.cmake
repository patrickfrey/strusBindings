set(Boost_USE_MULTITHREADED ON)
find_package( Boost 1.53.0 COMPONENTS atomic QUIET )
if( Boost_ATOMIC_FOUND )
	find_package( Boost 1.53.0 REQUIRED COMPONENTS thread system date_time atomic )
else()
	find_package( Boost 1.53.0 REQUIRED COMPONENTS thread system date_time )
endif()

# HACK: Because find package for boost_python does not work at least on Ubuntu we have to define the Boost_Python_LIBRARIES (currently debian only):
set( Boost_Python_LIBRARIES boost_python-py27 )


