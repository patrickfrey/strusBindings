#!/bin/sh

clean_up () {
	echo "shut down";
}

trap clean_up HUP INT QUIT KILL TERM

case "$1" in
	"start")
		echo "Starting nginx"
		service php5-fpm start
		service nginx start
	;;
	*)
		echo "Usage: docker run start" 
		exit
	;;
esac

