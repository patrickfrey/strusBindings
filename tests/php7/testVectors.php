<?php
include_once "utils.php";
include_once "dumpVectorStorage.php";

$outputdir = '.';
if (isset($argv[1])) {
	$outputdir = $argv[1];
}
$storage = $outputdir . "/storage";
$config = [
	'path' => $storage,
	'commit' => 10,
	'dim' => 10,
	'bit' => 8,
	'var' => 100,
	'simdist' => 12,
	'maxdist' => 20,
	'realvecweights' => True
];
$vectors = [];

$ctx = new StrusContext();
$ctx->loadModule( "storage_vector_std");

if ($ctx->storageExists( $config)) {
	$ctx->destroyStorage( $config);
}

for ($vi = 1; $vi <= 100; $vi++) {
	$vv = [];
	for ($xi = 1; $xi <= 10; $xi++) {
		array_push( $vv, 1.0 / sqrt( $vi + $xi));
	}
	array_push( $vectors, $vv);
}

$ctx->createVectorStorage( $config);
$storage = $ctx->createVectorStorageClient( $config);
$transaction = $storage->createTransaction();

foreach ($vectors as $iv => $vv) {
	$fidx = (int)$iv + 1;
	$transaction->addFeature( "F$fidx", $vv);
	$transaction->defineFeatureConceptRelation( "main", $iv, $fidx);
}
$transaction->commit();
$transaction->close();
$output = dumpVectorStorage( $ctx, $config);

$storage = $ctx->createVectorStorageClient( $config);
$searcher = $storage->createSearcher( 0, $storage->nofFeatures());
$simlist = $searcher->findSimilar( [0.1,0.1,0.1,0.1,0.1, 0.1,0.1,0.1,0.1,0.1], 10);
$output[ 'simlist 0.1x10'] = $simlist;

$result = "vector storage dump:" . dumpTree( $output);
$expected = <<<END_expected
vector storage dump:
string class '' feature 0: 
string class '' feature 1: 
string class '' feature 10: 
string class '' feature 11: 
string class '' feature 12: 
string class '' feature 13: 
string class '' feature 14: 
string class '' feature 15: 
string class '' feature 16: 
string class '' feature 17: 
string class '' feature 18: 
string class '' feature 19: 
string class '' feature 2: 
string class '' feature 20: 
string class '' feature 21: 
string class '' feature 22: 
string class '' feature 23: 
string class '' feature 24: 
string class '' feature 25: 
string class '' feature 26: 
string class '' feature 27: 
string class '' feature 28: 
string class '' feature 29: 
string class '' feature 3: 
string class '' feature 30: 
string class '' feature 31: 
string class '' feature 32: 
string class '' feature 33: 
string class '' feature 34: 
string class '' feature 35: 
string class '' feature 36: 
string class '' feature 37: 
string class '' feature 38: 
string class '' feature 39: 
string class '' feature 4: 
string class '' feature 40: 
string class '' feature 41: 
string class '' feature 42: 
string class '' feature 43: 
string class '' feature 44: 
string class '' feature 45: 
string class '' feature 46: 
string class '' feature 47: 
string class '' feature 48: 
string class '' feature 49: 
string class '' feature 5: 
string class '' feature 50: 
string class '' feature 51: 
string class '' feature 52: 
string class '' feature 53: 
string class '' feature 54: 
string class '' feature 55: 
string class '' feature 56: 
string class '' feature 57: 
string class '' feature 58: 
string class '' feature 59: 
string class '' feature 6: 
string class '' feature 60: 
string class '' feature 61: 
string class '' feature 62: 
string class '' feature 63: 
string class '' feature 64: 
string class '' feature 65: 
string class '' feature 66: 
string class '' feature 67: 
string class '' feature 68: 
string class '' feature 69: 
string class '' feature 7: 
string class '' feature 70: 
string class '' feature 71: 
string class '' feature 72: 
string class '' feature 73: 
string class '' feature 74: 
string class '' feature 75: 
string class '' feature 76: 
string class '' feature 77: 
string class '' feature 78: 
string class '' feature 79: 
string class '' feature 8: 
string class '' feature 80: 
string class '' feature 81: 
string class '' feature 82: 
string class '' feature 83: 
string class '' feature 84: 
string class '' feature 85: 
string class '' feature 86: 
string class '' feature 87: 
string class '' feature 88: 
string class '' feature 89: 
string class '' feature 9: 
string class '' feature 90: 
string class '' feature 91: 
string class '' feature 92: 
string class '' feature 93: 
string class '' feature 94: 
string class '' feature 95: 
string class '' feature 96: 
string class '' feature 97: 
string class '' feature 98: 
string class 'main' feature 0: 
  integer 0: 1
string class 'main' feature 1: 
  integer 0: 2
string class 'main' feature 10: 
  integer 0: 11
string class 'main' feature 11: 
  integer 0: 12
string class 'main' feature 12: 
  integer 0: 13
string class 'main' feature 13: 
  integer 0: 14
string class 'main' feature 14: 
  integer 0: 15
string class 'main' feature 15: 
  integer 0: 16
string class 'main' feature 16: 
  integer 0: 17
string class 'main' feature 17: 
  integer 0: 18
string class 'main' feature 18: 
  integer 0: 19
string class 'main' feature 19: 
  integer 0: 20
string class 'main' feature 2: 
  integer 0: 3
string class 'main' feature 20: 
  integer 0: 21
string class 'main' feature 21: 
  integer 0: 22
