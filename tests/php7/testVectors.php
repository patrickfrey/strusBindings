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
	'vecdim' => 10,
	'simdist' => 20,
	'probsimdist' => 40
];
$vectors = [];

$ctx = new StrusContext();
$ctx->loadModule( "storage_vector_std");

if ($ctx->storageExists( $config)) {
	$ctx->destroyStorage( $config);
}

srand( 123);
$examplevec = [0.1,0.1,0.1,0.1,0.1, 0.1,0.1,0.1,0.1,0.1];
for ($vi = 1; $vi <= 100; $vi++) {
	$vv = [];
	for ($xi = 1; $xi <= 10; $xi++) {
		$r = floatval( rand( 0, 9999)) / 10000.0;
		array_push( $vv, $examplevec[ $xi-1] - $r / 8.0);
	}
	array_push( $vectors, $vv);
}

$ctx->createVectorStorage( $config);
$storage = $ctx->createVectorStorageClient( $config);
$transaction = $storage->createTransaction();

foreach ($vectors as $iv => $vv) {
	$fidx = (int)$iv + 1;
	$transaction->defineVector( "word", "F$fidx", $vv);
	if ($fidx % 2 == 1) {
		$transaction->defineFeature( "nonvec", "F$fidx");
	}
}
$transaction->commit();
$transaction->close();

$output = dumpVectorStorage( $ctx, $config, $vectors, $examplevec);

$storage = $ctx->createVectorStorageClient( $config);
$searcher = $storage->createSearcher( "word", 0, 1);
$simlist = $searcher->findSimilar( $examplevec, 10, 0.85, True);
$output[ 'simlist 0.1x10'] = $simlist;


$result = "vector storage dump:" . dumpTree( $output);
$expected = <<<END_expected
vector storage dump:
string config: 
  string path: "storage"
  string probsimdist: 40
  string simdist: 20
  string vecdim: 10
string nof vec nonvec: 0
string nof vec word: 100
string rank 1: "0.93067 F72 0.04617, 0.04461, 0.06265, 0.05407, 0.02148, 0.08911, 0.0493, 0.02789, 0.05985, 0.09084"
string rank 2: "0.92193 F99 0.09578, 0.08003, 0.08905, 0.05257, 0.08979, 0.0774, 0.00829, 0.08126, 0.07861, 0.02279"
string rank 3: "0.90712 F56 0.0659, 0.0824, 0.02909, 0.01525, 0.09679, 0.08474, 0.02225, 0.05726, 0.05545, 0.0873"
string rank 4: "0.89976 F26 0.06899, 0.06983, 0.04096, 0.06066, 0.05447, 0.03385, 0.0485, -0.00174, 0.01908, 0.04396"
string rank 5: "0.89383 F42 0.08711, 0.06851, 0.03634, 0.02676, 0.04412, 0.09243, 0.04544, 0.09176, 0.09294, 0.00739"
string simlist 0.1x10: 
  integer 0: 
    string value: "F72"
    string weight: 0.93067
  integer 1: 
    string value: "F99"
    string weight: 0.92193
  integer 2: 
    string value: "F56"
    string weight: 0.90712
  integer 3: 
    string value: "F26"
    string weight: 0.89976
  integer 4: 
    string value: "F42"
    string weight: 0.89383
  integer 5: 
    string value: "F94"
    string weight: 0.87256
  integer 6: 
    string value: "F8"
    string weight: 0.86347
  integer 7: 
    string value: "F10"
    string weight: 0.85896
  integer 8: 
    string value: "F30"
    string weight: 0.85214
  integer 9: 
    string value: "F50"
    string weight: 0.84614
string types: 
  integer 0: "nonvec"
  integer 1: "word"
string types F1: 
  integer 0: "word"
  integer 1: "nonvec"
string types F10: 
  integer 0: "word"
string types F100: 
  integer 0: "word"
string types F11: 
  integer 0: "word"
  integer 1: "nonvec"
string types F12: 
  integer 0: "word"
string types F13: 
  integer 0: "word"
  integer 1: "nonvec"
string types F14: 
  integer 0: "word"
string types F15: 
  integer 0: "word"
  integer 1: "nonvec"
string types F16: 
  integer 0: "word"
string types F17: 
  integer 0: "word"
  integer 1: "nonvec"
string types F18: 
  integer 0: "word"
string types F19: 
  integer 0: "word"
  integer 1: "nonvec"
string types F2: 
  integer 0: "word"
string types F20: 
  integer 0: "word"
string types F21: 
  integer 0: "word"
  integer 1: "nonvec"
string types F22: 
  integer 0: "word"
string types F23: 
  integer 0: "word"
  integer 1: "nonvec"
string types F24: 
  integer 0: "word"
string types F25: 
  integer 0: "word"
  integer 1: "nonvec"
string types F26: 
  integer 0: "word"
string types F27: 
  integer 0: "word"
  integer 1: "nonvec"
string types F28: 
  integer 0: "word"
string types F29: 
  integer 0: "word"
  integer 1: "nonvec"
string types F3: 
  integer 0: "word"
  integer 1: "nonvec"
string types F30: 
  integer 0: "word"
string types F31: 
  integer 0: "word"
  integer 1: "nonvec"
string types F32: 
  integer 0: "word"
string types F33: 
  integer 0: "word"
  integer 1: "nonvec"
string types F34: 
  integer 0: "word"
string types F35: 
  integer 0: "word"
  integer 1: "nonvec"
string types F36: 
  integer 0: "word"
string types F37: 
  integer 0: "word"
  integer 1: "nonvec"
string types F38: 
  integer 0: "word"
string types F39: 
  integer 0: "word"
  integer 1: "nonvec"
string types F4: 
  integer 0: "word"
