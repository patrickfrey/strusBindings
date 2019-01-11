import strus
import math
import random
from utils import *
from dumpVectorStorage import *

if len(sys.argv) > 1:
	outputdir = sys.argv[1]
else:
	outputdir = "."

storage = outputdir + "/storage"
config = {
	'path':storage,
	'vecdim':10,
	'simdist':20,
	'probsimdist':40
}
vectors = []

random.seed(123)

examplevec = [0.1,0.1,0.1,0.1,0.1, 0.1,0.1,0.1,0.1,0.1]
for vi in range( 1,101):
	vv = []
	for xi in range(1,11):
		r = random.random()
		vv.append( examplevec[ xi-1] - r / 20)
	vectors.append( vv)

ctx = strus.Context()
ctx.loadModule( "storage_vector_std")

if ctx.storageExists( config):
	ctx.destroyStorage( config)

ctx.createVectorStorage( config);
storage = ctx.createVectorStorageClient( config)
transaction = storage.createTransaction()
for iv,vv in enumerate( vectors):
	transaction.defineVector( "word", "F%u" % (iv+1), vv)
	if (iv+1) % 2 == 1:
		transaction.defineFeature( "nonvec", "F%u" % (iv+1))

transaction.commit()
transaction.close()
output = dumpVectorStorage( ctx, config, vectors, examplevec)

storage = ctx.createVectorStorageClient( config)
searcher = storage.createSearcher( "word", 0, 1)
simlist = searcher.findSimilar( examplevec, 10, 0.85, True)
output[ 'simlist 0.1x10'] = simlist