string class 'main' feature 22: 
  integer 0: 23
string class 'main' feature 23: 
  integer 0: 24
string class 'main' feature 24: 
  integer 0: 25
string class 'main' feature 25: 
  integer 0: 26
string class 'main' feature 26: 
  integer 0: 27
string class 'main' feature 27: 
  integer 0: 28
string class 'main' feature 28: 
  integer 0: 29
string class 'main' feature 29: 
  integer 0: 30
string class 'main' feature 3: 
  integer 0: 4
string class 'main' feature 30: 
  integer 0: 31
string class 'main' feature 31: 
  integer 0: 32
string class 'main' feature 32: 
  integer 0: 33
string class 'main' feature 33: 
  integer 0: 34
string class 'main' feature 34: 
  integer 0: 35
string class 'main' feature 35: 
  integer 0: 36
string class 'main' feature 36: 
  integer 0: 37
string class 'main' feature 37: 
  integer 0: 38
string class 'main' feature 38: 
  integer 0: 39
string class 'main' feature 39: 
  integer 0: 40
string class 'main' feature 4: 
  integer 0: 5
string class 'main' feature 40: 
  integer 0: 41
string class 'main' feature 41: 
  integer 0: 42
string class 'main' feature 42: 
  integer 0: 43
string class 'main' feature 43: 
  integer 0: 44
string class 'main' feature 44: 
  integer 0: 45
string class 'main' feature 45: 
  integer 0: 46
string class 'main' feature 46: 
  integer 0: 47
string class 'main' feature 47: 
  integer 0: 48
string class 'main' feature 48: 
  integer 0: 49
string class 'main' feature 49: 
  integer 0: 50
string class 'main' feature 5: 
  integer 0: 6
string class 'main' feature 50: 
  integer 0: 51
string class 'main' feature 51: 
  integer 0: 52
string class 'main' feature 52: 
  integer 0: 53
string class 'main' feature 53: 
  integer 0: 54
string class 'main' feature 54: 
  integer 0: 55
string class 'main' feature 55: 
  integer 0: 56
string class 'main' feature 56: 
  integer 0: 57
string class 'main' feature 57: 
  integer 0: 58
string class 'main' feature 58: 
  integer 0: 59
string class 'main' feature 59: 
  integer 0: 60
string class 'main' feature 6: 
  integer 0: 7
string class 'main' feature 60: 
  integer 0: 61
string class 'main' feature 61: 
  integer 0: 62
string class 'main' feature 62: 
  integer 0: 63
string class 'main' feature 63: 
  integer 0: 64
string class 'main' feature 64: 
  integer 0: 65
string class 'main' feature 65: 
  integer 0: 66
string class 'main' feature 66: 
  integer 0: 67
string class 'main' feature 67: 
  integer 0: 68
string class 'main' feature 68: 
  integer 0: 69
string class 'main' feature 69: 
  integer 0: 70
string class 'main' feature 7: 
  integer 0: 8
string class 'main' feature 70: 
  integer 0: 71
string class 'main' feature 71: 
  integer 0: 72
string class 'main' feature 72: 
  integer 0: 73
string class 'main' feature 73: 
  integer 0: 74
string class 'main' feature 74: 
  integer 0: 75
string class 'main' feature 75: 
  integer 0: 76
string class 'main' feature 76: 
  integer 0: 77
string class 'main' feature 77: 
  integer 0: 78
string class 'main' feature 78: 
  integer 0: 79
string class 'main' feature 79: 
  integer 0: 80
string class 'main' feature 8: 
  integer 0: 9
string class 'main' feature 80: 
  integer 0: 81
string class 'main' feature 81: 
  integer 0: 82
string class 'main' feature 82: 
  integer 0: 83
string class 'main' feature 83: 
  integer 0: 84
string class 'main' feature 84: 
  integer 0: 85
string class 'main' feature 85: 
  integer 0: 86
string class 'main' feature 86: 
  integer 0: 87
string class 'main' feature 87: 
  integer 0: 88
string class 'main' feature 88: 
  integer 0: 89
string class 'main' feature 89: 
  integer 0: 90
string class 'main' feature 9: 
  integer 0: 10
string class 'main' feature 90: 
  integer 0: 91
string class 'main' feature 91: 
  integer 0: 92
string class 'main' feature 92: 
  integer 0: 93
string class 'main' feature 93: 
  integer 0: 94
string class 'main' feature 94: 
  integer 0: 95
string class 'main' feature 95: 
  integer 0: 96
string class 'main' feature 96: 
  integer 0: 97
string class 'main' feature 97: 
  integer 0: 98
string class 'main' feature 98: 
  integer 0: 99
string classes: 
  integer 0: ""
  integer 1: "main"
string config: 
  string assignments: 7
  string baff: 0.1
  string bit: 8
  string commit: 10
  string descendants: 10
  string dim: 10
  string eqdiff: 0.25
  string eqdist: 2
  string fdf: 0.25
  string forcesim: "no"
  string greediness: 3
  string isaf: 0.6
  string iterations: 20
  string logfile: ""
  string maxage: 20
  string maxconcepts: 0
  string maxdist: 20
  string maxfeatures: 0
  string maxsimsam: 0
  string mutations: 50
  string path: "storage"
  string probdist: 18
  string probsim: "yes"
  string raddist: 12
  string realvecweights: "yes"
  string rndsimsam: 0
  string simdist: 12
  string singletons: 0
  string threads: 0
  string var: 100
  string votes: 13
