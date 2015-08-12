%typemap(javacode) StrusContext %{
  static {
    System.loadLibrary( "strus_java");
  }
%}

