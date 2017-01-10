<?php
require "strus.php";

$config = "path=vsm;dim=7;bit=3;var=32;maxdist=5;probsim=yes;simdist=5;probdist=8;raddist=5;eqdist=1;singletons=yes";
$ctx = new StrusContext();
$ctx->loadModule( "storage_vector_std");

try {
	$ctx->destroyStorage( $config);
	# ... delete the storage files if they already exists
} catch( Exception $e ) {
}

# Create a new storage:
$ctx->createVectorStorage( $config);

# Add the vectors:
$config="path=vsm;dim=7;bit=3;var=32;maxdist=5;probsim=yes;simdist=5;probdist=8;raddist=5;eqdist=1;singletons=yes";
$builder = $ctx->createVectorStorageBuilder( $config);
$builder->addFeature( "A", array( 0.1, 0.2, 0.3, 0.1, 0.1, 0.1, 0.1) );
$builder->addFeature( "B", array( 0.15, 0.25, 0.35, 0.09, 0.11, 0.12, 0.1) );
$builder->addFeature( "C", array( 0.3, 0.2, 0.1, 0.12, 0.07, 0.09, 0.11) );
$builder->addFeature( "D", array( 0.35, 0.25, 0.15, 0.13, 0.13, 0.12, 0.13) );
$builder->addFeature( "E", array( 0.25, 0.2, 0.2, 0.12, 0.13, 0.13, 0.13) );
$builder->addFeature( "F", array( 0.2, 0.25, 0.2, 0.11, 0.09, 0.08, 0.12) );
$builder->addFeature( "G", array( 0.2, 0.2, 0.25, 0.12, 0.1, 0.08, 0.12) );
$builder->run( "");

# Get a client for the new created storage:
$storage = $ctx->createVectorStorageClient( $config);

print "storage config: " . $storage->config() . "\n";

$searcher = $storage->createSearcher( 0, $storage->nofFeatures());

$similarVectors = $searcher->findSimilar( array( 0.1, 0.2, 0.3, 0.1, 0.1, 0.1, 0.1 ), 4);
foreach ($similarVectors as &$sv)
{
    print "[1] index: " . $sv->index . " weight " . $sv->weight . "\n";
}
$similarVectors = $searcher->findSimilar( array( 0.35, 0.25, 0.15, 0.13, 0.13, 0.12, 0.13 ), 4);
foreach ($similarVectors as &$sv)
{
    print "[2] index: " . $sv->index . " weight " . $sv->weight . "\n";
}

$conceptClassNames = $storage->conceptClassNames();
foreach ($conceptClassNames as &$name) {
    print "concept class: '" . $name . "'\n";
}
$nofConcepts = $storage->nofConcepts( "");
for ($ci=1; $ci<=$nofConcepts; $ci++) {
    $conceptFeatures = $storage->conceptFeatures( "", $ci);
    foreach ($conceptFeatures as &$fi) {
        print "concept " . $ci . ", member " . $fi . "\n";
    }
}
$nofFeatures = $storage->nofFeatures();
for ($fi=0; $fi<$nofFeatures; $fi++) {
    $name = $storage->featureName( $fi);
    print "feature " . $storage->featureIndex($name) . " name: '" . $name . "'\n";
    $featureConcepts = $storage->featureConcepts( "", $fi);
    foreach ($featureConcepts as $ci) {
        print "feature " . $fi . " assigned to " . $ci . "\n";
        $featureVector = $storage->featureVector( $fi);
    }
    foreach ($featureVector as &$vi) {
        print "feature vector " . $fi . " element " . $vi . "\n";
    }
}
$searcher->close();
$storage->close();

print "done\n";

