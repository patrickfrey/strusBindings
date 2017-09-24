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
		sudo LC_ALL=en_US.UTF-8 apt-get install -y php7.1 php7.1-dev
		;;

	Darwin)
		brew update
		if test "X$CC" = "Xgcc"; then
			brew install gcc48 --enable-all-languages || true
			brew link --force gcc48 || true
		fi
		brew tap homebrew/dupes
		brew tap homebrew/versions
		brew tap homebrew/homebrew-php
		brew install \
			cmake \
			boost \
			gettext \
			snappy 
			leveldb \
			lapack \
			blas \
			python3 \
			php71 \
			|| true
		# make sure cmake finds the brew version of gettext
		brew link --force gettext || true
		brew link leveldb || true
		brew link snappy || true
		# rebuild leveldb to use gcc-4.8 ABI on OSX (libstc++ differs
		# from stdc++, leveldb uses std::string in API functions, C
		# libraries like gettext and snappy and even boost do not 
		# have this problem)
		if test "X$CC" = "Xgcc"; then
			brew reinstall leveldb --cc=gcc-4.8
		fi
		;;
	
	*)
		echo "ERROR: unknown operating system '$OS'."
		;;
esac

