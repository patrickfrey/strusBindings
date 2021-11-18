import sys
import strus
from utils import *
from config_t3s import *
from createCollection import createCollection
from dumpCollection import dumpCollection

testname = "Query_t3s"
if len(sys.argv) > 1:
        datadir = sys.argv[1]
else:
        datadir = "../data/t3s/"
if len(sys.argv) > 2:
        outputdir = sys.argv[2]
else:
        outputdir = "."
if len(sys.argv) > 3:
        ctxconfig = getContextConfig( sys.argv[3], testname)
else:
        ctxconfig = None

storagedir = outputdir + "/storage"
docfiles = ["A.xml","B.xml","C.xml"]
withrpc = False
if ctxconfig and 'rpc' in ctxconfig:
        withrpc = True

ctx = strus.Context( ctxconfig)
storageConfig = None
if not withrpc:
        storageConfig = "path='%s';cache=512M" % storagedir
output = {}

createCollection( ctx, storagedir, metadata_t3s(), createDocumentAnalyzer_t3s( ctx), False, datadir, docfiles, None, withrpc)

queryPhrase = "City visit tokyo"

# Get a client for the new created storage:
storage =  ctx.createStorageClient( storageConfig)

# Define the query analyzer to use:
analyzer = createQueryAnalyzer_t3s( ctx)

# Define the query evaluation scheme:
queryEval = createQueryEval_t3s( ctx)

# Now we build the query to issue:
query = queryEval.createQuery( storage)

# First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
queryAnalyzed = analyzer.analyzeTermExpression( ["word",queryPhrase])
exprAnalyzed = analyzer.analyzeTermExpression( [
                                        "sequence", 10,
                                                ["sequence", 2, ["word","completely"], ["word","different"]],
                                                ["sequence", 3, ["word","you"], ["word","expect"]]
                                ])

output[ "QueryString"] = queryPhrase
output[ "QueryAnalyzed"] = queryAnalyzed
output[ "QueryExpression"] = exprAnalyzed

if not withrpc:
        # ... For RPC there are no posting iterators implemented
        postings = {}
        for po in storage.postings( queryAnalyzed):
                docno = po[0]
                pos = po[1]
                postings[ "query analyzed doc %s" % (storage.docid(docno))] = pos
        for docno,pos in storage.postings( exprAnalyzed):
                postings[ "expr analyzed doc %s" % (storage.docid(docno))] = pos
        output[ "postings"] = postings;


# Then we iterate on the terms and create a single term feature for each term and collect
# all terms to create a selection expression out of them:
selexpr = ["contains", 0, 1, queryAnalyzed]

# Create a document feature 'seek'.
query.addFeature( "seek", queryAnalyzed, 1.0)

# We assign the feature created to the set named 'select' because this is the
# name of the set defined as selection feature in the query evaluation configuration
# (QueryEval.addSelectionFeature):
query.addFeature( "select", selexpr)

# Define the maximum number of best result (ranks) to return:
query.setMaxNofRanks( 20)
# Define the index of the first rank (for implementing scrolling: 0 for the first,
# 20 for the 2nd, 40 for the 3rd page, etc.):
query.setMinRank( 0)

# Dump query to output
output[ "QueryDump"] = query.introspection()

# Now we evaluate the query and iterate on the result to display them:
results = query.evaluate()
output[ "QueryResult"] = results
output_list = []
for pos,result in enumerate( results.ranks):
        output_list.append( "rank %u: %.5f" % (pos+1, result.weight))
        for summary in result.summary:
                output_list.append( "    %s: '%s'" % (summary.name, summary.value))
output[ "ResultList"] = output_list