string types F40: 
  integer 0: "word"
string types F41: 
  integer 0: "word"
  integer 1: "nonvec"
string types F42: 
  integer 0: "word"
string types F43: 
  integer 0: "word"
  integer 1: "nonvec"
string types F44: 
  integer 0: "word"
string types F45: 
  integer 0: "word"
  integer 1: "nonvec"
string types F46: 
  integer 0: "word"
string types F47: 
  integer 0: "word"
  integer 1: "nonvec"
string types F48: 
  integer 0: "word"
string types F49: 
  integer 0: "word"
  integer 1: "nonvec"
string types F5: 
  integer 0: "word"
  integer 1: "nonvec"
string types F50: 
  integer 0: "word"
string types F51: 
  integer 0: "word"
  integer 1: "nonvec"
string types F52: 
  integer 0: "word"
string types F53: 
  integer 0: "word"
  integer 1: "nonvec"
string types F54: 
  integer 0: "word"
string types F55: 
  integer 0: "word"
  integer 1: "nonvec"
string types F56: 
  integer 0: "word"
string types F57: 
  integer 0: "word"
  integer 1: "nonvec"
string types F58: 
  integer 0: "word"
string types F59: 
  integer 0: "word"
  integer 1: "nonvec"
string types F6: 
  integer 0: "word"
string types F60: 
  integer 0: "word"
string types F61: 
  integer 0: "word"
  integer 1: "nonvec"
string types F62: 
  integer 0: "word"
string types F63: 
  integer 0: "word"
  integer 1: "nonvec"
string types F64: 
  integer 0: "word"
string types F65: 
  integer 0: "word"
  integer 1: "nonvec"
string types F66: 
  integer 0: "word"
string types F67: 
  integer 0: "word"
  integer 1: "nonvec"
string types F68: 
  integer 0: "word"
string types F69: 
  integer 0: "word"
  integer 1: "nonvec"
string types F7: 
  integer 0: "word"
  integer 1: "nonvec"
string types F70: 
  integer 0: "word"
string types F71: 
  integer 0: "word"
  integer 1: "nonvec"
string types F72: 
  integer 0: "word"
string types F73: 
  integer 0: "word"
  integer 1: "nonvec"
string types F74: 
  integer 0: "word"
string types F75: 
  integer 0: "word"
  integer 1: "nonvec"
string types F76: 
  integer 0: "word"
string types F77: 
  integer 0: "word"
  integer 1: "nonvec"
string types F78: 
  integer 0: "word"
string types F79: 
  integer 0: "word"
  integer 1: "nonvec"
string types F8: 
  integer 0: "word"
string types F80: 
  integer 0: "word"
string types F81: 
  integer 0: "word"
  integer 1: "nonvec"
string types F82: 
  integer 0: "word"
string types F83: 
  integer 0: "word"
  integer 1: "nonvec"
string types F84: 
  integer 0: "word"
string types F85: 
  integer 0: "word"
  integer 1: "nonvec"
string types F86: 
  integer 0: "word"
string types F87: 
  integer 0: "word"
  integer 1: "nonvec"
string types F88: 
  integer 0: "word"
string types F89: 
  integer 0: "word"
  integer 1: "nonvec"
string types F9: 
  integer 0: "word"
  integer 1: "nonvec"
string types F90: 
  integer 0: "word"
string types F91: 
  integer 0: "word"
  integer 1: "nonvec"
string types F92: 
  integer 0: "word"
string types F93: 
  integer 0: "word"
  integer 1: "nonvec"
string types F94: 
  integer 0: "word"
string types F95: 
  integer 0: "word"
  integer 1: "nonvec"
string types F96: 
  integer 0: "word"
string types F97: 
  integer 0: "word"
  integer 1: "nonvec"
string types F98: 
  integer 0: "word"
string types F99: 
  integer 0: "word"
  integer 1: "nonvec"
string vec F1: 
  integer 0: 0.07023
  integer 1: -0.02236
  integer 2: -0.01378
  integer 3: -0.00763
  integer 4: -0.01975
  integer 5: 0.00197
  integer 6: -0.01108
  integer 7: 0.01179
  integer 8: 0.02876
  integer 9: -0.02193
string vec F1 example sim: 0.05873
string vec F10: 
  integer 0: 0.00491
  integer 1: 0.02159
  integer 2: 0.02046
  integer 3: 0.08727
  integer 4: 0.01194
  integer 5: 0.06611
  integer 6: 0.07989
  integer 7: 0.07177
  integer 8: 0.07575
  integer 9: 0.0675
string vec F10 example sim: 0.85896
string vec F100: 
  integer 0: 0.05619
  integer 1: 0.06584
  integer 2: 0.05746
  integer 3: 0.00912
  integer 4: 0.04781
  integer 5: 0.08313
  integer 6: 0.04796
  integer 7: 0.08624
  integer 8: -0.01338
  integer 9: -0.01831
string vec F100 example sim: 0.76645
string vec F11: 
  integer 0: 0.02279
  integer 1: 0.02565
  integer 2: 0.01617
  integer 3: 0.00719
  integer 4: 0.05599
  integer 5: -0.02324
  integer 6: -0.01575
  integer 7: 0.01034
  integer 8: -0.01831
  integer 9: 0.0062
string vec F11 example sim: 0.35847
string vec F12: 
  integer 0: -0.0107
  integer 1: 0.00486
  integer 2: 0.01559
  integer 3: 0.09093
  integer 4: 0.04396
  integer 5: 0.06696
  integer 6: -0.00634
  integer 7: -0.00826
  integer 8: 0.00929
  integer 9: 0.00811
