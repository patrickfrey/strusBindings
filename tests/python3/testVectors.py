import strus
import math
from utils import *
from dumpVectorStorage import *

if len(sys.argv) > 1:
	outputdir = sys.argv[1]
else:
	outputdir = "."

storage = outputdir + "/storage"
config = {
	'path':storage,
	'commit':10,
	'dim':10,
	'bit':8,
	'var':100,
	'simdist':12,
	'maxdist':20,
	'realvecweights':True
}
vectors = []

for vi in range( 1,101):
	vv = []
	for xi in range(1,11):
		vv.append( 1.0 / math.sqrt(vi + xi))
	vectors.append( vv)

ctx = strus.Context()
ctx.loadModule( "storage_vector_std")

if ctx.storageExists( config):
	ctx.destroyStorage( config)

ctx.createVectorStorage( config);
storage = ctx.createVectorStorageClient( config)
transaction = storage.createTransaction()
for iv,vv in enumerate( vectors):
	transaction.addFeature( "F%u" % (iv+1), vv)
	transaction.defineFeatureConceptRelation( "main", iv, iv+1)

transaction.commit()
transaction.close()
output = dumpVectorStorage( ctx, config)

storage = ctx.createVectorStorageClient( config)
searcher = storage.createSearcher( 0, storage.nofFeatures())
simlist = searcher.findSimilar( [0.1,0.1,0.1,0.1,0.1, 0.1,0.1,0.1,0.1,0.1], 10)
output[ 'simlist 0.1x10'] = simlist

