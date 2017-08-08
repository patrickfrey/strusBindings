comment --
file php_c { } {project}.c
file new_php_ini { } php.ini

template main END {{ }}
END

template new_php_ini=project $END$ {{ }}
{{php_ini}}
extension={{project}}.so
$END$

template php_c=project $END$ {{ }}
#define PHP_{{PROJECT}}_EXTNAME "{{project}}"
#define PHP_{{PROJECT}}_VERSION "{{release}}"
#include "papuga/lib/php7_dev.h"
#include "strus/bindingObjects.h"
#include "papuga.h"

// PHP & Zend includes:
typedef void siginfo_t;
#ifdef _MSC_VER
#include <zend_config.w32.h>
#else
#include <zend_config.nw.h>
#endif
#include <php.h>
#include <zend.h>
#include <zend_API.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>

{{zend_class_entry_decl_c}}
static papuga_zend_class_entry* g_class_entry_list[ STRUS_BINDINGS_NOF_CLASSES];
static const papuga_php_ClassEntryMap g_class_entry_map = {
	STRUS_BINDINGS_NOF_CLASSES,
	g_class_entry_list
};
static zend_object* create_zend_object_wrapper( zend_class_entry* ce TSRMLS_DC)
{
	return papuga_php_create_object( ce);
}
{{zend_method_impl_c}}
{{zend_php_class_decl_c}}

PHP_MINIT_FUNCTION({{project}})
{
    zend_class_entry tmp_ce;
    papuga_php_init();
{{zend_class_entry_init_c}}
    return SUCCESS;
}
PHP_MSHUTDOWN_FUNCTION({{project}})
{
    /*[-]*/fprintf( stderr, "CALL PHP_MSHUTDOWN_FUNCTION({{project}})\n");
    /*[-]*/fflush(stderr);
    return SUCCESS;
}
PHP_MINFO_FUNCTION({{project}})
{
    php_info_print_table_start();
    php_info_print_table_row(2, "strus library support", "enabled");
    php_info_print_table_end();
}
const zend_function_entry {{project}}_functions[] = {
    PHP_FE_END
};
zend_module_entry {{project}}_module_entry = {
    STANDARD_MODULE_HEADER,
    "{{project}}",
    {{project}}_functions,
    PHP_MINIT({{project}}),
    PHP_MSHUTDOWN({{project}}),
    NULL/*PHP_RINIT({{project}})*/,
    NULL/*PHP_RSHUTDOWN({{project}})*/,
    PHP_MINFO({{project}}),
    "{{release}}", /* Replace with version number for your extension */
    STANDARD_MODULE_PROPERTIES
};
/* }}} */
ZEND_GET_MODULE({{project}})
$END$

template zend_method_impl_c=constructor $END$ {{ }}
PHP_METHOD({{Project}}{{classname}}, __construct)
{
    papuga_HostObject thisHostObject;
    papuga_php_CallArgs argstruct;
    papuga_ErrorBuffer errbuf;
    void* self;
    char errstr[ 2048];
    const char* msg;
    int argc = ZEND_NUM_ARGS();

    if (!papuga_php_init_CallArgs( NULL/*self*/, argc, &argstruct))
    {
        zend_error( E_ERROR, papuga_ErrorCode_tostring( argstruct.errcode));
        return;
    }
    papuga_init_ErrorBuffer( &errbuf, errstr, sizeof(errstr));
    self = _{{project}}_bindings_constructor__{{classname}}( &errbuf, argstruct.argc, argstruct.argv);
    if (!self)
    {
        msg = papuga_ErrorBuffer_lastError( &errbuf);
        papuga_php_destroy_CallArgs( &argstruct);
        zend_error( E_ERROR, msg);
        return;
    }
    papuga_php_destroy_CallArgs( &argstruct);
    zval *thiszval = getThis();
    papuga_init_HostObject( &thisHostObject, STRUS_BINDINGS_CLASSID_{{classname}}, self, &_{{project}}_bindings_destructor__{{classname}});
    if (!papuga_php_init_object( thiszval, &thisHostObject))
    {
        zend_error( E_ERROR, "object initialization failed");
        return;
    }
}
$END$

template zend_method_impl_c=method $END$ {{ }}
PHP_METHOD({{Project}}{{classname}}, {{methodname}})
{
    papuga_php_CallArgs argstruct;
    papuga_CallResult retstruct;
    papuga_ErrorBuffer errbuf;
    char errstr[ 2048];
    const char* msg;
    int argc = ZEND_NUM_ARGS();

    /*[-]*/fprintf( stderr, "CALL METHOD {{Project}}{{classname}}::{{methodname}}\n");
    zval *obj = getThis();
    if (!papuga_php_init_CallArgs( (void*)obj, argc, &argstruct))
    {
        zend_error( E_ERROR, papuga_ErrorCode_tostring( argstruct.errcode));
        return;
    }
    papuga_init_CallResult( &retstruct, errstr, sizeof(errstr));
    if (!_{{project}}_bindings_{{classname}}__{{methodname}}( argstruct.self, &retstruct, argstruct.argc, argstruct.argv))
    {
        msg = papuga_CallResult_lastError( &retstruct);
        papuga_php_destroy_CallArgs( &argstruct);
        papuga_destroy_CallResult( &retstruct);
        zend_error( E_ERROR, msg);
        return;
    }
    papuga_php_destroy_CallArgs( &argstruct);
    papuga_init_ErrorBuffer( &errbuf, errstr, sizeof(errstr));
    papuga_php_move_CallResult( return_value, &retstruct, &g_class_entry_map, &errbuf);
    if (papuga_ErrorBuffer_hasError( &errbuf))
    {
        zend_error( E_ERROR, errbuf.ptr);
    }
}
$END$

template zend_class_entry_decl_c=class $END$ {{ }}
zend_class_entry* g_{{classname}}_ce = 0;
$END$

template zend_class_entry_init_c=class $END$ {{ }}
    INIT_CLASS_ENTRY(tmp_ce, "{{Project}}{{classname}}", {{classname}}_methods);
    g_{{classname}}_ce = zend_register_internal_class( &tmp_ce TSRMLS_CC);
    g_{{classname}}_ce->create_object = &create_zend_object_wrapper;
    g_class_entry_list[ STRUS_BINDINGS_CLASSID_{{classname}}-1] = g_{{classname}}_ce;
$END$

template zend_php_class_decl_c=class $END$ {{ }}
const zend_function_entry {{classname}}_methods[] = {
{{zend_php_constructor_decl_c}}
{{zend_php_method_decl_c}}
    PHP_FE_END
};
$END$

template zend_php_constructor_decl_c=constructor $END$ {{ }}
    PHP_ME({{Project}}{{classname}},  __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
$END$

template zend_php_method_decl_c=method $END$ {{ }}
    PHP_ME({{Project}}{{classname}}, {{methodname}}, NULL, ZEND_ACC_PUBLIC)
$END$

namespace classname=class
variable methodname=method
namespace Project=project
namespace project=project   locase
namespace PROJECT=project    upcase
variable release
# variable classname=class
# variable param[0]
ignore usage param remark note brief author copyright license


