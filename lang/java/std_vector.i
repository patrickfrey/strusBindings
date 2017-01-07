///\brief COPIED FROM ANSWER OF http://stackoverflow.com/questions/10596052/stdvector-to-java-util-vector-code-generation-with-swig
%{
#include <vector>
#include <stdexcept>
%}

%include <stdint.i>
%include <std_except.i>
%include <std_common.i> 

namespace std {
template<class T> class vector {
public:
	typedef size_t size_type;
	typedef T value_type;
	typedef const value_type& const_reference;
	vector();
	vector(size_type n);
	vector(const vector& o);
	size_type capacity() const;
	void reserve(size_type n);
	%rename(isEmpty) empty;
	bool empty() const;
	void clear();
	void push_back(const value_type& x);
	%extend {
		const_reference get(int i) const throw (std::out_of_range) {
			return $self->at(i);
		}
		value_type set(int i, const value_type& VECTOR_VALUE_IN) throw (std::out_of_range) {
			const T old = $self->at(i);
			$self->at(i) = VECTOR_VALUE_IN;
			return old;
		}
		int32_t size() const {
			return $self->size();
		}
		void removeRange(int32_t from, int32_t to) {
			$self->erase($self->begin()+from, $self->begin()+to);
		}
	}
};
}

%define specialize_std_vector(T)
#warning "specialize_std_vector - specialization for type T no longer needed"
%enddef


%{
#include <vector>
%}

%naturalvar std::vector<int>;

%typemap(jni) std::vector<int> "jintArray"
%typemap(jtype) std::vector<int> "int[]"
%typemap(jstype) std::vector<int> "int[]"

%typemap(out) std::vector<int>
%{
   $result = jenv->NewIntArray($1.size());
   jenv->SetIntArrayRegion($result, 0, $1.size(), $1.data());
%}

%typemap(javaout) std::vector<int>
  {
    return $jnicall;
  }

%typemap(jni) const std::vector<int> & "jintArray"
%typemap(jtype) const std::vector<int> & "int[]"
%typemap(jstype) const std::vector<int> & "int[]"

%typemap(in) const std::vector<int> &
%{ if(!$input) {
     SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null std::vector");
     return $null;
    }
    const jsize $1_size = jenv->GetArrayLength($input);
    int * $1_ptr = jenv->GetIntArrayElements($input, NULL);
    std::vector<int> $1_vect( $1_ptr, $1_ptr + $1_size );
    jenv->ReleaseIntArrayElements($input, $1_ptr, JNI_ABORT);
    $1 = &$1_vect;
%}

%typemap(javain) const std::vector<int> & "$javainput"

%typemap(out) const std::vector<int> &
%{
   $result = jenv->NewIntArray($1->size());
   jenv->SetIntArrayRegion($result, 0, $1->size(), &(*$1)[0]);
%}

%typemap(javaout) const std::vector<int> &
  {
    return $jnicall;
  } 

// Java typemaps for autoboxing in return types of generics
%define AUTOBOX(CTYPE, JTYPE)
%typemap(autobox) CTYPE, const CTYPE&, CTYPE& "JTYPE"
%enddef
AUTOBOX(std::string, String)
AUTOBOX(SWIGTYPE, $typemap(jstype,$1_basetype))

%typemap(javabase) std::vector "java.util.AbstractList<$typemap(autobox,$1_basetype::value_type)>"
%typemap(javainterface) std::vector "java.util.RandomAccess"
%typemap(jstype) std::vector get "$typemap(autobox,$1_basetype)"
%typemap(jstype) std::vector set "$typemap(autobox,$1_basetype)"
%typemap(jstype) std::vector &VECTOR_VALUE_IN "$typemap(autobox,$1_basetype)"
%typemap(javacode) std::vector %{
	$javaclassname(java.util.Collection<$typemap(autobox,$1_basetype::value_type)> e) {
		this.reserve(e.size());
		for($typemap(autobox,$1_basetype::value_type) value: e) {
			this.push_back(value);
		}
	}
%}



%naturalvar std::vector<double>;

%typemap(jni) std::vector<double> "jdoubleArray"
%typemap(jtype) std::vector<double> "double[]"
%typemap(jstype) std::vector<double> "double[]"

%typemap(out) std::vector<double>
%{
   $result = jenv->NewDoubleArray($1.size());
   jenv->SetDoubleArrayRegion($result, 0, $1.size(), $1.data());
%}

%typemap(javaout) std::vector<double>
  {
    return $jnicall;
  }

%typemap(jni) const std::vector<double> & "jdoubleArray"
%typemap(jtype) const std::vector<double> & "double[]"
%typemap(jstype) const std::vector<double> & "double[]"

%typemap(in) const std::vector<double> &
%{ if(!$input) {
     SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null std::vector");
     return $null;
    }
    const jsize $1_size = jenv->GetArrayLength($input);
    double * $1_ptr = jenv->GetDoubleArrayElements($input, NULL);
    std::vector<double> $1_vect( $1_ptr, $1_ptr + $1_size );
    jenv->ReleaseDoubleArrayElements($input, $1_ptr, JNI_ABORT);
    $1 = &$1_vect;
%}

%typemap(javain) const std::vector<double> & "$javainput"

%typemap(out) const std::vector<double> &
%{
   $result = jenv->NewDoubleArray($1->size());
   jenv->SetDoubleArrayRegion($result, 0, $1->size(), &(*$1)[0]);
%}

%typemap(javaout) const std::vector<double> &
  {
    return $jnicall;
  } 

// Java typemaps for autoboxing in return types of generics
%define AUTOBOX(CTYPE, JTYPE)
%typemap(autobox) CTYPE, const CTYPE&, CTYPE& "JTYPE"
%enddef
AUTOBOX(std::string, String)
AUTOBOX(SWIGTYPE, $typemap(jstype,$1_basetype))

%typemap(javabase) std::vector "java.util.AbstractList<$typemap(autobox,$1_basetype::value_type)>"
%typemap(javainterface) std::vector "java.util.RandomAccess"
%typemap(jstype) std::vector get "$typemap(autobox,$1_basetype)"
%typemap(jstype) std::vector set "$typemap(autobox,$1_basetype)"
%typemap(jstype) std::vector &VECTOR_VALUE_IN "$typemap(autobox,$1_basetype)"
%typemap(javacode) std::vector %{
	$javaclassname(java.util.Collection<$typemap(autobox,$1_basetype::value_type)> e) {
		this.reserve(e.size());
		for($typemap(autobox,$1_basetype::value_type) value: e) {
			this.push_back(value);
		}
	}
%}
