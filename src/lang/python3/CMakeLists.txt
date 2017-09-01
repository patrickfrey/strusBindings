cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# PYTHON
# --------------------------------------
include( ${PROJECT_SOURCE_DIR}/3rdParty/papuga/python3/python.cmake )

configure_file( "${PROJECT_SOURCE_DIR}/src/lang/python3/setup.py.in"  "${CMAKE_CURRENT_BINARY_DIR}/setup.py"  @ONLY )

include_directories(
        "${PYTHON3_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${CMAKE_BINARY_DIR}/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
)

link_directories(
        "${PYTHON3_LIBRARY_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/src"
	"${PROJECT_SOURCE_DIR}/src/bindings"
	"${PROJECT_SOURCE_DIR}/src/lang/python3"
	"${strusbase_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strustrace_LIBRARY_DIRS}"
)

add_custom_command(
    OUTPUT      strus.c
    DEPENDS    ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strusBindingsModuleGen strus_bindings_description
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen python3 module strus.c 
    COMMENT  "Generating Python3 module source strus.c"
    VERBATIM
 )
 
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/lang/python3/strus.py
    DEPENDS   ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strus_bindings_description strusBindingsModuleGen
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen python3 doc ${CMAKE_BINARY_DIR}/src/lang/python3/strus.py
    COMMENT  "Generating bindings interface doc description file ${CMAKE_BINARY_DIR}/src/lang/python3/strus.py"
    VERBATIM
 )
 
add_custom_command(
    OUTPUT setup.stamp
    COMMAND  ${PYTHON3_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/setup.py build
    COMMAND  ${CMAKE_COMMAND} -E  touch setup.stamp
    DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/setup.py
    COMMENT  "Building the Python (v3) module"
    VERBATIM
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/doc/pythonBindingsDoc.htm
    DEPENDS   papugaDoc  "${PROJECT_SOURCE_DIR}/src/lang/python3/strusPythonDoc.tpl"  ${CMAKE_BINARY_DIR}/src/lang/python3/strus.py
    COMMAND  papugaDoc -o ${CMAKE_BINARY_DIR}/doc/pythonBindingsDoc.htm "${PROJECT_SOURCE_DIR}/src/lang/python3/strusPythonDoc.tpl" ${CMAKE_BINARY_DIR}/src/lang/python3/strus.py
    COMMENT  "Generating bindings interface doc html file ${CMAKE_BINARY_DIR}/src/lang/python3/strus.py"
    VERBATIM
)

add_custom_target(
    python3_mod ALL
    DEPENDS strus.c ${CMAKE_CURRENT_BINARY_DIR}/setup.py setup.stamp ${CMAKE_BINARY_DIR}/src/lang/python3/strus.py papuga_python3_gen ${CMAKE_BINARY_DIR}/doc/pythonBindingsDoc.htm
)

# ------------------------------
# INSTALLATION
# ------------------------------
install( CODE "execute_process( COMMAND ${PYTHON3_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/setup.py install)" )


