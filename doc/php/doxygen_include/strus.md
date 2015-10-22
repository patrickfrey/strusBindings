strus PHP bindings	 {#mainpage}
==================

The strus PHP interface provides objects for accessing the 
retrieval storage, indexing documents and queries and evaluating queries.
It has been built using SWIG based on a wrapper C++ interface (@ref strus/bindingObjects.hpp).

<b>Because of lacking namespaces all type names of the strus PHP extension have the prefix <i>Strus</i></b>.
The class names in the interface documentation are without the <i>Strus</i> prefix. 
I failed to persuade doxygen to create the names with the <i>Strus</i> prefix.

The entry point of a strus application with PHP is the context (@ref Context) object (<b>StrusContext</b>).
It is the root object from which all other objects are created.
It can be constructed either as proxy, that redirects all method calls to an RpcServer
or it can be constructed as instance running in the PHP environment.
Because it is most likely that PHP will run in a webserver context, it is recommended
to use the RPC proxy variant there.

The following examples illustrate the usage of the PHP bindings.
You'll find a tutorial based on a docker image for how to build a search engine with strus and PHP <a href="http://www.codeproject.com/Articles/1009582/Building-a-search-engine-with-Strus-tutorial">here</a>.


