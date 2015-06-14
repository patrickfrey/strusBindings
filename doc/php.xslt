<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
	<link rel="icon" type="image/ico" href="images/strus.ico" />
	<meta http-equiv="content-type" content="text/html; charset=utf-8" />
	<meta name="description" content="Language bindings PHP for strus, a collection of C++ libraries for building a full-text search engine." />
	<meta name="keywords" content="fulltext search engine C++" />
	<meta name="author" content="Patrick Frey &lt;patrickpfrey (a) yahoo (dt) com&gt;" />
	<link rel="stylesheet" type="text/css" href="text-profile.css" title="Text Profile" media="all" />
	<title>PHP bindings for strus</title>
</head>
<body>
<xsl:for-each select="bindings/tracking">
<xsl:if test="@id = 'php'">
<script>
<xsl:value-of select="."/>
</script>
</xsl:if> 
</xsl:for-each>
<div id="wrap">
	<div id="content">
		<h1>PHP language bindings</h1>
		<h2>Introduction</h2>
		<p class="description">This chapter describes the language bindings of strus for PHP
		with examples of how to create and use them.<br/>
		The language bindings have been built using <a href="http://www.swig.org">SWIG</a>
		based on a wrapper C++ interface. This wrapper interface serves as common base
		for all language bindings as a sort of super interface (strus/bindingObjects.hpp).
		If you write a language binding you should have a look at this wrapper interface.
		It is not part of this documentation, because showing it could be misleading.
		</p>
		<h2>Overview</h2>
		<p class="description">
		The root object you construct first in PHP is <a href="#StrusContext">the
		strus context</a>. From this context you can construct the main instances
		of the objects doing the job you want to do. Depending on how you create
		the strus context, the objects you create beginning with the root object
		are accessed directly or they are residing on a server and the language 
		objects serve as proxy for the server objects.
		</p>
		<h2>Examples</h2>
		<p class="description">
		PHP is a value typed language with no explicit typing.<br/>
		In the following examples we refer to the type of an object that 
		exists only as a single instance in the context by using the name of 
		the object type starting with a lower case letter as name of the variable.
		The snippet
		</p>
		<pre>

<a href="#Query">$query</a> = <a href="#QueryEval">$queryEval</a>-&gt;createQuery( <a href="#Storage">$storage</a>);
		</pre>
		<p class="description">
		describes therefore the construction of an object of type <a href="#Query">Query</a> with the name <i>query</i>
		by an object of type <a href="#QueryEval">QueryEval</a> with name <i>queryEval</i> with the parameter
		of type <a href="#Storage">Storage</a> named <i>storage</i>.
		</p>
		<h3>Create a strus RPC context for a session residing on a server</h3>
		<p class="description">
		You create a remote strus context by calling its constructor with the RPC server address
		as argument.
		</p>
		<pre>
<a href="#StrusContext">$context</a> = new StrusContext( "localhost:7181" );
		</pre>
		<h3>Create a query evaluation scheme</h3>
		<p class="description">
		A query evaluation scheme is created by calling the context method to create
		a query evaluation instance. The scheme is built by calling the methods for
		building the scheme on this instance. The weighting function configuration is
		defined as associative array. The keys starting with dot '.' reference query
		feature sets used by the scheme.
		</p>
		<pre>

<a href="#QueryEval">$queryEval</a> = $context-&gt;createQueryEval();
<a href="#QueryEval">$queryEval</a>-&gt;addWeightingFunction( 1.0, "BM25", [
                "k1" =&gt; 0.75, "b" =&gt; 2.1,
                "avgdoclen" =&gt; 500,
                ".match" =&gt; "docfeat" ]);
		</pre>
		<h3>Analyze a query phrase</h3>
		<p class="description">
		A query in strus is modeled as a set of query phrases combined by some proprietary
		operators of a query language. Strus does not impose any defined query language.
		Maybe there is none. There is a proposition for a query language implemented in
		the utilities program loader library, but it is not mandatory to use it.<br/>
		The query analyzer allows you to define methods of analyzing the basic parts
		called phrases of your query language. The following example shows the definition
		of a phrase type and the analysis ot the phrase "hello&#32;world":
		</p>
		<pre>
<a href="#QueryAnalyzer">$queryAnalyzer</a> = <a href="#StrusContext">$context</a>-&gt;createQueryAnalyzer();
<a href="#QueryAnalyzer">$queryAnalyzer</a>-&gt;definePhraseType( "text", "stem", "word", 
                ["lc",
                ["dictmap", "irregular_verbs_en.txt"],
                ["stem", "en"],
                ["convdia", "en"],
                "lc"]);
<a href="#Term">$terms</a> = <a href="#QueryAnalyzer">$queryAnalyzer</a>-&gt;analyzePhrase( "text", "hello world");
		</pre>
		<h3>Create and evaluate a query</h3>
		<p class="description">
		A query is created by calling the constructing method of the query evaluation instance 
		of the query evaluation scheme you want to use. The query is built by calling the
		query instance methods for constructing the queried terms and structures.
		</p>
		<pre>
