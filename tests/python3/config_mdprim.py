import strus

# Create the document analyzer for the documents in data/mdprim:
# SCHEMA:
# <list>
#   <doc id=$docid : (integer number >= 1)>
#     <cross> $crossSum : (digits of the docid added together) </cross>
#     <factors> $factors : (number of prime factors  of docid without the docid itself, 0 => prime number) </factors>
#     <lo> $lowest : (lowest prime factor of docid) </lo>
#     <hi> $highest : (largest prime factor of docid)</hi>
#     <content> $primefactors : (list of primefactors separated by spaces)</content>
#   </doc>
# END SCHEMA

def createDocumentAnalyzer_mdprim( strusctx):
	analyzer = strusctx.createDocumentAnalyzer( ["xml"])

	analyzer.defineDocument( "doc", "/list/doc")

	# Define the features and attributes to store:
	mdelems = [ 'cross', 'factors', 'lo', 'hi' ]
	for name in mdelems:
		analyzer.defineMetaData( name, "/list/doc/" + name + "()", "word", "orig")
	analyzer.addSearchIndexFeature( "word", "/list/doc/content()", "word", "orig")
	analyzer.addForwardIndexFeature( "word", "/list/doc/content()", "word", "orig")
	analyzer.defineAttribute( "docid", "/list/doc@id", "content", "orig")
	analyzer.defineMetaData( "docidx", "/list/doc@id", "content", "orig")
	analyzer.defineAggregatedMetaData( "doclen", ["count", "word"])

	return analyzer

def createQueryAnalyzer_mdprim( strusctx):
	analyzer = strusctx.createQueryAnalyzer()
	mdelems = [ 'cross', 'factors', 'lo', 'hi' ]
	for name in mdelems:
		analyzer.addElement( name, name, "word", "orig")
	analyzer.addElement( "word", "word", "word", "orig")
	return analyzer

def metadata_mdprim():
	return 'cross UINT8, factors UINT8, lo UINT16, hi UINT16, doclen UINT16, docidx UINT32'

def createQueryEval_mdprim( strusctx):
	# Define the query evaluation scheme:
	queryEval = strusctx.createQueryEval()

	# Here we define what query features decide, what is ranked for the result:
	queryEval.addSelectionFeature( "select")
	
	# Here we define how we rank a document selected:
	queryEval.addWeightingFunction(
		"tf", {
				'match':{'feature':"seek"}, 'debug':"debug_weighting"
			})
	queryEval.addWeightingFunction(
		"metadata", {
				'name':"doclen"
			})
	queryEval.addWeightingFunction(
		"metadata", {
				'name':"docidx"
			})
	queryEval.defineWeightingFormula( "(_0 / _1) + ((1000 - _2) / 1000000)" )

	# Now we define what attributes of the documents are returned and how they are build:
	queryEval.addSummarizer( "attribute", [["name", "docid"],["debug","debug_attribute"]])
	queryEval.addSummarizer( "metadata", [["name", "cross"],["debug","debug_metadata"]])
	queryEval.addSummarizer( "metadata", [["name", "factors"],["debug","debug_metadata"]])
	queryEval.addSummarizer( "metadata", [["name", "lo"],["debug","debug_metadata"]])
	queryEval.addSummarizer( "metadata", [["name", "hi"],["debug","debug_metadata"]])

	# Then we add a summarizer that collects the sections that enclose the best matches 
	# in a ranked document:
	queryEval.addSummarizer( "forwardindex", [ ["type","word"], ["debug","debug_forwardindex"] ])

	return queryEval


