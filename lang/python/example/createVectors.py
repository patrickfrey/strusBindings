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
ctx.loadModule( "storage_vector_std");

try:
	ctx.destroyVectorStorage( config)
	# ... delete the storage files if they already exists
except:
	pass

# Create a new storage:
ctx.createVectorStorage( config)
# Get a client for the new created storage:
storage = ctx.createVectorStorageClient( config)

# Create the vectors:
config="path=vsm;logfile=-;dim=3;bit=4;var=8;maxdist=5;probsim=yes;simdist=5;probdist=8;raddist=5;eqdist=1;singletons=yes"
builder = ctx.createVectorStorageBuilder( config);
builder.addFeature( "A", [0.1, 0.2, 0.3] );
builder.addFeature( "B", [0.15, 0.25, 0.35] );
builder.addFeature( "C", [0.3, 0.2, 0.1] );
builder.addFeature( "D", [0.35, 0.25, 0.15] );
builder.addFeature( "E", [0.25, 0.2, 0.2] );
builder.addFeature( "F", [0.2, 0.25, 0.2] );
builder.addFeature( "G", [0.2, 0.2, 0.25] );
builder.run( "");
print "done"


