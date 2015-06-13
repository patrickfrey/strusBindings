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
<div id="wrap">
	<div id="content">
		<h2>Object description</h2>
		<p class="description">This section describes the objects of the strus PHP bindings
		with examples of how to create and use them. Because PHP is a value typed language
		with no explicit typeing we refer to the type of an object that exists only as 
		a single instance in the context by using the name of the object type starting with
		a lower case letter as name of the variable. The snippet
		<pre>
                        $query = $queryEval->createQuery( $storage);
		</pre>
		States the construction of an object of type <i>Query</i> with the name <i>query</i>
		by an object of type <i>QueryEval</i> with name <i>queryEval</i> with the parameter
		of type <i>Storage</i> named <i>storage</i>.
		</p>

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