string simlist 0.1x10: 
  integer 0: 
    string featidx: 98
    string weight: 0.99991
  integer 1: 
    string featidx: 97
    string weight: 0.9999
  integer 2: 
    string featidx: 96
    string weight: 0.9999
  integer 3: 
    string featidx: 95
    string weight: 0.9999
  integer 4: 
    string featidx: 94
    string weight: 0.9999
  integer 5: 
    string featidx: 93
    string weight: 0.9999
  integer 6: 
    string featidx: 92
    string weight: 0.99989
  integer 7: 
    string featidx: 91
    string weight: 0.99989
  integer 8: 
    string featidx: 90
    string weight: 0.99989
  integer 9: 
    string featidx: 89
    string weight: 0.99989
string vector 'F1': 
  integer 0: 0.70711
  integer 1: 0.57735
  integer 2: 0.5
  integer 3: 0.44721
  integer 4: 0.40825
  integer 5: 0.37796
  integer 6: 0.35355
  integer 7: 0.33333
  integer 8: 0.31623
  integer 9: 0.30151
string vector 'F10': 
  integer 0: 0.30151
  integer 1: 0.28868
  integer 2: 0.27735
  integer 3: 0.26726
  integer 4: 0.2582
  integer 5: 0.25
  integer 6: 0.24254
  integer 7: 0.2357
  integer 8: 0.22942
  integer 9: 0.22361
string vector 'F11': 
  integer 0: 0.28868
  integer 1: 0.27735
  integer 2: 0.26726
  integer 3: 0.2582
  integer 4: 0.25
  integer 5: 0.24254
  integer 6: 0.2357
  integer 7: 0.22942
  integer 8: 0.22361
  integer 9: 0.21822
string vector 'F12': 
  integer 0: 0.27735
  integer 1: 0.26726
  integer 2: 0.2582
  integer 3: 0.25
  integer 4: 0.24254
  integer 5: 0.2357
  integer 6: 0.22942
  integer 7: 0.22361
  integer 8: 0.21822
  integer 9: 0.2132
string vector 'F13': 
  integer 0: 0.26726
  integer 1: 0.2582
  integer 2: 0.25
  integer 3: 0.24254
  integer 4: 0.2357
  integer 5: 0.22942
  integer 6: 0.22361
  integer 7: 0.21822
  integer 8: 0.2132
  integer 9: 0.20851
string vector 'F14': 
  integer 0: 0.2582
  integer 1: 0.25
  integer 2: 0.24254
  integer 3: 0.2357
  integer 4: 0.22942
  integer 5: 0.22361
  integer 6: 0.21822
  integer 7: 0.2132
  integer 8: 0.20851
  integer 9: 0.20412
string vector 'F15': 
  integer 0: 0.25
  integer 1: 0.24254
  integer 2: 0.2357
  integer 3: 0.22942
  integer 4: 0.22361
  integer 5: 0.21822
  integer 6: 0.2132
  integer 7: 0.20851
  integer 8: 0.20412
  integer 9: 0.2
string vector 'F16': 
  integer 0: 0.24254
  integer 1: 0.2357
  integer 2: 0.22942
  integer 3: 0.22361
  integer 4: 0.21822
  integer 5: 0.2132
  integer 6: 0.20851
  integer 7: 0.20412
  integer 8: 0.2
  integer 9: 0.19612
string vector 'F17': 
  integer 0: 0.2357
  integer 1: 0.22942
  integer 2: 0.22361
  integer 3: 0.21822
  integer 4: 0.2132
  integer 5: 0.20851
  integer 6: 0.20412
  integer 7: 0.2
  integer 8: 0.19612
  integer 9: 0.19245
string vector 'F18': 
  integer 0: 0.22942
  integer 1: 0.22361
  integer 2: 0.21822
  integer 3: 0.2132
  integer 4: 0.20851
  integer 5: 0.20412
  integer 6: 0.2
  integer 7: 0.19612
  integer 8: 0.19245
  integer 9: 0.18898
string vector 'F19': 
  integer 0: 0.22361
  integer 1: 0.21822
  integer 2: 0.2132
  integer 3: 0.20851
  integer 4: 0.20412
  integer 5: 0.2
  integer 6: 0.19612
  integer 7: 0.19245
  integer 8: 0.18898
  integer 9: 0.1857
string vector 'F2': 
  integer 0: 0.57735
  integer 1: 0.5
  integer 2: 0.44721
  integer 3: 0.40825
  integer 4: 0.37796
  integer 5: 0.35355
  integer 6: 0.33333
  integer 7: 0.31623
  integer 8: 0.30151
  integer 9: 0.28868
string vector 'F20': 
  integer 0: 0.21822
  integer 1: 0.2132
  integer 2: 0.20851
  integer 3: 0.20412
  integer 4: 0.2
  integer 5: 0.19612
  integer 6: 0.19245
  integer 7: 0.18898
  integer 8: 0.1857
  integer 9: 0.18257
string vector 'F21': 
  integer 0: 0.2132
  integer 1: 0.20851
  integer 2: 0.20412
  integer 3: 0.2
  integer 4: 0.19612
  integer 5: 0.19245
  integer 6: 0.18898
  integer 7: 0.1857
  integer 8: 0.18257
  integer 9: 0.17961
string vector 'F22': 
  integer 0: 0.20851
  integer 1: 0.20412
  integer 2: 0.2
  integer 3: 0.19612
  integer 4: 0.19245
  integer 5: 0.18898
  integer 6: 0.1857
  integer 7: 0.18257
  integer 8: 0.17961
  integer 9: 0.17678
