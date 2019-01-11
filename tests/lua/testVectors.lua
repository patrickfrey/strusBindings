require "string"
require "math"
require "utils"
require "dumpVectorStorage"

local outputdir = arg[1] or '.'
local storage = outputdir .. "/storage"
local config = {
	path=storage,
	vecdim=10,
	simdist=20,
	probsimdist=40
}
local vectors = {}
math.randomseed(123)

local examplevec = {0.1,0.1,0.1,0.1,0.1, 0.1,0.1,0.1,0.1,0.1}
for vi=1,100 do
	vv = {}
	for xi=1,10 do
		local r = math.random()
		table.insert( vv, examplevec[ xi] - r / 12)
	end
	table.insert( vectors, vv)
end

local ctx = strus_Context.new()
ctx:loadModule( "storage_vector_std")

if ctx:storageExists( config) then
	ctx:destroyStorage( config)
end

ctx:createVectorStorage( config);
local storage = ctx:createVectorStorageClient( config)
local transaction = storage:createTransaction()
for iv,vv in ipairs( vectors) do
transaction:defineVector( "word", string.format( "F%u", iv), vv)
if math.fmod(iv,2) == 1 then
	transaction:defineFeature( "nonvec", string.format( "F%u", iv))
end
end
transaction:commit()
transaction:close()

local output = dumpVectorStorage( ctx, config, vectors, examplevec)

local storage = ctx:createVectorStorageClient( config)
local searcher = storage:createSearcher( "word", 0, 1)
local simlist = searcher:findSimilar( examplevec, 10, 0.85, true)
output[ 'simlist 0.1x10'] = simlist

