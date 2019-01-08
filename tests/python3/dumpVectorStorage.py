import strus
from utils import *

def vecToString( vec, delim):
	rt = ""
	for iv,vv in enumerate( vec):
		if rt:
			rt += delim
		rt += "%.5f" % vv
	return rt

def dumpVectorStorage( strusctx, config, vectors, examplevec):
	output = {}
	# Get a client for the vector storage:
	storage = strusctx.createVectorStorageClient( config)

	ranklist = []

	output[ "types"] = storage.types()
	output[ "nof vec word"] = storage.nofVectors( "word")
	output[ "nof vec nonvec"] = storage.nofVectors( "nonvec")
	for iv,vv in enumerate( vectors):
		featstr = "F%u" % (iv+1)
		featsim = storage.vectorSimilarity( vv, examplevec)
		featvec = storage.featureVector( "word", featstr)
		output[ "types %s" % featstr] = storage.featureTypes( featstr)
		output[ "vec %s" % featstr] = "{" + vecToString( featvec, ", ") + "}"
		output[ "vec %s example sim" % featstr] = storage.vectorSimilarity( vv, examplevec)
		ranklist.append( [featstr,featsim,featvec] )

	ranklist.sort( key = lambda x: (x[1], x[0]), reverse=True)
	for iv,vv in enumerate( ranklist):
		if iv >= 5:
			break
		output[ "rank %3d" % iv] = "%.5f %s {%s}" % (vv[1], vv[0], vecToString( vv[2],", "))

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


