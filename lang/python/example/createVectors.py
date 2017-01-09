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
ctx.loadModule( "storage_vector");

try:
	ctx.destroyVectorStorage( config)
	# ... delete the storage files if they already exists
except:
	pass

# Create a new storage:
ctx.createVectorStorage( config)
# Get a client for the new created storage:
storage = ctx.createVectorStorageClient( config)


# Read input files, analyze and insert them:
transaction = storage.createTransaction()
datadir = "./data/"
for (dirpath, dirnames, filenames) in walk( datadir):
	for idx, filename in enumerate( filenames):
		if filename.endswith('.xml'):
			print "%u process document %s" % (idx,filename)
			docid = filename[0:-4]
			doc = analyzer.analyze( readFile( datadir + filename))
			transaction.insertDocument( docid, doc)

# Without this the documents wont be inserted:
transaction.commit()
print "done"


