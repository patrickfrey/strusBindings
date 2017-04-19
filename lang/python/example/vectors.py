#!/usr/bin/python

import strus
import os

storagePath = os.environ[ "PYTHONPATH" ] + "/example/storage"
config="path=%s;dim=7;bit=3;var=32;maxdist=5;probsim=yes;simdist=5;probdist=8;raddist=5;eqdist=1;singletons=yes" % (storagePath)

ctx = strus.Context()
ctx.loadModule( "storage_vector_std")

try:
	ctx.destroyStorage( config)
	# ... delete the storage files if they already exists
except:
	pass

# Create a new storage:
ctx.createVectorStorage( config)

# Get a client for the new created storage:
storage = ctx.createVectorStorageClient( config)
print "storage config: %s" % storage.config()

# Add the vectors:
transaction = storage.createTransaction()
transaction.addFeature( "A", [0.1, 0.2, 0.3, 0.1, 0.1, 0.1, 0.1] )
transaction.addFeature( "B", [0.15, 0.25, 0.35, 0.09, 0.11, 0.12, 0.1] )
transaction.addFeature( "C", [0.3, 0.2, 0.1, 0.12, 0.07, 0.09, 0.11] )
transaction.addFeature( "D", [0.35, 0.25, 0.15, 0.13, 0.13, 0.12, 0.13] )
transaction.addFeature( "E", [0.25, 0.2, 0.2, 0.12, 0.13, 0.13, 0.13] )
transaction.addFeature( "F", [0.2, 0.25, 0.2, 0.11, 0.09, 0.08, 0.12] )
transaction.addFeature( "G", [0.2, 0.2, 0.25, 0.12, 0.1, 0.08, 0.12] )
transaction.commit()

searcher = storage.createSearcher( 0, storage.nofFeatures())

similarVectors = searcher.findSimilar( [0.1, 0.2, 0.3, 0.1, 0.1, 0.1, 0.1], 4)
for sv in similarVectors:
    print "[1] index: %u weight %f" % (sv.index(),sv.weight())

similarVectors = searcher.findSimilar( [0.35, 0.25, 0.15, 0.13, 0.13, 0.12, 0.13], 4)
for sv in similarVectors:
    print "[2] index: %u weight %f" % (sv.index(),sv.weight())

for name in storage.conceptClassNames():
    print "concept class: '%s'" % name

nofConcepts = storage.nofConcepts( "")
for ci in range(1,nofConcepts+1):
    for fi in storage.conceptFeatures( "", ci):
        print "concept %u, member %u" % (ci,fi)

nofFeatures = storage.nofFeatures()
for fi in range(0,nofFeatures):
    name = storage.featureName( fi)
    print "feature %u name: '%s'" % (storage.featureIndex(name),name)
    for ci in storage.featureConcepts( "", fi):
        print "feature %u assigned to %u" % (fi,ci)
    for vi in storage.featureVector( fi):
        print "feature vector %u element %f" % (fi,vi)

searcher.close()
storage.close()

print "done"

