<?php
require "strus.php";

$config = "path=storage; metadata=doclen UINT16";
$ctx = new StrusContext();
try {
	$ctx->destroyStorage( $config);
	# ... delete the storage files if they already exists
} catch( Exception $e ) {
}

# Create a new storage:
$ctx->createStorage( $config);
# Get a client for the new created storage:
$storage = $ctx->createStorageClient( $config);

# Define the document analyzer to use:
$analyzer = $ctx->createDocumentAnalyzer();

# Define the features and attributes to store:
$analyzer->defineAttribute( "title", "/doc/title()", "content", "orig");
$analyzer->addSearchIndexFeature( "word", "/doc/text()", "word", array( array( "stem","en"),"lc",array( "convdia","en")), "");
$analyzer->addForwardIndexFeature( "orig", "/doc/text()", "split", "orig", "");

# Read input files, analyze and insert them:
$datadir = "./data/";
$datafiles = scandir( $datadir);
$transaction = $storage->createTransaction();
foreach ($datafiles as &$datafile)
{
	if ($datafile[0] != '.')
	{
		print "process document " . $datafile . "\n";
		$docid = substr( $datafile, 0, strlen($datafile) -4);
		$fullname = $datadir . $datafile;
		$doc = $analyzer->analyze( file_get_contents( $fullname));
		$transaction->insertDocument( $docid, $doc, True);
	}
}
# Without this the documents wont be inserted:
$transaction->commit();
print "done";
?>


