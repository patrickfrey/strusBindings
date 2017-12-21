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
			python3-dev
		sudo apt-get install -y language-pack-en-base
		sudo locale-gen en_US.UTF-8
		sudo LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8 add-apt-repository -y ppa:ondrej/php
		sudo LC_ALL=en_US.UTF-8 apt-get update
		sudo LC_ALL=en_US.UTF-8 apt-get install -y php7.0 php7.0-dev
		;;

	Darwin)
		brew update
		brew tap homebrew/dupes
		brew tap homebrew/versions
		brew upgrade cmake
		brew upgrade boost
		brew tap homebrew/homebrew-php
		brew install gettext snappy leveldb || true
		brew install lapack blas python3 php70 || true
		# make sure cmake finds the brew version of gettext
		brew link --force gettext || true
		brew link leveldb || true
		brew link snappy || true
		;;
	
	*)
		echo "ERROR: unknown operating system '$OS'."
		;;
esac

