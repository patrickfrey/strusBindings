%exception {
	try {
		$action
	} catch (const std::bad_alloc& e) {
		PyErr_SetString( PyExc_MemoryError, "Memory allocation error");
		SWIG_fail;
	} catch (const std::runtime_error& e) {
		PyErr_SetString( PyExc_RuntimeError, e.what());
		SWIG_fail;
	} catch (const std::exception& e) {
		PyErr_SetString( PyExc_Exception, e.what());
		SWIG_fail;
	} catch (...) { 
		PyErr_SetString( PyExc_TypeError, "Unknown exception (program logic error)");
		SWIG_fail;
	}
}