string vec F12 example sim: 0.54767
string vec F13: 
  integer 0: 0.08989
  integer 1: -0.01782
  integer 2: -0.00384
  integer 3: 0.04526
  integer 4: 0.09876
  integer 5: 0.07956
  integer 6: 0.0073
  integer 7: -0.01257
  integer 8: 0.05973
  integer 9: -0.02311
string vec F13 example sim: 0.58173
string vec F14: 
  integer 0: -0.02209
  integer 1: 0.09283
  integer 2: 0.06493
  integer 3: 0.04456
  integer 4: -0.01411
  integer 5: -0.00548
  integer 6: 0.08425
  integer 7: 0.07593
  integer 8: 0.05086
  integer 9: 0.05733
string vec F14 example sim: 0.73282
string vec F15: 
  integer 0: 0.07339
  integer 1: -0.01396
  integer 2: -0.02217
  integer 3: 0.04987
  integer 4: 0.05256
  integer 5: -0.01125
  integer 6: -0.01964
  integer 7: 0.06639
  integer 8: 0.04481
  integer 9: 0.05424
string vec F15 example sim: 0.59572
string vec F16: 
  integer 0: 0.0314
  integer 1: -0.01906
  integer 2: 0.0477
  integer 3: 0.02544
  integer 4: 0.01892
  integer 5: 0.00483
  integer 6: 0.06441
  integer 7: 0.05466
  integer 8: 0.0177
  integer 9: 0.02969
string vec F16 example sim: 0.76507
string vec F17: 
  integer 0: 0.08284
  integer 1: 0.09659
  integer 2: -0.022
  integer 3: -0.02244
  integer 4: 0.00064
  integer 5: 0.00219
  integer 6: 0.09996
  integer 7: 0.09915
  integer 8: 0.05263
  integer 9: 0.09671
string vec F17 example sim: 0.69374
string vec F18: 
  integer 0: 0.03604
  integer 1: 0.04352
  integer 2: 0.08905
  integer 3: 0.05759
  integer 4: 0.05095
  integer 5: 0.07102
  integer 6: -0.01135
  integer 7: 0.00056
  integer 8: 0.04405
  integer 9: 0.05916
string vec F18 example sim: 0.83792
string vec F19: 
  integer 0: 0.03611
  integer 1: 0.09813
  integer 2: 0.0731
  integer 3: -0.00187
  integer 4: 0.06624
  integer 5: 0.02685
  integer 6: 0.00085
  integer 7: 0.02061
  integer 8: 0.03155
  integer 9: -0.01425
string vec F19 example sim: 0.70316
string vec F2: 
  integer 0: -0.00514
  integer 1: 0.05349
  integer 2: 0.09792
  integer 3: 0.01599
  integer 4: 0.0836
  integer 5: -0.00061
  integer 6: 0.01483
  integer 7: 0.05061
  integer 8: 0.07609
  integer 9: 0.009
string vec F2 example sim: 0.74301
string vec F20: 
  integer 0: 0.01751
  integer 1: 0.0248
  integer 2: 0.0068
  integer 3: 0.04749
  integer 4: 0.0923
  integer 5: 0.0729
  integer 6: -0.00265
  integer 7: -0.01592
  integer 8: 0.07061
  integer 9: 0.05407
string vec F20 example sim: 0.7325
string vec F21: 
  integer 0: -0.00816
  integer 1: 0.05044
  integer 2: 0.0921
  integer 3: -0.02177
  integer 4: 0.05652
  integer 5: 0.03539
  integer 6: -0.01918
  integer 7: -0.00728
  integer 8: 0.0229
  integer 9: -0.01229
string vec F21 example sim: 0.45612
string vec F22: 
  integer 0: 0.01008
  integer 1: 0.09705
  integer 2: 0.01805
  integer 3: -0.02131
  integer 4: 0.07267
  integer 5: 0.07626
  integer 6: 0.03139
  integer 7: -0.02469
  integer 8: 0.02744
  integer 9: 0.00465
string vec F22 example sim: 0.59811
string vec F23: 
  integer 0: 0.05426
  integer 1: 0.06739
  integer 2: 0.03511
  integer 3: -0.01251
  integer 4: 0.08923
  integer 5: 0.0509
  integer 6: 0.07911
  integer 7: 0.00661
  integer 8: 0.09759
  integer 9: 0.02796
string vec F23 example sim: 0.82569
string vec F24: 
  integer 0: 0.08239
  integer 1: 0.0661
  integer 2: -0.02038
  integer 3: 0.03071
  integer 4: 0.00714
  integer 5: -0.01981
  integer 6: 0.03577
  integer 7: 0.02794
  integer 8: 0.08457
  integer 9: 0.02397
string vec F24 example sim: 0.66762
string vec F25: 
  integer 0: 0.06922
  integer 1: 0.09584
  integer 2: -0.01551
  integer 3: 0.08476
  integer 4: 0.06917
  integer 5: -0.02074
  integer 6: 0.01743
  integer 7: -0.01608
  integer 8: -0.02329
  integer 9: -0.02471
string vec F25 example sim: 0.44356
string vec F26: 
  integer 0: 0.06899
  integer 1: 0.06983
  integer 2: 0.04096
  integer 3: 0.06066
  integer 4: 0.05447
  integer 5: 0.03385
  integer 6: 0.0485
  integer 7: -0.00174
  integer 8: 0.01908
  integer 9: 0.04396
string vec F26 example sim: 0.89976
string vec F27: 
  integer 0: 0.04044
  integer 1: 0.00428
  integer 2: 0.02287
  integer 3: 0.03266
  integer 4: 0.09131
  integer 5: 0.01129
  integer 6: -0.01934
  integer 7: 0.02889
  integer 8: 0.06083
  integer 9: 0.04966