result = "vector storage dump:" + dumpTree( output) + "\n"
expected = """vector storage dump:
str class '' feature 0: 
str class '' feature 1: 
str class '' feature 10: 
str class '' feature 11: 
str class '' feature 12: 
str class '' feature 13: 
str class '' feature 14: 
str class '' feature 15: 
str class '' feature 16: 
str class '' feature 17: 
str class '' feature 18: 
str class '' feature 19: 
str class '' feature 2: 
str class '' feature 20: 
str class '' feature 21: 
str class '' feature 22: 
str class '' feature 23: 
str class '' feature 24: 
str class '' feature 25: 
str class '' feature 26: 
str class '' feature 27: 
str class '' feature 28: 
str class '' feature 29: 
str class '' feature 3: 
str class '' feature 30: 
str class '' feature 31: 
str class '' feature 32: 
str class '' feature 33: 
str class '' feature 34: 
str class '' feature 35: 
str class '' feature 36: 
str class '' feature 37: 
str class '' feature 38: 
str class '' feature 39: 
str class '' feature 4: 
str class '' feature 40: 
str class '' feature 41: 
str class '' feature 42: 
str class '' feature 43: 
str class '' feature 44: 
str class '' feature 45: 
str class '' feature 46: 
str class '' feature 47: 
str class '' feature 48: 
str class '' feature 49: 
str class '' feature 5: 
str class '' feature 50: 
str class '' feature 51: 
str class '' feature 52: 
str class '' feature 53: 
str class '' feature 54: 
str class '' feature 55: 
str class '' feature 56: 
str class '' feature 57: 
str class '' feature 58: 
str class '' feature 59: 
str class '' feature 6: 
str class '' feature 60: 
str class '' feature 61: 
str class '' feature 62: 
str class '' feature 63: 
str class '' feature 64: 
str class '' feature 65: 
str class '' feature 66: 
str class '' feature 67: 
str class '' feature 68: 
str class '' feature 69: 
str class '' feature 7: 
str class '' feature 70: 
str class '' feature 71: 
str class '' feature 72: 
str class '' feature 73: 
str class '' feature 74: 
str class '' feature 75: 
str class '' feature 76: 
str class '' feature 77: 
str class '' feature 78: 
str class '' feature 79: 
str class '' feature 8: 
str class '' feature 80: 
str class '' feature 81: 
str class '' feature 82: 
str class '' feature 83: 
str class '' feature 84: 
str class '' feature 85: 
str class '' feature 86: 
str class '' feature 87: 
str class '' feature 88: 
str class '' feature 89: 
str class '' feature 9: 
str class '' feature 90: 
str class '' feature 91: 
str class '' feature 92: 
str class '' feature 93: 
str class '' feature 94: 
str class '' feature 95: 
str class '' feature 96: 
str class '' feature 97: 
str class '' feature 98: 
str class '' feature 99: 
str class 'main' feature 0:
  number 1: 1
str class 'main' feature 1:
  number 1: 2
str class 'main' feature 10:
  number 1: 11
str class 'main' feature 11:
  number 1: 12
str class 'main' feature 12:
  number 1: 13
str class 'main' feature 13:
  number 1: 14
str class 'main' feature 14:
  number 1: 15
str class 'main' feature 15:
  number 1: 16
str class 'main' feature 16:
  number 1: 17
str class 'main' feature 17:
  number 1: 18
str class 'main' feature 18:
  number 1: 19
str class 'main' feature 19:
  number 1: 20
str class 'main' feature 2:
  number 1: 3
str class 'main' feature 20:
  number 1: 21
str class 'main' feature 21:
  number 1: 22
str class 'main' feature 22:
  number 1: 23
str class 'main' feature 23:
  number 1: 24
str class 'main' feature 24:
  number 1: 25
str class 'main' feature 25:
  number 1: 26
str class 'main' feature 26:
  number 1: 27
str class 'main' feature 27:
  number 1: 28
str class 'main' feature 28:
  number 1: 29
str class 'main' feature 29:
  number 1: 30
str class 'main' feature 3:
  number 1: 4
str class 'main' feature 30:
  number 1: 31
str class 'main' feature 31:
  number 1: 32
str class 'main' feature 32:
  number 1: 33
str class 'main' feature 33:
  number 1: 34
str class 'main' feature 34:
  number 1: 35
str class 'main' feature 35:
  number 1: 36
str class 'main' feature 36:
  number 1: 37
str class 'main' feature 37:
  number 1: 38
str class 'main' feature 38:
  number 1: 39
str class 'main' feature 39:
  number 1: 40
str class 'main' feature 4:
  number 1: 5
str class 'main' feature 40:
  number 1: 41
str class 'main' feature 41:
  number 1: 42
str class 'main' feature 42:
  number 1: 43
str class 'main' feature 43:
  number 1: 44
str class 'main' feature 44:
  number 1: 45
str class 'main' feature 45:
  number 1: 46
str class 'main' feature 46:
  number 1: 47
str class 'main' feature 47:
  number 1: 48
str class 'main' feature 48:
  number 1: 49
str class 'main' feature 49:
  number 1: 50
str class 'main' feature 5:
  number 1: 6
str class 'main' feature 50:
  number 1: 51
str class 'main' feature 51:
  number 1: 52
str class 'main' feature 52:
  number 1: 53
str class 'main' feature 53:
  number 1: 54
str class 'main' feature 54:
  number 1: 55
str class 'main' feature 55:
  number 1: 56
str class 'main' feature 56:
  number 1: 57
str class 'main' feature 57:
  number 1: 58
str class 'main' feature 58:
  number 1: 59
str class 'main' feature 59:
  number 1: 60
str class 'main' feature 6:
  number 1: 7
str class 'main' feature 60:
  number 1: 61
str class 'main' feature 61:
  number 1: 62
str class 'main' feature 62:
  number 1: 63
str class 'main' feature 63:
  number 1: 64
str class 'main' feature 64:
  number 1: 65
str class 'main' feature 65:
  number 1: 66
str class 'main' feature 66:
  number 1: 67
str class 'main' feature 67:
  number 1: 68
str class 'main' feature 68:
  number 1: 69
str class 'main' feature 69:
  number 1: 70
str class 'main' feature 7:
  number 1: 8
str class 'main' feature 70:
  number 1: 71
str class 'main' feature 71:
  number 1: 72
str class 'main' feature 72:
  number 1: 73
str class 'main' feature 73:
  number 1: 74
str class 'main' feature 74:
  number 1: 75
str class 'main' feature 75:
  number 1: 76
str class 'main' feature 76:
  number 1: 77
str class 'main' feature 77:
  number 1: 78
str class 'main' feature 78:
  number 1: 79
str class 'main' feature 79:
  number 1: 80
str class 'main' feature 8:
  number 1: 9
str class 'main' feature 80:
  number 1: 81
str class 'main' feature 81:
  number 1: 82
str class 'main' feature 82:
  number 1: 83
str class 'main' feature 83:
  number 1: 84
str class 'main' feature 84:
  number 1: 85
str class 'main' feature 85:
  number 1: 86
str class 'main' feature 86:
  number 1: 87
str class 'main' feature 87:
  number 1: 88
str class 'main' feature 88:
  number 1: 89
str class 'main' feature 89:
  number 1: 90
str class 'main' feature 9:
  number 1: 10
str class 'main' feature 90:
  number 1: 91
str class 'main' feature 91:
  number 1: 92
str class 'main' feature 92:
  number 1: 93
str class 'main' feature 93:
  number 1: 94
str class 'main' feature 94:
  number 1: 95
str class 'main' feature 95:
  number 1: 96
str class 'main' feature 96:
  number 1: 97
str class 'main' feature 97:
  number 1: 98
str class 'main' feature 98:
  number 1: 99
str class 'main' feature 99:
  number 1: 100
str classes:
  number 1: ""
  number 2: "main"
str config:
  str assignments: "7"
  str baff: "0.10000"
  str bit: "8"
  str commit: "10"
  str descendants: "10"
  str dim: "10"
  str eqdiff: "0.25000"
  str eqdist: "2"
  str fdf: "0.25000"
  str forcesim: "no"
  str greediness: "3"
  str isaf: "0.60000"
  str iterations: "20"
  str logfile: ""
  str maxage: "20"
  str maxconcepts: "0"
  str maxdist: "20"
  str maxfeatures: "0"
  str maxsimsam: "0"
  str mutations: "50"
  str path: "storage"
  str probdist: "18"
  str probsim: "yes"
  str raddist: "12"
  str realvecweights: "yes"
  str rndsimsam: "0"
  str simdist: "12"
  str singletons: "0"
  str threads: "0"
  str var: "100"
  str votes: "13"
str simlist 0.1x10:
  number 1:
    str featidx: 98
    str weight: 0.99991
  number 2:
    str featidx: 97
    str weight: 0.99990
  number 3:
    str featidx: 96
    str weight: 0.99990
  number 4:
    str featidx: 95
    str weight: 0.99990
  number 5:
    str featidx: 94
    str weight: 0.99990
  number 6:
    str featidx: 93
    str weight: 0.99990
  number 7:
    str featidx: 92
    str weight: 0.99989
  number 8:
    str featidx: 91
    str weight: 0.99989
  number 9:
    str featidx: 90
    str weight: 0.99989
  number 10:
    str featidx: 89
    str weight: 0.99989
str vector 'F1':
  number 1: 0.70711
  number 2: 0.57735
  number 3: 0.50000
  number 4: 0.44721
  number 5: 0.40825
  number 6: 0.37796
  number 7: 0.35355
  number 8: 0.33333
  number 9: 0.31623
  number 10: 0.30151
str vector 'F10':
  number 1: 0.30151
  number 2: 0.28868
  number 3: 0.27735
  number 4: 0.26726
  number 5: 0.25820
  number 6: 0.25000
  number 7: 0.24254
  number 8: 0.23570
  number 9: 0.22942
  number 10: 0.22361
str vector 'F100':
  number 1: 0.09950
  number 2: 0.09901
  number 3: 0.09853
  number 4: 0.09806
  number 5: 0.09759
  number 6: 0.09713
  number 7: 0.09667
  number 8: 0.09623
  number 9: 0.09578
  number 10: 0.09535
str vector 'F11':
  number 1: 0.28868
  number 2: 0.27735
  number 3: 0.26726
  number 4: 0.25820
  number 5: 0.25000
  number 6: 0.24254
  number 7: 0.23570
  number 8: 0.22942
  number 9: 0.22361
  number 10: 0.21822
str vector 'F12':
  number 1: 0.27735
  number 2: 0.26726
  number 3: 0.25820
  number 4: 0.25000
  number 5: 0.24254
  number 6: 0.23570
  number 7: 0.22942
  number 8: 0.22361
  number 9: 0.21822
  number 10: 0.21320
str vector 'F13':
  number 1: 0.26726
  number 2: 0.25820
  number 3: 0.25000
  number 4: 0.24254
  number 5: 0.23570
  number 6: 0.22942
  number 7: 0.22361
  number 8: 0.21822
  number 9: 0.21320
  number 10: 0.20851
str vector 'F14':
  number 1: 0.25820
  number 2: 0.25000
  number 3: 0.24254
  number 4: 0.23570
  number 5: 0.22942
  number 6: 0.22361
  number 7: 0.21822
  number 8: 0.21320
  number 9: 0.20851
  number 10: 0.20412
str vector 'F15':
  number 1: 0.25000
  number 2: 0.24254
  number 3: 0.23570
  number 4: 0.22942
  number 5: 0.22361
  number 6: 0.21822
  number 7: 0.21320
  number 8: 0.20851
  number 9: 0.20412
  number 10: 0.20000
str vector 'F16':
  number 1: 0.24254
  number 2: 0.23570
  number 3: 0.22942
  number 4: 0.22361
  number 5: 0.21822
  number 6: 0.21320
  number 7: 0.20851
  number 8: 0.20412
  number 9: 0.20000
  number 10: 0.19612
str vector 'F17':
  number 1: 0.23570
  number 2: 0.22942
  number 3: 0.22361
  number 4: 0.21822
  number 5: 0.21320
  number 6: 0.20851
  number 7: 0.20412
  number 8: 0.20000
  number 9: 0.19612
  number 10: 0.19245
str vector 'F18':
  number 1: 0.22942
  number 2: 0.22361
  number 3: 0.21822
  number 4: 0.21320
  number 5: 0.20851
  number 6: 0.20412
  number 7: 0.20000
  number 8: 0.19612
  number 9: 0.19245
  number 10: 0.18898
str vector 'F19':
  number 1: 0.22361
  number 2: 0.21822
  number 3: 0.21320
  number 4: 0.20851
  number 5: 0.20412
  number 6: 0.20000
  number 7: 0.19612
  number 8: 0.19245
  number 9: 0.18898
  number 10: 0.18570
str vector 'F2':
  number 1: 0.57735
  number 2: 0.50000
  number 3: 0.44721
  number 4: 0.40825
  number 5: 0.37796
  number 6: 0.35355
  number 7: 0.33333
  number 8: 0.31623
  number 9: 0.30151
  number 10: 0.28868
str vector 'F20':
  number 1: 0.21822
  number 2: 0.21320
  number 3: 0.20851
  number 4: 0.20412
  number 5: 0.20000
  number 6: 0.19612
  number 7: 0.19245
  number 8: 0.18898
  number 9: 0.18570
  number 10: 0.18257
str vector 'F21':
  number 1: 0.21320
  number 2: 0.20851
  number 3: 0.20412
  number 4: 0.20000
  number 5: 0.19612
  number 6: 0.19245
  number 7: 0.18898
  number 8: 0.18570
  number 9: 0.18257
  number 10: 0.17961
str vector 'F22':
  number 1: 0.20851
  number 2: 0.20412
  number 3: 0.20000
  number 4: 0.19612
  number 5: 0.19245
  number 6: 0.18898
  number 7: 0.18570
  number 8: 0.18257
  number 9: 0.17961
  number 10: 0.17678
str vector 'F23':
  number 1: 0.20412
  number 2: 0.20000
  number 3: 0.19612
  number 4: 0.19245
  number 5: 0.18898
  number 6: 0.18570
  number 7: 0.18257
  number 8: 0.17961
  number 9: 0.17678
  number 10: 0.17408
str vector 'F24':
  number 1: 0.20000
  number 2: 0.19612
  number 3: 0.19245
  number 4: 0.18898
  number 5: 0.18570
  number 6: 0.18257
  number 7: 0.17961
  number 8: 0.17678
  number 9: 0.17408
  number 10: 0.17150
str vector 'F25':
  number 1: 0.19612
  number 2: 0.19245
  number 3: 0.18898
  number 4: 0.18570
  number 5: 0.18257
  number 6: 0.17961
  number 7: 0.17678
  number 8: 0.17408
  number 9: 0.17150
  number 10: 0.16903
str vector 'F26':
  number 1: 0.19245
  number 2: 0.18898
  number 3: 0.18570
  number 4: 0.18257
  number 5: 0.17961
  number 6: 0.17678
  number 7: 0.17408
  number 8: 0.17150
  number 9: 0.16903
  number 10: 0.16667
str vector 'F27':
  number 1: 0.18898
  number 2: 0.18570
  number 3: 0.18257
  number 4: 0.17961
  number 5: 0.17678
  number 6: 0.17408
  number 7: 0.17150
  number 8: 0.16903
  number 9: 0.16667
  number 10: 0.16440
str vector 'F28':
  number 1: 0.18570
  number 2: 0.18257
  number 3: 0.17961
  number 4: 0.17678
  number 5: 0.17408
  number 6: 0.17150
  number 7: 0.16903
  number 8: 0.16667
  number 9: 0.16440
  number 10: 0.16222
str vector 'F29':
  number 1: 0.18257
  number 2: 0.17961
  number 3: 0.17678
  number 4: 0.17408
  number 5: 0.17150
  number 6: 0.16903
  number 7: 0.16667
  number 8: 0.16440
  number 9: 0.16222
  number 10: 0.16013
str vector 'F3':
  number 1: 0.50000
  number 2: 0.44721
  number 3: 0.40825
  number 4: 0.37796
  number 5: 0.35355
  number 6: 0.33333
  number 7: 0.31623
  number 8: 0.30151
  number 9: 0.28868
  number 10: 0.27735
str vector 'F30':
  number 1: 0.17961
  number 2: 0.17678
  number 3: 0.17408
  number 4: 0.17150
  number 5: 0.16903
  number 6: 0.16667
  number 7: 0.16440
  number 8: 0.16222
  number 9: 0.16013
  number 10: 0.15811
str vector 'F31':
  number 1: 0.17678
  number 2: 0.17408
  number 3: 0.17150
  number 4: 0.16903
  number 5: 0.16667
  number 6: 0.16440
  number 7: 0.16222
  number 8: 0.16013
  number 9: 0.15811
  number 10: 0.15617
str vector 'F32':
  number 1: 0.17408
  number 2: 0.17150
  number 3: 0.16903
  number 4: 0.16667
  number 5: 0.16440
  number 6: 0.16222
  number 7: 0.16013
  number 8: 0.15811
  number 9: 0.15617
  number 10: 0.15430
str vector 'F33':
  number 1: 0.17150
  number 2: 0.16903
  number 3: 0.16667
  number 4: 0.16440
  number 5: 0.16222
  number 6: 0.16013
  number 7: 0.15811
  number 8: 0.15617
  number 9: 0.15430
  number 10: 0.15250
str vector 'F34':
  number 1: 0.16903
  number 2: 0.16667
  number 3: 0.16440
  number 4: 0.16222
  number 5: 0.16013
  number 6: 0.15811
  number 7: 0.15617
  number 8: 0.15430
  number 9: 0.15250
  number 10: 0.15076
str vector 'F35':
  number 1: 0.16667
  number 2: 0.16440
  number 3: 0.16222
  number 4: 0.16013
  number 5: 0.15811
  number 6: 0.15617
  number 7: 0.15430
  number 8: 0.15250
  number 9: 0.15076
  number 10: 0.14907
str vector 'F36':
  number 1: 0.16440
  number 2: 0.16222
  number 3: 0.16013
  number 4: 0.15811
  number 5: 0.15617
  number 6: 0.15430
  number 7: 0.15250
  number 8: 0.15076
  number 9: 0.14907
  number 10: 0.14744
str vector 'F37':
  number 1: 0.16222
  number 2: 0.16013
  number 3: 0.15811
  number 4: 0.15617
  number 5: 0.15430
  number 6: 0.15250
  number 7: 0.15076
  number 8: 0.14907
  number 9: 0.14744
  number 10: 0.14586
str vector 'F38':
  number 1: 0.16013
  number 2: 0.15811
  number 3: 0.15617
  number 4: 0.15430
  number 5: 0.15250
  number 6: 0.15076
  number 7: 0.14907
  number 8: 0.14744
  number 9: 0.14586
  number 10: 0.14434
str vector 'F39':
  number 1: 0.15811
  number 2: 0.15617
  number 3: 0.15430
  number 4: 0.15250
  number 5: 0.15076
  number 6: 0.14907
  number 7: 0.14744
  number 8: 0.14586
  number 9: 0.14434
  number 10: 0.14286
str vector 'F4':
  number 1: 0.44721
  number 2: 0.40825
  number 3: 0.37796
  number 4: 0.35355
  number 5: 0.33333
  number 6: 0.31623
  number 7: 0.30151
  number 8: 0.28868
  number 9: 0.27735
  number 10: 0.26726
str vector 'F40':
  number 1: 0.15617
  number 2: 0.15430
  number 3: 0.15250
  number 4: 0.15076
  number 5: 0.14907
  number 6: 0.14744
  number 7: 0.14586
  number 8: 0.14434
  number 9: 0.14286
  number 10: 0.14142
str vector 'F41':
  number 1: 0.15430
  number 2: 0.15250
  number 3: 0.15076
  number 4: 0.14907
  number 5: 0.14744
  number 6: 0.14586
  number 7: 0.14434
  number 8: 0.14286
  number 9: 0.14142
  number 10: 0.14003
str vector 'F42':
  number 1: 0.15250
  number 2: 0.15076
  number 3: 0.14907
  number 4: 0.14744
  number 5: 0.14586
  number 6: 0.14434
  number 7: 0.14286
  number 8: 0.14142
  number 9: 0.14003
  number 10: 0.13868
str vector 'F43':
  number 1: 0.15076
  number 2: 0.14907
  number 3: 0.14744
  number 4: 0.14586
  number 5: 0.14434
  number 6: 0.14286
  number 7: 0.14142
  number 8: 0.14003
  number 9: 0.13868
  number 10: 0.13736
str vector 'F44':
  number 1: 0.14907
  number 2: 0.14744
  number 3: 0.14586
  number 4: 0.14434
  number 5: 0.14286
  number 6: 0.14142
  number 7: 0.14003
  number 8: 0.13868
  number 9: 0.13736
  number 10: 0.13608
str vector 'F45':
  number 1: 0.14744
  number 2: 0.14586
  number 3: 0.14434
  number 4: 0.14286
  number 5: 0.14142
  number 6: 0.14003
  number 7: 0.13868
  number 8: 0.13736
  number 9: 0.13608
  number 10: 0.13484
str vector 'F46':
  number 1: 0.14586
  number 2: 0.14434
  number 3: 0.14286
  number 4: 0.14142
  number 5: 0.14003
  number 6: 0.13868
  number 7: 0.13736
  number 8: 0.13608
  number 9: 0.13484
  number 10: 0.13363
str vector 'F47':
  number 1: 0.14434
  number 2: 0.14286
  number 3: 0.14142
  number 4: 0.14003
  number 5: 0.13868
  number 6: 0.13736
  number 7: 0.13608
  number 8: 0.13484
  number 9: 0.13363
  number 10: 0.13245
str vector 'F48':
  number 1: 0.14286
  number 2: 0.14142
  number 3: 0.14003
  number 4: 0.13868
  number 5: 0.13736
  number 6: 0.13608
  number 7: 0.13484
  number 8: 0.13363
  number 9: 0.13245
  number 10: 0.13131
str vector 'F49':
  number 1: 0.14142
  number 2: 0.14003
  number 3: 0.13868
  number 4: 0.13736
  number 5: 0.13608
  number 6: 0.13484
  number 7: 0.13363
  number 8: 0.13245
  number 9: 0.13131
  number 10: 0.13019
str vector 'F5':
  number 1: 0.40825
  number 2: 0.37796
  number 3: 0.35355
  number 4: 0.33333
  number 5: 0.31623
  number 6: 0.30151
  number 7: 0.28868
  number 8: 0.27735
  number 9: 0.26726
  number 10: 0.25820
str vector 'F50':
  number 1: 0.14003
  number 2: 0.13868
  number 3: 0.13736
  number 4: 0.13608
  number 5: 0.13484
  number 6: 0.13363
  number 7: 0.13245
  number 8: 0.13131
  number 9: 0.13019
  number 10: 0.12910
str vector 'F51':
  number 1: 0.13868
  number 2: 0.13736
  number 3: 0.13608
  number 4: 0.13484
  number 5: 0.13363
  number 6: 0.13245
  number 7: 0.13131
  number 8: 0.13019
  number 9: 0.12910
  number 10: 0.12804
str vector 'F52':
  number 1: 0.13736
  number 2: 0.13608
  number 3: 0.13484
  number 4: 0.13363
  number 5: 0.13245
  number 6: 0.13131
  number 7: 0.13019
  number 8: 0.12910
  number 9: 0.12804
  number 10: 0.12700
str vector 'F53':
  number 1: 0.13608
  number 2: 0.13484
  number 3: 0.13363
  number 4: 0.13245
  number 5: 0.13131
  number 6: 0.13019
  number 7: 0.12910
  number 8: 0.12804
  number 9: 0.12700
  number 10: 0.12599
str vector 'F54':
  number 1: 0.13484
  number 2: 0.13363
  number 3: 0.13245
  number 4: 0.13131
  number 5: 0.13019
  number 6: 0.12910
  number 7: 0.12804
  number 8: 0.12700
  number 9: 0.12599
  number 10: 0.12500
str vector 'F55':
  number 1: 0.13363
  number 2: 0.13245
  number 3: 0.13131
  number 4: 0.13019
  number 5: 0.12910
  number 6: 0.12804
  number 7: 0.12700
  number 8: 0.12599
  number 9: 0.12500
  number 10: 0.12403
str vector 'F56':
  number 1: 0.13245
  number 2: 0.13131
  number 3: 0.13019
  number 4: 0.12910
  number 5: 0.12804
  number 6: 0.12700
  number 7: 0.12599
  number 8: 0.12500
  number 9: 0.12403
  number 10: 0.12309
str vector 'F57':
  number 1: 0.13131
  number 2: 0.13019
  number 3: 0.12910
  number 4: 0.12804
  number 5: 0.12700
  number 6: 0.12599
  number 7: 0.12500
  number 8: 0.12403
  number 9: 0.12309
  number 10: 0.12217
str vector 'F58':
  number 1: 0.13019
  number 2: 0.12910
  number 3: 0.12804
  number 4: 0.12700
  number 5: 0.12599
  number 6: 0.12500
  number 7: 0.12403
  number 8: 0.12309
  number 9: 0.12217
  number 10: 0.12127
str vector 'F59':
  number 1: 0.12910
  number 2: 0.12804
  number 3: 0.12700
  number 4: 0.12599
  number 5: 0.12500
  number 6: 0.12403
  number 7: 0.12309
  number 8: 0.12217
  number 9: 0.12127
  number 10: 0.12039
str vector 'F6':
  number 1: 0.37796
  number 2: 0.35355
  number 3: 0.33333
  number 4: 0.31623
  number 5: 0.30151
  number 6: 0.28868
  number 7: 0.27735
  number 8: 0.26726
  number 9: 0.25820
  number 10: 0.25000
str vector 'F60':
  number 1: 0.12804
  number 2: 0.12700
  number 3: 0.12599
  number 4: 0.12500
  number 5: 0.12403
  number 6: 0.12309
  number 7: 0.12217
  number 8: 0.12127
  number 9: 0.12039
  number 10: 0.11952
str vector 'F61':
  number 1: 0.12700
  number 2: 0.12599
  number 3: 0.12500
  number 4: 0.12403
  number 5: 0.12309
  number 6: 0.12217
  number 7: 0.12127
  number 8: 0.12039
  number 9: 0.11952
  number 10: 0.11868
str vector 'F62':
  number 1: 0.12599
  number 2: 0.12500
  number 3: 0.12403
  number 4: 0.12309
  number 5: 0.12217
  number 6: 0.12127
  number 7: 0.12039
  number 8: 0.11952
  number 9: 0.11868
  number 10: 0.11785
str vector 'F63':
  number 1: 0.12500
  number 2: 0.12403
  number 3: 0.12309
  number 4: 0.12217
  number 5: 0.12127
  number 6: 0.12039
  number 7: 0.11952
  number 8: 0.11868
  number 9: 0.11785
  number 10: 0.11704
str vector 'F64':
  number 1: 0.12403
  number 2: 0.12309
  number 3: 0.12217
  number 4: 0.12127
  number 5: 0.12039
  number 6: 0.11952
  number 7: 0.11868
  number 8: 0.11785
  number 9: 0.11704
  number 10: 0.11625
str vector 'F65':
  number 1: 0.12309
  number 2: 0.12217
  number 3: 0.12127
  number 4: 0.12039
  number 5: 0.11952
  number 6: 0.11868
  number 7: 0.11785
  number 8: 0.11704
  number 9: 0.11625
  number 10: 0.11547
str vector 'F66':
  number 1: 0.12217
  number 2: 0.12127
  number 3: 0.12039
  number 4: 0.11952
  number 5: 0.11868
  number 6: 0.11785
  number 7: 0.11704
  number 8: 0.11625
  number 9: 0.11547
  number 10: 0.11471
str vector 'F67':
  number 1: 0.12127
  number 2: 0.12039
  number 3: 0.11952
  number 4: 0.11868
  number 5: 0.11785
  number 6: 0.11704
  number 7: 0.11625
  number 8: 0.11547
  number 9: 0.11471
  number 10: 0.11396
str vector 'F68':
  number 1: 0.12039
  number 2: 0.11952
  number 3: 0.11868
  number 4: 0.11785
  number 5: 0.11704
  number 6: 0.11625
  number 7: 0.11547
  number 8: 0.11471
  number 9: 0.11396
  number 10: 0.11323
str vector 'F69':
  number 1: 0.11952
  number 2: 0.11868
  number 3: 0.11785
  number 4: 0.11704
  number 5: 0.11625
  number 6: 0.11547
  number 7: 0.11471
  number 8: 0.11396
  number 9: 0.11323
  number 10: 0.11251
str vector 'F7':
  number 1: 0.35355
  number 2: 0.33333
  number 3: 0.31623
  number 4: 0.30151
  number 5: 0.28868
  number 6: 0.27735
  number 7: 0.26726
  number 8: 0.25820
  number 9: 0.25000
  number 10: 0.24254
str vector 'F70':
  number 1: 0.11868
  number 2: 0.11785
  number 3: 0.11704
  number 4: 0.11625
  number 5: 0.11547
  number 6: 0.11471
  number 7: 0.11396
  number 8: 0.11323
  number 9: 0.11251
  number 10: 0.11180
str vector 'F71':
  number 1: 0.11785
  number 2: 0.11704
  number 3: 0.11625
  number 4: 0.11547
  number 5: 0.11471
  number 6: 0.11396
  number 7: 0.11323
  number 8: 0.11251
  number 9: 0.11180
  number 10: 0.11111
str vector 'F72':
  number 1: 0.11704
  number 2: 0.11625
  number 3: 0.11547
  number 4: 0.11471
  number 5: 0.11396
  number 6: 0.11323
  number 7: 0.11251
  number 8: 0.11180
  number 9: 0.11111
  number 10: 0.11043
str vector 'F73':
  number 1: 0.11625
  number 2: 0.11547
  number 3: 0.11471
  number 4: 0.11396
  number 5: 0.11323
  number 6: 0.11251
  number 7: 0.11180
  number 8: 0.11111
  number 9: 0.11043
  number 10: 0.10976
str vector 'F74':
  number 1: 0.11547
  number 2: 0.11471
  number 3: 0.11396
  number 4: 0.11323
  number 5: 0.11251
  number 6: 0.11180
  number 7: 0.11111
  number 8: 0.11043
  number 9: 0.10976
  number 10: 0.10911
str vector 'F75':
  number 1: 0.11471
  number 2: 0.11396
  number 3: 0.11323
  number 4: 0.11251
  number 5: 0.11180
  number 6: 0.11111
  number 7: 0.11043
  number 8: 0.10976
  number 9: 0.10911
  number 10: 0.10847
str vector 'F76':
  number 1: 0.11396
  number 2: 0.11323
  number 3: 0.11251
  number 4: 0.11180
  number 5: 0.11111
  number 6: 0.11043
  number 7: 0.10976
  number 8: 0.10911
  number 9: 0.10847
  number 10: 0.10783
str vector 'F77':
  number 1: 0.11323
  number 2: 0.11251
  number 3: 0.11180
  number 4: 0.11111
  number 5: 0.11043
  number 6: 0.10976
  number 7: 0.10911
  number 8: 0.10847
  number 9: 0.10783
  number 10: 0.10721
str vector 'F78':
  number 1: 0.11251
  number 2: 0.11180
  number 3: 0.11111
  number 4: 0.11043
  number 5: 0.10976
  number 6: 0.10911
  number 7: 0.10847
  number 8: 0.10783
  number 9: 0.10721
  number 10: 0.10660
str vector 'F79':
  number 1: 0.11180
  number 2: 0.11111
  number 3: 0.11043
  number 4: 0.10976
  number 5: 0.10911
  number 6: 0.10847
  number 7: 0.10783
  number 8: 0.10721
  number 9: 0.10660
  number 10: 0.10600
str vector 'F8':
  number 1: 0.33333
  number 2: 0.31623
  number 3: 0.30151
  number 4: 0.28868
  number 5: 0.27735
  number 6: 0.26726
  number 7: 0.25820
  number 8: 0.25000
  number 9: 0.24254
  number 10: 0.23570
str vector 'F80':
  number 1: 0.11111
  number 2: 0.11043
  number 3: 0.10976
  number 4: 0.10911
  number 5: 0.10847
  number 6: 0.10783
  number 7: 0.10721
  number 8: 0.10660
  number 9: 0.10600
  number 10: 0.10541
str vector 'F81':
  number 1: 0.11043
  number 2: 0.10976
  number 3: 0.10911
  number 4: 0.10847
  number 5: 0.10783
  number 6: 0.10721
  number 7: 0.10660
  number 8: 0.10600
  number 9: 0.10541
  number 10: 0.10483
str vector 'F82':
  number 1: 0.10976
  number 2: 0.10911
  number 3: 0.10847
  number 4: 0.10783
  number 5: 0.10721
  number 6: 0.10660
  number 7: 0.10600
  number 8: 0.10541
  number 9: 0.10483
  number 10: 0.10426
str vector 'F83':
  number 1: 0.10911
  number 2: 0.10847
  number 3: 0.10783
  number 4: 0.10721
  number 5: 0.10660
  number 6: 0.10600
  number 7: 0.10541
  number 8: 0.10483
  number 9: 0.10426
  number 10: 0.10370
str vector 'F84':
  number 1: 0.10847
  number 2: 0.10783
  number 3: 0.10721
  number 4: 0.10660
  number 5: 0.10600
  number 6: 0.10541
  number 7: 0.10483
  number 8: 0.10426
  number 9: 0.10370
  number 10: 0.10314
str vector 'F85':
  number 1: 0.10783
  number 2: 0.10721
  number 3: 0.10660
  number 4: 0.10600
  number 5: 0.10541
  number 6: 0.10483
  number 7: 0.10426
  number 8: 0.10370
  number 9: 0.10314
  number 10: 0.10260
str vector 'F86':
  number 1: 0.10721
  number 2: 0.10660
  number 3: 0.10600
  number 4: 0.10541
  number 5: 0.10483
  number 6: 0.10426
  number 7: 0.10370
  number 8: 0.10314
  number 9: 0.10260
  number 10: 0.10206
str vector 'F87':
  number 1: 0.10660
  number 2: 0.10600
  number 3: 0.10541
  number 4: 0.10483
  number 5: 0.10426
  number 6: 0.10370
  number 7: 0.10314
  number 8: 0.10260
  number 9: 0.10206
  number 10: 0.10153
str vector 'F88':
  number 1: 0.10600
  number 2: 0.10541
  number 3: 0.10483
  number 4: 0.10426
  number 5: 0.10370
  number 6: 0.10314
  number 7: 0.10260
  number 8: 0.10206
  number 9: 0.10153
  number 10: 0.10102
str vector 'F89':
  number 1: 0.10541
  number 2: 0.10483
  number 3: 0.10426
  number 4: 0.10370
  number 5: 0.10314
  number 6: 0.10260
  number 7: 0.10206
  number 8: 0.10153
  number 9: 0.10102
  number 10: 0.10050
str vector 'F9':
  number 1: 0.31623
  number 2: 0.30151
  number 3: 0.28868
  number 4: 0.27735
  number 5: 0.26726
  number 6: 0.25820
  number 7: 0.25000
  number 8: 0.24254
  number 9: 0.23570
  number 10: 0.22942
str vector 'F90':
  number 1: 0.10483
  number 2: 0.10426
  number 3: 0.10370
  number 4: 0.10314
  number 5: 0.10260
  number 6: 0.10206
  number 7: 0.10153
  number 8: 0.10102
  number 9: 0.10050
  number 10: 0.10000
str vector 'F91':
  number 1: 0.10426
  number 2: 0.10370
  number 3: 0.10314
  number 4: 0.10260
  number 5: 0.10206
  number 6: 0.10153
  number 7: 0.10102
  number 8: 0.10050
  number 9: 0.10000
  number 10: 0.09950
str vector 'F92':
  number 1: 0.10370
  number 2: 0.10314
  number 3: 0.10260
  number 4: 0.10206
  number 5: 0.10153
  number 6: 0.10102
  number 7: 0.10050
  number 8: 0.10000
  number 9: 0.09950
  number 10: 0.09901
str vector 'F93':
  number 1: 0.10314
  number 2: 0.10260
  number 3: 0.10206
  number 4: 0.10153
  number 5: 0.10102
  number 6: 0.10050
  number 7: 0.10000
  number 8: 0.09950
  number 9: 0.09901
  number 10: 0.09853
str vector 'F94':
  number 1: 0.10260
  number 2: 0.10206
  number 3: 0.10153
  number 4: 0.10102
  number 5: 0.10050
  number 6: 0.10000
  number 7: 0.09950
  number 8: 0.09901
  number 9: 0.09853
  number 10: 0.09806
str vector 'F95':
  number 1: 0.10206
  number 2: 0.10153
  number 3: 0.10102
  number 4: 0.10050
  number 5: 0.10000
  number 6: 0.09950
  number 7: 0.09901
  number 8: 0.09853
  number 9: 0.09806
  number 10: 0.09759
str vector 'F96':
  number 1: 0.10153
  number 2: 0.10102
  number 3: 0.10050
  number 4: 0.10000
  number 5: 0.09950
  number 6: 0.09901
  number 7: 0.09853
  number 8: 0.09806
  number 9: 0.09759
  number 10: 0.09713
str vector 'F97':
  number 1: 0.10102
  number 2: 0.10050
  number 3: 0.10000
  number 4: 0.09950
  number 5: 0.09901
  number 6: 0.09853
  number 7: 0.09806
  number 8: 0.09759
  number 9: 0.09713
  number 10: 0.09667
str vector 'F98':
  number 1: 0.10050
  number 2: 0.10000
  number 3: 0.09950
  number 4: 0.09901
  number 5: 0.09853
  number 6: 0.09806
  number 7: 0.09759
  number 8: 0.09713
  number 9: 0.09667
  number 10: 0.09623
str vector 'F99':
  number 1: 0.10000
  number 2: 0.09950
  number 3: 0.09901
  number 4: 0.09853
  number 5: 0.09806
  number 6: 0.09759
  number 7: 0.09713
  number 8: 0.09667
  number 9: 0.09623
  number 10: 0.09578
"""

verifyTestOutput( outputdir, result, expected)

