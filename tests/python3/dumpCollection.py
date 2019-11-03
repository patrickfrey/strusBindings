import strus
from utils import *

def dumpCollection( strusctx, storagePath):
	config = "path=%s; cache=512M; statsproc=std" % storagePath
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
	output[ "metadata"] = storage.introspection("metadata")
	output[ "attribute"] = storage.introspection("attribute")

	# Document identifiers:
	output_docids = []
	dociditr = storage.docids()
	for tp in dociditr:
		output_docids.append( tp )

	output[ "docids"] = sorted( output_docids)

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
			if colkey != 'docno':
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
	dfchangelist = []
	bloblist = []
	nofdocs = 0
	for blob in storage.getAllStatistics():
		bloblist.append( blob)
		statview = strusctx.unpackStatisticBlob( blob, "std")
		nofdocs += statview.nofdocs
		for dfchange in statview.dfchange:
			dfchangelist.append( {'type':dfchange.type, 'value':dfchange.value, 'increment':dfchange.increment})

	output[ "statblobs"] = bloblist
	output[ "stat"] = output_stat
	output_stat[ "dfchange"] = sorted( dfchangelist, key=lambda dfchange: "%s %s %d" % (dfchange['type'], dfchange['value'], dfchange['increment']) )
	output_stat[ "nofdocs"] = nofdocs

	storage.close()
	return output