string vec F27 example sim: 0.73913
string vec F28: 
  integer 0: 0.05656
  integer 1: 0.03536
  integer 2: 0.00563
  integer 3: 0.07234
  integer 4: 0.03886
  integer 5: -0.00328
  integer 6: -0.0122
  integer 7: 0.0801
  integer 8: 0.01691
  integer 9: -0.02349
string vec F28 example sim: 0.618
string vec F29: 
  integer 0: 0.03535
  integer 1: 0.07135
  integer 2: 0.00874
  integer 3: 0.0158
  integer 4: 0.0501
  integer 5: 0.07845
  integer 6: 0.06969
  integer 7: 0.04138
  integer 8: -0.01966
  integer 9: 0.08059
string vec F29 example sim: 0.80484
string vec F3: 
  integer 0: -0.00438
  integer 1: 0.0248
  integer 2: 0.04181
  integer 3: 0.00629
  integer 4: 0.05676
  integer 5: 0.06123
  integer 6: 0.04224
  integer 7: 0.09055
  integer 8: 0.012
  integer 9: 0.0828
string vec F3 example sim: 0.80631
string vec F30: 
  integer 0: -0.01846
  integer 1: 0.04845
  integer 2: 0.05409
  integer 3: 0.07461
  integer 4: 0.03286
  integer 5: 0.08125
  integer 6: 0.07338
  integer 7: 0.01247
  integer 8: 0.06595
  integer 9: 0.07476
string vec F30 example sim: 0.85214
string vec F31: 
  integer 0: 0.0028
  integer 1: -0.02164
  integer 2: 0.09251
  integer 3: 0.01766
  integer 4: 0.03251
  integer 5: 0.09858
  integer 6: 0.00574
  integer 7: 0.06179
  integer 8: 0.02191
  integer 9: 0.01114
string vec F31 example sim: 0.65156
string vec F32: 
  integer 0: 0.02809
  integer 1: -0.02381
  integer 2: 0.03207
  integer 3: 0.05697
  integer 4: 0.02644
  integer 5: 0.08507
  integer 6: 0.0485
  integer 7: 0.02794
  integer 8: 0.0522
  integer 9: -0.01171
string vec F32 example sim: 0.72749
string vec F33: 
  integer 0: 0.01936
  integer 1: 0.01996
  integer 2: 0.06616
  integer 3: -0.01921
  integer 4: 0.05131
  integer 5: -0.02427
  integer 6: 0.09276
  integer 7: 0.01859
  integer 8: 0.05931
  integer 9: -0.01202
string vec F33 example sim: 0.58849
string vec F34: 
  integer 0: 0.01165
  integer 1: 0.01053
  integer 2: -0.01366
  integer 3: 0.08249
  integer 4: 0.0865
  integer 5: 0.08709
  integer 6: 0.03795
  integer 7: 0.08528
  integer 8: 0.03291
  integer 9: 0.0865
string vec F34 example sim: 0.80622
string vec F35: 
  integer 0: 0.02054
  integer 1: 0.01049
  integer 2: 0.07258
  integer 3: 0.05235
  integer 4: 0.00025
  integer 5: 0.07105
  integer 6: 0.05896
  integer 7: 0.06776
  integer 8: -0.01261
  integer 9: 0.03331
string vec F35 example sim: 0.78253
string vec F36: 
  integer 0: 0.01481
  integer 1: 0.09065
  integer 2: 0.06484
  integer 3: 0.0563
  integer 4: 0.09554
  integer 5: 0.00051
  integer 6: -0.00649
  integer 7: 0.05043
  integer 8: 0.04902
  integer 9: 0.02211
string vec F36 example sim: 0.7937
string vec F37: 
  integer 0: -0.01957
  integer 1: 0.04065
  integer 2: 0.07434
  integer 3: 0.0816
  integer 4: 0.0466
  integer 5: -0.01378
  integer 6: 0.0257
  integer 7: -0.01538
  integer 8: 0.08361
  integer 9: 0.06959
string vec F37 example sim: 0.68962
string vec F38: 
  integer 0: -0.0022
  integer 1: 0.06921
  integer 2: 0.02442
  integer 3: 0.07023
  integer 4: 0.06595
  integer 5: 0.01086
  integer 6: -0.00119
  integer 7: 0.03607
  integer 8: 0.04294
  integer 9: 0.05955
string vec F38 example sim: 0.81089
string vec F39: 
  integer 0: 0.04474
  integer 1: 0.0888
  integer 2: 0.03431
  integer 3: -0.00681
  integer 4: 0.07738
  integer 5: -0.00112
  integer 6: 0.01017
  integer 7: 0.02911
  integer 8: 0.00927
  integer 9: 0.06951
string vec F39 example sim: 0.7415
string vec F4: 
  integer 0: 0.02355
  integer 1: 0.02419
  integer 2: 0.02471
  integer 3: 0.09776
  integer 4: -0.01942
  integer 5: 0.06855
  integer 6: 0.02811
  integer 7: 0.03977
  integer 8: 0.07375
  integer 9: -0.02119
string vec F4 example sim: 0.68446
string vec F40: 
  integer 0: 0.08456
  integer 1: -0.01572
  integer 2: -0.01328
  integer 3: 0.00519
  integer 4: -0.0069
  integer 5: 0.08715
  integer 6: 0.02659
  integer 7: 0.03364
  integer 8: 0.08793
  integer 9: -0.02258
string vec F40 example sim: 0.52974
string vec F41: 
  integer 0: 0.05534
  integer 1: 0.01269
  integer 2: 0.04927
  integer 3: 0.03061
  integer 4: 0.0908
  integer 5: 0.03056
  integer 6: -0.00241
  integer 7: 0.01616
  integer 8: 0.01983
  integer 9: 0.08171