result = "vector storage dump:" + dumpTree( output) + "\n"
expected = """vector storage dump:
str config:
  str path: "storage"
  str probsimdist: "40"
  str simdist: "20"
  str vecdim: "10"
str nof vec nonvec: 0
str nof vec word: 100
str rank   0: "0.9974 F9 {0.09326, 0.09034, 0.08718, 0.08240, 0.08640, 0.07829, 0.07898, 0.09457, 0.09687, 0.08225}"
str rank   1: "0.9946 F2 {0.08314, 0.08331, 0.08774, 0.09992, 0.07819, 0.09562, 0.07012, 0.09651, 0.08423, 0.07758}"
str rank   2: "0.9945 F56 {0.08307, 0.09154, 0.08126, 0.07070, 0.07546, 0.08138, 0.07893, 0.09114, 0.09476, 0.06713}"
str rank   3: "0.9941 F55 {0.07603, 0.08142, 0.05960, 0.07673, 0.06761, 0.08579, 0.07307, 0.07187, 0.07067, 0.06070}"
str rank   4: "0.9926 F83 {0.07675, 0.07476, 0.07820, 0.08449, 0.06197, 0.06814, 0.09293, 0.08329, 0.07795, 0.06258}"
str simlist 0.1x10:
  number 1:
    str value: "F9"
    str weight: 0.99744
  number 2:
    str value: "F2"
    str weight: 0.99458
  number 3:
    str value: "F56"
    str weight: 0.99449
  number 4:
    str value: "F55"
    str weight: 0.99413
  number 5:
    str value: "F83"
    str weight: 0.99265
  number 6:
    str value: "F69"
    str weight: 0.99214
  number 7:
    str value: "F86"
    str weight: 0.99157
  number 8:
    str value: "F26"
    str weight: 0.99138
  number 9:
    str value: "F61"
    str weight: 0.99066
  number 10:
    str value: "F85"
    str weight: 0.99008
str types:
  number 1: "nonvec"
  number 2: "word"
str types F1:
  number 1: "word"
  number 2: "nonvec"
str types F10:
  number 1: "word"
str types F100:
  number 1: "word"
str types F11:
  number 1: "word"
  number 2: "nonvec"
str types F12:
  number 1: "word"
str types F13:
  number 1: "word"
  number 2: "nonvec"
str types F14:
  number 1: "word"
str types F15:
  number 1: "word"
  number 2: "nonvec"
str types F16:
  number 1: "word"
str types F17:
  number 1: "word"
  number 2: "nonvec"
str types F18:
  number 1: "word"
str types F19:
  number 1: "word"
  number 2: "nonvec"
str types F2:
  number 1: "word"
str types F20:
  number 1: "word"
str types F21:
  number 1: "word"
  number 2: "nonvec"
str types F22:
  number 1: "word"
str types F23:
  number 1: "word"
  number 2: "nonvec"
str types F24:
  number 1: "word"
str types F25:
  number 1: "word"
  number 2: "nonvec"
str types F26:
  number 1: "word"
str types F27:
  number 1: "word"
  number 2: "nonvec"
str types F28:
  number 1: "word"
str types F29:
  number 1: "word"
  number 2: "nonvec"
str types F3:
  number 1: "word"
  number 2: "nonvec"
str types F30:
  number 1: "word"
str types F31:
  number 1: "word"
  number 2: "nonvec"
str types F32:
  number 1: "word"
str types F33:
  number 1: "word"
  number 2: "nonvec"
str types F34:
  number 1: "word"
str types F35:
  number 1: "word"
  number 2: "nonvec"
str types F36:
  number 1: "word"
str types F37:
  number 1: "word"
  number 2: "nonvec"
str types F38:
  number 1: "word"
str types F39:
  number 1: "word"
  number 2: "nonvec"
str types F4:
  number 1: "word"
str types F40:
  number 1: "word"
str types F41:
  number 1: "word"
  number 2: "nonvec"
str types F42:
  number 1: "word"
str types F43:
  number 1: "word"
  number 2: "nonvec"
str types F44:
  number 1: "word"
str types F45:
  number 1: "word"
  number 2: "nonvec"
str types F46:
  number 1: "word"
str types F47:
  number 1: "word"
  number 2: "nonvec"
str types F48:
  number 1: "word"
str types F49:
  number 1: "word"
  number 2: "nonvec"
str types F5:
  number 1: "word"
  number 2: "nonvec"
str types F50:
  number 1: "word"
str types F51:
  number 1: "word"
  number 2: "nonvec"
str types F52:
  number 1: "word"
str types F53:
  number 1: "word"
  number 2: "nonvec"
str types F54:
  number 1: "word"
str types F55:
  number 1: "word"
  number 2: "nonvec"
str types F56:
  number 1: "word"
str types F57:
  number 1: "word"
  number 2: "nonvec"
str types F58:
  number 1: "word"
str types F59:
  number 1: "word"
  number 2: "nonvec"
str types F6:
  number 1: "word"
str types F60:
  number 1: "word"
str types F61:
  number 1: "word"
  number 2: "nonvec"
str types F62:
  number 1: "word"
str types F63:
  number 1: "word"
  number 2: "nonvec"
str types F64:
  number 1: "word"
str types F65:
  number 1: "word"
  number 2: "nonvec"
str types F66:
  number 1: "word"
str types F67:
  number 1: "word"
  number 2: "nonvec"
str types F68:
  number 1: "word"
str types F69:
  number 1: "word"
  number 2: "nonvec"
str types F7:
  number 1: "word"
  number 2: "nonvec"
str types F70:
  number 1: "word"
str types F71:
  number 1: "word"
  number 2: "nonvec"
str types F72:
  number 1: "word"
str types F73:
  number 1: "word"
  number 2: "nonvec"
str types F74:
  number 1: "word"
str types F75:
  number 1: "word"
  number 2: "nonvec"
str types F76:
  number 1: "word"
str types F77:
  number 1: "word"
  number 2: "nonvec"
str types F78:
  number 1: "word"
str types F79:
  number 1: "word"
  number 2: "nonvec"
str types F8:
  number 1: "word"
str types F80:
  number 1: "word"
str types F81:
  number 1: "word"
  number 2: "nonvec"
str types F82:
  number 1: "word"
str types F83:
  number 1: "word"
  number 2: "nonvec"
str types F84:
  number 1: "word"
str types F85:
  number 1: "word"
  number 2: "nonvec"
str types F86:
  number 1: "word"
str types F87:
  number 1: "word"
  number 2: "nonvec"
str types F88:
  number 1: "word"
str types F89:
  number 1: "word"
  number 2: "nonvec"
str types F9:
  number 1: "word"
  number 2: "nonvec"
str types F90:
  number 1: "word"
str types F91:
  number 1: "word"
  number 2: "nonvec"
str types F92:
  number 1: "word"
str types F93:
  number 1: "word"
  number 2: "nonvec"
str types F94:
  number 1: "word"
str types F95:
  number 1: "word"
  number 2: "nonvec"
str types F96:
  number 1: "word"
str types F97:
  number 1: "word"
  number 2: "nonvec"
str types F98:
  number 1: "word"
str types F99:
  number 1: "word"
  number 2: "nonvec"
str vec F1: "{0.09738, 0.09564, 0.07964, 0.09461, 0.05494, 0.09809, 0.07319, 0.08339, 0.05740, 0.09202}"
str vec F1 example sim: 0.98315
str vec F10: "{0.09556, 0.07834, 0.09957, 0.07321, 0.09633, 0.06529, 0.07979, 0.05073, 0.08687, 0.07637}"
str vec F10 example sim: 0.98426
str vec F100: "{0.08952, 0.08859, 0.05637, 0.09365, 0.08762, 0.09316, 0.07089, 0.08088, 0.06257, 0.05394}"
str vec F100 example sim: 0.98262
str vec F11: "{0.06246, 0.08326, 0.08258, 0.06804, 0.05152, 0.05581, 0.07905, 0.06411, 0.06605, 0.08139}"
str vec F11 example sim: 0.98781
str vec F12: "{0.08825, 0.09608, 0.05605, 0.09714, 0.07407, 0.07312, 0.09269, 0.06720, 0.05421, 0.06119}"
str vec F12 example sim: 0.97936
str vec F13: "{0.08054, 0.09368, 0.05125, 0.07414, 0.06852, 0.05989, 0.08069, 0.09690, 0.09320, 0.06658}"
str vec F13 example sim: 0.98250
str vec F14: "{0.08252, 0.05645, 0.06299, 0.05098, 0.06656, 0.08481, 0.06044, 0.09859, 0.07392, 0.05643}"
str vec F14 example sim: 0.97900
str vec F15: "{0.07426, 0.09313, 0.07499, 0.07273, 0.08155, 0.05533, 0.08300, 0.08641, 0.07468, 0.08984}"
str vec F15 example sim: 0.99161
str vec F16: "{0.08390, 0.08009, 0.05309, 0.09598, 0.09015, 0.09003, 0.05024, 0.05546, 0.09029, 0.07177}"
str vec F16 example sim: 0.97741
str vec F17: "{0.09574, 0.08415, 0.08294, 0.07312, 0.05353, 0.05955, 0.07329, 0.06070, 0.07580, 0.06347}"
str vec F17 example sim: 0.98552
str vec F18: "{0.06115, 0.08716, 0.05836, 0.07110, 0.07569, 0.08021, 0.08879, 0.07494, 0.09369, 0.06826}"
str vec F18 example sim: 0.98946
str vec F19: "{0.05509, 0.06047, 0.06954, 0.07528, 0.06521, 0.09388, 0.09738, 0.05692, 0.08191, 0.06655}"
str vec F19 example sim: 0.98173
str vec F2: "{0.08314, 0.08331, 0.08774, 0.09992, 0.07819, 0.09562, 0.07012, 0.09651, 0.08423, 0.07758}"
str vec F2 example sim: 0.99458
str vec F20: "{0.07896, 0.08207, 0.06149, 0.08285, 0.06231, 0.05898, 0.09097, 0.05114, 0.09688, 0.07926}"
str vec F20 example sim: 0.98206
str vec F21: "{0.07498, 0.05853, 0.09126, 0.07205, 0.08157, 0.05392, 0.09388, 0.08673, 0.05901, 0.08389}"
str vec F21 example sim: 0.98413
str vec F22: "{0.05279, 0.09742, 0.08058, 0.09463, 0.07132, 0.05184, 0.08377, 0.06766, 0.08670, 0.07804}"
str vec F22 example sim: 0.98157
str vec F23: "{0.07142, 0.08758, 0.06548, 0.07495, 0.05473, 0.08625, 0.05226, 0.09089, 0.09900, 0.06699}"
str vec F23 example sim: 0.98090
str vec F24: "{0.06331, 0.07613, 0.09786, 0.05009, 0.09292, 0.05648, 0.05542, 0.08896, 0.05680, 0.07683}"
str vec F24 example sim: 0.97427
str vec F25: "{0.05502, 0.05562, 0.06129, 0.08556, 0.07099, 0.05093, 0.08397, 0.08067, 0.06560, 0.09687}"
str vec F25 example sim: 0.97904
str vec F26: "{0.08733, 0.09160, 0.07920, 0.07875, 0.06965, 0.07516, 0.09176, 0.09767, 0.07376, 0.06181}"
str vec F26 example sim: 0.99138
str vec F27: "{0.05366, 0.05802, 0.08486, 0.05672, 0.07663, 0.08588, 0.09539, 0.07786, 0.07275, 0.08604}"
str vec F27 example sim: 0.98388
str vec F28: "{0.09024, 0.05296, 0.09661, 0.05303, 0.07968, 0.06042, 0.09445, 0.07150, 0.06192, 0.07820}"
str vec F28 example sim: 0.97819
str vec F29: "{0.08218, 0.07534, 0.09028, 0.08942, 0.08793, 0.09812, 0.05450, 0.09633, 0.05364, 0.09945}"
str vec F29 example sim: 0.98202
str vec F3: "{0.05473, 0.09536, 0.09289, 0.06050, 0.09894, 0.05444, 0.07133, 0.08673, 0.05811, 0.06156}"
str vec F3 example sim: 0.97378
str vec F30: "{0.06719, 0.07265, 0.09791, 0.05293, 0.06346, 0.05604, 0.07822, 0.08543, 0.09070, 0.09436}"
str vec F30 example sim: 0.98058
str vec F31: "{0.06645, 0.05891, 0.08940, 0.09870, 0.08192, 0.09236, 0.08875, 0.08890, 0.06688, 0.07827}"
str vec F31 example sim: 0.98846
str vec F32: "{0.07198, 0.08050, 0.05082, 0.07515, 0.09787, 0.09304, 0.06344, 0.05437, 0.09370, 0.07699}"
str vec F32 example sim: 0.97991
str vec F33: "{0.05689, 0.07241, 0.09913, 0.07396, 0.08556, 0.08933, 0.05317, 0.09471, 0.05851, 0.06235}"
str vec F33 example sim: 0.97802
str vec F34: "{0.07036, 0.08376, 0.09586, 0.06656, 0.05204, 0.09101, 0.09369, 0.07624, 0.05743, 0.09973}"
str vec F34 example sim: 0.98020
str vec F35: "{0.06056, 0.09568, 0.09839, 0.09268, 0.07712, 0.05315, 0.06693, 0.09702, 0.09242, 0.07693}"
str vec F35 example sim: 0.98177
str vec F36: "{0.09549, 0.06658, 0.08205, 0.05220, 0.05901, 0.05915, 0.08824, 0.07843, 0.06642, 0.09553}"
str vec F36 example sim: 0.98028
str vec F37: "{0.09952, 0.08363, 0.07996, 0.06066, 0.05927, 0.09710, 0.09677, 0.05376, 0.08560, 0.09374}"
str vec F37 example sim: 0.98025
str vec F38: "{0.06037, 0.06075, 0.06977, 0.06163, 0.05702, 0.05734, 0.08684, 0.07896, 0.05078, 0.08161}"
str vec F38 example sim: 0.98537
str vec F39: "{0.06870, 0.05767, 0.05616, 0.06987, 0.08251, 0.06549, 0.09984, 0.08247, 0.06206, 0.06767}"
str vec F39 example sim: 0.98441
str vec F4: "{0.08283, 0.05993, 0.08966, 0.06955, 0.07365, 0.05953, 0.08424, 0.08009, 0.06154, 0.07436}"
str vec F4 example sim: 0.99047
str vec F40: "{0.09601, 0.06402, 0.06950, 0.07776, 0.09477, 0.07018, 0.06272, 0.09006, 0.06825, 0.09647}"
str vec F40 example sim: 0.98634
str vec F41: "{0.08973, 0.08209, 0.06110, 0.08584, 0.05548, 0.05085, 0.09183, 0.09866, 0.06356, 0.06178}"
str vec F41 example sim: 0.97643
str vec F42: "{0.05495, 0.06638, 0.06100, 0.08172, 0.09496, 0.05000, 0.06977, 0.08648, 0.05536, 0.09632}"
str vec F42 example sim: 0.97535
str vec F43: "{0.09716, 0.08726, 0.09350, 0.07946, 0.06895, 0.07403, 0.07231, 0.09403, 0.07325, 0.05124}"
str vec F43 example sim: 0.98586
str vec F44: "{0.08351, 0.06004, 0.06810, 0.06190, 0.06710, 0.07802, 0.08825, 0.09532, 0.09434, 0.09495}"
str vec F44 example sim: 0.98613
str vec F45: "{0.06163, 0.05813, 0.05912, 0.05576, 0.08206, 0.06630, 0.05899, 0.09338, 0.09998, 0.08952}"
str vec F45 example sim: 0.97634
str vec F46: "{0.05421, 0.05962, 0.09352, 0.05764, 0.09127, 0.08667, 0.09159, 0.05580, 0.05860, 0.09675}"
str vec F46 example sim: 0.97325
str vec F47: "{0.09881, 0.06044, 0.08032, 0.07158, 0.06809, 0.07282, 0.09060, 0.08448, 0.07013, 0.06079}"
str vec F47 example sim: 0.98784
str vec F48: "{0.08309, 0.09887, 0.09274, 0.08576, 0.05194, 0.07865, 0.07513, 0.07886, 0.05669, 0.05997}"
str vec F48 example sim: 0.98179
str vec F49: "{0.09230, 0.06670, 0.07251, 0.05512, 0.09686, 0.07109, 0.09051, 0.07153, 0.05914, 0.07830}"
str vec F49 example sim: 0.98474
str vec F5: "{0.06576, 0.06808, 0.06667, 0.05550, 0.07569, 0.06658, 0.08127, 0.09841, 0.06229, 0.09093}"
str vec F5 example sim: 0.98517
str vec F50: "{0.07326, 0.08174, 0.05276, 0.05688, 0.09552, 0.09786, 0.07325, 0.08443, 0.09844, 0.06624}"
str vec F50 example sim: 0.98054
str vec F51: "{0.09219, 0.06781, 0.09507, 0.06430, 0.05813, 0.06014, 0.05585, 0.06924, 0.08331, 0.09088}"
str vec F51 example sim: 0.98144
str vec F52: "{0.06711, 0.05129, 0.06710, 0.09480, 0.09782, 0.09239, 0.05986, 0.09320, 0.06470, 0.07081}"
str vec F52 example sim: 0.97835
str vec F53: "{0.08176, 0.07896, 0.08684, 0.07945, 0.09080, 0.07354, 0.06696, 0.07251, 0.05043, 0.05375}"
str vec F53 example sim: 0.98572
str vec F54: "{0.05549, 0.06980, 0.09280, 0.06206, 0.05282, 0.06997, 0.07730, 0.06724, 0.06865, 0.05862}"
str vec F54 example sim: 0.98690
str vec F55: "{0.07603, 0.08142, 0.05960, 0.07673, 0.06761, 0.08579, 0.07307, 0.07187, 0.07067, 0.06070}"
str vec F55 example sim: 0.99413
str vec F56: "{0.08307, 0.09154, 0.08126, 0.07070, 0.07546, 0.08138, 0.07893, 0.09114, 0.09476, 0.06713}"
str vec F56 example sim: 0.99449
str vec F57: "{0.09636, 0.07131, 0.05123, 0.07468, 0.06436, 0.06182, 0.07559, 0.07372, 0.08841, 0.06864}"
str vec F57 example sim: 0.98619
str vec F58: "{0.07374, 0.08324, 0.05311, 0.06842, 0.07919, 0.05487, 0.06183, 0.05150, 0.08311, 0.07596}"
str vec F58 example sim: 0.98557
str vec F59: "{0.08824, 0.06356, 0.05797, 0.06546, 0.06444, 0.09281, 0.05334, 0.09298, 0.05964, 0.09164}"
str vec F59 example sim: 0.97838
str vec F6: "{0.05899, 0.05432, 0.06710, 0.09149, 0.08315, 0.08044, 0.09634, 0.07716, 0.07134, 0.08319}"
str vec F6 example sim: 0.98625
str vec F60: "{0.06571, 0.06278, 0.09106, 0.07209, 0.08755, 0.05236, 0.09087, 0.05085, 0.07499, 0.09974}"
str vec F60 example sim: 0.97739
str vec F61: "{0.09221, 0.08249, 0.06617, 0.06808, 0.08876, 0.07866, 0.05757, 0.07638, 0.08407, 0.06811}"
str vec F61 example sim: 0.99066
str vec F62: "{0.05676, 0.06514, 0.05099, 0.07783, 0.09928, 0.09810, 0.05141, 0.08174, 0.06108, 0.05147}"
str vec F62 example sim: 0.96860
str vec F63: "{0.07259, 0.06633, 0.07190, 0.06353, 0.05889, 0.06524, 0.06672, 0.09837, 0.09367, 0.07756}"
str vec F63 example sim: 0.98618
str vec F64: "{0.05922, 0.05818, 0.07857, 0.05810, 0.08259, 0.06755, 0.09375, 0.05228, 0.08218, 0.08446}"
str vec F64 example sim: 0.98254
str vec F65: "{0.09643, 0.08971, 0.09765, 0.07208, 0.05116, 0.09950, 0.07570, 0.07898, 0.06479, 0.09679}"
str vec F65 example sim: 0.98251
str vec F66: "{0.09058, 0.08255, 0.08355, 0.07927, 0.07558, 0.08020, 0.08205, 0.05309, 0.09487, 0.08714}"
str vec F66 example sim: 0.99137
str vec F67: "{0.06538, 0.08159, 0.09050, 0.05672, 0.07889, 0.06773, 0.05393, 0.05355, 0.07816, 0.07763}"
str vec F67 example sim: 0.98532
str vec F68: "{0.09798, 0.09742, 0.05319, 0.05267, 0.08037, 0.05834, 0.06342, 0.08161, 0.09684, 0.06972}"
str vec F68 example sim: 0.97450
str vec F69: "{0.09395, 0.07778, 0.07500, 0.09835, 0.08977, 0.08565, 0.06485, 0.09241, 0.06975, 0.08197}"
str vec F69 example sim: 0.99214
str vec F7: "{0.09055, 0.09559, 0.06858, 0.09936, 0.08240, 0.08854, 0.07340, 0.06382, 0.05454, 0.06806}"
str vec F7 example sim: 0.98406
str vec F70: "{0.07328, 0.05672, 0.06922, 0.05808, 0.08960, 0.08796, 0.07621, 0.08892, 0.08439, 0.06117}"
str vec F70 example sim: 0.98668
str vec F71: "{0.09016, 0.07588, 0.08346, 0.06868, 0.07684, 0.05450, 0.07818, 0.05432, 0.09603, 0.08032}"
str vec F71 example sim: 0.98582
str vec F72: "{0.07670, 0.09507, 0.05642, 0.05778, 0.08086, 0.07181, 0.07348, 0.08834, 0.09713, 0.06519}"
str vec F72 example sim: 0.98454
str vec F73: "{0.06050, 0.07779, 0.06952, 0.06963, 0.06115, 0.09125, 0.07399, 0.05460, 0.07914, 0.06318}"
str vec F73 example sim: 0.98933
str vec F74: "{0.09586, 0.08072, 0.09684, 0.07671, 0.06837, 0.09326, 0.09159, 0.06502, 0.09358, 0.05177}"
str vec F74 example sim: 0.98394
str vec F75: "{0.06008, 0.07920, 0.09198, 0.08659, 0.08840, 0.05918, 0.08516, 0.07060, 0.09754, 0.05664}"
str vec F75 example sim: 0.98380
str vec F76: "{0.05353, 0.06161, 0.06944, 0.09871, 0.09208, 0.08335, 0.06671, 0.09194, 0.09802, 0.08783}"
str vec F76 example sim: 0.98218
str vec F77: "{0.08263, 0.09582, 0.05436, 0.07824, 0.09527, 0.07219, 0.09746, 0.06715, 0.06728, 0.07928}"
str vec F77 example sim: 0.98561
str vec F78: "{0.09221, 0.06640, 0.09146, 0.06176, 0.07090, 0.06310, 0.05215, 0.06923, 0.08445, 0.07841}"
str vec F78 example sim: 0.98540
str vec F79: "{0.06995, 0.05428, 0.09281, 0.08856, 0.06280, 0.06047, 0.07471, 0.05053, 0.07062, 0.05281}"
str vec F79 example sim: 0.97991
str vec F8: "{0.05041, 0.05180, 0.06850, 0.08833, 0.07581, 0.09112, 0.05040, 0.07778, 0.06242, 0.09779}"
str vec F8 example sim: 0.97376
str vec F80: "{0.09013, 0.07353, 0.08838, 0.05519, 0.09406, 0.08487, 0.09114, 0.09918, 0.06405, 0.06865}"
str vec F80 example sim: 0.98566
str vec F81: "{0.06658, 0.08843, 0.06811, 0.08841, 0.06898, 0.05402, 0.05024, 0.09610, 0.08659, 0.07192}"
str vec F81 example sim: 0.98093
str vec F82: "{0.08670, 0.05173, 0.06913, 0.05091, 0.09956, 0.06441, 0.09547, 0.07435, 0.05114, 0.09645}"
str vec F82 example sim: 0.96993
str vec F83: "{0.07675, 0.07476, 0.07820, 0.08449, 0.06197, 0.06814, 0.09293, 0.08329, 0.07795, 0.06258}"
str vec F83 example sim: 0.99265
str vec F84: "{0.07661, 0.08488, 0.06517, 0.07601, 0.08246, 0.05744, 0.08846, 0.08208, 0.05371, 0.08728}"
str vec F84 example sim: 0.98795
str vec F85: "{0.07129, 0.09539, 0.09954, 0.06432, 0.08129, 0.07749, 0.08063, 0.09108, 0.09103, 0.06624}"
str vec F85 example sim: 0.99008
str vec F86: "{0.08965, 0.07734, 0.09282, 0.06110, 0.07258, 0.06316, 0.07858, 0.08112, 0.08708, 0.07135}"
str vec F86 example sim: 0.99157
str vec F87: "{0.05820, 0.05139, 0.08023, 0.06488, 0.06564, 0.09972, 0.05395, 0.05081, 0.08873, 0.07011}"
str vec F87 example sim: 0.97459
str vec F88: "{0.09226, 0.07005, 0.07776, 0.06434, 0.08398, 0.06444, 0.06737, 0.08316, 0.05088, 0.09729}"
str vec F88 example sim: 0.98410
str vec F89: "{0.09886, 0.09334, 0.08646, 0.07542, 0.06651, 0.06733, 0.09204, 0.07347, 0.09135, 0.08617}"
str vec F89 example sim: 0.99144
str vec F9: "{0.09326, 0.09034, 0.08718, 0.08240, 0.08640, 0.07829, 0.07898, 0.09457, 0.09687, 0.08225}"
str vec F9 example sim: 0.99744
str vec F90: "{0.07993, 0.07628, 0.08092, 0.07903, 0.05132, 0.06530, 0.08375, 0.09324, 0.08839, 0.06155}"
str vec F90 example sim: 0.98732
str vec F91: "{0.05002, 0.08489, 0.09539, 0.08168, 0.05178, 0.08611, 0.06172, 0.07283, 0.07593, 0.09799}"
str vec F91 example sim: 0.97855
str vec F92: "{0.08804, 0.07678, 0.07860, 0.09485, 0.07528, 0.05452, 0.06894, 0.06891, 0.06074, 0.05116}"
str vec F92 example sim: 0.98352
str vec F93: "{0.09344, 0.09035, 0.09901, 0.05203, 0.09638, 0.07311, 0.07599, 0.06867, 0.08545, 0.05338}"
str vec F93 example sim: 0.97957
str vec F94: "{0.08379, 0.08039, 0.06861, 0.09006, 0.06889, 0.05867, 0.09825, 0.06612, 0.05591, 0.06868}"
str vec F94 example sim: 0.98489
str vec F95: "{0.08503, 0.05696, 0.08565, 0.07306, 0.08045, 0.09368, 0.09237, 0.05251, 0.06831, 0.05215}"
str vec F95 example sim: 0.97976
str vec F96: "{0.09468, 0.07810, 0.09719, 0.06243, 0.08150, 0.06211, 0.07177, 0.06759, 0.08046, 0.06371}"
str vec F96 example sim: 0.98750
str vec F97: "{0.06253, 0.06721, 0.09144, 0.09020, 0.06158, 0.05703, 0.08484, 0.07940, 0.07786, 0.06872}"
str vec F97 example sim: 0.98770
str vec F98: "{0.08987, 0.08347, 0.05387, 0.07082, 0.06170, 0.08728, 0.06869, 0.08154, 0.07078, 0.09209}"
str vec F98 example sim: 0.98755
str vec F99: "{0.07515, 0.09675, 0.09336, 0.06801, 0.06298, 0.06667, 0.08033, 0.07535, 0.08945, 0.08357}"
str vec F99 example sim: 0.99048
"""

verifyTestOutput( outputdir, result, expected)

