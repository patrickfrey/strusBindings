#!/bin/sh

cp swig/php/strus.so `php-config --extension-dir`/
cp swig/strus.php /usr/share/php/
service php5-fpm restart

