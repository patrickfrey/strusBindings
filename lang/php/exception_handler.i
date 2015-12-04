%inline %{
#include "objInitializers.hpp"
/*[-]*/#include <iostream>
%}

%exception {
	try {
		$action
	} catch (const std::runtime_error& e) {
		SWIG_exception( SWIG_RuntimeError, e.what());
		return;
	} catch (const std::logic_error& e) {
		SWIG_exception( SWIG_RuntimeError, e.what());
		return;
	} catch (const std::bad_alloc& e) {
		SWIG_exception( SWIG_MemoryError, e.what());
		return;
	} catch (...) { 
		SWIG_exception( SWIG_RuntimeError, "unknown exception");
		return;
	}
}