<a href="#Storage">$storageClient</a> = $context-&gt;createStorageClient( "" );
<a href="#Storage">$query</a> = <a href="#QueryEval">$queryEval</a>-&gt;createQuery( "" );

if (count( <a href="#Term">$terms</a>) > 0)
{
        foreach (<a href="#Term">$terms</a> as &amp;<a href="#Term">$term</a>)
        {
                <a href="#Query">$query</a>-&gt;pushTerm( "stem", <a href="#Term">$term</a>-&gt;value);
                <a href="#Query">$query</a>-&gt;pushDuplicate( "stem", <a href="#Term">$term</a>-&gt;value);
                <a href="#Query">$query</a>-&gt;defineFeature( "docfeat");
        }
        <a href="#Query">$query</a>-&gt;pushExpression( "within", count($terms), 100000);
        <a href="#Query">$query</a>-&gt;defineFeature( "selfeat");
}
<a href="#Query">$query</a>-&gt;setMaxNofRanks( $maxNofRanks);
<a href="#Query">$query</a>-&gt;setMinRank( $minRank);
$result = return <a href="#Query">$query</a>-&gt;evaluate();
		</pre>
		<h2>Objects</h2>
		<xsl:for-each select="bindings/class">
		<a>
			<xsl:attribute name="name">
			<xsl:value-of select="@id"/>
			</xsl:attribute>
			<xsl:value-of select="' '"/>
		</a>
		<h3><xsl:value-of select="@id"/></h3>
		<p class="description">
			<xsl:value-of select="description"/><xsl:value-of select="' '"/>
			<xsl:value-of select="remark"/><xsl:value-of select="' '"/>
			<xsl:value-of select="note"/>
		</p>
		<h4>Methods</h4>
		<div id="api">
		<table border="1">
		<tr>
		<th width="10%" align="left">Name</th>
		<th width="10%" align="left">Result</th>
		<th width="30%" align="left">Arguments</th>
		<th width="30%" align="left">Description</th>
		</tr>
		<xsl:for-each select="method">
		<tr>
		<td valign="top" align="left"><b><xsl:value-of select="@id"/></b></td>
		<td valign="top" align="left">
			<xsl:choose>
				<xsl:when test="@return = 'void'">
				</xsl:when>
				<xsl:when test="@return = 'GlobalCounter'">
					64 bit integer if available for PHP on the platform,
					otherwise 32 bit integer
				</xsl:when>
				<xsl:when test="@return = 'StorageClient'">
					<a href="#StorageClient"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'DocumentAnalyzer'">
					<a href="#DocumentAnalyzer"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'QueryAnalyzer'">
					<a href="#QueryAnalyzer"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'QueryEval'">
					<a href="#QueryEval"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'Document'">
					<a href="#Document"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'Query'">
					<a href="#Query"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'Term'">
					<a href="#Term"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'Term[]'">
					<a href="#Term"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'MetaData'">
					<a href="#MetaData"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'MetaData[]'">
					<a href="#MetaData"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'Attribute'">
					<a href="#Attribute"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'Attribute[]'">
					<a href="#Attribute"><xsl:value-of select="@return"/></a>
				</xsl:when>
				<xsl:when test="@return = 'Map'">
					Associative array with strings as keys and string or numeric values
				</xsl:when>
				<xsl:when test="@return = 'Map[]'">
				List of associative arrays with strings as keys and string or numeric values
				</xsl:when>
				<xsl:when test="@return = 'Variant'">
					Any numeric type (byte, int, float)
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="@return"/>
				</xsl:otherwise>
			</xsl:choose>
		</td>
		<td valign="top" align="left"><xsl:for-each select="param">
			<b><xsl:value-of select="@name"/></b>:&#32;&#32;<xsl:value-of select="description"/> <br/>
			<xsl:choose>
				<xsl:when test="@type = 'Document'">
					<a href="#Document">(type Document)</a>
				</xsl:when>
				<xsl:when test="@type = 'Map'">
					(type associative array)
				</xsl:when>
				<xsl:when test="@type = 'Map[]'">
					(type list of associative arrays)
				</xsl:when>
				<xsl:when test="@type = 'Variant'">
					Any numeric type (byte, int, float)
				</xsl:when>
				<xsl:otherwise>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each></td>
		<td valign="top" align="left">
			<xsl:value-of select="description"/><xsl:value-of select="' '"/>
			<xsl:value-of select="remark"/><xsl:value-of select="' '"/>
			<xsl:value-of select="note"/>
		</td>
		</tr>
		</xsl:for-each>
		</table>
		</div>
		</xsl:for-each>
	</div>
</div>
</body>
</html>
</xsl:template>
</xsl:stylesheet> 
