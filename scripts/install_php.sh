#!/bin/sh

cp swig/php/strus.php /usr/share/php/
service php5-fpm restart
service nginx restart
