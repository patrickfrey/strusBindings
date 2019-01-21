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
local seed = 123;
cur_rand = seed;

local examplevec = {0.1,0.1,0.1,0.1,0.1, 0.1,0.1,0.1,0.1,0.1}
for vi=1,100 do
	vv = {}
	for xi=1,10 do
		cur_rand = (cur_rand * 2654435761) % 2000003 + 1001;
		local r = tonumber( cur_rand % 1000) / 1000.0;
		table.insert( vv, examplevec[ xi] - r / 12.0)
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
local simlist = storage:findSimilar( "word", examplevec, 10, 0.85, true)
local simliststr = ""
for si,sv in ipairs( simlist) do
	simliststr = simliststr .. string.format( " %s=%.3f", sv.value, sv.weight)
end
output[ 'simlist 0.1x10'] = simliststr

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
string rank   1: "0.985 F70 {0.05458, 0.07975, 0.09400, 0.09075, 0.08125, 0.09408, 0.06642, 0.07125, 0.06058, 0.07125}"
string rank   2: "0.981 F85 {0.09683, 0.06875, 0.06675, 0.08675, 0.07208, 0.07017, 0.09083, 0.07408, 0.04925, 0.05250}"
string rank   3: "0.970 F52 {0.06525, 0.07225, 0.09442, 0.05633, 0.05425, 0.04000, 0.06200, 0.07992, 0.09942, 0.08050}"
string rank   4: "0.967 F91 {0.04592, 0.02458, 0.04142, 0.05233, 0.03200, 0.02517, 0.05725, 0.03850, 0.03233, 0.03850}"
string rank   5: "0.964 F99 {0.08575, 0.09733, 0.03875, 0.03775, 0.08208, 0.06050, 0.07317, 0.08875, 0.06908, 0.09050}"
string simlist 0.1x10: " F70=0.985 F85=0.981 F52=0.970 F91=0.967 F99=0.964 F97=0.960 F90=0.957 F39=0.957 F34=0.957 F14=0.957"
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
string vec F1: "(0.02808, 0.07333, 0.04258, 0.09158, 0.03267, 0.06975, 0.07850, 0.03942, 0.06367, 0.08683)"
string vec F1 example sim: 0.94000
string vec F10: "(0.08808, 0.03250, 0.03317, 0.02733, 0.02825, 0.06650, 0.04683, 0.08608, 0.08042, 0.05425)"
string vec F10 example sim: 0.92000
string vec F100: "(0.09625, 0.09225, 0.06542, 0.04225, 0.04142, 0.03850, 0.08775, 0.08633, 0.03858, 0.05983)"
string vec F100 example sim: 0.94300
string vec F11: "(0.09475, 0.06392, 0.09000, 0.02558, 0.06375, 0.03958, 0.03042, 0.03042, 0.02375, 0.09483)"
string vec F11 example sim: 0.89400
string vec F12: "(0.03550, 0.02217, 0.08475, 0.09000, 0.03217, 0.05117, 0.02033, 0.06192, 0.03567, 0.05317)"
string vec F12 example sim: 0.90400
string vec F13: "(0.04950, 0.04867, 0.04700, 0.05667, 0.02217, 0.09567, 0.03225, 0.02833, 0.03867, 0.09025)"
string vec F13 example sim: 0.90900
string vec F14: "(0.04425, 0.08025, 0.08333, 0.07133, 0.05925, 0.09008, 0.03875, 0.06967, 0.07125, 0.02850)"
string vec F14 example sim: 0.95700
string vec F15: "(0.07308, 0.06692, 0.07325, 0.09792, 0.09458, 0.02458, 0.02333, 0.03725, 0.02192, 0.03867)"
string vec F15 example sim: 0.89200
string vec F16: "(0.07208, 0.09167, 0.06150, 0.01958, 0.04183, 0.03650, 0.08375, 0.08950, 0.01675, 0.03192)"
string vec F16 example sim: 0.89400
string vec F17: "(0.06183, 0.03975, 0.08392, 0.03967, 0.08058, 0.05133, 0.03800, 0.08950, 0.02942, 0.02925)"
string vec F17 example sim: 0.92700
string vec F18: "(0.06850, 0.03400, 0.03250, 0.04542, 0.03867, 0.06992, 0.07400, 0.04733, 0.03758, 0.02000)"
string vec F18 example sim: 0.93800
string vec F19: "(0.05517, 0.03183, 0.03625, 0.09308, 0.09667, 0.04483, 0.09650, 0.08250, 0.06917, 0.06208)"
string vec F19 example sim: 0.94300
string vec F2: "(0.01767, 0.06358, 0.02492, 0.07917, 0.05200, 0.07775, 0.09817, 0.04717, 0.02733, 0.09508)"
string vec F2 example sim: 0.90300
string vec F20: "(0.08167, 0.04875, 0.09717, 0.02075, 0.06842, 0.09333, 0.01900, 0.03892, 0.05475, 0.06400)"
string vec F20 example sim: 0.91300
string vec F21: "(0.05033, 0.07375, 0.04817, 0.09625, 0.02092, 0.02017, 0.03258, 0.05167, 0.03208, 0.04225)"
string vec F21 example sim: 0.90200
string vec F22: "(0.05850, 0.06950, 0.08600, 0.02392, 0.05325, 0.07367, 0.01767, 0.08350, 0.03592, 0.09550)"
string vec F22 example sim: 0.92000
string vec F23: "(0.03067, 0.09050, 0.05150, 0.03617, 0.04192, 0.06183, 0.08300, 0.05817, 0.02067, 0.05200)"
string vec F23 example sim: 0.92900
string vec F24: "(0.05442, 0.07525, 0.04408, 0.04558, 0.07658, 0.02325, 0.04708, 0.06450, 0.09175, 0.08742)"
string vec F24 example sim: 0.94700
string vec F25: "(0.01692, 0.05683, 0.01950, 0.04292, 0.04442, 0.03950, 0.09658, 0.06800, 0.02275, 0.03650)"
string vec F25 example sim: 0.88700
string vec F26: "(0.03417, 0.07150, 0.08725, 0.03175, 0.04633, 0.05967, 0.04033, 0.04008, 0.09750, 0.04083)"
string vec F26 example sim: 0.92800
string vec F27: "(0.03925, 0.09450, 0.09175, 0.07483, 0.06742, 0.09058, 0.02167, 0.04792, 0.03450, 0.06617)"
string vec F27 example sim: 0.93100
string vec F28: "(0.03833, 0.02433, 0.06567, 0.09958, 0.07592, 0.04808, 0.06383, 0.02033, 0.03983, 0.03708)"
string vec F28 example sim: 0.90900
string vec F29: "(0.04175, 0.08333, 0.02817, 0.02275, 0.03442, 0.06100, 0.02733, 0.02658, 0.02608, 0.07033)"
string vec F29 example sim: 0.89900
string vec F3: "(0.09442, 0.04950, 0.03017, 0.06625, 0.05583, 0.06842, 0.02967, 0.05367, 0.09992, 0.04442)"
string vec F3 example sim: 0.93400
string vec F30: "(0.08975, 0.03667, 0.04267, 0.07817, 0.01842, 0.09667, 0.07633, 0.06958, 0.06592, 0.08783)"
string vec F30 example sim: 0.93900
string vec F31: "(0.07808, 0.03808, 0.01975, 0.05425, 0.03258, 0.03958, 0.05958, 0.03058, 0.04658, 0.05283)"
string vec F31 example sim: 0.94300
string vec F32: "(0.04583, 0.08567, 0.08583, 0.04342, 0.04633, 0.08583, 0.04750, 0.04783, 0.02517, 0.02883)"
string vec F32 example sim: 0.92700
string vec F33: "(0.02267, 0.02133, 0.04633, 0.06833, 0.02742, 0.09700, 0.02600, 0.04483, 0.04767, 0.05167)"
string vec F33 example sim: 0.89600
string vec F34: "(0.05500, 0.01950, 0.05792, 0.07550, 0.04492, 0.05408, 0.08392, 0.05758, 0.08292, 0.06392)"
string vec F34 example sim: 0.95700
string vec F35: "(0.03525, 0.05042, 0.08567, 0.03283, 0.04000, 0.07875, 0.08558, 0.01933, 0.03600, 0.08408)"
string vec F35 example sim: 0.91200
string vec F36: "(0.02508, 0.03758, 0.03050, 0.03508, 0.02458, 0.04825, 0.02008, 0.03775, 0.03408, 0.06183)"
string vec F36 example sim: 0.95000
string vec F37: "(0.02817, 0.09175, 0.05325, 0.04583, 0.03400, 0.04117, 0.08908, 0.06333, 0.07900, 0.08492)"
string vec F37 example sim: 0.93800
string vec F38: "(0.05275, 0.04575, 0.03683, 0.07742, 0.02333, 0.07442, 0.09917, 0.09592, 0.07500, 0.08192)"
string vec F38 example sim: 0.94000
string vec F39: "(0.03283, 0.07300, 0.07617, 0.06117, 0.05358, 0.05000, 0.08742, 0.05933, 0.08225, 0.03158)"
string vec F39 example sim: 0.95700
string vec F4: "(0.04858, 0.09975, 0.02592, 0.09892, 0.08600, 0.09033, 0.06533, 0.09175, 0.06408, 0.02383)"
string vec F4 example sim: 0.93100
string vec F40: "(0.05208, 0.03983, 0.06458, 0.06117, 0.08500, 0.06192, 0.01750, 0.07358, 0.08233, 0.06575)"
string vec F40 example sim: 0.95400
string vec F41: "(0.02442, 0.03067, 0.06683, 0.03333, 0.05717, 0.07283, 0.04192, 0.03975, 0.05458, 0.02767)"
string vec F41 example sim: 0.94100
string vec F42: "(0.06000, 0.01717, 0.03667, 0.03458, 0.05933, 0.07208, 0.06967, 0.08067, 0.02975, 0.07758)"
string vec F42 example sim: 0.93000
string vec F43: "(0.03908, 0.06158, 0.03333, 0.07567, 0.05050, 0.05567, 0.08533, 0.09933, 0.09933, 0.08592)"
string vec F43 example sim: 0.94900
string vec F44: "(0.01783, 0.09533, 0.06042, 0.03675, 0.02442, 0.07500, 0.02375, 0.07225, 0.05658, 0.06908)"
string vec F44 example sim: 0.90600
string vec F45: "(0.05808, 0.07017, 0.04767, 0.09108, 0.05783, 0.08050, 0.04225, 0.03450, 0.07975, 0.09267)"
string vec F45 example sim: 0.95900
string vec F46: "(0.07850, 0.08733, 0.07133, 0.05942, 0.09883, 0.04942, 0.07050, 0.04125, 0.02967, 0.05600)"
string vec F46 example sim: 0.95400
string vec F47: "(0.07683, 0.07517, 0.09792, 0.07683, 0.05825, 0.05317, 0.04358, 0.02383, 0.06992, 0.08400)"
string vec F47 example sim: 0.95500
string vec F48: "(0.07675, 0.02442, 0.04908, 0.08025, 0.08975, 0.02300, 0.08067, 0.03442, 0.08117, 0.09467)"
string vec F48 example sim: 0.92300
string vec F49: "(0.09400, 0.08533, 0.09025, 0.03958, 0.07667, 0.02750, 0.03892, 0.09083, 0.07525, 0.04967)"
string vec F49 example sim: 0.94100
string vec F5: "(0.07283, 0.03250, 0.03850, 0.05008, 0.05550, 0.07142, 0.08200, 0.05558, 0.09200, 0.02700)"
string vec F5 example sim: 0.94200
string vec F50: "(0.04867, 0.04550, 0.08892, 0.08158, 0.07758, 0.04117, 0.06150, 0.02617, 0.05600, 0.04925)"
string vec F50 example sim: 0.95100
string vec F51: "(0.03467, 0.08758, 0.04942, 0.01917, 0.02142, 0.07033, 0.05508, 0.08525, 0.07817, 0.03908)"
string vec F51 example sim: 0.91300
string vec F52: "(0.06525, 0.07225, 0.09442, 0.05633, 0.05425, 0.04000, 0.06200, 0.07992, 0.09942, 0.08050)"
string vec F52 example sim: 0.97000
string vec F53: "(0.08467, 0.05992, 0.09183, 0.01850, 0.07392, 0.02908, 0.03592, 0.07642, 0.05467, 0.03042)"
string vec F53 example sim: 0.91400
string vec F54: "(0.02658, 0.05208, 0.09150, 0.04867, 0.06117, 0.08233, 0.09092, 0.09367, 0.03358, 0.05425)"
string vec F54 example sim: 0.93800
string vec F55: "(0.04408, 0.06725, 0.08892, 0.06917, 0.02675, 0.05358, 0.06175, 0.09142, 0.04375, 0.02650)"
string vec F55 example sim: 0.93600
string vec F56: "(0.03967, 0.04008, 0.09283, 0.03442, 0.07967, 0.07033, 0.01875, 0.06975, 0.09225, 0.07575)"
string vec F56 example sim: 0.92800
string vec F57: "(0.09650, 0.02325, 0.04967, 0.04475, 0.02625, 0.04492, 0.09833, 0.07942, 0.04317, 0.07767)"
string vec F57 example sim: 0.91300
string vec F58: "(0.04400, 0.04608, 0.08242, 0.02917, 0.08667, 0.03883, 0.06417, 0.09050, 0.03575, 0.02475)"
string vec F58 example sim: 0.91800
string vec F59: "(0.09483, 0.04383, 0.06750, 0.06333, 0.03642, 0.02067, 0.09425, 0.04125, 0.05733, 0.05767)"
string vec F59 example sim: 0.93100
string vec F6: "(0.02400, 0.07558, 0.02000, 0.09925, 0.06808, 0.08858, 0.02100, 0.03842, 0.08775, 0.05925)"
string vec F6 example sim: 0.89600
string vec F60: "(0.05717, 0.02367, 0.06958, 0.03058, 0.02992, 0.07175, 0.03783, 0.02867, 0.01858, 0.04408)"
string vec F60 example sim: 0.91700
string vec F61: "(0.03708, 0.04192, 0.09233, 0.03883, 0.02358, 0.08750, 0.06217, 0.09358, 0.03650, 0.02800)"
string vec F61 example sim: 0.90100
string vec F62: "(0.07033, 0.07175, 0.07033, 0.07275, 0.08450, 0.02033, 0.04458, 0.08825, 0.05467, 0.02883)"
string vec F62 example sim: 0.94100
string vec F63: "(0.03400, 0.03317, 0.09133, 0.04233, 0.04542, 0.05025, 0.08883, 0.09267, 0.08483, 0.04775)"
string vec F63 example sim: 0.93200
string vec F64: "(0.01983, 0.08817, 0.02767, 0.07558, 0.07617, 0.02442, 0.04875, 0.02842, 0.06250, 0.03817)"
string vec F64 example sim: 0.90000
string vec F65: "(0.07233, 0.06583, 0.02900, 0.09708, 0.08225, 0.09933, 0.04967, 0.02008, 0.04767, 0.09900)"
string vec F65 example sim: 0.92400
string vec F66: "(0.09133, 0.06083, 0.07967, 0.03200, 0.04808, 0.06125, 0.07017, 0.04133, 0.07383, 0.05775)"
string vec F66 example sim: 0.96400
string vec F67: "(0.05425, 0.02317, 0.09492, 0.02333, 0.05592, 0.06517, 0.03083, 0.03242, 0.03000, 0.04775)"
string vec F67 example sim: 0.90500
string vec F68: "(0.07942, 0.02242, 0.09158, 0.02675, 0.01933, 0.06892, 0.06067, 0.07533, 0.07842, 0.08350)"
string vec F68 example sim: 0.91900
string vec F69: "(0.04583, 0.02358, 0.07925, 0.03458, 0.05325, 0.09175, 0.06258, 0.02933, 0.01692, 0.09475)"
string vec F69 example sim: 0.89400
string vec F7: "(0.03808, 0.02983, 0.07467, 0.09208, 0.06758, 0.03158, 0.08733, 0.05017, 0.03025, 0.06525)"
string vec F7 example sim: 0.92800
string vec F70: "(0.05458, 0.07975, 0.09400, 0.09075, 0.08125, 0.09408, 0.06642, 0.07125, 0.06058, 0.07125)"
string vec F70 example sim: 0.98500
string vec F71: "(0.04583, 0.07400, 0.04608, 0.09967, 0.09875, 0.03158, 0.05508, 0.06292, 0.09900, 0.08600)"
string vec F71 example sim: 0.94600
string vec F72: "(0.03833, 0.05217, 0.02683, 0.09625, 0.09175, 0.05833, 0.06858, 0.06125, 0.04633, 0.04092)"
string vec F72 example sim: 0.93900
string vec F73: "(0.03283, 0.09058, 0.05767, 0.06533, 0.05717, 0.05017, 0.06775, 0.02742, 0.04008, 0.08975)"
string vec F73 example sim: 0.94300
string vec F74: "(0.02033, 0.05592, 0.04658, 0.03542, 0.08067, 0.06608, 0.06325, 0.08900, 0.03042, 0.07108)"
string vec F74 example sim: 0.93500
string vec F75: "(0.04758, 0.07708, 0.04367, 0.04125, 0.09317, 0.03933, 0.03717, 0.08742, 0.05283, 0.02950)"
string vec F75 example sim: 0.93200
string vec F76: "(0.07217, 0.02225, 0.05792, 0.03750, 0.07258, 0.09192, 0.04092, 0.04533, 0.02258, 0.08567)"
string vec F76 example sim: 0.91800
string vec F77: "(0.06125, 0.02908, 0.05800, 0.07500, 0.06608, 0.07992, 0.04708, 0.07633, 0.03042, 0.09317)"
string vec F77 example sim: 0.95100
string vec F78: "(0.04617, 0.08825, 0.09317, 0.06175, 0.03408, 0.06183, 0.08167, 0.07242, 0.05875, 0.03175)"
string vec F78 example sim: 0.95200
string vec F79: "(0.04025, 0.02275, 0.06550, 0.07483, 0.07775, 0.06150, 0.08708, 0.05908, 0.07483, 0.08942)"
string vec F79 example sim: 0.95700
string vec F8: "(0.03892, 0.01867, 0.02175, 0.03267, 0.02458, 0.08658, 0.04425, 0.02692, 0.06192, 0.08217)"
string vec F8 example sim: 0.88100
string vec F80: "(0.04467, 0.06283, 0.09300, 0.03908, 0.08508, 0.06217, 0.01900, 0.04133, 0.02292, 0.08383)"
string vec F80 example sim: 0.91300
string vec F81: "(0.09883, 0.08083, 0.07933, 0.06758, 0.05217, 0.03508, 0.03742, 0.06167, 0.09467, 0.09608)"
string vec F81 example sim: 0.95300
string vec F82: "(0.02842, 0.03175, 0.02267, 0.06825, 0.09333, 0.07375, 0.07067, 0.08417, 0.06800, 0.04792)"
string vec F82 example sim: 0.93000
string vec F83: "(0.02242, 0.03708, 0.05867, 0.09825, 0.04208, 0.06242, 0.08692, 0.02733, 0.03933, 0.04850)"
string vec F83 example sim: 0.91300
string vec F84: "(0.07250, 0.06583, 0.08408, 0.09708, 0.05558, 0.08817, 0.05533, 0.02067, 0.08850, 0.05925)"
string vec F84 example sim: 0.95500
string vec F85: "(0.09683, 0.06875, 0.06675, 0.08675, 0.07208, 0.07017, 0.09083, 0.07408, 0.04925, 0.05250)"
string vec F85 example sim: 0.98100
string vec F86: "(0.09867, 0.07292, 0.01750, 0.03542, 0.04442, 0.04158, 0.06667, 0.04150, 0.09192, 0.02117)"
string vec F86 example sim: 0.89400
string vec F87: "(0.01967, 0.05533, 0.09708, 0.04642, 0.06417, 0.08875, 0.03417, 0.07825, 0.09358, 0.06750)"
string vec F87 example sim: 0.93400
string vec F88: "(0.05058, 0.04808, 0.02442, 0.01775, 0.06242, 0.05275, 0.08275, 0.04850, 0.07292, 0.02817)"
string vec F88 example sim: 0.92700
string vec F89: "(0.08800, 0.07258, 0.03183, 0.04608, 0.02708, 0.08550, 0.08133, 0.04725, 0.06542, 0.08667)"
string vec F89 example sim: 0.94400
string vec F9: "(0.05733, 0.03883, 0.02483, 0.07892, 0.04767, 0.03958, 0.08250, 0.09117, 0.04600, 0.05883)"
string vec F9 example sim: 0.94100
string vec F90: "(0.04567, 0.03475, 0.08358, 0.03642, 0.05033, 0.07475, 0.08908, 0.07583, 0.06733, 0.08092)"
string vec F90 example sim: 0.95700
string vec F91: "(0.04592, 0.02458, 0.04142, 0.05233, 0.03200, 0.02517, 0.05725, 0.03850, 0.03233, 0.03850)"
string vec F91 example sim: 0.96700
string vec F92: "(0.02458, 0.07458, 0.09258, 0.02392, 0.04342, 0.02650, 0.09283, 0.05250, 0.07108, 0.03550)"
string vec F92 example sim: 0.90100
string vec F93: "(0.07783, 0.08333, 0.02633, 0.08342, 0.07925, 0.06658, 0.09025, 0.01708, 0.05892, 0.04467)"
string vec F93 example sim: 0.93300
string vec F94: "(0.10000, 0.08692, 0.09633, 0.08800, 0.05242, 0.07367, 0.09450, 0.05750, 0.02517, 0.06842)"
string vec F94 example sim: 0.95700
string vec F95: "(0.04158, 0.06667, 0.09500, 0.07283, 0.02667, 0.08892, 0.06183, 0.07867, 0.02283, 0.07567)"
string vec F95 example sim: 0.93600
string vec F96: "(0.08775, 0.09408, 0.02925, 0.02383, 0.02242, 0.06258, 0.06942, 0.09617, 0.09467, 0.08892)"
string vec F96 example sim: 0.91600
string vec F97: "(0.06258, 0.06300, 0.03442, 0.04125, 0.05825, 0.02733, 0.03475, 0.06267, 0.03825, 0.03533)"
string vec F97 example sim: 0.96000
string vec F98: "(0.05542, 0.08458, 0.03258, 0.08700, 0.03342, 0.08058, 0.06150, 0.05342, 0.04158, 0.04500)"
string vec F98 example sim: 0.94700
string vec F99: "(0.08575, 0.09733, 0.03875, 0.03775, 0.08208, 0.06050, 0.07317, 0.08875, 0.06908, 0.09050)"
string vec F99 example sim: 0.96400
]]

verifyTestOutput( outputdir, result, expected)

