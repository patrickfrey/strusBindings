<?php
require "strus.php";

$ctx = new StrusContext();

$config = "path=storage; metadata=doclen UINT16";
$storage = $ctx->createStorageClient( $config);

# Query the number of documents inserted:
$nofDocuments = $storage->nofDocumentsInserted();

# Output:
print "Number of documents inserted: $nofDocuments\n";
?>


