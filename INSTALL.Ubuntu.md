Ubuntu 16.04 on x86_64, i686
----------------------------

# Build system
Cmake with gcc or clang. Here in this description we build with 
gcc >= 4.9 (has C++11 support). StrusAll can also be built with C++98.

# Prerequisites
Install packages with 'apt-get'/aptitude.

## StrusAll has the following CMake flags
	-DWITH_PHP=YES
	to enable build with Php 7 language bindings.
	-DWITH_PYTHON=YES
	to enable build with Python 3 language bindings.
	-DWITH_STRUS_VECTOR=YES
	to build with a module for vector search (e.g. word2vec).
	-DWITH_STRUS_PATTERN=YES
	to build with a module for fast pattern matching based on hyperscan.

The prerequisites are listen in 5 sections, a common section (first) and for
each of these flags toggled to YES another section.

## Common packages needed always
	boost-all >= 1.53 (>= 1.57, if -DWITH_STRUS_VECTOR=YES)
	snappy-dev leveldb-dev libuv-dev

## Packages needed with -DWITH_STRUS_PATTERN=YES
	libtre-dev

## Packages needed with -DWITH_STRUS_VECTOR=YES
	ragel atlas-dev lapack-dev blas-dev boost-all >= 1.57

## Packages needed with -DWITH_STRUS_PYTHON=YES
	python3-dev

## Packages needed with -DWITH_STRUS_PHP=YES
	php7.0-dev zlib1g-dev libxml2-dev

# Strus prerequisite packages to install before
	strusBase strus strusAnalyzer strusTrace strusModule strusRpc  

# Strus prerequisite packages to install before with -DWITH_STRUS_VECTOR=YES
	strusVector

# Strus prerequisite packages to install before with -DWITH_STRUS_VECTOR=YES
	strusPattern

# Configure build and install strus prerequisite packages with GNU C/C++
	With strusVector and strusPattern enabled:
	$ for strusprj in strusBase strus strusAnalyzer strusTrace \
		strusModule strusRpc strusVector strusPattern
	$ do
	$ git clone https://github.com/patrickfrey/$strusprj $strusprj
	$ cd $strusprj
	$ cmake -DCMAKE_BUILD_TYPE=Release -DLIB_INSTALL_DIR=/usr/local/lib .
	$ make
	$ make install
	$ done

# Configure build and install strus prerequisite packages with Clang C/C++
	Minimal build, only Lua bindings without Vector and Pattern and
	a reasonable default for library installation directory:
	$ for strusprj in strusBase strus strusAnalyzer strusTrace \
		strusModule strusRpc
	$ do
	$ git clone https://github.com/patrickfrey/$strusprj $strusprj
	$ cd $strusprj
	$ cmake -DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_C_COMPILER="clang" -DCMAKE_CXX_COMPILER="clang++" .
	$ make
	$ make install
	$ done

# Configure with GNU C/C++
	With all bindings and strusVector and strusPattern enabled:
	$ cmake -DCMAKE_BUILD_TYPE=Release \
		-DWITH_PYTHON=YES \
		-DWITH_PHP=YES \
		-DWITH_STRUS_VECTOR=YES \
		-DWITH_STRUS_PATTERN=YES \
		-DLIB_INSTALL_DIR=/usr/local/lib .

# Configure with Clang C/C++
	Minimal build, only Lua bindings without Vector and Pattern and
	a reasonable default for library installation directory:

	$ cmake -DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_C_COMPILER="clang" -DCMAKE_CXX_COMPILER="clang++" .

# Build
	$ make

# Run tests
	$ make test

# Install
	$ make install

