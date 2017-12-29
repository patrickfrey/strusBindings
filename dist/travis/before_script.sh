#!/bin/sh

set -e

OS=$(uname -s)

case $OS in
	Linux)
		sudo apt-get update -qq
		sudo apt-get install -y \
			cmake \
			libboost-all-dev \
			libleveldb-dev \
			libatlas-dev \
			liblapack-dev \
			libblas-dev \
			python3-dev \
			php7.0-dev
		;;

	Darwin)
		brew update
		brew upgrade cmake
		brew upgrade boost
		brew install gettext snappy leveldb || true
		brew link --force gettext || true
		brew link leveldb || true
		brew link snappy || true
		brew install lapack blas || true
		brew tap homebrew/versions
		brew tap homebrew/homebrew-php
		brew install php70 python3 || true
		# make sure cmake finds the brew version of gettext
		;;

	*)
		echo "ERROR: unknown operating system '$OS'."
		;;
esac

