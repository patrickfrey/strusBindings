comment --
file php_h { } phpenv/{project}.h
file php_c { } phpenv/{project}.c

template main END {{ }}
END

template php_h=project $END$ {{ }}
#define PHP_{{PROJECT}}_EXTNAME "{{project}}"
#define PHP_{{PROJECT}}_VERSION "{{release}}"

PHP_FUNCTION({{project}}_create);
$END$


template php_c=project $END$ {{ }}
#include <php.h>
#include "{{project}}.h"

zend_function_entry {{project}}_functions[] = {
            PHP_FE(hello_world, NULL)
        PHP_FE_END
};

zend_module_entry {{project}}_module_entry = {
            STANDARD_MODULE_HEADER,
        PHP_{{PROJECT}}_EXTNAME,
        {{project}}_functions,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        PHP_{{PROJECT}}_VERSION,
        STANDARD_MODULE_PROPERTIES,
};

ZEND_GET_MODULE({{project}});

PHP_FUNCTION(hello_world) {
            php_printf("Hello world\
");
};
$END$
variable Project=project
variable project=project    locase
variable PROJECT=project    upcase
variable release
# variable classname=class
# variable param[0]
ignore usage param class method constructor remark note brief author copyright license


