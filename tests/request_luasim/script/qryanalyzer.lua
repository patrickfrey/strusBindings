function GET( self, inputstr, path)
	qryanalyzer = self:get("qryanalyzer")
	if inputstr then
		local query = schema( "query", inputstr, true).query
		for _,f in ipairs( query.feature or {}) do
			f.analyzed = qryanalyzer:analyzeTermExpression( f.content)
		end
		if query.restriction then
			query.restriction.analyzed = qryanalyzer:analyzeMetaDataExpression( query.restriction.content)
		end
		return {query=query}
	else
		return {qryanalyzer=qryanalyzer:introspection( path)}
	end
end

function PUT( self, inputstr, path)
	if path then
		http_status( "404")
	else
		local input = schema( "qryanalyzer", inputstr, true).qryanalyzer
		local context = self:get("context")
		local qryanalyzer = context:createQueryAnalyzer()

		for _,f in ipairs(input.element or {}) do
			qryanalyzer:addElement( f.type, f.field, f.tokenizer, f.normalizer, f.priority)
		end
		for _,f in ipairs(input.group or {}) do
			qryanalyzer:defineImplicitGroupBy( f.field, f.by, f.op, f.range, f.cardinality)
		end
		self:set( "qryanalyzer", qryanalyzer)
	end
end

