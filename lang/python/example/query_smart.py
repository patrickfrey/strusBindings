#!/usr/bin/python

import strus
import os
from testCollection import createCollectionSmart

queryphrase = "brothers"
createCollectionSmart( "./data_smart", "storage")

storagePath = os.environ[ "PYTHONPATH" ] + "/example/storage"
config = "path=%s" % (storagePath)
ctx = strus.Context()

try:
	# Get a client for the storage:
	storage = ctx.createStorageClient( config)
	
	# Define the query analyzer to use:
	analyzer = ctx.createQueryAnalyzer()
	analyzer.addSearchIndexElement( "word", "word", "word", (("stem","en"),"lc",("convdia","en")))
	
	# Define the query evaluation scheme:
	queryEval = ctx.createQueryEval()

	# Here we define what query features decide, what is ranked for the result:
	queryEval.addSelectionFeature( "select")
	
	# Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
	queryEval.addWeightingFunction( "smart", (("function", "df * ff * variable1 / meta1"), ("metadata","meta1"), (".match", "seek")) )
	queryEval.addWeightingFunction( "smart", (("function", "df * ff * variable2 / meta2"), ("metadata","meta2"), (".match", "seek")) )
	queryEval.addWeightingFunction( "smart", (("function", "df * ff * variable3 / meta3"), ("metadata","meta3"), (".match", "seek")) )
	queryEval.addWeightingFormula( "d0 * _0 / frac0 + d1 * _1 / frac1 + d2 * _2 / frac2", (("d0",0.1),("d1",0.2),("d2",0.3)) )
	# Now we define what attributes of the documents are returned and how they are build.
	# The functions that extract stuff from documents for presentation are called summarizers.
	# First we add a summarizer that extracts us the title of the document:
	queryEval.addSummarizer( "attribute", [("name", "title")])
	queryEval.addSummarizer( "smart", (("function", "df * ff"), ("$FFDF","weight"), ("metadata","meta3"), (".match", "seek")) )
	queryEval.addSummarizer( "smart", (("function", "df * ff * variable1 / meta1"), ("$S1","weight"), ("metadata","meta1"), (".match", "seek")) )
	queryEval.addSummarizer( "smart", (("function", "df * ff * variable2 / meta2"), ("$S2","weight"), ("metadata","meta2"), (".match", "seek")) )
	queryEval.addSummarizer( "smart", (("function", "df * ff * variable3 / meta3"), ("$S3","weight"), ("metadata","meta3"), (".match", "seek")) )

	# Then we add a summarizer that collects the sections that enclose the best matches 
	# in a ranked document:
	queryEval.addSummarizer( "matchphrase", (("type","orig"),("sentencesize",40),("windowsize",30),(".match","seek")))

	# Now we build the query to issue:
	query = queryEval.createQuery( storage)

	# First we analyze the query phrase to get the terms to find in the form as they are stored in the storage
	terms = analyzer.analyzeField( "word", queryphrase)
	if len(terms) == 0:
		raise Exception("query is empty")

	# Then we iterate on the terms and create a single term feature for each term and collect
	# all terms to create a selection expression out of them:
	selexpr = strus.QueryExpression()

	for term in terms:
		expr = strus.QueryExpression()
		# We push the query terms on the stack and create a query feature 'seek' 
		# for each of it:
		expr.pushTerm( term.type(), term.value(), term.length())
		print "expr size ", expr.size()
		# We assign the features created to the set named 'seek' because they are 
		# referenced with this name in the query evaluation:
		query.defineFeature( "seek", expr, 1.0)
		# Each query term is also part of the selection expressions
		selexpr.pushTerm( term.type(), term.value(), term.length())
		print "term ", term.type()," ", term.value()," (",selexpr.size(),")"

	# Create a selection feature 'select' that matches documents that contain all query terms.
	selexpr.pushExpression( "contains", len( terms))
	# We assign the feature created to the set named 'select' because this is the
	# name of the set defined as selection feature in the query evaluation configuration
	# (QueryEval.addSelectionFeature):
	query.defineFeature( "select", selexpr)

	# Define the maximum number of best result (ranks) to return:
	query.setMaxNofRanks( 20)
	# Define the index of the first rank (for implementing scrolling: 0 for the first, 
	# 20 for the 2nd, 40 for the 3rd page, etc.):
	query.setMinRank( 0)

	query.setWeightingVariables(
		(
		("variable1",7), ("variable2",13), ("variable3",17),
		("frac0", 3.5), ("frac1", 6.5), ("frac2", 8.5)
		)
	)

	# Now we evaluate the query and iterate on the result to display them:
	results = query.evaluate()
	pos = 0
	for result in results.ranks():
		pos += 1
		print "rank ", pos, ": ", result.docno(), " ", result.weight(), ":"
		summaryElements = result.summaryElements()
		for se in summaryElements:
			print "\t", se.name(), ": ", se.value()
	print "done"
except Exception,err:
	print "Error: ", str(err)
	raise