string vec F41 example sim: 0.80006
string vec F42: 
  integer 0: 0.08711
  integer 1: 0.06851
  integer 2: 0.03634
  integer 3: 0.02676
  integer 4: 0.04412
  integer 5: 0.09243
  integer 6: 0.04544
  integer 7: 0.09176
  integer 8: 0.09294
  integer 9: 0.00739
string vec F42 example sim: 0.89383
string vec F43: 
  integer 0: -0.01986
  integer 1: 0.03989
  integer 2: 0.08999
  integer 3: -0.01284
  integer 4: 0.00224
  integer 5: -0.01831
  integer 6: -0.00141
  integer 7: 0.09534
  integer 8: 0.02024
  integer 9: 0.05452
string vec F43 example sim: 0.52014
string vec F44: 
  integer 0: 0.07348
  integer 1: 0.01504
  integer 2: -0.02421
  integer 3: 0.08314
  integer 4: 0.048
  integer 5: -0.01498
  integer 6: 0.01791
  integer 7: 0.0456
  integer 8: 0.03122
  integer 9: 0.01365
string vec F44 example sim: 0.65899
string vec F45: 
  integer 0: 0.08147
  integer 1: 0.05616
  integer 2: 0.0892
  integer 3: 0.01445
  integer 4: -0.00568
  integer 5: 0.02598
  integer 6: 0.03911
  integer 7: 0.08488
  integer 8: 0.01154
  integer 9: 0.07542
string vec F45 example sim: 0.81985
string vec F46: 
  integer 0: 0.03999
  integer 1: 0.003
  integer 2: 0.059
  integer 3: -0.01202
  integer 4: 0.06593
  integer 5: 0.07354
  integer 6: 0.00161
  integer 7: 0.05923
  integer 8: 0.07764
  integer 9: 0.09342
string vec F46 example sim: 0.8
string vec F47: 
  integer 0: 0.07584
  integer 1: 0.09645
  integer 2: 0.01649
  integer 3: -0.00681
  integer 4: 0.03054
  integer 5: 0.09023
  integer 6: 0.02116
  integer 7: -0.01792
  integer 8: 0.06912
  integer 9: -0.00545
string vec F47 example sim: 0.67461
string vec F48: 
  integer 0: 0.08847
  integer 1: -0.00749
  integer 2: 0.07764
  integer 3: 0.09059
  integer 4: -0.00577
  integer 5: 0.07593
  integer 6: 0.0306
  integer 7: 0.03176
  integer 8: -0.00261
  integer 9: -0.01644
string vec F48 example sim: 0.66064
string vec F49: 
  integer 0: 0.0082
  integer 1: -0.0191
  integer 2: 0.09742
  integer 3: 0.07463
  integer 4: -0.00916
  integer 5: 0.05663
  integer 6: -0.01966
  integer 7: -0.01381
  integer 8: 0.02835
  integer 9: 0.04276
string vec F49 example sim: 0.52509
string vec F5: 
  integer 0: 0.0827
  integer 1: 0.00624
  integer 2: 0.0778
  integer 3: -9.0E-5
  integer 4: 0.07609
  integer 5: -0.00964
  integer 6: 0.02485
  integer 7: 0.04985
  integer 8: 0.09115
  integer 9: 0.06152
string vec F5 example sim: 0.7896
string vec F50: 
  integer 0: 0.06238
  integer 1: 0.04197
  integer 2: 0.06644
  integer 3: 0.0428
  integer 4: 0.08363
  integer 5: 0.04098
  integer 6: 0.01206
  integer 7: 0.02507
  integer 8: -0.01077
  integer 9: 0.04763
string vec F50 example sim: 0.84614
string vec F51: 
  integer 0: -0.0202
  integer 1: 0.08057
  integer 2: -0.02229
  integer 3: 0.08449
  integer 4: 0.0492
  integer 5: 0.06063
  integer 6: -0.01284
  integer 7: 0.03261
  integer 8: -0.00771
  integer 9: 0.01506
string vec F51 example sim: 0.55148
string vec F52: 
  integer 0: 0.01991
  integer 1: 0.07921
  integer 2: 0.0808
  integer 3: 0.02387
  integer 4: 0.07806
  integer 5: 0.02743
  integer 6: 0.03641
  integer 7: -0.00486
  integer 8: 0.07783
  integer 9: -0.00214
string vec F52 example sim: 0.78679
string vec F53: 
  integer 0: -0.01205
  integer 1: 0.06761
  integer 2: -0.02087
  integer 3: 0.0328
  integer 4: 0.03271
  integer 5: -0.02356
  integer 6: -0.00336
  integer 7: 0.0093
  integer 8: 0.0663
  integer 9: 0.00061
string vec F53 example sim: 0.42542
string vec F54: 
  integer 0: 0.04786
  integer 1: 0.01619
  integer 2: 0.0182
  integer 3: 0.02647
  integer 4: 0.06179
  integer 5: -0.01989
  integer 6: 0.09134
  integer 7: 0.04236
  integer 8: -0.00999
  integer 9: 0.0778
string vec F54 example sim: 0.71748
string vec F55: 
  integer 0: 0.04741
  integer 1: 0.09475
  integer 2: 0.0145
  integer 3: 0.09195
  integer 4: 0.04609
  integer 5: 0.09943
  integer 6: 0.07876
  integer 7: 0.02494
  integer 8: 0.06459
  integer 9: -0.00817
string vec F55 example sim: 0.84556
string vec F56: 
  integer 0: 0.0659
  integer 1: 0.0824
  integer 2: 0.02909
  integer 3: 0.01525
  integer 4: 0.09679
  integer 5: 0.08474
  integer 6: 0.02225
  integer 7: 0.05726
  integer 8: 0.05545
  integer 9: 0.0873
