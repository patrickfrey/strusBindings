%exception {
	try {
		$action
	} catch (const std::bad_alloc& e) {
		jclass clazz = jenv->FindClass("java/lang/OutOfMemoryError");
		jenv->ThrowNew( clazz, "Memory allocation error");
		return $null;
	} catch (const std::exception& e) {
		jclass clazz = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew( clazz, e.what());
		return $null;
	} catch (...) { 
		jclass clazz = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew( clazz, "Unknown exception (program logic error)");
		return $null;
	}
}

