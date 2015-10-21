strus PHP bindings	 {#mainpage}
==================

The strus PHP bindings provide a PHP interface for accessing the 
retrieval storage, indexing documents and queries and evaluating queries.
<b>Because of lacking namespaces all type names of the strus PHP extension have the prefix <i>Strus</i></b>.
The class names in the interface documentation are without the <i>Strus</i> prefix. 
I failed to persuade doxygen to create the names with the <i>Strus</i> prefix.

The entry point of a strus application with PHP is the context (@ref Context) object (StrusContext).
It is the root object from which all other objects are created.
It can be constructed either as proxy, that redirects all method calls to an RpcServer
or it can be constructed as instance running in the PHP environment.
Because it is most likely that PHP will run in a webserver context, it is recommended
to use the RPC proxy variant there.

The following examples illustrate the usage of the PHP bindings.
PHP is a value typed language with no explicit typing.
In the following examples we refer to the type of an object that 
exists only as a single instance in the context by using the name of 
the object type starting with a lower case letter as name of the variable.

\section createContext Create the StrusContext object, the root object of the strus PHP interface as proxy to the RPC server running on the same machine on port 7181.
\code
	$context = new StrusContext( "localhost:7181" );
\endcode

\section createQueryEval Create and initialize a query evaluation scheme
\code
	$queryEval = $context->createQueryEval();

	# Add weighting of the features of the feature set "docfeat":
	$queryEval->addWeightingFunction( 1.0, "BM25", [
			"k1" => 0.75, "b" => 2.1,
			"avgdoclen" => 500,
			".match" => "docfeat" ]);
	
	# Define what data is shown to present a result of the query:
	$queryEval->addSummarizer(
			"TITLE", "attribute", [ "name" => "title" ] );
	$queryEval->addSummarizer( "CONTENT", "matchphrase", [
			"type" => "orig", "len" => 40, "nof" => 3, "structseek" => 30,
			"mark" => '$',
			".struct" => "sentence", ".match" => "docfeat" ] );
	
	# Define the set of features used to select the document that should be weighted as "selfeat":
	$queryEval->addSelectionFeature( "selfeat");
\endcode

\section analyzeQuery Analyze a query
A query in strus is modeled as a set of query phrases combined by some proprietary
operators of a query language. Strus does not impose any defined query language.
Maybe there is none. There is a proposition for a query language implemented in
the utilities program loader library, but it is just an example.<br/>
The query analyzer allows you to define methods of analyzing the basic parts
called phrases of your query language. The following example shows the definition
of a phrase type and the analysis ot the phrase "hello world":
\code
	$queryAnalyzer = $context->createQueryAnalyzer();
	$queryAnalyzer->definePhraseType( "text", "stem", "word", 
			["lc",
			["dictmap", "irregular_verbs_en.txt"],
			["stem", "en"],
			["convdia", "en"],
			"lc"]);
	$queryAnalyzer->analyzePhrase( "text", "hello world");
\endcode

\section createQuery Creates a query from a query evaluation (@ref QueryEval) scheme object for querying a storage specified and evaluates the query.
A query is created by calling the constructing method of the query evaluation instance 
of the query evaluation scheme you want to use. The query is built by calling the
query instance methods for constructing the queried terms and structures.
\code
	$query = $queryEval->createQuery( $storage);
	if (count( $terms) > 0)
	{
		foreach ($terms as $term)
		{
			$query->pushTerm( "stem", $term->value);
			$query->defineFeature( "docfeat");
		}
		foreach ($terms as $term)
		{
			$query->pushTerm( "stem", $term->value);
		}
	}
	$query->pushExpression( "within", count($terms), 100000);
	$query->defineFeature( "selfeat");

	$query->setMaxNofRanks( $maxNofRanks);
	$query->setMinRank( $minRank);
	$result = $query->evaluate();
\endcode

\section Iterate on the query evaluation result and print it
\code
foreach ($results as &$result)
{
	$title = "";
	$content = "";
	foreach ($result->attributes as &$attrib)
	{
		if ($attrib->name == 'CONTENT')
		{
			if ($content != "") $content .= ' ... ';
			$content .= $attrib->value;
		}
		if ($attrib->name == 'TITLE')
		{
			$title .= $attrib->value;
		}
	}
	echo "rank_docno = " . $result->docno . " weight = " + . $result->weight . " title = " . $title;
}
\endcode