string vector 'F23': 
  integer 0: 0.20412
  integer 1: 0.2
  integer 2: 0.19612
  integer 3: 0.19245
  integer 4: 0.18898
  integer 5: 0.1857
  integer 6: 0.18257
  integer 7: 0.17961
  integer 8: 0.17678
  integer 9: 0.17408
string vector 'F24': 
  integer 0: 0.2
  integer 1: 0.19612
  integer 2: 0.19245
  integer 3: 0.18898
  integer 4: 0.1857
  integer 5: 0.18257
  integer 6: 0.17961
  integer 7: 0.17678
  integer 8: 0.17408
  integer 9: 0.1715
string vector 'F25': 
  integer 0: 0.19612
  integer 1: 0.19245
  integer 2: 0.18898
  integer 3: 0.1857
  integer 4: 0.18257
  integer 5: 0.17961
  integer 6: 0.17678
  integer 7: 0.17408
  integer 8: 0.1715
  integer 9: 0.16903
string vector 'F26': 
  integer 0: 0.19245
  integer 1: 0.18898
  integer 2: 0.1857
  integer 3: 0.18257
  integer 4: 0.17961
  integer 5: 0.17678
  integer 6: 0.17408
  integer 7: 0.1715
  integer 8: 0.16903
  integer 9: 0.16667
string vector 'F27': 
  integer 0: 0.18898
  integer 1: 0.1857
  integer 2: 0.18257
  integer 3: 0.17961
  integer 4: 0.17678
  integer 5: 0.17408
  integer 6: 0.1715
  integer 7: 0.16903
  integer 8: 0.16667
  integer 9: 0.1644
string vector 'F28': 
  integer 0: 0.1857
  integer 1: 0.18257
  integer 2: 0.17961
  integer 3: 0.17678
  integer 4: 0.17408
  integer 5: 0.1715
  integer 6: 0.16903
  integer 7: 0.16667
  integer 8: 0.1644
  integer 9: 0.16222
string vector 'F29': 
  integer 0: 0.18257
  integer 1: 0.17961
  integer 2: 0.17678
  integer 3: 0.17408
  integer 4: 0.1715
  integer 5: 0.16903
  integer 6: 0.16667
  integer 7: 0.1644
  integer 8: 0.16222
  integer 9: 0.16013
string vector 'F3': 
  integer 0: 0.5
  integer 1: 0.44721
  integer 2: 0.40825
  integer 3: 0.37796
  integer 4: 0.35355
  integer 5: 0.33333
  integer 6: 0.31623
  integer 7: 0.30151
  integer 8: 0.28868
  integer 9: 0.27735
string vector 'F30': 
  integer 0: 0.17961
  integer 1: 0.17678
  integer 2: 0.17408
  integer 3: 0.1715
  integer 4: 0.16903
  integer 5: 0.16667
  integer 6: 0.1644
  integer 7: 0.16222
  integer 8: 0.16013
  integer 9: 0.15811
string vector 'F31': 
  integer 0: 0.17678
  integer 1: 0.17408
  integer 2: 0.1715
  integer 3: 0.16903
  integer 4: 0.16667
  integer 5: 0.1644
  integer 6: 0.16222
  integer 7: 0.16013
  integer 8: 0.15811
  integer 9: 0.15617
string vector 'F32': 
  integer 0: 0.17408
  integer 1: 0.1715
  integer 2: 0.16903
  integer 3: 0.16667
  integer 4: 0.1644
  integer 5: 0.16222
  integer 6: 0.16013
  integer 7: 0.15811
  integer 8: 0.15617
  integer 9: 0.1543
string vector 'F33': 
  integer 0: 0.1715
  integer 1: 0.16903
  integer 2: 0.16667
  integer 3: 0.1644
  integer 4: 0.16222
  integer 5: 0.16013
  integer 6: 0.15811
  integer 7: 0.15617
  integer 8: 0.1543
  integer 9: 0.1525
string vector 'F34': 
  integer 0: 0.16903
  integer 1: 0.16667
  integer 2: 0.1644
  integer 3: 0.16222
  integer 4: 0.16013
  integer 5: 0.15811
  integer 6: 0.15617
  integer 7: 0.1543
  integer 8: 0.1525
  integer 9: 0.15076
string vector 'F35': 
  integer 0: 0.16667
  integer 1: 0.1644
  integer 2: 0.16222
  integer 3: 0.16013
  integer 4: 0.15811
  integer 5: 0.15617
  integer 6: 0.1543
  integer 7: 0.1525
  integer 8: 0.15076
  integer 9: 0.14907
string vector 'F36': 
  integer 0: 0.1644
  integer 1: 0.16222
  integer 2: 0.16013
  integer 3: 0.15811
  integer 4: 0.15617
  integer 5: 0.1543
  integer 6: 0.1525
  integer 7: 0.15076
  integer 8: 0.14907
  integer 9: 0.14744
string vector 'F37': 
  integer 0: 0.16222
  integer 1: 0.16013
  integer 2: 0.15811
  integer 3: 0.15617
  integer 4: 0.1543
  integer 5: 0.1525
  integer 6: 0.15076
  integer 7: 0.14907
  integer 8: 0.14744
  integer 9: 0.14586
string vector 'F38': 
  integer 0: 0.16013
  integer 1: 0.15811
  integer 2: 0.15617
  integer 3: 0.1543
  integer 4: 0.1525
  integer 5: 0.15076
  integer 6: 0.14907
  integer 7: 0.14744
  integer 8: 0.14586
  integer 9: 0.14434
