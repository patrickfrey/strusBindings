#!/usr/bin/python

import strus
from os import walk

queryphrase = "city"

config = "path=storage"
ctx = strus.Context()

try:
	# Get a client for the storage:
	storage = ctx.createStorageClient( config)
	
	# Define the query analyzer to use:
	analyzer = ctx.createQueryAnalyzer()
	analyzer.definePhraseType( "word", "word", "word", (("stem","en"),"lc",("convdia","en")))
	
	# Define the query evaluation scheme:
	queryEval = ctx.createQueryEval()
	
	# Here we define what query features decide, what is ranked for the result:
	queryEval.addSelectionFeature( "select")
	
	# Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
	queryEval.addWeightingFunction( 1.0, "BM25", (("k1",0.75), ("b",2.1), ("avgdoclen", 1000), (".match", "seek")))
	
	# Now we define what attributes of the documents are returned and how they are build.
	# The functions that extract stuff from documents for presentation are called summarizers.
	# First we add a summarizer that extracts us the title of the document:
	queryEval.addSummarizer( "title", "attribute", [("name", "title")])
	
	# Then we add a summarizer that collects the sections that enclose the best matches 
	# in a ranked document:
	queryEval.addSummarizer( "summary", "matchphrase", (("type","orig"),("nof",4),("len",60),(".match","seek")))
	
	# Now we build the query to issue:
	query = queryEval.createQuery( storage)
	
	# First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
	terms = analyzer.analyzePhrase( "word", queryphrase)
	
	# Then we iterate on the terms and create a single term feature for each term and collect
	# all terms to create a selection expression out of them:
	for term in terms:
		# We push the query terms on the stack and create a query feature 'seek' 
		# for each of it:
		query.pushTerm( term.type(), term.value())
		# Ever feature is duplicated on the stack, because we use them to 
		# build the selection expression that selects all documents for ranking
		# that contain all terms
		query.pushDuplicate()
		# We assign the features created to the set named 'seek' because they are 
		# referenced with this name in the query evaluation:
		query.defineFeature( "seek", 1.0)
	
	# Create a selection feature 'select' that matches documents that contain all query terms:
	if len(terms) > 0:
		# Now we build the selection expression with the terms pushed as duplicates on
		# the stack in the loop before:
		query.pushExpression( "contains", len( terms))
		# We assign the feature created to the set named 'select' because this is the
		# name of the set defined as selection feature in the query evaluation configuration
		# (QueryEval.addSelectionFeature):
		query.defineFeature( "select")
	
	# Define the maximum number of best result (ranks) to return:
	query.setMaxNofRanks( 20)
	# Define the index of the first rank (for implementing scrolling: 0 for the first, 
	# 20 for the 2nd, 40 for the 3rd page, etc.):
	query.setMinRank( 0)
	
	# Now we evaluate the query and iterate on the result to display them:
	results = query.evaluate()
	pos = 0
	for result in results:
		pos += 1
		print "rank ", pos, ": ", result.docno(), " ", result.weight(), ":"
		attributes = result.attributes()
		for attribute in attributes:
			print "\t", attribute.name(), ": ", attribute.value()
	print "done"
except Exception,err:
	print "Error: ", str(err)
	raise