string vec F56 example sim: 0.90712
string vec F57: 
  integer 0: 0.0391
  integer 1: 0.05615
  integer 2: -0.01228
  integer 3: 0.0231
  integer 4: 0.09241
  integer 5: 0.09521
  integer 6: 0.00823
  integer 7: -0.01314
  integer 8: -0.00629
  integer 9: 0.01849
string vec F57 example sim: 0.61967
string vec F58: 
  integer 0: 0.02321
  integer 1: 0.07644
  integer 2: 0.08245
  integer 3: 0.00717
  integer 4: -0.02119
  integer 5: 0.0264
  integer 6: 0.0868
  integer 7: 0.09169
  integer 8: 0.00029
  integer 9: 0.04405
string vec F58 example sim: 0.73475
string vec F59: 
  integer 0: 0.04727
  integer 1: 0.03554
  integer 2: 0.02036
  integer 3: 0.06614
  integer 4: 0.01801
  integer 5: 0.05334
  integer 6: 0.04377
  integer 7: -0.01483
  integer 8: 0.03548
  integer 9: 0.07251
string vec F59 example sim: 0.84206
string vec F6: 
  integer 0: 0.00059
  integer 1: 0.01734
  integer 2: 0.09432
  integer 3: 0.03842
  integer 4: 0.06086
  integer 5: 0.03598
  integer 6: 0.07259
  integer 7: 0.00705
  integer 8: 0.0775
  integer 9: 0.00556
string vec F6 example sim: 0.78903
string vec F60: 
  integer 0: 0.05156
  integer 1: -0.01881
  integer 2: -0.01064
  integer 3: 0.06104
  integer 4: 0.00169
  integer 5: 0.05236
  integer 6: 0.03413
  integer 7: 0.03663
  integer 8: 0.07858
  integer 9: 0.06136
string vec F60 example sim: 0.741
string vec F61: 
  integer 0: 0.03539
  integer 1: 0.04649
  integer 2: 0.05884
  integer 3: 0.0233
  integer 4: 0.0704
  integer 5: 0.09098
  integer 6: -0.01124
  integer 7: 0.08951
  integer 8: -0.00392
  integer 9: 0.01049
string vec F61 example sim: 0.76289
string vec F62: 
  integer 0: 0.08426
  integer 1: 0.08708
  integer 2: -0.00502
  integer 3: 0.01531
  integer 4: 0.09689
  integer 5: 0.04261
  integer 6: 0.04111
  integer 7: -0.00581
  integer 8: 0.00307
  integer 9: -0.01905
string vec F62 example sim: 0.64063
string vec F63: 
  integer 0: 0.07924
  integer 1: -0.01315
  integer 2: 0.02048
  integer 3: -0.00838
  integer 4: 0.04706
  integer 5: -0.01016
  integer 6: -0.00809
  integer 7: 0.03675
  integer 8: 0.07341
  integer 9: -0.02129
string vec F63 example sim: 0.48195
string vec F64: 
  integer 0: 0.02983
  integer 1: -0.0221
  integer 2: 0.01928
  integer 3: 0.03683
  integer 4: 0.0636
  integer 5: -0.01142
  integer 6: 0.08631
  integer 7: 0.08695
  integer 8: 0.03856
  integer 9: 0.05591
string vec F64 example sim: 0.73981
string vec F65: 
  integer 0: 0.09076
  integer 1: 0.05231
  integer 2: 0.0664
  integer 3: 0.07704
  integer 4: -0.01089
  integer 5: 0.02733
  integer 6: 0.04784
  integer 7: 0.0417
  integer 8: -0.02128
  integer 9: -0.00719
string vec F65 example sim: 0.70429
string vec F66: 
  integer 0: 0.07468
  integer 1: 0.0901
  integer 2: 0.0914
  integer 3: 0.07182
  integer 4: 0.04565
  integer 5: 0.07394
  integer 6: 0.05156
  integer 7: -0.00623
  integer 8: 0.08921
  integer 9: -0.00405
string vec F66 example sim: 0.85767
string vec F67: 
  integer 0: 0.02356
  integer 1: -0.02444
  integer 2: 0.08669
  integer 3: -0.01429
  integer 4: 0.00639
  integer 5: 0.0791
  integer 6: 0.0638
  integer 7: 0.04199
  integer 8: -0.02478
  integer 9: 0.06675
string vec F67 example sim: 0.59689
string vec F68: 
  integer 0: 0.03111
  integer 1: 0.02578
  integer 2: 0.01145
  integer 3: 0.01361
  integer 4: -0.00994
  integer 5: 0.02386
  integer 6: 0.05595
  integer 7: 0.0475
  integer 8: 0.02296
  integer 9: 0.08415
string vec F68 example sim: 0.77555
string vec F69: 
  integer 0: 0.03829
  integer 1: 0.07643
  integer 2: 0.00132
  integer 3: 0.04478
  integer 4: 0.06171
  integer 5: 0.03571
  integer 6: -0.02179
  integer 7: -0.01129
  integer 8: 0.05815
  integer 9: 0.01724
string vec F69 example sim: 0.69543
string vec F7: 
  integer 0: 0.00387
  integer 1: -0.00806
  integer 2: 0.0706
  integer 3: 0.08026
  integer 4: 0.08157
  integer 5: 0.06487
  integer 6: 0.08308
  integer 7: -0.00247
  integer 8: 0.01024
  integer 9: 0.05097
string vec F7 example sim: 0.7691
string vec F70: 
  integer 0: 0.00328
  integer 1: -0.01269
  integer 2: 0.0089
  integer 3: 0.03655
  integer 4: 0.0582
  integer 5: 0.07391
  integer 6: 0.01263
  integer 7: 0.07106
  integer 8: 1.0E-5
  integer 9: 0.01224