string vector 'F39': 
  integer 0: 0.15811
  integer 1: 0.15617
  integer 2: 0.1543
  integer 3: 0.1525
  integer 4: 0.15076
  integer 5: 0.14907
  integer 6: 0.14744
  integer 7: 0.14586
  integer 8: 0.14434
  integer 9: 0.14286
string vector 'F4': 
  integer 0: 0.44721
  integer 1: 0.40825
  integer 2: 0.37796
  integer 3: 0.35355
  integer 4: 0.33333
  integer 5: 0.31623
  integer 6: 0.30151
  integer 7: 0.28868
  integer 8: 0.27735
  integer 9: 0.26726
string vector 'F40': 
  integer 0: 0.15617
  integer 1: 0.1543
  integer 2: 0.1525
  integer 3: 0.15076
  integer 4: 0.14907
  integer 5: 0.14744
  integer 6: 0.14586
  integer 7: 0.14434
  integer 8: 0.14286
  integer 9: 0.14142
string vector 'F41': 
  integer 0: 0.1543
  integer 1: 0.1525
  integer 2: 0.15076
  integer 3: 0.14907
  integer 4: 0.14744
  integer 5: 0.14586
  integer 6: 0.14434
  integer 7: 0.14286
  integer 8: 0.14142
  integer 9: 0.14003
string vector 'F42': 
  integer 0: 0.1525
  integer 1: 0.15076
  integer 2: 0.14907
  integer 3: 0.14744
  integer 4: 0.14586
  integer 5: 0.14434
  integer 6: 0.14286
  integer 7: 0.14142
  integer 8: 0.14003
  integer 9: 0.13868
string vector 'F43': 
  integer 0: 0.15076
  integer 1: 0.14907
  integer 2: 0.14744
  integer 3: 0.14586
  integer 4: 0.14434
  integer 5: 0.14286
  integer 6: 0.14142
  integer 7: 0.14003
  integer 8: 0.13868
  integer 9: 0.13736
string vector 'F44': 
  integer 0: 0.14907
  integer 1: 0.14744
  integer 2: 0.14586
  integer 3: 0.14434
  integer 4: 0.14286
  integer 5: 0.14142
  integer 6: 0.14003
  integer 7: 0.13868
  integer 8: 0.13736
  integer 9: 0.13608
string vector 'F45': 
  integer 0: 0.14744
  integer 1: 0.14586
  integer 2: 0.14434
  integer 3: 0.14286
  integer 4: 0.14142
  integer 5: 0.14003
  integer 6: 0.13868
  integer 7: 0.13736
  integer 8: 0.13608
  integer 9: 0.13484
string vector 'F46': 
  integer 0: 0.14586
  integer 1: 0.14434
  integer 2: 0.14286
  integer 3: 0.14142
  integer 4: 0.14003
  integer 5: 0.13868
  integer 6: 0.13736
  integer 7: 0.13608
  integer 8: 0.13484
  integer 9: 0.13363
string vector 'F47': 
  integer 0: 0.14434
  integer 1: 0.14286
  integer 2: 0.14142
  integer 3: 0.14003
  integer 4: 0.13868
  integer 5: 0.13736
  integer 6: 0.13608
  integer 7: 0.13484
  integer 8: 0.13363
  integer 9: 0.13245
string vector 'F48': 
  integer 0: 0.14286
  integer 1: 0.14142
  integer 2: 0.14003
  integer 3: 0.13868
  integer 4: 0.13736
  integer 5: 0.13608
  integer 6: 0.13484
  integer 7: 0.13363
  integer 8: 0.13245
  integer 9: 0.13131
string vector 'F49': 
  integer 0: 0.14142
  integer 1: 0.14003
  integer 2: 0.13868
  integer 3: 0.13736
  integer 4: 0.13608
  integer 5: 0.13484
  integer 6: 0.13363
  integer 7: 0.13245
  integer 8: 0.13131
  integer 9: 0.13019
string vector 'F5': 
  integer 0: 0.40825
  integer 1: 0.37796
  integer 2: 0.35355
  integer 3: 0.33333
  integer 4: 0.31623
  integer 5: 0.30151
  integer 6: 0.28868
  integer 7: 0.27735
  integer 8: 0.26726
  integer 9: 0.2582
string vector 'F50': 
  integer 0: 0.14003
  integer 1: 0.13868
  integer 2: 0.13736
  integer 3: 0.13608
  integer 4: 0.13484
  integer 5: 0.13363
  integer 6: 0.13245
  integer 7: 0.13131
  integer 8: 0.13019
  integer 9: 0.1291
string vector 'F51': 
  integer 0: 0.13868
  integer 1: 0.13736
  integer 2: 0.13608
  integer 3: 0.13484
  integer 4: 0.13363
  integer 5: 0.13245
  integer 6: 0.13131
  integer 7: 0.13019
  integer 8: 0.1291
  integer 9: 0.12804
string vector 'F52': 
  integer 0: 0.13736
  integer 1: 0.13608
  integer 2: 0.13484
  integer 3: 0.13363
  integer 4: 0.13245
  integer 5: 0.13131
  integer 6: 0.13019
  integer 7: 0.1291
  integer 8: 0.12804
  integer 9: 0.127
