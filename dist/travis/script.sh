#!/bin/sh

set -e

OS=$(uname -s)

PROJECT=strusBindings

build_dep_project() {
	prj_cmakeflags=$1
	case $OS in
		Linux)
			mkdir build
			cd build
			cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release \
				-DLIB_INSTALL_DIR=lib -DCMAKE_CXX_FLAGS=-g $prj_cmakeflags \
				..
			make
			sudo make install
			cd ..
			;;
	
		Darwin)
			# forcing brew versions (of gettext) over Mac versions
			export CFLAGS=-I/usr/local
			export CXXFLAGS=-I/usr/local
			export LDFLAGS=-L/usr/local/lib
			mkdir build
			cd build
			cmake \
				-DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release \
				-DCMAKE_CXX_FLAGS='-g -Wno-error=format-nonliteral -Wno-error=format-security' -G Xcode $prj_cmakeflags \
				..
			xcodebuild -configuration Release -target ALL_BUILD
			sudo xcodebuild -configuration Release -target install
			cd ..
			;;
			
		*)
			echo "ERROR: unknown operating system '$OS'."
			;;
	esac
}

build_strus_project() {
	prj_cmakeflags=$1
	case $OS in
		Linux)
			mkdir build
			cd build
			cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release \
				-DLIB_INSTALL_DIR=lib -DCMAKE_CXX_FLAGS=-g $prj_cmakeflags \
				..
			make VERBOSE=1
			make VERBOSE=1 CTEST_OUTPUT_ON_FAILURE=1 test
			sudo make VERBOSE=1 install
			cd ..
			;;

		Darwin)
			# forcing brew versions (of gettext) over Mac versions
			export CFLAGS=-I/usr/local
			export CXXFLAGS=-I/usr/local
			export LDFLAGS=-L/usr/local/lib
			mkdir build
			cd build
			cmake \
				-DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release \
				-DCMAKE_CXX_FLAGS='-g -Wno-error=format-nonliteral -Wno-error=format-security' -G Xcode $prj_cmakeflags \
				..
			xcodebuild -configuration Release -target ALL_BUILD
			xcodebuild -configuration Release -target RUN_TESTS
			sudo xcodebuild -configuration Release -target install
			cd ..
			;;
			
		*)
			echo "ERROR: unknown operating system '$OS'."
			;;
	esac
}

# build pre-requisites
DEPS="strusBase strus strusAnalyzer strusTrace strusModule strusRpc"

if test "x$STRUS_WITH_PATTERN" = "xYES"; then
	# install hyperscan
	git clone https://github.com/intel/hyperscan.git
	cd hyperscan
	git checkout v5.1.1
	build_dep_project "-DBUILD_SHARED_LIBS=1"
	cd ..

	DEPS="${DEPS} strusPattern"
fi
if test "x$STRUS_WITH_VECTOR" = "xYES"; then
	DEPS="${DEPS} strusVector"
fi

GITURL=`git config remote.origin.url`
cd ..
for i in $DEPS; do
	git clone `echo $GITURL | sed "s@/$PROJECT\.@/$i.@g"` $i
	cd $i
	echo "BUILD $i"
	git submodule update --init --recursive
	git submodule foreach --recursive git checkout master
	git submodule foreach --recursive git pull

	build_strus_project ""
	cd ..
done

# build the package itself
cd $PROJECT
echo "BUILD $PROJECT WITH -DWITH_WEBREQUEST=${STRUS_WITH_WEBREQUEST} -DWITH_PHP=${STRUS_WITH_PHP} -DWITH_PYTHON=${STRUS_WITH_PYTHON} -DWITH_LUA=${STRUS_WITH_LUA} -DWITH_STRUS_VECTOR=${STRUS_WITH_VECTOR} -DWITH_STRUS_PATTERN=${STRUS_WITH_PATTERN}"
build_strus_project "-DWITH_WEBREQUEST=${STRUS_WITH_WEBREQUEST} -DWITH_PHP=${STRUS_WITH_PHP} -DWITH_PYTHON=${STRUS_WITH_PYTHON} -DWITH_LUA=${STRUS_WITH_LUA} -DWITH_STRUS_VECTOR=${STRUS_WITH_VECTOR} -DWITH_STRUS_PATTERN=${STRUS_WITH_PATTERN}"
cd ..

