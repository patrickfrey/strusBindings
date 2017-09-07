import strus
from utils import concatValues,dumpValue,dumpTree,readFile,writeFile,verifyTestOutput,getPathArray,getFileParentDirectory,getFileName,joinLists,getContextConfig

def dumpCollection( strusctx, storagePath):
	config = "path=%s; cache=512M; statsproc=default" % storagePath
	output = {}

	# Get a client for the storage:
	storage = strusctx.createStorageClient( config)

	# Configuration of the storage:
	output_config = {}
	for key,value in storage.config().items():
		if key == 'path':
			output_config[ key] = getFileName( value)
		else:
			output_config[ key] = value

	output[ "config"] = output_config
	output[ "nofdocs"] = storage.nofDocumentsInserted()

	# Document identifiers:
	output_docids = []
	dociditr = storage.docids()
	for tp in dociditr:
		output_docids.append( {'id':tp, 'docno':storage.documentNumber(tp)} )

	output[ "docids"] = output_docids

	# Term types:
	termtypes = joinLists( storage.termTypes())
	output[ "types"] = termtypes

	# Document data (metadata,attributes,content):
	selectids = joinLists( "docno", "ACL", storage.attributeNames(), storage.metadataNames(), termtypes)
	output_docs = {}
	output_terms = {}
	for docrow in storage.select( selectids, None, None, 0):
		flatdocrow = {}
		for colkey,colval in docrow.items():
			flatdocrow[ colkey] = concatValues( colval)
		output_docs[ docrow['docid']] = flatdocrow

		for termtype in termtypes:
			fterms = storage.documentForwardIndexTerms( docrow['docno'], termtype)
			ftermlist = []
			for fterm,pos in fterms:
				ftermlist.append( {'value':fterm, 'pos':pos})
			output_terms[ "%s:%s (f)" % (docrow['docid'], termtype)] = ftermlist

			sterms = storage.documentSearchIndexTerms( docrow['docno'], termtype)
			stermlist = []
			for sterm in sterms:
				(stermval,tf,firstpos) = sterm
				stermlist.append( {'value':stermval, 'tf':tf, 'firstpos':firstpos})
			output_terms[ "%s:%s (s)" % (docrow['docid'], termtype)] = stermlist

	output[ "docs"] = output_docs
	output[ "terms"] = output_terms

	# Term statistics:
	output_stat = {}
	for blob in storage.getAllStatistics( True):
		statview = strusctx.unpackStatisticBlob( blob, "default")
		dfchangelist = []
		for dfchange in statview[ "dfchange"]:
			dfchangelist.append( dfchange)

		output_stat[ "dfchange"] = dfchangelist
		if "nofdocs" in output_stat:
			output_stat[ "nofdocs"] += statview[ "nofdocs"]
		else:
			output_stat[ "nofdocs"] = statview[ "nofdocs"]

	output[ "stat"] = output_stat

	storage.close()
	return output