string vec F70 example sim: 0.66447
string vec F71: 
  integer 0: -0.00692
  integer 1: 0.05248
  integer 2: 0.02041
  integer 3: 0.06553
  integer 4: -0.02324
  integer 5: 0.03555
  integer 6: -0.01101
  integer 7: 0.00059
  integer 8: 0.06924
  integer 9: 0.00451
string vec F71 example sim: 0.54866
string vec F72: 
  integer 0: 0.04617
  integer 1: 0.04461
  integer 2: 0.06265
  integer 3: 0.05407
  integer 4: 0.02148
  integer 5: 0.08911
  integer 6: 0.0493
  integer 7: 0.02789
  integer 8: 0.05985
  integer 9: 0.09084
string vec F72 example sim: 0.93067
string vec F73: 
  integer 0: 0.08561
  integer 1: 0.03128
  integer 2: 0.09175
  integer 3: 0.05104
  integer 4: -0.00645
  integer 5: 0.03395
  integer 6: -0.01049
  integer 7: -0.01189
  integer 8: 0.07189
  integer 9: 0.00306
string vec F73 example sim: 0.6669
string vec F74: 
  integer 0: 0.01569
  integer 1: 0.03903
  integer 2: 0.02555
  integer 3: 0.07019
  integer 4: 0.08194
  integer 5: 0.01274
  integer 6: -0.0131
  integer 7: 0.08489
  integer 8: 0.07985
  integer 9: 0.03054
string vec F74 example sim: 0.79471
string vec F75: 
  integer 0: 0.00479
  integer 1: -0.011
  integer 2: 0.04653
  integer 3: 0.0366
  integer 4: 0.09503
  integer 5: 0.07442
  integer 6: 0.07609
  integer 7: 0.06635
  integer 8: 0.06872
  integer 9: 0.02681
string vec F75 example sim: 0.83246
string vec F76: 
  integer 0: 0.01105
  integer 1: 0.08987
  integer 2: -0.01785
  integer 3: 0.09683
  integer 4: -0.02466
  integer 5: 0.08996
  integer 6: 0.07101
  integer 7: 0.08624
  integer 8: 0.08515
  integer 9: 0.05302
string vec F76 example sim: 0.77125
string vec F77: 
  integer 0: 0.08273
  integer 1: 0.05606
  integer 2: 0.00066
  integer 3: -0.00283
  integer 4: 0.04714
  integer 5: -0.00982
  integer 6: 0.03735
  integer 7: -0.00618
  integer 8: 0.03365
  integer 9: 0.03676
string vec F77 example sim: 0.68391
string vec F78: 
  integer 0: -0.01863
  integer 1: 0.08027
  integer 2: 0.06515
  integer 3: -0.0219
  integer 4: 0.0578
  integer 5: 0.01464
  integer 6: 0.05241
  integer 7: -0.01127
  integer 8: -0.0197
  integer 9: 0.00387
string vec F78 example sim: 0.47311
string vec F79: 
  integer 0: 0.0577
  integer 1: 0.09261
  integer 2: 0.07534
  integer 3: 0.03712
  integer 4: -0.01406
  integer 5: 0.03413
  integer 6: 0.012
  integer 7: 0.05335
  integer 8: -0.00299
  integer 9: -0.02175
string vec F79 example sim: 0.66308
string vec F8: 
  integer 0: 0.07855
  integer 1: 0.04901
  integer 2: 0.03866
  integer 3: 0.07506
  integer 4: 0.00944
  integer 5: 0.04705
  integer 6: 0.01983
  integer 7: 0.01295
  integer 8: 0.07373
  integer 9: 0.02364
string vec F8 example sim: 0.86347
string vec F80: 
  integer 0: -0.00111
  integer 1: 0.0364
  integer 2: 0.02286
  integer 3: 0.0083
  integer 4: 0.0942
  integer 5: -0.00406
  integer 6: 0.03644
  integer 7: 0.08353
  integer 8: -0.01699
  integer 9: 0.02992
string vec F80 example sim: 0.64257
string vec F81: 
  integer 0: 0.0085
  integer 1: -0.0199
  integer 2: 0.09355
  integer 3: 0.05315
  integer 4: -0.0138
  integer 5: 0.05659
  integer 6: -0.00971
  integer 7: 0.0102
  integer 8: 0.04091
  integer 9: 0.08526
string vec F81 example sim: 0.61469
string vec F82: 
  integer 0: -0.01598
  integer 1: 0.07204
  integer 2: 0.02434
  integer 3: -0.02425
  integer 4: 0.00484
  integer 5: 0.07203
  integer 6: 0.03831
  integer 7: 0.06501
  integer 8: 0.00521
  integer 9: 0.00237
string vec F82 example sim: 0.58204
string vec F83: 
  integer 0: 0.06296
  integer 1: 0.09742
  integer 2: 0.02634
  integer 3: -0.01706
  integer 4: 0.00695
  integer 5: -0.00823
  integer 6: -0.01736
  integer 7: 0.06912
  integer 8: 0.03244
  integer 9: 0.00429
string vec F83 example sim: 0.5645
string vec F84: 
  integer 0: 0.08284
  integer 1: 0.02491
  integer 2: -0.00276
  integer 3: 0.03849
  integer 4: 0.03815
  integer 5: 0.05863
  integer 6: 0.05825
  integer 7: -0.01653
  integer 8: 0.04489
  integer 9: 0.02344
