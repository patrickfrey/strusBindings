import strus
import os
from utils import *

def createCollection( strusctx, storagePath, metadata, analyzer, multipart, datadir, fnams, aclmap, withrpc):
	config = None
	if not withrpc:
		config = {
			'path': storagePath,
			'cache': '512M',
			'statsproc': 'std'
		}
		if aclmap:
			config['acl'] = True
		if strusctx.storageExists( config):
			strusctx.destroyStorage( config)
		strusctx.createStorage( config)

	# Get a client for the new created storage:
	storage = strusctx.createStorageClient( config)

	transaction = storage.createTransaction()

	# Create the meta data table structure:
	transaction.defineMetaDataTable( metadata)
	transaction.commit()
	
	# Read input files, analyze and insert them:
	files = {}
	idx = 0
	for fnam in fnams:
		filename = datadir + os.sep + fnam
		idx = idx + 1
		if multipart:
			for doc in analyzer.analyzeMultiPart( readFile( filename)):
				docid = doc.attribute['docid']
				if aclmap:
					doc.access = aclmap[ docid]
				transaction.insertDocument( docid, doc)
		else:
			content = readFile( filename)
			docclass = strusctx.detectDocumentClass( content)
			doc = analyzer.analyzeSingle( content, docclass)
			doc.attribute['docid'] = fnam
			doc.attribute['docclass'] = "mimetype='%s', encoding='%s', schema='%s'" % (docclass.mimetype, docclass.encoding, docclass.schema)
			if aclmap:
				doc.access = aclmap[ fnam]
			transaction.insertDocument( fnam, doc)

	# Without this the documents wont be inserted:
	transaction.commit()
	storage.close()




