%module strus
%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"
%include "exception.i"

%exception {
	try {
		$action
	} catch (const std::runtime_error& e) {
		SWIG_exception( SWIG_RuntimeError, e.what());
		goto fail;
	} catch (const std::logic_error& e) {
		SWIG_exception( SWIG_ValueError, e.what());
		goto fail;
	} catch (const std::bad_alloc& e) {
		SWIG_exception( SWIG_MemoryError, e.what());
		goto fail;
	}
}

%apply const std::string& {std::string*};

%{
#include "../include/strus/bindingObjects.hpp"
%}

%ignore Reference;
%ignore Variant::defined() const;
%ignore Variant::type() const;
%ignore Variant::getUInt() const;
%ignore Variant::getInt() const;
%ignore Variant::getFloat() const;
%ignore Variant::getText() const;
#if defined(SWIGOCAML)
%ignore Variant::Variant( unsigned int v);
%ignore Variant::Variant( double v);
%ignore Variant::assign( unsigned int v);
%ignore Variant::assign( double v);
#endif
%ignore Reference::Reference( Reference::Deleter deleter_);
%ignore Reference::Reference( const Reference& o);
%ignore Reference::~Reference();
%ignore Reference::reset( void* obj_=0);
%ignore Reference::get() const;
%ignore Reference::get();
%ignore MetaData::name() const;
%ignore MetaData::value() const;
%ignore Attribute::name() const;
%ignore Attribute::value() const;
%ignore FunctionDef::name() const;
%ignore FunctionDef::arguments() const;

%include "std_vector_string.i"
%include "std_vector_term.i"
%include "std_vector_rank.i"
%include "strus_variant.i"
%include "strus_tokenizer.i"
%include "strus_normalizer.i"
%include "../include/strus/bindingObjects.hpp"