string vector 'F53': 
  integer 0: 0.13608
  integer 1: 0.13484
  integer 2: 0.13363
  integer 3: 0.13245
  integer 4: 0.13131
  integer 5: 0.13019
  integer 6: 0.1291
  integer 7: 0.12804
  integer 8: 0.127
  integer 9: 0.12599
string vector 'F54': 
  integer 0: 0.13484
  integer 1: 0.13363
  integer 2: 0.13245
  integer 3: 0.13131
  integer 4: 0.13019
  integer 5: 0.1291
  integer 6: 0.12804
  integer 7: 0.127
  integer 8: 0.12599
  integer 9: 0.125
string vector 'F55': 
  integer 0: 0.13363
  integer 1: 0.13245
  integer 2: 0.13131
  integer 3: 0.13019
  integer 4: 0.1291
  integer 5: 0.12804
  integer 6: 0.127
  integer 7: 0.12599
  integer 8: 0.125
  integer 9: 0.12403
string vector 'F56': 
  integer 0: 0.13245
  integer 1: 0.13131
  integer 2: 0.13019
  integer 3: 0.1291
  integer 4: 0.12804
  integer 5: 0.127
  integer 6: 0.12599
  integer 7: 0.125
  integer 8: 0.12403
  integer 9: 0.12309
string vector 'F57': 
  integer 0: 0.13131
  integer 1: 0.13019
  integer 2: 0.1291
  integer 3: 0.12804
  integer 4: 0.127
  integer 5: 0.12599
  integer 6: 0.125
  integer 7: 0.12403
  integer 8: 0.12309
  integer 9: 0.12217
string vector 'F58': 
  integer 0: 0.13019
  integer 1: 0.1291
  integer 2: 0.12804
  integer 3: 0.127
  integer 4: 0.12599
  integer 5: 0.125
  integer 6: 0.12403
  integer 7: 0.12309
  integer 8: 0.12217
  integer 9: 0.12127
string vector 'F59': 
  integer 0: 0.1291
  integer 1: 0.12804
  integer 2: 0.127
  integer 3: 0.12599
  integer 4: 0.125
  integer 5: 0.12403
  integer 6: 0.12309
  integer 7: 0.12217
  integer 8: 0.12127
  integer 9: 0.12039
string vector 'F6': 
  integer 0: 0.37796
  integer 1: 0.35355
  integer 2: 0.33333
  integer 3: 0.31623
  integer 4: 0.30151
  integer 5: 0.28868
  integer 6: 0.27735
  integer 7: 0.26726
  integer 8: 0.2582
  integer 9: 0.25
string vector 'F60': 
  integer 0: 0.12804
  integer 1: 0.127
  integer 2: 0.12599
  integer 3: 0.125
  integer 4: 0.12403
  integer 5: 0.12309
  integer 6: 0.12217
  integer 7: 0.12127
  integer 8: 0.12039
  integer 9: 0.11952
string vector 'F61': 
  integer 0: 0.127
  integer 1: 0.12599
  integer 2: 0.125
  integer 3: 0.12403
  integer 4: 0.12309
  integer 5: 0.12217
  integer 6: 0.12127
  integer 7: 0.12039
  integer 8: 0.11952
  integer 9: 0.11868
string vector 'F62': 
  integer 0: 0.12599
  integer 1: 0.125
  integer 2: 0.12403
  integer 3: 0.12309
  integer 4: 0.12217
  integer 5: 0.12127
  integer 6: 0.12039
  integer 7: 0.11952
  integer 8: 0.11868
  integer 9: 0.11785
string vector 'F63': 
  integer 0: 0.125
  integer 1: 0.12403
  integer 2: 0.12309
  integer 3: 0.12217
  integer 4: 0.12127
  integer 5: 0.12039
  integer 6: 0.11952
  integer 7: 0.11868
  integer 8: 0.11785
  integer 9: 0.11704
string vector 'F64': 
  integer 0: 0.12403
  integer 1: 0.12309
  integer 2: 0.12217
  integer 3: 0.12127
  integer 4: 0.12039
  integer 5: 0.11952
  integer 6: 0.11868
  integer 7: 0.11785
  integer 8: 0.11704
  integer 9: 0.11625
string vector 'F65': 
  integer 0: 0.12309
  integer 1: 0.12217
  integer 2: 0.12127
  integer 3: 0.12039
  integer 4: 0.11952
  integer 5: 0.11868
  integer 6: 0.11785
  integer 7: 0.11704
  integer 8: 0.11625
  integer 9: 0.11547
string vector 'F66': 
  integer 0: 0.12217
  integer 1: 0.12127
  integer 2: 0.12039
  integer 3: 0.11952
  integer 4: 0.11868
  integer 5: 0.11785
  integer 6: 0.11704
  integer 7: 0.11625
  integer 8: 0.11547
  integer 9: 0.11471
string vector 'F67': 
  integer 0: 0.12127
  integer 1: 0.12039
  integer 2: 0.11952
  integer 3: 0.11868
  integer 4: 0.11785
  integer 5: 0.11704
  integer 6: 0.11625
  integer 7: 0.11547
  integer 8: 0.11471
  integer 9: 0.11396
string vector 'F68': 
  integer 0: 0.12039
  integer 1: 0.11952
  integer 2: 0.11868
  integer 3: 0.11785
  integer 4: 0.11704
  integer 5: 0.11625
  integer 6: 0.11547
  integer 7: 0.11471
  integer 8: 0.11396
  integer 9: 0.11323
