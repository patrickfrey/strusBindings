# --------------------------
# Helper module for tests
# --------------------------
import strus
import os

def readFile( path ):
	"This function reads a file"
	with open( path, mode='rb') as file:
		fileContent = file.read()
	return fileContent

def createCollection( datadir, storagename):
	storagePath = os.environ[ "PYTHONPATH" ] + "/example/" + storagename
	config = "path=%s; metadata=doclen UINT16, title_start UINT8, title_end UINT8" % (storagePath)
	ctx = strus.Context()
	ctx.loadModule( "analyzer_pattern");

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
	analyzer.addSearchIndexFeature( "word", "/doc/title()", "word", (("stem","en"),"lc",("convdia","en")))
	analyzer.addSearchIndexFeature( "endtitle", "/doc/title~", "content", "empty")
	analyzer.addForwardIndexFeature( "orig", "/doc/text()", "split", "orig")
	analyzer.addForwardIndexFeature( "orig", "/doc/title()", "split", "orig")
	analyzer.defineAttribute( "title", "/doc/title()", "content", "orig")
	analyzer.defineAggregatedMetaData( "title_end", ("nextpos", "endtitle"));
	
	analyzer.definePatternMatcherPostProc( "coresult", "std", (
		("city_that_is", ("sequence", 3, ["word","citi"],["word","that"],["word","is"])),
		("city_that", ("sequence", 2, ["word","citi"],["word","that"])),
		("city_with", ("sequence", 2, ["word","citi"],["word","with"]))
	));
	analyzer.addSearchIndexFeatureFromPatternMatch( "word", "coresult", ("lc"))
	
	# Read input files, analyze and insert them:
	transaction = storage.createTransaction()
	for (dirpath, dirnames, filenames) in os.walk( datadir):
		for idx, filename in enumerate( filenames):
			if filename.endswith('.xml'):
				print "%u process document %s" % (idx,filename)
				docid = filename[0:-4]
				doc = analyzer.analyze( readFile( os.path.join( datadir, filename)))
				transaction.insertDocument( docid, doc)
	
	# Without this the documents wont be inserted:
	transaction.commit()

