<?php
include_once "utils.php";

function dfchange_compare( $a, $b) {
	if ($a['type'] < $b['type']) {return -1;}
	if ($a['type'] > $b['type']) {return +1;}
	if ($a['value'] < $b['value']) {return -1;}
	if ($a['value'] > $b['value']) {return +1;}
	if ($a['increment'] < $b['increment']) {return -1;}
	if ($a['increment'] > $b['increment']) {return +1;}
	return 0;
}

function dumpCollection( $strusctx, $storagePath) {
	$config = "path=" . $storagePath . '; cache=512M; statsproc=std';
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
		array_push( $output_docids, $tp );
	}
	sort( $output_docids );
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
			if ($colkey != 'docno')
			{
				$flatdocrow[ $colkey] = concatValues( $colval);
			}
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
	$dfchangelist = [];
	$bloblist = [];
	$nofdocs = 0;
	foreach ($storage->getAllStatistics() as $blob) {
		array_push( $bloblist, $blob);
		$statview = $strusctx->unpackStatisticBlob( $blob, "std");
		$nofdocs += $statview->nofdocs;
		foreach ($statview->dfchange as $dfchange) {
			array_push( $dfchangelist, ["value" => $dfchange->value, "type" => $dfchange->type, "increment" => $dfchange->increment]);
		}
	}
	usort( $dfchangelist, "dfchange_compare");
	$output_stat[ "dfchange"] = $dfchangelist;
	$output_stat[ "nofdocs"] = $nofdocs;
	$output_stat[ "statblobs"] = $bloblist;
	$output[ "stat"] = $output_stat;

	$storage->close();
	return $output;
}


