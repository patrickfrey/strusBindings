#!/bin/sh

set -e

OS=$(uname -s)

case $OS in
	Linux)
		sudo apt-get update -qq
		sudo apt-get install -y \
			cmake \
			libleveldb-dev \
			libboost-all-dev
		if test "x$STRUS_WITH_WEBREQUEST" = "xYES"; then
			sudo apt-get install -y libcurl4-openssl-dev
		fi
		if test "x$STRUS_WITH_VECTOR" = "xYES"; then
			sudo apt-get install -y libatlas-dev liblapack-dev libblas-dev libarmadillo-dev
		fi
		if test "x$STRUS_WITH_PATTERN" = "xYES"; then
			sudo apt-get install -y libtre-dev ragel
		fi
		if test "x$STRUS_WITH_PYTHON" = "xYES"; then
			sudo apt-get install -y python3-dev
		fi
		if test "x$STRUS_WITH_PHP" = "xYES"; then
			sudo apt-get install -y libssl-dev
			sudo apt-get install -y language-pack-en-base
			sudo apt-get install -y php7.0 php7.0-dev
		fi
		;;

	Darwin)
		brew update
		brew upgrade cmake
		if test "x$STRUS_WITH_PHP" = "xYES"; then
			brew install openssl php71 || true
		fi
		brew install gettext snappy leveldb curl || true
		# make sure cmake finds the brew version of gettext
		brew link --force gettext || true
		brew link leveldb || true
		brew link snappy || true
		if test "x$STRUS_WITH_VECTOR" = "xYES"; then
			brew install lapack openblas armadillo || true
		fi
		if test "x$STRUS_WITH_PATTERN" = "xYES"; then
			brew install tre ragel
		fi
		;;
	*)
		echo "ERROR: unknown operating system '$OS'."
		;;
esac

