#!/bin/sh

set -e

OS=$(uname -s)

case $OS in
	Linux)
		sudo apt-get update -qq
		sudo apt-get install -y \
			cmake \
			libleveldb-dev \
			libatlas-dev \
			liblapack-dev \
			libblas-dev
		# Boost 1.58
		sudo add-apt-repository -y ppa:kojoley/boost
		sudo apt-get -q update
		sudo apt-get install libboost-atomic1.58-dev libboost-thread1.58-dev libboost-system1.58-dev libboost-filesystem1.58-dev libboost-regex1.58-dev

		if test "x$PAPUGA_WITH_PYTHON" = "xYES"; then
			sudo apt-get install -y python3-dev
		fi
		if test "x$PAPUGA_WITH_PHP" = "xYES"; then
			sudo apt-get install -y libssl-dev
			sudo apt-get install -y language-pack-en-base
			sudo locale-gen en_US.UTF-8
			sudo LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8 add-apt-repository -y ppa:ondrej/php
			sudo LC_ALL=en_US.UTF-8 apt-get update
			sudo LC_ALL=en_US.UTF-8 apt-get install -y php7.0 php7.0-dev
		fi
		;;

	Darwin)
		brew update
		brew upgrade cmake
		# -- brew upgrade boost
		if test "x$PAPUGA_WITH_PHP" = "xYES"; then
			brew install openssl php71 || true
		fi
		brew install gettext snappy leveldb || true
		# make sure cmake finds the brew version of gettext
		brew link --force gettext || true
		brew link leveldb || true
		brew link snappy || true
		brew install lapack openblas || true
		;;

	*)
		echo "ERROR: unknown operating system '$OS'."
		;;
esac

