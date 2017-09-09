#!/bin/sh

# Generate the binding interfaces:
src/parser/strusBindingsInterfaceGen src/impl/context.hpp src/impl/storage.hpp src/impl/statistics.hpp src/impl/vector.hpp src/impl/analyzer.hpp src/impl/query.hpp

