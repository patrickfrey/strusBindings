<?php
var_dump(extension_loaded('strus'));

$ctx = new StrusContext();
$ctx->createStorage( "path=storage");
?>

