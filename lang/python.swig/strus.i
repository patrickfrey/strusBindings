%module strus
%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"
%include "exception.i"

%{
#include "../include/strus/bindingObjects.hpp"
%}

%include "ignore_declarations.i"
%include "exception_handler.i"
%include "strus_std_string.i"
%include "std_vector_string.i"
%include "std_vector_int.i"
%include "std_vector_term.i"
%include "std_vector_rank.i"
%include "strus_variant.i"
%include "strus_tokenizer.i"
%include "strus_normalizer.i"
%include "strus_aggregator.i"
%include "strus_summarizer.i"
%include "strus_weighting.i"
%include "../../include/strus/bindingObjects.hpp"


