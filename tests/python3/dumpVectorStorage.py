$import strus
from utils import *

def dumpVectorStorage( strusctx, config, vectors, examplevec):
	output = {}
	# Get a client for the vector storage:
	storage = strusctx.createVectorStorageClient( config)

	output[ 'types'] = storage.types()
	output[ 'nof vec word"] = storage.nofVectors( "word")
	output[ 'nof vec nonvec"] = storage.nofVectors( "nonvec")
	for iv,vv in enumerate( vectors):
		output[ 'types F%u' % (iv+1)] = ' '.join( storage.featureTypes( "F%u" % (iv+1)))
		output[ 'vec F%u' % (iv+1)] = storage.featureVector( "word", "F%u" % (iv+1))
		output[ 'vec F%u example sim' % (iv+1)] = storage.vectorSimilarity( vv, examplevec)

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


