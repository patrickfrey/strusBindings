strus Python bindings	 {#mainpage}
=====================

The strus Python interface provides objects for accessing the 
retrieval storage, indexing documents and queries and evaluating queries.
It has been built using SWIG based on a wrapper C++ interface (@ref strus/bindingObjects.hpp).

To use the strus Python interface you have to load the module 'strus'.

The entry point of a strus application with Python is the context object (strus.Context).
It is the root object from which all other objects are created.
It can be constructed either as proxy, that redirects all method calls to an RpcServer
or it can be constructed as instance running in the Python environment.

The following examples illustrate the usage of the Python bindings.
They are taken from tests built and executed in the strusBindings project.

\section createCollection Create a collection, analyze and insert 3 documents 
\code
import strus
from os import walk

def readFile( path ):
	"This function reads a file"
	with open( path, mode='rb') as file:
		fileContent = file.read()
	return fileContent

config = "path=storage; metadata=doclen UINT16"
ctx = strus.Context()
try:
	ctx.destroyStorage( config)
	# ... delete the storage files if they already exists
except:
	pass

# Create a new storage:
ctx.createStorage( config)
# Get a client for the new created storage:
storage = ctx.createStorageClient( config)

# Define the document analyzer to use:
analyzer = ctx.createDocumentAnalyzer()

# Define the features and attributes to store:
analyzer.addSearchIndexFeature( "word", "/doc/text()", "word", (("stem","en"),"lc",("convdia","en")))
analyzer.addForwardIndexFeature( "orig", "/doc/text()", "split", "orig");
analyzer.defineAttribute( "title", "/doc/title()", "content", "orig");

# Read input files, analyze and insert them:
transaction = storage.createTransaction()
datadir = "./data/"
for (dirpath, dirnames, filenames) in walk( datadir):
	for idx, filename in enumerate( filenames):
		if filename.endswith('.xml'):
			print "%u process document %s" % (idx,filename)
			docid = filename[0:-4]
			doc = analyzer.analyze( readFile( datadir + filename))
			transaction.insertDocument( docid, doc, True)

# Without this the documents wont be inserted:
transaction.commit()
print "done"
\endcode

\section status Query some status info, the number of documents inserted
\code
import strus

config = "path=storage"
ctx = strus.Context()
storage = ctx.createStorageClient( config)
# Query the number of documents inserted:
nofDocuments = storage.nofDocumentsInserted();
# Output:
print "Number of documents inserted: ", nofDocuments
\endcode

\section query Issue a BM25 query on the created collection (first example) and show the best ranked documents
\code
import strus
from os import walk

queryphrase = "city"

config = "path=storage"
ctx = strus.Context()

try:
	# Get a client for the storage:
	storage = ctx.createStorageClient( config)
	
	# Define the query analyzer to use:
	analyzer = ctx.createQueryAnalyzer()
	analyzer.definePhraseType( "word", "word", "word", (("stem","en"),"lc",("convdia","en")))
	
	# Define the query evaluation scheme:
	queryEval = ctx.createQueryEval()
	
	# Here we define what query features decide, what is ranked for the result:
	queryEval.addSelectionFeature( "select")
	
	# Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
	queryEval.addWeightingFunction( 1.0, "BM25", (("k1",0.75), ("b",2.1), ("avgdoclen", 1000), (".match", "seek")))
	
	# Now we define what attributes of the documents are returned and how they are build.
	# The functions that extract stuff from documents for presentation are called summarizers.
	# First we add a summarizer that extracts us the title of the document:
	queryEval.addSummarizer( "title", "attribute", [("name", "title")])
	
	# Then we add a summarizer that collects the sections that enclose the best matches 
	# in a ranked document:
	queryEval.addSummarizer( "summary", "matchphrase", (("type","orig"),("nof",4),("len",60),(".match","seek")))
	
	# Now we build the query to issue:
	query = queryEval.createQuery( storage)

	# First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
	terms = analyzer.analyzePhrase( "word", queryphrase)

	# Then we iterate on the terms and create a single term feature for each term and collect
	# all terms to create a selection expression out of them:
	selexpr = [ "contains" ]
	for term in terms:
		# We assign the features created to the set named 'seek' because they are 
		# referenced with this name in the query evaluation:
		query.defineFeature( "seek", [term.type(), term.value()], 1.0)
		# Each query term is also part of the selection expressions
		selexpr.append( [term.type(), term.value()] )

	# We assign the feature created to the set named 'select' because this is the
	# name of the set defined as selection feature in the query evaluation configuration
	# (QueryEval.addSelectionFeature):
	query.defineFeature( "select", selexpr)

	# Define the maximum number of best result (ranks) to return:
	query.setMaxNofRanks( 20)
	# Define the index of the first rank (for implementing scrolling: 0 for the first, 
	# 20 for the 2nd, 40 for the 3rd page, etc.):
	query.setMinRank( 0)
	
	# Now we evaluate the query and iterate on the result to display them:
	results = query.evaluate()
	pos = 0
	for result in results:
		pos += 1
		print "rank ", pos, ": ", result.docno(), " ", result.weight(), ":"
		attributes = result.attributes()
		for attribute in attributes:
			print "\t", attribute.name(), ": ", attribute.value()
	print "done"
except Exception,err:
	print "Error: ", str(err)
	raise

\endcode



