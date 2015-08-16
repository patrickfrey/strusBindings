strus PHP bindings	 {#mainpage}
==================

The strus PHP bindings provide a PHP interface for accessing the 
retrieval storage, indexing documents and queries and evaluating queries.

The entry point of a strus application with PHP is the 
context (@ref StrusContext) object.
It is the root object from which all other objects are created.
It can be constructed either as proxy, that redirects all method calls to an RpcServer
or it can be constructed as instance running in the PHP environment.
Because it is most likely that PHP will run in a webserver context, it is recommended
to use the RPC proxy variant there.




