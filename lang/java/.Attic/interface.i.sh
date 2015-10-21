#!/bin/bash

for type in Normalizer; do
cat << !EOF | sed "s/#TYPE#/$type/g" > interface.i
%typemap(jni) std::vector<#TYPE#> "jlong"
%typemap(jtype) std::vector<#TYPE#> "#TYPE#[]"
%typemap(jstype) std::vector<#TYPE#> "#TYPE#[]"
%typemap(in) std::vector<#TYPE#> (std::vector<#TYPE#> objar) {
    if (!$input) {
	SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null array");
	return $null;
    }
    const jsize sz = jenv->GetArrayLength($input);
    objar.resize(sz);
    j#TYPE#* const jarr = jenv->Get#TYPE#ArrayElements($input, 0);
    if (!jarr) return $null;
    for ( jsize i = 0; i < sz; i++ )
	objar[i] = jarr[i];
    $1 = &objar;
}
%typemap(out) std::vector<#TYPE#> {
    const jsize sz = $1.size();
    $result = jenv->New#TYPE#Array(sz);
    j#TYPE#* const jarr = jenv->Get#TYPE#ArrayElements($result, 0);
    if (!jarr) return $null;
    for ( jsize i = 0; i < sz; i++ )
	jarr[i] = $1[i];
    jenv->Release#TYPE#ArrayElements($result, jarr, 0);
}
%typemap(javain) std::vector<#TYPE#> "$javainput"
%typemap(javaout) std::vector<#TYPE#> { return $jnicall; }

%apply std::vector<#TYPE#> { std::vector<#TYPE#> const & };
!EOF
done


