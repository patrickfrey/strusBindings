#!/usr/bin/python

import strus
import os
from testCollection import createCollection

createCollection( "./data", "storage")

storagePath = os.environ[ "PYTHONPATH" ] + "/example/storage"
config = "path=%s" % (storagePath)

ctx = strus.Context()
storage = ctx.createStorageClient( config)
# Query the number of documents inserted:
nofDocuments = storage.nofDocumentsInserted();
# Output:
print "Number of documents inserted: ", nofDocuments

