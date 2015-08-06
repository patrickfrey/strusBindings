#!/bin/sh

cp swig/php/strus_php.so `php-config --extension-dir`/strus.so
cp swig/strus.php /usr/share/php/
service php5-fpm restart
service nginx restart
