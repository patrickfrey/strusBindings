<?php
include_once "utils.php";

function ranklist_cmp( $a, $b)
{
	return ($a[1] < $b[1] || ($a[1] == $b[1] && $a[0] < $b[0]));
}

function vecToString( $vec, $delim)
{
	$rt = "";
	foreach ($vec as $iv => $vv) {
		if (!empty($rt)) {
			$rt .= $delim;
		}
		$vv_rounded = round( floatval( $vv), 5);
		$rt .= "$vv_rounded";
	}
	return $rt;
}

function dumpVectorStorage( $strusctx, $config, $vectors, $examplevec) {
	$output = [];
	// Get a client for the vector storage:
	$storage = $strusctx->createVectorStorageClient( $config);

	$ranklist = [];

	$output[ 'types'] = $storage->types();
	$output[ "nof vec word"] = $storage->nofVectors( "word");
	$output[ "nof vec nonvec"] = $storage->nofVectors( "nonvec");
	foreach ($vectors as $iv => $vv) {
		$fidx = (int)$iv + 1;
		$featstr = "F$fidx";
		$featsim = floatval( sprintf( "%.3f", round( $storage->vectorSimilarity( $vv, $examplevec), 4)));
		$featvec = $storage->featureVector( "word", $featstr);
		$output[ "types $featstr"] = $storage->featureTypes( $featstr);
		$output[ "vec $featstr"] = "(" . vecToString( $featvec, ", ") . ")";
		$output[ "vec $featstr example sim"] = $featsim;

		array_push( $ranklist, [$featstr,$featsim,$featvec] );
	}

	usort( $ranklist, "ranklist_cmp");
	foreach ($ranklist as $ir => $rr) {
		if ($ir >= 5) {
			break;
		}
		$ridx = $ir + 1;
		$weight = floatval( sprintf( "%.3f", round( floatval( $rr[1]), 4)));
		$feat = $rr[0];
		$vec = vecToString( $rr[2], ", ");
		$output[ "rank $ridx"] = "$weight $feat {$vec}";
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



