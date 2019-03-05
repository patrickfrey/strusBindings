import strus

ctx = strus.Context(None)
config = {
    'path': 'instance/',
    'cache': '512M',
    'statsproc': 'std'
}
if ctx.storageExists(config):
    ctx.destroyStorage(config)

ctx.createStorage(config)
storage = ctx.createStorageClient(config)

transaction = storage.createTransaction()

docclass = {"mimetype":"text/plain"}
content = 'Text of doc1.'

analyzer = ctx.createDocumentAnalyzer( docclass )
analyzer.addSearchIndexFeature( "word", "", "word", "lc")
doc = analyzer.analyzeSingle( content, docclass )

print( "%s" % ctx.debug_serialize( doc))

transaction.insertDocument( 'doc1', doc)

transaction.commit()
storage.close()

