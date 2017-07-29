comment --
file php_h { } phpenv/{project}.h
file php_c { } phpenv/{project}.c
file config_m4 { } phpenv/config.m4
file build_sh { } phpenv/build.sh
file new_php_ini { } phpenv/php.ini
file test_php { } phpenv/test.php

template main END {{ }}
END

template new_php_ini=project $END$ {{ }}
{{php_ini}}
extension={{project}}.so
$END$

template test_php=project $END$ {{ }}
<?php
var_dump(extension_loaded('{{project}}'));

var_dump({{project}}_create());
?>
$END$

template build_sh=project $END$ {{ }}
phpize
./configure
make
sudo make install
php -f test.php -c ./php.ini
$END$

template config_m4=project $END$ {{ }}
dnl Tell PHP about the argument to enable the {{Project}} extension

PHP_ARG_ENABLE({{project}}, Whether to enable the {{Project}} extension, [ --enable-{{project}}   Enable {{project}}])

if test "$PHP_{{PROJECT}}" != "no"; then
    PHP_NEW_EXTENSION({{project}}, {{project}}.c, $ext_shared)
fi
$END$

template php_h=project $END$ {{ }}
#define PHP_{{PROJECT}}_EXTNAME "{{project}}"
#define PHP_{{PROJECT}}_VERSION "{{release}}"

PHP_FUNCTION({{project}}_create);
$END$


template php_c=project $END$ {{ }}
#include "{{project}}.h"

// PHP7 stuff:
typedef struct sigaction sigaction;
typedef struct siginfo_t siginfo_t;
typedef unsigned int uint;
#include <php.h>
#include <zend.h>
#include <zend_API.h>
#include <zend_exceptions.h>

{{zend_php_class_decl_c}}
{{zend_class_entry_decl_c}}

PHP_MINIT_FUNCTION({{project}})
{
    zend_class_entry tmp_ce;
    {{zend_class_entry_init_c}}

    return SUCCESS;
}
$END$

template php_method_impl_c=method $END$ {{ }}
PHP_METHOD({{classname}}, {{methodname}})
{
    papuga_php_CallArgs argstruct;
    papuga_CallResult retstruct;
    char errbuf[ 2048];
    const char* msg;

    int argc = ZEND_NUM_ARGS();
    zval *obj = getThis();
    if (!papuga_php_init_CallArgs( (void*)obj, argc, &argcstruct))
    {
        papuga_php_throw_exception(
                "failed to initialize argument (%d): %s",
                argstruct.erridx, papuga_ErrorCode_tostring( argstruct.errcode));
        return;
    }
    papuga_init_CallResult( &retstruct, errbuf, sizeof(errbuf));
    if (!_{{project}}_bindings_{{classname}}__{{methodname}}( argstruct.self, &retstruct, argstruct.argc, argstruct.argv))
    {
        msg = papuga_CallResult_lastError( &retstruct);
        papuga_php_destroy_CallArgs( &argstruct);
        papuga_destroy_CallResult( &retstruct);
        papuga_php_throw_exception( "error calling method %s::%s: %s", "{{classname}}", "{{methodname}}", msg?msg:"unknown error");
        return;
    }
    papuga_php_destroy_CallArgs( &argstruct);
    papuga_php_move_CallResult( &retstruct);
}
$END$

template zend_class_entry_decl_c=class $END$ {{ }}
zend_class_entry* g_{{classname}}_ce = 0;
$END$

template zend_class_entry_init_c=class $END$ {{ }}
    INIT_CLASS_ENTRY(tmp_ce, "{{classname}}", {{classname}}_methods);
    g_{{classname}}_ce = zend_register_internal_class( &tmp_ce TSRMLS_CC);
$END$

template zend_php_class_decl_c=class $END$ {{ }}
const zend_function_entry {{classname}}_methods[] = {
    {{zend_php_method_decl_c}}
    PHP_FE_END
};
$END$

template zend_php_method_decl_c=method $END$ {{ }}
    PHP_ME( {{classname}}, {{methodname}}, NULL, ZEND_ACC_PUBLIC)
$END$

namespace classname=class
variable methodname=method
variable Project=project
variable project=project    locase
variable PROJECT=project    upcase
variable release
# variable classname=class
# variable param[0]
ignore usage param constructor remark note brief author copyright license


