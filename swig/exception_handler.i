#if defined(SWIGPHP)
%exception {
	try {
		$action
	} catch (const std::runtime_error& e) {
		zend_throw_exception( NULL, const_cast<char*>(e.what()), 0 TSRMLS_CC);
	} catch (const std::logic_error& e) {
		zend_throw_exception( NULL, const_cast<char*>(e.what()), 0 TSRMLS_CC);
	} catch (const std::bad_alloc& e) {
		zend_throw_exception( NULL, const_cast<char*>(e.what()), 0 TSRMLS_CC);
	} catch (...) { 
		zend_throw_exception( NULL, const_cast<char*>("unknown exception"), 0 TSRMLS_CC);
	}
}
#else
#error no exception handling defined for this language
#endif

