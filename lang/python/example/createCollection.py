#!/usr/bin/python

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
datadir = "./data/";
for (dirpath, dirnames, filenames) in walk( datadir):
	for idx, filename in enumerate( filenames):
		if filename.endswith('.xml'):
			print "%u process document %s" % (idx,filename)
			docid = filename[0:-4]
			doc = analyzer.analyze( readFile( datadir + filename))
			storage.insertDocument( docid, doc)

# Without this the documents wont be inserted:
storage.flush()
print "done"


