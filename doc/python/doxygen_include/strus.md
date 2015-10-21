strus Python bindings	 {#mainpage}
=====================

The strus Python bindings provide a Python interface for accessing the 
retrieval storage, indexing documents and queries and evaluating queries.
To use the strus Python interface you have to load the module 'strus'.

The entry point of a strus application with Python is the context object (strus.context).
It is the root object from which all other objects are created.
It can be constructed either as proxy, that redirects all method calls to an RpcServer
or it can be constructed as instance running in the Python environment.

The following examples illustrate the usage of the Python bindings.




