#!/bin/sh

mkdir -p tmp
# Copy all interface files to process sorted in lexical order to a temporary directory:
cp `find ../strus{,Analyzer}/include/strus -name "*Interface.hpp" | grep -v rrorBufferInterface | sort` tmp/
# Generate the binding interfaces:
src/parser/strusBindingsGen tmp/*.hpp
rm -Rf tmp

