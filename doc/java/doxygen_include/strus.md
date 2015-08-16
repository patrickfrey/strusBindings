strus Java bindings	 {#mainpage}
===================

The strus Java bindings provide a Java interface for accessing the 
retrieval storage, indexing documents and queries and evaluating queries.

The entry point of a strus application with Java is the 
context (@ref net::strus::api::StrusContext) object.
It is the root object from which all other objects are created.
It can be constructed either as proxy, that redirects all method calls to an RpcServer
or it can be constructed as instance running in the Java/JNI environment.