result = "query evaluation:" + dumpTreeWithFilter( output, {'docno'}) + "\n"
expected = """query evaluation:
str QueryAnalyzed:
  str arg:
    number 1:
      str type: "word"
      str value: "citi"
    number 2:
      str type: "word"
      str value: "visit"
    number 3:
      str type: "word"
      str value: "tokyo"
  str op: "union"
str QueryDump:
  str eval:
    str selection_sets:
      number 1: "select"
    str summarizers:
      number 1:
        str function: "attribute"
        str param:
          str attribute: "title"
      number 2:
        str function: "attribute"
        str param:
          str attribute: "docid"
      number 3:
        str feature:
          str match: "seek"
        str function: "matchphrase"
        str param:
          str cluster: 0.10000
          str dist_close: 8
          str dist_imm: 2
          str dist_near: 40
          str dist_sentence: 20
          str entity: ""
          str ffbase: 0.10000
          str maxdf: 1
          str sentences: 2
          str text: "orig"
    str weighting:
      number 1:
        str feature:
          str match: "seek"
        str function: "bm25"
        str param:
          str avgdoclen: 1000
          str b: 2.10000
          str k1: 0.75000
          str metadata_doclen: "doclen"
    str weighting_sets:
      number 1: "seek"
  str feature:
    number 1:
      str set: "seek"
      str struct:
        str arg:
          number 1:
            str node: "term"
            str type: "word"
            str value: "citi"
          number 2:
            str node: "term"
            str type: "word"
            str value: "visit"
          number 3:
            str node: "term"
            str type: "word"
            str value: "tokyo"
        str node: "expression"
        str op: "union"
    number 2:
      str set: "select"
      str struct:
        str arg:
          number 1:
            str arg:
              number 1:
                str node: "term"
                str type: "word"
                str value: "citi"
              number 2:
                str node: "term"
                str type: "word"
                str value: "visit"
              number 3:
                str node: "term"
                str type: "word"
                str value: "tokyo"
            str node: "expression"
            str op: "union"
        str cardinality: 1
        str node: "expression"
        str op: "contains"
  str merge: 0
  str minrank: 0
  str nofranks: 20
str QueryExpression:
  str arg:
    number 1:
      str arg:
        number 1:
          str type: "word"
          str value: "complet"
        number 2:
          str type: "word"
          str value: "differ"
      str op: "sequence"
      str range: 2
    number 2:
      str arg:
        number 1:
          str type: "word"
          str value: "you"
        number 2:
          str type: "word"
          str value: "expect"
      str op: "sequence"
      str range: 3
  str op: "sequence"
  str range: 10
str QueryResult:
  str evalpass: 0
  str nofranked: 3
  str nofvisited: 3
  str ranks:
    number 1:
      str field:
        str end: 0
        str start: 0
      str summary:
        number 1:
          str name: ""
          str value: "A journey through Germany"
        number 2:
          str name: ""
          str value: "C.xml"
        number 3:
          str name: "summary"
          str value: "A journey through Germany When I first visited germany it was still splitted into two parts."
      str weight: 0.00009
    number 2:
      str field:
        str end: 0
        str start: 0
      str summary:
        number 1:
          str name: ""
          str value: "A visit in New York"
        number 2:
          str name: ""
          str value: "B.xml"
        number 3:
          str name: "summary"
          str value: "A visit in New York New York is a city with dimensions you can't imagine."
      str weight: 0.00003
    number 3:
      str field:
        str end: 0
        str start: 0
      str summary:
        number 1:
          str name: ""
          str value: "One day in Tokyo"
        number 2:
          str name: ""
          str value: "A.xml"
        number 3:
          str name: "summary"
          str value: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen."
      str weight: 0.00002
str QueryString: "City visit tokyo"
str ResultList:
  number 1: "rank 1: 0.00009"
  number 2: "    : 'A journey through Germany'"
  number 3: "    : 'C.xml'"
  number 4: "    summary: 'A journey through Germany When I first visited germany it was still splitted into two parts.'"
  number 5: "rank 2: 0.00003"
  number 6: "    : 'A visit in New York'"
  number 7: "    : 'B.xml'"
  number 8: "    summary: 'A visit in New York New York is a city with dimensions you can't imagine.'"
  number 9: "rank 3: 0.00002"
  number 10: "    : 'One day in Tokyo'"
  number 11: "    : 'A.xml'"
  number 12: "    summary: 'One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen.'"
str postings:
  str expr analyzed doc A.xml:
    number 1: 12
  str query analyzed doc A.xml:
    number 1: 4
    number 2: 6
    number 3: 9
  str query analyzed doc B.xml:
    number 1: 2
    number 2: 11
  str query analyzed doc C.xml:
    number 1: 9
"""
verifyTestOutput( outputdir, result, expected)

