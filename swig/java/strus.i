%module strus
%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"
%include "exception.i"
%include "arrays_java.i"

%apply const std::string& {std::string*};

%{
#include "../include/strus/bindingObjects.hpp"
%}

%include "java_heap.i"
%include "ignore_declarations.i"
%include "exception_handler.i"

%typemap(javapackage) std::vector<std::string>  "net.strus.api.StringVector"
%template(StringVector) std::vector<std::string>;

%typemap(javapackage) std::vector<Term>  "net.strus.api.TermVector"
%template(TermVector) std::vector<Term>;

%typemap(javapackage) std::vector<Rank>  "net.strus.api.RankVector"
%template(RankVector) std::vector<Rank>;

%typemap(javapackage) std::vector<RankAttribute>  "net.strus.api.RankAttributeVector"
%template(RankAttributeVector) std::vector<RankAttribute>;

%typemap(javapackage) std::vector<Attribute>  "net.strus.api.AttributeVector"
%template(AttributeVector) std::vector<Attribute>;

%typemap(javapackage) std::vector<MetaData>  "net.strus.api.MetaDataVector"
%template(MetaDataVector) std::vector<MetaData>;

%typemap(javapackage) std::vector<Normalizer>  "net.strus.api.NormalizerVector"
%template(NormalizerVector) std::vector<Normalizer>;

%typemap(javacode) StrusContext %{
static {
	System.loadLibrary( "strus_java");
}
%}

%include "../../include/strus/bindingObjects.hpp"



