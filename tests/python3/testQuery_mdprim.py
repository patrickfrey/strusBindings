import sys
import gc
import strus
import pprint
from utils import *
from config_mdprim import *
from createCollection import createCollection
from dumpCollection import dumpCollection

testname = "Query_mdprim"
if len(sys.argv) > 1:
	datadir = sys.argv[1]
else:
	datadir = "../data/mdprim/"
if len(sys.argv) > 2:
	outputdir = sys.argv[2]
else:
	outputdir = "."
if len(sys.argv) > 3:
	ctxconfig = getContextConfig( sys.argv[3], testname)
else:
	ctxconfig = None

storagedir = outputdir + "/storage"
docfiles = ["doc1000.xml"]
withrpc = False
if ctxconfig and 'rpc' in ctxconfig:
	withrpc = True

ctx = strus.Context( ctxconfig)
storageConfig = None
if not withrpc:
	storageConfig = "path='%s';metadata='%s';cache=512M" % (storagedir, metadata_mdprim())
output = {}

aclmap = {}
for i in range(1,1001):
	usr = []
	if i > 500:
		usr.append( 'large')
	else:
		usr.append( 'small')
	if i < 50:
		usr.append( 'tiny')
	if i > 950:
		usr.append( 'huge')
	aclmap[ str(i)] = usr

createCollection( ctx, storagedir, metadata_mdprim(), createDocumentAnalyzer_mdprim( ctx), True, datadir, docfiles, aclmap, withrpc)

queryPhrase = "2 3"

# Get a client for the new created storage:
storage = ctx.createStorageClient( storageConfig)

# Define the query analyzer to use:
analyzer = createQueryAnalyzer_mdprim( ctx)

# Define the query evaluation scheme:
queryEval = createQueryEval_mdprim( ctx)

# Now we build the query to issue:
query = queryEval.createQuery( storage)

# First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
terms = analyzer.analyzeTermExpression( ["word",queryPhrase])
# texpr = analyzer.analyzeTermExpression( ["union", ["sequence", 10, ["sequence", 10, ["word","2"], ["word","3"]], ["word","5"]], ["sequence", 10, ["sequence", 10, ["word","2"], ["word","5"]], ["word","7"]] ] )
mexpr = analyzer.analyzeMetaDataExpression( [ [["<","cross","010 "],[">","cross"," 14"]], [">","factors"," 0 "] ] )
texpr_plain = [
			["sequence_imm", ["word","2"], ["word","2"], ["word","2"], ["word","2"]],
			["sequence_imm", ["sequence_imm", ["word","2"], ["word","3"]], ["union", ["sequence_imm", ["word","3"], ["word","5"]], ["word","5"], ["word","7"], ["word","11"]]]
		]
texpr = analyzer.analyzeTermExpression( texpr_plain)

output[ "QueryString"] = queryPhrase
output[ "QueryTerms"] = terms
output[ "QueryExpr"] = texpr
output[ "QueryRestr"] = mexpr

if not terms:
	error( "query is empty")

# Then we iterate on the terms and create a single term feature for each term and collect
# all terms to create a selection expression out of them:
selexpr = ["contains", 0, 2]

for term in terms:
	# Each query term is also part of the selection expressions
	selexpr.append( term)

query.addFeature( "seek", texpr)

# We assign the feature created to the set named 'select' because this is the
# name of the set defined as selection feature in the query evaluation configuration
# (QueryEval.addSelectionFeature):
query.addFeature( "select", selexpr)

# Define the maximum number of best result (ranks) to return:
query.setMaxNofRanks( 20)
# Define the index of the first rank (for implementing scrolling: 0 for the first, 
# 20 for the 2nd, 40 for the 3rd page, etc.):
query.setMinRank( 0)

# Query restriction
query.addMetaDataRestriction( [ [["<","cross",10],[">","cross",14]], [">","factors",0] ] )
query.addMetaDataRestriction( mexpr )

# Enable debugging
query.setDebugMode( False )

