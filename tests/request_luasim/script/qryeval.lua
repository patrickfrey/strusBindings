function GET( self, inputstr, path)
	qryeval = self:get("qryeval")
	if inputstr then
		local input = schema( "query", inputstr, true).query
		local storage = self:get("storage")
		local query = qryeval:createQuery( storage)
		for _,f in ipairs( input.feature or {}) do
			query:addFeature( f.set, f.analyzed, f.weight)
		end
		if input.restriction then
			query:addMetaDataRestriction( input.restriction.analyzed)
		end
		if input.globalstats then query:defineGlobalStatistics( globalstats.nofdocs) end
		for _,s in ipairs( input.termstats or {}) do
			query:defineTermStatistics( t.type, t.value, t.df)
		end
		if input.access then query:addAccess( input.access) end
		if input.evalset then query:addDocumentEvaluationSet( input.evalset) end
		if input.nofranks then query:setMaxNofRanks( input.nofranks) end
		if input.minrank then query:setMinRank( input.minrank) end
		if input.mergeres then query:useMergeResult() end

		return {queryresult=query:evaluate()}
	else
		return {qryeval=qryeval:introspection( path)}
	end
end

function PUT( self, inputstr, path)
	if path then
		http_status( "404")
	else
		local input = schema( "qryeval", inputstr, true).qryeval
		self:inherit( "storage", input.storage)
		local context = self:get("context")
		local qryeval = context:createQueryEval()

		for _,t in ipairs(input.cterm or {}) do
			qryeval:addTerm( t.set, t.type, t.value)
		end
		for _,s in ipairs(input.restriction or {}) do
			qryeval:addRestrictionFeature( s)
		end
		for _,s in ipairs(input.selection or {}) do
			qryeval:addSelectionFeature( s)
		end
		for _,s in ipairs(input.exclusion or {}) do
			qryeval:addExclusionFeature( s)
		end
		for _,w in ipairs(input.weighting or {}) do
			qryeval:addWeightingFunction( w.name, w.param, w.feature)
		end
		for _,s in ipairs(input.summarizer or {}) do
			qryeval:addSummarizer( s.id, s.name, s.param, s.feature)
		end
		if input.formula then
			qryeval:defineWeightingFormula( input.formula.expression, input.formula.param)
		end
		for _,s in ipairs(input.noposinfo or {}) do
			qryeval:usePositionInformation( s, false)
		end
		self:set( "qryeval", qryeval)
	end
end

