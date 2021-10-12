function GET( self, inputstr, path)
	local docanalyzer = self:get("docanalyzer")
	if inputstr then
		local docar = {}
		for doc in docanalyzer:analyzeMultiPart( inputstr) do
			table.insert( docar, doc)
		end
		if #docar > 1 then
			return {doclist={doc=docar}}
		elseif #docar == 1 then
			return {doc=docar[1]}
		end
	else
		return {docanalyzer=docanalyzer:introspection( path)}
	end
end

function PUT( self, inputstr, path)
	if path then
		http_error( "404")
	else
		local input = schema( "docanalyzer", inputstr, true).docanalyzer
		local context = self:get("context")
		local docanalyzer = context:createDocumentAnalyzer( input.class )

		if input.feature then
			for _,f in ipairs(input.feature.search or {}) do
				docanalyzer:addSearchIndexFeature( f.type, f.select, f.tokenizer, f.normalizer, f.priority, f.option)
			end
			for _,f in ipairs(input.feature.forward or {}) do
				docanalyzer:addForwardIndexFeature( f.type, f.select, f.tokenizer, f.normalizer, f.priority, f.option)
			end
			for _,m in ipairs(input.feature.metadata or {}) do
				if m.aggregator then
					docanalyzer:defineAggregatedMetaData( m.type, m.aggregator)
				else
					docanalyzer:defineMetaData( m.type, m.select, m.tokenizer, m.normalizer)
				end
			end
			for _,a in ipairs(input.feature.attribute or {}) do
				docanalyzer:defineAttribute( a.type, a.select, a.tokenizer, a.normalizer)
			end
		end
		for _,d in ipairs(input.document or {}) do
			docanalyzer:defineSubDocument( d.name, d.select)
		end
		for _,c in ipairs(input.content or {}) do
			docanalyzer:defineSubContent( c.select, c.class)
		end
		for _,f in ipairs(input.field or {}) do
			docanalyzer:addSearchIndexField( f.name, f.scope, f.select, f.key)
		end
		for _,s in ipairs(input.structure or {}) do
			docanalyzer:addSearchIndexStructure( s.name, s.header, s.content, s.class)
		end
		self:set( "docanalyzer", docanalyzer)
	end
end


