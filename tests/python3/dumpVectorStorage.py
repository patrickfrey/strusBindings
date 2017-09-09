import strus
from utils import *

def dumpVectorStorage( strusctx, config):
	output = {}
	# Get a client for the vector storage:
	storage = strusctx.createVectorStorageClient( config)

	# Dump concept classes of the storage:
	classes = storage.conceptClassNames()
	output[ 'classes'] = classes
	for cl in classes:
		csize = storage.nofConcepts( cl)
		for ci in range(1,csize+1):
			cfeats = storage.conceptFeatures( cl, ci)
			if next(cfeats) is not None:
				output[ "class '%s' concept %u" % (cl, ci)] = cfeats
	# Dump features of the storage:
	nofvecs = storage.nofFeatures()
	for fi in range(nofvecs):
		fname = storage.featureName( fi)
		if fi != storage.featureIndex( fname):
			raise Exception( "feature index does not match %u ~= %u (%s)" % (fi, storage.featureIndex( fname), fname))
		for cl in classes:
			concepts = storage.featureConcepts( cl, fi)
			output[ "class '%s' feature %u" % (cl, fi)] = concepts
		fvector = storage.featureVector( fi)
		output[ "vector '%s'" % (fname)] = fvector
	# Configuration of the storage:
	output_config = {}
	for key,value in storage.config().items():
		if key == 'path':
			output_config[ key] = getFileName( value)
		else:
			output_config[ key] = value

	output[ "config"] = output_config
	storage.close()
	return output