string vector 'F69': 
  integer 0: 0.11952
  integer 1: 0.11868
  integer 2: 0.11785
  integer 3: 0.11704
  integer 4: 0.11625
  integer 5: 0.11547
  integer 6: 0.11471
  integer 7: 0.11396
  integer 8: 0.11323
  integer 9: 0.11251
string vector 'F7': 
  integer 0: 0.35355
  integer 1: 0.33333
  integer 2: 0.31623
  integer 3: 0.30151
  integer 4: 0.28868
  integer 5: 0.27735
  integer 6: 0.26726
  integer 7: 0.2582
  integer 8: 0.25
  integer 9: 0.24254
string vector 'F70': 
  integer 0: 0.11868
  integer 1: 0.11785
  integer 2: 0.11704
  integer 3: 0.11625
  integer 4: 0.11547
  integer 5: 0.11471
  integer 6: 0.11396
  integer 7: 0.11323
  integer 8: 0.11251
  integer 9: 0.1118
string vector 'F71': 
  integer 0: 0.11785
  integer 1: 0.11704
  integer 2: 0.11625
  integer 3: 0.11547
  integer 4: 0.11471
  integer 5: 0.11396
  integer 6: 0.11323
  integer 7: 0.11251
  integer 8: 0.1118
  integer 9: 0.11111
string vector 'F72': 
  integer 0: 0.11704
  integer 1: 0.11625
  integer 2: 0.11547
  integer 3: 0.11471
  integer 4: 0.11396
  integer 5: 0.11323
  integer 6: 0.11251
  integer 7: 0.1118
  integer 8: 0.11111
  integer 9: 0.11043
string vector 'F73': 
  integer 0: 0.11625
  integer 1: 0.11547
  integer 2: 0.11471
  integer 3: 0.11396
  integer 4: 0.11323
  integer 5: 0.11251
  integer 6: 0.1118
  integer 7: 0.11111
  integer 8: 0.11043
  integer 9: 0.10976
string vector 'F74': 
  integer 0: 0.11547
  integer 1: 0.11471
  integer 2: 0.11396
  integer 3: 0.11323
  integer 4: 0.11251
  integer 5: 0.1118
  integer 6: 0.11111
  integer 7: 0.11043
  integer 8: 0.10976
  integer 9: 0.10911
string vector 'F75': 
  integer 0: 0.11471
  integer 1: 0.11396
  integer 2: 0.11323
  integer 3: 0.11251
  integer 4: 0.1118
  integer 5: 0.11111
  integer 6: 0.11043
  integer 7: 0.10976
  integer 8: 0.10911
  integer 9: 0.10847
string vector 'F76': 
  integer 0: 0.11396
  integer 1: 0.11323
  integer 2: 0.11251
  integer 3: 0.1118
  integer 4: 0.11111
  integer 5: 0.11043
  integer 6: 0.10976
  integer 7: 0.10911
  integer 8: 0.10847
  integer 9: 0.10783
string vector 'F77': 
  integer 0: 0.11323
  integer 1: 0.11251
  integer 2: 0.1118
  integer 3: 0.11111
  integer 4: 0.11043
  integer 5: 0.10976
  integer 6: 0.10911
  integer 7: 0.10847
  integer 8: 0.10783
  integer 9: 0.10721
string vector 'F78': 
  integer 0: 0.11251
  integer 1: 0.1118
  integer 2: 0.11111
  integer 3: 0.11043
  integer 4: 0.10976
  integer 5: 0.10911
  integer 6: 0.10847
  integer 7: 0.10783
  integer 8: 0.10721
  integer 9: 0.1066
string vector 'F79': 
  integer 0: 0.1118
  integer 1: 0.11111
  integer 2: 0.11043
  integer 3: 0.10976
  integer 4: 0.10911
  integer 5: 0.10847
  integer 6: 0.10783
  integer 7: 0.10721
  integer 8: 0.1066
  integer 9: 0.106
string vector 'F8': 
  integer 0: 0.33333
  integer 1: 0.31623
  integer 2: 0.30151
  integer 3: 0.28868
  integer 4: 0.27735
  integer 5: 0.26726
  integer 6: 0.2582
  integer 7: 0.25
  integer 8: 0.24254
  integer 9: 0.2357
string vector 'F80': 
  integer 0: 0.11111
  integer 1: 0.11043
  integer 2: 0.10976
  integer 3: 0.10911
  integer 4: 0.10847
  integer 5: 0.10783
  integer 6: 0.10721
  integer 7: 0.1066
  integer 8: 0.106
  integer 9: 0.10541
string vector 'F81': 
  integer 0: 0.11043
  integer 1: 0.10976
  integer 2: 0.10911
  integer 3: 0.10847
  integer 4: 0.10783
  integer 5: 0.10721
  integer 6: 0.1066
  integer 7: 0.106
  integer 8: 0.10541
  integer 9: 0.10483
string vector 'F82': 
  integer 0: 0.10976
  integer 1: 0.10911
  integer 2: 0.10847
  integer 3: 0.10783
  integer 4: 0.10721
  integer 5: 0.1066
  integer 6: 0.106
  integer 7: 0.10541
  integer 8: 0.10483
  integer 9: 0.10426
string vector 'F83': 
  integer 0: 0.10911
  integer 1: 0.10847
  integer 2: 0.10783
  integer 3: 0.10721
  integer 4: 0.1066
  integer 5: 0.106
  integer 6: 0.10541
  integer 7: 0.10483
  integer 8: 0.10426
  integer 9: 0.1037
