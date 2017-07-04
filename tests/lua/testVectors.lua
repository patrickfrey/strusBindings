require "string"
require "math"
require "utils"
require "dumpVectorStorage"

local outputdir = arg[1] or '.'
local storage = outputdir .. "/storage"
local config = {
	path=storage,
	['commit']=10,
	dim=10,
	bit=8,
	var=100,
	simdist=12,
	maxdist=20,
	realvecweights=true
}
local vectors = {}

for vi=1,100 do
	vv = {}
	for xi=1,10 do
		table.insert( vv, 1.0 / math.sqrt(vi + xi))
	end
	table.insert( vectors, vv)
end

local ctx = strus_Context.new()
ctx:loadModule( "storage_vector_std")

ctx:destroyStorage( config)
ctx:getLastError()

ctx:createVectorStorage( config);
local storage = ctx:createVectorStorageClient( config)
local transaction = storage:createTransaction()
for iv,vv in ipairs( vectors) do
	transaction:addFeature( string.format( "F%u", iv), vv)
	transaction:defineFeatureConceptRelation( "main", iv-1, iv)
end
transaction:commit()
transaction:close()
local output = dumpVectorStorage( ctx, config)

local storage = ctx:createVectorStorageClient( config)
local searcher = storage:createSearcher( 0, storage:nofFeatures())
local simlist = searcher:findSimilar( {0.1,0.1,0.1,0.1,0.1, 0.1,0.1,0.1,0.1,0.1}, 10)
output[ 'simlist 0.1x10'] = simlist