string vec F84 example sim: 0.78146
string vec F85: 
  integer 0: 0.05313
  integer 1: -0.02266
  integer 2: 0.07151
  integer 3: 0.05056
  integer 4: -0.02091
  integer 5: 0.05427
  integer 6: 0.00419
  integer 7: 0.07096
  integer 8: 0.06849
  integer 9: 0.09476
string vec F85 example sim: 0.73735
string vec F86: 
  integer 0: 0.0981
  integer 1: 0.05616
  integer 2: 0.09134
  integer 3: 0.03766
  integer 4: 0.07625
  integer 5: -0.00568
  integer 6: -0.00349
  integer 7: 0.05373
  integer 8: 0.00962
  integer 9: 0.05843
string vec F86 example sim: 0.80044
string vec F87: 
  integer 0: 0.07854
  integer 1: 0.05071
  integer 2: 0.09319
  integer 3: 0.03941
  integer 4: 0.00259
  integer 5: 0.04112
  integer 6: 0.01132
  integer 7: 0.03842
  integer 8: 0.07755
  integer 9: -0.00571
string vec F87 example sim: 0.80257
string vec F88: 
  integer 0: -0.01837
  integer 1: 0.0056
  integer 2: -0.01092
  integer 3: 0.02529
  integer 4: 0.02816
  integer 5: 0.02342
  integer 6: 0.0975
  integer 7: 0.05894
  integer 8: 0.02716
  integer 9: 0.00864
string vec F88 example sim: 0.60857
string vec F89: 
  integer 0: 0.02276
  integer 1: 0.0943
  integer 2: 0.06831
  integer 3: -0.01579
  integer 4: 0.059
  integer 5: 0.06119
  integer 6: -0.01829
  integer 7: 0.01617
  integer 8: 0.05096
  integer 9: 0.00034
string vec F89 example sim: 0.68141
string vec F9: 
  integer 0: 0.00596
  integer 1: 0.08427
  integer 2: 0.0096
  integer 3: 0.09992
  integer 4: 0.09042
  integer 5: 0.03761
  integer 6: 0.03735
  integer 7: 0.07691
  integer 8: -0.01854
  integer 9: 0.08952
string vec F9 example sim: 0.78721
string vec F90: 
  integer 0: 0.01686
  integer 1: 0.08834
  integer 2: 0.0438
  integer 3: 0.00137
  integer 4: 0.04834
  integer 5: 0.07126
  integer 6: 0.07622
  integer 7: 0.0447
  integer 8: 0.09723
  integer 9: 0.03602
string vec F90 example sim: 0.8738
string vec F91: 
  integer 0: 0.02269
  integer 1: 0.03371
  integer 2: -0.01951
  integer 3: 0.07311
  integer 4: 0.00215
  integer 5: 0.09905
  integer 6: 0.08874
  integer 7: -0.02381
  integer 8: 0.0303
  integer 9: 0.06453
string vec F91 example sim: 0.66932
string vec F92: 
  integer 0: 0.0015
  integer 1: -0.01219
  integer 2: 0.08695
  integer 3: 0.06718
  integer 4: 0.01731
  integer 5: 0.03484
  integer 6: -0.00421
  integer 7: 0.09204
  integer 8: 0.09872
  integer 9: 0.04118
string vec F92 example sim: 0.72955
string vec F93: 
  integer 0: -0.00288
  integer 1: -0.01066
  integer 2: 0.08105
  integer 3: -0.01292
  integer 4: 0.00292
  integer 5: 0.0146
  integer 6: -0.02159
  integer 7: 0.09416
  integer 8: -0.01974
  integer 9: 0.06038
string vec F93 example sim: 0.40984
string vec F94: 
  integer 0: 0.08686
  integer 1: 0.00717
  integer 2: 0.01689
  integer 3: 0.02985
  integer 4: 0.09287
  integer 5: 0.06991
  integer 6: 0.04174
  integer 7: 0.03714
  integer 8: 0.0956
  integer 9: 0.06964
string vec F94 example sim: 0.87256
string vec F95: 
  integer 0: 0.01325
  integer 1: -0.0148
  integer 2: 0.08964
  integer 3: 0.06346
  integer 4: 0.09209
  integer 5: -0.02296
  integer 6: 0.01462
  integer 7: -0.00224
  integer 8: 0.07651
  integer 9: 0.07099
string vec F95 example sim: 0.66677
string vec F96: 
  integer 0: 0.03864
  integer 1: 0.06361
  integer 2: 0.01727
  integer 3: 0.00105
  integer 4: 0.0567
  integer 5: 0.04522
  integer 6: -0.00237
  integer 7: 0.04217
  integer 8: -0.01056
  integer 9: 0.05243
string vec F96 example sim: 0.76658
string vec F97: 
  integer 0: 0.03834
  integer 1: 0.0682
  integer 2: 0.09599
  integer 3: -0.008
  integer 4: -0.00309
  integer 5: 0.01711
  integer 6: 0.01001
  integer 7: 0.02874
  integer 8: 0.06406
  integer 9: 0.0705
string vec F97 example sim: 0.75323
string vec F98: 
  integer 0: -0.00861
  integer 1: 0.09063
  integer 2: -0.02086
  integer 3: 0.06293
  integer 4: -0.01549
  integer 5: 0.0674
  integer 6: -0.00988
  integer 7: 0.01415
  integer 8: 0.03639
  integer 9: -0.02145
string vec F98 example sim: 0.44152
string vec F99: 
  integer 0: 0.09578
  integer 1: 0.08003
  integer 2: 0.08905
  integer 3: 0.05257
  integer 4: 0.08979
  integer 5: 0.0774
  integer 6: 0.00829
  integer 7: 0.08126
  integer 8: 0.07861
  integer 9: 0.02279
string vec F99 example sim: 0.92193
END_expected;

verifyTestOutput( $outputdir, $result, $expected);

?>
