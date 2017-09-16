import sys
import strus
from utils import *
from config_t3s import *
from createCollection import createCollection
from dumpCollection import dumpCollection

if len(sys.argv) > 1:
	datadir = sys.argv[1]
else:
	datadir = "../data/t3s/"
if len(sys.argv) > 2:
	outputdir = sys.argv[2]
else:
	outputdir = "."
if len(sys.argv) > 3:
	ctxconfig = getContextConfig( sys.argv[3])
else:
	ctxconfig = None

storagedir = outputdir + "/storage"
docfiles = ["A.xml","B.xml","C.xml"]
withrpc = False
if ctxconfig and ctxconfig['rpc']:
	withrpc = True

ctx = strus.Context( ctxconfig)
storageConfig = None
if not withrpc:
	storageConfig = "path='%s';metadata='%s';cache=512M" % (storagedir, metadata_t3s())
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
terms = analyzer.analyzeTermExpression( ["word",queryPhrase])
qexpr = analyzer.analyzeTermExpression( [
					"sequence", 10,
						["sequence", 2, ["word","completely"], ["word","different"]],
						["sequence", 3, ["word","you"], ["word","expect"]]
				])

output[ "QueryString"] = queryPhrase
output[ "QueryTerms"] = terms
output[ "QueryExpression"] = qexpr

if not terms:
	raise Exception( "query is empty")

if not withrpc:
	# ... For RPC there are no posting iterators implemented
	postings = {}
	for _,qe in enumerate(terms):
		for po in storage.postings( qe):
			docno = po[0]
			pos = po[1]
			postings[ "term %u doc %s" % (_, storage.docid(docno))] = pos
	for _,qe in enumerate(qexpr):
		for docno,pos in storage.postings( qe):
			postings[ "expr %u doc %s" % (_, storage.docid(docno))] = pos
	output[ "postings"] = postings;


# Then we iterate on the terms and create a single term feature for each term and collect
# all terms to create a selection expression out of them:
selexpr = ["contains", 0, 1]
for term in terms:
	# Each query term is also part of the selection expressions
	selexpr.append( term)
	# Create a document feature 'seek'.
	query.addFeature( "seek", term, 1.0)

# We assign the feature created to the set named 'select' because this is the
# name of the set defined as selection feature in the query evaluation configuration
# (QueryEval.addSelectionFeature):
query.addFeature( "select", selexpr)

# Define the maximum number of best result (ranks) to return:
query.setMaxNofRanks( 20)
# Define the index of the first rank (for implementing scrolling: 0 for the first, 
# 20 for the 2nd, 40 for the 3rd page, etc.):
query.setMinRank( 0)

# Define the title field:
query.addFeature( "titlefield", {'from':"title_start", 'to':"title_end"} )

# Enable debugging
query.setDebugMode( False )

# Dump query to output
output[ "QueryDump"] = query.tostring()

# Now we evaluate the query and iterate on the result to display them:
results = query.evaluate()
output[ "QueryResult"] = results
output_list = []
for pos,result in enumerate(results['ranks']):
	output_list.append( "rank %u: %u %.5f" % (pos+1, result['docno'], result['weight']))
	for summary in result['summary']:
		output_list.append( "    %s: '%s'" % (summary['name'], summary['value']))
output[ "ResultList"] = output_list

result = "query evaluation:" + dumpTree( output) + "\n"
expected = """query evaluation:
str QueryDump: "query evaluation program:
SELECT select;
EVAL  BM25( b=2.1, k1=0.75, avgdoclen=1000, metadata_doclen=, match= %seek);
SUMMARIZE attribute( metaname='title', resultname='title');
SUMMARIZE attribute( metaname='docid', resultname='docid');
SUMMARIZE matchphrase( type='orig', matchmark=(,), floatingmark=(... , ...), name para=para, name phrase=phrase, name docstart=docstart, paragraphsize=300, sentencesize=40, windowsize=30, cardinality='0', maxdf='0.1', match= %seek, title= %titlefield);
feature 'seek' 1.00000: 
  term word 'citi'

feature 'seek' 1.00000: 
  term word 'visit'

feature 'seek' 1.00000: 
  term word 'tokyo'

feature 'select' 1.00000: 
  contains range=0 cardinality=1:
    term word 'citi'
    term word 'visit'
    term word 'tokyo'

feature 'titlefield' 1.00000: 
  docfield title_start : title_end

maxNofRanks = 20
minRank = 0
"
str QueryExpression:
  number 1:
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
  str nofranked: 3
  str nofvisited: 3
  str pass: 0
  str ranks:
    number 1:
      str docno: 3
      str summary:
        number 1:
          str index: -1
          str name: "title"
          str value: "One day in Tokyo"
          str weight: 1
        number 2:
          str index: -1
          str name: "docid"
          str value: "A.xml"
          str weight: 1
        number 3:
          str index: -1
          str name: "docstart"
          str value: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen. ..."
          str weight: 1
      str weight: 0.64282
    number 2:
      str docno: 2
      str summary:
        number 1:
          str index: -1
          str name: "title"
          str value: "A visit in New York"
          str weight: 1
        number 2:
          str index: -1
          str name: "docid"
          str value: "B.xml"
          str weight: 1
        number 3:
          str index: -1
          str name: "docstart"
          str value: "A visit in New York New York is a city with dimensions you can't imagine. ..."
          str weight: 1
      str weight: 0.00017
    number 3:
      str docno: 1
      str summary:
        number 1:
          str index: -1
          str name: "title"
          str value: "A journey through Germany"
          str weight: 1
        number 2:
          str index: -1
          str name: "docid"
          str value: "C.xml"
          str weight: 1
        number 3:
          str index: -1
          str name: "docstart"
          str value: "A journey through Germany When I first visited germany it was still splitted into two parts. ..."
          str weight: 1
      str weight: 0.00009
str QueryString: "City visit tokyo"
str QueryTerms:
  number 1:
    str type: "word"
    str value: "citi"
  number 2:
    str type: "word"
    str value: "visit"
  number 3:
    str type: "word"
    str value: "tokyo"
str ResultList:
  number 1: "rank 1: 3 0.64282"
  number 2: "    title: 'One day in Tokyo'"
  number 3: "    docid: 'A.xml'"
  number 4: "    docstart: 'One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen. ...'"
  number 5: "rank 2: 2 0.00017"
  number 6: "    title: 'A visit in New York'"
  number 7: "    docid: 'B.xml'"
  number 8: "    docstart: 'A visit in New York New York is a city with dimensions you can't imagine. ...'"
  number 9: "rank 3: 1 0.00009"
  number 10: "    title: 'A journey through Germany'"
  number 11: "    docid: 'C.xml'"
  number 12: "    docstart: 'A journey through Germany When I first visited germany it was still splitted into two parts. ...'"
str postings:
  str expr 0 doc A.xml:
    number 1: 12
  str term 0 doc A.xml:
    number 1: 9
  str term 0 doc B.xml:
    number 1: 11
  str term 1 doc B.xml:
    number 1: 2
  str term 1 doc C.xml:
    number 1: 9
  str term 2 doc A.xml:
    number 1: 4
    number 2: 6
"""
verifyTestOutput( outputdir, result, expected)

