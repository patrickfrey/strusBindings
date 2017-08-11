<?php
include_once "utils.php";

function dumpCollection( $strusctx, $storagePath) {
	$config = "path=" . $storagePath . '; cache=512M; statsproc=default';
	$output = [];

	# Get a client for the storage:
	$storage = $strusctx->createStorageClient( $config);

	# Configuration of the storage:
	$output_config = [];
	foreach ($storage->config() as $key => $value) {
		if (strcmp( $key, 'path') == 0) {
			$output_config[ $key] = getFileName( $value);
		} else {
			$output_config[ $key] = $value;
		}
	}
	$output[ "config"] = $output_config;
	$output[ "nofdocs"] = $storage->nofDocumentsInserted();

	# Document identifiers:
	$output_docids = [];
	$dociditr = $storage->docids();
	foreach ($dociditr as $tp) {
		array_push( $output_docids, ['id' => $tp, 'docno' => $storage->documentNumber($tp)]);
	}
	$output[ "docids"] = $output_docids;

	# Term types:
	$termtypes = joinLists( $storage->termTypes());
	$output[ "types"] = $termtypes;

	# Document data (metadata,attributes,content):
	$selectids = joinLists( "docno", "ACL", $storage->attributeNames(), $storage->metadataNames(), $termtypes);
	$output_docs = [];
	$output_terms = [];
	foreach ($storage->select( $selectids, NULL, NULL, 0) as $docrow) {
		$flatdocrow = [];
		foreach ($docrow as $colkey => $colval) {
			$flatdocrow[ $colkey] = concatValues( $colval);
		}
		$output_docs[ $docrow['docid']] = $flatdocrow;

		foreach ($termtypes as $termtype) {
			$ftermlist = [];
			$ftermiter = $storage->documentForwardIndexTerms( $docrow['docno'], $termtype);
			foreach ($ftermiter as $fterm) {
				array_push( $ftermlist, ['value' => $fterm[0], 'pos' => $fterm[1]]);
			}
			$output_terms[ $docrow['docid'] . ":" . $termtype . ' (f)'] = $ftermlist;

			$stermiter = $storage->documentSearchIndexTerms( $docrow['docno'], $termtype);
			$stermlist = [];
			foreach ($stermiter as $sterm) {
				array_push( $stermlist, ["value" => $sterm[0], "tf" => $sterm[1], "firstpos" => $sterm[2]]);
			}
			$output_terms[ $docrow['docid'] . ":" . $termtype . ' (s)'] = $stermlist;
		}
	}
	$output[ "docs"] = $output_docs;
	$output[ "terms"] = $output_terms;

	# Term statistics:
	$output_stat = [];
	foreach ($storage->getAllStatistics( true) as $blob) {
		$statview = $strusctx->unpackStatisticBlob( $blob, "default");
		$dfchangelist = [];
		foreach ($statview[ "dfchange"] as $dfchange) {
			array_push( $dfchangelist, $dfchange);
		}
		$output_stat[ "dfchange"] = $dfchangelist;
		if (!isset($output_stat[ "nofdocs"])) {
			$output_stat[ "nofdocs"] = $statview[ "nofdocs"];
		} else {
			$output_stat[ "nofdocs"] += $statview[ "nofdocs"];
		}
	}
	$output[ "stat"] = $output_stat;

	$storage->close();
	return $output;
}