string vector 'F84': 
  integer 0: 0.10847
  integer 1: 0.10783
  integer 2: 0.10721
  integer 3: 0.1066
  integer 4: 0.106
  integer 5: 0.10541
  integer 6: 0.10483
  integer 7: 0.10426
  integer 8: 0.1037
  integer 9: 0.10314
string vector 'F85': 
  integer 0: 0.10783
  integer 1: 0.10721
  integer 2: 0.1066
  integer 3: 0.106
  integer 4: 0.10541
  integer 5: 0.10483
  integer 6: 0.10426
  integer 7: 0.1037
  integer 8: 0.10314
  integer 9: 0.1026
string vector 'F86': 
  integer 0: 0.10721
  integer 1: 0.1066
  integer 2: 0.106
  integer 3: 0.10541
  integer 4: 0.10483
  integer 5: 0.10426
  integer 6: 0.1037
  integer 7: 0.10314
  integer 8: 0.1026
  integer 9: 0.10206
string vector 'F87': 
  integer 0: 0.1066
  integer 1: 0.106
  integer 2: 0.10541
  integer 3: 0.10483
  integer 4: 0.10426
  integer 5: 0.1037
  integer 6: 0.10314
  integer 7: 0.1026
  integer 8: 0.10206
  integer 9: 0.10153
string vector 'F88': 
  integer 0: 0.106
  integer 1: 0.10541
  integer 2: 0.10483
  integer 3: 0.10426
  integer 4: 0.1037
  integer 5: 0.10314
  integer 6: 0.1026
  integer 7: 0.10206
  integer 8: 0.10153
  integer 9: 0.10102
string vector 'F89': 
  integer 0: 0.10541
  integer 1: 0.10483
  integer 2: 0.10426
  integer 3: 0.1037
  integer 4: 0.10314
  integer 5: 0.1026
  integer 6: 0.10206
  integer 7: 0.10153
  integer 8: 0.10102
  integer 9: 0.1005
string vector 'F9': 
  integer 0: 0.31623
  integer 1: 0.30151
  integer 2: 0.28868
  integer 3: 0.27735
  integer 4: 0.26726
  integer 5: 0.2582
  integer 6: 0.25
  integer 7: 0.24254
  integer 8: 0.2357
  integer 9: 0.22942
string vector 'F90': 
  integer 0: 0.10483
  integer 1: 0.10426
  integer 2: 0.1037
  integer 3: 0.10314
  integer 4: 0.1026
  integer 5: 0.10206
  integer 6: 0.10153
  integer 7: 0.10102
  integer 8: 0.1005
  integer 9: 0.1
string vector 'F91': 
  integer 0: 0.10426
  integer 1: 0.1037
  integer 2: 0.10314
  integer 3: 0.1026
  integer 4: 0.10206
  integer 5: 0.10153
  integer 6: 0.10102
  integer 7: 0.1005
  integer 8: 0.1
  integer 9: 0.0995
string vector 'F92': 
  integer 0: 0.1037
  integer 1: 0.10314
  integer 2: 0.1026
  integer 3: 0.10206
  integer 4: 0.10153
  integer 5: 0.10102
  integer 6: 0.1005
  integer 7: 0.1
  integer 8: 0.0995
  integer 9: 0.09901
string vector 'F93': 
  integer 0: 0.10314
  integer 1: 0.1026
  integer 2: 0.10206
  integer 3: 0.10153
  integer 4: 0.10102
  integer 5: 0.1005
  integer 6: 0.1
  integer 7: 0.0995
  integer 8: 0.09901
  integer 9: 0.09853
string vector 'F94': 
  integer 0: 0.1026
  integer 1: 0.10206
  integer 2: 0.10153
  integer 3: 0.10102
  integer 4: 0.1005
  integer 5: 0.1
  integer 6: 0.0995
  integer 7: 0.09901
  integer 8: 0.09853
  integer 9: 0.09806
string vector 'F95': 
  integer 0: 0.10206
  integer 1: 0.10153
  integer 2: 0.10102
  integer 3: 0.1005
  integer 4: 0.1
  integer 5: 0.0995
  integer 6: 0.09901
  integer 7: 0.09853
  integer 8: 0.09806
  integer 9: 0.09759
string vector 'F96': 
  integer 0: 0.10153
  integer 1: 0.10102
  integer 2: 0.1005
  integer 3: 0.1
  integer 4: 0.0995
  integer 5: 0.09901
  integer 6: 0.09853
  integer 7: 0.09806
  integer 8: 0.09759
  integer 9: 0.09713
string vector 'F97': 
  integer 0: 0.10102
  integer 1: 0.1005
  integer 2: 0.1
  integer 3: 0.0995
  integer 4: 0.09901
  integer 5: 0.09853
  integer 6: 0.09806
  integer 7: 0.09759
  integer 8: 0.09713
  integer 9: 0.09667
string vector 'F98': 
  integer 0: 0.1005
  integer 1: 0.1
  integer 2: 0.0995
  integer 3: 0.09901
  integer 4: 0.09853
  integer 5: 0.09806
  integer 6: 0.09759
  integer 7: 0.09713
  integer 8: 0.09667
  integer 9: 0.09623
string vector 'F99': 
  integer 0: 0.1
  integer 1: 0.0995
  integer 2: 0.09901
  integer 3: 0.09853
  integer 4: 0.09806
  integer 5: 0.09759
  integer 6: 0.09713
  integer 7: 0.09667
  integer 8: 0.09623
  integer 9: 0.09578
END_expected;

verifyTestOutput( $outputdir, $result, $expected);

?>