local result = "vector storage dump:" .. dumpTree( output) .. "\n"
local expected = [[
vector storage dump:
string class 'main' feature 0:
  number 1: 1
string class 'main' feature 1:
  number 1: 2
string class 'main' feature 10:
  number 1: 11
string class 'main' feature 11:
  number 1: 12
string class 'main' feature 12:
  number 1: 13
string class 'main' feature 13:
  number 1: 14
string class 'main' feature 14:
  number 1: 15
string class 'main' feature 15:
  number 1: 16
string class 'main' feature 16:
  number 1: 17
string class 'main' feature 17:
  number 1: 18
string class 'main' feature 18:
  number 1: 19
string class 'main' feature 19:
  number 1: 20
string class 'main' feature 2:
  number 1: 3
string class 'main' feature 20:
  number 1: 21
string class 'main' feature 21:
  number 1: 22
string class 'main' feature 22:
  number 1: 23
string class 'main' feature 23:
  number 1: 24
string class 'main' feature 24:
  number 1: 25
string class 'main' feature 25:
  number 1: 26
string class 'main' feature 26:
  number 1: 27
string class 'main' feature 27:
  number 1: 28
string class 'main' feature 28:
  number 1: 29
string class 'main' feature 29:
  number 1: 30
string class 'main' feature 3:
  number 1: 4
string class 'main' feature 30:
  number 1: 31
string class 'main' feature 31:
  number 1: 32
string class 'main' feature 32:
  number 1: 33
string class 'main' feature 33:
  number 1: 34
string class 'main' feature 34:
  number 1: 35
string class 'main' feature 35:
  number 1: 36
string class 'main' feature 36:
  number 1: 37
string class 'main' feature 37:
  number 1: 38
string class 'main' feature 38:
  number 1: 39
string class 'main' feature 39:
  number 1: 40
string class 'main' feature 4:
  number 1: 5
string class 'main' feature 40:
  number 1: 41
string class 'main' feature 41:
  number 1: 42
string class 'main' feature 42:
  number 1: 43
string class 'main' feature 43:
  number 1: 44
string class 'main' feature 44:
  number 1: 45
string class 'main' feature 45:
  number 1: 46
string class 'main' feature 46:
  number 1: 47
string class 'main' feature 47:
  number 1: 48
string class 'main' feature 48:
  number 1: 49
string class 'main' feature 49:
  number 1: 50
string class 'main' feature 5:
  number 1: 6
string class 'main' feature 50:
  number 1: 51
string class 'main' feature 51:
  number 1: 52
string class 'main' feature 52:
  number 1: 53
string class 'main' feature 53:
  number 1: 54
string class 'main' feature 54:
  number 1: 55
string class 'main' feature 55:
  number 1: 56
string class 'main' feature 56:
  number 1: 57
string class 'main' feature 57:
  number 1: 58
string class 'main' feature 58:
  number 1: 59
string class 'main' feature 59:
  number 1: 60
string class 'main' feature 6:
  number 1: 7
string class 'main' feature 60:
  number 1: 61
string class 'main' feature 61:
  number 1: 62
string class 'main' feature 62:
  number 1: 63
string class 'main' feature 63:
  number 1: 64
string class 'main' feature 64:
  number 1: 65
string class 'main' feature 65:
  number 1: 66
string class 'main' feature 66:
  number 1: 67
string class 'main' feature 67:
  number 1: 68
string class 'main' feature 68:
  number 1: 69
string class 'main' feature 69:
  number 1: 70
string class 'main' feature 7:
  number 1: 8
string class 'main' feature 70:
  number 1: 71
string class 'main' feature 71:
  number 1: 72
string class 'main' feature 72:
  number 1: 73
string class 'main' feature 73:
  number 1: 74
string class 'main' feature 74:
  number 1: 75
string class 'main' feature 75:
  number 1: 76
string class 'main' feature 76:
  number 1: 77
string class 'main' feature 77:
  number 1: 78
string class 'main' feature 78:
  number 1: 79
string class 'main' feature 79:
  number 1: 80
string class 'main' feature 8:
  number 1: 9
string class 'main' feature 80:
  number 1: 81
string class 'main' feature 81:
  number 1: 82
string class 'main' feature 82:
  number 1: 83
string class 'main' feature 83:
  number 1: 84
string class 'main' feature 84:
  number 1: 85
string class 'main' feature 85:
  number 1: 86
string class 'main' feature 86:
  number 1: 87
string class 'main' feature 87:
  number 1: 88
string class 'main' feature 88:
  number 1: 89
string class 'main' feature 89:
  number 1: 90
string class 'main' feature 9:
  number 1: 10
string class 'main' feature 90:
  number 1: 91
string class 'main' feature 91:
  number 1: 92
string class 'main' feature 92:
  number 1: 93
string class 'main' feature 93:
  number 1: 94
string class 'main' feature 94:
  number 1: 95
string class 'main' feature 95:
  number 1: 96
string class 'main' feature 96:
  number 1: 97
string class 'main' feature 97:
  number 1: 98
string class 'main' feature 98:
  number 1: 99
string class 'main' feature 99:
  number 1: 100
string classes:
  number 1: ""
  number 2: "main"
string config:
  string assignments: "7"
  string baff: "0.10000"
  string bit: "8"
  string commit: "10"
  string descendants: "10"
  string dim: "10"
  string eqdiff: "0.25000"
  string eqdist: "2"
  string fdf: "0.25000"
  string forcesim: "no"
  string greediness: "3"
  string isaf: "0.60000"
  string iterations: "20"
  string logfile: ""
  string maxage: "20"
  string maxconcepts: "0"
  string maxdist: "20"
  string maxfeatures: "0"
  string maxsimsam: "0"
  string mutations: "50"
  string path: "storage"
  string probdist: "18"
  string probsim: "yes"
  string raddist: "12"
  string realvecweights: "yes"
  string rndsimsam: "0"
  string simdist: "12"
  string singletons: "0"
  string threads: "0"
  string var: "100"
  string votes: "13"
string simlist 0.1x10:
  number 1:
    string featidx: 98
    string weight: 0.99991
  number 2:
    string featidx: 97
    string weight: 0.99990
  number 3:
    string featidx: 96
    string weight: 0.99990
  number 4:
    string featidx: 95
    string weight: 0.99990
  number 5:
    string featidx: 94
    string weight: 0.99990
  number 6:
    string featidx: 93
    string weight: 0.99990
  number 7:
    string featidx: 92
    string weight: 0.99989
  number 8:
    string featidx: 91
    string weight: 0.99989
  number 9:
    string featidx: 90
    string weight: 0.99989
  number 10:
    string featidx: 89
    string weight: 0.99989
string vector 'F1':
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
string vector 'F10':
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
string vector 'F100':
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
string vector 'F11':
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
string vector 'F12':
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
string vector 'F13':
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
string vector 'F14':
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
string vector 'F15':
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
string vector 'F16':
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
string vector 'F17':
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
string vector 'F18':
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
string vector 'F19':
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
string vector 'F2':
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
string vector 'F20':
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
string vector 'F21':
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
string vector 'F22':
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
string vector 'F23':
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
string vector 'F24':
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
string vector 'F25':
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
string vector 'F26':
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
string vector 'F27':
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
string vector 'F28':
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
string vector 'F29':
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
string vector 'F3':
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
string vector 'F30':
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
string vector 'F31':
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
string vector 'F32':
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
string vector 'F33':
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
string vector 'F34':
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
string vector 'F35':
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
string vector 'F36':
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
string vector 'F37':
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
string vector 'F38':
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
string vector 'F39':
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
string vector 'F4':
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
string vector 'F40':
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
string vector 'F41':
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
string vector 'F42':
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
string vector 'F43':
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
string vector 'F44':
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
string vector 'F45':
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
string vector 'F46':
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
string vector 'F47':
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
string vector 'F48':
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
string vector 'F49':
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
string vector 'F5':
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
string vector 'F50':
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
string vector 'F51':
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
string vector 'F52':
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
string vector 'F53':
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
string vector 'F54':
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
string vector 'F55':
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
string vector 'F56':
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
string vector 'F57':
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
string vector 'F58':
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
string vector 'F59':
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
string vector 'F6':
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
string vector 'F60':
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
string vector 'F61':
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
string vector 'F62':
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
string vector 'F63':
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
string vector 'F64':
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
string vector 'F65':
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
string vector 'F66':
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
string vector 'F67':
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
string vector 'F68':
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
string vector 'F69':
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
string vector 'F7':
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
string vector 'F70':
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
string vector 'F71':
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
string vector 'F72':
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
string vector 'F73':
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
string vector 'F74':
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
string vector 'F75':
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
string vector 'F76':
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
string vector 'F77':
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
string vector 'F78':
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
string vector 'F79':
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
string vector 'F8':
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
string vector 'F80':
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
string vector 'F81':
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
string vector 'F82':
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
string vector 'F83':
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
string vector 'F84':
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
string vector 'F85':
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
string vector 'F86':
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
string vector 'F87':
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
string vector 'F88':
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
string vector 'F89':
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
string vector 'F9':
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
string vector 'F90':
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
string vector 'F91':
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
string vector 'F92':
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
string vector 'F93':
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
string vector 'F94':
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
string vector 'F95':
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
string vector 'F96':
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
string vector 'F97':
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
string vector 'F98':
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
string vector 'F99':
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
]]

verifyTestOutput( outputdir, result, expected)