local result = "vector storage dump:" .. dumpTree( output) .. "\n"
local expected = [[
vector storage dump:
string config:
  string path: "storage"
  string probsimdist: "40"
  string simdist: "20"
  string vecdim: "10"
string nof vec nonvec: 0
string nof vec word: 100
string rank   1: "0.980 F68 {0.07636, 0.06993, 0.08954, 0.08199, 0.05620, 0.04334, 0.09187, 0.07932, 0.07380, 0.05925}"
string rank   2: "0.980 F56 {0.06114, 0.07913, 0.06968, 0.06678, 0.05327, 0.07512, 0.08499, 0.05479, 0.03874, 0.07901}"
string rank   3: "0.971 F27 {0.02547, 0.09039, 0.07448, 0.09069, 0.07830, 0.08444, 0.08651, 0.09313, 0.09488, 0.09935}"
string rank   4: "0.970 F22 {0.04691, 0.07283, 0.06515, 0.06316, 0.05879, 0.05849, 0.05577, 0.07531, 0.09776, 0.03660}"
string rank   5: "0.969 F96 {0.06311, 0.06332, 0.06552, 0.07182, 0.06648, 0.03503, 0.08762, 0.09228, 0.04861, 0.04993}"
string simlist 0.1x10:
  number 1:
    string value: "F68"
    string weight: 0.98028
  number 2:
    string value: "F56"
    string weight: 0.97954
  number 3:
    string value: "F27"
    string weight: 0.97117
  number 4:
    string value: "F22"
    string weight: 0.96990
  number 5:
    string value: "F96"
    string weight: 0.96892
  number 6:
    string value: "F53"
    string weight: 0.96774
  number 7:
    string value: "F57"
    string weight: 0.96637
  number 8:
    string value: "F61"
    string weight: 0.96307
  number 9:
    string value: "F28"
    string weight: 0.96301
  number 10:
    string value: "F36"
    string weight: 0.96247
string types:
  number 1: "nonvec"
  number 2: "word"
string types F1:
  number 1: "word"
  number 2: "nonvec"
string types F10:
  number 1: "word"
string types F100:
  number 1: "word"
string types F11:
  number 1: "word"
  number 2: "nonvec"
string types F12:
  number 1: "word"
string types F13:
  number 1: "word"
  number 2: "nonvec"
string types F14:
  number 1: "word"
string types F15:
  number 1: "word"
  number 2: "nonvec"
string types F16:
  number 1: "word"
string types F17:
  number 1: "word"
  number 2: "nonvec"
string types F18:
  number 1: "word"
string types F19:
  number 1: "word"
  number 2: "nonvec"
string types F2:
  number 1: "word"
string types F20:
  number 1: "word"
string types F21:
  number 1: "word"
  number 2: "nonvec"
string types F22:
  number 1: "word"
string types F23:
  number 1: "word"
  number 2: "nonvec"
string types F24:
  number 1: "word"
string types F25:
  number 1: "word"
  number 2: "nonvec"
string types F26:
  number 1: "word"
string types F27:
  number 1: "word"
  number 2: "nonvec"
string types F28:
  number 1: "word"
string types F29:
  number 1: "word"
  number 2: "nonvec"
string types F3:
  number 1: "word"
  number 2: "nonvec"
string types F30:
  number 1: "word"
string types F31:
  number 1: "word"
  number 2: "nonvec"
string types F32:
  number 1: "word"
string types F33:
  number 1: "word"
  number 2: "nonvec"
string types F34:
  number 1: "word"
string types F35:
  number 1: "word"
  number 2: "nonvec"
string types F36:
  number 1: "word"
string types F37:
  number 1: "word"
  number 2: "nonvec"
string types F38:
  number 1: "word"
string types F39:
  number 1: "word"
  number 2: "nonvec"
string types F4:
  number 1: "word"
string types F40:
  number 1: "word"
string types F41:
  number 1: "word"
  number 2: "nonvec"
string types F42:
  number 1: "word"
string types F43:
  number 1: "word"
  number 2: "nonvec"
string types F44:
  number 1: "word"
string types F45:
  number 1: "word"
  number 2: "nonvec"
string types F46:
  number 1: "word"
string types F47:
  number 1: "word"
  number 2: "nonvec"
string types F48:
  number 1: "word"
string types F49:
  number 1: "word"
  number 2: "nonvec"
string types F5:
  number 1: "word"
  number 2: "nonvec"
string types F50:
  number 1: "word"
string types F51:
  number 1: "word"
  number 2: "nonvec"
string types F52:
  number 1: "word"
string types F53:
  number 1: "word"
  number 2: "nonvec"
string types F54:
  number 1: "word"
string types F55:
  number 1: "word"
  number 2: "nonvec"
string types F56:
  number 1: "word"
string types F57:
  number 1: "word"
  number 2: "nonvec"
string types F58:
  number 1: "word"
string types F59:
  number 1: "word"
  number 2: "nonvec"
string types F6:
  number 1: "word"
string types F60:
  number 1: "word"
string types F61:
  number 1: "word"
  number 2: "nonvec"
string types F62:
  number 1: "word"
string types F63:
  number 1: "word"
  number 2: "nonvec"
string types F64:
  number 1: "word"
string types F65:
  number 1: "word"
  number 2: "nonvec"
string types F66:
  number 1: "word"
string types F67:
  number 1: "word"
  number 2: "nonvec"
string types F68:
  number 1: "word"
string types F69:
  number 1: "word"
  number 2: "nonvec"
string types F7:
  number 1: "word"
  number 2: "nonvec"
string types F70:
  number 1: "word"
string types F71:
  number 1: "word"
  number 2: "nonvec"
string types F72:
  number 1: "word"
string types F73:
  number 1: "word"
  number 2: "nonvec"
string types F74:
  number 1: "word"
string types F75:
  number 1: "word"
  number 2: "nonvec"
string types F76:
  number 1: "word"
string types F77:
  number 1: "word"
  number 2: "nonvec"
string types F78:
  number 1: "word"
string types F79:
  number 1: "word"
  number 2: "nonvec"
string types F8:
  number 1: "word"
string types F80:
  number 1: "word"
string types F81:
  number 1: "word"
  number 2: "nonvec"
string types F82:
  number 1: "word"
string types F83:
  number 1: "word"
  number 2: "nonvec"
string types F84:
  number 1: "word"
string types F85:
  number 1: "word"
  number 2: "nonvec"
string types F86:
  number 1: "word"
string types F87:
  number 1: "word"
  number 2: "nonvec"
string types F88:
  number 1: "word"
string types F89:
  number 1: "word"
  number 2: "nonvec"
string types F9:
  number 1: "word"
  number 2: "nonvec"
string types F90:
  number 1: "word"
string types F91:
  number 1: "word"
  number 2: "nonvec"
string types F92:
  number 1: "word"
string types F93:
  number 1: "word"
  number 2: "nonvec"
string types F94:
  number 1: "word"
string types F95:
  number 1: "word"
  number 2: "nonvec"
string types F96:
  number 1: "word"
string types F97:
  number 1: "word"
  number 2: "nonvec"
string types F98:
  number 1: "word"
string types F99:
  number 1: "word"
  number 2: "nonvec"
string vec F1: "{0.01848, 0.07446, 0.06479, 0.02918, 0.05758, 0.08939, 0.09065, 0.05523, 0.05946, 0.03290}"
string vec F1 example sim: 0.92700
string vec F10: "{0.06612, 0.07020, 0.02330, 0.04820, 0.03003, 0.03961, 0.04103, 0.06755, 0.07723, 0.07375}"
string vec F10 example sim: 0.94500
string vec F100: "{0.05734, 0.06079, 0.09027, 0.06621, 0.09121, 0.03230, 0.09516, 0.04775, 0.05659, 0.06558}"
string vec F100 example sim: 0.96000
string vec F11: "{0.09463, 0.06893, 0.05536, 0.07903, 0.01761, 0.03072, 0.05883, 0.09177, 0.02303, 0.02340}"
string vec F11 example sim: 0.89000
string vec F12: "{0.07979, 0.03012, 0.08376, 0.03974, 0.02630, 0.08917, 0.04288, 0.05150, 0.07037, 0.08280}"
string vec F12 example sim: 0.93400
string vec F13: "{0.09336, 0.06745, 0.06782, 0.06717, 0.03156, 0.03076, 0.08110, 0.01848, 0.04539, 0.08524}"
string vec F13 example sim: 0.92400
string vec F14: "{0.03023, 0.07920, 0.03917, 0.01830, 0.04483, 0.03513, 0.05957, 0.05393, 0.02366, 0.07099}"
string vec F14 example sim: 0.92200
string vec F15: "{0.08504, 0.05013, 0.02553, 0.06481, 0.06756, 0.09017, 0.03492, 0.01801, 0.01899, 0.06678}"
string vec F15 example sim: 0.90000
string vec F16: "{0.07092, 0.02208, 0.05906, 0.02735, 0.07474, 0.05273, 0.02099, 0.05754, 0.05399, 0.06047}"
string vec F16 example sim: 0.93700
string vec F17: "{0.02398, 0.01767, 0.07583, 0.03641, 0.07333, 0.09416, 0.06999, 0.08468, 0.02945, 0.05519}"
string vec F17 example sim: 0.90700
string vec F18: "{0.03433, 0.08622, 0.09105, 0.01890, 0.07182, 0.04395, 0.07986, 0.09154, 0.02880, 0.06566}"
string vec F18 example sim: 0.92000
string vec F19: "{0.07282, 0.09303, 0.02366, 0.02593, 0.05530, 0.02361, 0.05086, 0.09946, 0.03346, 0.05608}"
string vec F19 example sim: 0.89600
string vec F2: "{0.06984, 0.09801, 0.09349, 0.02771, 0.03928, 0.07611, 0.09900, 0.03121, 0.08453, 0.05465}"
string vec F2 example sim: 0.93200
string vec F20: "{0.09182, 0.08718, 0.09574, 0.05292, 0.07719, 0.05151, 0.06073, 0.03213, 0.04101, 0.03762}"
string vec F20 example sim: 0.94200
string vec F21: "{0.01732, 0.03387, 0.03463, 0.08043, 0.08016, 0.02518, 0.02280, 0.07406, 0.09197, 0.08013}"
string vec F21 example sim: 0.88800
string vec F22: "{0.04691, 0.07283, 0.06515, 0.06316, 0.05879, 0.05849, 0.05577, 0.07531, 0.09776, 0.03660}"
string vec F22 example sim: 0.97000
string vec F23: "{0.04909, 0.08300, 0.02500, 0.06765, 0.09080, 0.07591, 0.04580, 0.03087, 0.05498, 0.09858}"
string vec F23 example sim: 0.93400
string vec F24: "{0.06876, 0.02833, 0.07844, 0.04498, 0.04373, 0.02058, 0.05062, 0.07267, 0.07800, 0.07430}"
string vec F24 example sim: 0.94100
string vec F25: "{0.09632, 0.06962, 0.03169, 0.07203, 0.02381, 0.07463, 0.08432, 0.02965, 0.09541, 0.02556}"
string vec F25 example sim: 0.90700
string vec F26: "{0.06154, 0.09392, 0.06061, 0.02501, 0.05444, 0.04623, 0.01906, 0.03765, 0.07890, 0.09410}"
string vec F26 example sim: 0.91700
string vec F27: "{0.02547, 0.09039, 0.07448, 0.09069, 0.07830, 0.08444, 0.08651, 0.09313, 0.09488, 0.09935}"
string vec F27 example sim: 0.97100
string vec F28: "{0.05825, 0.03628, 0.07777, 0.06411, 0.03638, 0.07071, 0.08682, 0.05425, 0.08038, 0.09056}"
string vec F28 example sim: 0.96300
string vec F29: "{0.02714, 0.01721, 0.02951, 0.09959, 0.03425, 0.04505, 0.04463, 0.08545, 0.02504, 0.04511}"
string vec F29 example sim: 0.87200
string vec F3: "{0.06978, 0.02219, 0.07912, 0.03209, 0.08022, 0.08965, 0.08960, 0.02054, 0.02625, 0.04309}"
string vec F3 example sim: 0.89500
string vec F30: "{0.05594, 0.07031, 0.04566, 0.02893, 0.08053, 0.08773, 0.09818, 0.04130, 0.09981, 0.04986}"
string vec F30 example sim: 0.94100
string vec F31: "{0.05558, 0.05568, 0.05981, 0.02960, 0.06250, 0.07646, 0.08761, 0.07151, 0.03901, 0.03932}"
string vec F31 example sim: 0.95800
string vec F32: "{0.05592, 0.03766, 0.08436, 0.03480, 0.05077, 0.03229, 0.05456, 0.07941, 0.06875, 0.08788}"
string vec F32 example sim: 0.94800
string vec F33: "{0.09396, 0.07484, 0.01840, 0.08887, 0.05093, 0.05376, 0.08926, 0.08746, 0.06097, 0.04553}"
string vec F33 example sim: 0.94300
string vec F34: "{0.09132, 0.04947, 0.02246, 0.09977, 0.07878, 0.02910, 0.09984, 0.02000, 0.05726, 0.06077}"
string vec F34 example sim: 0.90100
string vec F35: "{0.02374, 0.05594, 0.09043, 0.08170, 0.08251, 0.01888, 0.04280, 0.06514, 0.06137, 0.03796}"
string vec F35 example sim: 0.92200
string vec F36: "{0.07133, 0.09936, 0.07619, 0.05693, 0.06768, 0.04577, 0.03389, 0.08539, 0.08078, 0.05177}"
string vec F36 example sim: 0.96200
string vec F37: "{0.08769, 0.06203, 0.02678, 0.04094, 0.09505, 0.05175, 0.09381, 0.02436, 0.02824, 0.02283}"
string vec F37 example sim: 0.88500
string vec F38: "{0.05521, 0.08318, 0.08522, 0.08766, 0.03287, 0.09560, 0.09500, 0.04524, 0.04504, 0.07235}"
string vec F38 example sim: 0.95400
string vec F39: "{0.02067, 0.07300, 0.05205, 0.04864, 0.05953, 0.07617, 0.03843, 0.02409, 0.02164, 0.04264}"
string vec F39 example sim: 0.92300
string vec F4: "{0.03523, 0.03809, 0.03654, 0.06249, 0.06475, 0.02931, 0.08588, 0.09994, 0.07335, 0.06189}"
string vec F4 example sim: 0.93400
string vec F40: "{0.02092, 0.09110, 0.02935, 0.09356, 0.02863, 0.04301, 0.04339, 0.05675, 0.04854, 0.04914}"
string vec F40 example sim: 0.90700
string vec F41: "{0.02705, 0.02356, 0.02240, 0.06815, 0.05958, 0.04592, 0.02346, 0.07474, 0.07716, 0.05164}"
string vec F41 example sim: 0.91400
string vec F42: "{0.04606, 0.02597, 0.06621, 0.08513, 0.01903, 0.09948, 0.08153, 0.05737, 0.06614, 0.06298}"
string vec F42 example sim: 0.93000
string vec F43: "{0.06997, 0.05375, 0.08150, 0.05607, 0.02138, 0.03725, 0.08238, 0.03182, 0.04770, 0.01977}"
string vec F43 example sim: 0.91800
string vec F44: "{0.05374, 0.09965, 0.08630, 0.08139, 0.07400, 0.08921, 0.04569, 0.06194, 0.03683, 0.03612}"
string vec F44 example sim: 0.95100
string vec F45: "{0.05810, 0.07548, 0.08828, 0.05165, 0.04414, 0.06978, 0.05744, 0.06770, 0.07697, 0.02886}"
string vec F45 example sim: 0.96600
string vec F46: "{0.02868, 0.07593, 0.05518, 0.04702, 0.03847, 0.08342, 0.09767, 0.09275, 0.02827, 0.02147}"
string vec F46 example sim: 0.90300
string vec F47: "{0.05758, 0.02117, 0.02119, 0.08621, 0.09492, 0.03787, 0.04271, 0.03444, 0.07428, 0.07271}"
string vec F47 example sim: 0.90600
string vec F48: "{0.07540, 0.09412, 0.03076, 0.09819, 0.03453, 0.03783, 0.02496, 0.02417, 0.07706, 0.02878}"
string vec F48 example sim: 0.88000
string vec F49: "{0.05238, 0.06513, 0.07633, 0.03733, 0.08469, 0.08214, 0.02659, 0.04748, 0.02559, 0.03573}"
string vec F49 example sim: 0.92800
string vec F5: "{0.04580, 0.02716, 0.06562, 0.06721, 0.02781, 0.08239, 0.03446, 0.08874, 0.01978, 0.09436}"
string vec F5 example sim: 0.90300
string vec F50: "{0.05633, 0.04239, 0.02199, 0.09794, 0.05628, 0.02562, 0.04216, 0.07537, 0.04355, 0.08571}"
string vec F50 example sim: 0.91800
string vec F51: "{0.02125, 0.03402, 0.03284, 0.07926, 0.02254, 0.01766, 0.01999, 0.02203, 0.04242, 0.02071}"
string vec F51 example sim: 0.87100
string vec F52: "{0.02655, 0.08643, 0.07595, 0.07699, 0.03439, 0.03856, 0.02305, 0.02038, 0.06313, 0.06726}"
string vec F52 example sim: 0.90600
string vec F53: "{0.08325, 0.03916, 0.04935, 0.07457, 0.08753, 0.07067, 0.09329, 0.08853, 0.05604, 0.05234}"
string vec F53 example sim: 0.96800
string vec F54: "{0.02427, 0.08099, 0.02765, 0.06445, 0.02604, 0.09451, 0.09260, 0.07343, 0.05609, 0.03895}"
string vec F54 example sim: 0.91200
string vec F55: "{0.07675, 0.03117, 0.06317, 0.09354, 0.08382, 0.08388, 0.08125, 0.06612, 0.02629, 0.06530}"
string vec F55 example sim: 0.95300
string vec F56: "{0.06114, 0.07913, 0.06968, 0.06678, 0.05327, 0.07512, 0.08499, 0.05479, 0.03874, 0.07901}"
string vec F56 example sim: 0.98000
string vec F57: "{0.06408, 0.05303, 0.09158, 0.09626, 0.04627, 0.09447, 0.04920, 0.07321, 0.09594, 0.06265}"
string vec F57 example sim: 0.96600
string vec F58: "{0.06193, 0.01847, 0.01673, 0.02615, 0.09683, 0.07600, 0.02265, 0.02405, 0.07618, 0.06781}"
string vec F58 example sim: 0.86300
string vec F59: "{0.09382, 0.07990, 0.02512, 0.06928, 0.03825, 0.06048, 0.06505, 0.05196, 0.08293, 0.05665}"
string vec F59 example sim: 0.95300
string vec F6: "{0.08684, 0.05884, 0.06592, 0.03080, 0.09332, 0.08606, 0.08622, 0.07781, 0.07261, 0.02698}"
string vec F6 example sim: 0.95100
string vec F60: "{0.08269, 0.04694, 0.03045, 0.04325, 0.06500, 0.03946, 0.07686, 0.06480, 0.06861, 0.09388}"
string vec F60 example sim: 0.95300
string vec F61: "{0.07133, 0.09259, 0.06019, 0.06952, 0.09906, 0.05272, 0.07060, 0.03953, 0.03998, 0.07041}"
string vec F61 example sim: 0.96300
string vec F62: "{0.05676, 0.03478, 0.02960, 0.04678, 0.03199, 0.06514, 0.05652, 0.04629, 0.08016, 0.06179}"
string vec F62 example sim: 0.95800
string vec F63: "{0.02711, 0.07149, 0.02171, 0.08483, 0.08573, 0.08458, 0.06849, 0.07478, 0.06753, 0.01813}"
string vec F63 example sim: 0.92000
string vec F64: "{0.09891, 0.06681, 0.08873, 0.05289, 0.09635, 0.07360, 0.07912, 0.03712, 0.05743, 0.04512}"
string vec F64 example sim: 0.96000
string vec F65: "{0.04712, 0.09197, 0.07702, 0.09660, 0.04359, 0.05328, 0.02689, 0.06517, 0.09953, 0.07885}"
string vec F65 example sim: 0.94500
string vec F66: "{0.05266, 0.04873, 0.02574, 0.07481, 0.09736, 0.08703, 0.04945, 0.02145, 0.01970, 0.02666}"
string vec F66 example sim: 0.88400
string vec F67: "{0.05507, 0.02300, 0.08895, 0.04579, 0.07893, 0.04654, 0.08851, 0.03616, 0.02372, 0.02323}"
string vec F67 example sim: 0.89800
string vec F68: "{0.07636, 0.06993, 0.08954, 0.08199, 0.05620, 0.04334, 0.09187, 0.07932, 0.07380, 0.05925}"
string vec F68 example sim: 0.98000
string vec F69: "{0.03851, 0.07268, 0.09790, 0.06611, 0.09019, 0.08928, 0.07147, 0.01672, 0.07929, 0.08844}"
string vec F69 example sim: 0.94700
string vec F7: "{0.03796, 0.07925, 0.05733, 0.07072, 0.06838, 0.03248, 0.05830, 0.03788, 0.09084, 0.05775}"
string vec F7 example sim: 0.95700
string vec F70: "{0.02504, 0.05370, 0.06525, 0.02222, 0.09887, 0.05244, 0.04279, 0.02374, 0.05420, 0.08228}"
string vec F70 example sim: 0.90800
string vec F71: "{0.03621, 0.05504, 0.03733, 0.09788, 0.07978, 0.09656, 0.09073, 0.02348, 0.08413, 0.04714}"
string vec F71 example sim: 0.92500
string vec F72: "{0.08532, 0.06769, 0.09100, 0.04958, 0.05263, 0.06333, 0.07117, 0.04734, 0.02738, 0.09732}"
string vec F72 example sim: 0.95300
string vec F73: "{0.02567, 0.02449, 0.08042, 0.02151, 0.02559, 0.08072, 0.08671, 0.06424, 0.02568, 0.07311}"
string vec F73 example sim: 0.88400
string vec F74: "{0.07260, 0.09103, 0.04874, 0.04228, 0.06628, 0.03482, 0.01684, 0.05946, 0.03204, 0.04796}"
string vec F74 example sim: 0.92800
string vec F75: "{0.05751, 0.03675, 0.02220, 0.08145, 0.04239, 0.05377, 0.01712, 0.04346, 0.04886, 0.03068}"
string vec F75 example sim: 0.92600
string vec F76: "{0.07800, 0.08851, 0.05731, 0.04443, 0.08572, 0.04247, 0.03039, 0.04351, 0.06962, 0.08461}"
string vec F76 example sim: 0.95100
string vec F77: "{0.08974, 0.07237, 0.01845, 0.04879, 0.06129, 0.06549, 0.08154, 0.07554, 0.06966, 0.05339}"
string vec F77 example sim: 0.95800
string vec F78: "{0.01679, 0.07785, 0.06699, 0.08134, 0.07718, 0.06046, 0.09752, 0.02344, 0.08776, 0.08071}"
string vec F78 example sim: 0.93500
string vec F79: "{0.05126, 0.08166, 0.03556, 0.04037, 0.06086, 0.03182, 0.03510, 0.02212, 0.06017, 0.02813}"
string vec F79 example sim: 0.93100
string vec F8: "{0.02646, 0.06131, 0.07692, 0.05155, 0.05017, 0.08580, 0.07090, 0.09805, 0.07325, 0.06184}"
string vec F8 example sim: 0.96000
string vec F80: "{0.02481, 0.05708, 0.07639, 0.03569, 0.06413, 0.02751, 0.04275, 0.07635, 0.05974, 0.02397}"
string vec F80 example sim: 0.92900
string vec F81: "{0.03398, 0.01872, 0.05910, 0.04150, 0.06696, 0.08196, 0.04162, 0.05758, 0.09866, 0.03438}"
string vec F81 example sim: 0.91800
string vec F82: "{0.02171, 0.06241, 0.02685, 0.03213, 0.06232, 0.06747, 0.05843, 0.04024, 0.06746, 0.04973}"
string vec F82 example sim: 0.94700
string vec F83: "{0.04135, 0.05703, 0.05955, 0.02375, 0.04279, 0.04976, 0.09389, 0.08140, 0.08851, 0.09159}"
string vec F83 example sim: 0.93800
string vec F84: "{0.04344, 0.08575, 0.03870, 0.04962, 0.09081, 0.05169, 0.07850, 0.07044, 0.06651, 0.07222}"
string vec F84 example sim: 0.96700
string vec F85: "{0.04410, 0.05592, 0.03856, 0.04450, 0.02925, 0.06632, 0.07538, 0.07942, 0.09690, 0.09579}"
string vec F85 example sim: 0.94000
string vec F86: "{0.02425, 0.07205, 0.06249, 0.04997, 0.05143, 0.08970, 0.02603, 0.02883, 0.06648, 0.04953}"
string vec F86 example sim: 0.93200
string vec F87: "{0.06090, 0.01926, 0.06767, 0.08508, 0.09045, 0.03593, 0.02907, 0.03037, 0.06624, 0.07183}"
string vec F87 example sim: 0.91900
string vec F88: "{0.07128, 0.08807, 0.07264, 0.03123, 0.09607, 0.06672, 0.08783, 0.02284, 0.07605, 0.08057}"
string vec F88 example sim: 0.95000
string vec F89: "{0.06869, 0.04981, 0.03700, 0.07839, 0.06356, 0.09388, 0.02876, 0.09806, 0.04614, 0.08742}"
string vec F89 example sim: 0.94300
string vec F9: "{0.02322, 0.08589, 0.02398, 0.07323, 0.04440, 0.08964, 0.01913, 0.08157, 0.05897, 0.04925}"
string vec F9 example sim: 0.90600
string vec F90: "{0.03268, 0.04688, 0.04395, 0.09299, 0.06751, 0.06612, 0.08970, 0.06149, 0.05596, 0.04280}"
string vec F90 example sim: 0.95400
string vec F91: "{0.01827, 0.06936, 0.03912, 0.03498, 0.05720, 0.09471, 0.09404, 0.09505, 0.04867, 0.04793}"
string vec F91 example sim: 0.91700
string vec F92: "{0.06925, 0.04567, 0.09261, 0.06736, 0.03476, 0.05917, 0.08489, 0.04233, 0.07445, 0.03659}"
string vec F92 example sim: 0.95300
string vec F93: "{0.05463, 0.04740, 0.02053, 0.05366, 0.02953, 0.06818, 0.06647, 0.06969, 0.08472, 0.01922}"
string vec F93 example sim: 0.92500
string vec F94: "{0.03014, 0.03000, 0.03521, 0.02602, 0.03408, 0.08536, 0.03836, 0.03206, 0.06406, 0.02632}"
string vec F94 example sim: 0.91000
string vec F95: "{0.04243, 0.08694, 0.03546, 0.08441, 0.05632, 0.06641, 0.02578, 0.04146, 0.05346, 0.06065}"
string vec F95 example sim: 0.94600
string vec F96: "{0.06311, 0.06332, 0.06552, 0.07182, 0.06648, 0.03503, 0.08762, 0.09228, 0.04861, 0.04993}"
string vec F96 example sim: 0.96900
string vec F97: "{0.03417, 0.03680, 0.06643, 0.03514, 0.08051, 0.07231, 0.06541, 0.03896, 0.04006, 0.01922}"
string vec F97 example sim: 0.93000
string vec F98: "{0.09668, 0.07779, 0.08788, 0.04885, 0.07075, 0.02574, 0.05289, 0.01875, 0.05888, 0.05972}"
string vec F98 example sim: 0.93000
string vec F99: "{0.02027, 0.05026, 0.05695, 0.04485, 0.07372, 0.08236, 0.09335, 0.01980, 0.04530, 0.05151}"
string vec F99 example sim: 0.92000
]]

verifyTestOutput( outputdir, result, expected)

