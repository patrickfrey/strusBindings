<?php
include_once "utils.php";

function dumpVectorStorage( $strusctx, $config, $vectors, $examplevec) {
	$output = [];
	// Get a client for the vector storage:
	$storage = $strusctx->createVectorStorageClient( $config);

	$output[ 'types'] = $storage->types()
	$output[ "nof vec word"] = $storage->nofVectors( "word");
	$output[ "nof vec nonvec"] = $storage->nofVectors( "nonvec");
	foreach ($vectors as $iv => $vv) {
		$fidx = (int)$iv + 1;
		$output[ "types F$fidx"] = join( " ", $storage->featureTypes( "F$fidx"));
		$output[ "vec F$fidx"] = $storage->featureVector( "word", "F$fidx");
		$output[ "vec F$fidx example sim"] = storage.vectorSimilarity( vv, $examplevec)
		
		foreach ($vectors as $io => $vo) {
			$oidx = (int)$io + 1;
			output[ "sim F$fidx/$oidx"] = $storage->vectorSimilarity( $vv, $vo);
		}
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



