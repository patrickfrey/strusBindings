<?php
include_once "utils.php";

function dumpVectorStorage( $strusctx, $config) {
	$output = [];
	// Get a client for the vector storage:
	$storage = $strusctx->createVectorStorageClient( $config);

	// Dump concept classes of the storage:
	$classes = $storage->conceptClassNames();
	$output[ 'classes'] = $classes;
	foreach ($classes as $cl) {
		$csize = $storage->nofConcepts( $cl);
		for ($ci = 1; $ci < $csize; $ci++) {
			$cfeats = $storage->conceptFeatures( $cl, $ci);
			$output[ "class '$cl' concept $ci"] = $cfeats;
		}
	}
	// Dump features of the storage:
	$nofvecs = $storage->nofFeatures();
	for ($fi = 0; $fi < $nofvecs-1; $fi++) {
		$fname = $storage->featureName( $fi);
		$fidx = $storage->featureIndex( $fname);
		if ($fi != $fidx) {
			fwrite(STDERR, "feature index does not match $fi ~= $fidx ($fname)");
			exit(1);
		}
		foreach ($classes as $cl) {
			$concepts = $storage->featureConcepts( $cl, $fi);
			$output[ "class '$cl' feature $fi" ] = $concepts;
		}
		$fvector = $storage->featureVector( $fi);
		$output[ "vector '$fname'"] = $fvector;
	}
	// Configuration of the storage:
	$output_config = [];
	foreach ($storage->config() as $key => $value) {
		if ($key == 'path') {
			$output_config[ $key] = getFileName( $value);
		} else {
			$output_config[ $key] = $value;
		}
	}
	$output[ "config"] = $output_config;
	$storage->close();
	return $output;
}