# Restrict document access
query.addAccess( ["tiny","huge","small"] )

# Dump query to output
output[ "QueryDump"] = query.introspection( None)

# Now we evaluate the query and iterate on the result to display them:
results = query.evaluate()
output[ "QueryResult"] = results
output_list = []
for pos,result in enumerate(results.ranks):
	output_list.append( "rank %u: %.5f" % (pos+1, result.weight))
	for summary in result.summary:
		output_list.append( "    %s: '%s'" % (summary.name, summary.value))
output[ "ResultList"] = output_list

result = "query evaluation:" + dumpTreeWithFilter( output, {'docno'}) + "\n"
expected = """query evaluation:
str QueryDump: "query evaluation program:
SELECT select;
EVAL  tf( match= %seek);
EVAL  metadata( {name:"doclen",weight:1});
EVAL  metadata( {name:"docidx",weight:1});
SUMMARIZE attribute( {attribute:"docid",resultname:"docid"});
SUMMARIZE metadata( {metaname:"cross",resultname:"cross"});
SUMMARIZE metadata( {metaname:"factors",resultname:"factors"});
SUMMARIZE metadata( {metaname:"lo",resultname:"lo"});
SUMMARIZE metadata( {metaname:"hi",resultname:"hi"});
SUMMARIZE forwardindex( {N:100,resultname:"word",type:"word"});
feature 'seek' 1.00000: 
  sequence_imm range=0 cardinality=0:
    sequence_imm range=0 cardinality=0:
      term word '2'
      term word '3'
    union range=0 cardinality=0:
      sequence_imm range=0 cardinality=0:
        term word '3'
        term word '5'
      term word '5'
      term word '7'
      term word '11'

feature 'seek' 1.00000: 
  sequence_imm range=0 cardinality=0:
    term word '2'
    term word '2'
    term word '2'
    term word '2'

feature 'select' 1.00000: 
  contains range=0 cardinality=2:
    term word '2'
    term word '3'

maxNofRanks = 20
minRank = 0
user access one of : tiny, huge, small
"
str QueryExpr:
  number 1:
    str arg:
      number 1:
        str type: "word"
        str value: "2"
      number 2:
        str type: "word"
        str value: "2"
      number 3:
        str type: "word"
        str value: "2"
      number 4:
        str type: "word"
        str value: "2"
    str op: "sequence_imm"
  number 2:
    str arg:
      number 1:
        str arg:
          number 1:
            str type: "word"
            str value: "2"
          number 2:
            str type: "word"
            str value: "3"
        str op: "sequence_imm"
      number 2:
        str arg:
          number 1:
            str arg:
              number 1:
                str type: "word"
                str value: "3"
              number 2:
                str type: "word"
                str value: "5"
            str op: "sequence_imm"
          number 2:
            str type: "word"
            str value: "5"
          number 3:
            str type: "word"
            str value: "7"
          number 4:
            str type: "word"
            str value: "11"
        str op: "union"
    str op: "sequence_imm"
str QueryRestr:
  number 1:
    number 1:
      str name: "cross"
      str op: "<"
      str value: 10
    number 2:
      str name: "cross"
      str op: ">"
      str value: 14
  number 2:
    str name: "factors"
    str op: ">"
    str value: 0
str QueryResult:
  str evalpass: 0
  str nofranked: 70
  str nofvisited: 91
  str ranks:
    number 1:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "384"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "15"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "7"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "3"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "2"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "2"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "2"
          str weight: 1
        number 11:
          str index: 6
          str name: "word"
          str value: "2"
          str weight: 1
        number 12:
          str index: 7
          str name: "word"
          str value: "2"
          str weight: 1
        number 13:
          str index: 8
          str name: "word"
          str value: "3"
          str weight: 1
      str weight: 0.50062
    number 2:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "960"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "15"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "7"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "2"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "2"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "2"
          str weight: 1
        number 11:
          str index: 6
          str name: "word"
          str value: "2"
          str weight: 1
        number 12:
          str index: 7
          str name: "word"
          str value: "3"
          str weight: 1
        number 13:
          str index: 8
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.50004
    number 3:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "30"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "3"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "2"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "3"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.33430
    number 4:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "42"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "6"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "2"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "7"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "3"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "7"
          str weight: 1
      str weight: 0.33429
    number 5:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "96"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "15"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "5"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "3"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "2"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "2"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "2"
          str weight: 1
        number 11:
          str index: 6
          str name: "word"
          str value: "3"
          str weight: 1
      str weight: 0.33424
    number 6:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "240"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "6"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "5"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "2"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "2"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "3"
          str weight: 1
        number 11:
          str index: 6
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.33409
    number 7:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "288"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "18"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "6"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "3"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "2"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "2"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "2"
          str weight: 1
        number 11:
          str index: 6
          str name: "word"
          str value: "3"
          str weight: 1
        number 12:
          str index: 7
          str name: "word"
          str value: "3"
          str weight: 1
      str weight: 0.28643
    number 8:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "60"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "6"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "3"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "3"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.25094
    number 9:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "90"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "9"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "3"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "3"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "3"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.25091
    number 10:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "132"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "6"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "3"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "11"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "3"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "11"
          str weight: 1
      str weight: 0.25087
    number 11:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "150"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "6"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "3"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "3"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "5"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.25085
    number 12:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "210"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "3"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "3"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "7"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "3"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "5"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "7"
          str weight: 1
      str weight: 0.25079
    number 13:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "294"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "15"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "3"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "7"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "3"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "7"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "7"
          str weight: 1
      str weight: 0.25071
    number 14:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "330"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "6"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "3"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "11"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "3"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "5"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "11"
          str weight: 1
      str weight: 0.25067
    number 15:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "966"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "21"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "3"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "23"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "3"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "7"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "23"
          str weight: 1
      str weight: 0.25003
    number 16:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "120"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "3"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "4"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "2"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "3"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.20088
    number 17:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "168"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "15"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "4"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "7"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "2"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "3"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "7"
          str weight: 1
      str weight: 0.20083
    number 18:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "180"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "9"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "4"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "3"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "3"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.20082
    number 19:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "300"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "3"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "4"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "5"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "3"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "5"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "5"
          str weight: 1
      str weight: 0.20070
    number 20:
      str summary:
        number 1:
          str index: -1
          str name: "docid"
          str value: "420"
          str weight: 1
        number 2:
          str index: -1
          str name: "cross"
          str value: "6"
          str weight: 1
        number 3:
          str index: -1
          str name: "factors"
          str value: "4"
          str weight: 1
        number 4:
          str index: -1
          str name: "lo"
          str value: "2"
          str weight: 1
        number 5:
          str index: -1
          str name: "hi"
          str value: "7"
          str weight: 1
        number 6:
          str index: 1
          str name: "word"
          str value: "2"
          str weight: 1
        number 7:
          str index: 2
          str name: "word"
          str value: "2"
          str weight: 1
        number 8:
          str index: 3
          str name: "word"
          str value: "3"
          str weight: 1
        number 9:
          str index: 4
          str name: "word"
          str value: "5"
          str weight: 1
        number 10:
          str index: 5
          str name: "word"
          str value: "7"
          str weight: 1
      str weight: 0.20058
str QueryString: "2 3"
str QueryTerms:
  number 1:
    str type: "word"
    str value: "2"
  number 2:
    str type: "word"
    str value: "3"
str ResultList:
  number 1: "rank 1: 0.50062"
  number 2: "    docid: '384'"
  number 3: "    cross: '15'"
  number 4: "    factors: '7'"
  number 5: "    lo: '2'"
  number 6: "    hi: '3'"
  number 7: "    word: '2'"
  number 8: "    word: '2'"
  number 9: "    word: '2'"
  number 10: "    word: '2'"
  number 11: "    word: '2'"
  number 12: "    word: '2'"
  number 13: "    word: '2'"
  number 14: "    word: '3'"
  number 15: "rank 2: 0.50004"
  number 16: "    docid: '960'"
  number 17: "    cross: '15'"
  number 18: "    factors: '7'"
  number 19: "    lo: '2'"
  number 20: "    hi: '5'"
  number 21: "    word: '2'"
  number 22: "    word: '2'"
  number 23: "    word: '2'"
  number 24: "    word: '2'"
  number 25: "    word: '2'"
  number 26: "    word: '2'"
  number 27: "    word: '3'"
  number 28: "    word: '5'"
  number 29: "rank 3: 0.33430"
  number 30: "    docid: '30'"
  number 31: "    cross: '3'"
  number 32: "    factors: '2'"
  number 33: "    lo: '2'"
  number 34: "    hi: '5'"
  number 35: "    word: '2'"
  number 36: "    word: '3'"
  number 37: "    word: '5'"
  number 38: "rank 4: 0.33429"
  number 39: "    docid: '42'"
  number 40: "    cross: '6'"
  number 41: "    factors: '2'"
  number 42: "    lo: '2'"
  number 43: "    hi: '7'"
  number 44: "    word: '2'"
  number 45: "    word: '3'"
  number 46: "    word: '7'"
  number 47: "rank 5: 0.33424"
  number 48: "    docid: '96'"
  number 49: "    cross: '15'"
  number 50: "    factors: '5'"
  number 51: "    lo: '2'"
  number 52: "    hi: '3'"
  number 53: "    word: '2'"
  number 54: "    word: '2'"
  number 55: "    word: '2'"
  number 56: "    word: '2'"
  number 57: "    word: '2'"
  number 58: "    word: '3'"
  number 59: "rank 6: 0.33409"
  number 60: "    docid: '240'"
  number 61: "    cross: '6'"
  number 62: "    factors: '5'"
  number 63: "    lo: '2'"
  number 64: "    hi: '5'"
  number 65: "    word: '2'"
  number 66: "    word: '2'"
  number 67: "    word: '2'"
  number 68: "    word: '2'"
  number 69: "    word: '3'"
  number 70: "    word: '5'"
  number 71: "rank 7: 0.28643"
  number 72: "    docid: '288'"
  number 73: "    cross: '18'"
  number 74: "    factors: '6'"
  number 75: "    lo: '2'"
  number 76: "    hi: '3'"
  number 77: "    word: '2'"
  number 78: "    word: '2'"
  number 79: "    word: '2'"
  number 80: "    word: '2'"
  number 81: "    word: '2'"
  number 82: "    word: '3'"
  number 83: "    word: '3'"
  number 84: "rank 8: 0.25094"
  number 85: "    docid: '60'"
  number 86: "    cross: '6'"
  number 87: "    factors: '3'"
  number 88: "    lo: '2'"
  number 89: "    hi: '5'"
  number 90: "    word: '2'"
  number 91: "    word: '2'"
  number 92: "    word: '3'"
  number 93: "    word: '5'"
  number 94: "rank 9: 0.25091"
  number 95: "    docid: '90'"
  number 96: "    cross: '9'"
  number 97: "    factors: '3'"
  number 98: "    lo: '2'"
  number 99: "    hi: '5'"
  number 100: "    word: '2'"
  number 101: "    word: '3'"
  number 102: "    word: '3'"
  number 103: "    word: '5'"
  number 104: "rank 10: 0.25087"
  number 105: "    docid: '132'"
  number 106: "    cross: '6'"
  number 107: "    factors: '3'"
  number 108: "    lo: '2'"
  number 109: "    hi: '11'"
  number 110: "    word: '2'"
  number 111: "    word: '2'"
  number 112: "    word: '3'"
  number 113: "    word: '11'"
  number 114: "rank 11: 0.25085"
  number 115: "    docid: '150'"
  number 116: "    cross: '6'"
  number 117: "    factors: '3'"
  number 118: "    lo: '2'"
  number 119: "    hi: '5'"
  number 120: "    word: '2'"
  number 121: "    word: '3'"
  number 122: "    word: '5'"
  number 123: "    word: '5'"
  number 124: "rank 12: 0.25079"
  number 125: "    docid: '210'"
  number 126: "    cross: '3'"
  number 127: "    factors: '3'"
  number 128: "    lo: '2'"
  number 129: "    hi: '7'"
  number 130: "    word: '2'"
  number 131: "    word: '3'"
  number 132: "    word: '5'"
  number 133: "    word: '7'"
  number 134: "rank 13: 0.25071"
  number 135: "    docid: '294'"
  number 136: "    cross: '15'"
  number 137: "    factors: '3'"
  number 138: "    lo: '2'"
  number 139: "    hi: '7'"
  number 140: "    word: '2'"
  number 141: "    word: '3'"
  number 142: "    word: '7'"
  number 143: "    word: '7'"
  number 144: "rank 14: 0.25067"
  number 145: "    docid: '330'"
  number 146: "    cross: '6'"
  number 147: "    factors: '3'"
  number 148: "    lo: '2'"
  number 149: "    hi: '11'"
  number 150: "    word: '2'"
  number 151: "    word: '3'"
  number 152: "    word: '5'"
  number 153: "    word: '11'"
  number 154: "rank 15: 0.25003"
  number 155: "    docid: '966'"
  number 156: "    cross: '21'"
  number 157: "    factors: '3'"
  number 158: "    lo: '2'"
  number 159: "    hi: '23'"
  number 160: "    word: '2'"
  number 161: "    word: '3'"
  number 162: "    word: '7'"
  number 163: "    word: '23'"
  number 164: "rank 16: 0.20088"
  number 165: "    docid: '120'"
  number 166: "    cross: '3'"
  number 167: "    factors: '4'"
  number 168: "    lo: '2'"
  number 169: "    hi: '5'"
  number 170: "    word: '2'"
  number 171: "    word: '2'"
  number 172: "    word: '2'"
  number 173: "    word: '3'"
  number 174: "    word: '5'"
  number 175: "rank 17: 0.20083"
  number 176: "    docid: '168'"
  number 177: "    cross: '15'"
  number 178: "    factors: '4'"
  number 179: "    lo: '2'"
  number 180: "    hi: '7'"
  number 181: "    word: '2'"
  number 182: "    word: '2'"
  number 183: "    word: '2'"
  number 184: "    word: '3'"
  number 185: "    word: '7'"
  number 186: "rank 18: 0.20082"
  number 187: "    docid: '180'"
  number 188: "    cross: '9'"
  number 189: "    factors: '4'"
  number 190: "    lo: '2'"
  number 191: "    hi: '5'"
  number 192: "    word: '2'"
  number 193: "    word: '2'"
  number 194: "    word: '3'"
  number 195: "    word: '3'"
  number 196: "    word: '5'"
  number 197: "rank 19: 0.20070"
  number 198: "    docid: '300'"
  number 199: "    cross: '3'"
  number 200: "    factors: '4'"
  number 201: "    lo: '2'"
  number 202: "    hi: '5'"
  number 203: "    word: '2'"
  number 204: "    word: '2'"
  number 205: "    word: '3'"
  number 206: "    word: '5'"
  number 207: "    word: '5'"
  number 208: "rank 20: 0.20058"
  number 209: "    docid: '420'"
  number 210: "    cross: '6'"
  number 211: "    factors: '4'"
  number 212: "    lo: '2'"
  number 213: "    hi: '7'"
  number 214: "    word: '2'"
  number 215: "    word: '2'"
  number 216: "    word: '3'"
  number 217: "    word: '5'"
  number 218: "    word: '7'"
"""

verifyTestOutput( outputdir, result, expected)

n = gc.collect( 0)
print('Remaining Garbage:', end=' ')
pprint.pprint(gc.garbage)
print('Unreachable objects:', n)

