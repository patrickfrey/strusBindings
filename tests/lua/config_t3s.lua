require "string"

-- Create the document analyzer for the documents in data/t3s:
function createDocumentAnalyzer_t3s( strusctx)
	local analyzer = strusctx:createDocumentAnalyzer( {"xml"})

	-- Define the features and attributes to store:
	analyzer:addSearchIndexFeature( "word", "/doc/text()", "word", {{"stem","en"},"lc",{"convdia","en"}})
	analyzer:addSearchIndexFeature( "word", "/doc/title()", "word", {{"stem","en"},"lc",{"convdia","en"}})
	analyzer:addSearchIndexFeature( "endtitle", "/doc/title~", "content", "empty")
	analyzer:addForwardIndexFeature( "orig", "/doc/text()", "split", "orig")
	analyzer:addForwardIndexFeature( "orig", "/doc/title()", "split", "orig")
	analyzer:defineAttribute( "title", "/doc/title()", "content", "orig")
	analyzer:defineAggregatedMetaData( "title_end", {"nextpos", "endtitle"})
	analyzer:defineAggregatedMetaData( "doclen", {"count", "word"})
	return analyzer
end

function createQueryAnalyzer_t3s( strusctx)
	local analyzer = strusctx:createQueryAnalyzer()
	analyzer:addElement( "word", "word", "word", {{"stem","en"},"lc",{"convdia","en"}})
	return analyzer
end

function metadata_t3s()
	return {{'doclen','UINT16'},
		{'title_start','UINT8'},
		{'title_end','UINT8'}}
end

function createQueryEval_t3s( strusctx)
	-- Define the query evaluation scheme:
	local queryEval = strusctx:createQueryEval()
	
	-- Here we define what query features decide, what is ranked for the result:
	queryEval:addSelectionFeature( "select")
		
	-- Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
	queryEval:addWeightingFunction( "BM25", {k1=0.75, b=2.1, avgdoclen=1000}, {match="seek"})

	-- Now we define what attributes of the documents are returned and how they are build.
	-- The functions that extract stuff from documents for presentation are called summarizers.
	-- First we add a summarizer that extracts us the title of the document:
	queryEval:addSummarizer( "title", "attribute", {{"name", "title"}})
	queryEval:addSummarizer( "docid", "attribute", {{"name", "docid"}})
	
	-- Then we add a summarizer that collects the sections that enclose the best matches 
	-- in a ranked document:
	queryEval:addSummarizer( "summary", "matchphrase", {{"text","orig"}, {"cluster",0.1}, {"maxdf", 1.0}}, {match="seek"})
	return queryEval
end


