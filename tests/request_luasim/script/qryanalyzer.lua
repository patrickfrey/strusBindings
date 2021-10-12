function GET( self, inputstr, path)
	qryanalyzer = self:get("qryanalyzer")
	if inputstr then
		local query = schema( "query", inputstr, true).query
		for _,f in ipairs( query.feature or {}) do
			f.analyzed = qryanalyzer:analyzeSchemaTermExpression( f.content)
		end
		for _,r in ipairs( query.restriction or {}) do
			r.analyzed = qryanalyzer:analyzeMetaDataExpression( r.content)
		end
		return {query=query}
	else
		return {qryanalyzer=qryanalyzer:introspection( path)}
	end
end

function PUT( self, inputstr, path)
	if path then
		http_error( "404")
	else
		local input = schema( "qryanalyzer", inputstr, true).qryanalyzer
		local context = self:get("context")
		local qryanalyzer = context:createQueryAnalyzer()

		if input.element then
			for _,f in ipairs(input.element) do
				qryanalyzer:addElement( f.type, f.field, f.tokenizer, f.normalizer, f.priority)
			end
		end
		if input.group then
			for _,f in ipairs(input.group) do
				qryanalyzer:defineImplicitGroupBy( f.field, f.by, f.op, f.range, f.cardinality)
			end
		end
		self:set( "qryanalyzer", qryanalyzer)
	end
end

