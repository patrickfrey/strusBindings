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
%include "std_vector.i"

%typemap(javapackage) std::vector<std::string>  "net.strus.api.StringVector"
%template(StringVector) std::vector<std::string>;

%typemap(javapackage) std::vector<Term>  "net.strus.api.TermVector"
%template(TermVector) std::vector<Term>;

%typemap(javapackage) std::vector<QueryTerm>  "net.strus.api.QueryTermVector"
%template(QueryTermVector) std::vector<QueryTerm>;

%typemap(javapackage) std::vector<Term>  "net.strus.api.DocumentFrequencyChangeVector"
%template(DocumentFrequencyChangeVector) std::vector<DocumentFrequencyChange>;

%typemap(javapackage) std::vector<Rank>  "net.strus.api.RankVector"
%template(RankVector) std::vector<Rank>;

%typemap(javapackage) std::vector<SummaryElement>  "net.strus.api.SummaryElementVector"
%template(SummaryElementVector) std::vector<SummaryElement>;

%typemap(javapackage) std::vector<Attribute>  "net.strus.api.AttributeVector"
%template(AttributeVector) std::vector<Attribute>;

%typemap(javapackage) std::vector<MetaData>  "net.strus.api.MetaDataVector"
%template(MetaDataVector) std::vector<MetaData>;

%typemap(javapackage) std::vector<Normalizer>  "net.strus.api.NormalizerVector"
%template(NormalizerVector) std::vector<Normalizer>;

%typemap(javapackage) std::vector<VecRank>  "net.strus.api.VecRankVector"
%template(VecRankVector) std::vector<VecRank>;

%rename(assign_operator) operator=;

%typemap(javacode) Context %{
static {
	System.loadLibrary( "strus_java");
}
%}

%include "../../include/strus/bindingObjects.hpp"



