<?php
include_once "utils.php";

function createCollection( $strusctx, $storagePath, $metadata, $analyzer, $multipart, $datadir, $fnams, $aclmap, $withrpc) {
	$config = NULL;
	if (!$withrpc) {
		$config = [
			"path" => $storagePath,
			"metadata" => $metadata,
			"cache" => '512M',
			"statsproc" => 'default'
		];
		if ($aclmap) {
			$config['acl'] = True;
		}
		if ($strusctx->storageExists( $config)) {
			$strusctx->destroyStorage( $config);
		}
		$strusctx->createStorage( $config);
	}
	# Get a client for the new created storage:
	$storage = $strusctx->createStorageClient( $config);

	# Read input files, analyze and insert them:
	$transaction = $storage->createTransaction();
	$files = [];
	$idx = 0;
	foreach ($fnams as $fnam) {
		$filename = $datadir . '/'. $fnam;
		$idx = $idx + 1;
		if ($multipart) {
			foreach ($analyzer->analyzeMultiPart( readFileContent( $filename)) as $doc) {
				if ($aclmap && isset($aclmap[ $doc->attribute['docid']])) {
					$doc->access = $aclmap[ $doc->attribute['docid']];
				}
				$transaction->insertDocument( $doc->attribute['docid'], $doc);
			}
		} else {
			$content = readFileContent( $filename);
			$docclass = $strusctx->detectDocumentClass( $content);
			$doc = $analyzer->analyzeSingle( $content);
			$doc->attribute['docid'] = $fnam;
			$doc->attribute['docclass'] =
				"mimetype=" . $docclass->mimetype
				. ", encoding=" . $docclass->encoding
				. ", scheme=" . $docclass->scheme;
			if ($aclmap && isset($aclmap[ $fnam])) {
				$doc->access = $aclmap[ $fnam];
			}
			$transaction->insertDocument( $fnam, $doc);
		}
	}
	# Without this the documents wont be inserted:
	$transaction->commit();
	# Explicit call of close for garbage collector (explicit free of storage access):
	$storage->close();
}
?>

