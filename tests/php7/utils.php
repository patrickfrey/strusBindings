<?php
function concatValues($o) {
	if (is_array($o)) {
		$rt = NULL;
		foreach($array as $item) {
			if ($rt) {
				$rt = $rt . " " . "$item";
			} else {
				$rt = "$item";
			}
		}
		return $rt;
	} else {
		return "$o";
	}
}

function dumpValue_( $o, $depth) {
	if (is_array($o)) {
		if ($depth == 0) {
			return "{...}";
		}
		$s = '{ ';
		$i = 0;
		foreach($o as $k => $v) {
			if ($i > 0) {
				$s = $s . ', ';
			}
			$i = $i + 1;
			$s = $s . '[' . $k . '] = ' . dumpValue_( $v, $depth-1);
		}
		return $s . '} ';
	} elseif (is_numeric($o)) {
		$num = round( floatval( $o), 5);
		return "$num";
	} else {
		return tostring( $o);
	}
}
function dumpValue( $o) {
	return dumpValue_( $o, 10);
}

function dumpTree_( $indent, $o, $depth) {
	if (is_array($o)) {
		if ($depth == 0) {
			return "{...}";
		}
		$keyset = [];
		foreach($o as $k => $v) {
			array_push( $keyset, $k);
		}
		sort( $keyset);
		$s = "";
		foreach($keyset as $k) {
			$ke = "\n" . $indent . gettype($k) . " " . $k;
			$ve = dumpTree_( $indent . '  ', $o[ $k], $depth-1);
			$s = $s . $ke . ":" . $ve;
		}
		return $s;
	} elseif (is_numeric($o)) {
		$num = round( floatval( $o), 5);
		return "$num";
	} else {
		return tostring( $o);
	}
}
function dumpTree( $o) {
	return dumpTree_( "", $o, 10);
}

function readFileContent( $path) {
	$myfile = fopen( $path, "r") or die("Unable to open file '$path' for reading");
	$content = fread( $myfile, filesize( $path));
	fclose($myfile);
	return $content;
}

function writeFileContent( $path, $content) {
	$myfile = fopen( $path, "w") or die("Unable to open file '$path' for writing");
	fwrite( $myfile, $content);
	fclose($myfile);
}

function verifyTestOutput( $outputdir, $result, $expected) {
	if (strcmp( $result, $expected) != 0) {
		writeFileContent( $outputdir . "/RES", $result);
		writeFileContent( $outputdir . "/EXP", $expected);

		echo "Failed";
		exit( 1);
	} else {
		echo "OK";
	}
}

function getPathArray( $filepath) {
	return preg_split( "[^/\\]+", $filepath);
}

function getFileName( $filepath) {
	$pathar = getPathArray( $filepath);
	return end($pathar);
}

function joinLists(...$args) {
	$rt = [];
	foreach($args as $arg) {
		if (is_object($arg) || is_array($arg)) {
			foreach($arg as $elem) {
				array_push( $rt, $elem);
			}
		} else {
			array_push( $rt, $arg);
		}
	}
	return $rt;
}

function getContextConfig( $argval) {
	if (strcmp($argval,'trace') == 0) {
		return ["trace" => ["log" => "dump", "file" => "stdout"]];
	} elseif ($argval) {
		return ["rpc" => argval];
	} else {
		return NULL;
	}
}
?>
