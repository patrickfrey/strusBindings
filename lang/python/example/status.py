#!/usr/bin/python

import strus

config = "path=storage"
ctx = strus.Context()
storage = ctx.createStorageClient( config)
# Query the number of documents inserted:
nofDocuments = storage.nofDocumentsInserted();
# Output:
print "Number of documents inserted: ", nofDocuments

